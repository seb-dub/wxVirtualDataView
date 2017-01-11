/**********************************************************************/
/** FILE    : VirtualIDataModel.h                                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a data model                              **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_DATA_MODEL_H_
#define WX_VIRTUAL_I_DATA_MODEL_H_

#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/VirtualDataView/Types/ArrayDefs.h>
#include <wx/variant.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewItemAttr;
class WXDLLIMPEXP_VDV wxVirtualDataViewCellAttrProvider;
class WXDLLIMPEXP_VDV wxVirtualIStateModel;
class WXDLLIMPEXP_VDV wxVirtualDataViewItemState;



/** \class wxVirtualIDataModel : a class for providing item data and attributes.
  *
  * This class defines an interface for defining an item hierarchy.
  * It is used for getting / setting data of the items, and query items hierarchy (parent, children, next, previous, ...)
  *
  * As a rule, all algorithms which run in O(n) or O(log(n)) (or whatever is different from O(1)) has been moved inside
  * this class. Default reasonable implementations have been provided. It extra performance is needed, the methods can be reimplemented.
  * This allow to exploit (or workaround) special data layout
  *
  * <B> OVERVIEW: </B>
  * The model is exposed as a hierarchy of items.
  *     - each item is identified by a wxVirtualItemID (see this topic for a more detailed description)
  *     - top level item is represented by a special item returned by GetRootItem()
  *       It is not recommended to override this method, although possible.
  *       It returns a Root item, as described in the documentation of wxVirtualItemID
  *       If you override it, you must override IsRootItem() as well.
  *     - each item can have an arbitrary amount of children, but only 1 parent
  *     - each item has a specified amount of field (given by GetFieldCount)
  *       This value is interpreted as "columns" for tree list controls, for example.
  *       The special value "-1" indicates that the item act has a category, and has no sub-fields
  *       A value of "0" is not recommended, as renderers generally expect at least 1 field
  *     - an invalid item can be constructed using CreateInvalidItem (calls wxVirtualItemID default constructor)
  *     - each item field has associated data which can retrieved or modified using GetItemData and SetItemData respectively
  *       In addition, an unlimited amount of data categories can be defined.
  *       3 main categories are defined (see enum EDataType)
  *         - WX_ITEM_MAIN_DATA     : this is the main data, what will be displayed in the control
  *         - WX_ITEM_TOOLTIP_DATA  : tooltip data: displayed when mouse is hovering on the item field
  *         - WX_ITEM_HELP_DATA     : the help data (displayed when wxWindow::GetHelpText() method is called)
  *
  * <B> ITEM HIERARCHY: </B>
  * The hierarchy is a directed rooted tree :
  *     - each item has only 1 parent
  *     - each item has an unlimited number of children
  *     - there is one root item which has no parent
  *     The hierarchy can be queried using the methods:
  *         - GetParent()
  *         - GetChildCount()
  *         - GetChild()
  *         - GetAllChildren()
  *
  * <B> SORTING : </B>
  *     If the model can implement sorting efficiently, it is possible to expose it.
  *     In that case, the following methods need to be reimplemented:
  *     \code
  *         virtual bool CanSort(const TSortFilters &vSortFilters) const; //check if the field can be sorted by the data model
  *         virtual void Sort(const TSortFilters &vSortFilters);          //sort the values
  *         virtual void ResetSorting(void);                              //remove all sort filters
  *     \endcode
  *     The default implementations do nothing (CanSort() returns false, and Sort() does nothing. ResetSort() pass
  *     empty filters to Sort())
  *
  *     If these methods are not reimplemented, a proxy data model can be used. This is the default behaviour for wxVirtualDataViewCtrl,
  *     and it is how the built-in sorting is implemented. See also wxVirtualSortingDataModel
  *
  *     On the other hand, if CanSort() and Sort() are reimplemented, the wxVirtualDataViewCtrl use them.
  *
  *     When to reimplement these methods: when you can have very efficient sorting implemented. If you rely on temporary arrays
  *     for sorting the children, then use wxVirtualSortingDataModel: it uses efficient LRU caching for avoiding resorting too many arrays.
  *
  * <B> FILTERING : </B>
  *
  * <B> Subclassing: </B>
  *     - models implementation must implement the pure-virtual methods. There are only 3,
  *       and they define the item hierarchy
  *         ITEM HIERARCHY:
  *             GetParent()
  *             GetChildCount()
  *             GetChild()
  *             For maximal performance, try to set the "child index" in the returned wxVirtualItemID for GetParent() and GetChild()
  *             This will considerably improve speed of other algorithms. For GetChild(), it is straight-forward. For GetParent(), it can
  *             be tricky (O(n) time, with n = amount of siblings). If this cannot be done, consider reimplementing GetChildIndex().
  *
  *     - models should implement the following methods in most of the cases.
  *         ITEM DATA:
  *             GetFieldCount() : default implementation return(1), this is probably not the case for complex model (tree-list, tables, ...)
  *                               This is actually just a hint.
  *             GetItemData()   : default implementation return an invalid variant. Implement this one if you want to see something in the controls.
  *             GetItemAttribute() : implement this one if you want per item graphic attributes. Default return always NULL
  *
  *     - for read / write models, the following method should be implemented:
  *         ITEM DATA
  *             SetItemData()   : for modifying the data inside the model. Called by the controls
  *             GetItemFlags()  : for disabling edition on a sub-set of items
  *
  *     - for maximal performances, the following methods should be re-implemented (in order of importance):
  *         RANK/ITEM mapping
  *             GetChildIndex() : used in many methods. Reimplement if you can get better than O(k) (k == amount of children)
  *                               This is highly recommended, because it is used by NextItem & PrevItem, which can be called often
  *                               So the combination of the method NextItem/GetChildIndex is O(k*k). If k > 10000, there is trouble
  *                               If this cannot be implemented efficiently (ex: you need to loop on an array of children), then a better
  *                               strategy is to re-implement the next 4 methods: GetNextSibling, GetPrevSibling, NextItem, PrevItem
  *
  *                               Another possible way is to store the child index inside a field of wxVirtualItemID
  *                               (row / column, or high word of ID for example). In that case, you do not need to reimplement this.
  *             GetNextSibling()
  *             GetPrevSibling(): used in many methods. Reimplement if you can get better than O(k) (k == amount of siblings)
  *             NextItem()
  *             PrevItem()      : reimplement if your data is organized as a linked list of some sort
  *                               Otherwise, it is probably not worth the time
  *             FindItem()      : RANK to ITEM: default implementation use NextItem / PrevItem sequentially,
  *                               or GetSubTreeSize and GetChildCount. It is generally good enough
  *             GetItemRank()   : ITEM to RANK: default implementation use NextItem : O(n) time
  *             GetLastDescendent() : used by PrevItem(). Do not bother if PrevItem has been already reimplemented
  *
  *     - Finding / searching: The Find* methods have been implemented using PrevItem / NextItem and GetItemData
  *                            So searching is essentially O(N). Reimplement if you can get better performances
  *
  *     - filtering : the GetAllValues() is used for initializing filters. Default implementation loops on all
  *                   items and is essentially linear (O(N)).
  *
  */
