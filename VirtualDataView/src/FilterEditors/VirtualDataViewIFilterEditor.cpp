/**********************************************************************/
/** FILE    : VirtualDataViewIFilterEditor.cpp                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : combines a filter and the UI for modifying it          **/
/**********************************************************************/

#include <wx/VirtualDataView/FilterEditors/VirtualDataViewIFilterEditor.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/window.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>

wxDEFINE_EVENT(wxEVT_VDV_FILTER_EDITION_BEGIN  , wxCommandEvent);
wxDEFINE_EVENT(wxEVT_VDV_FILTER_EDITION_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_VDV_FILTER_EDITION_END    , wxCommandEvent);

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewIFilterEditor::wxVirtualDataViewIFilterEditor(void)
    : wxEvtHandler(),
      m_DefaultSize(-1, -1),
      m_pFilter(WX_VDV_NULL_PTR)
{

}

/** Destructor
  */
wxVirtualDataViewIFilterEditor::~wxVirtualDataViewIFilterEditor(void)
{
    ReleaseFilter();
}

//---------------------- INTERFACE FOR UI ---------------------------//
/** Draw the filter
  * \param pWin  [input]: the window receiving the drawing
  * \param rDC   [input]: the DC to use for drawing the filter
  * \param rRect [input]: the clipping rectangle
  */
void wxVirtualDataViewIFilterEditor::Draw(wxWindow *pWin, wxDC &rDC, wxRect &rRect)
{
    rDC.DrawBitmap(m_EditorBitmap, rRect.GetLeft(), rRect.GetTop());
}

//-------------------- FILTER INTERFACES ----------------------------//
/** Get the filter
  * \return the filter. It is still owned by the wxVirtualDataViewIFilterEditor object
  *         Therefore, do not delete it.
  */
wxVirtualDataViewFilter* wxVirtualDataViewIFilterEditor::GetFilter(void)
{
    return(m_pFilter);
}

/** Set the filter
  * \param pFilter [input]: the new filter, allocated with "new"
  * The previous filter is deleted.
  *
  * Override if the filter should be owned by another object, or if "new" is
  * not desirable (or for any other reason you may like)
  */
void wxVirtualDataViewIFilterEditor::SetFilter(wxVirtualDataViewFilter *pFilter)
{
    if (m_pFilter == pFilter) return;
    ReleaseFilter();
    m_pFilter = pFilter;
}

/** Release the filter
  */
void wxVirtualDataViewIFilterEditor::ReleaseFilter(void)
{
    if (m_pFilter) delete(m_pFilter);
    m_pFilter = WX_VDV_NULL_PTR;
}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy base object
  */
void wxVirtualDataViewIFilterEditor::CopyBase(const wxVirtualDataViewIFilterEditor &rhs)
{
    m_DefaultSize   = rhs.m_DefaultSize;
}

/** Capture the editor to a bitmap
  * \param pEditor [input] : the editor to capture to bitmap
  */
void wxVirtualDataViewIFilterEditor::CaptureToBitmap(wxWindow* pEditor)
{
    if (!pEditor) return;

    //get window rect and create bitmap (destination)
    wxRect rRect = pEditor->GetRect();
    int x = rRect.GetLeft();
    int y = rRect.GetTop();
    int w = rRect.GetWidth();
    int h = rRect.GetHeight();
    pEditor->GetScreenPosition(&x, &y);

#ifdef __WXMAC__
    //on Mac, wxScreenDC::Blit does not work
    char captureCommand[80] =""; // a reasonable max size is 80
    sprintf(captureCommand, "sleep %d;%s", delay, "screencapture -x /tmp/wx_screen_capture.png");
    system(captureCommand);

    wxBitmap bmpFullScreen;
    do
    {
        bmpFullScreen = wxBitmap(wxT("/tmp/wx_screen_capture.png"), wxBITMAP_TYPE_PNG);
    }
    while(!bmpFullScreen.IsOk());

    m_EditorBitmap = bmpFullScreen.GetSubBitmap(wxRect(x, y, w, h));

    //to prevent loading the old screenshot next time
    system("rm /tmp/wx_screen_capture.png");

#else // Under other paltforms, take a real screenshot

    //idea : blit from screen to bitmap
    //source is screen: create a screen DC
    wxScreenDC oScreenDC;
    wxBitmap bmp(w, h);

    //destination DC (memory bitmap)
    wxMemoryDC oMemDC;
    oMemDC.SelectObject(bmp);
    oMemDC.Clear();

    //Blit
    oMemDC.Blit(0, 0, w, h, &oScreenDC, x, y);

    //copy back
    oMemDC.SelectObject(wxNullBitmap);
    m_EditorBitmap = bmp;
#endif // __WXMAC__
}

/** Helper method for destroying an editor
  * \param pEditor [input]: the editor to delete
  */
void wxVirtualDataViewIFilterEditor::DestroyEditor(wxWindow* pEditor)
{
    if (!pEditor) return;

    //draw the editor inside a bitmap
    CaptureToBitmap(pEditor);

    pEditor->Hide();
    wxPendingDelete.Append(pEditor);
}

/** Notify the parent that the filter edition has begun
  * (send a wxCommandEvent of type wxEVT_VDV_FILTER_BEGIN)
  * The event object is set to the parent of the editor
  */
void wxVirtualDataViewIFilterEditor::NotifyFilterBegin(void)
{
    DoNotify(wxEVT_VDV_FILTER_EDITION_BEGIN);
}

/** Notify the parent that the filter value has changed
  * (send a wxCommandEvent of type wxEVT_VDV_FILTER_CHANGED)
  * The event object is set to the parent of the editor
  */
void wxVirtualDataViewIFilterEditor::NotifyFilterChanged(void)
{
    DoNotify(wxEVT_VDV_FILTER_EDITION_CHANGED);
}

/** Notify the parent that the filter edition has ended
  * (send a wxCommandEvent of type wxEVT_VDV_FILTER_END)
  * The event object is set to the parent of the editor
  * \param bCancelled [input]: true if the edition was cancelled (wxCommandEvent::GetInt() return(0))
  *                            false if the edition was allowed (wxCommandEvent::GetInt() return(1))
  */
void wxVirtualDataViewIFilterEditor::NotifyFilterEnd(bool bCancelled)
{
    int iValue = 1;
    if (bCancelled) iValue = 0;
    else            iValue = 1;
    DoNotify(wxEVT_VDV_FILTER_EDITION_END, iValue);
}

/** Notify the parent
  * \param eType [input]: the kind of notification to send
  * The event object is set to the parent of the editor
  */
void wxVirtualDataViewIFilterEditor::DoNotify(wxEventType eType, int iValue)
{
    wxWindow *pEditor = GetEditorWindow();
    if (!pEditor) return;

    wxWindow *pParent = pEditor->GetParent();
    if (!pParent) return;

    wxCommandEvent e(eType);
    e.SetEventObject(pParent);
    e.SetInt(iValue);
    pParent->GetEventHandler()->ProcessEvent(e);
}
