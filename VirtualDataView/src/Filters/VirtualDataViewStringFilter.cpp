/**********************************************************************/
/** FILE    : VirtualDataViewStringFilter.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for string                          **/
/**********************************************************************/

#include <wx/VirtualDataView/Filters/VirtualDataViewStringFilter.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>
#include <wx/variant.h>
#include <wx/dc.h>
#include <wx/app.h>
#include <wx/window.h>
#include <wx/renderer.h>
#include <wx/combobox.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewStringFilter::wxVirtualDataViewStringFilter(void)
    : wxVirtualDataViewFilter(),
      m_sPattern(""),
      m_bCaseSensitive(false),
      m_bRegEx(false),
      m_bFullMatch(false),
      m_sCompiledRegEx("")
{

}

/** Destructor
  */
wxVirtualDataViewStringFilter::~wxVirtualDataViewStringFilter(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy object
  */
void wxVirtualDataViewStringFilter::CopyString(const wxVirtualDataViewStringFilter &rhs)
{
    CopyBase(rhs);
    m_sPattern          = rhs.m_sPattern;
    m_bCaseSensitive    = rhs.m_bCaseSensitive;
    m_bRegEx            = rhs.m_bRegEx;
    m_bFullMatch        = rhs.m_bFullMatch;
    m_vReferenceValue   = rhs.m_vReferenceValue;
    CompileRegEx();
}

/** Compile the regular expression
  */
void wxVirtualDataViewStringFilter::CompileRegEx(void)
{
    //not RegEx search : copy the string(s), and put them in upper case
    //for case insensitive search
    //The reference value can contains more than 1 string
    m_sSearchedString.Clear();
    if (m_vReferenceValue.GetType() == "arrstring")
    {
        m_sSearchedString = m_vReferenceValue.GetArrayString();
    }
    else
    {
        m_sSearchedString.push_back(GetStringValue(m_vReferenceValue));
    }

    if (!m_bCaseSensitive)
    {
        size_t uiNbStrings = m_sSearchedString.Count();
        size_t uiString;
        for(uiString = 0; uiString < uiNbStrings; uiString++)
        {
            m_sSearchedString[uiString].MakeUpper();
        }
    }

    //regular expression compilation. Only 1st string taken into account
    //in case of multiple strings in the reference value
    if (!m_bRegEx) return;
    int iFlags = wxRE_DEFAULT | wxRE_NOSUB;
    if (!m_bCaseSensitive) iFlags |= wxRE_ICASE;
    m_sCompiledRegEx.Compile(m_sPattern, iFlags);
}

/** Accept or reject a value based on its string representation
  * \param rvValue [input]: the value to test
  * \return true if the value is accepted (it will be displayed)
  *         false if the value is rejected (it will be hidden)
  */
bool wxVirtualDataViewStringFilter::AcceptString(const wxVariant& rvValue)
{
    //get value
    wxString sValue = GetStringValue(rvValue);

    //capitalize if needed
    if (!m_bCaseSensitive) sValue.MakeUpper();

    //match
    if (m_bRegEx)
    {
        //regex match
        if (m_sCompiledRegEx.Matches(sValue))
        {
            //match found in regex: full match or partial match
            if (!m_bFullMatch) return(true);
            size_t uiMatchCount = m_sCompiledRegEx.GetMatchCount();
            size_t uiMatch;
            for(uiMatch = 0; uiMatch < uiMatchCount; uiMatch++)
            {
                wxString sMatch;
                m_sCompiledRegEx.GetMatch(sMatch, uiMatch);
                if (sMatch == sValue) return(true);
            }
        }
    }
    else
    {
        //standard match
        size_t uiNbStrings = m_sSearchedString.Count();
        size_t uiString;
        if (m_bFullMatch)
        {
            for(uiString = 0; uiString < uiNbStrings; uiString++)
            {
                if (sValue == m_sSearchedString[uiString]) return(true);
            }
        }
        else
        {
            for(uiString = 0; uiString < uiNbStrings; uiString++)
            {
                if (sValue.Find(m_sSearchedString[uiString]) != wxNOT_FOUND) return(true);
            }
        }
    }

    return(false);
}

//--------------------- INTERFACE -----------------------------------//
/** Clone the filter
  * \return a copy of this filter, allocated with "new"
  */
wxVirtualDataViewFilter* wxVirtualDataViewStringFilter::Clone(void) const
{
    wxVirtualDataViewStringFilter *pFilter = new wxVirtualDataViewStringFilter;
    pFilter->CopyString(*this);
    return(pFilter);
}

/** Accept or reject a value
  * \param rvValue [input]: the value to test
  * \return true if the value is accepted (it will be displayed)
  *         false if the value is rejected (it will be hidden)
  */
bool wxVirtualDataViewStringFilter::AcceptValue(const wxVariant& rvValue)
{
    return(AcceptString(rvValue));
}

//---------------------------- PATTERN ------------------------------//
/** Get the string pattern
  * \return the string pattern
  */
wxString wxVirtualDataViewStringFilter::GetPattern(void) const
{
    return(m_sPattern);
}

/** Set the string pattern
  * \param sPattern [input] : the new pattern
  */
void wxVirtualDataViewStringFilter::SetPattern(const wxString &sPattern)
{
    if (sPattern == m_sPattern) return;
    m_sPattern = sPattern;
    CompileRegEx();
}

//---------------------- REFERENCE VALUE ----------------------------//
/** Get the reference value
  * \return the reference value
  */
wxVariant wxVirtualDataViewStringFilter::GetReferenceValue(void) const
{
    return(m_vReferenceValue);
}

/** Set the reference value
  * \param vValue [input]: the new reference value
  */
void wxVirtualDataViewStringFilter::SetReferenceValue(wxVariant vValue)
{
    m_vReferenceValue = vValue;
    wxString sValue = GetStringValue(m_vReferenceValue);
    if (m_vReferenceValue.GetType() == "arrstring")
    {
        wxArrayString vStrings = m_vReferenceValue.GetArrayString();
        if (vStrings.Count() > 0) sValue = vStrings[0];
        else                      sValue = wxEmptyString;
    }
    SetPattern(sValue);
}


//----------------------- OPTIONS -----------------------------------//
/** Check if the filter is case sensitive
  * \return true if the filter is case sensitive, false otherwise
  */
bool wxVirtualDataViewStringFilter::IsCaseSensitive(void) const
{
    return(m_bCaseSensitive);
}

/** Set case sensitive / insensitive
  * \param bCaseSensitive [input]: true for case sensitive
  *                                false for case insensitive
  */
void wxVirtualDataViewStringFilter::SetCaseSensitive(bool bCaseSensitive)
{
    if ((m_bCaseSensitive) && (bCaseSensitive)) return;
    if ((!m_bCaseSensitive) && (!bCaseSensitive)) return;
    m_bCaseSensitive = bCaseSensitive;
    CompileRegEx();
}

/** Set case insensitive / sensitive
  * \param bCaseInsensitive [input]: true for case insensitive
  *                                 false for case sensitive
  */
void wxVirtualDataViewStringFilter::SetCaseInsensitive(bool bCaseInsensitive)
{
    if ((m_bCaseSensitive) && (!bCaseInsensitive)) return;
    if ((!m_bCaseSensitive) && (bCaseInsensitive)) return;
    m_bCaseSensitive = !bCaseInsensitive;
    CompileRegEx();
}

/** Check if regular expressions are used
  * \return true if regular expressions are used, false otherwise
  */
bool wxVirtualDataViewStringFilter::IsRegEx(void) const
{
    return(m_bRegEx);
}

/** Toggle the use of regular expression ON/OFF
  * \param bUseRegEx [input]: true for using regular expression
  *                           false for using standard text matching
  */
void wxVirtualDataViewStringFilter::UseRegEx(bool bUseRegEx)
{
    if ((m_bRegEx) && (bUseRegEx)) return;
    if ((!m_bRegEx) && (!bUseRegEx)) return;
    m_bRegEx = bUseRegEx;
    CompileRegEx();
}

/** Toggle the use of regular expression OFF/ON
  * \param bUseRegEx [input]: true for using standard text matching
  *                           false for using regular expression
  */
void wxVirtualDataViewStringFilter::DoNotUseRegEx(bool bDoNotUseRegEx)
{
    if ((m_bRegEx) && (!bDoNotUseRegEx)) return;
    if ((!m_bRegEx) && (bDoNotUseRegEx)) return;
    m_bRegEx = !bDoNotUseRegEx;
    CompileRegEx();
}

/** Check if only full match are allowed
  * \return true if only full match are allowed
  *         false if partial matches are allowed
  */
bool wxVirtualDataViewStringFilter::IsFullMatchOnly(void) const
{
    return(m_bFullMatch);
}

/** Check if partial matches are allowed
  * \return true if partial matches are allowed
  *         false if only full match are allowed
  */
bool wxVirtualDataViewStringFilter::IsPartialMatchAllowed(void) const
{
    return(!m_bFullMatch);
}

/** Switch full match ON/OFF
  * \param bFullMatchOnly [input]: true if only full matches are allowed
  *                                false if partial matches are allowed
  */
void wxVirtualDataViewStringFilter::UseFullMatchOnly(bool bFullMatchOnly)
{
    m_bFullMatch = bFullMatchOnly;
}

/** Switch partial match ON/OFF
  * \param bPartialMatch [input]: true if partial matches are allowed
  *                               false if only full matches are allowed
  */
void wxVirtualDataViewStringFilter::UsePartialMatch(bool bPartialMatch)
{
    m_bFullMatch = !bPartialMatch;
}
