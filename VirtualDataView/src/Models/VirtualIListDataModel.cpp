/**********************************************************************/
/** FILE    : VirtualIListDataModel.cpp                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a list data model                         **/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualIListDataModel.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualIListDataModel::wxVirtualIListDataModel(void)
    : wxVirtualIDataModel()
{
}

/** Destructor
  */
wxVirtualIListDataModel::~wxVirtualIListDataModel(void)
{
}

//--------------------- ITEM ID METHODS -----------------------------//
/** Compute an item ID from a row index
  * \param uiRowIndex [input]: the row index of the item
  * \return an item ID for this item
  */
WX_VDV_INLINE wxVirtualItemID wxVirtualIListDataModel::DoGetItemID(size_t uiRowIndex) const
{
    return(GetItemID(uiRowIndex));
}

/** Compute the row index from the item ID
  * \param rID [input]: the item ID
  * \return the row index
  */
WX_VDV_INLINE size_t wxVirtualIListDataModel::DoGetRowIndex(const wxVirtualItemID &rID) const
{
    return(GetRowIndex(rID));
}

/** Compute an item ID from a row index
  * \param uiRowIndex [input]: the row index of the item
  * \return an item ID for this item
  */
wxVirtualItemID wxVirtualIListDataModel::GetItemID(size_t uiRowIndex) const
{
    wxVirtualItemID id(uiRowIndex + 1, 0, 0, const_cast<wxVirtualIListDataModel*>(this));
    id.SetChildIndex(uiRowIndex);
    return(id);
}

/** Compute the row index from the item ID
  * \param rID [input]: the item ID
  * \return the row index
  */
size_t wxVirtualIListDataModel::GetRowIndex(const wxVirtualItemID &rID)const
{
    return(rID.GetInt64() - 1);
}

//---------------------- INTERFACE : CLASS TYPE ---------------------//
/** Get the model class flags
  * This is an alternative to RTTI and dynamic_cast
  * This allows to refuse some kind of models for specialization of wxVirtualDataViewCtrl
  * \return a set of flags EClassType
  */
int wxVirtualIListDataModel::GetModelClass(void) const
{
    return(WX_MODEL_CLASS_BASE | WX_MODEL_CLASS_LIST);
}

//---------------- NEW INTERFACE : ITEM DATA ------------------------//
/** Get the item data
  * \param uiItemID [input]: row index of the item
  * \param uiField [input]: the field index of the item to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  *
  * Default implementation return wxVariant().
  * Reimplementation is strongly encouraged
  */
wxVariant wxVirtualIListDataModel::GetListItemData(size_t uiItemID, size_t uiField, EDataType eType)
{
    return(wxVariant());
}


/** Get the item graphic attributes
  * \param uiItemID [input]: row index of the item
  * \param uiField  [input]: field index of the sub-item
  * \param rState   [input]: current state of the item
  * \return a pointer to the graphic attributes to apply
  *
  * Default implementation return NULL
  * Reimplementation recommended for visually appealing controls
  */
wxVirtualDataViewItemAttr* wxVirtualIListDataModel::GetListItemAttribute(size_t uiItemID, size_t uiField, const wxVirtualDataViewItemState &rState)
{
    return(WX_VDV_NULL_PTR);
}

/** Get the item flags
  * \param uiItemID [input]: row index of the item
  * \param uiField  [input]: field index of the sub-item
  * \return the flags of the item
  *
  * Default implementation returns always WX_ITEM_FLAGS_ALL.
  * Reimplementation recommended for complex usage scenario
  */
wxVirtualIDataModel::EFlags wxVirtualIListDataModel::GetListItemFlags(size_t uiItemID, size_t uiField)
{
    return(WX_ITEM_FLAGS_ALL);
}

/** Set the item data
  * \param uiItemID [input]: row index of the item
  * \param uiField  [input]: field index of the sub-item
  * \param vValue  [input]: the new value
  * \param eType   [input]: the kind of value to modify
  * \return true if the data was modified, false otherwise
  *
  * Default implementation does nothing and returns true.
  * Reimplement for read/write models
  */
