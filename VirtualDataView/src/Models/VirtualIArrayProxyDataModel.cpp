/**********************************************************************/
/** FILE    : VirtualIArrayProxyDataModel.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model optimized for children stored in array**/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualIArrayProxyDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/stack.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualIArrayProxyDataModel::wxVirtualIArrayProxyDataModel(void)
    : wxVirtualIProxyDataModel(),
      m_CachedResult(1000),
      m_uiMinAmountOfChildrenForCaching(5),
      m_uiMaxAmountOfChildrenForCaching(size_t(-1))
{
}

/** Destructor
  */
wxVirtualIArrayProxyDataModel::~wxVirtualIArrayProxyDataModel(void)
{
}

//----------------------- CACHE -------------------------------------//
/** Clear the cache
  */
void wxVirtualIArrayProxyDataModel::ClearCache(void)
{
    m_CachedResult.ClearCache();
    m_vChildren.clear();
}

/** Set the cache size
  * \param uiMaxAmountOfItemsInCache [input]: max cache size.
  *                                           a size of 0 disables the cache
  */
void wxVirtualIArrayProxyDataModel::SetCacheSize(size_t uiMaxAmountOfItemsInCache)
{
    m_CachedResult.SetCacheSize(uiMaxAmountOfItemsInCache);
}

/** Get the cache size
  * \return the cache size (max amount of items to store in cache)
  */
size_t wxVirtualIArrayProxyDataModel::GetCacheSize(void) const
{
    return(m_CachedResult.GetCacheSize());
}

/** Get the minimal amount of children for caching the result
  * \return the minimal amount of children for caching the result
  *         Items with strictly less items than this value will not be cached.
  *         This avoids cache pollution with items which are very fast to compute.
  */
size_t wxVirtualIArrayProxyDataModel::MinAmountOfChildrenForCaching(void) const
{
    return(m_uiMinAmountOfChildrenForCaching);
}

/** Set the minimal amount of children for caching the result
  * \param uiMin [input]: the new minimum
  *         Items with strictly less items than this value will not be cached.
  *         This avoids cache pollution with items which are very fast to compute.
  */
void wxVirtualIArrayProxyDataModel::SetMinAmountOfChildrenForCaching(size_t uiMin)
{
    m_uiMinAmountOfChildrenForCaching = uiMin;
}

/** Get the maximal amount of children for caching the result
  * \return the maximal amount of children for caching the result
  *         Items with strictly more items than this value will not be cached.
  *         This avoids too large memory consumption
  */
size_t wxVirtualIArrayProxyDataModel::MaxAmountOfChildrenForCaching(void) const
{
    return(m_uiMaxAmountOfChildrenForCaching);
}

/** Set the maximal amount of children for caching the result
  * \param uiMax [input]: the new maximal
  *         Items with strictly more items than this value will not be cached.
  *         This avoids too large memory consumption
  */
void wxVirtualIArrayProxyDataModel::SetMaxAmountOfChildrenForCaching(size_t uiMax)
{
    m_uiMaxAmountOfChildrenForCaching = uiMax;
}

//------------------------------- GET CHILDREN ----------------------//
/** Get all children
  * \param rIDParent [input]: the parent item
  * \return a reference to the array containing the children
  */
const wxVirtualItemIDs& wxVirtualIArrayProxyDataModel::GetAllChildren(const wxVirtualItemID &rIDParent)
{
    //is it in cache ?
    if (m_CachedResult.HasKey(rIDParent))
    {
        return(m_CachedResult.GetValue(rIDParent));
    }

    //special case
    if (!m_pDataModel)
    {
        m_vChildren.clear();
        return(m_vChildren);
    }

    //get
    DoGetChildren(m_vChildren, rIDParent);

    //store in cache
    //size_t uiNbChildren = m_vChildren.size(); no, because for filtering models, the amount of children can be 0
    size_t uiNbChildren = m_pDataModel->GetChildCount(rIDParent);
    if (uiNbChildren < m_uiMinAmountOfChildrenForCaching) return(m_vChildren);
    if (uiNbChildren > m_uiMaxAmountOfChildrenForCaching) return(m_vChildren);
    m_CachedResult.Insert(rIDParent, m_vChildren);
    return(m_vChildren);
}

