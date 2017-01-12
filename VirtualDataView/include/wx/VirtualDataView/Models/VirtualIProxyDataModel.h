/**********************************************************************/
/** FILE    : VirtualIProxyDataModel.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for sorting / filtering items         **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_PROXY_DATA_MODEL_H_
#define WX_VIRTUAL_I_PROXY_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

/** \class wxVirtualIProxyDataModel : data model which provides sorting/filtering  on the items.
  * A master data model is provided, and this model acts as a proxy on it.
  * All the pure virtual methods are implemented using simple forwarding to the master model
  *
  * In addition, another variant of proxy data model is provided: wxVirtualIArrayProxyDataModel
  * In this proxy model, all methods are implemented or reimplemented, and optimized for array-based
  * operations. Request of the list of children is minimized (call counts to GetChildCount / GetChild / GetAllChildren
  * GetChildIndex / IsLastChild are minimized).
  * The child items are stored inside an array, and are further processed
  * This is useful for example for sorting where we need the access to all children at once
  * For speeding up the results, a cache system is implemented
  */
class WXDLLIMPEXP_VDV wxVirtualIProxyDataModel : public wxVirtualIDataModel
{
    public:
        //constructors & destructor
        wxVirtualIProxyDataModel(void);                                                     ///< \brief default constructor
        virtual ~wxVirtualIProxyDataModel(void);                                            ///< \brief destructor

        //master model
        wxVirtualIDataModel*    GetMasterModel(void);                                       ///< \brief get the master model
        void                    AttachMasterModel(wxVirtualIDataModel *pDataModel);         ///< \brief attach master model and takes ownership on it
        wxVirtualIDataModel*    DetachMasterModel(void);                                    ///< \brief detach the master model without deleting it

        //interface: class type
        virtual int GetModelClass(void) const;                                              ///< \brief get the model class flags

        //interface : hierarchy
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID);           ///< \brief get the parent item ID
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex);                              ///< \brief get child item ID
        virtual size_t          GetAllChildren(wxVirtualItemIDs &vIDs,
                                               const wxVirtualItemID &rIDParent);           ///< \brief get all children at once
        virtual size_t          GetFieldCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of fields in the item

        //interface : item data
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
        virtual wxVirtualItemID GetRootItem(void);                                          ///< \brief get the root item
        virtual bool            IsRootItem(const wxVirtualItemID &rID);                     ///< \brief check if the item is the root item
        virtual wxVirtualItemID GetLastItem(void);                                          ///< \brief get the last item
        virtual bool            HasExpander(const wxVirtualItemID &rID);                    ///< \brief return true if an item expander should be drawn for this item
        virtual bool            HasCheckbox(const wxVirtualItemID &rID);                    ///< \brief return true if an item has a checkbox next to the expander
        virtual bool            IsEditable(const wxVirtualItemID &rID);                     ///< \brief return true if an item is editable
        virtual bool            IsSelectable(const wxVirtualItemID &rID);                   ///< \brief return true if an item is selectable
        virtual bool            IsCheckable(const wxVirtualItemID &rID);                    ///< \brief return true if an item is checkable
        virtual bool            IsCheckable3DState(const wxVirtualItemID &rID);             ///< \brief return true if an item is checkable with 3 state
        virtual bool            IsDragSource(const wxVirtualItemID &rID);                   ///< \brief return true if an item is a drag source
        virtual bool            IsDropTarget(const wxVirtualItemID &rID);                   ///< \brief return true if an item is a drop target
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
        virtual bool IsMatching(wxVirtualItemID &rID, const wxVariant &rvValue,
                                bool bFullMatch, bool bCaseSensitive, bool bRegex,
                                size_t uiField = size_t(-1));                               ///< \brief main matching method for searching
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

        //sorting
        virtual bool CanSort(const TSortFilters &vSortFilters) const;                       ///< \brief check if the field can be sorted by the data model
        virtual void Sort(const TSortFilters &vSortFilters);                                ///< \brief sort the values
        virtual void ResetSorting(void);                                                    ///< \brief remove all sort filters

        //filtering - get all values as string
        virtual void GetAllValues(wxArrayString &rvStrings, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of strings

        //filtering - get all values as bool
        virtual void GetAllValues(wxArrayBool &rvBools, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of bool

        //filtering - get all values as signed integers
        virtual void GetAllValues(wxArrayShort &rvShorts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of short
        virtual void GetAllValues(wxArrayInt &rvInts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of int
        virtual void GetAllValues(wxArrayLong &rvLongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of long
        virtual void GetAllValues(wxArrayLongLong &rvLongLongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of longlong

        //filtering - get all values as unsigned integers
        virtual void GetAllValues(wxArrayUShort &rvUShorts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned short
        virtual void GetAllValues(wxArrayUInt &rvUInts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned int
        virtual void GetAllValues(wxArrayULong &rvULongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned long
        virtual void GetAllValues(wxArrayULongLong &rvULongLongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned longlong

        //filtering - get all values as floats/doubles
        virtual void GetAllValues(wxArrayFloat &rvFloats, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of floats
        virtual void GetAllValues(wxArrayDouble &rvDoubles, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of doubles

        //filtering - get all values as variants
        virtual void GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of variants

        //sorting comparison functors
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVirtualItemID &rID2,
                                        size_t uiField);                                    ///< \brief compare 2 items
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVariant &rValue1,
                                        const wxVirtualItemID &rID2,
                                        const wxVariant &rValue2);                          ///< \brief compare 2 items values

        //proxy data model
        virtual bool IsProxyDataModel(void) const;                                          ///< \brief return true if the model is a proxy data model

    protected:
        //data
        wxVirtualIDataModel*        m_pDataModel;                                           ///< \brief the model which is shadowed

};

#endif

