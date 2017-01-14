/**********************************************************************/
/** FILE    : VirtualIProxyDataModel.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for sorting / filtering items         **/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualIProxyDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>

//we need to reimplement all the virtual methods and forward them to the shadowed data model
//This is necessary even for virtual methods with common implementation.
//If a data model reimplements some of them (for performance reasons generally),
//then using the stock wxVirtualIDataModel methods will lose the advantages of the reimplementation
//This is why most of this class is looking like that:
//
//  virtual ret wxVirtualIProxyDataModel::SomeMethod(params)
//  {
//      if (m_pDataModel) return(m_pDataModel->SomeMethod(params));
//      return(SomeDefaultValue);
//  }

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualIProxyDataModel::wxVirtualIProxyDataModel(void)
    : wxVirtualIDataModel(),
      m_pDataModel(WX_VDV_NULL_PTR)
{
}

/** Destructor
  */
wxVirtualIProxyDataModel::~wxVirtualIProxyDataModel(void)
{
    if (m_pDataModel) delete(m_pDataModel);
    m_pDataModel = WX_VDV_NULL_PTR;
}

//---------------------- MASTER MODEL -------------------------------//
/** Get the master model
  * \return the master model
  */
wxVirtualIDataModel* wxVirtualIProxyDataModel::GetMasterModel(void)
{
    return(m_pDataModel);
}

/** Attach master model and takes ownership on it
  * \param pDataModel [input]: the new data model to use. It must have been allocated with "NEW"
  *                            previous model is DETACHED, and not released.
  */
void wxVirtualIProxyDataModel::AttachMasterModel(wxVirtualIDataModel *pDataModel)
{
    m_pDataModel = pDataModel;
    if (pDataModel) pDataModel->SetParentModel(this);
}

/** Detach the master model without deleting it
  * \return the detached master model
  */
wxVirtualIDataModel* wxVirtualIProxyDataModel::DetachMasterModel(void)
{
    wxVirtualIDataModel *pDataModel = m_pDataModel;
    m_pDataModel = WX_VDV_NULL_PTR;
    if (pDataModel) pDataModel->SetParentModel(WX_VDV_NULL_PTR);
    return(pDataModel);
}

//---------------------- INTERFACE : CLASS TYPE ---------------------//
/** Get the model class flags
  * This is an alternative to RTTI and dynamic_cast
  * This allows to refuse some kind of models for specialization of wxVirtualDataViewCtrl
  * \return a set of flags EClassType
  */
int wxVirtualIProxyDataModel::GetModelClass(void) const
{
    return(WX_MODEL_CLASS_BASE | WX_MODEL_CLASS_PROXY);
}

//--------------------- INTERFACE: HIERARCHY ------------------------//
/** Get the parent item ID
  * \param rID [input]: the child item ID
  * \return the parent item ID
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetParent(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->GetParent(rID));
    return(CreateInvalidItemID());
}

/** Get the amount of children
  * \param rID [input]: the ID of the item to query
  * \return the amount of children of rID
  */
size_t wxVirtualIProxyDataModel::GetChildCount(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->GetChildCount(rID));
    return(0);
}

/** Get child item ID
  * \param rIDParent    [input]: the ID of parent item
  * \param uiChildIndex [input]: the index of the child to retrieve
  * \return the child item ID
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetChild(const wxVirtualItemID &rIDParent,
                                                   size_t uiChildIndex)
{
    if (m_pDataModel) return(m_pDataModel->GetChild(rIDParent, uiChildIndex));
    return(CreateInvalidItemID());
}

/** Get all children at once
  * \param vIDs      [output]: the list of children item. Previous content is erased
  * \param rIDParent [input] : the ID of the parent item
  * \return the amount of children in vIDs
  */
size_t wxVirtualIProxyDataModel::GetAllChildren(wxVirtualItemIDs &vIDs,
                                               const wxVirtualItemID &rIDParent)
{
    if (m_pDataModel) return(m_pDataModel->GetAllChildren(vIDs, rIDParent));
    vIDs.clear();
    return(0);
}

/** Get the amount of fields in the item
  * The meaning of a field is model dependent. For example, in a tree-list model, it can be the
  * the amount of columns.
  * \return the amount of sub-field in the item.
  *         If the value is size_t(-1), then the item will be drawn on all columns
  */
size_t wxVirtualIProxyDataModel::GetFieldCount(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->GetFieldCount(rID));
    return(0);
}

