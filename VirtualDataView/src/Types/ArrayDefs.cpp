/**********************************************************************/
/** FILE    : ArrayDefs.cpp                                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : typedefs for common arrays                             **/
/**********************************************************************/


#include <wx/VirtualDataView/Types/ArrayDefs.h>

//signed integers
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayLongLong);

//unsigned integers
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayULongLong);

//date/time
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayDateTime);

//bitmap + text
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayBitmapText);
