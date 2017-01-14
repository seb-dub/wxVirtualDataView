/**********************************************************************/
/** FILE    : VirtualFilteringDataModel.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for filtering items                   **/
/**********************************************************************/

#include <wx/VirtualDataView/Models/VirtualFilteringDataModel.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/VirtualDataView/Types/HashSetDefs.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualFilteringDataModel::wxVirtualFilteringDataModel(void)
    : wxVirtualIArrayProxyDataModel(),
      m_bApplyFiltersForGetAllValues(true)
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
    if (m_vFilters.ClearFilters()) ClearCache();
}

/** Push filter
  * \param uiField [input]: the field to filter
  * \param pFilter [input]: the new filter to use. If NULL, the filter for this field is removed
  *                         Ownership of the filter is not taken. The caller is responsible for its allocation / release
  * A field can be filtered by several filters in sequence
  */
void wxVirtualFilteringDataModel::PushFilter(size_t uiField, wxVirtualDataViewFilter *pFilter)
{
    if (m_vFilters.PushFilter(uiField, pFilter))
    {
        ClearCache();
    }
}

/** Pop filter
  * \param uiField [input]: the field to unfilter
  */
void wxVirtualFilteringDataModel::PopFilter(size_t uiField)
{
    if (m_vFilters.PopFilter(uiField)) ClearCache();
}

/** Pop the last filter
  */
void wxVirtualFilteringDataModel::PopFilter(void)
{
    if (m_vFilters.PopFilter()) ClearCache();
}

/** Check if a field is filtering
  * \param uiField [input]: the field to query
  * \return true if the field is filtering, false otherwise
  */
bool wxVirtualFilteringDataModel::IsFiltering(size_t uiField) const
{
    return(m_vFilters.IsFiltering(uiField));
}

/** Check if at least one field is filtering
  * \return true if at least one field is filtering, false otherwise
  */
bool wxVirtualFilteringDataModel::IsFiltering(void) const
{
    return(m_vFilters.IsFiltering());
}

/** Get the filter for a field
  * \param uiField [input]: the field to query
  * \return the filter for this field
  */
wxVirtualDataViewFilter* wxVirtualFilteringDataModel::GetFilter(size_t uiField) const
{
    return(m_vFilters.GetFilter(uiField));
}

/** Get all the filters
  * \return a reference to all filters
  */
wxVirtualDataViewFiltersList& wxVirtualFilteringDataModel::Filters(void)
{
    ClearCache();
    return(m_vFilters);
}

/** Get all the filters - const version
  * \return a const reference to all filters
  */
const wxVirtualDataViewFiltersList& wxVirtualFilteringDataModel::Filters(void) const
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
    return(m_vFilters.IsAccepted(rID, m_pDataModel));
}

/** Check if an item is accepted, ignoring filter recursivity
  * \param rID [input]: the item to check
  * \return true if the item is accepted, false otherwise
  */
bool wxVirtualFilteringDataModel::IsAcceptedNonRecursive(const wxVirtualItemID &rID)
{
    return(m_vFilters.IsAcceptedNonRecursive(rID, m_pDataModel));
}

/** Apply filters to a list of IDs
  * \param vFiltered [output]: the list of accepted items. Previous content is lost
  * \param vUnfiltered [input]: the list of items to filter
  */
void wxVirtualFilteringDataModel::ApplyFilters(wxVirtualItemIDs &vFiltered,
                                               const wxVirtualItemIDs &vUnfiltered)
{
    m_vFilters.FilterItems(vFiltered, vUnfiltered, m_pDataModel);
}

//---------------------- HELPER METHOD FOR GetAllValues -------------//
/** Helper method for getting all the values
  * the main difference is that we want the filtered values, with all filters
  * actives EXCEPT THE ONE in the CURRENT FIELD.
  * The only way to do that is to:
  *  - deactivate the filter if it exists
  *  - get all values without filtering (forward to base model)
  *  - filter the results using the remaining active filters
  *  - reactivate the filters
  */
template<typename TSet, typename TArray, typename T>
void wxVirtualFilteringDataModel::DoGetAllValues(TArray &rArray, size_t uiField,
                                                 wxVirtualIStateModel *pStateModel)
{
    //special case: no master model
    if (!m_pDataModel) return;

    //special case : do not apply filter
    if (!m_bApplyFiltersForGetAllValues)
    {
        m_pDataModel->GetAllValues(rArray, uiField, pStateModel);
        return;
    }

    //check if the filter for the current field is active, and deactivate it if necessary
    wxVirtualDataViewFilter *pFilter = GetFilter(uiField);
    bool bIsFiltering = false;
    if (pFilter)
    {
        if (pFilter->IsActive())
        {
            //it is filtering - deactivate the filter
            bIsFiltering = true;
            pFilter->SetInactive();
        }
    }

    //get all values without the filter active
    if (IsFiltering())
    {
        //at least 1 another filter is active - so we need to filter out the data
        //No other ways than to iterate on all items
        TSet oSetOfValues;
        wxVirtualItemID id = m_pDataModel->GetRootItem();
        while(!id.IsInvalid())
        {
            if (IsAcceptedNonRecursive(id))
            {
                wxVariant v = m_pDataModel->GetItemData(id, uiField, WX_ITEM_MAIN_DATA);
                oSetOfValues.insert(wxGetVariantValueAs<T>(v));
            }
            id = m_pDataModel->NextItem(id, WX_VDV_NULL_PTR, 1);
        }

        //convert the set to an array
        rArray.reserve(oSetOfValues.size());
        typename TSet::iterator it      = oSetOfValues.begin();
        typename TSet::iterator itEnd   = oSetOfValues.end();
        while(it != itEnd)
        {
            rArray.push_back(*it);
            ++it;
        }
    }
    else
    {
        //no other filters active - forward to master model, which may have
        //optimized this method
        m_pDataModel->GetAllValues(rArray, uiField, pStateModel);
    }

    //reactivate the filter
    if (bIsFiltering) pFilter->SetActive();
}