//-------------------- INTERFACE : ITEM DATA ------------------------//
/** Get the item data
  * \param rID   [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  */
wxVariant wxVirtualIProxyDataModel::GetItemData(const wxVirtualItemID &rID,
                                           size_t uiField, EDataType eType)
{
    if (m_pDataModel) return(m_pDataModel->GetItemData(rID, uiField, eType));
    return(wxVariant());
}

/** Get the item graphic attributes
  * \param rID [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param rState [input] : the state of the item
  * \return a pointer to the graphic attributes for this item. The data is owned by the model.
  *         returning a NULL Pointer is valid: the default attributes will be used
  */
wxVirtualDataViewItemAttr* wxVirtualIProxyDataModel::GetItemAttribute(const wxVirtualItemID &rID,
                                                                      size_t uiField,
                                                                      const wxVirtualDataViewItemState &rState)
{
    if (m_pDataModel) return(m_pDataModel->GetItemAttribute(rID, uiField, rState));
    return(WX_VDV_NULL_PTR);
}

/** Get the item flags
  * \param rID     [input] : the ID of the item to query
  * \param uiField [input] : the field of the item to query
  * \return the item flags.
  */
wxVirtualIProxyDataModel::EFlags wxVirtualIProxyDataModel::GetItemFlags(const wxVirtualItemID &rID, size_t uiField)
{
    if (m_pDataModel) return(m_pDataModel->GetItemFlags(rID, uiField));
    return(WX_ITEM_FLAGS_ALL);
}

/** Set the item data
  * \param rID     [input]: the ID of the item to modify
  * \param uiField [input]: the field of the data to modify
  * \param vValue  [input]: the new value
  * \param eType   [input]: the kind of value to modify
  * \return true if the data was modified, false otherwise
  */
bool wxVirtualIProxyDataModel::SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                           const wxVariant &vValue, EDataType eType)
{
    if (m_pDataModel) return(m_pDataModel->SetItemData(rID, uiField, vValue, eType));
    return(true);
}

//--------------------- INTERFACE WITH COMMON IMPLEMENTATION --------//
/** Get the root item
  * \return the root item
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetRootItem(void)
{
    if (m_pDataModel) return(m_pDataModel->GetRootItem());
    return(wxVirtualItemID(0, 0, 0, this));
}

/** Check if the item is the root item
  * \param rID [input]: the item to check
  * \return true if rID is the root item, false otherwise
  */
bool wxVirtualIProxyDataModel::IsRootItem(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsRootItem(rID));
    return(rID.IsRootItem());
}

/** Get the last item
  * \return the last item of the mode
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetLastItem(void)
{
    if (m_pDataModel) return(m_pDataModel->GetLastItem());
    return(wxVirtualItemID(0, 0, 0, this));
}

/** Return true if an item expander should be drawn for this item
  * \param rID [input]: the item to query
  * \return true if an expander should be drawn
  *         false otherwise
  *
  * The default implementation returns true when GetChildCount(rID) > 0
  */
bool wxVirtualIProxyDataModel::HasExpander(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->HasExpander(rID));
    if (GetChildCount(rID) > 0) return(true);
    return(false);
}

/** Return true if an item has a checkbox next to the expander
  * \param rID [input]: the item to query
  * \return true if a checkbox should be drawn
  *         false otherwise
  *
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_CHECKABLE
  *     WX_ITEM_FLAGS_HAS_TRISTATE
  */
