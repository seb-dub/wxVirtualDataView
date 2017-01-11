/**********************************************************************/
/** FILE    : VirtualDataViewColourRenderer.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view colour renderer                            **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewColourRenderer.h>
#include <wx/dc.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewColourRenderer::wxVirtualDataViewColourRenderer(void)
    : wxVirtualDataViewRenderer()
{

}

/** Destructor - virtual
  */
wxVirtualDataViewColourRenderer::~wxVirtualDataViewColourRenderer(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewColourRenderer::Clone(void) const
{
    wxVirtualDataViewColourRenderer *pResult;
    pResult = new wxVirtualDataViewColourRenderer();
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
void wxVirtualDataViewColourRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                             wxDC &rDC,
                                             wxVirtualDataViewItemAttr *pAttr,
                                             const wxVirtualDataViewItemState &rState,
                                             const wxVariant& vValue)
{
    if (vValue.GetType() != "wxColour") return;
    wxColour c;
    c << vValue;

    wxBrush oBrush(c, wxBRUSHSTYLE_SOLID);
    wxPen   oPen(c, 1, wxPENSTYLE_SOLID);
    rDC.SetBrush(oBrush);
    rDC.SetPen(oPen);
    rDC.DrawRectangle(rRect);
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewColourRenderer::GetSize(wxWindow *pWindow,
                                                wxVirtualDataViewItemAttr *pAttr,
                                                const wxVirtualDataViewItemState &rState,
                                                const wxVariant& vValue) const
{
    return(wxSize(5, 5));
}

