/**********************************************************************/
/** FILE    : VirtualDataFilterModel.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : list model inside a vector of values, for filters UI   **/
/**********************************************************************/


#ifndef WX_VIRTUAL_FILTER_DATA_MODEL_H_
#define WX_VIRTUAL_FILTER_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIListDataModel.h>
#include <wx/vector.h>
#include <wx/variant.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewFilter;

/** \class wxVirtualDataFilterModel : a list model, storing all its data inside a flat
  * array of variants
  */
class WXDLLIMPEXP_VDV wxVirtualDataFilterModel : public wxVirtualIListDataModel
{
    public:
        //constructors & destructor
        wxVirtualDataFilterModel(void);                                                     ///< \brief default constructor
        virtual ~wxVirtualDataFilterModel(void);                                            ///< \brief destructor

        //new interface
        virtual size_t          GetItemCount(void);                                         ///< \brief get amount of items//item ID methods
        virtual wxVirtualItemID GetItemID(size_t uiRowIndex) const;                         ///< \brief compute an item ID from a row index
        virtual size_t          GetRowIndex(const wxVirtualItemID &rID) const;              ///< \brief compute the row index from the item ID

        //interface: class type
        virtual int GetModelClass(void) const;                                              ///< \brief get the model class flags

        //new interface : item data
        virtual wxVariant GetListItemData(size_t uiItemID,
                                      size_t uiField = 0,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief get the item data
        virtual wxVirtualDataViewItemAttr* GetListItemAttribute(size_t uiItemID,
                                                            size_t uiField,
                                              const wxVirtualDataViewItemState &rState);    ///< \brief get the item graphic attributes


        //array of values
        wxVector<wxVariant>&        Values(void);                                           ///< \brief direct access to the array of values
        const wxVector<wxVariant>&  Values(void) const;                                     ///< \brief direct access to the array of values - const version

        //sorting
        virtual bool CanSort(const TSortFilters &vSortFilters) const;                       ///< \brief check if the field can be sorted by the data model
        virtual void Sort(const TSortFilters &vSortFilters);                                ///< \brief sort the values
        void SortValues(ESorting eSorting = WX_E_SORT_ASCENDING);                           ///< \brief sort the values of field 0

        //filtering
        void ClearFilter(void);                                                             ///< \brief clear the filter
        void SetFilter(wxVirtualDataViewFilter *pFilter);                                   ///< \brief set & apply the filter
        void ApplyFilter(void);                                                             ///< \brief apply the current filter

        //blanks / non-blanks
        void SetHasBlanks(bool bHasBlanks);                                                 ///< \brief set if <blanks> item is present
        void SetHasNonBlanks(bool bHasNonBlanks);                                           ///< \brief set if <non-blanks> item is present
        bool HasBlanks(void) const;                                                         ///< \brief check if the <blanks> item is present
        bool HasNonBlanks(void) const;                                                      ///< \brief check if the <non-blanks> item is present
        wxVirtualItemID GetBlankItemID(void) const;                                         ///< \brief get the item ID for the <blanks> item
        wxVirtualItemID GetNonBlankItemID(void) const;                                      ///< \brief get the item ID for the <non-blanks> item

    private:
        //data
        bool                        m_bHasBlankItem;                                        ///< \brief true if the model has <blank> item
        bool                        m_bHasNonBlankItem;                                     ///< \brief true if the model has a <non-blank> item
        wxVector<wxVariant>         m_vValues;                                              ///< \brief list of values
        wxVirtualDataViewFilter*    m_pFilter;                                              ///< \brief current filter (not owned)
        wxVector<size_t>            m_vFilteredValues;                                      ///< \brief indices of filtered values
        wxVector<size_t>            m_vSortedValues;                                        ///< \brief indices of sorted values
        ESorting                    m_eCurrentSortOrder;                                    ///< \brief current sort order

        //methods
        size_t      GetRealItemIndex(size_t uiIndex) const;                                 ///< \brief get the real item index after sorting & filtering
};

#endif

