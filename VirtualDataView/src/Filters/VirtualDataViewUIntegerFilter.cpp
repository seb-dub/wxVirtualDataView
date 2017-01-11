/**********************************************************************/
/** FILE    : VirtualDataViewUIntegerFilter.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for unsigned integer values         **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewUIntegerFilter.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewUIntegerFilter::wxVirtualDataViewUIntegerFilter(void)
    : wxVirtualDataViewNumericFilter()
{

}

/** Destructor
  */
wxVirtualDataViewUIntegerFilter::~wxVirtualDataViewUIntegerFilter(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy object
  */
void wxVirtualDataViewUIntegerFilter::CopyUInteger(const wxVirtualDataViewUIntegerFilter &rhs)
{
    CopyNumeric(rhs);
}

//--------------------- INTERFACE -----------------------------------//
/** Clone the filter
  * \return a copy of this filter, allocated with "new"
  */
wxVirtualDataViewFilter* wxVirtualDataViewUIntegerFilter::Clone(void) const
{
    wxVirtualDataViewUIntegerFilter *pFilter = new wxVirtualDataViewUIntegerFilter;
    pFilter->CopyUInteger(*this);
    return(pFilter);
}

/** Accept or reject a value
  * \param rvValue [input]: the value to test
  * \return true if the value is accepted (it will be displayed)
  *         false if the value is rejected (it will be hidden)
  */
bool wxVirtualDataViewUIntegerFilter::AcceptValue(const wxVariant& rvValue)
{
    if (m_bUseStringMatching) return(AcceptString(rvValue));

    wxULongLong ulRefValue = m_vReferenceValue.GetULongLong();
    wxULongLong ulValue    = rvValue.GetULongLong();

    switch(m_eOperator)
    {
        case WX_E_EQUAL                     : return(ulValue == ulRefValue);
        case WX_E_NOT_EQUAL                 : return(ulValue != ulRefValue);
        case WX_E_GREATER_THAN              : return(ulValue >  ulRefValue);
        case WX_E_GREATER_THAN_OR_EQUAL     : return(ulValue >= ulRefValue);
        case WX_E_LESS_THAN                 : return(ulValue <  ulRefValue);
        case WX_E_LESS_THAN_OR_EQUAL        : return(ulValue <= ulRefValue);
        default                             : break;
    }
    return(true);
}

