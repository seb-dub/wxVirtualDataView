/**********************************************************************/
/** FILE    : VirtualDataViewFilterPanel.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a popup panel for showing values to filter             **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_FILTER_PANEL_H_
#define _WX_VIRTUAL_VIEW_FILTER_PANEL_H_

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterCtrl.h>
#include <wx/panel.h>

class WXDLLIMPEXP_CORE wxButton;
class WXDLLIMPEXP_CORE wxSearchCtrl;

class WXDLLIMPEXP_VDV wxVirtualDataViewFilterCtrl;
class WXDLLIMPEXP_VDV wxVirtualDataViewFilter;
class WXDLLIMPEXP_VDV wxThemedSearchCtrl;
extern const char wxVirtualDataViewFilterPanelNameStr[];

/** \class wxVirtualDataViewFilterPanel : a panel for showing the list of values contained in
  * a column of a wxVirtualDataViewCtrl. It is composed of :
  *     - a wxVirtualDataViewFilterCtrl
  *     - a wxSearchControl
  *     - a "OK" button
  *     - a "Cancel" button
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFilterPanel : public wxPanel
{
    public:

        //constructors & destructor
        wxVirtualDataViewFilterPanel(void);                                         ///< \brief default constructor. Create() must be called manually
        wxVirtualDataViewFilterPanel(wxWindow *pParent,
                                     wxWindowID id = wxID_ANY,
                                     const wxPoint& pos = wxDefaultPosition,
                                     const wxSize& size = wxDefaultSize,
                                     long lStyle = 0,
                           wxString sName = wxVirtualDataViewFilterPanelNameStr);   ///< \brief constructor
        virtual ~wxVirtualDataViewFilterPanel(void);                                ///< \brief destructor

        //initialization
        virtual bool Create(wxWindow *pParent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long lStyle = 0,
                            wxString sName = wxVirtualDataViewFilterPanelNameStr);  ///< \brief really creates the control and sets the initial number of items in it

        //fill filter
        virtual void            InitFilter(wxVirtualIDataModel *pSrcModel,
                                           size_t uiField,
                                           wxVirtualIStateModel *pStateModel);                         ///< \brief fill the filter with data from a source model

        //generate filter
        virtual wxVirtualDataViewFilter* GenerateFilter(void);                      ///< \brief generate the filter
        virtual bool UpdateFilter(wxVirtualDataViewFilter *pFilter);                ///< \brief update a filter

        //main components
        wxSearchCtrl*                   GetSearchControl(void);                     ///< \brief get the search control
        wxVirtualDataViewFilterCtrl*    GetFilterControl(void);                     ///< \brief access to the filter control
        wxButton*                       GetButtonOK(void);                          ///< \brief get button "OK"
        wxButton*                       GetButtonCancel(void);                      ///< \brief get button "Cancel"

        //save / load state
        virtual void SaveState(void);                                               ///< \brief save current state
        virtual void RestoreState(void);                                            ///< \brief restore state
        virtual void ApplySearchString(wxString sSearchedString, bool bExact);      ///< \brief apply a search string

    protected:
        //data
        wxThemedSearchCtrl*             m_pSearchCtrl;                              ///< \brief search control
        wxButton*                       m_pButtonOK;                                ///< \brief button "OK"
        wxButton*                       m_pButtonCancel;                            ///< \brief button "Cancel"
        wxButton*                       m_pButtonSelectAll;                         ///< \brief button "Select all"
        wxButton*                       m_pButtonUnselectAll;                       ///< \brief button "Unselect All"
        wxButton*                       m_pButtonToggleSelection;                   ///< \brief button "Toggle selection"
        wxVirtualDataViewFilterCtrl*    m_pFilterCtrl;                              ///< \brief the filter control
        wxVirtualDataViewFilter*        m_pSearchFilter;                            ///< \brief string search filter
        wxString                        m_sSearchedString;                          ///< \brief saved search string

        //methods
        virtual void BuildContents(void);                                           ///< \brief build panel contents
        virtual void ApplySearchFilter(void);                                       ///< \brief apply a search filter
        virtual void CancelSearchFilter(void);                                      ///< \brief remove any search filter
        virtual void ApplyStringSearchFilter(const wxString &rsSearchedText);       ///< \brief apply a simple string filter

        //event handlers
        void OnSelectAll(wxCommandEvent &rEvent);                                   ///< \brief "Select All" pushed
        void OnUnselectAll(wxCommandEvent &rEvent);                                 ///< \brief "Unselect All" pushed
        void OnToggleSelection(wxCommandEvent &rEvent);                             ///< \brief "Toggle selection" pushed
        void OnSearchButton(wxCommandEvent &rEvent);                                ///< \brief search button pressed
        void OnSearchCancelButton(wxCommandEvent &rEvent);                          ///< \brief search button cancel pressed
        void OnSearchEnter(wxCommandEvent &rEvent);                                 ///< \brief Enter key pressed in search control
        void OnSearchText(wxCommandEvent &rEvent);                                  ///< \brief search text updated

    private:
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataViewFilterPanel);
};

#endif // _WX_VLBOX_H_

