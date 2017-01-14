/**********************************************************************/
/** FILE    : VirtualDataViewFilter.cpp                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter                                     **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewFilter::wxVirtualDataViewFilter(void)
    : m_bIsActive(false),
      m_bIsRecursive(true)
{

}

/** Destructor
  */
wxVirtualDataViewFilter::~wxVirtualDataViewFilter(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy base object
  */
void wxVirtualDataViewFilter::CopyBase(const wxVirtualDataViewFilter &rhs)
{
    m_bIsActive     = rhs.m_bIsActive;
    m_bIsRecursive  = rhs.m_bIsRecursive;
}

//------------------------ INFORMATION ------------------------------//
/** Check if the filter is active
  * \return true if the filter is active, false otherwise
  */
bool wxVirtualDataViewFilter::IsActive(void) const
{
    return(m_bIsActive);
}

/** Check if the filter is inactive
  * \return true if the filter is inactive, false otherwise
  */
bool wxVirtualDataViewFilter::IsInactive(void) const
{
    return(!m_bIsActive);
}

/** Activate / de-activate the filter
  * \param bActive [input]: if true, the filter will be active
  *                         if false, the filter will be inactive
  */
void wxVirtualDataViewFilter::SetActive(bool bActive)
{
    m_bIsActive = bActive;
}

/** De-activate / activate the filter
  * \param
  * \param bInactive [input]: if true, the filter will be inactive
  *                           if false, the filter will be active
  */
void wxVirtualDataViewFilter::SetInactive(bool bInactive)
{
    m_bIsActive = !bInactive;
}

//-------------------- RECURSIVE ------------------------------------//
/** Return true if the filter is recursive
  * \return true if the filter is recursive, false otherwise
  *
  * Consider this item hierarchy
  *     PARENT_String_toto
  *         CHILD_String_tata
  *         CHILD_String_titi
  *
  * A filter on string "tata" should return CHILD_String_tata (1st child)
  * However, since the parent is rejected, the children are never scanned
  *
  * So when the filter is recursive, if an item is rejected, then its children
  * are scanned until all are rejected or at least one is accepted
  */
bool wxVirtualDataViewFilter::IsRecursive(void) const
{
    return(m_bIsRecursive);
}

/** Set if the filter is recursive
  * \param bRecursive [input]: true for recursive filter, false otherwise
  * See IsRecursive() for more info
  */
void wxVirtualDataViewFilter::SetRecursive(bool bRecursive)
{
    m_bIsRecursive = bRecursive;
}