bool wxVirtualIProxyDataModel::HasCheckbox(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->HasCheckbox(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return((eFlags & WX_ITEM_FLAGS_CHECKABLE) || (eFlags & WX_ITEM_FLAGS_HAS_TRISTATE));
}

/** Return true if an item is editable
  * \param rID [input]: the item to query
  * \return true if the item is editable, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_EDITABLE
  */
bool wxVirtualIProxyDataModel::IsEditable(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsEditable(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_EDITABLE);
}

/** Return true if an item is selectable
  * \param rID [input]: the item to query
  * \return true if the item is selectable, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_SELECTABLE
  */
bool wxVirtualIProxyDataModel::IsSelectable(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsSelectable(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_SELECTABLE);
}

/** Return true if an item is checkable
  * \param rID [input]: the item to query
  * \return true if the item is checkable, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_CHECKABLE
  *     WX_ITEM_FLAGS_HAS_TRISTATE
  */
bool wxVirtualIProxyDataModel::IsCheckable(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsCheckable(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return((eFlags & WX_ITEM_FLAGS_CHECKABLE) || (eFlags & WX_ITEM_FLAGS_HAS_TRISTATE));
}

/** Return true if an item is checkable with 3 state
  * \param rID [input]: the item to query
  * \return true if the item is checkable with 3 state, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_HAS_TRISTATE
  */
bool wxVirtualIProxyDataModel::IsCheckable3DState(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsCheckable3DState(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_HAS_TRISTATE);
}

/** Return true if an item is a drag source
  * \param rID [input]: the item to query
  * \return true if the item is a drag source, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_DRAGGABLE
  */
bool wxVirtualIProxyDataModel::IsDragSource(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsDragSource(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_DRAGGABLE);
}

/** Return true if an item is a drop target
  * \param rID [input]: the item to query
  * \return true if the item is a drop target, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_DROPPABLE
  */
bool wxVirtualIProxyDataModel::IsDropTarget(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->IsDropTarget(rID));
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_DROPPABLE);
}

/** Get the index of a child. Used by wxVirtualTreePath::Set
  * \param rChild  [input]: the child item
  * \return the index of rChild, if found
  *         size_t(-1) if not found
  */
size_t wxVirtualIProxyDataModel::GetChildIndex(const wxVirtualItemID &rChild)
{
    if (m_pDataModel) return(m_pDataModel->GetChildIndex(rChild));
    return(size_t(-1));
}

/** Get the index of a child. Used by GetNextItem internally
  * \param rParent [input]: the parent item
  * \param rChild  [input]: the child item
  * \return the index of rChild, if found
  *         size_t(-1) if not found
  */
size_t wxVirtualIProxyDataModel::GetChildIndex(const wxVirtualItemID &rParent,
                                               const wxVirtualItemID &rChild)
{
    if (m_pDataModel) return(m_pDataModel->GetChildIndex(rParent, rChild));
    return(size_t(-1));
}

/** Check if an item is the last child of a parent item
  * \param rChild [input]: the item to check
  * \return true if the item is the last child (GetChildIndex() == GetParent(rChild).GetChildCount() - 1)
  *         false otherwise
  */
bool wxVirtualIProxyDataModel::IsLastChild(const wxVirtualItemID &rChild)
{
    if (m_pDataModel) return(m_pDataModel->IsLastChild(rChild));
    return(false);
}

/** Get the depth (or level) of an item in the tree
  * Root item has depth == 0
  * This is also equals to the amount of ancestors nodes
  * \param rID [input] : the item to query
  * \return the depth of the item
  */
size_t wxVirtualIProxyDataModel::GetDepth(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->GetDepth(rID));
    return(0);
}

/** Compute the size of a sub-tree.
  * Used by FindItem (bottleneck of the FindItem method) -> this need to be efficient for
  *                                                         implementing fast scrolling on huge trees
  * \param rID [input]: the root node of sub-tree
  * \param pStateModel [input]: the state model.
  *                             If NULL, all sub-nodes are counted
  *                             If non-NULL, sub-nodes which are collapsed are counted as 1
  * \return the amount of nodes in the sub-tree, including the root node
  *         So it is always >= 1
  */
size_t wxVirtualIProxyDataModel::GetSubTreeSize(const wxVirtualItemID &rID,
                                                wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) return(m_pDataModel->GetSubTreeSize(rID, pStateModel));
    return(0);
}

/** Get next sibling
  * \param rID [input]: the item to query
  * \return the next sibling, if there is one
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetNextSibling(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->GetNextSibling(rID));
    return(CreateInvalidItemID());
}

/** Get previous sibling
  * \param rID [input]: the item to query
  * \return the next sibling, if there is one
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetPreviousSibling(const wxVirtualItemID &rID)
{
    if (m_pDataModel) return(m_pDataModel->GetPreviousSibling(rID));
    return(CreateInvalidItemID());
}

/** Get the left-most deepest descendent in the sub-tree
  * \param rID [input]: the root of the sub-tree
  * \param pStateModel [input]: the state model
  *                             If NULL, all items are examined
  *                             If non-null, children of collapsed node are not scanned
  * \return the Item ID of the left-most descendent
  *         This is useful for finding the next item
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetFirstDescendent(const wxVirtualItemID &rID,
                                                             wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) return(m_pDataModel->GetFirstDescendent(rID, pStateModel));
    return(rID);
}

/** Get the right-most deepest descendent in the sub-tree
  * \param rID [input]: the root of the sub-tree
  * \param pStateModel [input]: the state model
  *                             If NULL, all items are examined
  *                             If non-null, children of collapsed node are not scanned
  * \return the Item ID of the right-most descendent
  *         This is useful for finding the previous item
  */
