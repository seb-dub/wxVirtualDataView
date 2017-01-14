/**********************************************************************/
/** FILE    : VirtualDataViewCheckBoxRenderer.cpp                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view bool renderer using a checkbox             **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewCheckBoxRenderer.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/dc.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param bTriState   [input]: true if the check-box is tri-state
  */
wxVirtualDataViewCheckBoxRenderer::wxVirtualDataViewCheckBoxRenderer(bool bTriState)
    : wxVirtualDataViewRenderer(),
      m_bTriState(bTriState)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewCheckBoxRenderer::~wxVirtualDataViewCheckBoxRenderer(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewRenderer* wxVirtualDataViewCheckBoxRenderer::Clone(void) const
{
    wxVirtualDataViewCheckBoxRenderer *pResult;
    pResult = new wxVirtualDataViewCheckBoxRenderer(m_bTriState);
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
void wxVirtualDataViewCheckBoxRenderer::Render(wxWindow *pWindow, const wxRect &rRect,
                                               wxDC &rDC,
                                               wxVirtualDataViewItemAttr *pAttr,
                                               const wxVirtualDataViewItemState &rState,
                                               const wxVariant& vValue)
{
    //background
    RenderBackground(pWindow, rRect, rDC, pAttr);

    //checkbox
    int iValue = vValue.GetInteger();
    ECheckBoxValue eValue;
    if (m_bTriState)
    {
        if (iValue >= 2)      eValue = WX_CHECKBOX_UNDETERMINED;
        else if (iValue < 0)  eValue = WX_CHECKBOX_UNDETERMINED;
        else if (iValue == 0) eValue = WX_CHECKBOX_UNCHECKED;
        else                  eValue = WX_CHECKBOX_CHECKED;
    }
    else
    {
        if (iValue == 0)      eValue = WX_CHECKBOX_UNCHECKED;
        else                  eValue = WX_CHECKBOX_CHECKED;
    }

    RenderCheckBox(pWindow, rRect, rDC, eValue, false);
}

/** Return the size of the item
  * \param pWindow [input]: the window used for measuring
  * \param pAttr   [input]: the item attributes
  * \param rState  [input]: the item state
  * \param vValue  [input]: the value of the item
  * \return the size of the item when rendered
  */
wxSize wxVirtualDataViewCheckBoxRenderer::GetSize(wxWindow *pWindow,
                                                  wxVirtualDataViewItemAttr *pAttr,
                                                  const wxVirtualDataViewItemState &rState,
                                                  const wxVariant& vValue) const
{
    return(GetCheckboxSize(pWindow));
}

//---------------------- TRI-STATE ----------------------------------//
/** Check if the renderer is tri-state
  * \return true if the renderer is tri-state, false otherwise
  */
bool wxVirtualDataViewCheckBoxRenderer::IsTriState(void) const
{
    return(m_bTriState);
}

/** Set the renderer to tri-state
  * \param bTriState [input]: true for setting the renderer to tri-state
  *                           false for bi-state (true/false)
  */
void wxVirtualDataViewCheckBoxRenderer::SetTriState(bool bTriState)
{
    m_bTriState = bTriState;
}

