/**********************************************************************/
/** FILE    : VirtualDataViewBitmapTextRenderer.cpp                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view bitmap + text renderer                     **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewBitmapTextRenderer.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/dc.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param eMode   [input]: the ellipsization mode
  * \param eHAlign [input]: the horizontal alignment
  * \param eVAlign [input]: the vertical alignment
  */
wxVirtualDataViewBitmapTextRenderer::wxVirtualDataViewBitmapTextRenderer(EEllipsizeMode eMode,
                                                                         EHAlign eHAlign,
                                                                         EVAlign eVAlign)
    : wxVirtualDataViewRenderer()
{

}

/** Destructor - virtual
  */
wxVirtualDataViewBitmapTextRenderer::~wxVirtualDataViewBitmapTextRenderer(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewBitmapTextRenderer::Clone(void) const
{
    wxVirtualDataViewBitmapTextRenderer *pResult;
    pResult = new wxVirtualDataViewBitmapTextRenderer(m_eEllipsizeMode,
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
void wxVirtualDataViewBitmapTextRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                           wxDC &rDC,
                                           wxVirtualDataViewItemAttr *pAttr,
                                           const wxVirtualDataViewItemState &rState,
                                           const wxVariant& vValue)
{
    //background
    RenderBackground(pWindow, rRect, rDC, pAttr);

    if (vValue.GetType() != "wxBitmapText")
    {
        wxString sValue = vValue.GetString();
        RenderText(pWindow, rRect, rDC, sValue,
                   0, 0,
                   pAttr, rState,
                   m_eHorizontalAlignment,
                   m_eVerticalAlignment,
                   m_eEllipsizeMode);
        return;
    }

    //get information
    wxBitmapText oValue;
    oValue << vValue;

    //bitmap
    wxRect r = rRect;
    r.x += 1;
    r.width -= 1;
    wxPoint pt = r.GetLeftTop();
    wxIcon &rIcon = oValue.GetIcon();
    rDC.DrawIcon(rIcon, pt);

    int w = rIcon.GetWidth() + 1;
    r.x += w;
    r.width -= w;

    //text
    wxString sValue = oValue.GetText();
    RenderText(pWindow, r, rDC, sValue,
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
wxSize wxVirtualDataViewBitmapTextRenderer::GetSize(wxWindow *pWindow,
                                                    wxVirtualDataViewItemAttr *pAttr,
                                                    const wxVirtualDataViewItemState &rState,
                                                    const wxVariant& vValue) const
{
    //get information
    wxBitmapText oValue;
    if (vValue.GetType() != "wxBitmapText")
    {
        oValue.SetText(vValue.GetString());
        if (oValue.GetText() == "") oValue.SetText("AgMf0");
    }
    else
    {
        oValue << vValue;
    }

    //bitmap
    int w = 0;
    int h = 0;
    wxIcon &rIcon = oValue.GetIcon();
    w += rIcon.GetWidth() + 2;
    h += rIcon.GetHeight() + 2;

    //text
    wxString sValue = oValue.GetText();
    wxSize s = GetTextExtent(pWindow, sValue, pAttr);
    w += s.GetWidth();
    if (s.GetHeight() > h) h = s.GetHeight();
    return(wxSize(w, h));
}

//-------------- ALIGNMENT ------------------------------------------//
/** Set alignment
  * \param eHAlign [input]: the new horizontal alignment
  * \param eVAlign [input]: the new vertical alignment
  */
void wxVirtualDataViewBitmapTextRenderer::SetAlignment(EHAlign eHAlign, EVAlign eVAlign)
{
    m_eHorizontalAlignment = eHAlign;
    m_eVerticalAlignment   = eVAlign;
}

/** Get alignment
  * \param eHAlign [output]: the horizontal alignment
  * \param eVAlign [output]: the vertical alignment
  */
void wxVirtualDataViewBitmapTextRenderer::GetAlignment(EHAlign &eHAlign, EVAlign &eVAlign) const
{
    eHAlign = m_eHorizontalAlignment;
    eVAlign = m_eVerticalAlignment;
}

//--------------------- ELLIPSIZATION -------------------------------//
/** Set ellipsization mode
  * \param eMode [input]: the new mode
  */
void wxVirtualDataViewBitmapTextRenderer::SetEllipsizeMode(EEllipsizeMode eMode)
{
    m_eEllipsizeMode = eMode;
}

/** Get ellipsization mode
  * \return the current mode
  */
wxVirtualDataViewRenderer::EEllipsizeMode
 wxVirtualDataViewBitmapTextRenderer::GetEllipsizeMode(void) const
{
    return(m_eEllipsizeMode);
}

