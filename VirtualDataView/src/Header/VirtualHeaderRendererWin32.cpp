/**********************************************************************/
/** FILE    : VirtualHeaderRendererWin32.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a generic implementation of wxVirtualHeaderIRenderer   **/
/**********************************************************************/

#include <wx/VirtualDataView/Header/VirtualHeaderRendererWin32.h>
#include <wx/VirtualDataView/Header/VirtualHeaderRendererGeneric.h>

#if wxUSE_MS_WIN_THEME != 0
#include <wx/dcclient.h>

#include <vsstyle.h>
#include <vssym32.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Constructor
  * \param pWin [input]: window receiving the rendering
  */
wxVirtualHeaderRendererWin32::wxVirtualHeaderRendererWin32(wxWindow *pWin)
    : wxVirtualHeaderIRenderer(pWin),
      m_pThemeEngine(wxUxThemeEngine::Get()),
      m_HeaderTheme(pWin, L"HEADER")
{
    if (!m_pThemeEngine)
    {

    }
}

/** Destructor
  */
wxVirtualHeaderRendererWin32::~wxVirtualHeaderRendererWin32(void)
{
}

//----------------- HELPER ------------------------------------------//
/** Helper: convert a wxRect to a Win32 RECT
  * \param rDest [output]: the Win32 RECT
  * \param rSrc  [input] : the wxRect to convert
  */
WX_VDV_INLINE void wxRectToWin32Rect(RECT &rDest, const wxRect &rSrc)
{
    rDest.left   = rSrc.x;
    rDest.right  = rSrc.x + rSrc.width;
    rDest.top    = rSrc.y;
    rDest.bottom = rSrc.y + rSrc.height;
}

/** Draw the header background only, using Win32 Theme engine
  * \param pThemeEngine [input] : the theme engine
  * \param rhTheme      [input] : the theme handle (HTHEME wrapper)
  * \param hWnd         [input] : the Win32 window handle
  * \param hDC          [input] : the Win32 device context
  * \param rRect        [input] : the Win32 RECT
  * \param iState       [input] : item state
  * \return true on success, false on failure
  */
WX_VDV_INLINE bool DoDrawHeaderBackground(wxUxThemeEngine *pThemeEngine,
                                          const wxThemeHandleWin32 &rhTheme,
                                          HWND hWnd, HDC hDC, RECT &rRect,
                                          int iState)
{
    int iPart  = HP_HEADERITEM;

    HRESULT hr;
    if (pThemeEngine->IsThemeBackgroundPartiallyTransparent(rhTheme, iPart, iState))
    {
        hr = pThemeEngine->DrawThemeParentBackground(hWnd, hDC, &rRect);
        if (FAILED(hr)) return(false);
        pThemeEngine->DrawThemeBackground(rhTheme, hDC, iPart, iState, &rRect, WX_VDV_NULL_PTR);
    }
    else
    {
        hr = pThemeEngine->DrawThemeBackground(rhTheme, hDC, iPart, iState, &rRect, WX_VDV_NULL_PTR);
        if (FAILED(hr)) return(false);
    }
    return(true);
}

/** Draw the header background only, using Win32 Theme engine
  * \param pThemeEngine [input] : the theme engine
  * \param rhTheme      [input] : the theme handle (HTHEME wrapper)
  * \param hWnd         [input] : the Win32 window handle
  * \param hDC          [input] : the Win32 device context
  * \param rRect        [input] : the Win32 RECT
  * \param rData        [input] : additional rendering data
  * \return true on success, false on failure
  */
