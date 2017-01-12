/**********************************************************************/
/** FILE    : VirtualDataFilterModel.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : list model inside a vector of values, for filters UI   **/
/**********************************************************************/

#include <wx/VirtualDataView/FiltersUI/VirtualDataFilterModel.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>

//------------------- CONSTRUCTORS & DESTRUCTOR ---------------------//
/** Default constructor
  */
wxVirtualDataFilterModel::wxVirtualDataFilterModel(void)
    : wxVirtualIListDataModel(),
      m_bHasBlankItem(true),
      m_bHasNonBlankItem(true),
      m_pFilter(WX_VDV_NULL_PTR),
      m_eCurrentSortOrder(WX_E_SORT_NOT_SORTING)
{
}

/** Destructor
  */
wxVirtualDataFilterModel::~wxVirtualDataFilterModel(void)
{
}

//---------------------- INTERFACE : CLASS TYPE ---------------------//
/** Get the model class flags
  * This is an alternative to RTTI and dynamic_cast
  * This allows to refuse some kind of models for specialization of wxVirtualDataViewCtrl
  * \return a set of flags EClassType
  */
int wxVirtualDataFilterModel::GetModelClass(void) const
{
    return(WX_MODEL_CLASS_BASE | WX_MODEL_CLASS_FILTER | WX_MODEL_CLASS_LIST);
}

//---------------- INTERFACE : ITEM IDS -----------------------------//
//HELPER: get real item index from an indirection array
// param uiIndex       [in/out]: on input, the indirect index (filtered & sorted)
//                               on output, the real index
// param vIndicesArray [input] : the indirection array (vIndicesArray[i] = real unfiltered index in m_vValues)
// param bHasBlank     [input] : true if the special item <blanks> is present
// param bHasNonBlank  [input] : true if the special item <non-blanks> is present
WX_VDV_INLINE void DoGetRealItemIndex(size_t &uiIndex, const wxVector<size_t> &vIndicesArray,
                                      bool bHasBlank, bool bHasNonBlank)
{
    size_t uiNbSpecials = 0;
    if (bHasBlank) uiNbSpecials++;
    if (bHasNonBlank) uiNbSpecials++;
    if (uiIndex >= uiNbSpecials)
    {
        uiIndex -= uiNbSpecials;
        if (uiIndex < vIndicesArray.size()) uiIndex = vIndicesArray[uiIndex];
        uiIndex += uiNbSpecials;
    }
}


/** Get the real item index after taking into account sorting & filtering
  * \param uiIndex [input]: the filtered & sorted item index
  * \return the real item index
  */
WX_VDV_INLINE size_t wxVirtualDataFilterModel::GetRealItemIndex(size_t uiIndex) const
{
    size_t uiUnfilteredRow = uiIndex;
    if (m_eCurrentSortOrder != WX_E_SORT_NOT_SORTING)
    {
        DoGetRealItemIndex(uiUnfilteredRow, m_vSortedValues, m_bHasBlankItem, m_bHasNonBlankItem);
    }
    else if (m_pFilter)
    {
        DoGetRealItemIndex(uiUnfilteredRow, m_vFilteredValues, m_bHasBlankItem, m_bHasNonBlankItem);
    }
    return(uiUnfilteredRow);
}

/** Compute an item ID from a row index
  * \param uiRowIndex [input]: the row index of the item
  * \return an item ID for this item
  */
wxVirtualItemID wxVirtualDataFilterModel::GetItemID(size_t uiRowIndex) const
{
    //normal list item ID:
    //  ID = row index + 1
    //  row = 0
    //  col = 0

    //indexed list item ID
    //  ID = real row index + 1 (index in m_vValues)
    //  row = filtered row index + 1 (index of visible row)
    //  col = 0
    size_t uiFilteredRow  = uiRowIndex + 1;
    size_t uiUnfilteredRow = GetRealItemIndex(uiRowIndex);

    wxVirtualItemID id(uiUnfilteredRow + 1, uiFilteredRow, 0, const_cast<wxVirtualDataFilterModel*>(this));
    id.SetChildIndex(uiRowIndex);
    return(id);
}

/** Compute the row index from the item ID
  * \param rID [input]: the item ID
  * \return the row index
  */
size_t wxVirtualDataFilterModel::GetRowIndex(const wxVirtualItemID &rID)const
{
    size_t uiRow = rID.GetRow() - 1;
    return(uiRow);
}

//------------ INTERFACE IMPLEMENTATION -----------------------------//
/** Get amount of items
  * \return the amount of items
  */
size_t wxVirtualDataFilterModel::GetItemCount(void)
{
    size_t uiSize = 0;
    if (m_pFilter) uiSize = m_vFilteredValues.size();
    else           uiSize = m_vValues.size();
    if (m_bHasBlankItem   ) uiSize++;
    if (m_bHasNonBlankItem) uiSize++;
    return(uiSize);
}

