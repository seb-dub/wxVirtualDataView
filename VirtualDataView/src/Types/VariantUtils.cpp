/**********************************************************************/
/** FILE    : VariantUtils.cpp                                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a set of utilities for wxVariant                       **/
/**********************************************************************/


#include <wx/VirtualDataView/Types/VariantUtils.h>

/** Convert a variant to string
  * \param rvValue [input]: the variant to convert
  * \return the value as a string
  */
wxString GetStringValue(const wxVariant &rvValue)
{
    if (rvValue.GetType() == "wxBitmapText")
    {
        wxBitmapText bt;
        bt << rvValue;
        return(bt.GetText());
    }
    return(rvValue.GetString());
}

//----------------- VARIANT COMPARISON METHOD -----------------------//
bool IsNumericType(const wxString &sType)
{
    if (sType == "bool")        return(true); //can convert to 0 or 1
    if (sType == "char")        return(true);
    if (sType == "datetime")    return(true);
    if (sType == "double")      return(true);
    if (sType == "list")        return(false);
    if (sType == "long")        return(true);
    if (sType == "longlong")    return(true);
    if (sType == "string")      return(false);
    if (sType == "ulonglong")   return(true);
    if (sType == "arrstring")   return(false);
    if (sType == "void*")       return(true);
    return(false);
}

/** Compare 2 variants. Both variants are assumed to be of the same type
  * \param vValue1          [input]: the 1st variant
  * \param vValue2          [input]: the 2nd variant
  * \param bAssumeSameType  [input]: if true, both variant are assumed to be of the same type,
  *                                  and the type of vValue1 is used for the comparison type.
  *                                  if false, both variants type are checked, and
  *                                  WX_VARIANT_CANNOT_COMPARE is returned if they are incompatible
  * \return WX_VARIANT_LESS_THAN     if vValue1 < vValue2
  *         WX_VARIANT_EQUAL         if vValue1 == vValue2
  *         WX_VARIANT_GREATER_THAN  if vValue1 > vValue2
  *         WX_VARIANT_CANNOT_COMPARE if one of the variant has an unknown type
  */
EVariantComparison CompareVariants(const wxVariant &vValue1, const wxVariant &vValue2,
                                   bool bAssumeSameType)
{
    wxString sType1 = vValue1.GetType();
    if (!bAssumeSameType)
    {
        wxString sType2 = vValue2.GetType();
        if (sType1 != sType2)
        {
            bool bNumeric1 = IsNumericType(sType1);
            bool bNumeric2 = IsNumericType(sType2);
            if ((bNumeric1) && (!bNumeric2)) return(WX_VARIANT_CANNOT_COMPARE);
            if ((!bNumeric1) && (bNumeric2)) return(WX_VARIANT_CANNOT_COMPARE);
        }
    }

    if (sType1 == wxT("string"))
    {
        wxString str1 = vValue1.GetString();
        wxString str2 = vValue2.GetString();
        int res = str1.Cmp(str2);
        if (res < 0) return(WX_VARIANT_LESS_THAN); //str1.Cmp(str2) returns a positive value if str1 > str2
        if (res > 0) return(WX_VARIANT_GREATER_THAN);
        return(WX_VARIANT_EQUAL);
    }
    else if (sType1 == wxT("long"))
    {
        long l1 = vValue1.GetLong();
        long l2 = vValue2.GetLong();
        if (l1 < l2) return(WX_VARIANT_LESS_THAN);
        if (l1 > l2) return(WX_VARIANT_GREATER_THAN);
        return(WX_VARIANT_EQUAL);
    }
    else if (sType1 == wxT("double"))
    {
        double f1 = vValue1.GetDouble();
        double f2 = vValue2.GetDouble();
        if (f1 < f2) return(WX_VARIANT_LESS_THAN);
        if (f1 > f2) return(WX_VARIANT_GREATER_THAN);
        return(WX_VARIANT_EQUAL);
    }

#if wxUSE_DATETIME
    else if (sType1 == wxT("datetime"))
    {
        wxDateTime dt1 = vValue1.GetDateTime();
        wxDateTime dt2 = vValue2.GetDateTime();
        if (dt1.IsEarlierThan(dt2)) return(WX_VARIANT_LESS_THAN);
        if (dt2.IsEarlierThan(dt1)) return(WX_VARIANT_GREATER_THAN);
        return(WX_VARIANT_EQUAL);
    }
#endif // wxUSE_DATETIME

    else if (sType1 == wxT("bool"))
    {
        bool b1 = vValue1.GetBool();
        bool b2 = vValue2.GetBool();

        //false < true
        if ((!b1) && (b2)) return(WX_VARIANT_LESS_THAN);
        if ((b1) && (!b2)) return(WX_VARIANT_GREATER_THAN);
        return(WX_VARIANT_EQUAL);
    }
    else if (sType1 == wxT("wxBitmapText"))
    {
        wxBitmapText bt1;
        wxBitmapText bt2;

        bt1 << vValue1;
        bt2 << vValue2;

        int res = bt1.GetText().Cmp(bt2.GetText());
        if (res < 0) return(WX_VARIANT_LESS_THAN); //str1.Cmp(str2) returns a positive value if str1 > str2
        if (res > 0) return(WX_VARIANT_GREATER_THAN);
        return(WX_VARIANT_EQUAL);
    }

    return(WX_VARIANT_CANNOT_COMPARE);
}

