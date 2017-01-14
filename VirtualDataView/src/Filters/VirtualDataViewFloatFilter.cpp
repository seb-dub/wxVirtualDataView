/**********************************************************************/
/** FILE    : VirtualDataViewFloatFilter.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for floating point values           **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewFloatFilter.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewFloatFilter::wxVirtualDataViewFloatFilter(void)
    : wxVirtualDataViewNumericFilter()
{

}

/** Destructor
  */
wxVirtualDataViewFloatFilter::~wxVirtualDataViewFloatFilter(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy object
  */
void wxVirtualDataViewFloatFilter::CopyFloat(const wxVirtualDataViewFloatFilter &rhs)
{
    CopyNumeric(rhs);
}

//--------------------- INTERFACE -----------------------------------//
/** Clone the filter
  * \return a copy of this filter, allocated with "new"
  */
wxVirtualDataViewFilter* wxVirtualDataViewFloatFilter::Clone(void) const
{
    wxVirtualDataViewFloatFilter *pFilter = new wxVirtualDataViewFloatFilter;
    pFilter->CopyFloat(*this);
    return(pFilter);
}

/** Accept or reject a value
  * \param rvValue [input]: the value to test
  * \return true if the value is accepted (it will be displayed)
  *         false if the value is rejected (it will be hidden)
  */
bool wxVirtualDataViewFloatFilter::AcceptValue(const wxVariant& rvValue)
{
    if (m_bUseStringMatching) return(AcceptString(rvValue));

    double fRefValue = m_vReferenceValue.GetDouble();
    double fValue    = rvValue.GetDouble();

    switch(m_eOperator)
    {
        case WX_E_EQUAL                     : return(fValue == fRefValue);
        case WX_E_NOT_EQUAL                 : return(fValue != fRefValue);
        case WX_E_GREATER_THAN              : return(fValue >  fRefValue);
        case WX_E_GREATER_THAN_OR_EQUAL     : return(fValue >= fRefValue);
        case WX_E_LESS_THAN                 : return(fValue <  fRefValue);
        case WX_E_LESS_THAN_OR_EQUAL        : return(fValue <= fRefValue);
        default                             : break;
    }
    return(true);
}