wxVirtualItemID wxVirtualIProxyDataModel::GetLastDescendent(const wxVirtualItemID &rID,
                                                            wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) return(m_pDataModel->GetLastDescendent(rID, pStateModel));
    return(rID);
}

/** Find an item based on its rank
  * \param uiRank [input]     : the rank of the item. This is the depth first rank of the item
  *                             0 = root item
  *                             1 = 1st child of root item
  *                             2 = 1st child of item 1, or 2nd child of root item if 1 has no children
  *                             ...
  * \param pStateModel [input]: the state model to use. Children of collapsed items are skipped
  *                             if NULL, then all items are considered expanded
  * \param rStart      [input]: the item identifying the start point
  * \param uiStartRank [input]: the rank of the starting point
  * \return the item ID
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindItem(size_t uiRank,
                                                   wxVirtualIStateModel *pStateModel,
                                                   const wxVirtualItemID &rStart,
                                                   size_t uiStartRank)
{
    if (m_pDataModel) return(m_pDataModel->FindItem(uiRank, pStateModel, rStart, uiStartRank));
    return(CreateInvalidItemID());
}

/** Find the rank of an item
  * \param rID         [input]: the item to query
  * \param pStateModel [input]: the state model to use
  *                             if NULL, all items are considered to be expanded
  * \return the rank of the item. Root item has rank 0
  */
size_t wxVirtualIProxyDataModel::GetItemRank(const wxVirtualItemID &rID,
                                             wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) return(m_pDataModel->GetItemRank(rID, pStateModel));
    return(0);
}

/** Find the next item, in depth first order
  * The next item is defined as :
  *     - the 1st child, if there is one
  *     - the next brother, if there is one
  *     - the parent next brother, if there is one
  *     - ...
  * \param rID             [input] : the item to query
  * \param uiLevel         [input] : the level of the item to query
  * \param uiNextItemLevel [output]: the level of the next item
  * \param pStateModel     [input] : the state model
  *                                  If NULL, all items are examined
  *                                  If non-null, children of collapsed node are not scanned
  * \param uiNbItems       [input] : by how many items to go forward (default = 1)
  * \return the next item
  */
wxVirtualItemID wxVirtualIProxyDataModel::NextItem(const wxVirtualItemID &rID,
                                                   size_t uiLevel, size_t &uiNextItemLevel,
                                                   wxVirtualIStateModel *pStateModel,
                                                   size_t uiNbItems)
{
    if (m_pDataModel) return(m_pDataModel->NextItem(rID, uiLevel, uiNextItemLevel, pStateModel, uiNbItems));
    return(CreateInvalidItemID());
}

/** Find the next item, in depth first order
  * The next item is defined as :
  *     - the 1st child, if there is one
  *     - the next brother, if there is one
  *     - the parent next brother, if there is one
  *     - ...
  * \param rID             [input] : the item to query
  * \param pStateModel     [input] : the state model
  *                                  If NULL, all items are examined
  *                                  If non-null, children of collapsed node are not scanned
  * \param uiNbItems       [input] : by how many items to go forward (default = 1)
  * \return the next item
  */
wxVirtualItemID wxVirtualIProxyDataModel::NextItem(const wxVirtualItemID &rID,
                                                   wxVirtualIStateModel *pStateModel,
                                                   size_t uiNbItems)
{
    if (m_pDataModel) return(m_pDataModel->NextItem(rID, pStateModel, uiNbItems));
    size_t uiLevel = 1;
    return(NextItem(rID, uiLevel, uiLevel, pStateModel, uiNbItems));
}

/** Find the previous item, in depth first order
  * The previous item is defined as :
  *     1 the previous sibling, if there is one
  *     2 otherwise:
  *         -> go up in the hierarchy : get the parent idParent
  *         -> find the previous sibling of idParent : this is idSibling
  *         -> get the last node of the sub tree rooted by idSibling
  *         -> if idSibling is invalid, repeat step 2
  * \param rID             [input] : the item to query
  * \param uiLevel         [input] : the level of the item to query
  * \param uiPrevItemLevel [output]: the level of the previous item
  * \param pStateModel     [input] : the state model
  *                                  If NULL, all items are examined
  *                                  If non-null, children of collapsed node are not scanned
  * \param uiNbItems       [input] : by how many items to go backwards (default = 1)
  * \return the previous item
  */