//----------- INTERFACE REIMPLEMENTATION FROM wxVirtualIDataModel ---//
//------- FILTERING - GET ALL VALUES AS STRINGS ---------------------//
/** Get all the values of a field
  * \param rvStrings [output]: an array of string. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayString &rvStrings, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvStrings.Clear();
    DoGetAllValues<TSetOfStrings, wxArrayString, wxString>(rvStrings, uiField, pStateModel);
}

//------- FILTERING - GET ALL VALUES AS BOOL ------------------------//
/** Get all the values of a field
  * \param rvBools [output]: an array of bool. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayBool &rvBools, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvBools.Clear();
    DoGetAllValues<TSetOfBools, wxArrayBool, bool>(rvBools, uiField, pStateModel);
}

//------- FILTERING - GET ALL VALUES AS SIGNED INTEGERS -------------//
/** Get all the values of a field
  * \param rvShorts [output]: an array of shorts. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayShort &rvShorts, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvShorts.Clear();
    DoGetAllValues<TSetOfLongs, wxArrayShort, short>(rvShorts, uiField, pStateModel);
}

/** Get all the values of a field
  * \param rvInts [output]: an array of int. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayInt &rvInts, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvInts.Clear();
    DoGetAllValues<TSetOfLongs, wxArrayInt, int>(rvInts, uiField, pStateModel);
}

/** Get all the values of a field
  * \param rvLongs [output]: an array of long. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayLong &rvLongs, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvLongs.Clear();
    DoGetAllValues<TSetOfLongs, wxArrayLong, long>(rvLongs, uiField, pStateModel);
}

/** Get all the values of a field
  * \param rvLongLongs [output]: an array of wxLongLong. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayLongLong &rvLongLongs, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvLongLongs.Clear();
    DoGetAllValues<TSetOfLongLongs, wxArrayLongLong, wxLongLong>(rvLongLongs, uiField, pStateModel);
}

//------- FILTERING - GET ALL VALUES AS UNSIGNED INTEGERS -----------//
/** Get all the values of a field
  * \param rvUShorts [output]: an array of unsigned shorts. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayUShort &rvUShorts, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvUShorts.Clear();
    DoGetAllValues<TSetOfULongs, wxArrayUShort, unsigned short>(rvUShorts, uiField, pStateModel);
}

/** Get all the values of a field
  * \param rvUInts [output]: an array of unsigned int. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayUInt &rvUInts, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvUInts.Clear();
    DoGetAllValues<TSetOfULongs, wxArrayUInt, unsigned int>(rvUInts, uiField, pStateModel);
}

/** Get all the values of a field
  * \param rvULongs [output]: an array of unsigned long. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayULong &rvULongs, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvULongs.Clear();
    DoGetAllValues<TSetOfULongs, wxArrayULong, unsigned long>(rvULongs, uiField, pStateModel);
}

/** Get all the values of a field
  * \param rvULongLongs [output]: an array of wxULongLong. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayULongLong &rvULongLongs, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvULongLongs.Clear();
    DoGetAllValues<TSetOfULongLongs, wxArrayULongLong, wxULongLong>(rvULongLongs, uiField, pStateModel);
}

//------- FILTERING - GET ALL VALUES AS FLOAT / DOUBLE --------------//
/** Get all the values inside an array of floats
  * \param rvFloats [output]: an array of floats. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayFloat &rvFloats, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvFloats.Clear();
    DoGetAllValues<TSetOfDoubles, wxArrayFloat, double>(rvFloats, uiField, pStateModel);
}

/** Get all the values inside an array of doubles
  * \param rvDoubles [output]: an array of doubles. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxArrayDouble &rvDoubles, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvDoubles.Clear();
    DoGetAllValues<TSetOfDoubles, wxArrayDouble, double>(rvDoubles, uiField, pStateModel);
}

//------- FILTERING - GET ALL VALUES AS VARIANT ---------------------//
/** Get all the values inside an array of variants
  * \param rvVariants [output]: an array of variants. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void wxVirtualFilteringDataModel::GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                               wxVirtualIStateModel *pStateModel)
{
    rvVariants.clear();
    DoGetAllValues<TSetOfVariants, wxVector<wxVariant>, wxVariant>(rvVariants, uiField, pStateModel);
}
