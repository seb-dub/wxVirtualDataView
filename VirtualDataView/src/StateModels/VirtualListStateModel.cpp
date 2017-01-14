/**********************************************************************/
/** FILE    : VirtualListStateModel.cpp                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : state model optimized for list views                   **/
/**********************************************************************/

#include <wx/VirtualDataView/StateModels/VirtualListStateModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

//store state on 1 int (actually 5 bits per item are enough)
//default state : enabled, unselected, unchecked, EXPANDED : value = 0
//enabled : bit 0 unset disabled : bit 0 set
//selected : bit 1 set
//collapsed : bit 2 set (this is the reversed default state from wxVirtualDefaultStateModel. Watch out !)
//check state : bits 3 & 4

//macro
#define WX_IS_DISABLED(iValue)              (iValue & 0x01)
#define WX_IS_ENABLED(iValue)               ((iValue & 0x01) == 0)
#define WX_IS_SELECTED(iValue)              (iValue & 0x02)
#define WX_IS_COLLAPSED(iValue)             (iValue & 0x04)
#define WX_GET_CHECKSTATE(iValue)           (wxVirtualIStateModel::ECheckState) ((iValue >> 3) & 0x03)

#define WX_ENABLE(iValue)                   iValue &= ~0x01
#define WX_DISABLE(iValue)                  iValue |= 0x01
#define WX_SELECT(iValue)                   iValue |= 0x02
#define WX_DESELECT(iValue)                 iValue &= ~0x02
#define WX_EXPAND(iValue)                   iValue &= ~0x04
#define WX_COLLAPSE(iValue)                 iValue |= 0x04
#define WX_SET_CHECKSTATE(iValue, iState)   iValue &= ~0x18; iValue |= ((iState << 3) & 0x18);

//----------------------- CONSTRUCTORS & DESTRUCTOR -----------------//
/** Default constructor
  */
wxVirtualListStateModel::wxVirtualListStateModel(void)
    : wxVirtualIStateModel(),
      m_pDataModel(WX_VDV_NULL_PTR)
{
}

/** Destructor
  */
wxVirtualListStateModel::~wxVirtualListStateModel(void)
{

}

//--------------------- ITEM ID METHODS -----------------------------//
/** Compute an item ID from a row index
  * \param uiRowIndex [input]: the row index of the item
  * \return an item ID for this item
  */
WX_VDV_INLINE wxVirtualItemID wxVirtualListStateModel::DoGetItemID(size_t uiRowIndex) const
{
    return(wxVirtualItemID(uiRowIndex + 1, 0, 0, m_pDataModel));
}

/** Compute the row index from the item ID
  * \param rID [input]: the item ID
  * \return the row index
  */
WX_VDV_INLINE size_t wxVirtualListStateModel::DoGetRowIndex(const wxVirtualItemID &rID) const
{
    return(rID.GetInt64() - 1);
}


/** Compute an item ID from a row index
  * \param uiRowIndex [input]: the row index of the item
  * \return an item ID for this item
  */
wxVirtualItemID wxVirtualListStateModel::GetItemID(size_t uiRowIndex) const
{
    return(DoGetItemID(uiRowIndex));
}

/** Compute the row index from the item ID
  * \param rID [input]: the item ID
  * \return the row index
  */
size_t wxVirtualListStateModel::GetRowIndex(const wxVirtualItemID &rID) const
{
    return(DoGetRowIndex(rID));
}

//-------------------- INTERNAL METHODS -----------------------------//
/** Get the state
  * \param rID [input]: the item to query
  * \return the state of the item
  */
WX_VDV_INLINE int wxVirtualListStateModel::GetState(const wxVirtualItemID &rID) const
{
    if (rID.IsRootItem()) return(0);
    size_t uiRow = DoGetRowIndex(rID);
    return(GetState(uiRow));
}

/** Get the state
  * \param uiRow [input]: the row index of the item to query
  * \return the state of the item
  */
WX_VDV_INLINE int wxVirtualListStateModel::GetState(size_t uiRow) const
{
    THashMapListState::const_iterator it = m_MapOfStates.find(uiRow);

    int iState = 0;
    if (it != m_MapOfStates.end()) iState = it->second;
    return(iState);
}

/** Set the state
  * \param rID    [input]: the item to modify
  * \param iState [input]: the new state
  */
WX_VDV_INLINE void wxVirtualListStateModel::SetState(const wxVirtualItemID &rID, int iState)
{
    size_t uiRow = DoGetRowIndex(rID);
    SetState(uiRow, iState);
}

