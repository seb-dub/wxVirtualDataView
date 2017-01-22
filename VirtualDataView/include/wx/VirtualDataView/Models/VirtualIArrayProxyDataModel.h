/**********************************************************************/
/** FILE    : VirtualIArrayProxyDataModel.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model optimized for children stored in array**/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_ARRAY_PROXY_DATA_MODEL_H_
#define WX_VIRTUAL_I_ARRAY_PROXY_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIProxyDataModel.h>
#include <wx/VirtualDataView/LRUCache/LRUCache.h>

/** \class wxVirtualIArrayProxyDataModel: all methods are implemented or reimplemented.
  *       The child items are stored inside an array, and are further processed
  *       This is useful for example for sorting where we need the access to all children at once
  *       For speeding up the results, a cache system is implemented
  *
  *       The cache system is configurable:
  *         - SetCacheSize gives the maximal cache size. As soon as the max cache size is reached,
  *           the least-used item is removed to make room (LRU cache : Least-Recently-Used)
  *           10000 is a good suggested value
  *         - SetMinAmountOfChildrenForCaching indicates that items with strictly less than N children will not be cached
  *           This avoids polluting the cache with items which can be recomputed quickly
  *           A suggested value is 5
  *         - SetMaxAmountOfChildrenForCaching indicates that items with strictly more than N children will not be cached.
  *           This avoids high memory consumption when many large items need to be cached.
  *           Set this limit only if memory is very scarce.
  *           A suggested value is size_t(-1) (no limit)
  */
class WXDLLIMPEXP_VDV wxVirtualIArrayProxyDataModel : public wxVirtualIProxyDataModel
{
    public:
        //constructors & destructor
        wxVirtualIArrayProxyDataModel(void);                                                ///< \brief default constructor
        virtual ~wxVirtualIArrayProxyDataModel(void);                                       ///< \brief destructor

        //interface : hierarchy
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID);           ///< \brief get the parent of the item
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex);                              ///< \brief get child item ID
        virtual size_t          GetAllChildren(wxVirtualItemIDs &vIDs,
                                               const wxVirtualItemID &rIDParent);           ///< \brief get all children at once

        //interface : reimplement all methods which are impacted by reordering/filtering
        //this is to avoid redirecting to the master model
        virtual bool            HasExpander(const wxVirtualItemID &rID);                    ///< \brief return true if an item expander should be drawn for this item
        virtual size_t          GetChildIndex(const wxVirtualItemID &rChild);               ///< \brief get the index of a child
        virtual size_t          GetChildIndex(const wxVirtualItemID &rParent,
                                              const wxVirtualItemID &rChild);               ///< \brief get the index of a child
        virtual bool            IsLastChild(const wxVirtualItemID &rChild);                 ///< \brief check if an item is the last child of a parent item
        virtual size_t          GetSubTreeSize(const wxVirtualItemID &rID,
                                               wxVirtualIStateModel *pStateModel);          ///< \brief compute the size of a sub-tree
        virtual wxVirtualItemID GetNextSibling(const wxVirtualItemID &rID);                 ///< \brief get next sibling
        virtual wxVirtualItemID GetPreviousSibling(const wxVirtualItemID &rID);             ///< \brief get previous sibling
        virtual wxVirtualItemID GetFirstDescendent(const wxVirtualItemID &rID,
                                                   wxVirtualIStateModel *pStateModel);      ///< \brief get the left-most deepest descendent in the sub-tree
        virtual wxVirtualItemID GetLastDescendent(const wxVirtualItemID &rID,
                                                  wxVirtualIStateModel *pStateModel);       /// \brief get the right-most deepest descendent in the sub-tree
        virtual wxVirtualItemID FindItem(size_t uiRank,
                                         wxVirtualIStateModel *pStateModel,
                                         const wxVirtualItemID &rStart = s_RootID,
                                         size_t uiStartRank = 0);                           ///< \brief find an item based on its rank
        virtual size_t          GetItemRank(const wxVirtualItemID &rID,
                                            wxVirtualIStateModel *pStateModel);             ///< \brief find the rank of an item
        virtual wxVirtualItemID NextItem(const wxVirtualItemID &rID,
                                         size_t uiLevel, size_t &uiNextItemLevel,
                                         wxVirtualIStateModel *pStateModel,
                                         size_t uiNbItems = 1);                             ///< \brief find the next item
        virtual wxVirtualItemID NextItem(const wxVirtualItemID &rID,
                                         wxVirtualIStateModel *pStateModel,
                                         size_t uiNbItems = 1);                             ///< \brief find the next item
        virtual wxVirtualItemID PrevItem(const wxVirtualItemID &rID,
                                         size_t uiLevel, size_t &uiPrevItemLevel,
                                         wxVirtualIStateModel *pStateModel,
                                         size_t uiNbItems = 1);                             ///< \brief find the previous item
        virtual wxVirtualItemID PrevItem(const wxVirtualItemID &rID,
                                         wxVirtualIStateModel *pStateModel,
                                         size_t uiNbItems = 1);                             ///< \brief find the previous item

        //cache
        void    ClearCache(void);                                                           ///< \brief clear the cached results
        void    SetCacheSize(size_t uiMaxAmountOfItemsInCache);                             ///< \brief set the cache size
        size_t  GetCacheSize(void) const;                                                   ///< \brief get the cache size
        size_t  MinAmountOfChildrenForCaching(void) const;                                  ///< \brief get the minimal amount of children for caching the result
        void    SetMinAmountOfChildrenForCaching(size_t uiMin);                             ///< \brief set the minimal amount of children for caching the result
        size_t  MaxAmountOfChildrenForCaching(void) const;                                  ///< \brief get the maximal amount of children for caching the result
        void    SetMaxAmountOfChildrenForCaching(size_t uiMin);                             ///< \brief set the maximal amount of children for caching the result

    protected:
        //typedef
        typedef LRUCache<wxVirtualItemID, wxVirtualItemIDs> TCachedResults;
        typedef LRUCache<wxVirtualItemID, size_t> TCachedChildrenIndices;

        //data
        wxVirtualItemIDs        m_vChildren;                                                ///< \brief temp array
        TCachedResults          m_CachedResult;                                             ///< \brief cached results
        size_t                  m_uiMinAmountOfChildrenForCaching;                          ///< \brief minimal amount of children for caching
        size_t                  m_uiMaxAmountOfChildrenForCaching;                          ///< \brief maximal amount of children for caching
        TCachedChildrenIndices  m_CachedChildrenIndices;                                    ///< \brief cached children indices

        //methods
        const wxVirtualItemIDs& GetAllChildren(const wxVirtualItemID &rIDParent);           ///< \brief get all children
        size_t                  GetChildrenCount(const wxVirtualItemID &rIDParent);         ///< \brief get the amount of children

        //new interface
        virtual void            DoGetChildren(wxVirtualItemIDs &vChildren,
                                              const wxVirtualItemID &rIDParent) = 0;        ///< \brief get all children
};

#endif

