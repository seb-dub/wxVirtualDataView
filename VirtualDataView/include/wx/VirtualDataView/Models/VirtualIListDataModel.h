/**********************************************************************/
/** FILE    : VirtualIListDataModel.h                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a list data model                         **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_LIST_DATA_MODEL_H_
#define WX_VIRTUAL_I_LIST_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIDataModel.h>


/** \class wxVirtualIListDataModel : a partial model implementation. Helps for list models
  * The model is optimized for a flat list, namely:
  *     - there is no hierarchy (the items have no children)
  *     - the items can be accessed by a unique index from 0 to GetItemCount() - 1
  *
  * If the 2nd condition cannot be fulfilled (or if it is not desirable), then derive a model directly from wxVirtualIDataModel
  */
class WXDLLIMPEXP_VDV wxVirtualIListDataModel : public wxVirtualIDataModel
{
    public:
        //constructors & destructor
        wxVirtualIListDataModel(void);                                                      ///< \brief default constructor
        virtual ~wxVirtualIListDataModel(void);                                             ///< \brief destructor

        //item ID methods
        virtual wxVirtualItemID GetItemID(size_t uiRowIndex) const;                         ///< \brief compute an item ID from a row index
        virtual size_t          GetRowIndex(const wxVirtualItemID &rID) const;              ///< \brief compute the row index from the item ID

        //interface: class type
        virtual int GetModelClass(void) const;                                              ///< \brief get the model class flags

        //new interface
        virtual size_t GetItemCount(void) = 0;                                              ///< \brief get amount of items

        //new interface : item data
        virtual wxVariant GetListItemData(size_t uiItemID,
                                      size_t uiField = 0,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief get the item data
        virtual wxVirtualDataViewItemAttr* GetListItemAttribute(size_t uiItemID,
                                                            size_t uiField,
                                              const wxVirtualDataViewItemState &rState);    ///< \brief get the item graphic attributes
        virtual EFlags    GetListItemFlags(size_t uiItemID, size_t uiField);                ///< \brief get the item flags
        virtual bool      SetListItemData(size_t uiItemID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief set the item data

        //wxVirtualIDataModel core interface implementation : hierarchy
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID);           ///< \brief get the parent item ID
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex);                              ///< \brief get child item ID
        virtual size_t          GetAllChildren(wxVirtualItemIDs &vIDs,
                                               const wxVirtualItemID &rIDParent);           ///< \brief get all children at once
        virtual size_t          GetFieldCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of fields in the item

        //wxVirtualIDataModel core interface implementation: item data
        virtual wxVariant GetItemData(const wxVirtualItemID &rID,
                                      size_t uiField = 0,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief get the item data
        virtual wxVirtualDataViewItemAttr* GetItemAttribute(const wxVirtualItemID &rID,
                                                            size_t uiField,
                                              const wxVirtualDataViewItemState &rState);    ///< \brief get the item graphic attributes
        virtual EFlags    GetItemFlags(const wxVirtualItemID &rID, size_t uiField);         ///< \brief get the item flags
        virtual bool      SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief set the item data

        //interface with common implementation
        virtual wxVirtualItemID GetLastItem(void);                                          ///< \brief get the last item
        virtual size_t          GetChildIndex(const wxVirtualItemID &rChild);               ///< \brief get the index of a child
        virtual size_t          GetChildIndex(const wxVirtualItemID &rParent,
                                              const wxVirtualItemID &rChild);               ///< \brief get the index of a child
        virtual bool            IsLastChild(const wxVirtualItemID &rChild);                 ///< \brief check if an item is the last child of a parent item
        virtual size_t          GetDepth(const wxVirtualItemID &rID);                       ///< \brief get the depth (or level) of an item in the tree
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
        virtual bool IsInSubTree(const wxVirtualItemID &rID,
                                 const wxVirtualItemID &rSubTreeRootID);                    ///< \brief check if rID is inside a sub-tree

        //searching
        virtual wxVirtualItemID FindFirst(const wxVariant &rvValue,
                                          bool bFullMatch, bool bCaseSensitive,
                                          bool bRegEx,
                                          size_t uiField);                                  ///< \brief find the 1st item matching a criteria in 1 field
        virtual wxVirtualItemID FindNext(const wxVirtualItemID &rID,
                                         const wxVariant &rvValue,
                                         bool bFullMatch, bool bCaseSensitive,
                                         bool bRegEx,
                                         size_t uiField);                                   ///< \brief find the next item matching a criteria in 1 field
        virtual wxVirtualItemID FindPrev(const wxVirtualItemID &rID,
                                         const wxVariant &rvValue,
                                         bool bFullMatch, bool bCaseSensitive,
                                         bool bRegEx,
                                         size_t uiField);                                   ///< \brief find the previous item matching a criteria in 1 field
        virtual wxVirtualItemID FindFirst(const wxVariant &rvValue,
                                          bool bFullMatch, bool bCaseSensitive,
                                          bool bRegEx);                                     ///< \brief find the 1st item matching a criteria in all fields
        virtual wxVirtualItemID FindNext(const wxVirtualItemID &rID,
                                         const wxVariant &rvValue,
                                         bool bFullMatch, bool bCaseSensitive,
                                         bool bRegEx);                                      ///< \brief find the next item matching a criteria in all fields
        virtual wxVirtualItemID FindPrev(const wxVirtualItemID &rID,
                                         const wxVariant &rvValue,
                                         bool bFullMatch, bool bCaseSensitive,
                                         bool bRegEx);                                      ///< \brief find the previous item matching a criteria in all fields

    private:
        //methods
        wxVirtualItemID DoGetItemID(size_t uiRowIndex) const;                               ///< \brief compute an item ID from a row index
        size_t          DoGetRowIndex(const wxVirtualItemID &rID) const;                    ///< \brief compute the row index from the item ID
};

#endif