/** Set the state
  * \param uiRow [input]: the row index of the item to modify
  * \param iState [input]: the new state
  */
WX_VDV_INLINE void wxVirtualListStateModel::SetState(size_t uiRow, int iState)
{
    if (iState == 0)
    {
        m_MapOfStates.erase(uiRow);
        return;
    }

    m_MapOfStates[uiRow] = iState;
}

//------ wxVirtualIStateModeL INTERFACE IMPLEMENTATION --------------//
/** Check if the item is enabled / disabled
  * \param rID [input]: the item to check
  * \return always true
  */
bool wxVirtualListStateModel::IsEnabled(const wxVirtualItemID &rID)
{
    if (rID.IsRootItem()) return(true); //root always enabled

    int iState = GetState(rID);
    return(WX_IS_ENABLED(iState));
}

/** Check if the item is selected / deselected
  * \param rID [input]: the item to check
  * \return true if the item is selected, false if it is deselected
  */
bool wxVirtualListStateModel::IsSelected(const wxVirtualItemID &rID)
{
    if (rID.IsRootItem()) return(false); //root always deselected

    int iState = GetState(rID);
    return(WX_IS_SELECTED(iState));
}

/** Check if the item has the focus
  * \param rID [input]: the item to check
  * \return true if the item has the focus, false otherwise
  */
bool wxVirtualListStateModel::HasFocus(const wxVirtualItemID &rID)
{
    if (rID.IsRootItem()) return(false); //no focus on root item

    if (rID == m_FocusedItem) return(true);
    return(false);
}

/** Check if the item is expanded / collapsed
  * \param rID [input]: the item to check
  * \return true if the item has the focus, false otherwise
  */
bool wxVirtualListStateModel::IsExpanded(const wxVirtualItemID &rID)
{
    if (rID.IsRootItem()) return(true); //root item always expanded

    int iState = GetState(rID);
    return(!(WX_IS_COLLAPSED(iState)));
}

/** Get the check state of the item
  * \param rID [input]: the item to query
  * \return the check state of the item
  */
wxVirtualIStateModel::ECheckState wxVirtualListStateModel::GetCheck(const wxVirtualItemID &rID)
{
    int iState = GetState(rID);
    return(WX_GET_CHECKSTATE(iState));
}

/** Get the focused item
  * \return the ID of the focused item
  */
wxVirtualItemID wxVirtualListStateModel::GetFocusedItem(void)
{
    return(m_FocusedItem);
}

/** Get the list of selected items
  * \param vIDs [output]: the list of selected items. The previous content is erased
  * \return the amount of currently selected items
  */
size_t wxVirtualListStateModel::GetSelectedItems(wxVirtualItemIDs &vIDs)
{
    vIDs.clear();
    vIDs.reserve(m_MapOfStates.size());
    if (!m_pDataModel) return(vIDs.size());

    int iState;
    THashMapListState::iterator it    = m_MapOfStates.begin();
    THashMapListState::iterator itEnd = m_MapOfStates.end();
    while(it != itEnd)
    {
        iState = it->second;
        if (WX_IS_SELECTED(iState)) vIDs.push_back(DoGetItemID(it->first));
        ++it;
    }

    return(vIDs.size());
}

//---------- INTERFACE WITH DEFAULT IMPLEMENTATION ------------------//
/** Set all states at once
  * \param rState   [input] : the state to set
  * \param rID      [input] : the ID of the item to modify
  * \param bRecurse [input] : apply the state on all the children as well
  * \return true if all states were set, false otherwise
  */
bool wxVirtualListStateModel::SetState(const wxVirtualDataViewItemState &rState,
                                       const wxVirtualItemID &rID,
                                       bool bRecurse)
{
    bool bResult = true;

    int iState = 0;
    if (rState.IsEnabled())     WX_ENABLE(iState);
    else                        WX_DISABLE(iState);

    if (rState.IsSelected())    WX_SELECT(iState);
    else                        WX_DESELECT(iState);

    if (rState.IsExpanded())    WX_EXPAND(iState);
    else                        WX_COLLAPSE(iState);

    WX_SET_CHECKSTATE(iState, rState.GetCheckState());
    SetState(rID, iState);

    return(bResult);
}

/** Get all states at once
  * \param rState [output]: the state of the item
  * \param rID    [input] : the ID of the item to query
  */
