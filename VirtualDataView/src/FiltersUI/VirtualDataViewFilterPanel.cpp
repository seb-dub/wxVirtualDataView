/**********************************************************************/
/** FILE    : VirtualDataViewFilterPanel.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a popup panel for showing values to filter             **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPanel.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterCtrl.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataFilterModel.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewStringFilter.h>
#include <wx/VirtualDataView/Generic/ThemedSearchCtrl.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/srchctrl.h>
#include <wx/systhemectrl.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewFilterPanel, wxPanel);
const char wxVirtualDataViewFilterPanelNameStr[] = "wxVirtualDataViewFilterPanel";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewFilterPanel::wxVirtualDataViewFilterPanel(void)
    : wxPanel(),
      m_pFilterCtrl(WX_VDV_NULL_PTR),
      m_pSearchFilter(WX_VDV_NULL_PTR)
{
}

/** Constructor
  * Creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param sName     [input]: the name of the object
  */
wxVirtualDataViewFilterPanel::wxVirtualDataViewFilterPanel(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                                         const wxSize& size, long lStyle, wxString sName)

    : wxPanel(pParent, id, pos, size, lStyle, sName),
      m_pFilterCtrl(WX_VDV_NULL_PTR),
      m_pSearchFilter(WX_VDV_NULL_PTR)
{
    BuildContents();
}

/** Destructor (virtual)
  */
wxVirtualDataViewFilterPanel::~wxVirtualDataViewFilterPanel(void)
{
    if (m_pSearchFilter) delete(m_pSearchFilter);
    m_pSearchFilter = WX_VDV_NULL_PTR;
}

//----------------------- CREATION METHODS --------------------------//
/** Really creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent        [input]: parent of the window
  * \param id             [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos            [input]: initial position of the window. Default to wxDefaultPosition
  * \param size           [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle         [input]: window style. Default to 0
  * \param sName          [input]: the name of the object
  * \return true on success or false if the control couldn't be created
  */
bool wxVirtualDataViewFilterPanel::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                         const wxSize& size, long lStyle, wxString sName)
{
    //create panel
    if (!wxPanel::Create(pParent, id, pos, size, lStyle, sName))
    {
        return(false);
    }

    BuildContents();
    return(true);
}

/** Build panel contents
  */
