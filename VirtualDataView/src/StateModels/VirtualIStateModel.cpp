/**********************************************************************/
/** FILE    : VirtualIStateModel.cpp                                 **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a state model                             **/
/**********************************************************************/

#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/Utilities/VirtualItemIDSearch.hpp>

//----------------------- CONSTRUCTORS & DESTRUCTOR -----------------//
/** Default constructor
  */
wxVirtualIStateModel::wxVirtualIStateModel(void)
{
}

/** Destructor
  */
wxVirtualIStateModel::~wxVirtualIStateModel(void)
{

}

//------------------ HELPER METHOD FOR FINDING ITEMS ----------------//
/** Helper for finding first item with a binary state
  * \param TJobON           [template]: the search functor to use if the state is ON
  * \param TJobOFF          [template]: the search functor to use if the state is OFF
  * \param pStateModel         [input]: the state model
  * \param pDataModel          [input]: the data model
  * \param bON                 [input]: true if the state is ON, false if the state is OFF
  * \return the found item, or an invalid item if not found
  */
template<typename TJobON, typename TJobOFF>
wxVirtualItemID FindFirstItemWithState(wxVirtualIStateModel *pStateModel, wxVirtualIDataModel *pDataModel,
                                       bool bON)
{
    if (!pDataModel) return(wxVirtualItemID());
    wxVirtualItemID id = pDataModel->GetRootItem();

    if (bON)
    {
        TJobON oJob(pStateModel, pDataModel, id);
        return(wxGetNextVirtualItemID<TJobON, false>(oJob));
    }
    else
    {
        TJobOFF oJob(pStateModel, pDataModel, id);
        return(wxGetNextVirtualItemID<TJobOFF, false>(oJob));
    }
    return(wxVirtualItemID());
}

/** Helper for finding next item with a binary state
  * \param TJobON           [template]: the search functor to use if the state is ON
  * \param TJobOFF          [template]: the search functor to use if the state is OFF
  * \param pStateModel         [input]: the state model
  * \param pDataModel          [input]: the data model
  * \param idStart             [input]: the start item. It is excluded from the search
  * \param bON                 [input]: true if the state is ON, false if the state is OFF
  * \return the found item, or an invalid item if not found
  */
template<typename TJobON, typename TJobOFF>
wxVirtualItemID FindNextItemWithState(wxVirtualIStateModel *pStateModel, wxVirtualIDataModel *pDataModel,
                                      const wxVirtualItemID &idStart, bool bON)
{
    if (!pDataModel) return(wxVirtualItemID());
    if (bON)
    {
        TJobON oJob(pStateModel, pDataModel, idStart);
        return(wxGetNextVirtualItemID<TJobON, false>(oJob));
    }
    else
    {
        TJobOFF oJob(pStateModel, pDataModel, idStart);
        return(wxGetNextVirtualItemID<TJobOFF, false>(oJob));
    }
    return(wxVirtualItemID());
}

/** Helper for finding previous item with a binary state
  * \param TJobON           [template]: the search functor to use if the state is ON
  * \param TJobOFF          [template]: the search functor to use if the state is OFF
  * \param pStateModel         [input]: the state model
  * \param pDataModel          [input]: the data model
  * \param idStart             [input]: the start item. It is excluded from the search
  * \param bON                 [input]: true if the state is ON, false if the state is OFF
  * \return the found item, or an invalid item if not found
  */
template<typename TJobON, typename TJobOFF>
wxVirtualItemID FindPrevItemWithState(wxVirtualIStateModel *pStateModel, wxVirtualIDataModel *pDataModel,
                                      const wxVirtualItemID &idStart, bool bON)
{
    if (!pDataModel) return(wxVirtualItemID());
    if (bON)
    {
        TJobON oJob(pStateModel, pDataModel, idStart);
        return(wxGetPrevVirtualItemID<TJobON, false>(oJob));
    }
    else
    {
        TJobOFF oJob(pStateModel, pDataModel, idStart);
        return(wxGetPrevVirtualItemID<TJobOFF, false>(oJob));
    }
    return(wxVirtualItemID());
}

//------------------------ GET SELECTED/UNSELECTED ITEMS ------------//
/** Get the first selected item
  * \return the 1st selected item
  */