/** Get amount of children
  * \param rID [input]: the ID of the item to query
  * \return the amount of children to rID
  */
size_t wxVirtualIArrayProxyDataModel::GetChildrenCount(const wxVirtualItemID &rIDParent)
{
    const wxVirtualItemIDs& vChildren = GetAllChildren(rIDParent);
    return(vChildren.size());
}

//--------------------- INTERFACE: HIERARCHY ------------------------//
/** Get the amount of children
  * \param rID [input]: the ID of the item to query
  * \return the amount of children to rID
  */
size_t wxVirtualIArrayProxyDataModel::GetChildCount(const wxVirtualItemID &rID)
{
    return(GetChildrenCount(rID));
}

/** Get all children at once
  * \param vIDs      [output]: the list of children item. Previous content is erased
  * \param rIDParent [input] : the ID of the parent item
  * \return the amount of children in vIDs
  */
size_t wxVirtualIArrayProxyDataModel::GetAllChildren(wxVirtualItemIDs &vIDs,
                                                     const wxVirtualItemID &rIDParent)
{
    if (m_pDataModel)
    {
        vIDs = GetAllChildren(rIDParent);
    }
    else
    {
        vIDs.clear();
    }
    return(vIDs.size());
}

/** Get child item ID
  * \param rIDParent    [input]: the ID of parent item
  * \param uiChildIndex [input]: the index of the child to retrieve
  * \return the child item ID
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::GetChild(const wxVirtualItemID &rIDParent,
                                                        size_t uiChildIndex)
{
    const wxVirtualItemIDs &rvChildren = GetAllChildren(rIDParent);
    if (rvChildren.size() > uiChildIndex) return(rvChildren[uiChildIndex]);
    return(CreateInvalidItemID());
}

//---------------------------- INTERFACE : REIMPLEMENTATION ---------//
/** Return true if an item expander should be drawn for this item
  * \param rID [input]: the item to query
  * \return true if an expander should be drawn
  *         false otherwise
  *
  * The default implementation returns true when GetChildCount(rID) > 0
  */
bool wxVirtualIArrayProxyDataModel::HasExpander(const wxVirtualItemID &rID)
{
    if (!m_pDataModel) return(false);
    return(m_pDataModel->HasExpander(rID));
}

/** Get the index of a child. Used by wxVirtualTreePath::Set
  * \param rChild  [input]: the child item
  * \return the index of rChild, if found
  *         size_t(-1) if not found
  */
size_t wxVirtualIArrayProxyDataModel::GetChildIndex(const wxVirtualItemID &rChild)
{
    return(wxVirtualIDataModel::GetChildIndex(rChild));
}

/** Get the index of a child. Used by GetNextItem internally
  * \param rParent [input]: the parent item
  * \param rChild  [input]: the child item
  * \return the index of rChild, if found
  *         size_t(-1) if not found
  */
size_t wxVirtualIArrayProxyDataModel::GetChildIndex(const wxVirtualItemID &rParent,
                                                    const wxVirtualItemID &rChild)
{
    return(wxVirtualIDataModel::GetChildIndex(rParent, rChild));
}

/** Check if an item is the last child of a parent item
  * \param rChild [input]: the item to check
  * \return true if the item is the last child (GetChildIndex() == GetParent(rChild).GetChildCount() - 1)
  *         false otherwise
  */