void wxVirtualDataViewFilterPanel::BuildContents(void)
{
    //main list
    m_pFilterCtrl = new wxVirtualDataViewFilterCtrl(this);

    //search control
    long lSearchStyle = wxTE_PROCESS_ENTER;
    m_pSearchCtrl = new wxThemedSearchCtrl(this, wxNewId(), wxEmptyString,
                                           wxDefaultPosition, wxDefaultSize, lSearchStyle);
    m_pSearchCtrl->ShowSearchButton(true);
    m_pSearchCtrl->ShowCancelButton(true);
    m_pSearchCtrl->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &wxVirtualDataViewFilterPanel::OnSearchButton, this);
    m_pSearchCtrl->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &wxVirtualDataViewFilterPanel::OnSearchCancelButton, this);
    m_pSearchCtrl->Bind(wxEVT_TEXT_ENTER , &wxVirtualDataViewFilterPanel::OnSearchEnter, this);
    m_pSearchCtrl->Bind(wxEVT_TEXT, &wxVirtualDataViewFilterPanel::OnSearchText, this);

    //measure best button size - otherwise they may appear too big
    int x, y, iDescent;
    GetTextExtent("SelectUnselectAllToggle ", &x, &y, &iDescent);
    if (y >= 0) y++;
    wxSize sSelectButtonSize(-1, y + iDescent);

    //selection buttons
    m_pButtonSelectAll = new wxButton(this, wxNewId(), "Select All", wxDefaultPosition, sSelectButtonSize);
    m_pButtonUnselectAll = new wxButton(this, wxNewId(), "Unselect All", wxDefaultPosition, sSelectButtonSize);
    m_pButtonToggleSelection = new wxButton(this, wxNewId(), "Toggle selection", wxDefaultPosition, sSelectButtonSize);
    m_pButtonSelectAll->Bind(wxEVT_BUTTON, &wxVirtualDataViewFilterPanel::OnSelectAll, this);
    m_pButtonUnselectAll->Bind(wxEVT_BUTTON, &wxVirtualDataViewFilterPanel::OnUnselectAll, this);
    m_pButtonToggleSelection->Bind(wxEVT_BUTTON, &wxVirtualDataViewFilterPanel::OnToggleSelection, this);

    //sizer for selections buttons
    int iAlignFlags = wxALIGN_CENTER_VERTICAL;
    int iBorderFlags = wxUP | wxDOWN; //wxALL
    int iSizerFlags = iBorderFlags | iAlignFlags;

    wxSizer *pSelectButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    pSelectButtonSizer->Add(m_pButtonSelectAll, 1, iSizerFlags, 1);
    pSelectButtonSizer->Add(1,-1,0, iSizerFlags, 1); //spacer
    pSelectButtonSizer->Add(m_pButtonUnselectAll, 1, iSizerFlags, 1);
    pSelectButtonSizer->Add(1,-1,0, iSizerFlags, 1); //spacer
    pSelectButtonSizer->Add(m_pButtonToggleSelection, 1, iSizerFlags, 1);

    //OK/Cancel buttons
    GetTextExtent("OKCancel", &x, &y, &iDescent);
    if (y >= 0) y++;
    wxSize sButtonSize(-1, y + iDescent);

    m_pButtonOK = new wxButton(this, wxNewId(), "OK", wxDefaultPosition, sButtonSize);
    m_pButtonCancel = new wxButton(this, wxNewId(), "Cancel", wxDefaultPosition, sButtonSize);

    //sizer for OK/Cancel buttons
    wxSizer *pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(1,-1,0, iSizerFlags, 1); //spacer
    pButtonSizer->Add(m_pButtonOK, 1, iSizerFlags, 1);
    pButtonSizer->Add(1,-1,0, iSizerFlags, 1); //spacer
    pButtonSizer->Add(m_pButtonCancel, 1, iSizerFlags, 1);
    pButtonSizer->Add(1,-1,0, iSizerFlags, 1); //spacer

    //add to sizer
    wxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(m_pSearchCtrl, 0, wxGROW);
    pMainSizer->Add(pSelectButtonSizer, 0, wxGROW);
    pMainSizer->Add(m_pFilterCtrl, 1, wxGROW, 0);
    pMainSizer->Add(pButtonSizer, 0, wxGROW);

    //ensure that at least 5 items are visible
    size_t uiCount = 5;
    wxVirtualDataFilterModel *pFilterModel = m_pFilterCtrl->GetFilterDataModel();
    if (pFilterModel) uiCount = pFilterModel->GetItemCount();
    wxSize sBestSize = pMainSizer->GetMinSize();
    wxSize sMinSize(sBestSize);
    if (uiCount < 5)
    {
        wxVirtualItemID idRoot = m_pFilterCtrl->GetRootItem();
        wxRect rRectItem;
        m_pFilterCtrl->GetItemRect(rRectItem, idRoot);

        wxSize sFilterMinSize = m_pFilterCtrl->GetMinSize();

        int iHeight = rRectItem.GetHeight() * 5;
        int iMinHeight = sFilterMinSize.GetHeight();
        int iDiff = iHeight - iMinHeight;
        if (iDiff > 0)
        {
            iHeight = sMinSize.GetHeight();
            iHeight += iDiff;
            sMinSize.SetHeight(iHeight);
        }
    }
    m_pFilterCtrl->SetMinSize(sMinSize);
    SetMinSize(sMinSize);
    if (GetParent()) GetParent()->SetMinClientSize(sMinSize);
    SetSizerAndFit(pMainSizer);
}

//---------------------- FILL FILTER --------------------------------//
/** Fill the filter with data from a source model
  * \param pSrcModel   [input]: the data model containing the value to display inside the filter
  * \param uiField     [input]: the field of the data model to consider
  * \param pStateModel [input]: the state model to use
  */
void wxVirtualDataViewFilterPanel::InitFilter(wxVirtualIDataModel *pSrcModel, size_t uiField, wxVirtualIStateModel *pStateModel)
{
    wxVirtualDataViewFilterCtrl *pFilterCtrl = GetFilterControl();
    if (pFilterCtrl)
    {
        pFilterCtrl->InitFilter(pSrcModel, uiField, pStateModel);
    }
    InvalidateBestSize();
    if (GetParent()) GetParent()->InvalidateBestSize();
}

//-------------------- GENERATE FILTER ------------------------------//
/** Generate the filter
  * \return a new filter, allocated with "new"
  */
wxVirtualDataViewFilter* wxVirtualDataViewFilterPanel::GenerateFilter(void)
{
    wxVirtualDataViewStringFilter *pFilter = new wxVirtualDataViewStringFilter;
    if (!pFilter) return(pFilter);
    if (!UpdateFilter(pFilter))
    {
        delete(pFilter);
        pFilter = WX_VDV_NULL_PTR;
    }
    return(pFilter);
}

