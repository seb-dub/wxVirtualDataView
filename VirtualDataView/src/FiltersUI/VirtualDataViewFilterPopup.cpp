/**********************************************************************/
/** FILE    : VirtualDataViewFilterPopup.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a popup dialog for showing values to filter            **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPanel.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPopup.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterComboCtrl.h>
#include <wx/VirtualDataView/Generic/ResizeableWindow.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/app.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewFilterPopup, wxFrame);

/** Default constructor. The method CreatePopup() must be called
  */
wxVirtualDataViewFilterPopup::wxVirtualDataViewFilterPopup(void)
    : wxFrame(),
      m_pFilterPanel(WX_VDV_NULL_PTR),
      m_bKilledFocus(false)
{
}

/** Constructor
  * \param pParent   [input]: parent of the window
  * \param iFlags    [input]: the style flags to pass to wxFrame
  */
wxVirtualDataViewFilterPopup::wxVirtualDataViewFilterPopup(wxWindow *pParent, int iFlags)

    : wxFrame(pParent, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize, iFlags | wxFRAME_NO_TASKBAR),
      m_pFilterPanel(WX_VDV_NULL_PTR),
      m_bKilledFocus(false)
{
    BuildContents();
}

/** Destructor (virtual)
  */
wxVirtualDataViewFilterPopup::~wxVirtualDataViewFilterPopup(void)
{

}

//----------------------- CREATION METHODS --------------------------//
/** Really creates the control
  * \param pParent        [input]: parent of the window
  * \param iFlags    [input]: the style flags to pass to wxPopupTransientWindow
  */
bool wxVirtualDataViewFilterPopup::CreatePopup(wxWindow *pParent, int iFlags)
{
    //create panel
    if (!wxFrame::Create(pParent, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize, iFlags | wxFRAME_NO_TASKBAR))
    {
        return(false);
    }

    BuildContents();
    return(true);
}

/** Build panel contents
  */
void wxVirtualDataViewFilterPopup::BuildContents(void)
{
    m_pResizeableWindow = new wxResizeableWindow(this);
    m_pResizeableWindow->SetNativeResizeBorder(false);
    m_pResizeableWindow->SetResizeBorders(-1, 5, -1, 5);
    m_pFilterPanel = new wxVirtualDataViewFilterPanel(m_pResizeableWindow);

    //add to sizer
    wxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(m_pResizeableWindow, 1, wxGROW, 0);
    SetSizerAndFit(pMainSizer);

    //connect events
    RegisterFocusEvents();
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &wxVirtualDataViewFilterPopup::OnAppActivateEvent, this);
    m_pFilterPanel->GetButtonOK()->Bind(wxEVT_BUTTON, &wxVirtualDataViewFilterPopup::OnButtonOK, this);
    m_pFilterPanel->GetButtonCancel()->Bind(wxEVT_BUTTON, &wxVirtualDataViewFilterPopup::OnButtonCancel, this);

    wxWindow* pTopLevelWin = wxGetTopLevelParent(GetParent());
    if (pTopLevelWin) pTopLevelWin->Bind(wxEVT_ICONIZE, &wxVirtualDataViewFilterPopup::OnIconize, this);
}

//----------------------- MAIN COMPONENTS ---------------------------//
/** Access to the filter panel
  * \return the filter panel
  */
wxVirtualDataViewFilterPanel* wxVirtualDataViewFilterPopup::GetFilterPanel(void)
{
    return(m_pFilterPanel);
}

/** Access to the filter control
  * \return the filter control
  */
wxVirtualDataViewFilterCtrl* wxVirtualDataViewFilterPopup::GetFilterCtrl(void)
{
    if (!m_pFilterPanel) return(WX_VDV_NULL_PTR);
    return(m_pFilterPanel->GetFilterControl());
}

//------------------------ DISMISS ----------------------------------//
/** Dismiss the popup and notify parent of the change
  * \param bAcceptChanges [input]: true for accepting the changes,
  *                                false for discarding them
  */
void wxVirtualDataViewFilterPopup::Dismiss(bool bAcceptChanges)
{
    //hide popup
    Hide();

    //update combo box if needed
    if (bAcceptChanges)
    {
        wxWindow *pParent = GetParent();
        if (!pParent) return;

        wxVirtualDataViewFilterComboCtrl *pComboCtrl;
        pComboCtrl = (wxVirtualDataViewFilterComboCtrl *) pParent;

        wxVirtualDataViewFilterCtrl *pFilterCtrl;
        pFilterCtrl = GetFilterCtrl();
        if (!pFilterCtrl) return;

        //display string
        wxString sDisplay = pFilterCtrl->GetFilterDisplayString();
        pComboCtrl->ChangeValue(sDisplay); //does not generate a wxEVT_TEXT: this is what we want
        pComboCtrl->Dismiss(); //generate a wxEVT_COMBOBOX_CLOSEUP, caught by the editor
    }
    else
    {
        //cancelled : no event generated
        RestoreState();
    }
}