void wxVirtualListStateModel::GetState(wxVirtualDataViewItemState &rState,
                                       const wxVirtualItemID &rID)
{
    int iState = GetState(rID);

    rState.Enable(WX_IS_ENABLED(iState));
    rState.SetCheckState(WX_GET_CHECKSTATE(iState));
    rState.Select(WX_IS_SELECTED(iState));
    if (WX_IS_COLLAPSED(iState)) rState.Collapse(); else rState.Expand();
    if (m_FocusedItem == rID) rState.SetFocused(true);
    else                      rState.SetFocused(false);
}

//------------------ CALLBACKS --------------------------------------//
/** Data model has changed
  * \param pDataModel [input]: the new data model
  */
void wxVirtualListStateModel::OnDataModelChanged(wxVirtualIDataModel* pDataModel)
{
    m_pDataModel = pDataModel;
}

/** Delete an item
  * \param rID [input]: the item to delete
  * \return true if the item was deleted, false otherwise
  */
bool wxVirtualListStateModel::DeleteItem(const wxVirtualItemID &rID)
{
    size_t uiRow = DoGetRowIndex(rID);
    m_MapOfStates.erase(uiRow);
    return(true);
}

/** Enable / disable items
  * \param rID        [input]: the item to modify
  * \param bEnable    [input]: true for enabling the item, false for disabling it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::Enable(const wxVirtualItemID &rID, bool bEnable, bool bRecursive)
{
    if ((bRecursive) && (rID.IsRootItem()))
    {
        wxVirtualIDataModel *pDataModel = rID.GetTopModel();
        if (!pDataModel)
        {
            pDataModel = m_pDataModel;
            if (!pDataModel) return(false);
        }

        wxVirtualItemID rRoot = pDataModel->GetRootItem();
        size_t uiNbChildren = pDataModel->GetChildCount(rRoot);
        size_t uiChild;
        if (bEnable)
        {
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                int iStateChild = GetState(uiChild);
                WX_ENABLE(iStateChild);
                SetState(rID, iStateChild);
            }
        }
        else
        {
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                int iStateChild = GetState(uiChild);
                WX_DISABLE(iStateChild);
                SetState(rID, iStateChild);
            }
        }
        return(true);
    }

    int iState = GetState(rID);
    if (bEnable) WX_ENABLE(iState); else WX_DISABLE(iState);
    SetState(rID, iState);
    return(true);
}

/** Select / deselect items
  * \param rID        [input]: the item to modify
  * \param bSelect    [input]: true for selecting the item, false for deselecting it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::Select(const wxVirtualItemID &rID, bool bSelect, bool bRecursive)
{
    if ((bRecursive) && (rID.IsRootItem()))
    {
        wxVirtualIDataModel *pDataModel = rID.GetTopModel();
        if (!pDataModel)
        {
            pDataModel = m_pDataModel;
            if (!pDataModel) return(false);
        }

        wxVirtualItemID rRoot = pDataModel->GetRootItem();
        size_t uiNbChildren = pDataModel->GetChildCount(rRoot);
        size_t uiChild;
        if (bSelect)
        {
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                int iStateChild = GetState(uiChild);
                WX_SELECT(iStateChild);
                SetState(rID, iStateChild);
            }
        }
        else
        {
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                int iStateChild = GetState(uiChild);
                WX_DESELECT(iStateChild);
                SetState(rID, iStateChild);
            }
        }
        return(true);
    }

    int iState = GetState(rID);
    if (bSelect) WX_SELECT(iState); else WX_DESELECT(iState);
    SetState(rID, iState);
    return(true);
}

/** Select a range of items.
  * \param rID1 [input] : the 1st item in the range
  * \param rID2 [input] : the 2nd item in the range
  * \param bSelect [input]: true for selection, false for de-selecting
  * \return true if the state of the items have changed, false otherwise
  */
bool wxVirtualListStateModel::SelectRange(const wxVirtualItemID &rID1, const wxVirtualItemID &rID2, bool bSelect)
{
    size_t uiRow1 = DoGetRowIndex(rID1);
    size_t uiRow2 = DoGetRowIndex(rID2);
    if (uiRow1 > uiRow2)
    {
        size_t uiSwap = uiRow1;
        uiRow1 = uiRow2;
        uiRow2 = uiSwap;
    };

    size_t uiRow;
    if (bSelect)
    {
        for(uiRow = uiRow1; uiRow <= uiRow2; uiRow++)
        {
            int iState = GetState(uiRow);
            WX_SELECT(iState);
            SetState(uiRow, iState);
        }
    }
    else
    {
        for(uiRow = uiRow1; uiRow <= uiRow2; uiRow++)
        {
            int iState = GetState(uiRow);
            WX_DESELECT(iState);
            SetState(uiRow, iState);
        }
    }

    return(true);
}

