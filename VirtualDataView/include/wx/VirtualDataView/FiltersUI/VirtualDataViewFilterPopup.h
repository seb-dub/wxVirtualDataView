/**********************************************************************/
/** FILE    : VirtualDataViewFilterDialog.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a popup dialog for showing values to filter            **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_FILTER_POPUP_H_
#define _WX_VIRTUAL_VIEW_FILTER_POPUP_H_

#include <wx/frame.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewFilterCtrl;
class WXDLLIMPEXP_VDV wxVirtualDataViewFilterPanel;
class WXDLLIMPEXP_VDV wxResizeableWindow;

/** \class wxVirtualDataViewFilterPopup : a popup dialog for showing the list of values contained in
  * a column of a wxVirtualDataViewCtrl. It is composed of :
  *     - a wxVirtualDataViewFilterCtrl
  *     - a wxSearchControl
  *     - a "OK" button
  *     - a "Cancel" button
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFilterPopup : public wxFrame
{
    public:

        //constructors & destructor
        wxVirtualDataViewFilterPopup(void);                                     ///< \brief default constructor. CreatePopup() must be called manually
        wxVirtualDataViewFilterPopup(wxWindow *pParent,
                                     int iFlags = wxBORDER_SIMPLE);             ///< \brief constructor
        virtual ~wxVirtualDataViewFilterPopup(void);                            ///< \brief destructor

        //initialization
        virtual bool CreatePopup(wxWindow *pParent,
                                 int iFlags = wxBORDER_SIMPLE);                 ///< \brief really creates the control and sets the initial number of items in it

        //main components
        wxVirtualDataViewFilterPanel*    GetFilterPanel(void);                  ///< \brief access to the filter control
        wxVirtualDataViewFilterCtrl*     GetFilterCtrl(void);                   ///< \brief access to the filter control

        //dismiss
        virtual void Dismiss(bool bAcceptChanges);                              ///< \brief dismiss the popup and notify parent of the change

        //save / load state
        virtual void SaveState(void);                                           ///< \brief save current state
        virtual void RestoreState(void);                                        ///< \brief restore state

    protected:
        //data
        wxResizeableWindow*              m_pResizeableWindow;                   ///< \brief resizeable window
        wxVirtualDataViewFilterPanel*    m_pFilterPanel;                        ///< \brief the filter control
        bool                             m_bKilledFocus;                        ///< \brief true when a kill focus was received

        //methods
        virtual void BuildContents(void);                                       ///< \brief build panel contents
        virtual void OnButtonOK(wxCommandEvent &rEvent);                        ///< \brief button "OK" pressed
        virtual void OnButtonCancel(wxCommandEvent &rEvent);                    ///< \brief button "Cancel" pressed
        virtual wxSize DoGetBestSize(void) const;                               ///< \brief compute best size - wxWindow override

        //popup & focus
        bool ChildHasFocus(void);                                               ///< \brief check if this window or one of its child has the focus
        void RegisterFocusEvents(void);                                         ///< \brief register focus events
        void RegisterFocusEvent(wxWindow *pWin);                                ///< \brief register focus events - recursive
        void OnKillFocusEvent(wxFocusEvent &rEvent);                            ///< \brief a kill focus event occurred
        void OnSetFocusEvent(wxFocusEvent &rEvent);                             ///< \brief a set focus event occurred
        void OnIdleEvent(wxIdleEvent &rEvent);                                  ///< \brief idle event handler
        void OnAppActivateEvent(wxActivateEvent &rEvent);                       ///< \brief application has been activated / deactivated
        void OnIconize(wxIconizeEvent &rEvent);                                 ///< \brief the top level window has been iconized

    private:
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataViewFilterPopup);
};

#endif // _WX_VLBOX_H_

