/**********************************************************************/
/** FILE    : VirtualDataViewDateTimeRenderer.cpp                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view date/time renderer                         **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewDateTimeRenderer.h>
#include <wx/datetime.h>
#include <wx/variant.h>
#include <wx/gdicmn.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param bISOFormat [input]: true if ISO format is false (YYYY-MM-DD)
  *                            false if local format is used
  * \param eMode      [input]: the ellipsization mode
  * \param eHAlign    [input]: the horizontal alignment
  * \param eVAlign    [input]: the vertical alignment
  */
wxVirtualDataViewDateTimeRenderer::wxVirtualDataViewDateTimeRenderer(bool bISOFormat,
                                                                     EEllipsizeMode eMode,
                                                                     EHAlign eHAlign,
                                                                     EVAlign eVAlign)
    : wxVirtualDataViewTextRenderer(eMode, eHAlign, eVAlign),
      m_bISOFormat(bISOFormat)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewDateTimeRenderer::~wxVirtualDataViewDateTimeRenderer(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Convert the variant to string
  * \param rvValue [input]: the value to convert
  * \return the converted value
  */
wxString wxVirtualDataViewDateTimeRenderer::ConvertToString(const wxVariant &rvValue) const
{
    wxDateTime dt = rvValue.GetDateTime();
    if (m_bISOFormat) return(dt.FormatISOCombined());
    return(dt.Format());
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewDateTimeRenderer::Clone(void) const
{
    wxVirtualDataViewDateTimeRenderer *pResult;
    pResult = new wxVirtualDataViewDateTimeRenderer(m_bISOFormat,
                                                    m_eEllipsizeMode,
                                                    m_eHorizontalAlignment,
                                                    m_eVerticalAlignment);
    if (pResult) pResult->CopyBaseRenderer(*this);
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
void wxVirtualDataViewDateTimeRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
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
               m_eHorizontalAlignment,
               m_eVerticalAlignment,
               m_eEllipsizeMode);
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewDateTimeRenderer::GetSize(wxWindow *pWindow,
                                                  wxVirtualDataViewItemAttr *pAttr,
                                                  const wxVirtualDataViewItemState &rState,
                                                  const wxVariant& vValue) const
{
    wxString sValue = ConvertToString(vValue);
    return(GetTextExtent(pWindow, sValue, pAttr));
}

//----------------------------- OPTIONS -----------------------------//
/** Return true if ISO format is used (YYYY-MM-DD)
  * \return true if ISO format is used (YYYY-MM-DD)
  *         false if local format is used
  */
bool wxVirtualDataViewDateTimeRenderer::UseISOFormat(void) const
{
    return(m_bISOFormat);
}

/** Use ISO format (YYYY-MM-DD) or local format
  * \param bISO [input]: true for using ISO format (YYYY-MM-DD)
  *                      false for using local format
  */
void wxVirtualDataViewDateTimeRenderer::SetUseISOFormat(bool bISO)
{
    m_bISOFormat = bISO;
}

/** Use local format
  */
void wxVirtualDataViewDateTimeRenderer::SetUseLocalFormat(void)
{
    m_bISOFormat = false;
}