wxVirtualItemID wxVirtualIStateModel::GetFirstSelectedItem(void)
{
    wxVirtualItemIDs vIDs;
    GetSelectedItems(vIDs);
    if (vIDs.size() > 0) return(vIDs[0]);
    return(wxVirtualItemID());
}

/** Get the first selected / unselected item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bSelected  [input]: true for getting the first selected item
  *                            false for getting the first unselected item
  * \return the first selected / unselected item
  */
wxVirtualItemID wxVirtualIStateModel::GetFirstSelectedItem(wxVirtualIDataModel *pDataModel, bool bSelected)
{
    return(FindFirstItemWithState<wxVirtualItemSearchSelected,
                                  wxVirtualItemSearchUnselected>(this, pDataModel, bSelected));
}

/** Get the previous selected / unselected item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bSelected  [input]: true for getting the previous selected item
  *                            false for getting the previous unselected item
  * \return the previous selected / unselected item
  */
wxVirtualItemID wxVirtualIStateModel::GetPrevSelectedItem(const wxVirtualItemID &rID,
                                                          wxVirtualIDataModel *pDataModel,
                                                          bool bSelected)
{
    return(FindPrevItemWithState<wxVirtualItemSearchSelected,
                                  wxVirtualItemSearchUnselected>(this, pDataModel, rID, bSelected));
}

/** Get the next selected / unselected item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bSelected  [input]: true for getting the next selected item
  *                            false for getting the next unselected item
  * \return the next selected / unselected item
  */
wxVirtualItemID wxVirtualIStateModel::GetNextSelectedItem(const wxVirtualItemID &rID,
                                                          wxVirtualIDataModel *pDataModel,
                                                          bool bSelected)
{
    return(FindNextItemWithState<wxVirtualItemSearchSelected,
                                  wxVirtualItemSearchUnselected>(this, pDataModel, rID, bSelected));
}

//----------------- GET CHECKED / UNCHECKED ITEMS -------------------//
/** Get the first checked/unchecked/mixed item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param eState     [input]: the searched state of the item
  * \return the first item with the searched state
  */
wxVirtualItemID wxVirtualIStateModel::GetFirstCheckedItem(wxVirtualIDataModel *pDataModel, ECheckState eState)
{
    if (!pDataModel) return(wxVirtualItemID());
    wxVirtualItemID id = pDataModel->GetRootItem();

    wxVirtualItemSearchChecked oJob(this, pDataModel, id, eState);
    return(wxGetNextVirtualItemID<wxVirtualItemSearchChecked, false>(oJob));
}

/** Get the previous checked/unchecked/mixed item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param eState     [input]: the searched state of the item
  * \return the previous item with the searched state
  */
wxVirtualItemID wxVirtualIStateModel::GetPrevCheckedItem(const wxVirtualItemID &rID,
                                                         wxVirtualIDataModel *pDataModel,
                                                         ECheckState eState)
{
    if (!pDataModel) return(wxVirtualItemID());
    wxVirtualItemSearchChecked oJob(this, pDataModel, rID, eState);
    return(wxGetPrevVirtualItemID<wxVirtualItemSearchChecked, true>(oJob));
}

/** Get the next checked/unchecked/mixed item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param eState     [input]: the searched state of the item
  * \return the next item with the searched state
  */
wxVirtualItemID wxVirtualIStateModel::GetNextCheckedItem(const wxVirtualItemID &rID,
                                                   wxVirtualIDataModel *pDataModel,
                                                   ECheckState eState)
{
    if (!pDataModel) return(wxVirtualItemID());
    wxVirtualItemSearchChecked oJob(this, pDataModel, rID, eState);
    return(wxGetNextVirtualItemID<wxVirtualItemSearchChecked, true>(oJob));
}

//------------------------ GET ENABLED/DISBALED ITEMS ---------------//
/** Get the first enabled/disabled item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bEnabled  [input]: true for getting the first enabled item
  *                            false for getting the first disabled item
  * \return the first enabled/disabled item
  */
wxVirtualItemID wxVirtualIStateModel::GetFirstEnabledItem(wxVirtualIDataModel *pDataModel, bool bEnabled)
{
    return(FindFirstItemWithState<wxVirtualItemSearchEnabled,
                                  wxVirtualItemSearchDisabled>(this, pDataModel, bEnabled));
}

