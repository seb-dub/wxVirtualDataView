/**********************************************************************/
/** FILE    : VirtualDataViewUIntegerFilter.h                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for unsigned integer values         **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_UINTEGER_FILTER_H_
#define _VIRTUAL_DATA_VIEW_UINTEGER_FILTER_H_

#include <wx/VirtualDataView/Filters/VirtualDataViewNumericFilter.h>

//forward declarations
class WXDLLIMPEXP_BASE wxVariant;

/** \class wxVirtualDataViewUIntegerFilter : a filter for unsigned integer values
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewUIntegerFilter : public wxVirtualDataViewNumericFilter
{
    public:
        //constructors & destructor
        wxVirtualDataViewUIntegerFilter(void);                                  ///< \brief constructor
        virtual ~wxVirtualDataViewUIntegerFilter(void);                         ///< \brief destructor

        //interface
        virtual wxVirtualDataViewFilter* Clone(void) const;                     ///< \brief clone the filter
        virtual bool AcceptValue(const wxVariant& rvValue);                     ///< \brief accept or reject a value

    protected:

        //methods
        void CopyUInteger(const wxVirtualDataViewUIntegerFilter &rhs);          ///< \brief copy object
};

#endif

