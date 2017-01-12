/**********************************************************************/
/** FILE    : VirtualIDataModel.cpp                                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a data model                              **/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/Models/VirtualIProxyDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>
#include <wx/VirtualDataView/Types/HashUtils.h>
#include <wx/stack.h>
#include <wx/regex.h>
#include <wx/hashset.h>

#define SPEED_OPTIMIZATION_1     1
#define SPEED_OPTIMIZATION_2     1

//---------------------- STATIC MEMBERS -----------------------------//
wxVirtualItemID wxVirtualIDataModel::s_RootID = wxVirtualItemID();

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualIDataModel::wxVirtualIDataModel(void)
    : m_pParentModel(WX_VDV_NULL_PTR)
{
}

/** Destructor
  */
wxVirtualIDataModel::~wxVirtualIDataModel(void)
{
}

//---------------------- INTERFACE : CLASS TYPE ---------------------//
/** Get the model class flags
  * This is an alternative to RTTI and dynamic_cast
  * This allows to refuse some kind of models for specialization of wxVirtualDataViewCtrl
  * \return a set of flags EClassType
  */
int wxVirtualIDataModel::GetModelClass(void) const
{
    return(WX_MODEL_CLASS_BASE);
}

//--------------------- INTERFACE: HIERARCHY ------------------------//
/** Get all children at once
  * \param vIDs      [output]: the list of children item. Previous content is erased
  * \param rIDParent [input] : the ID of the parent item
  * \return the amount of children in vIDs
  *
  * The default implementation calls GetChildCount, followed by N calls to GetChild
  * O(k) time (k == amount of children), O(1) space
  */