wxVirtualItemID wxVirtualIProxyDataModel::PrevItem(const wxVirtualItemID &rID,
                                                   size_t uiLevel, size_t &uiPrevItemLevel,
                                                   wxVirtualIStateModel *pStateModel,
                                                   size_t uiNbItems)
{
    if (m_pDataModel) return(m_pDataModel->PrevItem(rID, uiLevel, uiPrevItemLevel, pStateModel, uiNbItems));
    return(CreateInvalidItemID());
}

/** Find the previous item, in depth first order
  * The previous item is defined as :
  *     1 the previous sibling, if there is one
  *     2 otherwise:
  *         -> go up in the hierarchy : get the parent idParent
  *         -> find the previous sibling of idParent : this is idSibling
  *         -> get the last node of the sub tree rooted by idSibling
  *         -> if idSibling is invalid, repeat step 2
  * \param rID             [input] : the item to query
  * \param pStateModel     [input] : the state model
  *                                  If NULL, all items are examined
  *                                  If non-null, children of collapsed node are not scanned
  * \param uiNbItems       [input] : by how many items to go backwards (default = 1)
  * \return the previous item
  */
wxVirtualItemID wxVirtualIProxyDataModel::PrevItem(const wxVirtualItemID &rID,
                                                   wxVirtualIStateModel *pStateModel,
                                                   size_t uiNbItems)
{
    if (m_pDataModel) return(m_pDataModel->PrevItem(rID, pStateModel, uiNbItems));
    size_t uiLevel = 1;
    return(PrevItem(rID, uiLevel, uiLevel, pStateModel, uiNbItems));
}

/** Check if rID is inside a sub-tree
  * \param rID [input]: the item ID to test
  * \param rSubTreeRootID [input]: the top node of the sub-tree to test
  * \return true if rID belongs to the sub-tree below rSubTreeRootID
  *         false otherwise
  */
bool wxVirtualIProxyDataModel::IsInSubTree(const wxVirtualItemID &rID,
                                           const wxVirtualItemID &rSubTreeRootID)
{
    if (m_pDataModel) return(m_pDataModel->IsInSubTree(rID, rSubTreeRootID));
    return(false);
}

//---------------------- SEARCHING ----------------------------------//
/** Main matching method for searching
  * \param rID            [input]: the item to examine
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param uiField        [input]: the field to examine. If equal to size_t(-1), then all fields must be examined
  * \return true if the item rID matches the criteria, false otherwise
  */
bool wxVirtualIProxyDataModel::IsMatching(wxVirtualItemID &rID, const wxVariant &rvValue,
                                          bool bFullMatch, bool bCaseSensitive, bool bRegex,
                                          size_t uiField)
{
    if (m_pDataModel) return(m_pDataModel->IsMatching(rID, rvValue, bFullMatch,
                                                      bCaseSensitive, bRegex, uiField));
    return(false);
}

