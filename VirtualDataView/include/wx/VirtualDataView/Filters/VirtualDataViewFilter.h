/**********************************************************************/
/** FILE    : VirtualDataViewFilter.h                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view filter                                     **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_FILTER_H_
#define _VIRTUAL_DATA_VIEW_FILTER_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

//forward declarations
class WXDLLIMPEXP_BASE wxVariant;

/** \class wxVirtualDataViewFilter : a filter for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFilter
{
    public:
        //constructors & destructor
        wxVirtualDataViewFilter(void);                                              ///< \brief constructor
        virtual ~wxVirtualDataViewFilter(void);                                     ///< \brief destructor

        //interface
        virtual wxVirtualDataViewFilter* Clone(void) const                  = 0;    ///< \brief clone the filter
        virtual bool      AcceptValue(const wxVariant& rvValue)             = 0;    ///< \brief accept or reject a value

        //information
        bool    IsActive(void) const;                                               ///< \brief check if the filter is active
        bool    IsInactive(void) const;                                             ///< \brief check if the filter is inactive
        void    SetActive(bool bActive = true);                                     ///< \brief activate / de-activate the filter
        void    SetInactive(bool bInactive = true);                                 ///< \brief de-activate / activate the filter

        //recursive
        bool    IsRecursive(void) const;                                            ///< \brief return true if the filter is recursive
        void    SetRecursive(bool bRecursive);                                      ///< \brief set if the filter is recursive

    protected:
        //data
        bool    m_bIsActive;                                                        ///< \brief true if the filter is active
        bool    m_bIsRecursive;                                                     ///< \brief true if the filter is recursive (item accepted when 1 child is accepted)

        //methods
        void CopyBase(const wxVirtualDataViewFilter &rhs);                          ///< \brief copy base object
};

#endif

