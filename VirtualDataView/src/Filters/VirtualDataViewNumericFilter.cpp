/**********************************************************************/
/** FILE    : VirtualDataViewNumericFilter.cpp                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for numerical values                **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewNumericFilter.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewNumericFilter::wxVirtualDataViewNumericFilter(void)
    : wxVirtualDataViewStringFilter(),
      m_eOperator(WX_E_EQUAL),
      m_bUseStringMatching(false)
{

}

/** Destructor
  */
wxVirtualDataViewNumericFilter::~wxVirtualDataViewNumericFilter(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy object
  */
void wxVirtualDataViewNumericFilter::CopyNumeric(const wxVirtualDataViewNumericFilter &rhs)
{
    CopyString(rhs);
    m_eOperator             = rhs.m_eOperator;
    m_bUseStringMatching    = rhs.m_bUseStringMatching;
}

//--------------------------- OPERATOR ------------------------------//
/** Get the comparison operator
  * \return the comparison operator
  */
wxVirtualDataViewNumericFilter::EOperator wxVirtualDataViewNumericFilter::GetComparisonOperator(void) const
{
    return(m_eOperator);
}

/** Set the comparison operator
  * \param eOperator [input]: the new comparison operator
  */
void wxVirtualDataViewNumericFilter::SetComparisonOperator(EOperator eOperator)
{
    m_eOperator = eOperator;
}

//----------------- STRING MATCHING & NUMERIC MATCHING --------------//
/** Check if string comparison is used
  * \return true if string matching is used
  *         false if numeric matching is used
  */
bool wxVirtualDataViewNumericFilter::IsStringMatchingUsed(void) const
{
    return(m_bUseStringMatching);
}

/** Check if numeric comparison is used
  * \return true if numeric matching is used
  *         false if string matching is used
  */
bool wxVirtualDataViewNumericFilter::IsNumericMatchingUsed(void) const
{
    return(!m_bUseStringMatching);
}

/** Toggle the use of string matching / numeric matching
  * \param bUseStringMatching [input]: if true, string matching is used
  *                                    if false, numeric matching is used
  */
void wxVirtualDataViewNumericFilter::UseStringMatch(bool bUseStringMatching)
{
    m_bUseStringMatching = bUseStringMatching;
}

/** Toggle the use of numeric matching / string matching
  * \param bUseNumericMatching [input]: if true, numeric matching is used
  *                                     if false, string matching is used
  */
void wxVirtualDataViewNumericFilter::UseNumericMatching(bool bUseNumericMatching)
{
    m_bUseStringMatching = !bUseNumericMatching;
}
