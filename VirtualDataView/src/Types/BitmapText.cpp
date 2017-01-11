/**********************************************************************/
/** FILE    : BitmapText.cpp                                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : an association of bitmap + text                        **/
/**********************************************************************/


#include <wx/VirtualDataView/Types/BitmapText.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapText, wxObject);

IMPLEMENT_VARIANT_OBJECT_EXPORTED(wxBitmapText, WXDLLIMPEXP_VDV)

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  * \param sText   [input]: the text
  * \param rIcon   [input]: the icon
  */
wxBitmapText::wxBitmapText(const wxString &sText, const wxIcon &rIcon)
    : wxObject(),
      m_sText(sText),
      m_Icon(rIcon)
{
}

/** Default constructor
  * \param sText   [input]: the text
  * \param rBitmap [input]: the image
  */
wxBitmapText::wxBitmapText(const wxString &sText, const wxBitmap &rBitmap)
    : wxObject(),
      m_sText(sText),
      m_Icon()
{
    m_Icon.CopyFromBitmap(rBitmap);
}

/** Copy constructor
  */
wxBitmapText::wxBitmapText(const wxBitmapText &rhs)
    : wxObject(rhs),
      m_sText(rhs.m_sText),
      m_Icon(rhs.m_Icon)
{
}

/** Assignation operator
  */
wxBitmapText& wxBitmapText::operator=(const wxBitmapText &rhs)
{
    m_sText     = rhs.m_sText;
    m_Icon      = rhs.m_Icon;
    return(*this);
}

/** Destructor
  */
wxBitmapText::~wxBitmapText(void)
{

}

//----------------------- TEXT --------------------------------------//
/** Set the text
  * \param sText [input]: the new text
  */
void wxBitmapText::SetText(const wxString &sText)
{
    m_sText = sText;
}

/** Get the text
  * \return the text
  */
wxString wxBitmapText::GetText(void) const
{
    return(m_sText);
}

//-------------------------- BITMAP ---------------------------------//
/** Set the bitmap
  * \param rBitmap [input]: the bitmap
  */
void wxBitmapText::SetBitmap(const wxBitmap &rBitmap)
{
    m_Icon.CopyFromBitmap(rBitmap);
}

/** Set the icon
  * \param rIcon [input]: the new icon
  */
void wxBitmapText::SetIcon(const wxIcon &rIcon)
{
    m_Icon = rIcon;
}

/** Get the icon
  * \return the icon
  */
wxIcon& wxBitmapText::GetIcon(void)
{
    return(m_Icon);
}

/** Get the icon - const version
  * \return the icon
  */
const wxIcon& wxBitmapText::GetIcon(void) const
{
    return(m_Icon);
}

//------------------------- OPERATORS -------------------------------//
/** Comparison test
  * \param rhs [input]: the 2nd object
  * \return true if the 2 objects are identical, false otherwise
  */
bool wxBitmapText::IsSameAs(const wxBitmapText &rhs) const
{
    if (m_sText != rhs.m_sText) return(false);
    if (!m_Icon.IsSameAs(rhs.m_Icon)) return(false);
    return(true);
}

/** Operator ==
  */
bool wxBitmapText::operator==(const wxBitmapText &rhs) const
{
    return(IsSameAs(rhs));
}

/** Operator !=
  */
bool wxBitmapText::operator!=(const wxBitmapText& rhs) const
{
    return(!IsSameAs(rhs));
}
