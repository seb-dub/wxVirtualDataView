/**********************************************************************/
/** FILE    : ArrayDefs.h                                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : typedefs for common arrays                             **/
/**********************************************************************/

#ifndef _VIRTUAL_DATA_VIEW_ARRAY_DEFINITIONS_H_
#define _VIRTUAL_DATA_VIEW_ARRAY_DEFINITIONS_H_

#include <wx/dynarray.h>
#include <wx/VirtualDataView/Types/BitmapText.h>

//define some commonly used array (simple types) which are missing from wxWidgets
//bool
WX_DEFINE_USER_EXPORTED_ARRAY_INT(int, wxArrayBool, class WXDLLIMPEXP_VDV);

//signed integers. int, short & longs already defined
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxLongLong, wxArrayLongLong, WXDLLIMPEXP_VDV);

//unsigned integers.
WX_DEFINE_USER_EXPORTED_ARRAY_SIZE_T(unsigned short, wxArrayUShort, class WXDLLIMPEXP_VDV);
WX_DEFINE_USER_EXPORTED_ARRAY_SIZE_T(unsigned int, wxArrayUInt, class WXDLLIMPEXP_VDV);
WX_DEFINE_USER_EXPORTED_ARRAY_SIZE_T(unsigned long, wxArrayULong, class WXDLLIMPEXP_VDV);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxULongLong, wxArrayULongLong, WXDLLIMPEXP_VDV);
WX_DEFINE_USER_EXPORTED_ARRAY_SIZE_T(size_t, wxArraySizeT, class WXDLLIMPEXP_VDV);

//float
WX_DEFINE_USER_EXPORTED_ARRAY_DOUBLE(float, wxArrayFloat, class WXDLLIMPEXP_VDV);

//date/time
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxDateTime, wxArrayDateTime, WXDLLIMPEXP_VDV);

//bitmap + text
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxBitmapText, wxArrayBitmapText, WXDLLIMPEXP_VDV);

#endif // _VIRTUAL_DATA_VIEW_ARRAY_DEFINITIONS_H_
