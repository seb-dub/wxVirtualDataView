/**********************************************************************/
/** FILE    : VirtualSortingDataModel.cpp                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for sorting items                     **/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualSortingDataModel.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>
//#include <wx/log.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualSortingDataModel::wxVirtualSortingDataModel(void)
    : wxVirtualIArrayProxyDataModel(),
      m_bFastSorting(true)
{
    SetMinAmountOfChildrenForCaching(5);
    SetCacheSize(10000);
}

/** Destructor
  */
wxVirtualSortingDataModel::~wxVirtualSortingDataModel(void)
{
}

//-------------------- INTERFACE : HIERARCHY ------------------------//
/** Get amount of children
  * \param rID [input]: the ID of the item to query
  * \return the amount of children to rID
  */
size_t wxVirtualSortingDataModel::GetChildCount(const wxVirtualItemID &rIDParent)
{
    //the sorting model does not modify the amount of children
    //so bypass the proxy mechanism
    if (m_pDataModel) return(m_pDataModel->GetChildCount(rIDParent));
    return(0);
}

//---------------- INTERFACE : COMMON METHODS -----------------------//
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
size_t wxVirtualSortingDataModel::GetSubTreeSize(const wxVirtualItemID &rID,
                                           wxVirtualIStateModel *pStateModel)
{
    //the sub tree size will not be affected by sorting, so simply forward
    //to the potentially optimized model
    return(m_pDataModel->GetSubTreeSize(rID, pStateModel));
}

//------------ INTEFACE: SORTING ------------------------------------//
/** Check if the field can be sorted by the data model
  * \param vSortFilters [input]: the sort filters to apply
  * \return true if the model can sort the items according to the filters, false otherwise
  *         Note that true is returned only if ALL THE SORT FILTERS can be taken into account
  */
bool wxVirtualSortingDataModel::CanSort(const TSortFilters &vSortFilters) const
{
    return(true);
}

/** Sort the values
  * \param vSortFilters [input]: the sort filters to apply
  */
void wxVirtualSortingDataModel::Sort(const TSortFilters &vSortFilters)
{
    m_vSortFilters = vSortFilters;
    ClearCache();
}

/** Remove all sort filters
  */
void wxVirtualSortingDataModel::ResetSorting(void)
{
    m_vSortFilters.clear();
    ClearCache();
}

//---------------------- SORTING ------------------------------------//
/** Clear all sorts filter
  */
void wxVirtualSortingDataModel::ClearSortFilters(void)
{
    m_vSortFilters.clear();
    ClearCache();
}

/** Push sort filter - it is put at the end of the filters list
  * \param uiField [input]: the field to sort
  * \param eSort   [input]: the sorting order
  * If the field was already in the list, it will be removed first
  */
void wxVirtualSortingDataModel::PushSortFilter(size_t uiField, ESorting eSort)
{
    PopSortFilter(uiField);
    switch(eSort)
    {
        case WX_E_SORT_ASCENDING    : ClearCache(); break;
        case WX_E_SORT_DESCENDING   : ClearCache(); break;
        case WX_E_SORT_NOT_SORTING  :
        default                     : return;
    }

    m_vSortFilters.push_back(TSort(uiField, eSort));
}

/** Pop sort filter
  * \param uiField [input]: the field to remove
  */
void wxVirtualSortingDataModel::PopSortFilter(size_t uiField)
{
    TSortFilters::iterator it    = m_vSortFilters.begin();
    TSortFilters::iterator itEnd = m_vSortFilters.end();
    while (it != itEnd)
    {
        TSort &tSort = *it;
        if (tSort.m_uiSortedField == uiField)
        {
            ClearCache();
            m_vSortFilters.erase(it);
            it    = m_vSortFilters.begin();
            itEnd = m_vSortFilters.end();
        }
        else
        {
            ++it;
        }
    }
}

/** Pop the last sort filter
  */
void wxVirtualSortingDataModel::PopSortFilter(void)
{
    m_vSortFilters.pop_back();
    ClearCache();
}

/** Check if a field is sorting
  * \param uiField [input]: the field to check
  * \return true if the field is sorted either ascending or descending
  */
bool wxVirtualSortingDataModel::IsSorting(size_t uiField) const
{
    TSortFilters::const_iterator it    = m_vSortFilters.begin();
    TSortFilters::const_iterator itEnd = m_vSortFilters.end();
    while (it != itEnd)
    {
        const TSort &rSort = *it;
        if (rSort.m_uiSortedField == uiField)
        {
            switch(rSort.m_eSortOrder)
            {
                case WX_E_SORT_ASCENDING    : return(true);
                case WX_E_SORT_DESCENDING   : return(true);
                case WX_E_SORT_NOT_SORTING  : break;
                default                     : break;
            }
        }

        ++it;
    }
    return(false);
}

/** Check if at least one field is sorting
  * \return true if 1 or more fields are sorted either ascending or descending
  */
