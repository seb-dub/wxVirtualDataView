/**********************************************************************/
/** FILE    : VirtualDataViewIntegerFilter.cpp                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for integer values                  **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewIntegerFilter.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewIntegerFilter::wxVirtualDataViewIntegerFilter(void)
    : wxVirtualDataViewNumericFilter()
{

}

/** Destructor
  */
wxVirtualDataViewIntegerFilter::~wxVirtualDataViewIntegerFilter(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy object
  */
void wxVirtualDataViewIntegerFilter::CopyInteger(const wxVirtualDataViewIntegerFilter &rhs)
{
    CopyNumeric(rhs);
}

//--------------------- INTERFACE -----------------------------------//
/** Clone the filter
  * \return a copy of this filter, allocated with "new"
  */
wxVirtualDataViewFilter* wxVirtualDataViewIntegerFilter::Clone(void) const
{
    wxVirtualDataViewIntegerFilter *pFilter = new wxVirtualDataViewIntegerFilter;
    pFilter->CopyInteger(*this);
    return(pFilter);
}

/** Accept or reject a value
  * \param rvValue [input]: the value to test
  * \return true if the value is accepted (it will be displayed)
  *         false if the value is rejected (it will be hidden)
  */
bool wxVirtualDataViewIntegerFilter::AcceptValue(const wxVariant& rvValue)
{
    if (m_bUseStringMatching) return(AcceptString(rvValue));

    wxLongLong lRefValue = m_vReferenceValue.GetLongLong();
    wxLongLong lValue    = rvValue.GetLongLong();

    switch(m_eOperator)
    {
        case WX_E_EQUAL                     : return(lValue == lRefValue);
        case WX_E_NOT_EQUAL                 : return(lValue != lRefValue);
        case WX_E_GREATER_THAN              : return(lValue >  lRefValue);
        case WX_E_GREATER_THAN_OR_EQUAL     : return(lValue >= lRefValue);
        case WX_E_LESS_THAN                 : return(lValue <  lRefValue);
        case WX_E_LESS_THAN_OR_EQUAL        : return(lValue <= lRefValue);
        default                             : break;
    }
    return(true);
}