/** Find the 1st item matching a criteria in 1 field
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param uiField        [input]: the field to search
  * \return the first item found matching the criteria
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindFirst(const wxVariant &rvValue,
                                                    bool bFullMatch,
                                                    bool bCaseSensitive,
                                                    bool bRegEx,
                                                    size_t uiField)
{
    return(FindNext(GetRootItem(), rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField));
}

/** Find the next item matching a criteria in 1 field
  * \param rID            [input]: the starting item (not included in search)
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param uiField        [input]: the field to search
  * \return the next item found matching the criteria
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindNext(const wxVirtualItemID &rID,
                                                   const wxVariant &rvValue,
                                                   bool bFullMatch,
                                                   bool bCaseSensitive,
                                                   bool bRegEx,
                                                   size_t uiField)
{

    if (m_pDataModel)
    {
        wxVirtualItemID id = NextItem(rID, WX_VDV_NULL_PTR, 1);
        while (id.IsOK())
        {
            if (m_pDataModel->IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField)) return(id);
            id = NextItem(id, WX_VDV_NULL_PTR, 1);
        }
    }
    return(CreateInvalidItemID());
}

/** Find the previous item matching a criteria in 1 field
  * \param rID            [input]: the starting item (not included in search)
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param uiField        [input]: the field to search
  * \return the previous item found matching the criteria
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindPrev(const wxVirtualItemID &rID,
                                                   const wxVariant &rvValue,
                                                   bool bFullMatch,
                                                   bool bCaseSensitive,
                                                   bool bRegEx,
                                                   size_t uiField)
{
    if (m_pDataModel)
    {
        wxVirtualItemID id = PrevItem(rID, WX_VDV_NULL_PTR, 1);
        while (id.IsOK())
        {
            if (m_pDataModel->IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField)) return(id);
            id = PrevItem(id, WX_VDV_NULL_PTR, 1);
        }
    }
    return(CreateInvalidItemID());
}

/** Find the 1st item matching a criteria in all fields
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \return the first item found matching the criteria
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindFirst(const wxVariant &rvValue,
                                                    bool bFullMatch,
                                                    bool bCaseSensitive,
                                                    bool bRegEx)
{
    return(FindNext(GetRootItem(), rvValue, bFullMatch, bCaseSensitive, bRegEx));
}

/** Find the next item matching a criteria in all fields
  * \param rID            [input]: the starting item (not included in search)
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \return the next item found matching the criteria
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindNext(const wxVirtualItemID &rID,
                                                   const wxVariant &rvValue,
                                                   bool bFullMatch,
                                                   bool bCaseSensitive,
                                                   bool bRegEx)
{
    if (m_pDataModel)
    {
        wxVirtualItemID id = NextItem(rID, WX_VDV_NULL_PTR, 1);
        while (id.IsOK())
        {
            if (m_pDataModel->IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx)) return(id);
            id = NextItem(id, WX_VDV_NULL_PTR, 1);
        }
    }
    return(CreateInvalidItemID());
}

/** Find the previous item matching a criteria in 1 field
  * \param rID            [input]: the starting item (not included in search)
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \return the previous item found matching the criteria
  */
wxVirtualItemID wxVirtualIProxyDataModel::FindPrev(const wxVirtualItemID &rID,
                                                   const wxVariant &rvValue,
                                                   bool bFullMatch,
                                                   bool bCaseSensitive,
                                                   bool bRegEx)
{
    if (m_pDataModel)
    {
        wxVirtualItemID id = PrevItem(rID, WX_VDV_NULL_PTR, 1);
        while (id.IsOK())
        {
            if (m_pDataModel->IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx)) return(id);
            id = PrevItem(id, WX_VDV_NULL_PTR, 1);
        }
    }
    return(CreateInvalidItemID());
}

//------------------------- SORTING ---------------------------------//
/** Check if the field can be sorted by the data model
  * \param vSortFilters [input]: the sort filters to apply
  * \return true if the model can sort the items according to the filters, false otherwise
  *         Note that true is returned only if ALL THE SORT FILTERS can be taken into account
  */
bool wxVirtualIProxyDataModel::CanSort(const TSortFilters &vSortFilters) const
{
    if (m_pDataModel) return(m_pDataModel->CanSort(vSortFilters));
    return(false);
}

/** Sort the values
  * \param vSortFilters [input]: the sort filters to apply
  */
void wxVirtualIProxyDataModel::Sort(const TSortFilters &vSortFilters)
{
    if (m_pDataModel) m_pDataModel->Sort(vSortFilters);
}

/** Remove all sort filters
  */
void wxVirtualIProxyDataModel::ResetSorting(void)
{
    if (m_pDataModel) m_pDataModel->ResetSorting();
}

//------------------------- SORTING COMPARISON ----------------------//
/** Compare 2 items
  * \param rID1       [input]: the 1st item to compare
  * \param rID2       [input]: the 2nd item to compare
  * \param uiField    [input]: the field to compare
  * \return WX_E_LESS_THAN    if rID1 < rID2
  *         WX_E_EQUAL        if rID1 == rID2
  *         WX_E_GREATER_THAN if rID1 > rID2
  */
wxVirtualIDataModel::ECompareResult wxVirtualIProxyDataModel::Compare(const wxVirtualItemID &rID1,
                                                                      const wxVirtualItemID &rID2,
                                                                      size_t uiField)
{
    if (m_pDataModel) return(m_pDataModel->Compare(rID1, rID2, uiField));
    return(WX_E_EQUAL);
}

