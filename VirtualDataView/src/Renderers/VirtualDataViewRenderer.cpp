/**********************************************************************/
/** FILE    : VirtualDataViewRenderer.cpp                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view renderer                                   **/
/**********************************************************************/

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/window.h>
#include <wx/renderer.h>
#include <wx/dc.h>
#include <wx/control.h> //wxControl::Ellipsize

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewRenderer::wxVirtualDataViewRenderer(void)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewRenderer::~wxVirtualDataViewRenderer(void)
{

}

//---------------------- INTERNAL METHODS ---------------------------//
/** Get the renderer
  * \return the renderer
  */
WX_VDV_INLINE wxRendererNative& GetRenderer(void)
{
    return(wxRendererNative::GetDefault());
}

/** Copy the base renderer
  * Only members from wxVirtualDataViewRenderer base class are copied
  * This is a helper method intended to help implementing the Clone virtual method
  * \param rhs [input] : the base renderer to copy
  */
void wxVirtualDataViewRenderer::CopyBaseRenderer(const wxVirtualDataViewRenderer &rhs)
{
    //nothing to do
}

//---------- HELPER METHODS : RENDERING -----------------------------//
/** Draw a background - static
  * \param pWindow           [input]: the window where the rendering occurs
  * \param rRect             [input]: the clipping region
  * \param rDC               [input]: the device context to use
  * \param pAttr             [input]: the graphic attributes
  * \param bWindowBackground [input]: if the cell attributes do not define a background for the cell,
  *                                   then redraw the window background if bWindowBackground is true
  */
void wxVirtualDataViewRenderer::RenderBackground(wxWindow *pWindow,
                                                 const wxRect &rRect, wxDC &rDC,
                                                 wxVirtualDataViewItemAttr *pAttr,
                                                 bool bWindowBackground)
{
    wxColour cColour;
    if (pAttr)
    {
        if (pAttr->HasBackgroundColour())
        {
            cColour = pAttr->GetBackgroundColour();
        }
        else
        {
            if (!bWindowBackground) return;
            cColour = pWindow->GetBackgroundColour();
        }
    }
    else
    {
        if (!bWindowBackground) return;
        cColour = pWindow->GetBackgroundColour();
    }

    wxDCPenChanger   oChangePen(rDC, cColour);
    wxDCBrushChanger oChangeBrush(rDC, cColour);

    rDC.DrawRectangle(rRect);
}

/** Draw a selection rectangle - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param rState         [input]: the item state
  * \param pColour        [input]: if non-null, this colour override the renderer colour
  */
void wxVirtualDataViewRenderer::RenderSelectionRect(wxWindow *pWindow,
                                                    const wxRect &rRect, wxDC &rDC,
                                                    const wxVirtualDataViewItemState &rState,
                                                    wxColour *pColour)
{
    int iFlags = 0;

    if (!pColour)
    {
        //no colour override
        if (rState.IsSelected()) iFlags |= wxCONTROL_SELECTED;
        if (rState.IsFocused())  iFlags |= wxCONTROL_CURRENT;
        if (pWindow->HasFocus()) iFlags |= wxCONTROL_FOCUSED;
        GetRenderer().DrawItemSelectionRect(pWindow, rDC, rRect, iFlags);
    }
    else
    {
        //colour override
        //draw background
        wxColour &rColour = *pColour;
        wxDCPenChanger   oChangePen(rDC, rColour);
        wxDCBrushChanger oChangeBrush(rDC, rColour);

        rDC.DrawRectangle(rRect);

        //draw focus rectangle
        if ((rState.IsFocused()) && (pWindow->HasFocus()))
        {
            if (rState.IsSelected()) iFlags |= wxCONTROL_SELECTED;
            GetRenderer().DrawFocusRect(pWindow, rDC, rRect, iFlags);
        }
    }
}

/** Draw a focus rectangle - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param rState         [input]: the item state
  */
void wxVirtualDataViewRenderer::RenderFocusRect(wxWindow *pWindow,
                                                const wxRect &rRect, wxDC &rDC,
                                                const wxVirtualDataViewItemState &rState)
{
    int iFlags = 0;
    if (rState.IsSelected()) iFlags |= wxCONTROL_SELECTED;
    GetRenderer().DrawFocusRect(pWindow, rDC, rRect, iFlags);
}

/** Compute text colour - static
  * \param pWindow        [input] : the window where the rendering occurs
  * \param rDC            [input] : the device context to use
  * \param pAttr          [input] : the attributes to use
  * \param rState         [input] : the item state
  * \param rColour        [output]: the compute text colour
  */
