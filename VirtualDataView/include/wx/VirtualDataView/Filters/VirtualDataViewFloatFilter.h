/**********************************************************************/
/** FILE    : VirtualDataViewFloatFilter.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter for floating point values           **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_FLOAT_FILTER_H_
#define _VIRTUAL_DATA_VIEW_FLOAT_FILTER_H_

#include <wx/VirtualDataView/Filters/VirtualDataViewNumericFilter.h>

//forward declarations
class WXDLLIMPEXP_BASE wxVariant;

/** \class wxVirtualDataViewFloatFilter : a filter for floating point values
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFloatFilter : public wxVirtualDataViewNumericFilter
{
    public:
        //constructors & destructor
        wxVirtualDataViewFloatFilter(void);                                     ///< \brief constructor
        virtual ~wxVirtualDataViewFloatFilter(void);                            ///< \brief destructor

        //interface
        virtual wxVirtualDataViewFilter* Clone(void) const;                     ///< \brief clone the filter
        virtual bool AcceptValue(const wxVariant& rvValue);                     ///< \brief accept or reject a value

    protected:

        //methods
        void CopyFloat(const wxVirtualDataViewFloatFilter &rhs);                ///< \brief copy object
};

#endif