//----------------------- SAVE / LOAD STATE -------------------------//
/** Save current state
  */
void wxVirtualDataViewFilterPopup::SaveState(void)
{
    wxVirtualDataViewFilterPanel *pPanel = GetFilterPanel();
    if (pPanel) pPanel->SaveState();
}

/** Restore state
  */
void wxVirtualDataViewFilterPopup::RestoreState(void)
{
    wxVirtualDataViewFilterPanel *pPanel = GetFilterPanel();
    if (pPanel) pPanel->RestoreState();
}

//----------------------- EVENT HANDLERS ----------------------------//
/** Button "OK" pressed
  */
void wxVirtualDataViewFilterPopup::OnButtonOK(wxCommandEvent &rEvent)
{
    Dismiss(true);
}

/** Button "Cancel" pressed
  */
void wxVirtualDataViewFilterPopup::OnButtonCancel(wxCommandEvent &rEvent)
{
    Dismiss(false);
}

//------------------ wxWindow OVERRIDE ------------------------------//
/** Compute best size - wxWindow override
  * \return the best window size
  */
wxSize wxVirtualDataViewFilterPopup::DoGetBestSize(void) const
{
    if (m_pFilterPanel) m_pFilterPanel->InvalidateBestSize();
    if (m_pResizeableWindow)
    {
        m_pResizeableWindow->InvalidateBestSize();
        return(m_pResizeableWindow->GetBestSize());
    }
    return(wxFrame::DoGetBestSize());
}

//--------------------- POPUP & DISMISS -----------------------------//
/** Check if this window or one of its child has the focus
  * \return true if this window or one of its child has the focus
  *         false otherwise
  */
bool wxVirtualDataViewFilterPopup::ChildHasFocus(void)
{
    wxWindow *pFocusedWin = wxWindow::FindFocus();
    while (pFocusedWin)
    {
        if (pFocusedWin == this) return(true);
        pFocusedWin = pFocusedWin->GetParent();
    }
    return(false);
}

/** Register focus events for this window and all its descendent (children, grand children, ...)
  */
void wxVirtualDataViewFilterPopup::RegisterFocusEvents(void)
{
    RegisterFocusEvent(this);
    Bind(wxEVT_IDLE, &wxVirtualDataViewFilterPopup::OnIdleEvent, this);
}

/** Register focus events for this window and its direct children - recursive
  * \param pWin [input] : the window to register
  */
void wxVirtualDataViewFilterPopup::RegisterFocusEvent(wxWindow *pWin)
{
    pWin->Bind(wxEVT_KILL_FOCUS, &wxVirtualDataViewFilterPopup::OnKillFocusEvent, this);
    pWin->Bind(wxEVT_SET_FOCUS , &wxVirtualDataViewFilterPopup::OnSetFocusEvent, this);

    wxWindowList &vChildren = pWin->GetChildren();
    wxWindowList::iterator it;
    for(it = vChildren.begin(); it != vChildren.end(); ++it)
    {
        wxWindow *pChild = *it;
        if (pChild) RegisterFocusEvent(pChild);
    }
}

/** A kill focus event occurred
  * \param rEvent [input]: the event which occurred
  */
void wxVirtualDataViewFilterPopup::OnKillFocusEvent(wxFocusEvent &rEvent)
{
    m_bKilledFocus = true;

    //for system handling. Must be done all the time
    rEvent.Skip();
}

/** A set focus event occurred
  * \param rEvent [input]: the event which occurred
  */
void wxVirtualDataViewFilterPopup::OnSetFocusEvent(wxFocusEvent &rEvent)
{
    m_bKilledFocus = false;

    //for system handling. Must be done all the time
    rEvent.Skip();
}

/** Idle event handler
  * \param rEvent [input]: the event which occurred
  */
void wxVirtualDataViewFilterPopup::OnIdleEvent(wxIdleEvent &rEvent)
{
    if ((m_bKilledFocus) || (!ChildHasFocus()))
    {
        m_bKilledFocus = false;
        if (IsShown()) Dismiss(false);
    }
    rEvent.Skip();
}

/** Application has been activated / deactivated
  * \param rEvent [input]: the event which occurred
  */
void wxVirtualDataViewFilterPopup::OnAppActivateEvent(wxActivateEvent &rEvent)
{
    if (!rEvent.GetActive())
    {
        if (IsShown()) Dismiss(false);
    }
    rEvent.Skip();
}

/** The top level window has been iconized
  * \param rEvent [input]: the event which occurred
  */
void wxVirtualDataViewFilterPopup::OnIconize(wxIconizeEvent &rEvent)
{
    if (rEvent.IsIconized())
    {
        if (IsShown()) Dismiss(false);
    }
    rEvent.Skip();
}