WX_VDV_INLINE bool DoDrawHeaderBackground(wxUxThemeEngine *pThemeEngine,
                                          const wxThemeHandleWin32 &rhTheme,
                                          HWND hWnd, HDC hDC, RECT &rRect,
                                          const wxVirtualHeaderIRenderer::THeaderData &rData)
{
    int iState = HIS_NORMAL;

    if (rData.m_bIsHotTracked) iState = HIS_HOT;
    if (rData.m_bIsCurrent)    iState = HIS_SORTEDNORMAL;
    if (rData.m_bIsPushed)     iState = HIS_PRESSED;

    //draw
    bool bResult = DoDrawHeaderBackground(pThemeEngine, rhTheme, hWnd, hDC, rRect, iState);

    //filtering : draw a pink rectangle (RGB = 255, 192, 203)
//    if (rData.m_bIsFiltering)
//    {
//        RECT rFilterRect = rRect;
//        rFilterRect.top = (rRect.bottom - rRect.top) / 2;
//        HBRU::DeleteObject(hBr);SH hBr = ::CreateSolidBrush(RGB(255, 192, 203));
//        ::FillRect(hDC, &rFilterRect, hBr);
//        ::DeleteObject(hBr);
//    }

    //special case : use selection colour when selected
    if ((rData.m_bIsCurrent) && (rData.m_SelectionColour.IsOk()))
    {
        RECT rDeflatedRect = rRect;
        rDeflatedRect.bottom -= 1;
        rDeflatedRect.top    += 1;
        rDeflatedRect.left   += 1;
        rDeflatedRect.right  -= 1;
        HBRUSH hBr = ::CreateSolidBrush(rData.m_SelectionColour.GetPixel());
        FillRect(hDC, &rDeflatedRect, hBr);
        ::DeleteObject(hBr);
        return(true);
    }

    return(bResult);
}

/** Draw the sort arrow only, using Win32 Theme engine
  * \param pThemeEngine [input] : the theme engine
  * \param rhTheme      [input] : the theme handle (HTHEME wrapper)
  * \param hDC          [input] : the Win32 device context
  * \param rRect        [input] : the Win32 RECT
  * \param iArrow       [input] : the arrow to use
  * \param cColour      [input] : the colour to use
  * \return true on success, false on failure
  */
