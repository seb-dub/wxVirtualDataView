/**********************************************************************/
/** FILE    : VirtualDataViewBitmapRenderer.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view bitmap renderer                            **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewBitmapRenderer.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/dc.h>
#include <wx/image.h>
#include <wx/icon.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewBitmapRenderer::wxVirtualDataViewBitmapRenderer(void)
    : wxVirtualDataViewRenderer()
{

}

/** Destructor - virtual
  */
wxVirtualDataViewBitmapRenderer::~wxVirtualDataViewBitmapRenderer(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewBitmapRenderer::Clone(void) const
{
    wxVirtualDataViewBitmapRenderer *pResult;
    pResult = new wxVirtualDataViewBitmapRenderer();
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
void wxVirtualDataViewBitmapRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                             wxDC &rDC,
                                             wxVirtualDataViewItemAttr *pAttr,
                                             const wxVirtualDataViewItemState &rState,
                                             const wxVariant& vValue)
{
    //background
    RenderBackground(pWindow, rRect, rDC, pAttr);

    //image
    wxPoint pt = rRect.GetLeftTop();
    pt.x += 1;
    pt.y += 1;
    if (vValue.GetType() == "wxImage")
    {
        wxImage img;
        img << vValue;

        wxBitmap bmp(img);
        rDC.DrawBitmap(bmp, pt, true);

    }
    else if (vValue.GetType() == "wxIcon")
    {
        wxBitmap bmp;
        bmp << vValue;
        rDC.DrawBitmap(bmp, pt, true);
    }
    else if (vValue.GetType() == "wxBitmap")
    {
        wxIcon oIcon;
        oIcon << vValue;
        rDC.DrawIcon(oIcon, pt);
    }
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewBitmapRenderer::GetSize(wxWindow *pWindow,
                                                wxVirtualDataViewItemAttr *pAttr,
                                                const wxVirtualDataViewItemState &rState,
                                                const wxVariant& vValue) const
{
    wxSize s(0, 0);
    if (vValue.GetType() == "wxImage")
    {
        wxImage img;
        img << vValue;
        s = img.GetSize();
    }
    else if (vValue.GetType() == "wxIcon")
    {
        wxBitmap bmp;
        bmp << vValue;
        s = bmp.GetSize();
    }
    else if (vValue.GetType() == "wxBitmap")
    {
        wxIcon oIcon;
        oIcon << vValue;
        s = oIcon.GetSize();
    }

    s.IncBy(1, 1);
    return(s);
}

