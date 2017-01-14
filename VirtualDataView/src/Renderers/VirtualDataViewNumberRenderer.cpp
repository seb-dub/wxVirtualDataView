/**********************************************************************/
/** FILE    : VirtualDataViewNumberRenderer.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view text renderer                              **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewNumberRenderer.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/dc.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param eMode   [input]: the ellipsization mode
  * \param eHAlign [input]: the horizontal alignment
  * \param eVAlign [input]: the vertical alignment
  */
wxVirtualDataViewNumberRenderer::wxVirtualDataViewNumberRenderer(bool bFloat,
                                                                 EEllipsizeMode eMode,
                                                                 EHAlign eHAlign,
                                                                 EVAlign eVAlign)
    : wxVirtualDataViewRenderer(),
      m_bFloat(bFloat),
      m_iFloatingPointFormat(WX_FP_FORMAT_COMPACT),
      m_bPrependPlusSign(false),
      m_bUnsigned(false),
      m_bUseZeroAsPadChar(false),
      m_eEllipsizeMode(eMode),
      m_eHorizontalAlignment(eHAlign),
      m_eVerticalAlignment(eVAlign),
      m_iPrecision(0),
      m_iWidth(0)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewNumberRenderer::~wxVirtualDataViewNumberRenderer(void)
{

}

//------------------------ INTERNAL METHODS -------------------------//
/** Convert the variant to a string
  */
