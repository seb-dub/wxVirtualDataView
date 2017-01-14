/**********************************************************************/
/** FILE    : HashSetDefs.h                                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : typedefs for common hash sets                          **/
/**********************************************************************/

#ifndef _VIRTUAL_DATA_VIEW_HASH_SET_DEFINITIONS_H_
#define _VIRTUAL_DATA_VIEW_HASH_SET_DEFINITIONS_H_

#include <wx/VirtualDataView/Types/VariantUtils.h>
#include <wx/VirtualDataView/Types/HashUtils.h>
#include <wx/hashset.h>

WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, TSetOfStrings);
WX_DECLARE_HASH_SET(double, wxDoubleHash, wxDoubleEqual, TSetOfDoubles);
WX_DECLARE_HASH_SET(long, wxIntegerHash, wxIntegerEqual, TSetOfLongs);
WX_DECLARE_HASH_SET(int, wxIntegerHash, wxIntegerEqual, TSetOfBools);
WX_DECLARE_HASH_SET(unsigned long, wxIntegerHash, wxIntegerEqual, TSetOfULongs);
WX_DECLARE_HASH_SET(wxLongLong, wxLongLongHash, wxLongLongEqual, TSetOfLongLongs);
WX_DECLARE_HASH_SET(wxULongLong, wxLongLongHash, wxLongLongEqual, TSetOfULongLongs);
WX_DECLARE_HASH_SET(wxVariant, wxVariantHash, wxVariantEqual, TSetOfVariants);

#endif // _VIRTUAL_DATA_VIEW_HASH_SET_DEFINITIONS_H_