size_t wxVirtualIDataModel::GetAllChildren(wxVirtualItemIDs &vIDs,
                                               const wxVirtualItemID &rIDParent)
{
    vIDs.clear();
    size_t uiNbChildren = GetChildCount(rIDParent);
    size_t uiChild;
    for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
    {
        wxVirtualItemID idChild = GetChild(rIDParent, uiChild);
        if (idChild.IsOK())vIDs.push_back(idChild);
    }

    return(vIDs.size());
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
size_t wxVirtualIDataModel::GetFieldCount(const wxVirtualItemID &rID)
{
    return(1);
}

//-------------------- INTERFACE : ITEM DATA ------------------------//
/** Get the item data
  * \param rID   [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  *
  * Default implementation return wxVariant().
  * Reimplementation is strongly encouraged
  */
wxVariant wxVirtualIDataModel::GetItemData(const wxVirtualItemID &rID,
                                           size_t uiField, EDataType eType)
{
    return(wxVariant());
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
wxVirtualDataViewItemAttr* wxVirtualIDataModel::GetItemAttribute(const wxVirtualItemID &rID,
                                                                 size_t uiField,
                                                                 const wxVirtualDataViewItemState &rState)
{
    return(WX_VDV_NULL_PTR);
}

/** Get the item flags
  * \param rID     [input] : the ID of the item to query
  * \param uiField [input] : the field of the item to query
  * \return the item flags.
  *
  * Default implementation returns always WX_ITEM_FLAGS_ALL.
  * Reimplementation recommended for complex usage scenario
  */
wxVirtualIDataModel::EFlags wxVirtualIDataModel::GetItemFlags(const wxVirtualItemID &rID, size_t uiField)
{
    return(WX_ITEM_FLAGS_ALL);
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
bool wxVirtualIDataModel::SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType)
{
    return(true);
}

//--------------------- INTERFACE WITH COMMON IMPLEMENTATION --------//
/** Get the root item
  * \return the root item
  *
  * Default implementation returns wxVirtualItemID(0, 0, 0, this)
  * Reimplement if root item has special ID other than all 0
  */
wxVirtualItemID wxVirtualIDataModel::GetRootItem(void)
{
    return(wxVirtualItemID(0, 0, 0, this));
}

/** Check if the item is the root item
  * \param rID [input]: the item to check
  * \return true if rID is the root item, false otherwise
  */
bool wxVirtualIDataModel::IsRootItem(const wxVirtualItemID &rID)
{
    return(rID.IsRootItem());
}

/** Get the last item
  * \return the last item of the mode
  */
wxVirtualItemID wxVirtualIDataModel::GetLastItem(void)
{
    wxVirtualItemID id = GetRootItem();
    size_t uiNbChildren = id.GetChildCount();
    while((uiNbChildren > 0) && (!id.IsInvalid()))
    {
        wxVirtualItemID idChild = GetChild(id, uiNbChildren - 1);
        if (idChild.IsInvalid()) return(id);
        id = idChild;
        uiNbChildren = id.GetChildCount();
    }
    return(id);
}

/** Return true if an item expander should be drawn for this item
  * \param rID [input]: the item to query
  * \return true if an expander should be drawn
  *         false otherwise
  *
  * The default implementation returns true when GetChildCount(rID) > 0
  */
bool wxVirtualIDataModel::HasExpander(const wxVirtualItemID &rID)
{
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
bool wxVirtualIDataModel::HasCheckbox(const wxVirtualItemID &rID)
{
    EFlags eFlags = GetItemFlags(rID, 0);
    return((eFlags & WX_ITEM_FLAGS_CHECKABLE) || (eFlags & WX_ITEM_FLAGS_HAS_TRISTATE));
}

/** Return true if an item is editable
  * \param rID [input]: the item to query
  * \return true if the item is editable, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_EDITABLE
  */
bool wxVirtualIDataModel::IsEditable(const wxVirtualItemID &rID)
{
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_EDITABLE);
}

/** Return true if an item is selectable
  * \param rID [input]: the item to query
  * \return true if the item is selectable, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_SELECTABLE
  */
bool wxVirtualIDataModel::IsSelectable(const wxVirtualItemID &rID)
{
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
bool wxVirtualIDataModel::IsCheckable(const wxVirtualItemID &rID)
{
    EFlags eFlags = GetItemFlags(rID, 0);
    return((eFlags & WX_ITEM_FLAGS_CHECKABLE) || (eFlags & WX_ITEM_FLAGS_HAS_TRISTATE));
}

/** Return true if an item is checkable with 3 state
  * \param rID [input]: the item to query
  * \return true if the item is checkable with 3 state, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_HAS_TRISTATE
  */
bool wxVirtualIDataModel::IsCheckable3DState(const wxVirtualItemID &rID)
{
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_HAS_TRISTATE);
}

/** Return true if an item is a drag source
  * \param rID [input]: the item to query
  * \return true if the item is a drag source, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_DRAGGABLE
  */
bool wxVirtualIDataModel::IsDragSource(const wxVirtualItemID &rID)
{
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_DRAGGABLE);
}

/** Return true if an item is a drop target
  * \param rID [input]: the item to query
  * \return true if the item is a drop target, false otherwise
  * The default implementation returns true if the item in field 0 has one of this flag set:
  *     WX_ITEM_FLAGS_DROPPABLE
  */
bool wxVirtualIDataModel::IsDropTarget(const wxVirtualItemID &rID)
{
    EFlags eFlags = GetItemFlags(rID, 0);
    return(eFlags & WX_ITEM_FLAGS_DROPPABLE);
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
size_t wxVirtualIDataModel::GetChildIndex(const wxVirtualItemID &rChild)
{
    //special cases
    if (!rChild.IsOK()) return(size_t(-1));
    if (rChild.HasChildIndex()) return(rChild.GetChildIndex());

    wxVirtualItemID idParent = GetParent(rChild);
    if (idParent == rChild) return(size_t(-1));

    //scan
    size_t uiNbChildren = GetChildCount(idParent);
    size_t uiChild;
    for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
    {
        wxVirtualItemID idChild = GetChild(idParent, uiChild);
        if (idChild == rChild) return(uiChild);
    }

    //not found
    return(size_t(-1));
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
size_t wxVirtualIDataModel::GetChildIndex(const wxVirtualItemID &rParent,
                                          const wxVirtualItemID &rChild)
{
    //special cases
    if (!rChild.IsOK()) return(size_t(-1));
    if (rParent == rChild) return(size_t(-1));
    if (rChild.HasChildIndex()) return(rChild.GetChildIndex());

    //scan
    size_t uiNbChildren = GetChildCount(rParent);
    size_t uiChild;
    for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
    {
        wxVirtualItemID idChild = GetChild(rParent, uiChild);
        if (idChild == rChild) return(uiChild);
    }

    //not found
    return(size_t(-1));
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
bool wxVirtualIDataModel::IsLastChild(const wxVirtualItemID &rChild)
{
    wxVirtualItemID idParent = GetParent(rChild);
    size_t uiIndex           = GetChildIndex(idParent, rChild);
    size_t uiNbChildren      = GetChildCount(idParent);
    if (uiNbChildren == uiIndex + 1) return(true);
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
size_t wxVirtualIDataModel::GetDepth(const wxVirtualItemID &rID)
{
    size_t uiResult = 0;
    wxVirtualItemID id = rID;
    while (!id.IsRootOrInvalid())
    {
        id = GetParent(id);
        uiResult++;
    }
    return(uiResult);
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
size_t wxVirtualIDataModel::GetSubTreeSize(const wxVirtualItemID &rID,
                                           wxVirtualIStateModel *pStateModel)
{
    size_t uiResult = 0;
    long lChild, lNbChildren;
    wxStack<wxVirtualItemID> StackOfNodes;
    StackOfNodes.push(rID);

    if (pStateModel)
    {
        //do not recurse on collapse items
        while(StackOfNodes.size() > 0)
        {
            wxVirtualItemID id = StackOfNodes.top();
            StackOfNodes.pop();

            uiResult++;

            if (!pStateModel->IsExpanded(id)) continue;
            lNbChildren = GetChildCount(id);
            for(lChild = lNbChildren - 1; lChild >= 0; lChild--)
            {
                wxVirtualItemID idChild = GetChild(id, lChild);
                StackOfNodes.push(idChild);
            }
        }
    }
    else
    {
        //count all items
        while(StackOfNodes.size() > 0)
        {
            wxVirtualItemID id = StackOfNodes.top();
            StackOfNodes.pop();

            uiResult++;

            lNbChildren = GetChildCount(id);
            for(lChild = lNbChildren - 1; lChild >= 0; lChild--)
            {
                wxVirtualItemID idChild = GetChild(id, lChild);
                StackOfNodes.push(idChild);
            }
        }
    }

    return(uiResult);
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
wxVirtualItemID wxVirtualIDataModel::FindItem(size_t uiRank,
                                              wxVirtualIStateModel *pStateModel,
                                              const wxVirtualItemID &rStart,
                                              size_t uiStartRank)
{
    //special cases
    if (uiRank == uiStartRank) return(rStart);

    //case where uiStartRank = 0 (starting at root)
    if (uiStartRank == 0)
    {
        size_t uiNbNodes;
        size_t uiCurrentRank = uiStartRank;
        wxStack<wxVirtualItemID> StackOfNodes;
        long lNbChildren, lChild;

        StackOfNodes.push(rStart);
        while(StackOfNodes.size() > 0)
        {
            wxVirtualItemID id = StackOfNodes.top();
            StackOfNodes.pop();

            if (uiCurrentRank == uiRank) return(id);

            //recurse on children
            lNbChildren = GetChildCount(id);
            //for(lChild = lNbChildren - 1; lChild >= 0; lChild--)
            for(lChild = 0; lChild < lNbChildren; lChild++)
            {
                wxVirtualItemID idChild = GetChild(id, lChild);

                //add to the stack only the child which contains the searched node
                uiNbNodes = GetSubTreeSize(idChild, pStateModel);
                if (uiCurrentRank + uiNbNodes < uiRank)
                {
                    //skip this child - it is located before the node we search
                    uiCurrentRank += uiNbNodes;
                    continue;
                }

                //(uiCurrentRank + uiNodes >= uiRank)
                //this child contains the node somewhere in the sub-tree
                //add it, and avoid searching for other children
                StackOfNodes.push(idChild);
                break;
            }

            uiCurrentRank++;
        }
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
size_t wxVirtualIDataModel::GetItemRank(const wxVirtualItemID &rID,
                                        wxVirtualIStateModel *pStateModel)
{
    size_t uiResult = 0;
    if (!rID.IsOK()) return(uiResult);

    wxVirtualItemID id = GetRootItem();
    size_t uiLevel = 1;
    while(id != rID)
    {
        id = NextItem(id, uiLevel, uiLevel, pStateModel, 1);
        if (!id.IsOK()) return(0);
        uiResult++;
    }

    return(uiResult);
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
wxVirtualItemID wxVirtualIDataModel::GetNextSibling(const wxVirtualItemID &rID)
{
    wxVirtualItemID idParent = GetParent(rID);
    size_t uiChildIndex = GetChildIndex(idParent, rID);
    size_t uiChildCount = GetChildCount(idParent);

    if (uiChildIndex + 1 < uiChildCount) return(GetChild(idParent, uiChildIndex + 1));
    return(CreateInvalidItemID());
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
wxVirtualItemID wxVirtualIDataModel::GetPreviousSibling(const wxVirtualItemID &rID)
{
    wxVirtualItemID idParent = GetParent(rID);
    size_t uiChildIndex = GetChildIndex(idParent, rID);

    if (uiChildIndex > 0) return(GetChild(idParent, uiChildIndex - 1));
    return(CreateInvalidItemID());
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
wxVirtualItemID wxVirtualIDataModel::GetFirstDescendent(const wxVirtualItemID &rID,
                                                        wxVirtualIStateModel *pStateModel)
{
    wxVirtualItemID id = rID;
    if (pStateModel)
    {
        while ((pStateModel->IsExpanded(id)) && (GetChildCount(id) > 0))
        {
            wxVirtualItemID idChild = GetChild(id, 0);
            if (!idChild.IsOK()) return(id);
            id = idChild;
        }
    }
    else
    {
        while (GetChildCount(id) > 0)
        {
            wxVirtualItemID idChild = GetChild(id, 0);
            if (!idChild.IsOK()) return(id);
            id = idChild;
        }
    }

    return(id);
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
wxVirtualItemID wxVirtualIDataModel::GetLastDescendent(const wxVirtualItemID &rID,
                                                       wxVirtualIStateModel *pStateModel)
{
    wxVirtualItemID id = rID;
    if (pStateModel)
    {
        while ((pStateModel->IsExpanded(id)) && (GetChildCount(id) > 0))
        {
            wxVirtualItemID idChild = GetChild(id, GetChildCount(id) - 1);
            if (!idChild.IsOK()) return(id);
            id = idChild;
        }
    }
    else
    {
        while (GetChildCount(id) > 0)
        {
            wxVirtualItemID idChild = GetChild(id, GetChildCount(id) - 1);
            if (!idChild.IsOK()) return(id);
            id = idChild;
        }
    }

    return(id);
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
wxVirtualItemID wxVirtualIDataModel::NextItem(const wxVirtualItemID &rID,
                                              size_t uiLevel, size_t &uiNextItemLevel,
                                              wxVirtualIStateModel *pStateModel,
                                              size_t uiNbItems)
{
    bool bIsExpanded = true;

    #if SPEED_OPTIMIZATION_1 != 0
        wxVirtualItemID idParent = GetParent(rID);
        size_t uiChildIndex = GetChildIndex(idParent, rID);
        size_t uiChildCount = GetChildCount(idParent);
    #endif // SPEED_OPTIMIZATION_1

    size_t i;
    wxVirtualItemID idCurrent = rID;
    for(i=0;i<uiNbItems;i++)
    {
        //get fist child
        if (pStateModel) bIsExpanded = pStateModel->IsExpanded(idCurrent);

        if (bIsExpanded)
        {
            wxVirtualItemID idChild = GetChild(idCurrent, 0);
            if ((idChild.IsOK()) && (idChild != idCurrent))
            {
                uiNextItemLevel = uiLevel + 1; //GetDepth(idChild);
                #if SPEED_OPTIMIZATION_1 != 0
                    idParent = idCurrent;
                    uiChildIndex = 0;
                    uiChildCount = GetChildCount(idParent);
                #endif // SPEED_OPTIMIZATION_1
                idCurrent = idChild;
                continue;
            }
        }

        //next brother
        wxVirtualItemID id = idCurrent;
        uiNextItemLevel = uiLevel;
        do
        {
            //get next sibling
            #if SPEED_OPTIMIZATION_1 != 0
                if (uiChildIndex + 1 < uiChildCount)
                {
                    uiChildIndex++;
                    wxVirtualItemID idBrother = GetChild(idParent, uiChildIndex);
                    if (idBrother.IsOK())
                    {
                        id = idBrother;
                        idParent = GetParent(id);
                        uiChildIndex = GetChildIndex(idParent, id);
                        uiChildCount = GetChildCount(idParent);

                        break;
                    }
                }
                id = idParent;
                if (uiNextItemLevel > 0) uiNextItemLevel--;

                idParent = GetParent(id);
                uiChildIndex = GetChildIndex(idParent, id);
                uiChildCount = GetChildCount(idParent);
//
            #else
                wxVirtualItemID idBrother = GetNextSibling(id);
                if (idBrother.IsOK())
                {
                    id = idBrother;
                    break;
                }
                id = GetParent(id);
                if (uiNextItemLevel > 0) uiNextItemLevel--;
            #endif // SPEED_OPTIMIZATION_1

        } while (id.IsOK());

        idCurrent = id;
    }
    if (idCurrent.IsOK()) return(idCurrent);

    //root
    uiNextItemLevel = 0;
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
  *
  * Default implementation use:
  *     GetChild(id, 0) //first child
  *     GetNextSibling()
  *     GetParent()
  *
  * Runs in O(k) time (k == amount of children) and O(1) space
  */
wxVirtualItemID wxVirtualIDataModel::NextItem(const wxVirtualItemID &rID,
                                              wxVirtualIStateModel *pStateModel,
                                              size_t uiNbItems)
{
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
wxVirtualItemID wxVirtualIDataModel::PrevItem(const wxVirtualItemID &rID,
                                              size_t uiLevel, size_t &uiPrevItemLevel,
                                              wxVirtualIStateModel *pStateModel,
                                              size_t uiNbItems)
{
    size_t i;
    wxVirtualItemID idCurrent = rID;

    #if SPEED_OPTIMIZATION_2 != 0
        wxVirtualItemID idParent = GetParent(idCurrent);
        size_t uiChildIndex = GetChildIndex(idParent, idCurrent);
    #endif // SPEED_OPTIMIZATION_2

    for(i=0;i<uiNbItems;i++)
    {
        //last child of previous sibling
        #if SPEED_OPTIMIZATION_2 != 0
            wxVirtualItemID idSibling;
            if (uiChildIndex > 0)
            {
                uiChildIndex--;
                idSibling = GetChild(idParent, uiChildIndex);
            }
            else
            {
                idSibling = CreateInvalidItemID();
            }
        #else
            wxVirtualItemID idSibling = GetPreviousSibling(idCurrent);
        #endif // SPEED_OPTIMIZATION_2

        if (idSibling.IsOK())
        {
            wxVirtualItemID id = GetLastDescendent(idSibling, pStateModel);
            uiPrevItemLevel = GetDepth(id);
            idCurrent = id;

            #if SPEED_OPTIMIZATION_2 != 0
                idParent = GetParent(idCurrent);
                uiChildIndex = GetChildIndex(idParent, idCurrent);
            #endif // SPEED_OPTIMIZATION_2
            continue;
        }

        //parent
        if (uiLevel > 0) uiPrevItemLevel = uiLevel - 1;
        else             uiPrevItemLevel = 0;
        idCurrent = GetParent(idCurrent);

        #if SPEED_OPTIMIZATION_2 != 0
            idParent = GetParent(idCurrent);
            uiChildIndex = GetChildIndex(idParent, idCurrent);
        #endif // SPEED_OPTIMIZATION_2
    }

    return(idCurrent);
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
wxVirtualItemID wxVirtualIDataModel::PrevItem(const wxVirtualItemID &rID,
                                              wxVirtualIStateModel *pStateModel,
                                              size_t uiNbItems)
{
    size_t uiLevel = 1;
    return(PrevItem(rID, uiLevel, uiLevel, pStateModel, uiNbItems));
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
bool wxVirtualIDataModel::IsInSubTree(const wxVirtualItemID &rID,
                                      const wxVirtualItemID &rSubTreeRootID)
{
    wxVirtualItemID id = rID;
    while (id != rSubTreeRootID)
    {
        if (!id.IsOK()) return(false);
        id = GetParent(id);
    }

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
wxVirtualItemID wxVirtualIDataModel::FindFirst(const wxVariant &rvValue,
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
wxVirtualItemID wxVirtualIDataModel::FindNext(const wxVirtualItemID &rID,
                                              const wxVariant &rvValue,
                                              bool bFullMatch,
                                              bool bCaseSensitive,
                                              bool bRegEx,
                                              size_t uiField)
{
    wxVirtualItemID id = NextItem(rID, WX_VDV_NULL_PTR, 1);
    while (id.IsOK())
    {
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField)) return(id);
        id = NextItem(id, WX_VDV_NULL_PTR, 1);
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
wxVirtualItemID wxVirtualIDataModel::FindPrev(const wxVirtualItemID &rID,
                                              const wxVariant &rvValue,
                                              bool bFullMatch,
                                              bool bCaseSensitive,
                                              bool bRegEx,
                                              size_t uiField)
{
    wxVirtualItemID id = PrevItem(rID, WX_VDV_NULL_PTR, 1);
    while (id.IsOK())
    {
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField)) return(id);
        id = PrevItem(id, WX_VDV_NULL_PTR, 1);
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
wxVirtualItemID wxVirtualIDataModel::FindFirst(const wxVariant &rvValue,
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
wxVirtualItemID wxVirtualIDataModel::FindNext(const wxVirtualItemID &rID,
                                              const wxVariant &rvValue,
                                              bool bFullMatch,
                                              bool bCaseSensitive,
                                              bool bRegEx)
{
    wxVirtualItemID id = NextItem(rID, WX_VDV_NULL_PTR, 1);
    while (id.IsOK())
    {
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx)) return(id);
        id = NextItem(id, WX_VDV_NULL_PTR, 1);
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
wxVirtualItemID wxVirtualIDataModel::FindPrev(const wxVirtualItemID &rID,
                                              const wxVariant &rvValue,
                                              bool bFullMatch,
                                              bool bCaseSensitive,
                                              bool bRegEx)
{
    wxVirtualItemID id = PrevItem(rID, WX_VDV_NULL_PTR, 1);
    while (id.IsOK())
    {
        if (IsMatching(id, rvValue, bFullMatch, bCaseSensitive, bRegEx)) return(id);
        id = PrevItem(id, WX_VDV_NULL_PTR, 1);
    }
    return(CreateInvalidItemID());
}

/** Main matching method for searching
  * \param rID            [input]: the item to examine
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param uiField        [input]: the field to examine. If equal to size_t(-1), then all fields must be examined
  * \return true if the item rID matches the criteria, false otherwise
  */
bool wxVirtualIDataModel::IsMatching(wxVirtualItemID &rID, const wxVariant &rvValue,
                                     bool bFullMatch, bool bCaseSensitive, bool bRegex,
                                     size_t uiField)
{
    //get field bounds
    size_t uiStartField = uiField;
    size_t uiLastField  = uiField + 1;
    size_t i;
    if (uiField == size_t(-1))
    {
        uiStartField = 0;
        uiLastField  = GetFieldCount(rID);
    }

    //compile regex
    wxRegEx re;
    wxString sSearchedString = GetStringValue(rvValue);
    if (bRegex)
    {
        int iFlags = wxRE_DEFAULT;
        if (bCaseSensitive) iFlags |= wxRE_ICASE;
        re.Compile(sSearchedString, iFlags);
    }
    else if (!bCaseSensitive)
    {
        sSearchedString.MakeUpper();
    }

    //examine the fields one by one
    for(i=uiStartField; i < uiLastField; i++)
    {
        wxVariant v = GetItemData(rID, i, WX_ITEM_MAIN_DATA);
        if (bRegex)
        {
            //regex matching
            wxString s = GetStringValue(v);
            if (re.Matches(s))
            {
                //match found in regex: full match or partial match
                if (!bFullMatch) return(true);
                size_t uiMatchCount = re.GetMatchCount();
                size_t uiMatch;
                for(uiMatch = 0; uiMatch < uiMatchCount; uiMatch++)
                {
                    wxString sMatch;
                    re.GetMatch(sMatch, uiMatch);
                    if (sMatch == s) return(true);
                }
            }
        }
        else
        {
            //standard matching
            if ((bFullMatch) && (!bCaseSensitive))
            {
                if (v == rvValue) return(true);
                continue;
            }

            //conversion to string unavoidable
            wxString s = GetStringValue(v);
            if (!bCaseSensitive) s.MakeUpper();
            if (bFullMatch)
            {
                if (s == sSearchedString) return(true);
            }

            //partial string matching
            if (s.Find(sSearchedString) != wxNOT_FOUND) return(true);
        }
    }

    return(false);
}

//------------------ TSort NESTED STRUCTURE -------------------------//
/** Default constructor
  * \param uiField [input]: the field to sort
  * \param eSort   [input]: the sorting order
  */
wxVirtualIDataModel::TSort::TSort(size_t uiField, ESorting eSorting)
    : m_uiSortedField(uiField),
      m_eSortOrder(eSorting)
{
}

//------------------------- SORTING ---------------------------------//
/** Check if the field can be sorted by the data model
  * \param vSortFilters [input]: the sort filters to apply
  * \return true if the model can sort the items according to the filters, false otherwise
  *         Note that true is returned only if ALL THE SORT FILTERS can be taken into account
  *
  * The default implementation returns always false.
  */
bool wxVirtualIDataModel::CanSort(const TSortFilters &vSortFilters) const
{
    return(false);
}

/** Sort the values
  * \param vSortFilters [input]: the sort filters to apply
  *
  * The default implementation does nothing
  */
void wxVirtualIDataModel::Sort(const TSortFilters &vSortFilters)
{

}

/** Remove all sort filters
  * The default implementation does the following
  * \code
  *     TSortFilters vSortFilters;
        Sort(vSortFilters);
  * \endcode
  */
void wxVirtualIDataModel::ResetSorting(void)
{
    TSortFilters vSortFilters;
    Sort(vSortFilters);
}

//------------------- ID COMPARISON METHOD --------------------------//
#define WX_ID_LESS_THAN        -1
#define WX_ID_EQUAL             0
#define WX_ID_GREATER_THAN      1

/** Compare 2 items IDs
  * \param rID1 [input]: the 1st item ID
  * \param rID2 [input]: the 2nd item ID
  * \return WX_VARIANT_LESS_THAN     if rID1 < rID2
  *         WX_VARIANT_EQUAL         if rID1 == rID2
  *         WX_VARIANT_GREATER_THAN  if rID1 > rID2
  */
WX_VDV_INLINE int CompareItemID(const wxVirtualItemID &rID1, const wxVirtualItemID &rID2)
{
        //check on item IDs
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
    if (rID1.GetUInt64() < rID2.GetUInt64()) return(WX_ID_LESS_THAN);
    if (rID1.GetUInt64() > rID2.GetUInt64()) return(WX_ID_GREATER_THAN);
#else
    if (rID1.GetUInt32() < rID2.GetUInt32()) return(WX_ID_LESS_THAN);
    if (rID1.GetUInt32() > rID2.GetUInt32()) return(WX_ID_GREATER_THAN);
#endif // WX_VDD_ALLOW_64BITS_ITEMID
    return(WX_ID_EQUAL);
}


//------------------------ MAIN COMPARISON METHOD -------------------//
/** Compare 2 items
  * \param rID1    [input]: the 1st item ID
  * \param vValue1 [input]: the 1st variant
  * \param vValue2 [input]: the 2nd variant
  * \param rID2    [input]: the 2nd item ID
  * \return WX_VARIANT_LESS_THAN     if item1 < item2
  *         WX_VARIANT_EQUAL         if item1 == item2
  *         WX_VARIANT_GREATER_THAN  if item1 > item2
  */
WX_VDV_INLINE int CompareItems(const wxVirtualItemID &rID1, const wxVariant &vValue1,
                               const wxVirtualItemID &rID2, const wxVariant &vValue2)
{
    int iRes = CompareVariants(vValue1, vValue2);
    if (iRes == WX_VARIANT_CANNOT_COMPARE) iRes = CompareItemID(rID1, rID2);
    return(iRes);
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
wxVirtualIDataModel::ECompareResult wxVirtualIDataModel::Compare(const wxVirtualItemID &rID1,
                                                                 const wxVirtualItemID &rID2,
                                                                 size_t uiField)
{
    wxVariant vValue1 = GetItemData(rID1, uiField);
    wxVariant vValue2 = GetItemData(rID2, uiField);

    int iRes = CompareItems(rID1, vValue1, rID2, vValue2);
    return((ECompareResult) iRes);
}

/** Compare 2 items values
  * \param rID1       [input]: the 1st item to compare
  * \param rValue1    [input]: the value of the 1st item to compare
  * \param rID2       [input]: the 2nd item to compare
  * \param rValue2    [input]: the value of the 2nd item to compare
  * \return WX_E_LESS_THAN    if rID1 < rID2
  *         WX_E_EQUAL        if rID1 == rID2
  *         WX_E_GREATER_THAN if rID1 > rID2
  */
wxVirtualIDataModel::ECompareResult wxVirtualIDataModel::Compare(const wxVirtualItemID &rID1,
                                                                 const wxVariant &rValue1,
                                                                 const wxVirtualItemID &rID2,
                                                                 const wxVariant &rValue2)
{
    int iRes = CompareItems(rID1, rValue1, rID2, rValue2);
    return((ECompareResult) iRes);
}


//--------------------- FILTERING HELPERS ---------------------------//
//hash set containing strings, doubles
WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, TSetOfStrings);
WX_DECLARE_HASH_SET(double, wxDoubleHash, wxDoubleEqual, TSetOfDoubles);
WX_DECLARE_HASH_SET(long, wxIntegerHash, wxIntegerEqual, TSetOfLongs);
WX_DECLARE_HASH_SET(int, wxIntegerHash, wxIntegerEqual, TSetOfBools);
WX_DECLARE_HASH_SET(unsigned long, wxIntegerHash, wxIntegerEqual, TSetOfULongs);
WX_DECLARE_HASH_SET(wxLongLong, wxLongLongHash, wxLongLongEqual, TSetOfLongLongs);
WX_DECLARE_HASH_SET(wxULongLong, wxLongLongHash, wxLongLongEqual, TSetOfULongLongs);
WX_DECLARE_HASH_SET(wxVariant, wxVariantHash, wxVariantEqual, TSetOfVariants);


template<typename TSet, typename T>
WX_VDV_INLINE void AddVariantToSet(TSet &rSet, const wxVariant &v)
{
    if (v.IsNull()) return;
    T tValue = wxGetVariantValueAs<T>(v);
    rSet.insert(tValue);
}

/** Helper methods for finding all the values of a field in the data model
  * It will fill a set
  * \param rSet        [output]: the set to fill
  * \param pDataModel  [input] : the data model to scan
  * \param uiField     [input] : the field to scan
  * \param pStateModel [input] : the state model. If NULL, all items are scanned
  *                              if Non-NULL, the children of collapsed items are ignored
  */
template<typename TSet, typename T>
void FillSetOfValues(TSet &rSet, wxVirtualIDataModel *pDataModel, size_t uiField,
                     wxVirtualIStateModel *pStateModel)
{
    wxVirtualItemID id = pDataModel->GetRootItem();

    //loop on all items, and store the data inside a set
    if (pStateModel)
    {
        id = pDataModel->NextItem(id, pStateModel, 1);
        while (id.IsOK())
        {
            wxVariant v = pDataModel->GetItemData(id, uiField, wxVirtualIDataModel::WX_ITEM_MAIN_DATA);
            AddVariantToSet<TSet, T>(rSet, v);

            id = pDataModel->NextItem(id, pStateModel, 1);
        }
    }
    else
    {
        wxStack<wxVirtualItemID> oStackOfIDs;
        oStackOfIDs.push(id);
        while (!oStackOfIDs.empty())
        {
            id = oStackOfIDs.top();
            oStackOfIDs.pop();

            wxVariant v = pDataModel->GetItemData(id, uiField, wxVirtualIDataModel::WX_ITEM_MAIN_DATA);
            AddVariantToSet<TSet, T>(rSet, v);

            size_t uiNbChildren = pDataModel->GetChildCount(id);
            size_t uiChild;
            for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
            {
                wxVirtualItemID idChild = pDataModel->GetChild(id, uiChild);
                oStackOfIDs.push(idChild);
            }
        }
    }
}

/** Get all the values of a field
  * \param rvArray     [output]: an array of T. Previous content is lost
  *                              It contains the list of all values
  *                              Each value should be represented only once
  * \param pDataModel  [input] : the data model to scan
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
template<typename TArray, typename TSet, typename T>
void DoGetAllValues(TArray &rvArray, wxVirtualIDataModel *pDataModel, size_t uiField,
                    wxVirtualIStateModel *pStateModel)
{
    //loop on all items, and store the data inside a set
    TSet oSetOfValues;
    FillSetOfValues<TSet, T>(oSetOfValues, pDataModel, uiField, pStateModel);

    //convert the set to an array
    rvArray.Clear();
    rvArray.reserve(oSetOfValues.size());
    typename TSet::iterator it      = oSetOfValues.begin();
    typename TSet::iterator itEnd   = oSetOfValues.end();
    while(it != itEnd)
    {
        rvArray.push_back(*it);
        ++it;
    }
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
void wxVirtualIDataModel::GetAllValues(wxArrayString &rvStrings, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayString, TSetOfStrings, wxString>(rvStrings, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayBool &rvBools, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayBool, TSetOfBools, bool>(rvBools, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayShort &rvShorts, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayShort, TSetOfLongs, short>(rvShorts, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayInt &rvInts, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayInt, TSetOfLongs, int>(rvInts, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayLong &rvLongs, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayLong, TSetOfLongs, long>(rvLongs, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayLongLong &rvLongLongs, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayLongLong, TSetOfLongLongs, wxLongLong>(rvLongLongs, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayUShort &rvUShorts, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayUShort, TSetOfULongs, unsigned short>(rvUShorts, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayUInt &rvUInts, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayUInt, TSetOfULongs, unsigned int>(rvUInts, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayULong &rvULongs, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayULong, TSetOfULongs, unsigned long>(rvULongs, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayULongLong &rvULongLongs, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayULongLong, TSetOfULongLongs, wxULongLong>(rvULongLongs, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayFloat &rvFloats, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayFloat, TSetOfDoubles, double>(rvFloats, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxArrayDouble &rvDoubles, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    DoGetAllValues<wxArrayDouble, TSetOfDoubles, double>(rvDoubles, this, uiField, pStateModel);
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
void wxVirtualIDataModel::GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                       wxVirtualIStateModel *pStateModel)
{
    //loop on all items, and store the data inside a set
    TSetOfVariants oSetOfValues;
    FillSetOfValues<TSetOfVariants, wxVariant>(oSetOfValues, this, uiField, pStateModel);

    //convert the set to an array
    rvVariants.clear(); //just because of the lower case "clear" instead of "Clear", we cannot call DoGetAllValues<>(). Grrrr
    rvVariants.reserve(oSetOfValues.size());
    TSetOfVariants::iterator it      = oSetOfValues.begin();
    TSetOfVariants::iterator itEnd   = oSetOfValues.end();
    while(it != itEnd)
    {
        rvVariants.push_back(*it);
        ++it;
    }
}

//------------------------ PROXY DATA MODEL -------------------------//
/** Return true if the model is a proxy data model
  * \return true for proxy model, false for base model
  */
bool wxVirtualIDataModel::IsProxyDataModel(void) const
{
    return(false);
}

/** Get the parent model
  * \return the parent model. May be NULL if it is already the top-most model
  */
wxVirtualIDataModel* wxVirtualIDataModel::GetParentModel(void)
{
    return(m_pParentModel);
}

/** Set parent model
  * \param pParentModel [input]: the new parent model. Ownership NOT taken
  */
void wxVirtualIDataModel::SetParentModel(wxVirtualIDataModel *pParentModel)
{
    m_pParentModel = pParentModel;
}

/** Get top model (the top-most proxy one)
  * \return the top most model. It will never be NULL
  */
wxVirtualIDataModel* wxVirtualIDataModel::GetTopModel(void)
{
    wxVirtualIDataModel *pDataModel = this;
    while(pDataModel->m_pParentModel)
    {
        pDataModel = pDataModel->m_pParentModel;
    }
    return(pDataModel);
}

/** Get last model (the real one)
  * \return the bottom model (the one after all the proxy)
  */
wxVirtualIDataModel* wxVirtualIDataModel::GetBottomModel(void)
{
    wxVirtualIDataModel *pDataModel = this;
    while(pDataModel->IsProxyDataModel())
    {
        wxVirtualIProxyDataModel *pProxyModel = reinterpret_cast<wxVirtualIProxyDataModel*>(pDataModel);
        wxVirtualIDataModel *pMasterModel = pProxyModel->GetMasterModel();
        if (!pMasterModel) break;
        pDataModel = pMasterModel;
    }
    return(pDataModel);
}

//-------------------- INDEX CREATION METHODS -----------------------//
/** Create an item ID from a void pointer
  * \param pID   [input]: the ID of the model item ID
  * \return the model item ID
  */
wxVirtualItemID wxVirtualIDataModel::CreateItemID(void *pID) const
{
    wxVirtualItemID id;
    id.SetClientData(pID);
    id.SetModel(const_cast<wxVirtualIDataModel*>(this));
    return(id);
}

/** Create an invalid item ID
  * \return an invalid item ID
  */
wxVirtualItemID wxVirtualIDataModel::CreateInvalidItemID(void) const
{
    wxVirtualItemID id;
    id.SetClientData(WX_VDV_NULL_PTR);
    id.SetModel(WX_VDV_NULL_PTR);
    return(id);
}