bool wxVirtualIListDataModel::SetListItemData(size_t uiItemID, size_t uiField, const wxVariant &vValue, EDataType eType)
{
    return(true);
}

//---- WXVIRTUALIDATAMODEL CORE INTERFACE IMPLEMENTATION : HIERARCHY -//
/** Get the parent item ID
  * \param rID [input]: the item identifier
  * \return the parent of the item.
  * For list model, it always return the root item, or invalid item for root
  */
wxVirtualItemID wxVirtualIListDataModel::GetParent(const wxVirtualItemID &rID)
{
    if (rID.IsOK()) return(GetRootItem());
    return(CreateInvalidItemID());
}

/** Get the amount of children
  * \param rID [input]: the item identifier
  * \return the amount of children for this item
  *
  * For list models, only the root item has children
  */
size_t wxVirtualIListDataModel::GetChildCount(const wxVirtualItemID &rID)
{
    if (!rID.IsRootItem()) return(0);
    return(GetItemCount());
}

/** Get child item ID
  * \param rIDParent [input]: the ID of the parent item
  * \param uiChildIndex [input]: index of the child item to get
  * \return the ID of the child item
  *
  * For list models, only the root item has children
  */
wxVirtualItemID wxVirtualIListDataModel::GetChild(const wxVirtualItemID &rIDParent, size_t uiChildIndex)
{
    if (!rIDParent.IsRootItem()) return(CreateInvalidItemID());
    if (uiChildIndex >= GetItemCount()) return(CreateInvalidItemID());

    return(DoGetItemID(uiChildIndex));
}

/** Get all children at once
  * \param vIDs [output]: the list of children items. Previous content is lost
  * \param rIDParent [input]: the ID of the parent item
  * \return the amount of children item
  */
size_t wxVirtualIListDataModel::GetAllChildren(wxVirtualItemIDs &vIDs, const wxVirtualItemID &rIDParent)
{
    vIDs.clear();

    if (!rIDParent.IsRootItem()) return(0);

    size_t uiCount = GetItemCount();
    vIDs.reserve(uiCount);

    size_t uiChildIndex;
    for(uiChildIndex = 0; uiChildIndex < uiCount; uiChildIndex++)
    {
        vIDs.push_back(DoGetItemID(uiChildIndex));
    }
    return(uiCount);
}

/** Get the amount of fields in the item
  * The meaning of a field is model dependent. For example, in a tree-list model, it can be the
  * the amount of columns.
  * \return the amount of sub-field in the item.
  *         If the value is size_t(-1), then the item will be drawn on all columns
  * The default implementation returns 1
  *
  * Reimplementation is strongly encouraged
  */
size_t wxVirtualIListDataModel::GetFieldCount(const wxVirtualItemID &rID)
{
    return(1);
}

//--- WXVIRTUALIDATAMODEL CORE INTERFACE IMPLEMENTATION: ITEM DATA --//
/** Get the item data
  * \param rID   [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  *
  * Default implementation return wxVariant().
  * Reimplementation is strongly encouraged
  */
wxVariant wxVirtualIListDataModel::GetItemData(const wxVirtualItemID &rID, size_t uiField, EDataType eType)
{
    if (!rID.IsOK()) return(wxVariant());
    return(GetListItemData(DoGetRowIndex(rID), uiField, eType));
}

/** Get the item graphic attributes
  * \param rID [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param rState [input] : the state of the item
  * \return a pointer to the graphic attributes for this item. The data is owned by the model.
  *         returning a NULL Pointer is valid: the default attributes will be used
  *
  * Default implementation return NULL
  * Reimplementation recommended for visually appealing controls
  */
