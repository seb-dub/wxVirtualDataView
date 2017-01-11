/**********************************************************************/
/** FILE    : VirtualDefaultStateModel.cpp                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : default state model                                    **/
/**********************************************************************/

#include <wx/VirtualDataView/StateModels/VirtualDefaultStateModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/stack.h>

//store state on 1 int (actually 5 bits per item are enough)
//default state : enabled, unselected, unchecked, collapsed : value = 0
//enabled : bit 0 unset disabled : bit 0 set
//selected : bit 1 set
//expanded : bit 2 set
//check state : bits 3 & 4

//macro
#define WX_IS_DISABLED(iValue)              (iValue & 0x01)
#define WX_IS_ENABLED(iValue)               ((iValue & 0x01) == 0)
#define WX_IS_SELECTED(iValue)              (iValue & 0x02)
#define WX_IS_EXPANDED(iValue)              (iValue & 0x04)
#define WX_GET_CHECKSTATE(iValue)           (wxVirtualIStateModel::ECheckState) ((iValue >> 3) & 0x03)

#define WX_ENABLE(iValue)                   iValue &= ~0x01
#define WX_DISABLE(iValue)                  iValue |= 0x01
#define WX_SELECT(iValue)                   iValue |= 0x02
#define WX_DESELECT(iValue)                 iValue &= ~0x02
#define WX_EXPAND(iValue)                   iValue |= 0x04
#define WX_COLLAPSE(iValue)                 iValue &= ~0x04
#define WX_SET_CHECKSTATE(iValue, iState)   iValue &= ~0x18; iValue |= ((iState << 3) & 0x18);

//--------------- HELPERS FOR TREE TRAVERSAL ------------------------//
/** Traverse the tree, and perform the job without recursion
  * A stack is used internally
  * \param rID     [input]: the 1st node to process
  * \param j       [input]: a functor which will perform the job.
  *                         It must implement bool operator()(wxVirtualItemID &rID)
  * \return true if the job was performed successfully on all items
  *         false if at least one job failed
  */
template<typename TJob>
bool TraverseTreeNoRecurse(const wxVirtualItemID &rID, TJob j)
{
    //check & init
    wxVirtualIDataModel *pModel = rID.GetTopModel();
    if (!pModel) return(false);

    //traversal (without recursion)
    size_t uiChild, uiNbChildren;
    wxStack<wxVirtualItemID> StackOfNodes;
    StackOfNodes.push(rID);
    while(StackOfNodes.size() > 0)
    {
        //do the work
        wxVirtualItemID id = StackOfNodes.top();
        StackOfNodes.pop();
        if (!j(id)) return(false);

        //recurse
        uiNbChildren = pModel->GetChildCount(id);
        for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
        {
            wxVirtualItemID idChild = pModel->GetChild(id, uiChild);
            if (!idChild.IsInvalid()) StackOfNodes.push(idChild);
        }
    }

    return(true);
}

/** Traverse the tree, and perform the job with recursion
  * \param rID     [input]: the 1st node to process
  * \param pModel  [input]: the data model
  * \param j       [input]: a functor which will perform the job.
  *                         It must implement bool operator()(wxVirtualItemID &rID)
  * \return true if the job was performed successfully on all items
  *         false if at least one job failed
  */
template<typename TJob>
bool DoTraverseTreeRecurse(const wxVirtualItemID &rID, wxVirtualIDataModel *pModel, TJob j)
{
    //perform the job on this item
    if (rID.IsInvalid()) return(false);
    bool bResult = j(rID);

    //recurse on children
    size_t uiChild;
    size_t uiNbChildren = pModel->GetChildCount(rID);
    for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
    {
        wxVirtualItemID idChild = pModel->GetChild(rID, uiChild);
        if (!DoTraverseTreeRecurse(idChild, pModel, j)) bResult = false;
    }

    return(bResult);
}

/** Traverse the tree, and perform the job with recursion
  * \param rID     [input]: the 1st node to process
  * \param j       [input]: a functor which will perform the job.
  *                         It must implement bool operator()(wxVirtualItemID &rID)
  * \return true if the job was performed successfully on all items
  *         false if at least one job failed
  */
template<typename TJob>
bool TraverseTreeRecurse(const wxVirtualItemID &rID, TJob j)
{
    //check & init
    wxVirtualIDataModel *pModel = rID.GetTopModel();
    if (!pModel) return(false);

    //traversal (with recursion)
    return(DoTraverseTreeRecurse<TJob>(rID, pModel, j));
}

