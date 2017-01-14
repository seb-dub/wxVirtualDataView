/**********************************************************************/
/** FILE    : ComboboxState.cpp                                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : state of a combobox for static painting                **/
/**********************************************************************/

#include "ComboboxState.hpp"
#include <wx/combo.h>
#include <wx/app.h>
#include <wx/dc.h>

#ifdef __WXMSW__
    #if wxUSE_UXTHEME
        #include <wx/msw/uxtheme.h>
    #endif
#endif // __WXMSW__

//----------------------- CONSTANTS ---------------------------------//
#define DEFAULT_DROPBUTTON_WIDTH                19
#define DEFAULT_TEXT_INDENT                     3
#define NATIVE_TEXT_INDENT_XP                   4
#define NATIVE_TEXT_INDENT_CLASSIC              2
#define BMP_BUTTON_MARGIN                       4

#if defined(__WXMSW__)
    #define FOCUS_RING                          0 // No focus ring on wxMSW
#elif defined(__WXGTK__)
    #define FOCUS_RING                          0 // No focus ring on wxGTK
#elif defined(__WXMAC__)
    #define FOCUS_RING                          3 // Reserve room for the textctrl's focus ring to display
#else
    #define FOCUS_RING                          0
#endif

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//
/** Constructor
  */
wxComboboxState::wxComboboxState(void)
{
    Init();
}

/** Destructor
  */
wxComboboxState::~wxComboboxState(void)
{

}

//---------------------- INITIALIZATION -----------------------------//
/** Initialize from a combo box
  * \param pCombobox            [input]: the combobox to copy
  * \param rButtonRect          [input]: the rectangle of the button
  * \param iButtonState         [input]: state flags of the button
  * \param iButtonSpacingX      [input]: the horizontal spacing around the button
  * \param bBlankButtonBG       [input]: true if the button has a blank background
  * \param bHasForegroundColour [input]: true if a foreground colour is defined
  * \param bIsDestroying        [input]: true if the combobox pComboBox is currently being destroyed
  */
void wxComboboxState::InitFromComboBox(wxComboCtrl *pComboBox, const wxRect &rButtonRect, int iButtonState,
                                       int iButtonSpacingX, bool bBlankButtonBG, bool bHasForegroundColour,
                                       bool bIsDestroying)
{
    if (!pComboBox)
    {
        Init();
        return;
    }

    //generic
    m_iStyle                    = pComboBox->GetWindowStyle();
    m_iFlags                    = pComboBox->GetInternalFlags();
    m_bIsEnabled                = pComboBox->IsEnabled();
    m_bFocus                    = pComboBox->ShouldDrawFocus();

    //boundaries
    m_iWidthBorder              = 1;
    m_iWidthCustomPaint         = pComboBox->GetCustomPaintWidth();

    //background
    m_bHasTransparentBackground = HasTransparentBG(pComboBox);
    m_cBackColour               = pComboBox->GetBackgroundColour();
    m_cParentBackColour         = m_cBackColour;
    wxWindow *pParent           = pComboBox->GetParent();
    if (pParent) m_cParentBackColour = pParent->GetBackgroundColour();

    //text field
    m_bHasText                  = pComboBox->GetTextCtrl();
    m_rRectTextArea             = pComboBox->GetTextRect();
    m_oFont                     = pComboBox->GetFont();
    m_sText                     = pComboBox->GetValue();
    m_iMarginLeft               = pComboBox->GetMargins().x;
    m_bHasFgCol                 = bHasForegroundColour;
    m_cForegroundCol            = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    if (m_bHasFgCol) m_cForegroundCol = pComboBox->GetForegroundColour();

    //button
    m_ButtonSize                = pComboBox->GetButtonSize();
    m_bHasButton                = pComboBox->GetButton();
    m_rRectButton               = rButtonRect;
    m_iButtonState              = iButtonState;
    m_iButtonSpacingX           = iButtonSpacingX;
    m_bmpNormal                 = pComboBox->GetBitmapNormal();
    m_bmpPressed                = pComboBox->GetBitmapPressed();
    m_bmpDisabled               = pComboBox->GetBitmapDisabled();
    m_bmpHover                  = pComboBox->GetBitmapHover();
    m_bBlankButtonBg            = bBlankButtonBG;
    m_iButtonWidth              = -1; //not needed
    m_iButtonHeight             = -1; //not needed
    m_iDefaultButtonWidth       = 0;  //not needed

    //popup
    m_pPopupInterface           = pComboBox->GetPopupControl();
    m_iPopupState               = pComboBox->GetPopupWindowState();

    //if the combobox is currently being destroyed, we have to
    //override the sub-windows
    if (bIsDestroying)
    {
        m_bHasText = false;
        m_bHasButton = false;
    }
}

