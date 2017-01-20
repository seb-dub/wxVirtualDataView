/**********************************************************************/
/** FILE    : VirtualDataViewRendererCompat.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualDataViewRenderer wrapping a wxDVCRenderer   **/
/**********************************************************************/

#include <wx/VirtualDataView/Compatibility/VirtualDataViewRendererCompat.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
    #include <wx/dataview.h>
    #include <wx/renderer.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param eMode        [input]: the ellipsization mode
  * \param eHAlign      [input]: the horizontal alignment
  * \param eVAlign      [input]: the vertical alignment
  * \param pRenderer    [input]: the renderer to wrap
  */
wxVirtualDataViewRendererCompat::wxVirtualDataViewRendererCompat(wxDataViewCustomRenderer* pRenderer)
    : wxVirtualDataViewRenderer(),
      m_pRenderer(pRenderer)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewRendererCompat::~wxVirtualDataViewRendererCompat(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewRendererCompat::Clone(void) const
{
    wxVirtualDataViewRendererCompat *pResult;
    pResult = new wxVirtualDataViewRendererCompat(m_pRenderer);
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
void wxVirtualDataViewRendererCompat::Render(wxWindow *pWindow, const wxRect &rRect,
                                             wxDC &rDC,
                                             wxVirtualDataViewItemAttr *pAttr,
                                             const wxVirtualDataViewItemState &rState,
                                             const wxVariant& vValue)
{
    if (!m_pRenderer) return;
    m_pRenderer->SetValue(vValue);
    int iState = 0;
    if (rState.IsSelected())  iState |= wxDATAVIEW_CELL_SELECTED;
    if (rState.IsFocused())   iState |= wxDATAVIEW_CELL_FOCUSED;
    if (rState.IsDisabled())  iState |= wxDATAVIEW_CELL_INSENSITIVE;
    m_pRenderer->Render(rRect, &rDC, iState);
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewRendererCompat::GetSize(wxWindow *pWindow,
                                                wxVirtualDataViewItemAttr *pAttr,
                                                const wxVirtualDataViewItemState &rState,
                                                const wxVariant& vValue) const
{
    if (!m_pRenderer) return(wxDefaultSize);
    return(m_pRenderer->GetSize());
}

//-------------- ALIGNMENT ------------------------------------------//
/** Set alignment
  * \param eHAlign [input]: the new horizontal alignment
  * \param eVAlign [input]: the new vertical alignment
  */
void wxVirtualDataViewRendererCompat::SetAlignment(EHAlign eHAlign, EVAlign eVAlign)
{
    if (!m_pRenderer) return;
    int iAlign = 0;
    switch(eHAlign)
    {
        case WX_HORIZ_ALIGN_LEFT    : iAlign |= wxALIGN_LEFT; break;
        case WX_HORIZ_ALIGN_CENTRE  : iAlign |= wxALIGN_CENTRE; break;
        case WX_HORIZ_ALIGN_RIGHT   : iAlign |= wxALIGN_RIGHT; break;
        default                     : iAlign |= wxALIGN_LEFT; break;
    }

    switch(eVAlign)
    {
        case WX_VERT_ALIGN_TOP      : iAlign |= wxALIGN_TOP; break;
        case WX_VERT_ALIGN_CENTRE   : iAlign |= wxALIGN_CENTRE_VERTICAL; break;
        case WX_VERT_ALIGN_BOTTOM   : iAlign |= wxALIGN_BOTTOM; break;
        default                     : iAlign |= wxALIGN_TOP; break;
    }
    m_pRenderer->SetAlignment(iAlign);
}

/** Get alignment
  * \param eHAlign [output]: the horizontal alignment
  * \param eVAlign [output]: the vertical alignment
  */
void wxVirtualDataViewRendererCompat::GetAlignment(EHAlign &eHAlign, EVAlign &eVAlign) const
{
    eHAlign = WX_HORIZ_ALIGN_LEFT;
    eVAlign = WX_VERT_ALIGN_TOP;
    if (!m_pRenderer) return;

    int iAlign = m_pRenderer->GetAlignment();
         if (iAlign & wxALIGN_LEFT)             eHAlign = WX_HORIZ_ALIGN_LEFT;
    else if (iAlign & wxALIGN_CENTRE)           eHAlign = WX_HORIZ_ALIGN_CENTRE;
    else if (iAlign & wxALIGN_RIGHT)            eHAlign = WX_HORIZ_ALIGN_RIGHT;
    else                                        eHAlign = WX_HORIZ_ALIGN_LEFT;

         if (iAlign & wxALIGN_TOP)              eVAlign = WX_VERT_ALIGN_TOP;
    else if (iAlign & wxALIGN_CENTRE_VERTICAL)  eVAlign = WX_VERT_ALIGN_CENTRE;
    else if (iAlign & wxALIGN_BOTTOM)           eVAlign = WX_VERT_ALIGN_BOTTOM;
    else                                        eVAlign = WX_VERT_ALIGN_TOP;
}

//--------------------- ELLIPSIZATION -------------------------------//
/** Set ellipsization mode
  * \param eMode [input]: the new mode
  */
void wxVirtualDataViewRendererCompat::SetEllipsizeMode(EEllipsizeMode eMode)
{
    if (!m_pRenderer) return;
    switch(eMode)
    {
        case WX_ELLIPSIZE_NONE      :   m_pRenderer->DisableEllipsize(); break;
        case WX_ELLIPSIZE_START     :   m_pRenderer->EnableEllipsize(wxELLIPSIZE_START); break;
        case WX_ELLIPSIZE_MIDDLE    :   m_pRenderer->EnableEllipsize(wxELLIPSIZE_MIDDLE); break;
        case WX_ELLIPSIZE_END       :   m_pRenderer->EnableEllipsize(wxELLIPSIZE_END); break;
        default                     :   m_pRenderer->DisableEllipsize(); break;
    }
}

/** Get ellipsization mode
  * \return the current mode
  */
wxVirtualDataViewRenderer::EEllipsizeMode
 wxVirtualDataViewRendererCompat::GetEllipsizeMode(void) const
{
    if (!m_pRenderer) return(WX_ELLIPSIZE_NONE);
    switch(m_pRenderer->GetEllipsizeMode())
    {
        case wxELLIPSIZE_NONE   :   return(WX_ELLIPSIZE_NONE);
        case wxELLIPSIZE_START  :   return(WX_ELLIPSIZE_START);
        case wxELLIPSIZE_MIDDLE :   return(WX_ELLIPSIZE_MIDDLE);
        case wxELLIPSIZE_END    :   return(WX_ELLIPSIZE_END);
        default                 :   break;
    }
    return(WX_ELLIPSIZE_NONE);
}

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
