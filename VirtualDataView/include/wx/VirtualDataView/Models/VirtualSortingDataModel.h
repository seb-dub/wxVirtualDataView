/**********************************************************************/
/** FILE    : VirtualSortingDataModel.h                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for sorting items                     **/
/**********************************************************************/


#ifndef WX_VIRTUAL_SORTING_DATA_MODEL_H_
#define WX_VIRTUAL_SORTING_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIArrayProxyDataModel.h>

/** \class wxVirtualSortingDataModel : data model which provides sorting
  * A master data model is provided, and this model acts as a proxy on it.
  *
  * There are two sorting algorithms:
  *     - a memory efficient one, but slow (it is slow because it makes many calls to GetItemData() : O(N log(N)) calls to this method)
  *     - a fast algorithm, but memory hungry (get all items to sort by calling GetItemData for each item/field to sort, then sort)
  *
  *  By default, the fast algorithm is used, because the amount of memory required is O(N), where N is the amount of children of a given item
  *  (and not the total amount of items in the model)
  *  In case the amount of items to sort is large, the slow algorithm is even slower because of the O(N.Log(N)) calls to GetItemData().
  *  On the other hand, the memory is used only during the sorting process. Once sorted, the memory is freed (and the results potentially cached)
  *
  * To get a custom comparison behaviour, reimplement the following methods in the BASE data model:
  *     virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
  *                                     const wxVirtualItemID &rID2,
  *                                     size_t uiField);                //compare 2 items
  *     virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
  *                                     const wxVariant &rValue1,
  *                                     const wxVirtualItemID &rID2,
  *                                     const wxVariant &rValue2);      //compare 2 items values
  */
class WXDLLIMPEXP_VDV wxVirtualSortingDataModel : public wxVirtualIArrayProxyDataModel
{
    public:
        //constructors & destructor
        wxVirtualSortingDataModel(void);                                                    ///< \brief default constructor
        virtual ~wxVirtualSortingDataModel(void);                                           ///< \brief destructor

        //interface : hierarchy
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID);           ///< \brief get the parent of the item
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of children

        //sorting
        void                ClearSortFilters(void);                                         ///< \brief clear all sorts filter
        void                PushSortFilter(size_t uiField, ESorting eSort);                 ///< \brief push sort filter
        void                PopSortFilter(size_t uiField);                                  ///< \brief pop sort filter
        void                PopSortFilter(void);                                            ///< \brief pop the last sort filter
        bool                IsSorting(size_t uiField) const;                                ///< \brief check if a field is sorting
        bool                IsSorting(void) const;                                          ///< \brief check if at least one field is sorting
        ESorting            GetSortFilter(size_t uiField) const;                            ///< \brief get the sort filter for a field
        TSortFilters&       SortFilters(void);                                              ///< \brief get all the sort filters
        const TSortFilters& SortFilters(void) const;                                        ///< \brief get all the sort filters - const version

        virtual bool CanSort(const TSortFilters &vSortFilters) const;                       ///< \brief check if the field can be sorted by the data model
        virtual void Sort(const TSortFilters &vSortFilters);                                ///< \brief sort the values
        virtual void ResetSorting(void);                                                    ///< \brief remove all sort filters

        //methods
        virtual void        SortItems(wxVirtualItemIDs &vIDs);                              ///< \brief sort all the items according to the filter

    protected:
        //data
        TSortFilters        m_vSortFilters;                                                 ///< \brief list of sort filters
        bool                m_bFastSorting;                                                 ///< \brief true for fast sorting, false for memory efficient sorting

        //interface
        virtual void            DoGetChildren(wxVirtualItemIDs &vChildren,
                                              const wxVirtualItemID &rIDParent);            ///< \brief get all children

        //methods
        virtual void FastSort(wxVirtualItemIDs &vIDs);                                      ///< \brief fast sorting of the items
        virtual void MemoryEfficientSort(wxVirtualItemIDs &vIDs);                           ///< \brief memory efficient sorting of the items


};

#endif