/** Select / deselect a list of items
  * \param vIDs [input]: the list of items to select / deselect
  * \param bSelect [input]: true for selection, false for de-selecting
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::SelectList(const wxVirtualItemIDs &vIDs, bool bSelect)
{
    size_t i, uiCount;
    uiCount = vIDs.size();

    if (bSelect)
    {
        for(i=0;i<uiCount;i++)
        {
            size_t uiRow = DoGetRowIndex(vIDs[i]);
            int iState = GetState(uiRow);
            WX_SELECT(iState);
            SetState(uiRow, iState);
        }
    }
    else
    {
        for(i=0;i<uiCount;i++)
        {
            size_t uiRow = DoGetRowIndex(vIDs[i]);
            int iState = GetState(uiRow);
            WX_DESELECT(iState);
            SetState(uiRow, iState);
        }
    }

    return(true);
}

/** Select all items
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::SelectAll(void)
{
    wxVirtualIDataModel *pDataModel = m_pDataModel;
    if (!pDataModel) return(false);

    wxVirtualItemID rIDRoot = pDataModel->GetRootItem();
    size_t uiChildCount = pDataModel->GetChildCount(rIDRoot);

    size_t uiChild;
    for(uiChild = 0; uiChild < uiChildCount; uiChild++)
    {
        int iState = GetState(uiChild);
        WX_SELECT(iState);
        SetState(uiChild, iState);
    }

    return(true);
}

/** Deselect all items
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::DeselectAll(void)
{
    THashMapListState::iterator it      = m_MapOfStates.begin();
    THashMapListState::iterator itEnd   = m_MapOfStates.end();
    while (it != itEnd)
    {
        WX_DESELECT(it->second);
        ++it;
    }
    return(true);
}

/** Expand / collapse items
  * \param rID        [input]: the item to modify
  * \param bExpand    [input]: true for expanding the item, false for collapsing it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::Expand(const wxVirtualItemID &rID, bool bExpand, bool bRecursive)
{
    if ((bRecursive) && (rID.IsRootItem()))
    {
        wxVirtualIDataModel *pDataModel = rID.GetTopModel();
        if (!pDataModel)
        {
            pDataModel = m_pDataModel;
            if (!pDataModel) return(false);
        }

        wxVirtualItemID rRoot = pDataModel->GetRootItem();
        size_t uiNbChildren = pDataModel->GetChildCount(rRoot);
        size_t uiChild;
        if (bExpand)
        {
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                int iStateChild = GetState(uiChild);
                WX_EXPAND(iStateChild);
                SetState(rID, iStateChild);
            }
        }
        else
        {
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                int iStateChild = GetState(uiChild);
                WX_COLLAPSE(iStateChild);
                SetState(rID, iStateChild);
            }
        }
        return(true);
    }

    size_t uiRowIndex = DoGetRowIndex(rID);
    int iState = GetState(uiRowIndex);
    if (bExpand) WX_EXPAND(iState); else WX_COLLAPSE(iState);
    SetState(uiRowIndex, iState);
    return(true);
}

/** Set the check state of the item
  * \param rID        [input]: the item to modify
  * \param eCheck     [input]: the new check state
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::SetCheck(const wxVirtualItemID &rID, ECheckState eCheck,
                                          bool bRecursive)
{
    if ((bRecursive) && (rID.IsRootItem()))
    {
        wxVirtualIDataModel *pDataModel = rID.GetTopModel();
        if (!pDataModel)
        {
            pDataModel = m_pDataModel;
            if (!pDataModel) return(false);
        }

        wxVirtualItemID rRoot = pDataModel->GetRootItem();
        size_t uiNbChildren = pDataModel->GetChildCount(rRoot);
        size_t uiChild;
        for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
        {
            int iStateChild = GetState(uiChild);
            WX_SET_CHECKSTATE(iStateChild, eCheck);
            SetState(rID, iStateChild);
        }

        return(true);
    }

    size_t uiRowIndex = DoGetRowIndex(rID);
    int iState = GetState(uiRowIndex);
    WX_SET_CHECKSTATE(iState, eCheck);
    SetState(uiRowIndex, iState);
    return(true);
}

/** Set focus on one item
  * \param rID [input]: the item to focus
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualListStateModel::SetFocus(const wxVirtualItemID &rID)
{
    m_FocusedItem = rID;
    return(true);
}