WX_VDV_INLINE bool DoDrawSortArrow(wxUxThemeEngine *pThemeEngine,
                                   const wxThemeHandleWin32 &rhTheme, HDC hDC,
                                   RECT &rRect, int iArrow, const wxColour &cColour)
{
    //content : sort icon. Warning : there are some magic constants here !
    //The sorting arrow position is not something exposed by the theme engine...
    RECT rArrow;

    rArrow.left   = rRect.left;
    rArrow.right  = rRect.right;
    rArrow.top    = rRect.top;
    LONG lHeigth = rRect.bottom - rRect.top;
    rArrow.bottom = rRect.top + lHeigth / 2 - 4;
    if (rArrow.bottom < rRect.top + 4) rArrow.bottom = rRect.top + 4;

    if (cColour.IsOk())
    {
        COLORREF cCol = cColour.GetPixel();
        POINT pts[3];
        HPEN hPe = WX_VDV_NULL_PTR;
        HBRUSH hBr = WX_VDV_NULL_PTR;
        HGDIOBJ hOldPen = WX_VDV_NULL_PTR;
        HGDIOBJ hOldBr  = WX_VDV_NULL_PTR;
        int x0, y0, y1;
        int iWidth  = 3;
        int iTopMargin = 2;
        int iHeight = 3;

        switch(iArrow)
        {
            case wxHDR_SORT_ICON_UP     :   x0 = (rArrow.left + rArrow.right) / 2 - 1;
                                            y0 = rArrow.top + iTopMargin;
                                            y1 = y0 + iHeight;
                                            pts[0].x = x0 - iWidth;
                                            pts[0].y = y1;
                                            pts[1].x = x0 + iWidth;
                                            pts[1].y = pts[0].y;
                                            pts[2].x = x0;
                                            pts[2].y = y0;

                                            hBr = ::CreateSolidBrush(cCol);
                                            hPe = ::CreatePen(PS_SOLID, 1, cCol);
                                            hOldPen = ::SelectObject(hDC, hPe);
                                            hOldBr  = ::SelectObject(hDC, hBr);
                                            ::Polygon(hDC, pts, 3);
                                            ::SelectObject(hDC, hOldPen);
                                            ::SelectObject(hDC, hOldBr);
                                            ::DeleteObject(hPe);
                                            ::DeleteObject(hBr);
                                            break;

            case wxHDR_SORT_ICON_DOWN   :   x0 = (rArrow.left + rArrow.right) / 2 - 1;
                                            y0 = rArrow.top + iTopMargin;
                                            y1 = y0 + iHeight;
                                            pts[0].x = x0 - iWidth;
                                            pts[0].y = y0;
                                            pts[1].x = x0 + iWidth;
                                            pts[1].y = pts[0].y;
                                            pts[2].x = x0;
                                            pts[2].y = y1;

                                            hBr = ::CreateSolidBrush(cCol);
                                            hPe = ::CreatePen(PS_SOLID, 1, cCol);
                                            hOldPen = ::SelectObject(hDC, hPe);
                                            hOldBr  = ::SelectObject(hDC, hBr);
                                            ::Polygon(hDC, pts, 3);
                                            ::SelectObject(hDC, hOldPen);
                                            ::SelectObject(hDC, hOldBr);
                                            ::DeleteObject(hPe);
                                            ::DeleteObject(hBr);
                                            break;

            case wxHDR_SORT_ICON_NONE   :
            default                     :   break;
        }


        return(true);
    }
    else
    {
        HRESULT hr = S_OK;
        switch(iArrow)
        {
            case wxHDR_SORT_ICON_UP     :   hr = pThemeEngine->DrawThemeBackground(rhTheme, hDC,
                                                                                   HP_HEADERSORTARROW, HSAS_SORTEDUP,
                                                                                   &rArrow, WX_VDV_NULL_PTR);
                                            break;

            case wxHDR_SORT_ICON_DOWN   :   hr = pThemeEngine->DrawThemeBackground(rhTheme, hDC,
                                                                                   HP_HEADERSORTARROW, HSAS_SORTEDDOWN,
                                                                                   &rArrow, WX_VDV_NULL_PTR);
                                            break;

            case wxHDR_SORT_ICON_NONE   :
            default                     :   break;
        }
        if (FAILED(hr)) return(false);
    }
    return(true);
}

/** Draw the header content only, using Win32 Theme engine
  * \param pThemeEngine [input] : the theme engine
  * \param rhTheme      [input] : the theme handle (HTHEME wrapper)
  * \param hDC          [input] : the Win32 device context
  * \param rRect        [input] : the Win32 RECT
  * \param rData        [input] : additional rendering data
  * \return true on success, false on failure
  */
