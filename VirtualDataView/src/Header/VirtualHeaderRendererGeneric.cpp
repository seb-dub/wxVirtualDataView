/**********************************************************************/
/** FILE    : VirtualHeaderRendererGeneric.cpp                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a generic implementation of wxVirtualHeaderIRenderer   **/
/**********************************************************************/

#include <wx/VirtualDataView/Header/VirtualHeaderRendererGeneric.h>
#include <wx/dcclient.h>
#include <wx/window.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Constructor
  * \param pWin [input]: window receiving the rendering
  */
wxVirtualHeaderRendererGeneric::wxVirtualHeaderRendererGeneric(wxWindow *pWin)
    : wxVirtualHeaderIRenderer(pWin)
{
}

/** Destructor
  */
wxVirtualHeaderRendererGeneric::~wxVirtualHeaderRendererGeneric(void)
{
}

//------------------------ RENDERING --------------------------------//
WX_VDV_INLINE wxRendererNative& GetRenderer(void)
{
    return(wxRendererNative::Get());
}

/** Get the header button size
  * \param rData [input] : additional data for rendering
  * \return the size of the header button
  */
WX_VDV_INLINE wxSize DoGetHeaderButtonSize(wxWindow *pWin,
                                           const wxVirtualHeaderIRenderer::THeaderData &rData)
{
    int iHeight = GetRenderer().GetHeaderButtonHeight(pWin);

    int iFlags = 0;
    if (rData.m_bIsHotTracked) iFlags |= wxCONTROL_CURRENT;
    if (rData.m_bIsCurrent)    iFlags |= wxCONTROL_SELECTED;
    if (rData.m_bIsPushed)     iFlags |= wxCONTROL_PRESSED;

    int iVerticalMargin = 5; //magic constant. Do not touch !

    wxClientDC rDC(pWin);

    //set font
    wxFont f = rData.m_FontLabel;
    if (!f.IsOk())
    {
        if (pWin) f = pWin->GetFont();
        if (!f.IsOk()) f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    }
    wxDCFontChanger dcFontChanger(rDC, f);

    wxRect r(0, 0, 1, 1);
    int iWidth = GetRenderer().DrawHeaderButton(pWin, rDC, r, iFlags,
                                                rData.m_eSortIconType,
                                                (wxHeaderButtonParams *) &rData);

    //take into account size of text
    int iMargin    = GetRenderer().GetHeaderButtonMargin(pWin);

    //text width
    wxSize sTextSize = rDC.GetTextExtent(rData.m_sLabel);
    int iTextWidth = sTextSize.GetWidth();
    if ((iTextWidth > iWidth) || (iWidth <= iMargin)) iWidth = iTextWidth + iMargin;

    //text height
    int iTextHeight = sTextSize.GetHeight();
    if ((iTextHeight + iVerticalMargin * 2 > iHeight)) iHeight = iTextHeight + iVerticalMargin * 2;

    //take into account bitmap size
    if (rData.m_Bitmap.IsOk())
    {
        int iBitmapWidth  = rData.m_Bitmap.GetWidth();
        int iBitmapHeight = rData.m_Bitmap.GetHeight();
        if (iBitmapWidth > 0)
        {
            iWidth += iBitmapWidth;
            //if there is text, add only iMargin / 2 (half margin between bitmap and text)
            //otherwise add the full margin (half margin on left and right)
            if (iTextWidth > 0) iWidth += iMargin / 2;
            else                iWidth += iMargin;
        }

        //bitmap height : add 2 pixels above and below
        if (iBitmapHeight + iVerticalMargin * 2 > iHeight) iHeight = iBitmapHeight + iVerticalMargin * 2;
    }

    return(wxSize(iWidth, iHeight));
}

/** Draw the header background only - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  */
WX_VDV_INLINE void DoDrawBackgroundGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &r)
{
    GetRenderer().DrawHeaderButton(pWin, rDC, r, 0,
                                   wxHDR_SORT_ICON_NONE,
                                   WX_VDV_NULL_PTR);
}