/** Get the previous enabled/disabled item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bEnabled  [input]: true for getting the previous enabled item
  *                            false for getting the previous disabled item
  * \return the previous enabled/disabled item
  */
wxVirtualItemID wxVirtualIStateModel::GetPrevEnabledItem(const wxVirtualItemID &rID,
                                                         wxVirtualIDataModel *pDataModel,
                                                         bool bEnabled)
{
    return(FindPrevItemWithState<wxVirtualItemSearchEnabled,
                                 wxVirtualItemSearchDisabled>(this, pDataModel, rID, bEnabled));
}

/** Get the next enabled/disabled item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bEnabled   [input]: true for getting the next enabled item
  *                            false for getting the next disabled item
  * \return the next enabled/disabled item
  */
wxVirtualItemID wxVirtualIStateModel::GetNextEnabledItem(const wxVirtualItemID &rID,
                                                         wxVirtualIDataModel *pDataModel,
                                                         bool bEnabled)
{
    return(FindNextItemWithState<wxVirtualItemSearchEnabled,
                                 wxVirtualItemSearchDisabled>(this, pDataModel, rID, bEnabled));
}

//------------------------ GET EXPANDED/COLLAPSED ITEMS -------------//
/** Get the first expanded/collapsed item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bExpanded  [input]: true for getting the first expanded item
  *                            false for getting the first collapsed item
  * \return the first expanded/collapsed item
  */
wxVirtualItemID wxVirtualIStateModel::GetFirstExpandedItem(wxVirtualIDataModel *pDataModel, bool bExpanded)
{
    return(FindFirstItemWithState<wxVirtualItemSearchExpanded,
                                  wxVirtualItemSearchCollapsed>(this, pDataModel, bExpanded));
}

/** Get the previous expanded/collapsed item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bExpanded  [input]: true for getting the previous expanded item
  *                            false for getting the previous collapsed item
  * \return the previous expanded/collapsed item
  */
wxVirtualItemID wxVirtualIStateModel::GetPrevExpandedItem(const wxVirtualItemID &rID,
                                                         wxVirtualIDataModel *pDataModel,
                                                         bool bExpanded)
{
    return(FindPrevItemWithState<wxVirtualItemSearchExpanded,
                                 wxVirtualItemSearchCollapsed>(this, pDataModel, rID, bExpanded));
}

/** Get the next expanded/collapsed item
  * The order of the items is the same as the one given in wxVirtualIDataModel::NextItem/PrevItem
  * \param rID        [input]: the item at which to start the search. This item is excluded
  * \param pDataModel [input]: the data model used to define the order of the items
  * \param bExpanded  [input]: true for getting the next expanded item
  *                            false for getting the next collapsed item
  * \return the next expanded/collapsed item
  */
wxVirtualItemID wxVirtualIStateModel::GetNextExpandedItem(const wxVirtualItemID &rID,
                                                         wxVirtualIDataModel *pDataModel,
                                                         bool bExpanded)
{
    return(FindNextItemWithState<wxVirtualItemSearchExpanded,
                                 wxVirtualItemSearchCollapsed>(this, pDataModel, rID, bExpanded));
}

//---------- INTERFACE WITH DEFAULT IMPLEMENTATION ------------------//


/** Set all states at once
  * \param rState   [input] : the state to set
  * \param rID      [input] : the ID of the item to modify
  * \param bRecurse [input] : apply the state on all the children as well
  * \return true if all states were set, false otherwise
  */
bool wxVirtualIStateModel::SetState(const wxVirtualDataViewItemState &rState,
                                    const wxVirtualItemID &rID,
                                    bool bRecurse)
{
    bool bResult = true;

    if (!Enable(rID, rState.IsEnabled(), bRecurse))  bResult = false;
    if (!Select(rID, rState.IsSelected(), bRecurse)) bResult = false;
    if (!Expand(rID, rState.IsExpanded(), bRecurse)) bResult = false;
    if (!SetCheck(rID, (ECheckState) rState.GetCheckState(), bRecurse)) bResult = false;
    if (rState.IsFocused())
    {
        if (!SetFocus(rID)) bResult = false;
    }
    else
    {
        if (!SetFocus(wxVirtualItemID())) bResult = false;
    }

    return(bResult);
}

