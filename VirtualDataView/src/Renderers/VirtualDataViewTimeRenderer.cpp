/**********************************************************************/
/** FILE    : VirtualDataViewTimeRenderer.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view time renderer                              **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewTimeRenderer.h>
#include <wx/datetime.h>
#include <wx/variant.h>
#include <wx/gdicmn.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param bISOFormat [input]: true if ISO format is false (HH:MM:SS)
  *                            false if local format is used
  * \param eMode      [input]: the ellipsization mode
  * \param eHAlign    [input]: the horizontal alignment
  * \param eVAlign    [input]: the vertical alignment
  */
wxVirtualDataViewTimeRenderer::wxVirtualDataViewTimeRenderer(bool bISOFormat,
                                                             EEllipsizeMode eMode,
                                                             EHAlign eHAlign,
                                                             EVAlign eVAlign)
    : wxVirtualDataViewTextRenderer(eMode, eHAlign, eVAlign),
      m_bISOFormat(bISOFormat)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewTimeRenderer::~wxVirtualDataViewTimeRenderer(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Convert the variant to string
  * \param rvValue [input]: the value to convert
  * \return the converted value
  */
wxString wxVirtualDataViewTimeRenderer::ConvertToString(const wxVariant &rvValue) const
{
    wxDateTime dt = rvValue.GetDateTime();
    if (m_bISOFormat) return(dt.FormatISOTime());
    return(dt.FormatTime());
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewTimeRenderer::Clone(void) const
{
    wxVirtualDataViewTimeRenderer *pResult;
    pResult = new wxVirtualDataViewTimeRenderer(m_bISOFormat,
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
void wxVirtualDataViewTimeRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
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
wxSize wxVirtualDataViewTimeRenderer::GetSize(wxWindow *pWindow,
                                              wxVirtualDataViewItemAttr *pAttr,
                                              const wxVirtualDataViewItemState &rState,
                                              const wxVariant& vValue) const
{
    wxString sValue = ConvertToString(vValue);
    return(GetTextExtent(pWindow, sValue, pAttr));
}

//----------------------------- OPTIONS -----------------------------//
/** Return true if ISO format is used (HH:MM:SS)
  * \return true if ISO format is used (HH:MM:SS)
  *         false if local format is used
  */
bool wxVirtualDataViewTimeRenderer::UseISOFormat(void) const
{
    return(m_bISOFormat);
}

/** Use ISO format (HH:MM:SS) or local format
  * \param bISO [input]: true for using ISO format (HH:MM:SS)
  *                      false for using local format
  */
void wxVirtualDataViewTimeRenderer::SetUseISOFormat(bool bISO)
{
    m_bISOFormat = bISO;
}

/** Use local format
  */
void wxVirtualDataViewTimeRenderer::SetUseLocalFormat(void)
{
    m_bISOFormat = false;
}