bool wxVirtualIArrayProxyDataModel::IsLastChild(const wxVirtualItemID &rChild)
{
    wxVirtualItemID idParent            = GetParent(rChild);
    const wxVirtualItemIDs &rvChildren  = GetAllChildren(idParent);
    size_t uiNbChildren                 = rvChildren.size();
    if (uiNbChildren == 0) return(false);
    const wxVirtualItemID &rLastChildID = rvChildren[uiNbChildren - 1];
    if (rLastChildID == rChild) return(true);
    return(false);
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
size_t wxVirtualIArrayProxyDataModel::GetSubTreeSize(const wxVirtualItemID &rID,
                                                     wxVirtualIStateModel *pStateModel)
{
    return(wxVirtualIDataModel::GetSubTreeSize(rID, pStateModel));
}


/** Get next sibling
  * \param rID [input]: the item to query
  * \return the next sibling, if there is one
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::GetNextSibling(const wxVirtualItemID &rID)
{
    return(wxVirtualIDataModel::GetNextSibling(rID));
}

/** Get previous sibling
  * \param rID [input]: the item to query
  * \return the previous sibling, if there is one
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::GetPreviousSibling(const wxVirtualItemID &rID)
{
    return(wxVirtualIDataModel::GetPreviousSibling(rID));
}

/** Get the left-most deepest descendent in the sub-tree
  * \param rID [input]: the root of the sub-tree
  * \param pStateModel [input]: the state model
  *                             If NULL, all items are examined
  *                             If non-null, children of collapsed node are not scanned
  * \return the Item ID of the left-most descendent
  *         This is useful for finding the next item
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::GetFirstDescendent(const wxVirtualItemID &rID,
                                                                  wxVirtualIStateModel *pStateModel)
{
    return(wxVirtualIDataModel::GetFirstDescendent(rID, pStateModel));
}

/** Get the right-most deepest descendent in the sub-tree
  * \param rID [input]: the root of the sub-tree
  * \param pStateModel [input]: the state model
  *                             If NULL, all items are examined
  *                             If non-null, children of collapsed node are not scanned
  * \return the Item ID of the right-most descendent
  *         This is useful for finding the previous item
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::GetLastDescendent(const wxVirtualItemID &rID,
                                                                 wxVirtualIStateModel *pStateModel)
{
    return(wxVirtualIDataModel::GetLastDescendent(rID, pStateModel));
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
wxVirtualItemID wxVirtualIArrayProxyDataModel::FindItem(size_t uiRank,
                                                        wxVirtualIStateModel *pStateModel,
                                                        const wxVirtualItemID &rStart,
                                                        size_t uiStartRank)
{
    return(wxVirtualIDataModel::FindItem(uiRank, pStateModel, rStart, uiStartRank));
}

/** Find the rank of an item
  * \param rID         [input]: the item to query
  * \param pStateModel [input]: the state model to use
  *                             if NULL, all items are considered to be expanded
  * \return the rank of the item. Root item has rank 0
  */
size_t wxVirtualIArrayProxyDataModel::GetItemRank(const wxVirtualItemID &rID,
                                                  wxVirtualIStateModel *pStateModel)
{
    return(wxVirtualIDataModel::GetItemRank(rID, pStateModel));
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
wxVirtualItemID wxVirtualIArrayProxyDataModel::NextItem(const wxVirtualItemID &rID,
                                                        size_t uiLevel, size_t &uiNextItemLevel,
                                                        wxVirtualIStateModel *pStateModel,
                                                        size_t uiNbItems)
{
    return(wxVirtualIDataModel::NextItem(rID, uiLevel, uiNextItemLevel, pStateModel, uiNbItems));
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
wxVirtualItemID wxVirtualIArrayProxyDataModel::NextItem(const wxVirtualItemID &rID,
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
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::PrevItem(const wxVirtualItemID &rID,
                                                        size_t uiLevel, size_t &uiPrevItemLevel,
                                                        wxVirtualIStateModel *pStateModel,
                                                        size_t uiNbItems)
{
    return(wxVirtualIDataModel::PrevItem(rID, uiLevel, uiPrevItemLevel, pStateModel, uiNbItems));
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
  */
wxVirtualItemID wxVirtualIArrayProxyDataModel::PrevItem(const wxVirtualItemID &rID,
                                                        wxVirtualIStateModel *pStateModel,
                                                        size_t uiNbItems)
{
    size_t uiLevel = 1;
    return(PrevItem(rID, uiLevel, uiLevel, pStateModel, uiNbItems));
}