/** Get the item data
  * \param uiItemID [input]: index of the item to query
  * \param uiField [input]: the field of the data to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  */
wxVariant wxVirtualDataFilterModel::GetListItemData(size_t uiItemID,  size_t uiField, EDataType eType)
{
    //non data
    if (eType != WX_ITEM_MAIN_DATA) return(wxVariant());

    //wrong field
    if (uiField != 0) return(wxVariant());

    //special
    size_t uiID = uiItemID;
    if (m_bHasBlankItem)
    {
        if (uiID == 0) return("<blanks>");
        uiID--;
    }
    if (m_bHasNonBlankItem)
    {
        if (uiID == 0) return("<non-blanks>");
        uiID--;
    }

    //get value from array
    if (m_eCurrentSortOrder != WX_E_SORT_NOT_SORTING)
    {
        if (uiID >= m_vSortedValues.size()) return(wxVariant());
        uiID = m_vSortedValues[uiID];
    }
    else if (m_pFilter)
    {
        if (uiID >= m_vFilteredValues.size()) return(wxVariant());
        uiID = m_vFilteredValues[uiID];
    }
    if (uiID < m_vValues.size()) return(m_vValues[uiID]);
    return(wxVariant());
}

/** Get the item graphic attributes
  * \param rID [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param rState [input] : the state of the item
  * \return a pointer to the graphic attributes for this item. The data is owned by the model.
  *         returning a NULL Pointer is valid: the default attributes will be used
  */
wxVirtualDataViewItemAttr* wxVirtualDataFilterModel::GetListItemAttribute(size_t uiItemID, size_t uiField,
                                                                      const wxVirtualDataViewItemState &rState)
{
    return(WX_VDV_NULL_PTR);
}

//------------------------- ARRAY OF VALUES -------------------------//
/** Direct access to the array of values
  * \return a reference to the list of values
  */
wxVector<wxVariant>& wxVirtualDataFilterModel::Values(void)
{
    return(m_vValues);
}

/** Direct access to the array of values - const version
  * \return a const reference to the list of values
  */
const wxVector<wxVariant>&  wxVirtualDataFilterModel::Values(void) const
{
    return(m_vValues);
}

//----------------------- BLANKS / NON-BLANKS -----------------------//
/** Set if <blanks> item is present
  * \param bHasBlanks [input]: true for adding the item, false for removing it
  */
void wxVirtualDataFilterModel::SetHasBlanks(bool bHasBlanks)
{
    m_bHasBlankItem = bHasBlanks;
}

/** Set if <non-blanks> item is present
  * \param bHasNonBlanks [input]: true for adding the item, false for removing it
  */
void wxVirtualDataFilterModel::SetHasNonBlanks(bool bHasNonBlanks)
{
    m_bHasNonBlankItem = bHasNonBlanks;
}

/** Check if the <blanks> item is present
  * \return true if the item is present, false otherwise
  */
bool wxVirtualDataFilterModel::HasBlanks(void) const
{
    return(m_bHasBlankItem);
}

/** Check if the <non-blanks> item is present
  * \return true if the item is present, false otherwise
  */
bool wxVirtualDataFilterModel::HasNonBlanks(void) const
{
    return(m_bHasNonBlankItem);
}

/** Get the item ID for the <blanks> item
  * \return the item ID of the blank item, if present
  *         an invalid item ID otherwise
  */
wxVirtualItemID wxVirtualDataFilterModel::GetBlankItemID(void) const
{
    if (!m_bHasBlankItem) return(CreateInvalidItemID());
    return(GetItemID(0));
}

/** Get the item ID for the <non-blanks> item
  * \return the item ID of the non-blank item, if present
  *         an invalid item ID otherwise
  */
wxVirtualItemID wxVirtualDataFilterModel::GetNonBlankItemID(void) const
{
    if (!m_bHasNonBlankItem) return(CreateInvalidItemID());
    if (m_bHasBlankItem) return(GetItemID(1));
    return(GetItemID(0));
}

//------------------ SORTING ----------------------------------------//
/** Check if the field can be sorted by the data model
  * \param vSortFilters [input] : the sort filters to apply
  * \return true if the model can sort on his own, false otherwise
  */
bool wxVirtualDataFilterModel::CanSort(const TSortFilters &vSortFilters) const
{
    size_t uiNbSortFilters = vSortFilters.size();
    if (uiNbSortFilters != 1) return(false);
    if (vSortFilters[0].m_uiSortedField == 0) return(true);
    return(false);
}

class wxCompareFunctorBase
{
    public:
        wxCompareFunctorBase(wxVirtualDataFilterModel &rModel)
            : m_vValues(rModel.Values())
        {}

        bool IsLess(size_t i1, size_t i2) const
        {
            int iRes = IsVariantLessThan(m_vValues[i1], m_vValues[i2]);
            if (iRes < 0) return(i1 < i2);
            if (iRes > 0) return(true);
            return(false);
        }

