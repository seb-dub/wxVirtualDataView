/**********************************************************************/
/** FILE    : VirtualDataViewStringFilter.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for string                          **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_STRING_FILTER_H_
#define _VIRTUAL_DATA_VIEW_STRING_FILTER_H_

#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/variant.h>

/** \class wxVirtualDataViewStringFilter : a string filter for wxVirtualDataViewCtrl
  * The searched string can be:
  *     - 1 or more strings to search. A match will occur if at least one of the
  *       searched string is found
  *     - 1 regular expression
  * The searched string can be set using SetReferenceValue
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewStringFilter : public wxVirtualDataViewFilter
{
    public:
        //constructors & destructor
        wxVirtualDataViewStringFilter(void);                                    ///< \brief constructor
        virtual ~wxVirtualDataViewStringFilter(void);                           ///< \brief destructor

        //interface
        virtual wxVirtualDataViewFilter* Clone(void) const;                     ///< \brief clone the filter
        virtual bool AcceptValue(const wxVariant& rvValue);                     ///< \brief accept or reject a value

        //reference value - stored as variant
        wxVariant GetReferenceValue(void) const;                                ///< \brief get the reference value
        void      SetReferenceValue(wxVariant vValue);                          ///< \brief set the reference value

        //options
        bool IsCaseSensitive(void) const;                                       ///< \brief check if the filter is case sensitive
        void SetCaseSensitive(bool bCaseSensitive = true);                      ///< \brief set case sensitive / insensitive
        void SetCaseInsensitive(bool bCaseInsensitive = true);                  ///< \brief set case insensitive / sensitive

        bool IsRegEx(void) const;                                               ///< \brief check if regular expressions are used
        void UseRegEx(bool bUseRegEx = true);                                   ///< \brief toggle the use of regular expression ON/OFF
        void DoNotUseRegEx(bool bDoNotUseRegEx = true);                         ///< \brief toggle the use of regular expression OFF/ON

        bool IsFullMatchOnly(void) const;                                       ///< \brief check if only full match are allowed
        bool IsPartialMatchAllowed(void) const;                                 ///< \brief check if partial matches are allowed
        void UseFullMatchOnly(bool bFullMatchOnly = true);                      ///< \brief switch full match ON/OFF
        void UsePartialMatch(bool bPartialMatch = true);                        ///< \brief switch partial match ON/OFF

    protected:
        //data
        wxString        m_sPattern;                                             ///< \brief string to search
        bool            m_bCaseSensitive;                                       ///< \brief true if the filter is active
        bool            m_bRegEx;                                               ///< \brief true if regular expressions are used
        bool            m_bFullMatch;                                           ///< \brief true if only full matches are accepted
        wxRegEx         m_sCompiledRegEx;                                       ///< \brief compiled regular expression
        wxArrayString   m_sSearchedString;                                      ///< \brief searched string in upper case if needed
        wxVariant       m_vReferenceValue;                                      ///< \brief reference value

        //methods
        void CopyString(const wxVirtualDataViewStringFilter &rhs);              ///< \brief copy object
        void CompileRegEx(void);                                                ///< \brief compile the regular expression
        bool AcceptString(const wxVariant& rvValue);                            ///< \brief accept or reject a value based on its string representation

        //pattern - stored as string
        wxString    GetPattern(void) const;                                     ///< \brief get the string pattern
        void        SetPattern(const wxString &sPattern);                       ///< \brief set the string pattern
};

#endif