/** Return true if vValue1 < vValue2
  * \param vValue1          [input]: the 1st variant
  * \param vValue2          [input]: the 2nd variant
  * \param bAssumeSameType  [input]: if true, both variant are assumed to be of the same type,
  *                                  and the type of vValue1 is used for the comparison type.
  *                                  if false, both variants type are checked, and
  *                                  -1 is returned if they are incompatible
  * \return 1 if vValue1 < vValue2
  *         0 if vValue1 >= vValue2
  *         -1 if both types are incompatible
  */
int IsVariantLessThan(const wxVariant &vValue1, const wxVariant &vValue2, bool bAssumeSameType)
{
    wxString sType1 = vValue1.GetType();
    if (!bAssumeSameType)
    {
        wxString sType2 = vValue2.GetType();
        if (sType1 != sType2)
        {
            bool bNumeric1 = IsNumericType(sType1);
            bool bNumeric2 = IsNumericType(sType2);
            if ((bNumeric1) && (!bNumeric2)) return(-1);
            if ((!bNumeric1) && (bNumeric2)) return(-1);
        }
    }

    if (sType1 == wxT("string"))
    {
        wxString str1 = vValue1.GetString();
        wxString str2 = vValue2.GetString();
        int res = str1.Cmp(str2);
        if (res < 0) return(1); //str1.Cmp(str2) returns a positive value if str1 > str2
        return(0);
    }
    else if (sType1 == wxT("long"))
    {
        long l1 = vValue1.GetLong();
        long l2 = vValue2.GetLong();
        if (l1 < l2) return(1);
        return(0);
    }
    else if (sType1 == wxT("double"))
    {
        double f1 = vValue1.GetDouble();
        double f2 = vValue2.GetDouble();
        if (f1 < f2) return(1);
        return(0);
    }

#if wxUSE_DATETIME
    else if (sType1 == wxT("datetime"))
    {
        wxDateTime dt1 = vValue1.GetDateTime();
        wxDateTime dt2 = vValue2.GetDateTime();
        if (dt1.IsEarlierThan(dt2)) return(1);
        return(0);
    }
#endif // wxUSE_DATETIME

    else if (sType1 == wxT("bool"))
    {
        bool b1 = vValue1.GetBool();
        bool b2 = vValue2.GetBool();

        //false < true
        if ((!b1) && (b2)) return(1);
        return(0);
    }
    else if (sType1 == wxT("wxBitmapText"))
    {
        wxBitmapText bt1;
        wxBitmapText bt2;

        bt1 << vValue1;
        bt2 << vValue2;

        int res = bt1.GetText().Cmp(bt2.GetText());
        if (res < 0) return(1); //str1.Cmp(str2) returns a positive value if str1 > str2
        return(0);
    }

    return(-1);
}