wxVirtualDataViewItemAttr* wxVirtualIListDataModel::GetItemAttribute(const wxVirtualItemID &rID, size_t uiField, const wxVirtualDataViewItemState &rState)
{
    if (!rID.IsOK()) return(WX_VDV_NULL_PTR);
    return(GetListItemAttribute(DoGetRowIndex(rID), uiField, rState));
}

/** Get the item flags
  * \param rID     [input] : the ID of the item to query
  * \param uiField [input] : the field of the item to query
  * \return the item flags.
  *
  * Default implementation returns always WX_ITEM_FLAGS_ALL.
  * Reimplementation recommended for complex usage scenario
  */
wxVirtualIListDataModel::EFlags wxVirtualIListDataModel::GetItemFlags(const wxVirtualItemID &rID, size_t uiField)
{
    if (!rID.IsOK()) return(WX_ITEM_FLAGS_ALL);
    return(GetListItemFlags(DoGetRowIndex(rID), uiField));
}

/** Set the item data
  * \param rID     [input]: the ID of the item to modify
  * \param uiField [input]: the field of the data to modify
  * \param vValue  [input]: the new value
  * \param eType   [input]: the kind of value to modify
  * \return true if the data was modified, false otherwise
  *
  * Default implementation does nothing and returns true.
  * Reimplement for read/write models
  */
bool wxVirtualIListDataModel::SetItemData(const wxVirtualItemID &rID, size_t uiField, const wxVariant &vValue, EDataType eType)
{
    if (!rID.IsOK()) return(false);
    return(SetListItemData(DoGetRowIndex(rID), uiField, vValue, eType));
}

//--------------------- INTERFACE WITH COMMON IMPLEMENTATION --------//
/** Get the last item
  * \return the last item of the mode
  */
wxVirtualItemID wxVirtualIListDataModel::GetLastItem(void)
{
    size_t uiCount = GetItemCount();
    if (uiCount == 0) return(CreateInvalidItemID());
    return(DoGetItemID(uiCount - 1));
}

/** Get the index of a child. Used by wxVirtualTreePath::Set
  * \param rChild  [input]: the child item
  * \return the index of rChild, if found
  *         size_t(-1) if not found
  *
  * Reimplement if maximal performance is needed.
  * Default implementation use:
  *     GetChildCount
  *     GetChild
  *     GetParent
  * Used by default implementation of :
  *     IsLastChild
  *     GetNextSibling
  *     GetPrevSibling
  *
  * O(k) time, O(1) space (k == amount of siblings)
  */
size_t wxVirtualIListDataModel::GetChildIndex(const wxVirtualItemID &rChild)
{
    //special cases
    if (!rChild.IsOK()) return(size_t(-1));
    if (rChild.IsRootItem()) return(size_t(-1));
    return(DoGetRowIndex(rChild));
}

/** Get the index of a child. Used by GetNextItem internally
  * \param rParent [input]: the parent item
  * \param rChild  [input]: the child item
  * \return the index of rChild, if found
  *         size_t(-1) if not found
  *
  * Reimplement if maximal performance is needed.
  * Default implementation use:
  *     GetChildCount
  *     GetChild
  * Used by default implementation of :
  *     IsLastChild
  *     GetNextSibling
  *     GetPrevSibling
  *
  * O(k) time, O(1) space (k == amount of siblings)
  */
size_t wxVirtualIListDataModel::GetChildIndex(const wxVirtualItemID &rParent,
                                          const wxVirtualItemID &rChild)
{
    //special cases
    if (!rChild.IsOK()) return(size_t(-1));
    if (!rParent.IsRootItem()) return(size_t(-1));
    if (rParent == rChild) return(size_t(-1));

    return(DoGetRowIndex(rChild));
}

/** Check if an item is the last child of a parent item
  * \param rChild [input]: the item to check
  * \return true if the item is the last child (GetChildIndex() == GetParent(rChild).GetChildCount() - 1)
  *         false otherwise
  *
  * Default implementation use GetParent, GetChildIndex, GetChildCount
  * O(k) time, with k == amount of children
  * O(1) space
  *
  * Reimplementation recommended if better than O(k) time is possible
  */
