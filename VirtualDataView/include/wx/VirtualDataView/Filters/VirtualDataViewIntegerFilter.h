/**********************************************************************/
/** FILE    : VirtualDataViewIntegerFilter.h                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for integer values                  **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_INTEGER_FILTER_H_
#define _VIRTUAL_DATA_VIEW_INTEGER_FILTER_H_

#include <wx/VirtualDataView/Filters/VirtualDataViewNumericFilter.h>

//forward declarations
class WXDLLIMPEXP_BASE wxVariant;

/** \class wxVirtualDataViewIntegerFilter : a filter for integer values
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewIntegerFilter : public wxVirtualDataViewNumericFilter
{
    public:
        //constructors & destructor
        wxVirtualDataViewIntegerFilter(void);                                   ///< \brief constructor
        virtual ~wxVirtualDataViewIntegerFilter(void);                          ///< \brief destructor

        //interface
        virtual wxVirtualDataViewFilter* Clone(void) const;                     ///< \brief clone the filter
        virtual bool AcceptValue(const wxVariant& rvValue);                     ///< \brief accept or reject a value

    protected:

        //methods
        void CopyInteger(const wxVirtualDataViewIntegerFilter &rhs);            ///< \brief copy object
};

#endif

