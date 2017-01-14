/**********************************************************************/
/** FILE    : VariantUtils.h                                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a set of utilities for wxVariant                       **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_VARIANT_UTILS_H_
#define _VIRTUAL_DATA_VIEW_VARIANT_UTILS_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/variant.h>
#include <wx/hashmap.h>

//----------------------- CONVERSION --------------------------------//
wxString GetStringValue(const wxVariant &rvValue);                          ///< \brief convert a variant to string

template<typename T>
T wxGetVariantValueAs(const wxVariant &rvValue);                            ///< \brief convert variant

//---------------------- COMPARISON ---------------------------------//
/// \enum EVariantComparison : result of variant comparisons
enum EVariantComparison
{
    WX_VARIANT_LESS_THAN        = -1,                                       ///< \brief LeftValue <  RightValue
    WX_VARIANT_EQUAL            =  0,                                       ///< \brief LeftValue == RightValue
    WX_VARIANT_GREATER_THAN     =  1,                                       ///< \brief LeftValue > RightValue
    WX_VARIANT_CANNOT_COMPARE   =  2                                        ///< \brief cannot compare variants (unknown type)
};
EVariantComparison CompareVariants(const wxVariant &vValue1,
                                   const wxVariant &vValue2,
                                   bool bAssumeSameType = true);            ///< \brief compare 2 variants
int                 IsVariantLessThan(const wxVariant &vValue1,
                                      const wxVariant &vValue2,
                                      bool bAssumeSameType = true);          ///< \brief return true if vValue1 < vValue2

//--------------------- HASHING -------------------------------------//
struct WXDLLIMPEXP_VDV wxVariantHash
{
    wxVariantHash() { }
    unsigned long operator()( wxVariant x ) const
    { return wxStringHash::stringHash(GetStringValue(x).wx_str()); }

    wxVariantHash& operator=(const wxVariantHash&) { return *this; }
};

struct WXDLLIMPEXP_VDV wxVariantEqual
{
    wxVariantEqual() { }
    bool operator()( wxVariant a, wxVariant b ) const
    {
        EVariantComparison eRes = CompareVariants(a, b);
        if (eRes == WX_VARIANT_CANNOT_COMPARE)
        {
            wxString s1 = GetStringValue(a);
            wxString s2 = GetStringValue(b);
            return(s1 == s2);
        }
        if (eRes == WX_VARIANT_EQUAL) return(true);
        return(false);
    }

    wxVariantEqual& operator=(const wxVariantEqual&) { return *this; }
};

//----------------- INLINE IMPLEMENTATION ---------------------------//
/** Convert variant to a specified type
  * \param rvValue [input]: the variant to convert
  * \return the converted value
  */
template<typename T>
WX_VDV_INLINE T wxGetVariantValueAs(const wxVariant &rvValue)
{
    return((T) rvValue);
}

//strings specialization
template<>
WX_VDV_INLINE wxString wxGetVariantValueAs(const wxVariant &rvValue)
{
    return(GetStringValue(rvValue));
}

//macro
#define WX_DEFINE_VARIANT_CONVERSION(TYPE, METHOD)                      \
    template<>                                                          \
    WX_VDV_INLINE TYPE wxGetVariantValueAs(const wxVariant &rvValue)    \
    {                                                                   \
        return((TYPE) rvValue.METHOD());                                \
    }

//signed integers
WX_DEFINE_VARIANT_CONVERSION(char, GetChar);
WX_DEFINE_VARIANT_CONVERSION(short, GetLong);
WX_DEFINE_VARIANT_CONVERSION(int, GetLong);
WX_DEFINE_VARIANT_CONVERSION(long, GetLong);
WX_DEFINE_VARIANT_CONVERSION(wxLongLong, GetLongLong().ToLong);

//unsigned integers
WX_DEFINE_VARIANT_CONVERSION(unsigned char, GetLong);
WX_DEFINE_VARIANT_CONVERSION(unsigned short, GetLong);
WX_DEFINE_VARIANT_CONVERSION(unsigned int, GetULongLong().ToULong);
WX_DEFINE_VARIANT_CONVERSION(unsigned long, GetULongLong().ToULong);
WX_DEFINE_VARIANT_CONVERSION(wxULongLong, GetULongLong().ToULong);

//floats
WX_DEFINE_VARIANT_CONVERSION(float, GetDouble);
WX_DEFINE_VARIANT_CONVERSION(double, GetDouble);

//date/time
WX_DEFINE_VARIANT_CONVERSION(wxDateTime, GetDateTime);

#undef WX_DEFINE_VARIANT_CONVERSION

#endif