bool wxVirtualIListDataModel::IsLastChild(const wxVirtualItemID &rChild)
{
    if (rChild.IsRootItem()) return(false);
    if (!rChild.IsOK()) return(false);

    size_t uiCount = GetItemCount();
    size_t uiRow   = DoGetRowIndex(rChild);
    if (uiRow + 1 == uiCount) return(true);
    return(false);
}

/** Get the depth (or level) of an item in the tree
  * Root item has depth == 0
  * This is also equals to the amount of ancestors nodes
  * \param rID [input] : the item to query
  * \return the depth of the item
  *
  * Used by default implementation of PrevItem
  *
  * Default implementation use GetParent repeatidly, until the parent is the root
  * O(log(n)) time, O(1) space
  *
  */
size_t wxVirtualIListDataModel::GetDepth(const wxVirtualItemID &rID)
{
    if (rID.IsRootItem()) return(0);
    if (!rID.IsOK()) return(0);
    return(1);
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
  *
  * Default implementation use a wxStack internally
  * O(n) time, O(n) space (worst cases, generally O(log(n)) time and space)
  *
  * Used by FindItem when start item is the root item
  */
size_t wxVirtualIListDataModel::GetSubTreeSize(const wxVirtualItemID &rID,
                                               wxVirtualIStateModel *pStateModel)
{
    if (rID.IsRootItem()) return(GetItemCount() + 1); //root : amount of items + root item
    if (!rID.IsOK()) return(0); //invalid item : 0
    return(1); //normal item : no children
}

/** Get next sibling
  * \param rID [input]: the item to query
  * \return the next sibling, if there is one
  *
  * Default implementation use :
  *     GetParent
  *     GetChildIndex
  *     GetChildCount
  *
  * O(k) time, O(1) space (k == amount of siblings)
  *
  * Reimplementation recommended if better performance can be expected
  */
wxVirtualItemID wxVirtualIListDataModel::GetNextSibling(const wxVirtualItemID &rID)
{
    if (!rID.IsOK()) return(CreateInvalidItemID());
    size_t uiRowIndex = DoGetRowIndex(rID);
    if (uiRowIndex + 1 >= GetItemCount()) return(CreateInvalidItemID());
    return(DoGetItemID(uiRowIndex + 1));
}

/** Get previous sibling
  * \param rID [input]: the item to query
  * \return the next sibling, if there is one
  *
  * Default implementation use :
  *     GetParent
  *     GetChildIndex
  *     GetChildCount
  *
  * O(k) time, O(1) space (k == amount of siblings)
  *
  */
wxVirtualItemID wxVirtualIListDataModel::GetPreviousSibling(const wxVirtualItemID &rID)
{
    if (!rID.IsOK()) return(CreateInvalidItemID());
    size_t uiRowIndex = DoGetRowIndex(rID);
    if (uiRowIndex >= GetItemCount()) return(CreateInvalidItemID());
    if (uiRowIndex == 0) return(CreateInvalidItemID());
    return(DoGetItemID(uiRowIndex - 1));
}

/** Get the left-most deepest descendent in the sub-tree
  * \param rID [input]: the root of the sub-tree
  * \param pStateModel [input]: the state model
  *                             If NULL, all items are examined
  *                             If non-null, children of collapsed node are not scanned
  * \return the Item ID of the left-most descendent
  *         This is useful for finding the next item
  *
  * The default implementation use in a loop :
  *     GetChildCount > 0
  *     GetChild
  *
  * So reimplementation is probably not necessary unless GetChildCount/GetChild runs in O(k) time
  */
wxVirtualItemID wxVirtualIListDataModel::GetFirstDescendent(const wxVirtualItemID &rID,
                                                            wxVirtualIStateModel *pStateModel)
{
    if (!rID.IsRootItem()) return(CreateInvalidItemID());
    size_t uiCount = GetItemCount();
    if (uiCount == 0) return(CreateInvalidItemID());
    return(DoGetItemID(0));
}

/** Get the right-most deepest descendent in the sub-tree
  * \param rID [input]: the root of the sub-tree
  * \param pStateModel [input]: the state model
  *                             If NULL, all items are examined
  *                             If non-null, children of collapsed node are not scanned
  * \return the Item ID of the right-most descendent
  *         This is useful for finding the previous item
  *
  * The default implementation use in a loop :
  *     GetChildCount > 0
  *     GetChild
  *
  * So reimplementation is probably not necessary unless GetChildCount/GetChild runs in O(k) time
  */
wxVirtualItemID wxVirtualIListDataModel::GetLastDescendent(const wxVirtualItemID &rID,
                                                           wxVirtualIStateModel *pStateModel)
{
    if (!rID.IsRootItem()) return(CreateInvalidItemID());
    size_t uiRow = DoGetRowIndex(rID);
    size_t uiCount = GetItemCount();
    if (uiCount == 0) return(CreateInvalidItemID());
    if (uiRow == 0) return(CreateInvalidItemID());
    if (uiRow - 1 >= uiCount) return(CreateInvalidItemID());
    return(DoGetItemID(uiRow - 1));
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
  *
  * The default implementation use the following methods:
  *     GetSubTreeSize() : O(nLog(n)) time, O(Log(n)) space
  *     GetChildCount()  : O(k) worst case with k == amount of children
  *     NextItem()       : O(n) time, O(1) space
  *     PrevItem()       : O(n) time, O(1) space
  *
  * Reimplementation recommended if better performances can be expected.
  * Default implementation was tested with millions of items without too much problems.
  */
wxVirtualItemID wxVirtualIListDataModel::FindItem(size_t uiRank, wxVirtualIStateModel *pStateModel,
                                                  const wxVirtualItemID &rStart, size_t uiStartRank)
{
    //special cases
    if (uiRank == uiStartRank) return(rStart);

    //case where uiStartRank = 0 (starting at root)
    if (uiStartRank == 0)
    {
        if (uiRank == 0) return(GetRootItem());
        if (uiRank >= GetItemCount()) return(CreateInvalidItemID());
        return(DoGetItemID(uiRank - 1));
    }
    else if (uiStartRank < uiRank)
    {
        size_t uiDiff = uiRank - uiStartRank;
        size_t uiLevel = 100; //arbitrary but not 0
        return(NextItem(rStart, uiLevel, uiLevel, pStateModel, uiDiff));
    }
    else if (uiStartRank > uiRank)
    {
        size_t uiDiff  = uiStartRank - uiRank;
        size_t uiLevel = 100; //arbitrary but not 0
        return(PrevItem(rStart, uiLevel, uiLevel, pStateModel, uiDiff));
    }

    return(CreateInvalidItemID());
}

/** Find the rank of an item
  * \param rID         [input]: the item to query
  * \param pStateModel [input]: the state model to use
  *                             if NULL, all items are considered to be expanded
  * \return the rank of the item. Root item has rank 0
  *
  * The default implementation use NextItem and runs in O(n) time and O(1) space
  */
size_t wxVirtualIListDataModel::GetItemRank(const wxVirtualItemID &rID, wxVirtualIStateModel *pStateModel)
{
    if (rID.IsRootItem()) return(0);
    if (!rID.IsOK()) return(0);

    return(DoGetRowIndex(rID) + 1);
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
  *
  * Default implementation use:
  *     GetChild(id, 0) //first child
  *     GetNextSibling()
  *     GetParent()
  *
  * Runs in O(k) time (k == amount of children) and O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::NextItem(const wxVirtualItemID &rID,
                                                  size_t uiLevel, size_t &uiNextItemLevel,
                                                  wxVirtualIStateModel *pStateModel,
                                                  size_t uiNbItems)
{
    size_t uiCount = GetItemCount();
    if (rID.IsRootItem())
    {
        if (uiCount <= uiNbItems) return(CreateInvalidItemID());
        uiNextItemLevel = uiLevel + 1;
        return(DoGetItemID(uiNbItems - 1));
    }

    size_t uiRowIndex = DoGetRowIndex(rID);
    if (uiRowIndex + uiNbItems >= uiCount) return(CreateInvalidItemID());
    uiNextItemLevel = uiLevel;
    return(DoGetItemID(uiRowIndex + uiNbItems));
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
  *
  * Default implementation use:
  *     GetChild(id, 0) //first child
  *     GetNextSibling()
  *     GetParent()
  *
  * Runs in O(k) time (k == amount of children) and O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::NextItem(const wxVirtualItemID &rID,
                                                  wxVirtualIStateModel *pStateModel,
                                                  size_t uiNbItems)
{
    size_t uiCount = GetItemCount();
    if (rID.IsRootItem())
    {
        if (uiCount <= uiNbItems) return(CreateInvalidItemID());
        return(DoGetItemID(uiNbItems - 1));
    }

    size_t uiRowIndex = DoGetRowIndex(rID);
    if (uiRowIndex + uiNbItems >= uiCount) return(CreateInvalidItemID());
    return(DoGetItemID(uiRowIndex + uiNbItems));
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
  * \return the next item
  *
  * Default implementation use:
  *     GetPreviousSibling() : O(k) time
  *     GetLastDescendent()  : O(k) time
  *     GetDepth()           : O(log(n)) time
  *     GetParent()          : O(log(n)) time
  *
  * Runs in O(k) time (k == amount of children) and O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::PrevItem(const wxVirtualItemID &rID,
                                                  size_t uiLevel, size_t &uiPrevItemLevel,
                                                  wxVirtualIStateModel *pStateModel,
                                                  size_t uiNbItems)
{
    if (!rID.IsOK()) return(CreateInvalidItemID());
    size_t uiCount = GetItemCount();
    size_t uiRowIndex = DoGetRowIndex(rID);

    if (uiRowIndex >= uiCount) return(CreateInvalidItemID());
    if (uiRowIndex < uiNbItems) return(CreateInvalidItemID());
    if (uiRowIndex - uiNbItems >= uiCount) return(CreateInvalidItemID());
    uiPrevItemLevel = uiLevel;
    return(DoGetItemID(uiRowIndex - uiNbItems));
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
  * \return the next item
  *
  * Default implementation use:
  *     GetPreviousSibling() : O(k) time
  *     GetLastDescendent()  : O(k) time
  *     GetDepth()           : O(log(n)) time
  *     GetParent()          : O(log(n)) time
  *
  * Runs in O(k) time (k == amount of children) and O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::PrevItem(const wxVirtualItemID &rID,
                                                  wxVirtualIStateModel *pStateModel,
                                                  size_t uiNbItems)
{
    if (!rID.IsOK()) return(CreateInvalidItemID());
    size_t uiCount = GetItemCount();
    size_t uiRowIndex = DoGetRowIndex(rID);

    if (uiRowIndex >= uiCount) return(CreateInvalidItemID());
    if (uiRowIndex < uiNbItems) return(CreateInvalidItemID());
    if (uiRowIndex - uiNbItems >= uiCount) return(CreateInvalidItemID());
    return(DoGetItemID(uiRowIndex - uiNbItems));
}

/** Check if rID is inside a sub-tree
  * \param rID [input]: the item ID to test
  * \param rSubTreeRootID [input]: the top node of the sub-tree to test
  * \return true if rID belongs to the sub-tree below rSubTreeRootID
  *         false otherwise
  *
  * Used to move focus when the focused item is collapsed.
  *
  * The default implementation use GetParent() in loop
  *
  * O(log(n)) time, O(1) space
  */
bool wxVirtualIListDataModel::IsInSubTree(const wxVirtualItemID &rID,
                                          const wxVirtualItemID &rSubTreeRootID)
{
    if (rID == rSubTreeRootID) return(true);
    if (!rSubTreeRootID.IsRootItem()) return(false);
    if (!rID.IsOK()) return(false);

    size_t uiRowIndex = DoGetRowIndex(rID);
    size_t uiCount    = GetItemCount();
    if (uiRowIndex >= uiCount) return(false);

    return(true);
}

//---------------------- SEARCHING ----------------------------------//
/** Find the 1st item matching a criteria in 1 field
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param uiField        [input]: the field to search
  * \return the first item found matching the criteria
  *
  * The default implementation use FindNext on root item
  * O(n) time, O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::FindFirst(const wxVariant &rvValue,
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
  *
  * The default implementation use :
  *     NextItem
  *     IsMatching
  *
  * O(n) time, O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::FindNext(const wxVirtualItemID &rID,
                                                  const wxVariant &rvValue,
                                                  bool bFullMatch,
                                                  bool bCaseSensitive,
                                                  bool bRegEx,
                                                  size_t uiField)
{
    size_t uiCount = GetItemCount();
    size_t uiStart = DoGetRowIndex(rID);
    if (rID.IsRootItem()) uiStart = 0;

    size_t i;
    for(i = uiStart; i < uiCount; i++)
    {
        wxVirtualItemID id = DoGetItemID(i);
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField)) return(id);
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
  *
  * The default implementation use :
  *     PrevItem
  *     IsMatching
  *
  */
wxVirtualItemID wxVirtualIListDataModel::FindPrev(const wxVirtualItemID &rID,
                                                  const wxVariant &rvValue,
                                                  bool bFullMatch,
                                                  bool bCaseSensitive,
                                                  bool bRegEx,
                                                  size_t uiField)
{
    size_t uiCount = GetItemCount();
    if (uiCount == 0) return(CreateInvalidItemID());

    size_t uiStart = DoGetRowIndex(rID) + 1;
    if (rID.IsRootItem()) uiStart = uiCount;

    size_t i;
    for(i = uiStart; i > 0; i--)
    {
        wxVirtualItemID id = DoGetItemID(i - 1);
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField)) return(id);
    }
    return(CreateInvalidItemID());
}

/** Find the 1st item matching a criteria in all fields
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \return the first item found matching the criteria
  *
  * The default implementation use FindNext on root item
  * O(n) time, O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::FindFirst(const wxVariant &rvValue,
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
  *
  * The default implementation use :
  *     NextItem
  *     IsMatching
  *
  * O(n) time, O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::FindNext(const wxVirtualItemID &rID,
                                                  const wxVariant &rvValue,
                                                  bool bFullMatch,
                                                  bool bCaseSensitive,
                                                  bool bRegEx)
{
    size_t uiCount = GetItemCount();
    size_t uiStart = DoGetRowIndex(rID);
    if (rID.IsRootItem()) uiStart = 0;

    size_t i;
    for(i = uiStart; i < uiCount; i++)
    {
        wxVirtualItemID id = DoGetItemID(i);
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx)) return(id);
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
  *
  * The default implementation use :
  *     PrevItem
  *     IsMatching
  *
  * O(n) time, O(1) space
  */
wxVirtualItemID wxVirtualIListDataModel::FindPrev(const wxVirtualItemID &rID,
                                                  const wxVariant &rvValue,
                                                  bool bFullMatch,
                                                  bool bCaseSensitive,
                                                  bool bRegEx)
{
    size_t uiCount = GetItemCount();
    if (uiCount == 0) return(CreateInvalidItemID());

    size_t uiStart = DoGetRowIndex(rID) + 1;
    if (rID.IsRootItem()) uiStart = uiCount;

    size_t i;
    for(i = uiStart; i > 0; i--)
    {
        wxVirtualItemID id = DoGetItemID(i - 1);
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx)) return(id);
    }
    return(CreateInvalidItemID());
}