//-------------------- JOB DEFINITIONS ------------------------------//
//base class for job : provides helper for accessing the hash map
class JobBase
{
    public:
        JobBase(wxVirtualDefaultStateModel &rStates) : m_States(rStates)
        {}

        void SetInt(const wxVirtualItemID &rID, int iValue) const
        {
            m_States.SetState(rID, iValue);
        }

        int GetInt(const wxVirtualItemID &rID) const
        {
            return(m_States.GetState(rID));
        }

    protected:
        wxVirtualDefaultStateModel&  m_States;
};

//Job : enable the items
class JobEnable : public JobBase
{
    public:
        JobEnable(wxVirtualDefaultStateModel &rStates) : JobBase(rStates)
        {}
        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_ENABLE(iState);
            SetInt(rID, iState);
            return(true);
        }
};

//Job : disable the items
class JobDisable : public JobBase
{
    public:
        JobDisable(wxVirtualDefaultStateModel &rStates) : JobBase(rStates)
        {}
        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_DISABLE(iState);
            SetInt(rID, iState);
            return(true);
        }
};

//Job : select the items
class JobSelect : public JobBase
{
    public:
        JobSelect(wxVirtualDefaultStateModel &rStates) : JobBase(rStates)
        {}
        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_SELECT(iState);
            SetInt(rID, iState);
            return(true);
        }
};

//Job : deselect the items
class JobDeselect : public JobBase
{
    public:
        JobDeselect(wxVirtualDefaultStateModel &rStates) : JobBase(rStates)
        {}
        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_DESELECT(iState);
            SetInt(rID, iState);
            return(true);
        }
};

//Job : expand the items
class JobExpand : public JobBase
{
    public:
        JobExpand(wxVirtualDefaultStateModel &rStates) : JobBase(rStates)
        {}
        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_EXPAND(iState);
            SetInt(rID, iState);
            return(true);
        }
};

//Job : collapse the items
class JobCollapse : public JobBase
{
    public:
        JobCollapse(wxVirtualDefaultStateModel &rStates) : JobBase(rStates)
        {}
        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_COLLAPSE(iState);
            SetInt(rID, iState);
            return(true);
        }
};

//Job : set check state
class JobSetCheckState : public JobBase
{
    public:
        JobSetCheckState(wxVirtualDefaultStateModel &rStates,
                         int iCheckState)
            : JobBase(rStates), m_iCheckState(iCheckState)
        {}

        bool operator()(const wxVirtualItemID &rID)
        {
            int iState = GetInt(rID);
            WX_SET_CHECKSTATE(iState, m_iCheckState);
            SetInt(rID, iState);
            return(true);
        }

    protected:
        int m_iCheckState;
};

//Job : delete an item
class JobDeleteItem : public JobBase
{
    public:
        JobDeleteItem(wxVirtualDefaultStateModel &rStates)
            : JobBase(rStates)
        {}

        bool operator()(const wxVirtualItemID &rID)
        {
            SetInt(rID, 0);
            return(true);
        }
};

//----------------------- CONSTRUCTORS & DESTRUCTOR -----------------//
/** Default constructor
  */
wxVirtualDefaultStateModel::wxVirtualDefaultStateModel(void)
    : wxVirtualIStateModel(), m_bAllowRecursion(true)
{
    m_eSelectionType = WX_STATE_DEFAULT;
}

/** Destructor
  */
wxVirtualDefaultStateModel::~wxVirtualDefaultStateModel(void)
{

}

//-------------------- INTERNAL METHODS -----------------------------//
/** Get the state
  * \param rID [input]: the item to query
  * \return the state of the item
  */
WX_VDV_INLINE int wxVirtualDefaultStateModel::GetState(const wxVirtualItemID &rID) const
{
    THashMapState::const_iterator it = m_MapOfStates.find(rID);

    int iState = 0;
    if (it == m_MapOfStates.end())
    {
        //return 0 for all valid items (collapsed, enabled, un-selected
        //for root item, return expanded
        if (rID.IsRoot()) WX_EXPAND(iState);
    }
    else
    {
        iState = it->second;
    }

    switch(m_eSelectionType)
    {
        case WX_STATE_DEFAULT   : break;
        case WX_STATE_ALL       : WX_SELECT(iState); break;
        case WX_STATE_NONE      : WX_DESELECT(iState); break;
        default                 : break;
    }

    return(iState);
}

/** Set the state
  * \param rID    [input]: the item to modify
  * \param iState [input]: the new state
  */
