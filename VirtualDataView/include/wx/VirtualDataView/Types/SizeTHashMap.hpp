/**********************************************************************/
/** FILE    : SizeTHashMap.hpp                                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : hash map for size_t type                               **/
/**********************************************************************/


#ifndef WX_SIZE_T_HASH_MAP_HPP_
#define WX_SIZE_T_HASH_MAP_HPP_

#include <wx/hashmap.h>

struct WXDLLIMPEXP_VDV wxSizeTHash
{
    wxSizeTHash() { }
    unsigned long operator()( size_t x ) const { return (unsigned long)x; }

    wxSizeTHash& operator=(const wxSizeTHash&) { return *this; }
};

struct WXDLLIMPEXP_VDV wxSizeTEqual
{
    wxSizeTEqual() { }
    bool operator()( size_t a, size_t b ) const { return a == b; }

    wxSizeTEqual& operator=(const wxSizeTEqual&) { return *this; }
};

#endif