/** Compare 2 items values
  * \param rID1       [input]: the 1st item to compare
  * \param rValue1    [input]: the value of the 1st item to compare
  * \param rID2       [input]: the 2nd item to compare
  * \param rValue2    [input]: the value of the 2nd item to compare
  * \param uiField    [input]: the field to compare
  * \return WX_E_LESS_THAN    if rID1 < rID2
  *         WX_E_EQUAL        if rID1 == rID2
  *         WX_E_GREATER_THAN if rID1 > rID2
  */
wxVirtualIDataModel::ECompareResult wxVirtualIProxyDataModel::Compare(const wxVirtualItemID &rID1,
                                                                      const wxVariant &rValue1,
                                                                      const wxVirtualItemID &rID2,
                                                                      const wxVariant &rValue2,
                                                                      size_t uiField)
{
    if (m_pDataModel) return(m_pDataModel->Compare(rID1, rValue1, rID2, rValue2, uiField));
    return(WX_E_EQUAL);
}

//------- FILTERING - GET ALL VALUES AS STRINGS ---------------------//
/** Get all the values of a field
  * \param rvStrings [output]: an array of string. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayString &rvStrings, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) m_pDataModel->GetAllValues(rvStrings, uiField, pStateModel);
    else              rvStrings.Clear();
}

//------- FILTERING - GET ALL VALUES AS BOOL ------------------------//
/** Get all the values of a field
  * \param rvBools [output]: an array of bool. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayBool &rvBools, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) m_pDataModel->GetAllValues(rvBools, uiField, pStateModel);
    else              rvBools.Clear();
}

//------- FILTERING - GET ALL VALUES AS SIGNED INTEGERS -------------//
/** Get all the values of a field
  * \param rvShorts [output]: an array of shorts. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayShort &rvShorts, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvShorts, uiField, pStateModel);
    else              rvShorts.Clear();
}

/** Get all the values of a field
  * \param rvInts [output]: an array of int. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayInt &rvInts, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvInts, uiField, pStateModel);
    else              rvInts.Clear();
}

/** Get all the values of a field
  * \param rvLongs [output]: an array of long. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayLong &rvLongs, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvLongs, uiField, pStateModel);
    else              rvLongs.Clear();
}

/** Get all the values of a field
  * \param rvLongLongs [output]: an array of wxLongLong. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayLongLong &rvLongLongs, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvLongLongs, uiField, pStateModel);
    else              rvLongLongs.Clear();
}

//------- FILTERING - GET ALL VALUES AS UNSIGNED INTEGERS -----------//
/** Get all the values of a field
  * \param rvUShorts [output]: an array of unsigned shorts. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayUShort &rvUShorts, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvUShorts, uiField, pStateModel);
    else              rvUShorts.Clear();
}

/** Get all the values of a field
  * \param rvUInts [output]: an array of unsigned int. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayUInt &rvUInts, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvUInts, uiField, pStateModel);
    else              rvUInts.Clear();
}

/** Get all the values of a field
  * \param rvULongs [output]: an array of unsigned long. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayULong &rvULongs, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvULongs, uiField, pStateModel);
    else              rvULongs.Clear();
}

/** Get all the values of a field
  * \param rvULongLongs [output]: an array of wxULongLong. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayULongLong &rvULongLongs, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvULongLongs, uiField, pStateModel);
    else              rvULongLongs.Clear();
}

//------- FILTERING - GET ALL VALUES AS FLOAT / DOUBLE --------------//
/** Get all the values inside an array of floats
  * \param rvFloats [output]: an array of floats. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayFloat &rvFloats, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{
    if (m_pDataModel) m_pDataModel->GetAllValues(rvFloats, uiField, pStateModel);
    else              rvFloats.Clear();
}

/** Get all the values inside an array of doubles
  * \param rvDoubles [output]: an array of doubles. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxArrayDouble &rvDoubles, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvDoubles, uiField, pStateModel);
    else              rvDoubles.Clear();
}

//------- FILTERING - GET ALL VALUES AS VARIANT ---------------------//
/** Get all the values inside an array of variants
  * \param rvVariants [output]: an array of variants. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualIProxyDataModel::GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                            wxVirtualIStateModel *pStateModel)
{

    if (m_pDataModel) m_pDataModel->GetAllValues(rvVariants, uiField, pStateModel);
    else              rvVariants.clear();
}


//------------------------ PROXY DATA MODEL -------------------------//
/** Return true if the model is a proxy data model
  * \return true for proxy model, false for base model
  */
bool wxVirtualIProxyDataModel::IsProxyDataModel(void) const
{
    return(true);
}