WX_VDV_INLINE void wxVirtualDefaultStateModel::SetState(const wxVirtualItemID &rID, int iState)
{
    if (iState == 0)
    {
        m_MapOfStates.erase(rID);
        return;
    }

    m_MapOfStates[rID] = iState;
}

//---------------------- PARAMETERS ---------------------------------//
/** Check if algorithms are allowed to run recursively
  * \return true if the algorithms can run recursively. Easier to code, but risk of stack overflow
  *         false otherwise : in this case, a stack is used to simulate recursive function calls
  */
bool wxVirtualDefaultStateModel::AllowRecursion(void) const
{
    return(m_bAllowRecursion);
}

/** Allow/forbid algorithms to run recursively
  * \param bAllow [input]: true for allowing recursion
  *                        false otherwise
  */
void wxVirtualDefaultStateModel::AllowRecursion(bool bAllow)
{
    m_bAllowRecursion = bAllow;
}

//------ wxVirtualIStateModeL INTERFACE IMPLEMENTATION --------------//
/** Check if the item is enabled / disabled
  * \param rID [input]: the item to check
  * \return always true
  */
bool wxVirtualDefaultStateModel::IsEnabled(const wxVirtualItemID &rID)
{
    if (rID.IsRoot()) return(true); //root always enabled

    int iState = GetState(rID);
    return(WX_IS_ENABLED(iState));
}

/** Check if the item is selected / deselected
  * \param rID [input]: the item to check
  * \return true if the item is selected, false if it is deselected
  */
bool wxVirtualDefaultStateModel::IsSelected(const wxVirtualItemID &rID)
{
    if (rID.IsRoot()) return(false); //root always deselected

    int iState = GetState(rID);
    return(WX_IS_SELECTED(iState));
}

/** Check if the item has the focus
  * \param rID [input]: the item to check
  * \return true if the item has the focus, false otherwise
  */
bool wxVirtualDefaultStateModel::HasFocus(const wxVirtualItemID &rID)
{
    if (rID.IsRoot()) return(false); //no focus on root item

    if (rID == m_FocusedItem) return(true);
    return(false);
}

/** Check if the item is expanded / collapsed
  * \param rID [input]: the item to check
  * \return true if the item has the focus, false otherwise
  */
bool wxVirtualDefaultStateModel::IsExpanded(const wxVirtualItemID &rID)
{
    if (rID.IsRoot()) return(true); //root item always expanded

    int iState = GetState(rID);
    return(WX_IS_EXPANDED(iState));
}

/** Get the check state of the item
  * \param rID [input]: the item to query
  * \return the check state of the item
  */
wxVirtualIStateModel::ECheckState wxVirtualDefaultStateModel::GetCheck(const wxVirtualItemID &rID)
{
    int iState = GetState(rID);
    return(WX_GET_CHECKSTATE(iState));
}

/** Get the focused item
  * \return the ID of the focused item
  */
wxVirtualItemID wxVirtualDefaultStateModel::GetFocusedItem(void)
{
    return(m_FocusedItem);
}

/** Get the list of selected items
  * \param vIDs [output]: the list of selected items. The previous content is erased
  * \return the amount of currently selected items
  */