/** Update a filter
  * \param pFilter [input]: the filter to update
  * \return true on success, false on failure
  */
bool wxVirtualDataViewFilterPanel::UpdateFilter(wxVirtualDataViewFilter *pFilter)
{
    if (!pFilter) return(false);

    //special case
    if (!m_pFilterCtrl) return(false);

    wxVirtualDataFilterModel *pDataModel = m_pFilterCtrl->GetFilterDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pFilterCtrl->GetStateModel();
    if (!pStateModel) return(false);

    //easy case : everything is checked
    size_t uiCount = pDataModel->GetItemCount();
    size_t i, uiNbChecked;
    uiNbChecked = 0;
    for(i=0;i<uiCount;i++)
    {
        wxVirtualItemID id = pDataModel->GetItemID(i);
        if (pStateModel->GetCheck(id) == wxVirtualIStateModel::WX_CHECKSTATE_CHECKED) uiNbChecked++;
        else break;
    }
    if (uiNbChecked == uiCount) return(false);

    //now we need to use a real filter
    wxVirtualDataViewStringFilter *pStringFilter;
    pStringFilter = reinterpret_cast<wxVirtualDataViewStringFilter*>(pFilter);

    pStringFilter->SetActive(true);
    pStringFilter->SetCaseInsensitive();
    pStringFilter->DoNotUseRegEx();
    pStringFilter->UseFullMatchOnly();

    wxArrayString vValues;

    for(i=0;i<uiCount;i++)
    {
        wxVirtualItemID id = pDataModel->GetItemID(i);
        if (pStateModel->GetCheck(id) != wxVirtualIStateModel::WX_CHECKSTATE_CHECKED) continue;

        wxVariant v = pDataModel->GetItemData(id, 0);
        vValues.push_back(GetStringValue(v));
    }

    //do not do it like that, because a search filter might be active in the filter control
//    wxVirtualIDataModel *pDataModel = m_pFilterCtrl->GetDataModel();
//    if (!pDataModel)
//    {
//        pStringFilter->SetReferenceValue(vValues);
//        return(true);
//    }
//    wxVirtualItemID id = m_pFilterCtrl->GetFirstCheckedItem(wxVirtualDataViewCtrl::WX_CHECKSTATE_CHECKED);
//    while (!id.IsInvalid())
//    {
//        wxVariant v = pDataModel->GetItemData(id, 0);
//        vValues.push_back(GetStringValue(v));
//        id = m_pFilterCtrl->GetNextCheckedItem(id, wxVirtualDataViewCtrl::WX_CHECKSTATE_CHECKED);
//    }

    pStringFilter->SetReferenceValue(vValues);
    return(true);
}

//----------------------- MAIN COMPONENTS ---------------------------//
/** Get the search control
  * \return the search control
  */
wxSearchCtrl* wxVirtualDataViewFilterPanel::GetSearchControl(void)
{
    return(m_pSearchCtrl);
}

/** Access to the filter control
  * \return the filter control
  */
wxVirtualDataViewFilterCtrl* wxVirtualDataViewFilterPanel::GetFilterControl(void)
{
    return(m_pFilterCtrl);
}

/** Get button "OK"
  * \return the button "OK"
  */
wxButton* wxVirtualDataViewFilterPanel::GetButtonOK(void)
{
    return(m_pButtonOK);
}

/** Get button "Cancel"
  * \return the button "Cancel"
  */
wxButton* wxVirtualDataViewFilterPanel::GetButtonCancel(void)
{
    return(m_pButtonCancel);
}

//------------------ SIMPLE FILTERING -------------------------------//
/** Apply a search filter
  */
void wxVirtualDataViewFilterPanel::ApplySearchFilter(void)
{
    if (!m_pFilterCtrl) return;
    wxVirtualDataFilterModel *pDataModel = m_pFilterCtrl->GetFilterDataModel();
    if (!pDataModel) return;

    pDataModel->ClearFilter();
    pDataModel->SetFilter(m_pSearchFilter);
    m_pFilterCtrl->OnDataModelChanged();
}

/** Remove any search filter
  */
void wxVirtualDataViewFilterPanel::CancelSearchFilter(void)
{
    if (m_pFilterCtrl)
    {
        wxVirtualDataFilterModel *pDataModel;
        pDataModel = m_pFilterCtrl->GetFilterDataModel();
        if (pDataModel) pDataModel->ClearFilter();
        m_pFilterCtrl->OnDataModelChanged();
    }

    if (m_pSearchFilter) delete(m_pSearchFilter);
    m_pSearchFilter = WX_VDV_NULL_PTR;
}