wxString wxVirtualDataViewNumberRenderer::ConvertToString(const wxVariant &rvValue) const
{
    //common formatting options
    wxString sFormatString = "%";
    //'+' sign
    if (m_bPrependPlusSign) sFormatString += "+";

    //alternative form
    if ((m_iFloatingPointFormat & WX_FP_FORMAT_SHOW_ALWAYS_DECIMAL_SEPARATOR) && (m_bFloat))
    {
        sFormatString += "#";
    }

    //padding char
    if (m_bUseZeroAsPadChar) sFormatString += "0";

    //width
    if (m_iWidth > 0) sFormatString += wxString::Format("%d", m_iWidth);

    //precision
    if (m_iPrecision > 0) sFormatString += wxString::Format(".%d", m_iPrecision);

    //decimal conversion
    if (m_bFloat)
    {
        double f = rvValue.GetDouble();

        //get format string
        switch(m_iFloatingPointFormat & 0x00000003)
        {
            case WX_FP_FORMAT_DECIMAL   :  sFormatString += "f"; break;
            case WX_FP_FORMAT_SCIENTIFIC:  sFormatString += "e"; break;
            case WX_FP_FORMAT_COMPACT   :  sFormatString += "g"; break;
            default                     :  sFormatString += "f"; break;
        }

        //upper / lower case
        bool bUpperCase = m_iFloatingPointFormat & WX_FP_FORMAT_UPPER_CASE;
        if (bUpperCase) sFormatString.MakeUpper();

        return(wxString::Format(sFormatString, f));
    }
    else if (m_bUnsigned)
    {
        wxULongLong u = rvValue.GetULongLong();

        sFormatString += "llu";
        return(wxString::Format(sFormatString, u));
    }
    else
    {
        wxLongLong l = rvValue.GetLongLong();

        sFormatString += "lld";
        return(wxString::Format(sFormatString, l));
    }
    return(wxEmptyString);
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewNumberRenderer::Clone(void) const
{
    wxVirtualDataViewNumberRenderer *pResult;
    pResult = new wxVirtualDataViewNumberRenderer(m_bFloat,
                                                  (EEllipsizeMode) m_eEllipsizeMode,
                                                  (EHAlign) m_eHorizontalAlignment,
                                                  (EVAlign) m_eVerticalAlignment);
    if (pResult)
    {
        pResult->CopyBaseRenderer(*this);
        pResult->m_iFloatingPointFormat = m_iFloatingPointFormat;
        pResult->m_bPrependPlusSign     = m_bPrependPlusSign;
        pResult->m_bUnsigned            = m_bUnsigned;
        pResult->m_bUseZeroAsPadChar    = m_bUseZeroAsPadChar;
        pResult->m_iPrecision           = m_iPrecision;
        pResult->m_iWidth               = m_iWidth;
    }
    return(pResult);
}

/** Render the item
  * \param pWindow [input]: the window where the rendering occurs
  * \param rRect   [input]: the clipping rectangle
  * \param rDC     [input]: the device context to use for drawing
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  */
void wxVirtualDataViewNumberRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                             wxDC &rDC,
                                             wxVirtualDataViewItemAttr *pAttr,
                                             const wxVirtualDataViewItemState &rState,
                                             const wxVariant& vValue)
{
    wxString sValue = ConvertToString(vValue);

    //background
    RenderBackground(pWindow, rRect, rDC, pAttr);

    //text
    RenderText(pWindow, rRect, rDC, sValue,
               0, 0,
               pAttr, rState,
               (EHAlign) m_eHorizontalAlignment,
               (EVAlign) m_eVerticalAlignment,
               (EEllipsizeMode) m_eEllipsizeMode);
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewNumberRenderer::GetSize(wxWindow *pWindow,
                                              wxVirtualDataViewItemAttr *pAttr,
                                              const wxVirtualDataViewItemState &rState,
                                              const wxVariant& vValue) const
{
    wxString sValue = ConvertToString(vValue);
    return(GetTextExtent(pWindow, sValue, pAttr));
}

//-------------- ALIGNMENT ------------------------------------------//
/** Set alignment
  * \param eHAlign [input]: the new horizontal alignment
  * \param eVAlign [input]: the new vertical alignment
  */
void wxVirtualDataViewNumberRenderer::SetAlignment(EHAlign eHAlign, EVAlign eVAlign)
{
    m_eHorizontalAlignment = eHAlign;
    m_eVerticalAlignment   = eVAlign;
}

/** Get alignment
  * \param eHAlign [output]: the horizontal alignment
  * \param eVAlign [output]: the vertical alignment
  */
void wxVirtualDataViewNumberRenderer::GetAlignment(EHAlign &eHAlign, EVAlign &eVAlign) const
{
    eHAlign = (EHAlign) m_eHorizontalAlignment;
    eVAlign = (EVAlign) m_eVerticalAlignment;
}

//--------------------- ELLIPSIZATION -------------------------------//
/** Set ellipsization mode
  * \param eMode [input]: the new mode
  */
void wxVirtualDataViewNumberRenderer::SetEllipsizeMode(EEllipsizeMode eMode)
{
    m_eEllipsizeMode = eMode;
}

/** Get ellipsization mode
  * \return the current mode
  */
wxVirtualDataViewRenderer::EEllipsizeMode
 wxVirtualDataViewNumberRenderer::GetEllipsizeMode(void) const
{
    return((EEllipsizeMode) m_eEllipsizeMode);
}

//--------------------- FLOATING POINT / INTEGER --------------------//
/** Return true if the format is floating point
  * \return true if the format is floating point
  *         false if the format is integer
  */
bool wxVirtualDataViewNumberRenderer::IsFloatingPoint(void) const
{
    return(m_bFloat);
}

/** Return true if the format is integer
  * \return true if the format is integer
  *         false if the format is floating point
  */
bool wxVirtualDataViewNumberRenderer::IsInteger(void) const
{
    return(!m_bFloat);
}

/** Set floating point / integer
  * \param bFloat [input]: true for floating point format
  *                        false for integer format
  */
void wxVirtualDataViewNumberRenderer::SetFloatingPoint(bool bFloat)
{
    m_bFloat = bFloat;
}

/** Set integer / floating point
  * \param bInteger [input]: true for integer format
  *                          false for floating point format
  */
void wxVirtualDataViewNumberRenderer::SetInteger(bool bInteger)
{
    m_bFloat = !bInteger;
}

//--------------------- FLOATING POINT OPTIONS ----------------------//
/** Get the floating point format
  * \return the floating point formatting options. This is a combination of EFormat
  */
int wxVirtualDataViewNumberRenderer::GetFloatingPointFormat(void) const
{
    return(m_iFloatingPointFormat);
}

/** Set the floating point format
  * \param iFormat [input]: the new floating point format. Combination of EFormat
  */
void wxVirtualDataViewNumberRenderer::SetFloatingPointFormat(int iFormat)
{
    m_iFloatingPointFormat = iFormat;
}

//----------------------- PRECISION ---------------------------------//
/** Get the minimal amount of digits to write
  * \return the minimal amount of digits to write.
  *         0 means default
  *         for floating point, this is the amount of digits after decimal separator
  *         for integer, this is the total amount of digits
  */
int wxVirtualDataViewNumberRenderer::GetPrecision(void) const
{
    return(m_iPrecision);
}

/** Set the minimal amount of digits to write
  * \param iPrecision [input]: the minimal amount of digits to write.
  *                            0 or less means default
  *         for floating point, this is the amount of digits after decimal separator
  *         for integer, this is the total amount of digits
  */
void wxVirtualDataViewNumberRenderer::SetPrecision(int iPrecision)
{
    if (iPrecision < 0) m_iPrecision = 0;
    else                m_iPrecision = iPrecision;
}

//---------------------- WIDTH --------------------------------------//
/** Get the amount of characters to print
  * \return the amount of characters to print. 0 means default (no limits)
  */
int wxVirtualDataViewNumberRenderer::GetWidth(void) const
{
    return(m_iWidth);
}

/** Set the amount of characters to print
  * \param iWidth [input] : the amount of characters to print. 0 means default (no limits)
  */
void wxVirtualDataViewNumberRenderer::SetWidth(int iWidth)
{
    if (iWidth < 0) m_iWidth = 0;
    else            m_iWidth = iWidth;

}

/** Return true if '0' is used as padding character
  * \return true if '0' is used as padding character
  *         false if space is used as padding character
  */
bool wxVirtualDataViewNumberRenderer::IsZeroUsedAsPaddingChar(void) const
{
    return(m_bUseZeroAsPadChar);
}

/** Return true if ' ' is used as padding character
  * \return true if space is used as padding character
  *         false if '0' is used as padding character
  */
bool wxVirtualDataViewNumberRenderer::IsSpaceUsedAsPaddingChar(void) const
{
    return(!m_bUseZeroAsPadChar);
}

/** Toggle '0'/' ' as padding character
  * \param bUseZero [input]: true for using '0' as padding character
  *                          false for using space as padding character
  */
void wxVirtualDataViewNumberRenderer::SetUseZeroAsPaddingChar(bool bUseZero)
{
    m_bUseZeroAsPadChar = bUseZero;
}

/** Toggle ' '/'0' as padding character
  * \param bUseSpace [input]: true for using space as padding character
  *                           false for using '0' as padding character
  */
void wxVirtualDataViewNumberRenderer::SetUseSpaceAsPaddingChar(bool bUseSpace)
{
    m_bUseZeroAsPadChar = !bUseSpace;
}

//----------------------------- PLUS SIGN ---------------------------//
/** Return true if '+' sign is prepended on positive numbers
  * \return true if '+' sign is prepended on positive numbers
  *         false if no sign is prepended on positive numbers
  */
bool wxVirtualDataViewNumberRenderer::IsPlusSignAlwaysShown(void) const
{
    return(m_bPrependPlusSign);
}

/** Toggle '+' sign ON/OFF on positive numbers
  * \param bShow [input]: true for prepending '+' sign on positive numbers
  *                       false otherwise
  */
void wxVirtualDataViewNumberRenderer::SetPlusSignAlwaysShown(bool bShow)
{
    m_bPrependPlusSign = bShow;
}

/** Toggle '+' sign OFF/ON on positive numbers
  * \param bHidden [input]: true for hidding '+' sign on positive numbers
  *                         false for always showing it
  */
void wxVirtualDataViewNumberRenderer::SetPlusSignHidden(bool bHidden)
{
    m_bPrependPlusSign = !bHidden;
}

//------------------------ SIGNED / UNSIGNED ------------------------//
/** Return true if the number is signed (integer format only)
  * \return true if the number is signed
  *         false if the number is unsigned
  */
bool wxVirtualDataViewNumberRenderer::IsSigned(void) const
{
    return(!m_bUnsigned);
}

/** Return true if the number is unsigned (integer format only)
  * \return true if the number is unsigned
  *         false if the number is signed
  */
bool wxVirtualDataViewNumberRenderer::IsUnsigned(void) const
{
    return(m_bUnsigned);
}

/** Toggle signed/unsigned (integer format only)
  * \param bSigned [input]: true for signed integers
  *                         false for unsigned integers
  */
void wxVirtualDataViewNumberRenderer::SetSigned(bool bSigned)
{
    m_bUnsigned = !bSigned;
}

/** Toggle unsigned/signed
  * \param bUnsigned [input]: true for unsigned integers
  *                           false for signed integers
  */
void wxVirtualDataViewNumberRenderer::SetUnsigned(bool bUnsigned)
{
    m_bUnsigned = bUnsigned;
}