/** Draw the header content, without background - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
WX_VDV_INLINE void DoDrawContentGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &r,
                                        const wxVirtualHeaderIRenderer::THeaderData &rData)
{
    int iFlags = 0;
    if (rData.m_bIsHotTracked) iFlags |= wxCONTROL_CURRENT;
    if (rData.m_bIsCurrent)    iFlags |= wxCONTROL_SELECTED;
    if (rData.m_bIsPushed)     iFlags |= wxCONTROL_PRESSED;

    wxHeaderButtonParams* pData = (wxHeaderButtonParams *) &rData;
    if (rData.m_bIsFiltering)
    {
        wxHeaderButtonParams rHData = *pData;
        rHData.m_labelColour = wxColour(0, 0, 255);
        GetRenderer().DrawHeaderButtonContents(pWin, rDC, r, iFlags,
                                               rData.m_eSortIconType,
                                               &rHData);
    }
    else
    {
        GetRenderer().DrawHeaderButtonContents(pWin, rDC, r, iFlags,
                                               rData.m_eSortIconType,
                                               pData);
    }

}

/** Draw the header content + background - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
WX_VDV_INLINE void DoDrawHeaderGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &r,
                                       const wxVirtualHeaderIRenderer::THeaderData &rData)
{
    int iFlags = 0;
    if (rData.m_bIsHotTracked) iFlags |= wxCONTROL_CURRENT;
    if (rData.m_bIsCurrent)    iFlags |= wxCONTROL_SELECTED;
    if (rData.m_bIsPushed)     iFlags |= wxCONTROL_PRESSED;



    wxHeaderButtonParams* pData = (wxHeaderButtonParams *) &rData;
    if (rData.m_bIsFiltering)
    {
        wxHeaderButtonParams rHData = *pData;
        rHData.m_labelColour = wxColour(0, 0, 255);
        GetRenderer().DrawHeaderButton(pWin, rDC, r, iFlags,
                                       rData.m_eSortIconType,
                                       &rHData);
    }
    else
    {
        GetRenderer().DrawHeaderButton(pWin, rDC, r, iFlags,
                                       rData.m_eSortIconType,
                                       pData);
    }
}

//--------------------- INTERFACE -----------------------------------//
/** Check if the engine is usable
  * \return true if the renderer is usable, false otherwise
  */
bool wxVirtualHeaderRendererGeneric::IsOK(void) const
{
    return(true);
}

/** Get the header button size
  * \param rData [input] : additional data for rendering
  * \return the size of the header button
  */
wxSize wxVirtualHeaderRendererGeneric::GetHeaderButtonSize(const THeaderData &rData)
{
    return(DoGetHeaderButtonSize(m_pWindow, rData));
}

/** Draw the header
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererGeneric::DrawHeader(wxDC &rDC, const wxRect &r, const THeaderData &rData)
{
    DoDrawHeaderGeneric(m_pWindow, rDC, r, rData);
}

/** Draw the header content, without background
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererGeneric::DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                                       const THeaderData &rData)
{
    DoDrawContentGeneric(m_pWindow, rDC, r, rData);
}

/** Draw the header background only
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  */
void wxVirtualHeaderRendererGeneric::DrawHeaderBackground(wxDC &rDC, const wxRect &r)
{
    DoDrawBackgroundGeneric(m_pWindow, rDC, r);
}

//-------------------- FALLBACK METHODS -----------------------------//
/** Get the header button size - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rData    [input] : additional data for rendering
  * \return the header button size
  */
wxSize wxVirtualHeaderRendererGeneric::GetHeaderButtonSizeGeneric(wxWindow *pWin, const THeaderData &rData)
{
    return(DoGetHeaderButtonSize(pWin, rData));
}

/** Draw the header background only - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  */
void wxVirtualHeaderRendererGeneric::DrawBackgroundGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &r)
{
    DoDrawBackgroundGeneric(pWin, rDC, r);
}

/** Draw the header content, without background - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererGeneric::DrawContentGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &r,
                                                        const THeaderData &rData)
{
    DoDrawContentGeneric(pWin, rDC, r, rData);
}

/** Draw the header content + background - static
  * Fallback in case other renderer fails
  * \param pWin     [input] : the window receiving the rendering
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererGeneric::DrawHeaderGeneric(wxWindow *pWin, wxDC &rDC, const wxRect &r,
                                                       const THeaderData &rData)
{
    DoDrawHeaderGeneric(pWin, rDC, r, rData);
}