bool wxVirtualSortingDataModel::IsSorting(void) const
{
    TSortFilters::const_iterator it    = m_vSortFilters.begin();
    TSortFilters::const_iterator itEnd = m_vSortFilters.end();
    while (it != itEnd)
    {
        const TSort &rSort = *it;
        switch(rSort.m_eSortOrder)
        {
            case WX_E_SORT_ASCENDING    : return(true);
            case WX_E_SORT_DESCENDING   : return(true);
            case WX_E_SORT_NOT_SORTING  : break;
            default                     : break;
        }

        ++it;
    }
    return(false);
}

/** Get the sort filter for a field
  * \param uiField [input]: the field to query
  * \return the sorting order for this field
  */
wxVirtualSortingDataModel::ESorting wxVirtualSortingDataModel::GetSortFilter(size_t uiField) const
{
    TSortFilters::const_iterator it    = m_vSortFilters.begin();
    TSortFilters::const_iterator itEnd = m_vSortFilters.end();
    while (it != itEnd)
    {
        const TSort &rSort = *it;
        if (rSort.m_uiSortedField == uiField) return(rSort.m_eSortOrder);
        ++it;
    }
    return(WX_E_SORT_NOT_SORTING);
}

/** Get all the sort filters
  * \return a reference to all sort filters
  */
wxVirtualSortingDataModel::TSortFilters& wxVirtualSortingDataModel::SortFilters(void)
{
    ClearCache();
    return(m_vSortFilters);
}

/** Get all the sort filters - const version
  * \return a const reference to all sort filters
  */
const wxVirtualSortingDataModel::TSortFilters& wxVirtualSortingDataModel::SortFilters(void) const
{
    return(m_vSortFilters);
}

//---------------------- SLOW SORTING (MEMORY EFFICIENT) ------------//
class TCompareFunctor
{
    public:
        TCompareFunctor(const wxVirtualSortingDataModel::TSortFilters &rSortFilters,
                        wxVirtualIDataModel *pDataModel)
            : m_rSortFilters(rSortFilters),
              m_pDataModel(pDataModel)
        {}

        bool operator()(const wxVirtualItemID &rID1, const wxVirtualItemID &rID2)
        {
            wxVirtualIDataModel::TSortFilters::const_iterator it    = m_rSortFilters.begin();
            wxVirtualIDataModel::TSortFilters::const_iterator itEnd = m_rSortFilters.end();
            while(it != itEnd)
            {
                const wxVirtualIDataModel::TSort &rSort = *it;

                wxVirtualIDataModel::ECompareResult eResult;
                switch(rSort.m_eSortOrder)
                {
                    case wxVirtualIDataModel::WX_E_SORT_ASCENDING :
                        eResult = m_pDataModel->Compare(rID1, rID2, rSort.m_uiSortedField);
                        break;
                    case wxVirtualIDataModel::WX_E_SORT_DESCENDING:
                        eResult = m_pDataModel->Compare(rID2, rID1, rSort.m_uiSortedField);
                        break;
                    case wxVirtualIDataModel::WX_E_SORT_NOT_SORTING:
                    default                                        :
                        eResult = wxVirtualIDataModel::WX_E_EQUAL;
                        break;
                }

                switch(eResult)
                {
                    case wxVirtualIDataModel::WX_E_LESS_THAN    : return(true);
                    case wxVirtualIDataModel::WX_E_GREATER_THAN : return(false);
                    case wxVirtualIDataModel::WX_E_EQUAL        :
                    default                                     : break;
                }

                ++it;
            }
            return(false);
        }

    protected:
        const wxVirtualIDataModel::TSortFilters &m_rSortFilters;
        wxVirtualIDataModel*                     m_pDataModel;
};

/** Memory efficient sorting of the items
  * \param vIDs     [input / output]: on input, the array to sort
  *                                   on output, the sorted array
  */
void wxVirtualSortingDataModel::MemoryEfficientSort(wxVirtualItemIDs &vIDs)
{
    TCompareFunctor cmp(m_vSortFilters, m_pDataModel);
    std::sort(vIDs.begin(), vIDs.end(), cmp);

    size_t uiSize, i;
    uiSize = vIDs.size();
    for(i=0;i<uiSize;i++)
    {
        vIDs[i].SetChildIndex(i);
    }
}

//--------------------- FAST SORTING --------------------------------//
typedef wxVector<wxVariant> TVariantList;

struct TVariant
{
    TVariantList                                    m_Variant;
    wxVirtualItemID                                 m_id;

    TVariant(void)
    {
    }

    TVariant(const wxVirtualItemID &id)
        : m_id(id)
    {

    }

    void Clear(void)
    {
        m_Variant.clear();
    }

    void Reserve(size_t uiSize)
    {
        m_Variant.clear();
        m_Variant.reserve(uiSize);
    }

    void PushBack(const wxVariant &v)
    {
        m_Variant.push_back(GetStringValue(v));
    }
};