/** Initialize to a default state
  */
void wxComboboxState::Init(void)
{
    wxRect r(0, 0, 20, 10);
    Init(r);
}

/** Initialize to a default state
  * \param rRect [input]: the rectangle of the combobox
  */
void wxComboboxState::Init(const wxRect &rRect)
{
    //generic
    m_iStyle                    = 0;
    m_iFlags                    = 0;
    m_bIsEnabled                = true;
    m_bFocus                    = false;

    //boundaries
    m_iWidthBorder              = 0; //on generic versions, it can be one. Keep 0
    m_iWidthCustomPaint         = 0;

    //background
    m_bHasTransparentBackground = false;
    m_cBackColour               = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    m_cParentBackColour         = m_cBackColour;

    //text field
    m_bHasText                  = false; //we must draw the text ourselves
    m_oFont                     = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_sText                     = wxEmptyString;
    m_iMarginLeft               = GetNativeTextIndent();
    m_bHasFgCol                 = false;
    m_cForegroundCol            = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    //button
    m_bHasButton                = false; //we must draw the button ourselves
    m_iButtonState              = 0;
    m_iButtonSpacingX           = 0;
    m_bmpNormal                 = wxNullBitmap;
    m_bmpPressed                = wxNullBitmap;
    m_bmpDisabled               = wxNullBitmap;
    m_bmpHover                  = wxNullBitmap;
    m_bBlankButtonBg            = false;
    m_iButtonWidth              = -1;
    m_iButtonHeight             = -1;
    m_iDefaultButtonWidth       = 0;

    //popup
    m_pPopupInterface           = WX_VDV_NULL_PTR; //no popup
    m_iPopupState               = 0; //hidden

    //text + button areas
    InitTextAndButtonAreas(rRect);
}

/** Get the native text indent
  * \return the native text indent
  */
int wxComboboxState::GetNativeTextIndent(void) const
{
#ifdef __WXMSW__
    #if wxUSE_UXTHEME
    if (wxUxThemeEngine::GetIfActive())
        return NATIVE_TEXT_INDENT_XP;
    #endif // wxUSE_UXTHEME
        return NATIVE_TEXT_INDENT_CLASSIC;
#else
    return(DEFAULT_TEXT_INDENT);
#endif // __WXMSW__
}

/** Recalculate sizes
  * \param rRect [input]: the rectangle of the combobox
  */
void wxComboboxState::RecalculateSize(const wxRect &rRect)
{
    wxSize sSize(rRect.GetWidth(), rRect.GetHeight());

    #ifdef __WXMSW__
        wxWindow *pTopLevelWin = wxTheApp->GetTopWindow();
        int iBtnWidth = wxWindow::FromDIP(17, pTopLevelWin);
    #else
        int iBtnWidth = DEFAULT_DROPBUTTON_WIDTH
    #endif // __WXMSW__

    ComputeButtonSize(iBtnWidth, sSize, true);
}

/** Calculate text and button rectangle
  * \param rRect [input]: the rectangle of the combobox
  */
void wxComboboxState::InitTextAndButtonAreas(const wxRect &rRect)
{
    RecalculateSize(rRect);
}

/** Compute button size
  * \param iBtnWidth            [input]: initial button width
  * \param rsClientSize         [input]: client size of the control
  * \param bButtonOnRightSide   [input]: true if the button is on the right side
  */