/** Apply a simple string filter
  * \param rsSearchedText [input]: the text to search
  */
void wxVirtualDataViewFilterPanel::ApplyStringSearchFilter(const wxString &rsSearchedText)
{
    if (rsSearchedText == wxEmptyString)
    {
        CancelSearchFilter();
        return;
    }

    if (m_pSearchFilter) delete(m_pSearchFilter);

    wxVirtualDataViewStringFilter *pStringFilter = new wxVirtualDataViewStringFilter;
    m_pSearchFilter = pStringFilter;
    if (!pStringFilter) return;

    pStringFilter->SetCaseSensitive(false);
    pStringFilter->DoNotUseRegEx();
    pStringFilter->UsePartialMatch();
    pStringFilter->SetReferenceValue(rsSearchedText);

    ApplySearchFilter();
}

//--------------------- EVENT HANDLERS ------------------------------//
/** Select All" pushed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnSelectAll(wxCommandEvent &rEvent)
{
    if (m_pFilterCtrl)
    {
        m_pFilterCtrl->CheckAll(wxVirtualDataViewCtrl::WX_CHECKSTATE_CHECKED, false);
        m_pFilterCtrl->RefreshAll();
    }
}

/** "Unselect All" pushed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnUnselectAll(wxCommandEvent &rEvent)
{
    if (m_pFilterCtrl)
    {
        m_pFilterCtrl->CheckAll(wxVirtualDataViewCtrl::WX_CHECKSTATE_UNCHECKED, false);
        m_pFilterCtrl->RefreshAll();
    }
}

/** "Toggle selection" pushed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnToggleSelection(wxCommandEvent &rEvent)
{
    if (m_pFilterCtrl)
    {
        wxVirtualItemID idRoot = m_pFilterCtrl->GetRootItem();
        m_pFilterCtrl->ToggleCheckState(idRoot, true, false);
        m_pFilterCtrl->RefreshAll();
    }
}

/** Search button pressed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnSearchButton(wxCommandEvent &rEvent)
{
    ApplyStringSearchFilter(rEvent.GetString());
}

/** Search button cancel pressed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnSearchCancelButton(wxCommandEvent &rEvent)
{
    m_pSearchCtrl->ChangeValue(wxEmptyString);
    CancelSearchFilter();
}

/** Enter key pressed in search control
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnSearchEnter(wxCommandEvent &rEvent)
{
    ApplyStringSearchFilter(rEvent.GetString());
}

/** Search text updated
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterPanel::OnSearchText(wxCommandEvent &rEvent)
{
    ApplyStringSearchFilter(rEvent.GetString());
}

//----------------------- SAVE / LOAD STATE -------------------------//
/** Save current state
  */
void wxVirtualDataViewFilterPanel::SaveState(void)
{
    if (m_pFilterCtrl) m_pFilterCtrl->SaveState();
    if (m_pSearchCtrl) m_sSearchedString = m_pSearchCtrl->GetValue();
}

/** Restore state
  */
void wxVirtualDataViewFilterPanel::RestoreState(void)
{
    if (m_pFilterCtrl) m_pFilterCtrl->RestoreState();
    if (m_pSearchCtrl) m_pSearchCtrl->SetValue(m_sSearchedString); //generate event, which is what we want
}

/** Apply a search string
  * \param sSearchedString [input]: the search string to apply.
  *                                 This is generally a string typed by the user in a combobox
  */
void wxVirtualDataViewFilterPanel::ApplySearchString(wxString sSearchedString, bool bExact)
{
    if (sSearchedString == "<custom>")
    {
        if (m_pSearchCtrl) m_pSearchCtrl->SetValue(wxEmptyString); //generate event, which is what we want
        return;
    }

    if (m_pSearchCtrl) m_pSearchCtrl->SetValue(sSearchedString); //generate event, which is what we want
    if (m_pFilterCtrl)
    {
        if (sSearchedString == wxEmptyString)
        {
            m_pFilterCtrl->CheckAll(wxVirtualDataViewCtrl::WX_CHECKSTATE_UNCHECKED, false);
        }
        else
        {
            m_pFilterCtrl->CheckAll(wxVirtualDataViewCtrl::WX_CHECKSTATE_CHECKED, false);
        }
        m_pFilterCtrl->CheckSpecialItems(wxVirtualDataViewCtrl::WX_CHECKSTATE_UNCHECKED);
    }
}
