/**********************************************************************/
/** FILE    : VirtualFilteringDataModel.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for filtering items                   **/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualFilteringDataModel.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualFilteringDataModel::wxVirtualFilteringDataModel(void)
    : wxVirtualIArrayProxyDataModel()
{
}

/** Destructor
  */
wxVirtualFilteringDataModel::~wxVirtualFilteringDataModel(void)
{
}

//--------------------- FILTERS -------------------------------------//
/** Clear all filter
  */
void wxVirtualFilteringDataModel::ClearFilters(void)
{
    m_vFilters.clear();
    ClearCache();
}

/** Push filter
  * \param uiField [input]: the field to filter
  * \param pFilter [input]: the new filter to use. If NULL, the filter for this field is removed
  *                         Ownership of the filter is not taken. The caller is responsible for its allocation / release
  * A field can be filtered by several filters in sequence
  */
void wxVirtualFilteringDataModel::PushFilter(size_t uiField, wxVirtualDataViewFilter *pFilter)
{
    if (!pFilter)
    {
        PopFilter(uiField);
        return;
    }

    TFilter t;
    t.m_uiField = uiField;
    t.m_pFilter = pFilter;
    m_vFilters.push_back(t);
    ClearCache();
}

/** Pop filter
  * \param uiField [input]: the field to unfilter
  */
void wxVirtualFilteringDataModel::PopFilter(size_t uiField)
{
    TFilters::iterator it    = m_vFilters.begin();
    TFilters::iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        TFilter &t = *it;
        if (t.m_uiField == uiField)
        {
            ClearCache();
            m_vFilters.erase(it);
            it    = m_vFilters.begin();
            itEnd = m_vFilters.end();
        }
        else
        {
            ++it;
        }
    }
}

/** Pop the last filter
  */
void wxVirtualFilteringDataModel::PopFilter(void)
{
    m_vFilters.pop_back();
    ClearCache();
}

/** Check if a field is filtering
  * \param uiField [input]: the field to query
  * \return true if the field is filtering, false otherwise
  */
bool wxVirtualFilteringDataModel::IsFiltering(size_t uiField) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &t = *it;
        if ((t.m_uiField == uiField) && (t.m_pFilter))
        {
            return(true);
        }
        ++it;
    }
    return(false);
}

/** Check if at least one field is filtering
  * \return true if at least one field is filtering, false otherwise
  */
bool wxVirtualFilteringDataModel::IsFiltering(void) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &t = *it;
        if (t.m_pFilter) return(true);
        ++it;
    }
    return(false);
}

/** Get the filter for a field
  * \param uiField [input]: the field to query
  * \return the filter for this field
  */
wxVirtualDataViewFilter* wxVirtualFilteringDataModel::GetFilter(size_t uiField) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &t = *it;
        if (t.m_uiField == uiField) return(t.m_pFilter);
        ++it;
    }
    return(WX_VDV_NULL_PTR);
}

/** Get all the filters
  * \return a reference to all filters
  */
wxVirtualFilteringDataModel::TFilters& wxVirtualFilteringDataModel::Filters(void)
{
    ClearCache();
    return(m_vFilters);
}

/** Get all the filters - const version
  * \return a const reference to all filters
  */
const wxVirtualFilteringDataModel::TFilters& wxVirtualFilteringDataModel::Filters(void) const
{
    return(m_vFilters);
}

//----------------------- INTERFACE ---------------------------------//
/** Get all children
  * \param rIDParent [input]: the parent item
  * \return a reference to the array containing the children
  */
void wxVirtualFilteringDataModel::DoGetChildren(wxVirtualItemIDs &vChildren, const wxVirtualItemID &rIDParent)
{
    m_pDataModel->GetAllChildren(m_vTempArray, rIDParent);
    ApplyFilters(vChildren, m_vTempArray);
}

//--------------------- APPLY FILTERS -------------------------------//
/** Check if an item is accepted
  * \param rID [input]: the item to check
  * \return true if the item is accepted, false otherwise
  */
bool wxVirtualFilteringDataModel::IsAccepted(const wxVirtualItemID &rID)
{
    TFilters::const_iterator it     = m_vFilters.begin();
    TFilters::const_iterator itEnd  = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &rFilter = *it;
        if (rFilter.m_pFilter)
        {
            if (rFilter.m_pFilter->IsActive())
            {
                wxVariant vValue = GetItemData(rID, rFilter.m_uiField, WX_ITEM_MAIN_DATA);
                if (!rFilter.m_pFilter->AcceptValue(vValue))
                {
                    bool bRecursive = rFilter.m_pFilter->IsRecursive();
                    if (!bRecursive) return(false);

                    //scan children
                    wxVirtualItemIDs vChildren;
                    m_pDataModel->GetAllChildren(vChildren, rID);
                    size_t uiNbChildren = vChildren.size();
                    size_t uiChild;
                    for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
                    {
                        wxVirtualItemID &rIDChild = vChildren[uiChild];
                        if (IsAccepted(rIDChild)) return(true);
                    }
                    return(false);
                }
            }
        }

        ++it;
    }
    return(true);
}

/** Apply filters to a list of IDs
  * \param vFiltered [output]: the list of accepted items. Previous content is lost
  * \param vUnfiltered [input]: the list of items to filter
  */
void wxVirtualFilteringDataModel::ApplyFilters(wxVirtualItemIDs &vFiltered,
                                               const wxVirtualItemIDs &vUnfiltered)
{
    vFiltered.clear();

    size_t uiChildIndex = 0;
    wxVirtualItemIDs::const_iterator it     = vUnfiltered.begin();
    wxVirtualItemIDs::const_iterator itEnd  = vUnfiltered.end();
    while(it != itEnd)
    {
        const wxVirtualItemID &rID = *it;
        if (IsAccepted(rID))
        {
            vFiltered.push_back(rID);
            vFiltered[uiChildIndex].SetChildIndex(uiChildIndex);
            uiChildIndex++;
        }
        ++it;
    }
}