void wxComboboxState::ComputeButtonSize(int iBtnWidth, const wxSize &rsClientSize, bool bButtonOnRightSide)
{
    //this section is unshamefully copied from the otherwise excellent
    //wxComboCtrlBase::CalculateAreas. Minor text edition done (renaming of
    //variables + indentation)
    int iBtnBorder; // border for button only

    //check if button should really be outside the border: we'll do it it if
    //its platform default or bitmap+pushbutton background is used, but not if
    //there is vertical size adjustment or horizontal spacing.
    const bool bButtonOutsideBorder = m_iFlags & wxCC_BUTTON_OUTSIDE_BORDER;
    const bool bDrawButtonBitmap    = (m_bmpNormal.IsOk()) && m_bBlankButtonBg;
    if (   ((bButtonOutsideBorder) || (bDrawButtonBitmap))
        && (m_iButtonSpacingX == 0)
        && (m_iButtonHeight <= 0)
       )
    {
        m_iFlags |= wxCC_IFLAG_BUTTON_OUTSIDE;
        iBtnBorder = 0;
    }
    else if (  (m_iFlags & wxCC_BUTTON_COVERS_BORDER)
             && (m_iButtonSpacingX == 0)
             && (!m_bmpNormal.IsOk())
            )
    {
        m_iFlags &= ~(wxCC_IFLAG_BUTTON_OUTSIDE);
        iBtnBorder = 0;
    }
    else
    {
        m_iFlags &= ~(wxCC_IFLAG_BUTTON_OUTSIDE);
        iBtnBorder = m_iWidthBorder;
    }

    //Default indentation
    if (m_iMarginLeft < 0)  m_iMarginLeft = GetNativeTextIndent();

    //button width
    int iButWidth = iBtnWidth;
    if (iButWidth <= 0) iButWidth = m_iDefaultButtonWidth;
    else                m_iDefaultButtonWidth = iButWidth;
    if (iButWidth <= 0) return;

    //button height
    int iButHeight = rsClientSize.y - iBtnBorder * 2;

    //Adjust button width
    if (m_iButtonWidth > 0)
        iButWidth = m_iButtonWidth;
//    else
//    {
//        //we cannot do it without a real window pointer
//        //so ignore
//        //Adjust button width to match aspect ratio
//        //(but only if control is smaller than best size).
//        int bestHeight = GetBestSize().y;
//        int height = GetSize().y;
//
//        if ( height < bestHeight )
//        {
//            // Make very small buttons square, as it makes
//            // them accommodate arrow image better and still
//            // looks decent.
//            if ( height > FromDIP(18) )
//                iButWidth = (height*iButWidth)/bestHeight;
//            else
//                iButWidth = iButHeight;
//        }
//    }

    //Adjust button height
    if (m_iButtonHeight > 0) iButHeight = m_iButtonHeight;

    // Use size of normal bitmap if...
    //   It is larger
    //   OR
    //   button width is set to default and blank button bg is not drawn
    if (m_bmpNormal.IsOk())
    {
        int iBmpReqWidth  = m_bmpNormal.GetWidth();
        int iBmpReqHeight = m_bmpNormal.GetHeight();

        //If drawing blank button background, we need to add some margin.
        if (m_bBlankButtonBg )
        {
            iBmpReqWidth  += BMP_BUTTON_MARGIN * 2;
            iBmpReqHeight += BMP_BUTTON_MARGIN * 2;
        }

        if ((iButWidth < iBmpReqWidth) || ((m_iButtonWidth == 0) && (!m_bBlankButtonBg)))
        {
            iButWidth = iBmpReqWidth;
        }

        if ((iButHeight < iBmpReqHeight) || ((m_iButtonHeight == 0) && (!m_bBlankButtonBg)))
        {
            iButHeight = iBmpReqHeight;
        }


        //Need to fix height?
        if (((rsClientSize.y - (m_iWidthBorder * 2)) < iButHeight) && (iBtnWidth == 0))
        {
//            int newY = iButHeight + (m_iWidthBorder * 2);
//            SetClientSize(wxDefaultCoord, newY);
//
            if (  (m_bmpNormal.IsOk())
                || (m_rRectButton.width != iButWidth)
                || (m_rRectButton.height != iButHeight)
               )
            {
                m_iFlags |= wxCC_IFLAG_HAS_NONSTANDARD_BUTTON;
            }
            else
            {
                m_iFlags &= ~wxCC_IFLAG_HAS_NONSTANDARD_BUTTON;
            }
//            rsClientSize.y = newY;
        }
    }

    int iButAreaWidth = iButWidth + (m_iButtonSpacingX*2);

    m_ButtonSize.x = iButWidth;
    m_ButtonSize.y = iButHeight;

    m_rRectButton.x = (bButtonOnRightSide ? rsClientSize.x - iButAreaWidth - iBtnBorder : iBtnBorder);
    m_rRectButton.y = iBtnBorder + FOCUS_RING;
    m_rRectButton.width = iButAreaWidth;
    m_rRectButton.height = rsClientSize.y - ((iBtnBorder + FOCUS_RING) * 2);

    m_rRectTextArea.x = (bButtonOnRightSide ? 0 : iButAreaWidth ) + m_iWidthBorder;
    m_rRectTextArea.y = m_iWidthBorder + FOCUS_RING;
    m_rRectTextArea.width = rsClientSize.x - iButAreaWidth - (m_iWidthBorder * 2) - FOCUS_RING;
    m_rRectTextArea.height = rsClientSize.y - ((m_iWidthBorder + FOCUS_RING) * 2);
}