WX_VDV_INLINE bool DoDrawHeaderContent(wxUxThemeEngine *pThemeEngine,
                                       const wxThemeHandleWin32 &rhTheme,
                                       HDC hDC, RECT &rRect,
                                       const wxVirtualHeaderIRenderer::THeaderData &rData)
{
    int iPart  = HP_HEADERITEM;
    int iState = HIS_NORMAL;
    HRESULT hr;

    //formatting
    DWORD dwFormat = 0;
         if (rData.m_iAlignment & wxALIGN_RIGHT)             dwFormat = DT_RIGHT;
    else if (rData.m_iAlignment & wxALIGN_CENTER_HORIZONTAL) dwFormat = DT_CENTER;
    else                                                     dwFormat = DT_LEFT;
         if (rData.m_iAlignment & wxALIGN_TOP)               dwFormat |= DT_TOP;
    else if (rData.m_iAlignment & wxALIGN_CENTER_VERTICAL)   dwFormat |= DT_VCENTER;
    else                                                     dwFormat |= DT_BOTTOM;
    dwFormat |= DT_SINGLELINE;

    //normally, we should call GetThemeTextExtent for computing the final text rectangle
    //and pass it to DrawThemeText. But it does not do anything good.
    //Calling DrawThemeText directly works though. The MSDN docs are appalling for the
    //theme engines...
//    RECT rComputedRect;
//    hr = pThemeEngine->GetThemeTextExtent(rhTheme, hDC, iPart, iState,
//                                         (const wchar_t*) rData.m_sLabel.c_str(), -1,
//                                         dwFormat | DT_CALCRECT,
//                                         &rRect, &rComputedRect);


    //content : bitmap
    int iBmpWidth  = 0;
    int iBmpHeight = 0;
    int x0 = 1;
    int y0 = 1;
    if (rData.m_Bitmap.IsOk())
    {
        HBITMAP hBmp = rData.m_Bitmap.GetHBITMAP();
        iBmpWidth  = rData.m_Bitmap.GetWidth();
        iBmpHeight = rData.m_Bitmap.GetHeight();

        HDC hDCMem = ::CreateCompatibleDC(hDC);
        HGDIOBJ hOldBmp = ::SelectObject(hDCMem, hBmp);
//        if (rData.m_Bitmap.HasAlpha())
//        {
//            BLENDFUNCTION op;
//            op.BlendOp = AC_SRC_OVER;
//            op.BlendFlags = 0;
//            op.AlphaFormat = AC_SRC_ALPHA;
//            op.SourceConstantAlpha = 255;
//            ::AlphaBlend(hDC   , x0, y0, iBmpWidth, iBmpHeight,
//                         hDCMem, 0 , 0 , iBmpWidth, iBmpHeight,
//                         op);
//        }
//        else if (rData.m_Bitmap.GetMask())
        if (rData.m_Bitmap.GetMask())
        {
            #define DSTCOPY 0x00AA0029
            wxBitmap bmpMask = rData.m_Bitmap.GetMask()->GetBitmap();
            HBITMAP hBmpMask = bmpMask.GetHBITMAP();
            ::MaskBlt(hDC, x0, y0, iBmpWidth, iBmpHeight,
                      hDCMem, 0, 0, hBmpMask, 0, 0, MAKEROP4(SRCCOPY, DSTCOPY));
            #undef DSTCOPY
        }
        else
        {
            ::BitBlt(hDC, x0, y0, iBmpWidth, iBmpHeight, hDCMem, 0, 0, SRCCOPY);
        }

        ::SelectObject(hDCMem, hOldBmp);
        ::DeleteDC(hDCMem);
    }

    //adjust rect after bitmap
    if ((iBmpWidth > 0) && (iBmpHeight > 0))
    {
        rRect.left += iBmpWidth + 2;
        rRect.top  += iBmpWidth + 2;
    }

    //content : text label
    if (rData.m_bIsFiltering)
    {
        COLORREF cOldColor = ::SetTextColor(hDC, RGB(0, 0, 255));
        ::DrawText(hDC, (const wchar_t*) rData.m_sLabel.c_str(), -1, &rRect, dwFormat);
        ::SetTextColor(hDC, cOldColor);
    }
    else
    {
        if ((rData.m_FontLabel.IsOk()) || (rData.m_LabelColour.IsOk()))
        {
            //override theme font or colour
            HGDIOBJ hOldFont = WX_VDV_NULL_PTR;
            COLORREF cOldColor = 0x00000000;
            if (rData.m_FontLabel.IsOk())   hOldFont = ::SelectObject(hDC, rData.m_FontLabel.GetHFONT());
            if (rData.m_LabelColour.IsOk()) cOldColor = ::SetTextColor(hDC, rData.m_LabelColour.GetPixel());
            ::DrawText(hDC, (const wchar_t*) rData.m_sLabel.c_str(), -1, &rRect, dwFormat);

            if (rData.m_FontLabel.IsOk())  ::SelectObject(hDC, hOldFont);
            if (rData.m_LabelColour.IsOk()) ::SetTextColor(hDC, cOldColor);
        }
        else
        {
            hr = pThemeEngine->DrawThemeText(rhTheme, hDC, iPart, iState,
                                             (const wchar_t*) rData.m_sLabel.c_str(), -1,
                                             dwFormat, 0,
                                             &rRect);
            if (FAILED(hr)) return(false);
        }
    }

    //content : sort arrow
    int iArrow = rData.m_eSortIconType;
    bool bResult = DoDrawSortArrow(pThemeEngine, rhTheme, hDC, rRect, iArrow, rData.m_ArrowColour);

    return(bResult);
}

