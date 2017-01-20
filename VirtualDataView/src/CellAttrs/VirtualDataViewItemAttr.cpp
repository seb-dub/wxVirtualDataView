/**********************************************************************/
/** FILE    : VirtualDataViewItemAttr.cpp                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a data model                              **/
/**********************************************************************/

#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/settings.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
    #include <wx/dataview.h>
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

//----------- CONSTRUCTORS & DESTRUCTOR -----------------------------//
/** Copy constructor
  */
wxVirtualDataViewItemAttr::wxVirtualDataViewItemAttr(const wxVirtualDataViewItemAttr &rhs)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewItemAttr& wxVirtualDataViewItemAttr::operator=(const wxVirtualDataViewItemAttr &rhs)
{
    if (&rhs == this) return(*this);
    Release();
    Copy(rhs);
    return(*this);
}

/** Destructor
  */
wxVirtualDataViewItemAttr::~wxVirtualDataViewItemAttr(void)
{
    Release();
}

//--------------------------- INTERNAL METHODS ----------------------//
/** Release the object
  */
void wxVirtualDataViewItemAttr::Release(void)
{
}

/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewItemAttr::Copy(const wxVirtualDataViewItemAttr &rhs)
{
    m_cForeColour       = rhs.m_cForeColour;
    m_cBackColour       = rhs.m_cBackColour;
    m_Font              = rhs.m_Font;
    m_bIsBold           = rhs.m_bIsBold;
    m_bIsItalic         = rhs.m_bIsItalic;
}


//------------------- DEFAULT ---------------------------------------//
/** Get the default object - static
  * \return the default attributes. Use system values
  */
wxVirtualDataViewItemAttr wxVirtualDataViewItemAttr::GetDefault(void)
{
    wxVirtualDataViewItemAttr attr;
    attr.m_cBackColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    attr.m_cForeColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    attr.m_Font        = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    attr.m_bIsBold     = false;
    attr.m_bIsItalic   = false;

    return(attr);
}

//-------------------------- STATUS ---------------------------------//
/** Clear the attribute
  */
void wxVirtualDataViewItemAttr::Clear(void)
{
    m_cForeColour = wxColour();
    m_cBackColour = wxColour();
    m_Font       = wxFont();
    m_bIsBold    = false;
    m_bIsItalic  = false;
}

//---------------------- FONT ATTRIBUTES ----------------------------//
/** Set the user defined font
  * \param rFont [input] : the font to use
  */
void wxVirtualDataViewItemAttr::SetFont(const wxFont &rFont)
{
    m_Font = rFont;
}

/** Return the font to apply
  * \param rFont [input]: the base font to modify
  * \return the font to apply
  */
wxFont wxVirtualDataViewItemAttr::GetEffectiveFont(const wxFont& rFont) const
{
    if (m_Font.IsOk())
    {
        wxFont f(m_Font);
        if (m_bIsBold)      f.MakeBold();
        if (m_bIsItalic)    f.MakeItalic();
        return(f);
    }

    wxFont fDefault(rFont);
    if (m_bIsBold)      fDefault.MakeBold();
    if (m_bIsItalic)    fDefault.MakeItalic();
    return(fDefault);
}

//------------------------ COMBINE ----------------------------------//
/** Combine this attribute with another one
  * \param rhs [input]: the 2nd attribute
  * The 1st attribute (this one) takes precedence.
  * For example if both this and rhs have a font defined, the font defined
  * in THIS OBJECT is used
  */
void wxVirtualDataViewItemAttr::Combine(wxVirtualDataViewItemAttr *pAttr)
{
    if (pAttr) Combine(*pAttr);
}

/** Combine this attribute with another one
  * \param rhs [input]: the 2nd attribute
  * The 1st attribute (this one) takes precedence.
  * For example if both this and rhs have a font defined, the font defined
  * in THIS OBJECT is used
  */
void wxVirtualDataViewItemAttr::Combine(const wxVirtualDataViewItemAttr &rhs)
{
    if (!m_cForeColour.IsOk()) m_cForeColour = rhs.m_cForeColour;
    if (!m_cBackColour.IsOk()) m_cBackColour = rhs.m_cBackColour;
    if (!m_Font.IsOk())        m_Font        = rhs.m_Font;
    if (!m_bIsBold)            m_bIsBold     = rhs.m_bIsBold;
    if (!m_bIsItalic)          m_bIsItalic   = rhs.m_bIsItalic;
}

//----------------------- COMPARE -----------------------------------//
/** Check if the 2 attributes are identical
  * \param rhs [input] : the 2nd attribute
  * \return true if the 2 attributes are identical, false otherwise
  */
bool wxVirtualDataViewItemAttr::IsSame(const wxVirtualDataViewItemAttr &rhs) const
{
    if (m_cForeColour != rhs.m_cForeColour)  return(false);
    if (m_cBackColour != rhs.m_cBackColour)  return(false);
    if (m_Font        != rhs.m_Font)         return(false);
    if ((m_bIsBold) && (!rhs.m_bIsBold))     return(false);
    if ((!m_bIsBold) && (rhs.m_bIsBold))     return(false);
    if ((m_bIsItalic) && (!rhs.m_bIsItalic)) return(false);
    if ((!m_bIsItalic) && (rhs.m_bIsItalic)) return(false);

    return(true);
}

//------------- COMPATIBILITY WITH wxDataViewCtrl -------------------//
#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
/** Constructor from a wxDataViewAttr
  * \param rAttr [input]: the object to copy
  */
wxVirtualDataViewItemAttr::wxVirtualDataViewItemAttr(const wxDataViewItemAttr &rAttr)
    : m_cForeColour(rAttr.GetColour()),
      m_cBackColour(rAttr.GetBackgroundColour()),
      m_Font(),
      m_bIsBold(rAttr.GetBold()),
      m_bIsItalic(rAttr.GetItalic())
{

}

/** Set from a wxVirtualDataViewItemAttr
  * \param rAttr [input]: the object to copy
  */
void wxVirtualDataViewItemAttr::FromDataViewAttr(const wxVirtualDataViewItemAttr &rAttr)
{
    m_cForeColour = rAttr.GetColour();
    m_cBackColour = rAttr.GetBackgroundColour();
    m_Font        = wxFont();
    m_bIsBold     = rAttr.GetBold();
    m_bIsItalic   = rAttr.GetItalic();
}

/** Convert to a wxVirtualDataViewItemAttr
  * \return the wxDataViewItemAttr
  */
wxVirtualDataViewItemAttr wxVirtualDataViewItemAttr::ToDataViewAttr(void) const
{
    wxVirtualDataViewItemAttr oAttr;
    oAttr.SetColour(m_cForeColour);
    oAttr.SetBackgroundColour(m_cBackColour);
    oAttr.SetBold(m_bIsBold);
    oAttr.SetItalic(m_bIsItalic);
    return(oAttr);
}

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