/** Check if the window has a transparent background
  * \param pWin [input]: the window to check
  * \return true if the window has a transparent background, false otherwise
  */
bool wxComboboxState::HasTransparentBG(wxWindow *pWin)
{
    #if defined(__WXMSW__)
        if (pWin) return(pWin->IsDoubleBuffered());
    #elif defined(__WXGTK__)
        if (pWin) return(pWin->IsDoubleBuffered());
    #endif // __WXMSW__

    #if wxALWAYS_NATIVE_DOUBLE_BUFFER
        return(true);
    #else
        return(false);
    #endif // wxALWAYS_NATIVE_DOUBLE_BUFFER
}

//----------------------- VALUE -------------------------------------//
/** Get the string value
  * \return the string value
  */
wxString wxComboboxState::GetValue(void) const
{
    return(m_sText);
}

/** Set the string value
  * \param sValue [input]: set the string value
  */
void wxComboboxState::SetValue(wxString sValue)
{
    m_sText = sValue;
}

//-------------------------- DRAWING --------------------------------//
/** Paint the combo box
  * \param pWin     [input]: the window receiving the painting
  * \param rDC      [input]: the device context to use
  * \param rRect    [input]: the clipping rectangle used for painting
  */
void wxComboboxState::DrawComboBoxClientArea(wxWindow *pWin, wxDC &rDC, const wxRect &rRect)
{
    DrawComboBoxClientAreaGeneric(pWin, rDC, rRect);
}

/** Paint the combo box - generic implementation
  * \param pWin     [input]: the window receiving the painting
  * \param rDC      [input]: the device context to use
  * \param rRect    [input]: the clipping rectangle used for painting
  */
void wxComboboxState::DrawComboBoxClientAreaGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &rRect)
{
    wxSize sz(rRect.GetWidth(), rRect.GetHeight());

    //Clear the main background if the system doesn't do it by itself
    if (   (!m_bHasTransparentBackground)
        && ((m_rRectTextArea.x > 0) || (m_rRectTextArea.y > 0))
       )
    {
        rDC.SetBrush(m_cParentBackColour);
        rDC.SetPen(m_cParentBackColour);
        rDC.DrawRectangle(rRect);
    }

    //artificial simple border
    if (m_iWidthBorder)
    {
        // Set border colour
#ifdef __WXMAC__
        wxPen oPen1(wxColour(133,133,133),
                    m_iWidthBorder,
                    wxPENSTYLE_SOLID );
#else
        wxPen oPen1(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT),
                    m_iWidthBorder,
                    wxPENSTYLE_SOLID);
#endif
        rDC.SetPen(oPen1);

        //area around both controls
        wxRect rRectBorder(rRect);
        if (m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE)
        {
            rRectBorder = m_rRectTextArea;
            if (m_iWidthBorder == 1)
            {
                rRectBorder.Inflate(1);
            }
            else
            {
            #ifdef __WXGTK__
                rRectBorder.x -= 1;
                rRectBorder.y -= 1;
            #else
                rRectBorder.x -= m_iWidthBorder;
                rRectBorder.y -= m_iWidthBorder;
            #endif
                rRectBorder.width  += 1 + m_iWidthBorder;
                rRectBorder.height += 1 + m_iWidthBorder;
            }
        }

        rDC.SetBrush(*wxTRANSPARENT_BRUSH);
        rDC.DrawRectangle(rRectBorder);
    }

    if (!m_bHasButton)
    {
        //Standard button rendering
        DrawButton(pWin, rDC, Button_PaintBackground, rRect.x, rRect.y);
    }

    //paint required portion on the control
    wxRect rTextArea(m_rRectTextArea);
    rTextArea.Offset(rRect.x, rRect.y);
    if (!m_bHasText || m_iWidthCustomPaint)
    {
        // Clear the text-control area background
        rDC.SetBrush(m_cBackColour);
        rDC.SetPen(m_cBackColour);
        rDC.DrawRectangle(rTextArea);

        //this is intentionally here to allow drawn rectangle's
        //right edge to be hidden
        if (m_bHasText) rTextArea.width = m_iWidthCustomPaint;

        rDC.SetFont(m_oFont);
        rDC.SetClippingRegion(rTextArea);
        if (m_pPopupInterface) m_pPopupInterface->PaintComboControl(rDC, rTextArea);
        else                   DefaultPaintComboControl(rDC, rTextArea);
        rDC.DestroyClippingRegion();
    }
}