size_t wxVirtualDefaultStateModel::GetSelectedItems(wxVirtualItemIDs &vIDs)
{
    vIDs.clear();
    vIDs.reserve(m_MapOfStates.size());

    int iState;
    THashMapState::iterator it    = m_MapOfStates.begin();
    THashMapState::iterator itEnd = m_MapOfStates.end();
    while(it != itEnd)
    {
        iState = it->second;
        if (WX_IS_SELECTED(iState)) vIDs.push_back(it->first);
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
bool wxVirtualDefaultStateModel::SetState(const wxVirtualDataViewItemState &rState,
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
void wxVirtualDefaultStateModel::GetState(wxVirtualDataViewItemState &rState,
                                          const wxVirtualItemID &rID)
{
    int iState = GetState(rID);

    rState.Enable(WX_IS_ENABLED(iState));
    rState.Select(WX_IS_SELECTED(iState));
    rState.SetCheckState(WX_GET_CHECKSTATE(iState));
    if (WX_IS_EXPANDED(iState)) rState.Expand(); else rState.Collapse();
    if (m_FocusedItem == rID) rState.SetFocused(true);
    else                      rState.SetFocused(false);
}

//------------------ CALLBACKS --------------------------------------//
/** Delete an item
  * \param rID [input]: the item to delete
  * \return true if the item was deleted, false otherwise
  */
bool wxVirtualDefaultStateModel::DeleteItem(const wxVirtualItemID &rID)
{
    JobDeleteItem j(*this);
    if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
    return(TraverseTreeNoRecurse(rID, j));
}


/** Enable / disable items
  * \param rID        [input]: the item to modify
  * \param bEnable    [input]: true for enabling the item, false for disabling it
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualDefaultStateModel::Enable(const wxVirtualItemID &rID, bool bEnable, bool bRecursive)
{
    if (bRecursive)
    {
        if (bEnable)
        {
            JobEnable j(*this);
            if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
            return(TraverseTreeNoRecurse(rID, j));
        }
        else
        {
            JobDisable j(*this);
            if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
            return(TraverseTreeNoRecurse(rID, j));
        }
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
bool wxVirtualDefaultStateModel::Select(const wxVirtualItemID &rID, bool bSelect, bool bRecursive)
{
    if (bRecursive)
    {
        if (bSelect)
        {
            JobSelect j(*this);
            if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
            return(TraverseTreeNoRecurse(rID, j));
        }
        else
        {
            JobDeselect j(*this);
            if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
            return(TraverseTreeNoRecurse(rID, j));
        }
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
bool wxVirtualDefaultStateModel::SelectRange(const wxVirtualItemID &rID1,
                                             const wxVirtualItemID &rID2,
                                             bool bSelect)
{
    wxVirtualIDataModel *pDataModel = rID1.GetTopModel();
    if (!pDataModel) return(false);

    size_t uiLevel = 1;
    wxVirtualItemID id = rID1;
    wxVirtualItemID idLast = pDataModel->NextItem(rID2, uiLevel, uiLevel, this, 1);
    do
    {
        int iState = GetState(id);
        if (bSelect) WX_SELECT(iState); else WX_DESELECT(iState);
        SetState(id, iState);
        id = pDataModel->NextItem(id, uiLevel, uiLevel, this, 1);
    }
    while(id != idLast);

    return(true);
}

/** Select / deselect a list of items
  * \param vIDs [input]: the list of items to select / deselect
  * \param bSelect [input]: true for selection, false for de-selecting
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualDefaultStateModel::SelectList(const wxVirtualItemIDs &vIDs, bool bSelect)
{
    size_t i, uiCount;
    uiCount = vIDs.size();
    for(i=0;i<uiCount;i++)
    {
        int iState = GetState(vIDs[i]);
        if (bSelect) WX_SELECT(iState); else WX_DESELECT(iState);
        SetState(vIDs[i], i);
    }
    return(true);
}

/** Select all items
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualDefaultStateModel::SelectAll(void)
{
    m_eSelectionType = WX_STATE_ALL;
    return(true);
}

/** Deselect all items
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualDefaultStateModel::DeselectAll(void)
{
    m_eSelectionType = WX_STATE_DEFAULT;
    THashMapState::iterator it      = m_MapOfStates.begin();
    THashMapState::iterator itEnd   = m_MapOfStates.end();
    while(it != itEnd)
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
bool wxVirtualDefaultStateModel::Expand(const wxVirtualItemID &rID, bool bExpand, bool bRecursive)
{
    if (bRecursive)
    {
        if (bExpand)
        {
            JobExpand j(*this);
            if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
            return(TraverseTreeNoRecurse(rID, j));
        }
        else
        {
            JobCollapse j(*this);
            if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
            return(TraverseTreeNoRecurse(rID, j));
        }
    }

    int iState = GetState(rID);
    if (bExpand) WX_EXPAND(iState); else WX_COLLAPSE(iState);
    SetState(rID, iState);
    return(true);
}

/** Set the check state of the item
  * \param rID        [input]: the item to modify
  * \param eCheck     [input]: the new check state
  * \param bRecursive [input]: apply the state on all the children as well
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualDefaultStateModel::SetCheck(const wxVirtualItemID &rID, ECheckState eCheck,
                                          bool bRecursive)
{
    if (bRecursive)
    {
        JobSetCheckState j(*this, eCheck);
        if (m_bAllowRecursion) return(TraverseTreeRecurse(rID, j));
        return(TraverseTreeNoRecurse(rID, j));
    }

    int iState = GetState(rID);
    WX_SET_CHECKSTATE(iState, eCheck);
    SetState(rID, iState);
    return(true);
}

/** Set focus on one item
  * \param rID [input]: the item to focus
  * \return true if the state was applied, false otherwise
  */
bool wxVirtualDefaultStateModel::SetFocus(const wxVirtualItemID &rID)
{
    m_FocusedItem = rID;
    return(true);
}

