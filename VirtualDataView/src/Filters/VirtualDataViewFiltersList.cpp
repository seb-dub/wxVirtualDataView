/**********************************************************************/
/** FILE    : VirtualDataViewFiltersList.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a collection of filters                                **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewFiltersList.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewFiltersList::wxVirtualDataViewFiltersList(void)
    : m_vFilters()
{

}

/** Destructor
  */
wxVirtualDataViewFiltersList::~wxVirtualDataViewFiltersList(void)
{

}

//-------------------- LIST MANAGEMENT ------------------------------//
/** Clear all filter
  * \return true if some filters were erased
  */
bool wxVirtualDataViewFiltersList::ClearFilters(void)
{
    if (m_vFilters.size() == 0) return(false);

    bool bIsFiltering = IsFiltering();
    m_vFilters.clear();
    return(bIsFiltering);
}

/** Push filter
  * \param uiField [input]: the field to filter
  * \param pFilter [input]: the new filter to use. If NULL, the filter for this field is removed
  *                         Ownership of the filter is not taken. The caller is responsible for its allocation / release
  * A field can be filtered by several filters in sequence
  * \return true if a filter was added or removed
  */
bool wxVirtualDataViewFiltersList::PushFilter(size_t uiField, wxVirtualDataViewFilter *pFilter)
{
    if (!pFilter) return(PopFilter(uiField));

    TFilter t;
    t.m_uiField = uiField;
    t.m_pFilter = pFilter;
    m_vFilters.push_back(t);
    return(true);
}

/** Pop filter
  * \param uiField [input]: the field to unfilter
  * \return true if a filter was removed
  */
bool wxVirtualDataViewFiltersList::PopFilter(size_t uiField)
{
    bool bRemoved = false;
    TFilters::iterator it    = m_vFilters.begin();
    TFilters::iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        TFilter &t = *it;
        if (t.m_uiField == uiField)
        {
            m_vFilters.erase(it);
            it    = m_vFilters.begin();
            itEnd = m_vFilters.end();
            bRemoved = true;
        }
        else
        {
            ++it;
        }
    }
    return(bRemoved);
}

/** Pop the last filter
  * \return true if a filter was removed
  */
bool wxVirtualDataViewFiltersList::PopFilter(void)
{
    if (m_vFilters.size() == 0) return(false);
    m_vFilters.pop_back();
    return(true);
}

/** Check if a field is filtering
  * \param uiField [input]: the field to query
  * \return true if the field is filtering, false otherwise
  */
bool wxVirtualDataViewFiltersList::IsFiltering(size_t uiField) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &t = *it;
        if ((t.m_uiField == uiField) && (t.m_pFilter))
        {
            if (t.m_pFilter->IsActive()) return(true);
        }
        ++it;
    }
    return(false);
}

/** Check if at least one field is filtering
  * \return true if at least one field is filtering, false otherwise
  */
bool wxVirtualDataViewFiltersList::IsFiltering(void) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &t = *it;
        if (t.m_pFilter)
        {
            if (t.m_pFilter->IsActive()) return(true);
        }
        ++it;
    }
    return(false);
}

/** Get the filter for a field
  * \param uiField [input]: the field to query
  * \return the filter for this field
  */
wxVirtualDataViewFilter* wxVirtualDataViewFiltersList::GetFilter(size_t uiField) const
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
wxVirtualDataViewFiltersList::TFilters& wxVirtualDataViewFiltersList::Filters(void)
{
    return(m_vFilters);
}

/** Get all the filters - const version
  * \return a const reference to all filters
  */
const wxVirtualDataViewFiltersList::TFilters& wxVirtualDataViewFiltersList::Filters(void) const
{
    return(m_vFilters);
}

//--------------------- TESTING VALUES ------------------------------//
/** Acept or reject a value
  * \param rvValue [input]: the value to test
  * \return true if the value is accepted by at least one filter, false otherwise
  */
bool wxVirtualDataViewFiltersList::AcceptValue(const wxVariant& rvValue) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &rFilter = *it;
        if (rFilter.m_pFilter)
        {
            if (rFilter.m_pFilter->IsActive())
            {
                if ((rFilter.m_pFilter->AcceptValue(rvValue))) return(true);
            }
        }
        ++it;
    }
    return(false);
}