/** Draw the button
  * \param pWin       [input]: the window receiving the painting
  * \param rDC        [input]: the device context to use
  * \param iDrawFlags [input]: combination of EButtonDrawFlags
  * \param x          [input]: X coordinate of the top left corner of the button
  * \param y          [input]: Y coordinate of the top left corner of the button
  */
void wxComboboxState::DrawButton(wxWindow *pWin, wxDC &rDC, int iDrawFlags, int x, int y)
{
    int iDrawState = m_iButtonState;
    if ((m_iFlags & wxCC_BUTTON_STAYS_DOWN) && (m_iPopupState >= wxComboCtrl::Animating))
    {
        iDrawState |= wxCONTROL_PRESSED;
    }

    wxRect rDrawRect(m_rRectButton.x + m_iButtonSpacingX + x,
                     m_rRectButton.y + ((m_rRectButton.height - m_ButtonSize.y ) / 2) + y,
                     m_ButtonSize.x,
                     m_ButtonSize.y);

    // Make sure area is not larger than the control
    if (rDrawRect.y < m_rRectButton.y) rDrawRect.y = m_rRectButton.y;
    if (rDrawRect.height > m_rRectButton.height) rDrawRect.height = m_rRectButton.height;

    if (!m_bIsEnabled) iDrawState |= wxCONTROL_DISABLED;

    //Need to clear button background even if m_btn is present
    //and also when using custom bitmap for the button
    if (   (iDrawFlags & Button_PaintBackground)
        && ((!m_bHasTransparentBackground) || (!(m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE)))
       )
    {
        wxRect rButton(m_rRectButton);
        rButton.Offset(x, y);
        wxColour cBackColour;
        if ( m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE ) cBackColour = m_cParentBackColour;
        else                                        cBackColour = m_cBackColour;

        rDC.SetBrush(cBackColour);
        rDC.SetPen(cBackColour);
        rDC.DrawRectangle(rButton);
    }

    if (!m_bmpNormal.IsOk())
    {
        if (iDrawFlags & Button_BitmapOnly) return;

        // Draw standard button
        wxRendererNative::Get().DrawComboBoxDropButton(pWin,
                                                       rDC,
                                                       rDrawRect,
                                                       iDrawState);
    }
    else
    {
        //Draw bitmap
        const wxBitmap* pBmp;

        if (!m_bIsEnabled)                              pBmp = &m_bmpDisabled;
        else if (m_iButtonState & wxCONTROL_PRESSED)    pBmp = &m_bmpPressed;
        else if (m_iButtonState & wxCONTROL_CURRENT)    pBmp = &m_bmpHover;
        else                                            pBmp = &m_bmpNormal;

        if (m_bBlankButtonBg)
        {
            if (!(iDrawFlags & Button_BitmapOnly))
            {
                wxRendererNative::Get().DrawPushButton(pWin,
                                                       rDC,
                                                       rDrawRect,
                                                       iDrawState);
            }
        }

        // Draw bitmap centered in rDrawRect
        rDC.DrawBitmap(*pBmp,
                       rDrawRect.x + (rDrawRect.width  - pBmp->GetWidth()) / 2,
                       rDrawRect.y + (rDrawRect.height - pBmp->GetHeight()) / 2,
                       true);
    }
}