    protected:
        //data
        const wxVector<wxVariant>   &m_vValues; //list of values
};

//functor for sorting values in ascending order
struct wxCompareFunctorAscending : public wxCompareFunctorBase
{
    wxCompareFunctorAscending(wxVirtualDataFilterModel &rModel)
        : wxCompareFunctorBase(rModel)
    {}

    bool operator()(const size_t &i1, const size_t &i2)
    {
        return(IsLess(i1, i2));
    }
};

//functor for sorting values in descending order
struct wxCompareFunctorDescending : public wxCompareFunctorBase
{
    wxCompareFunctorDescending(wxVirtualDataFilterModel &rModel)
        : wxCompareFunctorBase(rModel)
    {}

    bool operator()(const size_t &i1, const size_t &i2)
    {
        return(IsLess(i2, i1));
    }
};

/** Sort the values
  */
void wxVirtualDataFilterModel::Sort(const TSortFilters &vSortFilters)
{
    size_t uiNbSortFilters = vSortFilters.size();
    if (uiNbSortFilters != 1) return;

    const TSort &rSort = vSortFilters[0];
    if (rSort.m_uiSortedField != 0) return;

    //already sorted ?
    if (m_eCurrentSortOrder == rSort.m_uiSortedField) return;

    //not sorting (special case) ?
    m_vSortedValues.clear();
    if (rSort.m_eSortOrder == WX_E_SORT_NOT_SORTING)
    {
        m_eCurrentSortOrder = WX_E_SORT_NOT_SORTING;
        return;
    }

    //for speed reasons, we sort on filtered values
    size_t i, uiSize;
    if (m_pFilter)
    {
        uiSize = m_vFilteredValues.size();
        m_vSortedValues.reserve(uiSize);
        for(i=0;i<uiSize;i++)
        {
            m_vSortedValues.push_back(m_vFilteredValues[i]);
        }
    }
    else
    {
        uiSize = m_vValues.size();
        m_vSortedValues.reserve(uiSize);
        for(i=0;i<uiSize;i++)
        {
            m_vSortedValues.push_back(i);
        }
    }

    //actual sorting
    switch(rSort.m_eSortOrder)
    {
        case WX_E_SORT_ASCENDING:   {
                                        wxCompareFunctorAscending cmp(*this);
                                        std::sort(m_vSortedValues.begin(), m_vSortedValues.end(), cmp);
                                    }
                                    break;

        case WX_E_SORT_DESCENDING:  {
                                        wxCompareFunctorDescending cmp(*this);
                                        std::sort(m_vSortedValues.begin(), m_vSortedValues.end(), cmp);
                                    }
                                    break;

        default                 :   break;
    }
    m_eCurrentSortOrder = rSort.m_eSortOrder;
}

/** Sort the values of field 0
  * \param eSorting [input]: the sorting order for field 0
  */
void wxVirtualDataFilterModel::SortValues(ESorting eSorting)
{
    TSort oSort;
    oSort.m_eSortOrder = eSorting;
    oSort.m_uiSortedField = 0;

    TSortFilters vSortFilters;
    vSortFilters.push_back(oSort);
    Sort(vSortFilters);
}


//---------------- SIMPLE FILTERING ---------------------------------//
/** Clear the filter
  */
void wxVirtualDataFilterModel::ClearFilter(void)
{
    m_pFilter = WX_VDV_NULL_PTR;
    m_vFilteredValues.clear();

    //we need to resort the values
    if (m_eCurrentSortOrder != WX_E_SORT_NOT_SORTING)
    {
        ESorting eSort = m_eCurrentSortOrder;
        m_eCurrentSortOrder = WX_E_SORT_NOT_SORTING;
        SortValues(eSort);
    }
}

/** Set & apply the filter
  * \param pFilter [input] : the new filter to apply. Ownership is not taken
  */
void wxVirtualDataFilterModel::SetFilter(wxVirtualDataViewFilter *pFilter)
{
    m_pFilter = pFilter;
    ApplyFilter();
}

/** Apply the current filter
  */
void wxVirtualDataFilterModel::ApplyFilter(void)
{
    m_vFilteredValues.clear();
    if (!m_pFilter) return;

    m_vFilteredValues.reserve(m_vValues.size());

    size_t uiSize = m_vValues.size();
    size_t i;
    for(i=0;i<uiSize;i++)
    {
        const wxVariant &rValue = m_vValues[i];
        if (m_pFilter->AcceptValue(rValue))
        {
            m_vFilteredValues.push_back(i);
        }
    }

    //we need to resort the values
    if (m_eCurrentSortOrder != WX_E_SORT_NOT_SORTING)
    {
        ESorting eSort = m_eCurrentSortOrder;
        m_eCurrentSortOrder = WX_E_SORT_NOT_SORTING;
        SortValues(eSort);
    }
}