/** Accept or reject a value
  * \param rvValue [input]: the value to test
  * \param uiField [input]: the field to which this value belongs.
  *                         Only filter(s) belonging to this field will be checked
  * \return true if the value is accepted by at least one filter, false otherwise
  */
bool wxVirtualDataViewFiltersList::AcceptValue(const wxVariant& rvValue, size_t uiField) const
{
    TFilters::const_iterator it    = m_vFilters.begin();
    TFilters::const_iterator itEnd = m_vFilters.end();
    while (it != itEnd)
    {
        const TFilter &rFilter = *it;
        if (rFilter.m_uiField != uiField) {++it; continue;}
        if (rFilter.m_pFilter)
        {
            if (rFilter.m_pFilter->IsActive())
            {
                if ((rFilter.m_pFilter->AcceptValue(rvValue))) return(true);
            }
        }
        ++it;
    }
    return(false);
}

/** Accept or reject an item from a wxVirtualIDataModel
  * \param rID          [input]: the ID of the item to check
  * \param pDataModel   [input]: the data model to which the ID belongs
  */
bool wxVirtualDataViewFiltersList::IsAccepted(const wxVirtualItemID &rID,
                                              wxVirtualIDataModel *pDataModel)
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
                wxVariant vValue = pDataModel->GetItemData(rID, rFilter.m_uiField,
                                                           wxVirtualIDataModel::WX_ITEM_MAIN_DATA);
                if (!rFilter.m_pFilter->AcceptValue(vValue))
                {
                    bool bRecursive = rFilter.m_pFilter->IsRecursive();
                    if (!bRecursive) return(false);

                    //scan children
                    wxVirtualItemIDs vChildren;
                    pDataModel->GetAllChildren(vChildren, rID);
                    size_t uiNbChildren = vChildren.size();
                    size_t uiChild;
                    for(uiChild = 0; uiChild < uiNbChildren; uiChild++)
                    {
                        wxVirtualItemID &rIDChild = vChildren[uiChild];
                        if (IsAccepted(rIDChild, pDataModel)) return(true);
                    }
                    return(false);
                }
            }
        }

        ++it;
    }
    return(true);
}


/** Accept or reject an item from a wxVirtualIDataModel
  * The recursivity of the filters is ignored.
  * \param rID          [input]: the ID of the item to check
  * \param pDataModel   [input]: the data model to which the ID belongs
  */
bool wxVirtualDataViewFiltersList::IsAcceptedNonRecursive(const wxVirtualItemID &rID,
                                                          wxVirtualIDataModel *pDataModel)
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
                wxVariant vValue = pDataModel->GetItemData(rID, rFilter.m_uiField,
                                                           wxVirtualIDataModel::WX_ITEM_MAIN_DATA);
                if (!rFilter.m_pFilter->AcceptValue(vValue)) return(false);
            }
        }

        ++it;
    }
    return(true);
}

/** Filter a list of items from a wxVirtualIDataModel
  * \param vFilteredIDs     [output]: the list of filtered item IDs
  * \param vUnfilteredIDs   [input] : the list of item IDs to test. Order is preserved in output
  * \param pDataModel       [input]: the data model to which the ID belong
  */
void wxVirtualDataViewFiltersList::FilterItems(wxVirtualItemIDs &vFilteredIDs,
                                               const wxVirtualItemIDs &vUnfilteredIDs,
                                               wxVirtualIDataModel *pDataModel)
{
    vFilteredIDs.clear();

    size_t uiSize = vUnfilteredIDs.size();
    if (uiSize == 0) return;

    size_t i, uiChildIndex;
    uiChildIndex = 0;
    for(i = 0; i < uiSize; i++)
    {
        const wxVirtualItemID &rID = vUnfilteredIDs[i];
        if (IsAccepted(rID, pDataModel))
        {
            vFilteredIDs.push_back(rID);
            vFilteredIDs[uiChildIndex].SetChildIndex(uiChildIndex);
            uiChildIndex++;
        }
    }
}
