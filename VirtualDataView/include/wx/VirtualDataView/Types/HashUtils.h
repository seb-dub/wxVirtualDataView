/**********************************************************************/
/** FILE    : HashUtils.h                                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : hash maps/sets utilities                               **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_HASH_UTILITIES_H_
#define _VIRTUAL_DATA_VIEW_HASH_UTILITIES_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

//----------------- FLOAT / DOUBLE HASHING --------------------------//
unsigned long wxGetFloatBits(float x)
{
    union u
    {
        float         x;
        unsigned long ul[(sizeof(float) + sizeof(unsigned long) - 1) / sizeof(unsigned long)];
    } uu;
    uu.x = x;

    const size_t uiSize = sizeof(uu.ul) / sizeof(unsigned long);
    if (uiSize == 1) return(uu.ul[0]);

    size_t i;
    unsigned long ul = 0;
    for(i=0;i<uiSize;i++) ul ^= uu.ul[i];
    return(ul);
};

unsigned long wxGetDoubleBits(double x)
{
    union u
    {
        double        x;
        unsigned long ul[(sizeof(float) + sizeof(unsigned long) - 1) / sizeof(unsigned long)];
    } uu;
    uu.x = x;

    const size_t uiSize = sizeof(uu.ul) / sizeof(unsigned long);
    if (uiSize == 1) return(uu.ul[0]);

    size_t i;
    unsigned long ul = 0;
    for(i=0;i<uiSize;i++) ul ^= uu.ul[i];
    return(ul);
}

struct WXDLLIMPEXP_VDV wxDoubleHash
{
    wxDoubleHash() { }
    unsigned long operator()( float x ) const { return wxGetFloatBits(x); }
    unsigned long operator()( double x ) const { return wxGetDoubleBits(x); }

    wxDoubleHash& operator=(const wxDoubleHash&) { return *this; }
};

struct WXDLLIMPEXP_VDV wxDoubleEqual
{
    wxDoubleEqual() { }
    bool operator()( float a, float b ) const { return a == b; }
    bool operator()( double a, double b ) const { return a == b; }

    wxDoubleEqual& operator=(const wxDoubleEqual&) { return *this; }
};

//------------------- wxLongLong and wxULongLong HASHING ------------//
struct WXDLLIMPEXP_VDV wxLongLongHash
{
    wxLongLongHash() { }
    unsigned long operator()( wxLongLong x ) const { return x.ToLong(); }
    unsigned long operator()( wxULongLong x ) const { return x.ToULong(); }

    wxLongLongHash& operator=(const wxLongLongHash&) { return *this; }
};

struct WXDLLIMPEXP_VDV wxLongLongEqual
{
    wxLongLongEqual() { }
    bool operator()( wxLongLong a, wxLongLong b ) const { return a == b; }
    bool operator()( wxULongLong a, wxULongLong b ) const { return a == b; }

    wxLongLongEqual& operator=(const wxLongLongEqual&) { return *this; }
};

#endif