WX_VDV_INLINE void DoGetTextColour(wxWindow *pWindow, wxDC &rDC, wxVirtualDataViewItemAttr *pAttr,
                                   const wxVirtualDataViewItemState &rState,
                                   wxColour &rColour)
{
    //override custom foreground with the standard one for the selected items
    //because we currently don't allow changing the selection background and
    //custom colours may be unreadable on it
    if (rState.IsSelected())
    {
        rColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    }
    else
    {
        if (pAttr)
        {
            if (pAttr->HasColour()) rColour = pAttr->GetColour();
            else                    rColour = pWindow->GetForegroundColour();
        }
        else
        {
            rColour = pWindow->GetForegroundColour();
        }
    }
}

/** Compute text colour - static
  * \param pWindow        [input] : the window where the rendering occurs
  * \param rDC            [input] : the device context to use
  * \param pAttr          [input] : the attributes to use
  * \param rState         [input] : the item state
  * \param rColour        [output]: the compute text colour
  */
void wxVirtualDataViewRenderer::GetTextColour(wxWindow *pWindow, wxDC &rDC, wxVirtualDataViewItemAttr *pAttr,
                                              const wxVirtualDataViewItemState &rState,
                                              wxColour &rColour)
{
    DoGetTextColour(pWindow, rDC, pAttr, rState, rColour);
}

/** Draw text without background - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param rsText         [input]: the text to draw
  * \param iXOffset       [input]: the X offset of the text
  * \param iYOffset       [input]: the Y offset of the text
  * \param pAttr          [input]: the text attribute to use (font & text foreground colour)
  * \param rState         [input]: the item state
  * \param eHAlign        [input]: the horizontal alignment
  * \param eVAlign        [input]: the vertical alignment
  * \param eEllipsization [input]: the ellipsization mode
  */
