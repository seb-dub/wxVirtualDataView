/**********************************************************************/
/** FILE    : VirtualDataViewTextRenderer.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view text renderer                              **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewTextRenderer.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>
#include <wx/dc.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param eMode   [input]: the ellipsization mode
  * \param eHAlign [input]: the horizontal alignment
  * \param eVAlign [input]: the vertical alignment
  */
wxVirtualDataViewTextRenderer::wxVirtualDataViewTextRenderer(EEllipsizeMode eMode,
                                                             EHAlign eHAlign,
                                                             EVAlign eVAlign)
    : wxVirtualDataViewRenderer()
{

}

/** Destructor - virtual
  */
wxVirtualDataViewTextRenderer::~wxVirtualDataViewTextRenderer(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewTextRenderer::Clone(void) const
{
    wxVirtualDataViewTextRenderer *pResult;
    pResult = new wxVirtualDataViewTextRenderer(m_eEllipsizeMode,
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
void wxVirtualDataViewTextRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                           wxDC &rDC,
                                           wxVirtualDataViewItemAttr *pAttr,
                                           const wxVirtualDataViewItemState &rState,
                                           const wxVariant& vValue)
{
    wxString sValue = GetStringValue(vValue); //vValue.GetString();

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
wxSize wxVirtualDataViewTextRenderer::GetSize(wxWindow *pWindow,
                                              wxVirtualDataViewItemAttr *pAttr,
                                              const wxVirtualDataViewItemState &rState,
                                              const wxVariant& vValue) const
{
    wxString sValue = vValue.GetString();
    if (sValue == "") sValue = "AgMf0";
    return(GetTextExtent(pWindow, sValue, pAttr));
}

//-------------- ALIGNMENT ------------------------------------------//
/** Set alignment
  * \param eHAlign [input]: the new horizontal alignment
  * \param eVAlign [input]: the new vertical alignment
  */
void wxVirtualDataViewTextRenderer::SetAlignment(EHAlign eHAlign, EVAlign eVAlign)
{
    m_eHorizontalAlignment = eHAlign;
    m_eVerticalAlignment   = eVAlign;
}

/** Get alignment
  * \param eHAlign [output]: the horizontal alignment
  * \param eVAlign [output]: the vertical alignment
  */
void wxVirtualDataViewTextRenderer::GetAlignment(EHAlign &eHAlign, EVAlign &eVAlign) const
{
    eHAlign = m_eHorizontalAlignment;
    eVAlign = m_eVerticalAlignment;
}

//--------------------- ELLIPSIZATION -------------------------------//
/** Set ellipsization mode
  * \param eMode [input]: the new mode
  */
void wxVirtualDataViewTextRenderer::SetEllipsizeMode(EEllipsizeMode eMode)
{
    m_eEllipsizeMode = eMode;
}

/** Get ellipsization mode
  * \return the current mode
  */
wxVirtualDataViewRenderer::EEllipsizeMode
 wxVirtualDataViewTextRenderer::GetEllipsizeMode(void) const
{
    return(m_eEllipsizeMode);
}