class wxComparisonFunctor
{
    public:
        wxComparisonFunctor(wxVirtualIDataModel *pDataModel, size_t uiSize,
                            wxVector<wxVirtualSortingDataModel::TSort> &vSortOrder)
            : m_pDataModel(pDataModel),
              m_uiSize(uiSize),
              m_SortOrder(vSortOrder)
        {}

        WX_VDV_INLINE bool operator()(const TVariant &v1, const TVariant &v2) const
        {
            size_t uiSize = m_uiSize;
            size_t i;

            for(i=0;i<uiSize;i++)
            {
                int iRes = wxVirtualSortingDataModel::WX_E_EQUAL;
                const wxVirtualSortingDataModel::TSort &rSort = m_SortOrder[i];
                switch(rSort.m_eSortOrder)
                {
                    case wxVirtualSortingDataModel::WX_E_SORT_ASCENDING  :
                        iRes = m_pDataModel->Compare(v1.m_id, v1.m_Variant[i],
                                                     v2.m_id, v2.m_Variant[i],
                                                     rSort.m_uiSortedField);
                        break;

                    case wxVirtualSortingDataModel::WX_E_SORT_DESCENDING :
                        iRes = m_pDataModel->Compare(v2.m_id, v2.m_Variant[i],
                                                     v1.m_id, v1.m_Variant[i],
                                                     rSort.m_uiSortedField);
                        break;

                    case wxVirtualSortingDataModel::WX_E_SORT_NOT_SORTING:
                    default                                              : continue;
                                                                           break;
                }

                switch(iRes)
                {
                    case wxVirtualSortingDataModel::WX_E_LESS_THAN   : return(true);
                    case wxVirtualSortingDataModel::WX_E_GREATER_THAN: return(false);
                    default                                          : break;
                }
            }

            return(false);
        }

    protected:
        //data
        wxVirtualIDataModel*                            m_pDataModel;       //BASE data model
        size_t                                          m_uiSize;           //amount of fields
        wxVector<wxVirtualSortingDataModel::TSort>      m_SortOrder;        //sort order
};

/** Fast sorting of the items
  * \param vIDs     [input / output]: on input, the array to sort
  *                                   on output, the sorted array
  */
void wxVirtualSortingDataModel::FastSort(wxVirtualItemIDs &vIDs)
{
    if (!m_pDataModel) return;
    size_t uiSize = vIDs.size();
    size_t i;

    TVariant tv;
    size_t uiNbFields = m_vSortFilters.size();
    tv.Reserve(uiNbFields);

    //fill the temporary array with all variants values
    std::vector<TVariant> vVariants;
    vVariants.reserve(uiSize);
    for(i=0;i<uiSize;i++)
    {
        tv.Clear();
        tv.m_id = vIDs[i];

        TSortFilters::const_iterator it    = m_vSortFilters.begin();
        TSortFilters::const_iterator itEnd = m_vSortFilters.end();
        while(it != itEnd)
        {
            const TSort &rSort = *it;
            size_t uiField = rSort.m_uiSortedField;

            wxVariant v = GetItemData(vIDs[i], uiField);
            tv.PushBack(v);

            ++it;
        }

        vVariants.push_back(tv);
    }

    //initialize the comparison functor
    wxVector<wxVirtualSortingDataModel::TSort> vSortOrder;
    TSortFilters::const_iterator it    = m_vSortFilters.begin();
    TSortFilters::const_iterator itEnd = m_vSortFilters.end();
    while(it != itEnd)
    {
        const TSort &rSort = *it;
        vSortOrder.push_back(rSort);
        ++it;
    }

    wxComparisonFunctor oCmp(m_pDataModel, uiNbFields, vSortOrder);

    //sort the temporary array
    std::sort(vVariants.begin(), vVariants.end(), oCmp);

    //output the results
    vIDs.clear();
    vIDs.reserve(uiSize);
    for(i=0;i<uiSize;i++)
    {
        vVariants[i].m_id.SetChildIndex(i);
        vIDs.push_back(vVariants[i].m_id);
    }
}

//---------------------- DO THE SORTING -----------------------------//

/** Sort all the items according to the filter
  * \param vIDs     [input / output]: on input, the array to sort
  *                                   on output, the sorted array
  */
void wxVirtualSortingDataModel::SortItems(wxVirtualItemIDs &vIDs)
{
    if (!IsSorting()) return;
    if (vIDs.size() < 2) return;

//    wxLogMessage("Sorting children = %d", vIDs.size());
    if (m_bFastSorting) FastSort(vIDs);
    else                MemoryEfficientSort(vIDs);
//    wxLogMessage("Sorted children = %d", vIDs.size());
}

//----------------------- INTERFACE ---------------------------------//
/** Get all children
  * \param rIDParent [input]: the parent item
  * \return a reference to the array containing the children
  */
void wxVirtualSortingDataModel::DoGetChildren(wxVirtualItemIDs &vChildren, const wxVirtualItemID &rIDParent)
{
    m_pDataModel->GetAllChildren(vChildren, rIDParent);
    SortItems(vChildren);
}
