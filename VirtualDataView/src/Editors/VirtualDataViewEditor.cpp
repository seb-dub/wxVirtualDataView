/**********************************************************************/
/** FILE    : VirtualDataViewEditor.cpp                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view editor                                     **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/window.h>

wxDEFINE_EVENT(wxEVT_VDV_EDITION_FINISHED, wxCommandEvent);

//idea : end edition when the focus on the editor is lost
//       So when the window lose focus, we need to send NotifyEditionFinished(false)
//       Exception: do not send it when one of the CHILD window of the editor receives the focus
//                  In addition, this apply to grand-child, grand-grand-child, ...
//                  So we need to know if one of the descendent window is receiving the focus or not
//                  For that we need the wxIdleEvent.
//                  The normal sequence of events is :
//                      wxEVT_KILL_FOCUS    on the editor top window
//                      wxEVT_SET_FOCUS to any window in the application. Or it might be not sent in case the app is minimized
//                      wxIDLE_EVENT once
//                  So we bind wxEVT_IDLE on
class wxVirtualDataViewEvtHandler : public wxEvtHandler
{
    public:
        wxVirtualDataViewEvtHandler(wxVirtualDataViewEditor *pEditor)
            : wxEvtHandler(),
              m_pEditor(pEditor)
        {
            Bind(wxEVT_KILL_FOCUS, &wxVirtualDataViewEvtHandler::OnKillFocus, this);
        }

    protected:
        //data
        wxVirtualDataViewEditor*        m_pEditor;

        bool HasFocus(void)
        {
            if (m_pEditor) return(m_pEditor->HasFocus());
            return(false);
        }

        void EndEdition(void)
        {
            if (m_pEditor) m_pEditor->NotifyEditionFinished(false);
        }

        void OnKillFocus(wxFocusEvent &rEvent)
        {
            rEvent.Skip();
            Bind(wxEVT_IDLE, &wxVirtualDataViewEvtHandler::OnIdleEvent, this);
        }

        void OnIdleEvent(wxIdleEvent &rEvent)
        {
            //unbind
            Unbind(wxEVT_IDLE, &wxVirtualDataViewEvtHandler::OnIdleEvent, this);
            if (!HasFocus()) EndEdition();
        }
};

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewEditor::wxVirtualDataViewEditor(void)
    : wxEvtHandler()
{
}

/** Destructor - virtual
  */
wxVirtualDataViewEditor::~wxVirtualDataViewEditor(void)
{

}

//--------------------- START EDITION -------------------------------//
/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewEditor::StartEditing(const wxVirtualItemID &rID, size_t uiField,
                                           const wxVariant &rvInitialValue, wxWindow *pParent,
                                           wxRect rRect)
{
    if (DoStartEditing(rID, uiField, rvInitialValue, pParent, rRect))
    {
        wxWindow *pEditor = GetEditorWindow();
        RegisterFocusHandler(pEditor);
        if (pEditor) pEditor->SetFocus();
        return(true);
    }
    return(false);
}

//------------------------- FOCUS -----------------------------------//
/** Check if the editor has the focus
  * \return true if the editor has the focus, false otherwise
  */
bool wxVirtualDataViewEditor::HasFocus(void)
{
    wxWindow *pEditor = GetEditorWindow();
    if (!pEditor) return(false);

    wxWindow *pFocusedWin = wxWindow::FindFocus();
    while (pFocusedWin)
    {
        if (pFocusedWin == pEditor) return(true);
        pFocusedWin = pFocusedWin->GetParent();
    }
    return(false);
}

//-------------------- INTERNAL METHODS -----------------------------//
/** Copy base object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewEditor::CopyBase(const wxVirtualDataViewEditor &rhs)
{

}

/** Helper method for destroying an editor
  * \param pEditor [input]: the editor to delete
  */
void wxVirtualDataViewEditor::DestroyEditor(wxWindow* pEditor)
{
    if (!pEditor) return;
    pEditor->Hide();

    wxEvtHandler * const pEvtHandler = pEditor->PopEventHandler();

    if (pEvtHandler) wxPendingDelete.Append(pEvtHandler);
    wxPendingDelete.Append(pEditor);
}

/** Lost focus event handler
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewEditor::OnLostFocus(wxFocusEvent &rEvent)
{
    NotifyEditionFinished(false);
    rEvent.Skip();
}

/** Register the focus handler
  */
void wxVirtualDataViewEditor::RegisterFocusHandler(wxWindow *pEditor)
{
    if (!pEditor) return;
    //pEditor->Bind(wxEVT_KILL_FOCUS, &wxVirtualDataViewEditor::OnLostFocus, this);

    wxVirtualDataViewEvtHandler *pEvtHandler = new wxVirtualDataViewEvtHandler(this);
    pEditor->PushEventHandler(pEvtHandler);
}

/** Notify the parent that the edition was finished (send a wxEVT_VDV_EDITION_FINISHED event)
  * The event object is set to the parent of the editor
  * \param bCancelled [input]: true if the edition was cancelled (wxCommandEvent::GetInt() return(0))
  *                            false if the edition was allowed (wxCommandEvent::GetInt() return(1))
  */
void wxVirtualDataViewEditor::NotifyEditionFinished(bool bCancelled)
{
    wxWindow *pEditor = GetEditorWindow();
    if (!pEditor) return;

    wxWindow *pParent = pEditor->GetParent();
    if (!pParent) return;

    wxCommandEvent e(wxEVT_VDV_EDITION_FINISHED);
    if (bCancelled) e.SetInt(0);
    else            e.SetInt(1);

    e.SetEventObject(pParent);
    pParent->GetEventHandler()->ProcessEvent(e);
}