void wxVirtualDataViewRenderer::RenderText(wxWindow *pWindow,
                                           const wxRect &rRect, wxDC &rDC,
                                           const wxString &rsText,
                                           int iXOffset, int iYOffset,
                                           wxVirtualDataViewItemAttr *pAttr,
                                           const wxVirtualDataViewItemState &rState,
                                           EHAlign eHAlign, EVAlign eVAlign,
                                           EEllipsizeMode eEllipsization)
{
    int iAlign = 0;
    switch(eHAlign)
    {
        case WX_HORIZ_ALIGN_LEFT    : iAlign |= wxALIGN_LEFT; break;
        case WX_HORIZ_ALIGN_CENTRE  : iAlign |= wxALIGN_CENTRE_HORIZONTAL; break;
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

    int iFlags = 0;
    if (rState.IsSelected()) iFlags |= wxCONTROL_SELECTED;
    if (rState.IsFocused())  iFlags |= wxCONTROL_FOCUSED;
    if (rState.IsDisabled()) iFlags |= wxCONTROL_DISABLED;

    wxRect r(rRect);
    r.x += iXOffset;
    r.width -= iXOffset;
    r.y += iYOffset;
    r.height -= iYOffset;

    //apply colour
    wxColour cTextColour;
    DoGetTextColour(pWindow, rDC, pAttr, rState, cTextColour);
    wxDCTextColourChanger oChangeTextColour(rDC, cTextColour);

    //apply font
    wxDCFontChanger oChangeFont(rDC);
    if (pAttr)
    {
        //wxFont fFont = pAttr->GetEffectiveFont(rDC.GetFont());
        wxFont fFont = pAttr->GetEffectiveFont(pWindow->GetFont());
        oChangeFont.Set(fFont);
    }

    //draw
    wxString sText;
    if (eEllipsization != WX_ELLIPSIZE_NONE)
        sText = wxControl::Ellipsize(rsText, rDC, (wxEllipsizeMode) eEllipsization, rRect.width, wxELLIPSIZE_FLAGS_NONE);
    else
        sText = rsText;
    int iMode = rDC.GetBackgroundMode();
    rDC.SetBackgroundMode(wxPENSTYLE_TRANSPARENT);

    wxDCClipper dcc(rDC, rRect); //wxDC::DrawLabel needs this, at least on MSW
    if (rState.IsSelected()) GetRenderer().DrawItemSelectionRect(pWindow, rDC, r, iFlags);
    rDC.DrawLabel(sText, rRect, iAlign);
    rDC.SetBackgroundMode(iMode);
    //wxRendererNative::DrawItemText does not respect the theme colour
//    GetRenderer().DrawItemText(pWindow, rDC, rsText, r,
//                               iAlign, iFlags,
//                               (wxEllipsizeMode) eEllipsization);
}

/** Draw a check box - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param eValue         [input]: the checkbox value
  * \param bMouseOver     [input]: if true, the mouse cursor is on the checkbox
  */
void wxVirtualDataViewRenderer::RenderCheckBox(wxWindow *pWindow, const wxRect &rRect, wxDC &rDC,
                                               ECheckBoxValue eValue, bool bMouseOver)
{
    int iFlags = 0;
    if (bMouseOver) iFlags |= wxCONTROL_CURRENT;
    switch(eValue)
    {
        case WX_CHECKBOX_UNCHECKED    :  break;
        case WX_CHECKBOX_CHECKED      : iFlags |= wxCONTROL_CHECKED; break;
        case WX_CHECKBOX_UNDETERMINED : iFlags |= wxCONTROL_UNDETERMINED; break;
        default                       : break;
    }


    //centre the checkbox
    wxRect r = rRect;
    wxSize s = GetCheckboxSize(pWindow);

    //horizontally
    int w = s.GetWidth();
    if (w < r.width)
    {
        int iOffset = (r.width - w) / 2;
        r.x += iOffset;
        r.width = w;
    }

    //vertically
    int h = s.GetHeight();
    if (h < r.height)
    {
        int iOffset = (r.height - h) / 2;
        r.y += iOffset;
        r.height = h;
    }

    GetRenderer().DrawCheckBox(pWindow, rDC, r, iFlags);
}

/** Draw a blank push button - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param bPressed       [input]: if true, the button is pressed
  * \param bMouseOver     [input]: if true, the mouse cursor is over the button
  * \param bDefault       [input]: if true, the button is the default button
  */
void wxVirtualDataViewRenderer::RenderButtonFrame(wxWindow *pWindow,
                                                  const wxRect &rRect, wxDC &rDC,
                                                  bool bPressed, bool bMouseOver,
                                                  bool bDefault)
{
    int iFlags = 0;
    if (bPressed)       iFlags |= wxCONTROL_PRESSED;
    if (bMouseOver)     iFlags |= wxCONTROL_CURRENT;
    if (bDefault)       iFlags |= wxCONTROL_ISDEFAULT;
    GetRenderer().DrawPushButton(pWindow, rDC, rRect, iFlags);
}

/** Draw a progress bar - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param uiValue        [input]: the current value of the progress bar
  * \param uiMax          [input]: the max value of the progress bar
  */
void wxVirtualDataViewRenderer::RenderProgressBar(wxWindow *pWindow,
                                                  const wxRect &rRect, wxDC &rDC,
                                                  size_t uiValue, size_t uiMax)
{
    if (uiValue > uiMax) GetRenderer().DrawGauge(pWindow, rDC, rRect, uiMax, uiMax, 0);
    else                 GetRenderer().DrawGauge(pWindow, rDC, rRect, uiValue, uiMax, 0);
}

/** Draw a tree button - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param bExpanded      [input]: true if the button is expanded
  */
void wxVirtualDataViewRenderer::RenderTreeButton(wxWindow *pWindow,
                                                 const wxRect &rRect, wxDC &rDC,
                                                 bool bExpanded)
{
    int iFlags = 0;
    if (bExpanded) iFlags |= wxCONTROL_EXPANDED;
    GetRenderer().DrawTreeItemButton(pWindow, rDC, rRect, iFlags);
}

/** Draw a tree line - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rRect          [input]: the clipping region
  * \param rDC            [input]: the device context to use
  * \param x0             [input]: X coordinate of the starting point
  * \param y0             [input]: Y coordinate of the starting point
  * \param x1             [input]: X coordinate of the ending point
  * \param y1             [input]: Y coordinate of the ending point
  */
void wxVirtualDataViewRenderer::RenderTreeLine(wxWindow *pWindow,
                                               const wxRect &rRect, wxDC &rDC,
                                               wxCoord x0, wxCoord y0,
                                               wxCoord x1, wxCoord y1)
{
    wxPen oPen(wxGREY_PEN->GetColour(), wxGREY_PEN->GetWidth(), wxPENSTYLE_DOT);
    wxDCPenChanger oPenC(rDC, oPen);
    rDC.DrawLine(x0, y0, x1, y1);
}

//----------- HELPER METHODS : MEASURING ----------------------------//
/** Get a check box size - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \return the size of a checkbox
  */
wxSize wxVirtualDataViewRenderer::GetCheckboxSize(wxWindow *pWindow)
{
    return(GetRenderer().GetCheckBoxSize(pWindow));
}

/** Measure text size - static
  * \param pWindow        [input]: the window where the rendering occurs
  * \param rsText         [input]: the text to measure
  * \param pAttr          [input]: optional attributes
  * \return the size
  */
wxSize wxVirtualDataViewRenderer::GetTextExtent(wxWindow *pWindow, const wxString &rsText, wxVirtualDataViewItemAttr *pAttr)
{
    wxFont oFont(pWindow->GetFont());
    if (pAttr) pAttr->GetEffectiveFont(oFont);

    int x, y;
    pWindow->GetTextExtent(rsText, &x, &y,
                           WX_VDV_NULL_PTR, WX_VDV_NULL_PTR, &oFont);
    return(wxSize(x, y));
}