//--------------------- INTERFACE -----------------------------------//
/** Check if the engine is usable
  * \return true if the renderer is usable, false otherwise
  */
bool wxVirtualHeaderRendererWin32::IsOK(void) const
{
    if (!m_pThemeEngine) return(false);
    if (!m_HeaderTheme)  return(false);
    return(true);
}

/** Get the header button height
  * \param rData [input] : additional data for rendering
  * \return the height of the header button
  */
wxSize wxVirtualHeaderRendererWin32::GetHeaderButtonSize(const THeaderData &rData)
{
    //initialize to the generic implementation
    wxSize sMinSize;
    sMinSize = wxVirtualHeaderRendererGeneric::GetHeaderButtonSizeGeneric(m_pWindow, rData);

    // create a temporary header window just to get its geometry
    HWND hHeader = ::CreateWindow(WC_HEADER, WX_VDV_NULL_PTR, 0,
                                  0, 0, 0, 0,
                                  WX_VDV_NULL_PTR, WX_VDV_NULL_PTR,
                                  WX_VDV_NULL_PTR, WX_VDV_NULL_PTR);
    if (!hHeader) return(sMinSize);

    //Set the font, even if it's the default one, before measuring the window.
    wxFont font = rData.m_FontLabel;
    if ((m_pWindow) && (!font.IsOk()))
    {
        font = m_pWindow->GetFont();
    }
    if (!font.IsOk())
    {
        font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    }
    ::SendMessage(hHeader, WM_SETFONT, (WPARAM)GetHfontOf(font), 0);

    //initialize the struct filled with the values by Header_Layout()
    //The Header_Layout does not really compute the width : so just get the height
    RECT rParentRect = { 0, 0, 10, 10 };
    WINDOWPOS wp = { 0, 0, 0, 0, 0, 0, 0 };
    HDLAYOUT hdl = { &rParentRect, &wp };

    if (Header_Layout(hHeader, &hdl)) sMinSize.SetHeight(wp.cy);

    //compute text size
    const int iHorizMargin = 5;
    const int iVertMargin  = 5;
    int iWidth = 0;
    int iHeight = 0;
    HDC hDC = GetDC(hHeader);
    if (hDC)
    {
        HGDIOBJ hOldFont = ::SelectObject(hDC, GetHfontOf(font));
        SIZE sTextSize;
        if (GetTextExtentPoint32(hDC, rData.m_sLabel.c_str(), rData.m_sLabel.Len(), &sTextSize))
        {
            //add a margin of 5 left, right, top and bottom
            iWidth  = sTextSize.cx + 2 * iHorizMargin;
            iHeight = sTextSize.cy + 2 * iVertMargin;
        }
        ::SelectObject(hDC, hOldFont);
        ::ReleaseDC(hHeader, hDC);
    }

    //compute bitmap size
    if (rData.m_Bitmap.IsOk())
    {
        int iBmpWidth  = rData.m_Bitmap.GetWidth();
        int iBmpHeight = rData.m_Bitmap.GetHeight();
        if (iWidth > 0)
        {
            //there is already text : add one margin between bitmap and text
            iWidth += iBmpWidth + iHorizMargin;
        }
        else
        {
            iWidth = iBmpWidth + 2 * iHorizMargin;
        }
        if (iBmpHeight + 2 * iVertMargin > iHeight)
        {
            iHeight = iBmpHeight + 2 * iVertMargin;
        }
    }

    //cleanup
    ::DestroyWindow(hHeader);

    //adjust size
    if (iWidth > sMinSize.GetWidth()) sMinSize.SetWidth(iWidth);
    if (iHeight > sMinSize.GetHeight()) sMinSize.SetHeight(iHeight);

    return(sMinSize);
}

