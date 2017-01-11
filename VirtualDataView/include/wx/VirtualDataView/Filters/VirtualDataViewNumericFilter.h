/**********************************************************************/
/** FILE    : VirtualDataViewNumericFilter.h                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for numerical values                **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_NUMERIC_FILTER_H_
#define _VIRTUAL_DATA_VIEW_NUMERIC_FILTER_H_

#include <wx/VirtualDataView/Filters/VirtualDataViewStringFilter.h>
#include <wx/variant.h>


/** \class wxVirtualDataViewNumericFilter : a filter for numerical values
  * There are 2 modes for matching:
  *     1 - the numeric matching : the numeric value is compared against a reference value
  *         using the standard math operators (==, !=, <, >, <=, >=)
  *         This allow to filter all values which are above >= 10 for example
  *     2 - the string matching : this behaves as the string filter. This allow to match "10" in "54100" for example
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewNumericFilter : public wxVirtualDataViewStringFilter
{
    public:
        //constructors & destructor
        wxVirtualDataViewNumericFilter(void);                                   ///< \brief constructor
        virtual ~wxVirtualDataViewNumericFilter(void);                          ///< \brief destructor

        //operator
        /// \enum EOperator [input]: the comparison operator for numeric filter
        enum EOperator
        {
            WX_E_EQUAL                  = 0,                                    ///< \brief value accepted if equal to reference value
            WX_E_NOT_EQUAL              = 1,                                    ///< \brief value accepted if not equal to reference value
            WX_E_GREATER_THAN           = 2,                                    ///< \brief value accepted if greater than reference value
            WX_E_GREATER_THAN_OR_EQUAL  = 3,                                    ///< \brief value accepted if greater than or equal to reference value
            WX_E_LESS_THAN              = 4,                                    ///< \brief value accepted if less than reference value
            WX_E_LESS_THAN_OR_EQUAL     = 5                                     ///< \brief value accepted if less than or equal to reference value
        };
        EOperator GetComparisonOperator(void) const;                            ///< \brief get the comparison operator
        void      SetComparisonOperator(EOperator eOperator);                   ///< \brief set the comparison operator

        //string matching & numeric matching
        bool IsStringMatchingUsed(void) const;                                  ///< \brief check if string comparison is used
        bool IsNumericMatchingUsed(void) const;                                 ///< \brief check if numeric matching is used
        void UseStringMatch(bool bUseStringMatching = true);                    ///< \brief toggle the use of string matching / numeric matching
        void UseNumericMatching(bool bUseNumericMatching = true);               ///< \brief toggle the use of numeric matching / string matching

    protected:
        //data
        EOperator   m_eOperator;                                                ///< \brief comparison operator
        bool        m_bUseStringMatching;                                       ///< \brief true if string matching is used

        //methods
        void CopyNumeric(const wxVirtualDataViewNumericFilter &rhs);            ///< \brief copy object
};

#endif