/** Paint the background and text of the control
  * \param rDC       [input]: the device context to use
  * \param rRectText [input]: where to draw the text
  */
void wxComboboxState::DefaultPaintComboControl(wxDC &rDC, const wxRect &rRectText)
{
    if ((m_iStyle & wxCB_READONLY) || (!m_bHasText))// ie. no textctrl
    {
        PrepareBackground(rDC, rRectText, 0);

        rDC.DrawText(m_sText,
                     rRectText.x + m_iMarginLeft,
                     (rRectText.height - rDC.GetCharHeight()) / 2 + rRectText.y);
    }
}

/** Prepare combo box background on area in a way typical on platform
  * \param rDC       [input]: the device context to use
  */
void wxComboboxState::PrepareBackground(wxDC& rDC, const wxRect& rRect, int iFlags)
{
#if (defined(__WXMSW__)) || defined(__WXUNIVERSAL__)
    bool bIsEnabled;
    bool bDoDrawFocusRect; //also selected

    //For smaller size control (and for disabled background) use less spacing
    int iFocusSpacingX;
    int iFocusSpacingY;

    if (!(iFlags & wxCONTROL_ISSUBMENU))
    {
        //Drawing control
        bIsEnabled = m_bIsEnabled;
        bDoDrawFocusRect = m_bFocus && !(m_iFlags & wxCC_FULL_BUTTON);

        //Windows-style: for smaller size control (and for disabled background) use less spacing
        int iClientHeight = rRect.GetHeight() - m_iWidthBorder * 2;
        iFocusSpacingX = bIsEnabled ? 2 : 1;
        iFocusSpacingY = iClientHeight > (rDC.GetCharHeight() + 2) && bIsEnabled ? 2 : 1;
    }
    else
    {
        //Drawing a list item
        bIsEnabled = true; // they are never disabled
        bDoDrawFocusRect = (iFlags & wxCONTROL_SELECTED) != 0;

        iFocusSpacingX = 0;
        iFocusSpacingY = 0;
    }

    //Set the background sub-rectangle for selection, disabled etc
    wxRect rSelRect(rRect);
    rSelRect.y      +=  iFocusSpacingY;
    rSelRect.height -= (iFocusSpacingY * 2);

    int wcp = 0;

    if (!(iFlags & wxCONTROL_ISSUBMENU)) wcp += m_iWidthCustomPaint;

    rSelRect.x     += wcp + iFocusSpacingX;
    rSelRect.width -= wcp + (iFocusSpacingX * 2);

    wxColour bgCol;
    wxColour fgCol;

    bool bDoDrawSelRect = true;

    //Determine foreground colour
    if (bIsEnabled)
    {
        if (bDoDrawFocusRect)
        {
            fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        }
        else if (m_bHasFgCol)
        {
            // Honour the custom foreground colour
            fgCol = m_cForegroundCol;
        }
        else
        {
            fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        }
    }
    else
    {
        fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    }

    // Determine background colour
    if (bIsEnabled)
    {
        if (bDoDrawFocusRect)
        {
            bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        }
//        else if (m_hasTcBgCol)
//        {
//            // Honour the custom background colour
//            bgCol = m_tcBgCol;
//        }
        else if (m_cBackColour.IsOk())
        {
            bgCol = m_cBackColour;
        }
        else
        {
#ifndef __WXMAC__  // see note in OnThemeChange
            bDoDrawSelRect = false;
            bgCol = m_cBackColour;
#else
            bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif
        }
    }
    else
    {
#ifndef __WXMAC__  // see note in OnThemeChange
        bgCol = m_cBackColour;
#else
        bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif
    }

    rDC.SetTextForeground(fgCol);
    rDC.SetBrush(bgCol);
    if (bDoDrawSelRect)
    {
        rDC.SetPen(bgCol);
        rDC.DrawRectangle(rSelRect);
    }

    //Don't clip exactly to the selection rectangle so we can draw
    //to the non-selected area in front of it.
    wxRect rClipRect(rRect.x,rRect.y,
                    (rSelRect.x+rSelRect.width)-rRect.x,rRect.height);
    rDC.SetClippingRegion(rClipRect);
#else

#endif
}