/** Draw the header
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererWin32::DrawHeader(wxDC &rDC, const wxRect &r, const THeaderData &rData)
{
    //main GDI data
    RECT rr;
    wxRectToWin32Rect(rr, r);
    HDC hDC = rDC.GetHDC();
    HWND hWnd = m_pWindow->GetHWND();
    if (!m_pThemeEngine) return;

    //background
    if (!DoDrawHeaderBackground(m_pThemeEngine, m_HeaderTheme, hWnd, hDC, rr, rData))
    {
        wxVirtualHeaderRendererGeneric::DrawHeaderGeneric(m_pWindow,
                                                          rDC, r, rData);
        return;
    }

    DoDrawHeaderContent(m_pThemeEngine, m_HeaderTheme, hDC, rr, rData);
}

/** Draw the header content, without background
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererWin32::DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                                     const THeaderData &rData)
{
    //main GDI data
    RECT rr;
    wxRectToWin32Rect(rr, r);
    HDC hDC = rDC.GetHDC();
    if (!m_pThemeEngine) return;

    //content
    if (!DoDrawHeaderContent(m_pThemeEngine, m_HeaderTheme, hDC, rr, rData))
    {
        wxVirtualHeaderRendererGeneric::DrawContentGeneric(m_pWindow,
                                                           rDC, r, rData);
    }
}

/** Draw the header background only
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  */
void wxVirtualHeaderRendererWin32::DrawHeaderBackground(wxDC &rDC, const wxRect &r)
{
    //main GDI data
    RECT rr;
    wxRectToWin32Rect(rr, r);
    HDC hDC = rDC.GetHDC();
    HWND hWnd = m_pWindow->GetHWND();
    if (!m_pThemeEngine) return;

    //background
    if (!DoDrawHeaderBackground(m_pThemeEngine, m_HeaderTheme, hWnd, hDC, rr, HIS_NORMAL))
    {
        wxVirtualHeaderRendererGeneric::DrawBackgroundGeneric(m_pWindow, rDC, r);
    }
}


/** Reset theme data
  * To respond for wxSysColourChangedEvent
  */
void wxVirtualHeaderRendererWin32::ResetTheme(void)
{
    //we need to reset all the methods pointers
    m_pThemeEngine = wxUxThemeEngine::Get();
    m_HeaderTheme.Reopen(GetWindow(), L"HEADER");
}

//-------------------------- SETTINGS -------------------------------//
/** Retrieve back colours of background (assumed gradient)
  * This is mainly a test / debug - it has no real use, and is not accurate
  * \param rcFirst      [output]: the 1st gradient colour
  * \param rcSecond     [output]: the 2nd gradient colour
  */
void wxVirtualHeaderRendererWin32::GetBackColours(wxColour &rcFirst, wxColour &rcSecond) const
{
    if (IsOK())
    {
        COLORREF c1 = m_pThemeEngine->GetThemeSysColor(m_HeaderTheme, COLOR_BTNFACE); //COLOR_3DFACE
        COLORREF c2 = m_pThemeEngine->GetThemeSysColor(m_HeaderTheme, COLOR_WINDOW);
        rcFirst.Set( c1        & 0x000000FF,
                    (c1 >> 8 ) & 0x000000FF,
                    (c1 >> 16) & 0x000000FF);
        rcSecond.Set( c2        & 0x000000FF,
                     (c2 >> 8 ) & 0x000000FF,
                     (c2 >> 16) & 0x000000FF);
    }
    else
    {
        rcFirst = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
        rcSecond = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    }
}

#endif