class WXDLLIMPEXP_VDV wxVirtualIDataModel
{
    public:
        //static data
        static wxVirtualItemID s_RootID;

        //enums
        /// \enum EDataType : the kind of data to get
        enum EDataType
        {
            WX_ITEM_MAIN_DATA           = 0,                                                ///< \brief main data to retrieve
            WX_ITEM_TOOLTIP_DATA        = 1,                                                ///< \brief the tool-tip data
            WX_ITEM_HELP_DATA           = 2                                                 ///< \brief the help data
        };

        /// \enum EFlags : list of features that an item support
        enum EFlags
        {
            WX_ITEM_FLAGS_NONE          = 0x00,                                             ///< \brief nothing supported
            WX_ITEM_FLAGS_SELECTABLE    = 0x01,                                             ///< \brief item can be selected
            WX_ITEM_FLAGS_EDITABLE      = 0x02,                                             ///< \brief item can edited
            WX_ITEM_FLAGS_CHECKABLE     = 0x04,                                             ///< \brief item can be checked
            WX_ITEM_FLAGS_HAS_TRISTATE  = 0x08,                                             ///< \brief item has a 3-state checked/unchecked/unknown
            WX_ITEM_FLAGS_DRAGGABLE     = 0x10,                                             ///< \brief item can be source of drag/drop operation
            WX_ITEM_FLAGS_DROPPABLE     = 0x20,                                             ///< \brief item can be destination of drag/drop operation
            WX_ITEM_FLAGS_ALL           = 0xFF                                              ///< \brief item supports everything
        };

