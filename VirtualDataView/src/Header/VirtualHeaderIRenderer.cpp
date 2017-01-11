/**********************************************************************/
/** FILE    : VirtualHeaderIRenderer.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : class for handling header control drawing with theme   **/
/**********************************************************************/

#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Constructor
  * \param pWin [input]: window receiving the rendering
  */
wxVirtualHeaderIRenderer::wxVirtualHeaderIRenderer(wxWindow *pWin)
    : m_pWindow(pWin)
{
}

/** Destructor
  */
wxVirtualHeaderIRenderer::~wxVirtualHeaderIRenderer(void)
{
}

//----------------------- WINDOW TO RENDER --------------------------//
/** Get the window
  * \return the window to render
  */
wxWindow* wxVirtualHeaderIRenderer::GetWindow(void) const
{
    return(m_pWindow);
}

/** Set the window
  * \param pWindow [input]: the window receiving the rendering
  */
void wxVirtualHeaderIRenderer::SetWindow(wxWindow* pWindow)
{
    m_pWindow = pWindow;
}

//-------------- DEFAULT INTERFACE IMPLEMENTATION -------------------//
/** Prepare the DC for rendering
  * \param rDC [input]: the DC to prepare
  * The default implementation does nothing
  */
void wxVirtualHeaderIRenderer::PrepareDC(wxDC &rDC)
{

}

/** Reset theme data
  * To respond for wxSysColourChangedEvent
  * The default implementation does nothing
  */
void wxVirtualHeaderIRenderer::ResetTheme(void)
{

}