/** Get all states at once
  * \param rState [output]: the state of the item
  * \param rID    [input] : the ID of the item to query
  */
void wxVirtualIStateModel::GetState(wxVirtualDataViewItemState &rState,
                                    const wxVirtualItemID &rID)
{
    rState.Select(IsSelected(rID));
    rState.SetFocused(HasFocus(rID));
    if (IsExpanded(rID)) rState.Expand(); else rState.Collapse();
    rState.Enable(IsEnabled(rID));
    rState.SetCheckState(GetCheck(rID));
}

//------------------ CALLBACKS --------------------------------------//
/** Data model has changed
  * \param pDataModel [input]: the new data model
  * The default implementation does nothing
  */
void wxVirtualIStateModel::OnDataModelChanged(wxVirtualIDataModel* pDataModel)
{

}

/** Delete an item
  * \param rID [input]: the item to delete
  * \return true if the item was deleted, false otherwise
  */
bool wxVirtualIStateModel::DeleteItem(const wxVirtualItemID &rID)
{
    return(true);
}

/** Enable / disable items
  * \param rID        [input]: the item to modify
  * \param bEnable    [input]: true for enabling the item, false for disabling it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::Enable(const wxVirtualItemID &rID, bool bEnable, bool bRecursive)
{
    return(false);
}

/** Select / deselect items
  * \param rID        [input]: the item to modify
  * \param bSelect    [input]: true for selecting the item, false for deselecting it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::Select(const wxVirtualItemID &rID, bool bSelect, bool bRecursive)
{
    return(false);
}

/** Select / deselect a range of items.
  * \param rID1 [input] : the 1st item in the range
  * \param rID2 [input] : the 2nd item in the range
  * \param bSelect [input]: true for selection, false for de-selecting
  * \return true if the state of the items have changed, false otherwise
  * Default implementation does use rID1.GetModel() and wxVirtualIDataModel::NextItem internally
  */
bool wxVirtualIStateModel::SelectRange(const wxVirtualItemID &rID1, const wxVirtualItemID &rID2, bool bSelect)
{
    wxVirtualIDataModel *pDataModel = rID1.GetTopModel();
    if (!pDataModel) return(false);

    wxVirtualItemID id = rID1;
    wxVirtualItemID idLast = pDataModel->NextItem(rID2, this, 1);
    while ((id != idLast) && (!id.IsInvalid()))
    {
        Select(id, bSelect, false);
        id = pDataModel->NextItem(id, this, 1);
    }

    return(true);
}

/** Select / deselect a list of items
  * \param vIDs [input]: the list of items to select / deselect
  * \param bSelect [input]: true for selection, false for de-selecting
  * \return true if the state was applied, false otherwise
  * Default implementation call Select on each item in the list
  */
bool wxVirtualIStateModel::SelectList(const wxVirtualItemIDs &vIDs, bool bSelect)
{
    size_t i, uiCount;
    uiCount = vIDs.size();
    for(i=0; i < uiCount; i++)
    {
        Select(vIDs[i], bSelect, false);
    }
    return(true);
}

/** Select all items
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::SelectAll(void)
{
    wxVirtualItemID rRootID;
    return(Select(rRootID, true, true));
}

/** Deselect all items
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::DeselectAll(void)
{
    wxVirtualItemID rRootID;
    return(Select(rRootID, false, true));
}

/** Expand / collapse items
  * \param rID        [input]: the item to modify
  * \param bExpand    [input]: true for expanding the item, false for collapsing it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::Expand(const wxVirtualItemID &rID, bool bExpand, bool bRecursive)
{
    return(false);
}

/** Set the check state of the item
  * \param rID        [input]: the item to modify
  * \param eCheck     [input]: the new check state
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::SetCheck(const wxVirtualItemID &rID, ECheckState eCheck,
                                    bool bRecursive)
{
    return(false);
}

/** Set focus on one item
  * \param rID [input]: the item to focus
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualIStateModel::SetFocus(const wxVirtualItemID &rID)
{
    return(false);
}