        //constructors & destructor
        wxVirtualIDataModel(void);                                                          ///< \brief default constructor
        virtual ~wxVirtualIDataModel(void);                                                 ///< \brief destructor

        //interface: class type
        /// \enum EClassType gives the kind of model it is. Avoids RTTI flags and dynamic_cast
        enum EClassType
        {
            WX_MODEL_CLASS_BASE     = 0x00,                                                 ///< \brief base data model
            WX_MODEL_CLASS_LIST     = 0x01,                                                 ///< \brief list data model
            WX_MODEL_CLASS_FILTER   = 0x02,                                                 ///< \brief filter data model
            WX_MODEL_CLASS_PROXY    = 0x04                                                  ///< \brief proxy data model
        };
        virtual int GetModelClass(void) const;                                              ///< \brief get the model class flags

        //interface : hierarchy
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID)    = 0;    ///< \brief get the parent item ID
        virtual size_t          GetChildCount(const wxVirtualItemID &rID)           = 0;    ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex)                       = 0;    ///< \brief get child item ID
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
        /// \enum ESorting : the kind of sorting
        enum ESorting
        {
            WX_E_SORT_ASCENDING         = 0,                                                ///< \brief sort in ascending order
            WX_E_SORT_DESCENDING        = 1,                                                ///< \brief sort in descending order
            WX_E_SORT_NOT_SORTING       = 2                                                 ///< \brief not sorting the data
        };

        struct TSort
        {
            size_t      m_uiSortedField;                                                    ///< \brief the sorted field
            ESorting    m_eSortOrder;                                                       ///< \brief the sorting order for this field

            TSort(size_t uiField = 0, ESorting eSorting = WX_E_SORT_NOT_SORTING);           ///< \brief default constructor
        };
        typedef wxVector<TSort>  TSortFilters;                                              ///< \brief array of sort filters

        virtual bool CanSort(const TSortFilters &vSortFilters) const;                       ///< \brief check if the field can be sorted by the data model
        virtual void Sort(const TSortFilters &vSortFilters);                                ///< \brief sort the values
        virtual void ResetSorting(void);                                                    ///< \brief remove all sort filters

        //sorting comparison functors
        /// \enum ECompareResult : comparison result between 2 values
        enum ECompareResult
        {
            WX_E_LESS_THAN      = -1,                                                       ///< \brief value1 < value2
            WX_E_EQUAL          =  0,                                                       ///< \brief value1 == value2
            WX_E_GREATER_THAN   =  1                                                        ///< \brief value1 > value2
        };
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVirtualItemID &rID2,
                                        size_t uiField);                                    ///< \brief compare 2 items
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVariant &rValue1,
                                        const wxVirtualItemID &rID2,
                                        const wxVariant &rValue2);                          ///< \brief compare 2 items values

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

        //proxy data model
        virtual bool IsProxyDataModel(void) const;                                          ///< \brief return true if the model is a proxy data model
        virtual wxVirtualIDataModel* GetParentModel(void);                                  ///< \brief get parent model
        virtual void                 SetParentModel(wxVirtualIDataModel *pParentModel);     ///< \brief set parent model
        virtual wxVirtualIDataModel* GetTopModel(void);                                     ///< \brief get top model (the top-most proxy one)
        virtual wxVirtualIDataModel* GetBottomModel(void);                                  ///< \brief get last model (the real one)

    protected:
        //data
        wxVirtualIDataModel*        m_pParentModel;                                         ///< \brief parent data model

        //methods
        wxVirtualItemID CreateItemID(void *pID) const;                                      ///< \brief create an item ID from a void pointer
        wxVirtualItemID CreateInvalidItemID(void) const;                                    ///< \brief create an invalid item ID
};

#endif

