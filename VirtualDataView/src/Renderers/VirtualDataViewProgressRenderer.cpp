/**********************************************************************/
/** FILE    : VirtualDataViewProgressRenderer.cpp                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view progress bar renderer                      **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewProgressRenderer.h>
#include <wx/dc.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewProgressRenderer::wxVirtualDataViewProgressRenderer(void)
    : wxVirtualDataViewRenderer(),
      m_iMin(0), m_iMax(100),
      m_bDrawValue(true)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewProgressRenderer::~wxVirtualDataViewProgressRenderer(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewProgressRenderer::Clone(void) const
{
    wxVirtualDataViewProgressRenderer *pResult;
    pResult = new wxVirtualDataViewProgressRenderer();
    if (pResult)
    {
        pResult->CopyBaseRenderer(*this);
        pResult->m_iMin       = m_iMin;
        pResult->m_iMax       = m_iMax;
        pResult->m_bDrawValue = m_bDrawValue;
    }
    return(pResult);
}

/** Render the item
  * \param pWindow [input]: the window where the rendering occurs
  * \param rRect   [input]: the clipping rectangle
  * \param rDC     [input]: the device context to use for drawing
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item (integer between 0 and 100 included)
  */
void wxVirtualDataViewProgressRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                               wxDC &rDC,
                                               wxVirtualDataViewItemAttr *pAttr,
                                               const wxVirtualDataViewItemState &rState,
                                               const wxVariant& vValue)
{
    int iValue = vValue.GetInteger();

    //scale
    iValue -= m_iMin;

    //clamp
    if (iValue < 0)      iValue = 0;
    if (iValue > m_iMax) iValue = m_iMax;


    RenderProgressBar(pWindow, rRect, rDC, iValue, m_iMax - m_iMin);

    if (m_bDrawValue) RenderText(pWindow, rRect, rDC, vValue.GetString(), 0, 0, pAttr, rState,
                                 WX_HORIZ_ALIGN_CENTRE, WX_VERT_ALIGN_CENTRE, WX_ELLIPSIZE_NONE);
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewProgressRenderer::GetSize(wxWindow *pWindow,
                                                  wxVirtualDataViewItemAttr *pAttr,
                                                  const wxVirtualDataViewItemState &rState,
                                                  const wxVariant& vValue) const
{
    wxSize s(10, 10);
    if (m_bDrawValue)
    {
        wxSize sTextSize = GetTextExtent(pWindow, vValue.GetString(), pAttr);
        if (sTextSize.GetWidth()  > s.GetWidth() ) s.SetWidth(sTextSize.GetWidth());
        if (sTextSize.GetHeight() > s.GetHeight()) s.SetHeight(sTextSize.GetHeight());
    }
    return(s);
}

//------------------ RANGE ------------------------------------------//
/** Set the valid range of the progress bar
  * \param iMin [input]: the new minimal value of the progress bar
  * \param iMax [input]: the new maximal value of the progress bar
  */
void wxVirtualDataViewProgressRenderer::SetRange(int iMin, int iMax)
{
    m_iMin = iMin;
    m_iMax = iMax;

    //check & correct
    if (m_iMin > m_iMax)
    {
        int iTemp = m_iMin;
        m_iMin = m_iMax;
        m_iMax = iTemp;
    }
    if (m_iMin == m_iMax) m_iMax++;
}

/** Get the valid range of the progress bar
  * \param iMin [output]: the minimal value of the progress bar
  * \param iMax [output]: the maximal value of the progress bar
  */
void wxVirtualDataViewProgressRenderer::GetRange(int &iMin, int &iMax) const
{
    iMin = m_iMin;
    iMax = m_iMax;
}

//----------------- DRAW VALUE --------------------------------------//
/** Check if the value is drawn as text
  * \return true if the text value is drawn on top of the progress bar
  */
bool wxVirtualDataViewProgressRenderer::IsValueDrawn(void) const
{
    return(m_bDrawValue);
}

/** Set if the value is drawn as text
  * \param bIsDrawn [input] : true if the text value is drawn on top of the progress bar
  */
void wxVirtualDataViewProgressRenderer::SetValueIsDrawn(bool bIsDrawn)
{
    m_bDrawValue = bIsDrawn;
}

