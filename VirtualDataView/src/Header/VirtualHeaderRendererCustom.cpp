/**********************************************************************/
/** FILE    : VirtualHeaderRendererCustom.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a custom implementation of wxVirtualHeaderIRenderer    **/
/**********************************************************************/

#include <wx/VirtualDataView/Header/VirtualHeaderRendererCustom.h>
#include <wx/settings.h>
#include <wx/window.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Constructor
  * \param pWin [input]: window receiving the rendering
  */
wxVirtualHeaderRendererCustom::wxVirtualHeaderRendererCustom(wxWindow *pWin)
    : wxVirtualHeaderIRenderer(pWin)
{
    Init();
}

/** Destructor
  */
wxVirtualHeaderRendererCustom::~wxVirtualHeaderRendererCustom(void)
{
}

//------------------------- INTERNAL METHODS ------------------------//
/** Initialization
  */
void wxVirtualHeaderRendererCustom::Init(void)
{
    ResetGraphicSettings(WX_VHRC_NORMAL_STATE_SETTINGS);
    ResetGraphicSettings(WX_VHRC_CURRENT_STATE_SETTINGS);
    ResetGraphicSettings(WX_VHRC_HOTTRACKED_STATE_SETTINGS);
    ResetGraphicSettings(WX_VHRC_PUSHED_STATE_SETTINGS);

    SetMargins(1, 1);
}

//------------------------- RENDERING SETTINGS ----------------------//
/** Set the graphic settings
  * \param eSettings [input]: the index of the settings to set
  * \param rSettings [input]: the new settings
  */
void wxVirtualHeaderRendererCustom::SetGraphicSettings(ESettings eSetting, const TSettings &rSettings)
{
    if ((eSetting < 0) || (eSetting >= s_iNbSettings)) return;
    m_Settings[eSetting] = rSettings;
    CreateDCObjects(eSetting);
}

/** Get the graphic settings
  * \param eSettings [input]: the index of the settings to get
  * \param rSettings [output]: the current settings
  */
void wxVirtualHeaderRendererCustom::GetGraphicSettings(ESettings eSetting,
                                                       TSettings &rSettings) const
{
    int iIndex = eSetting;
    if ((iIndex < 0) || (iIndex >= s_iNbSettings)) iIndex = 0;
    rSettings = m_Settings[iIndex];
}

/** Reset the graphic settings
  * \param eSetting [input]: the settings to reset
  */
void wxVirtualHeaderRendererCustom::ResetGraphicSettings(ESettings eSetting)
{
    switch(eSetting)
    {
        //current state (clicked once)
        case WX_VHRC_CURRENT_STATE_SETTINGS:
            {
                TSettings &rSettings = m_Settings[WX_VHRC_CURRENT_STATE_SETTINGS];
                rSettings.m_cFirstColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
                rSettings.m_cSecondColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
                rSettings.m_iGradientDirection = wxTOP;
                rSettings.m_cTextColour   = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                rSettings.m_Font          = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
                rSettings.m_EdgeLightColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
                rSettings.m_EdgeShadowColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
                rSettings.m_SortArrowColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                CreateDCObjects(WX_VHRC_CURRENT_STATE_SETTINGS);
            }
            break;

        //hot tracked state (mouse hovering)
        case WX_VHRC_HOTTRACKED_STATE_SETTINGS:
            {
                TSettings &rSettings = m_Settings[WX_VHRC_HOTTRACKED_STATE_SETTINGS];
                rSettings.m_cFirstColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
                rSettings.m_cSecondColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
                rSettings.m_iGradientDirection = wxTOP;
                rSettings.m_cTextColour.Set(255, 0, 0);
                rSettings.m_Font          = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
                rSettings.m_EdgeLightColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
                rSettings.m_EdgeShadowColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
                rSettings.m_SortArrowColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                CreateDCObjects(WX_VHRC_HOTTRACKED_STATE_SETTINGS);
            }
            break;

        //pushed state (dragging, reordering)
        case WX_VHRC_PUSHED_STATE_SETTINGS:
            {
                TSettings &rSettings = m_Settings[WX_VHRC_PUSHED_STATE_SETTINGS];
                rSettings.m_cFirstColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
                rSettings.m_cSecondColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
                rSettings.m_iGradientDirection = wxTOP;
                rSettings.m_cTextColour   = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                rSettings.m_Font          = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
                rSettings.m_EdgeLightColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
                rSettings.m_EdgeShadowColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
                rSettings.m_SortArrowColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                CreateDCObjects(WX_VHRC_PUSHED_STATE_SETTINGS);
            }
            break;

        //normal state
        case WX_VHRC_NORMAL_STATE_SETTINGS:
        default                           :
            {
                TSettings &rSettings = m_Settings[WX_VHRC_NORMAL_STATE_SETTINGS];
                rSettings.m_cFirstColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
                rSettings.m_cSecondColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
                rSettings.m_iGradientDirection = wxTOP;
                rSettings.m_cTextColour   = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                rSettings.m_Font          = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
                rSettings.m_EdgeLightColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
                rSettings.m_EdgeShadowColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
                rSettings.m_SortArrowColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
                CreateDCObjects(WX_VHRC_NORMAL_STATE_SETTINGS);
            }
            break;
    }
}

/** Create the DC objects
  * \param eSettings [input]: the settings to apply
  */
void wxVirtualHeaderRendererCustom::CreateDCObjects(ESettings eSetting)
{
    TSettingsObjects &rSO = m_DCObjects[eSetting];
    rSO.m_PenEdgesShadow = wxPen(m_Settings[eSetting].m_EdgeShadowColour, 1, wxPENSTYLE_SOLID);
    rSO.m_PenEdgesLight  = wxPen(m_Settings[eSetting].m_EdgeLightColour, 1, wxPENSTYLE_SOLID);
}

//--------------------- MARGINS -------------------------------------//
/** Get the margins size
  * \param iMarginX [output] : the X margin (amount of free pixels left and right)
  * \param iMarginY [output] : the Y margin (amount of free pixels top and bottom
  */
void wxVirtualHeaderRendererCustom::GetMargins(int &iMarginX, int &iMarginY) const
{
    iMarginX = m_iMarginX;
    iMarginY = m_iMarginY;
}

/** Set the margins size
  * \param iMarginX [input] : the new X margin (amount of free pixels left and right)
  * \param iMarginY [input] : the new Y margin (amount of free pixels top and bottom
  */
void wxVirtualHeaderRendererCustom::SetMargins(int iMarginX, int iMarginY)
{
    m_iMarginX = iMarginX;
    m_iMarginY = iMarginY;
}

//------------------ INLINED DRAW HELPERS ---------------------------//
/** Get current settings
  * \param rData [input] : additional data for rendering
  * \return the index of the settings to use
  */
WX_VDV_INLINE wxVirtualHeaderRendererCustom::ESettings
    wxVirtualHeaderRendererCustom::DoGetCurrentSetting(const THeaderData &rData) const
{
    ESettings eSettings = WX_VHRC_NORMAL_STATE_SETTINGS;
         if (rData.m_bIsPushed)     eSettings = WX_VHRC_PUSHED_STATE_SETTINGS;
    else if (rData.m_bIsCurrent)    eSettings = WX_VHRC_CURRENT_STATE_SETTINGS;
    else if (rData.m_bIsHotTracked) eSettings = WX_VHRC_HOTTRACKED_STATE_SETTINGS;
    return(eSettings);
}

/** Draw the header background
  * \param rDC       [input] : the DC to use for rendering
  * \param r         [input] : the clipping rectangle
  * \param eSetting  [input] : the settings state
  * \param cColour   [input] : override colour
  */
WX_VDV_INLINE void wxVirtualHeaderRendererCustom::DoDrawHeaderBackground(wxDC &rDC,
                                                                         const wxRect &r,
                                                                         ESettings eSetting,
                                                                         const wxColour &cColour) const
{
    const TSettings &rSettings = m_Settings[eSetting];
    const TSettingsObjects &rObjects = m_DCObjects[eSetting];

    //draw background
    if ((cColour.IsOk()) && (eSetting == WX_VHRC_CURRENT_STATE_SETTINGS))
    {
        wxBrush b(cColour, wxBRUSHSTYLE_SOLID);
        wxPen   p(cColour, 1, wxPENSTYLE_SOLID);
        wxDCBrushChanger dcb(rDC, b);
        wxDCPenChanger dcp(rDC, p);

        rDC.DrawRectangle(r);
    }
    else
    {
        rDC.GradientFillLinear(r, rSettings.m_cFirstColour,
                                  rSettings.m_cSecondColour,
                                  rSettings.m_iGradientDirection);
    }


    //draw edges
    const int x0 = r.x;
    const int x1 = x0 + r.width - 1;
    const int y0 = r.y;
    const int y1 = y0 + r.height - 1;

    rDC.SetPen(rObjects.m_PenEdgesLight);
    rDC.DrawLine(x0, y0, x1, y0);
    rDC.DrawLine(x0, y0, x0, y1);

    rDC.SetPen(rObjects.m_PenEdgesShadow);
    rDC.DrawLine(x1, y0 + 1, x1, y1);
    rDC.DrawLine(x0 + 1, y1, x1, y1);
}

/** Draw the sort arrow
  * \param pts      [input] : an array of 3 wxPoint, corresponding to the 3 corners of the triangle
  * \param rData    [input] : additional data for rendering
  * \param eSetting [input] : the graphic attributes set to use
  */
WX_VDV_INLINE void wxVirtualHeaderRendererCustom::DoDrawArrow(wxDC &rDC, wxPoint *pts, const THeaderData &rData,
                                                              ESettings eSetting) const
{
    wxColour cColour = rData.m_ArrowColour;
    if (!cColour.IsOk()) cColour = m_Settings[eSetting].m_SortArrowColour;
    if (!cColour.IsOk()) cColour = wxColour(255, 0, 0);

    wxBrush b(cColour, wxBRUSHSTYLE_SOLID);
    wxPen p(cColour, 1, wxPENSTYLE_SOLID);

    wxDCBrushChanger dcb(rDC, b);
    wxDCPenChanger dcp(rDC, p);
    rDC.DrawPolygon(3, pts);
}

/** Draw the header content
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
WX_VDV_INLINE void wxVirtualHeaderRendererCustom::DoDrawHeaderContent(wxDC &rDC, const wxRect &r,
                                                                      const THeaderData &rData) const
{
    //get settings
    ESettings eSettings = DoGetCurrentSetting(rData);

    //text & bitmap
    wxFont f = m_Settings[eSettings].m_Font;
    if (rData.m_FontLabel.IsOk()) f = rData.m_FontLabel;
    if (!f.IsOk()) f = rDC.GetFont();

    wxColour cColour = m_Settings[eSettings].m_cTextColour;
    if (rData.m_LabelColour.IsOk()) cColour = rData.m_LabelColour;
    if (!cColour.IsOk()) cColour = rDC.GetTextForeground();

    wxDCFontChanger dcFont(rDC, f);
    wxDCTextColourChanger dcTextColour(rDC, cColour);
    rDC.DrawLabel(rData.m_sLabel, rData.m_Bitmap, r, rData.m_iAlignment);

    //sort arrow
    const int iWidth     = 8;
    const int iHeight    = 4;
    const int iHalfWidth = 4;
    const int iHalfHeight= iHeight / 2;
    switch(rData.m_eSortIconType)
    {
        case wxHDR_SORT_ICON_UP   : {
                                        wxPoint pts[3];
                                        int iRectRight = r.x + r.width - m_iMarginX - 2;
                                        int yMiddleLine = r.y + r.height / 2;
                                        pts[1].x = iRectRight;
                                        pts[1].y = yMiddleLine + iHalfHeight;

                                        pts[0].x = pts[1].x - iWidth;
                                        pts[0].y = pts[1].y;

                                        pts[2].x = iRectRight - iHalfWidth;
                                        pts[2].y = yMiddleLine - iHalfHeight;

                                        DoDrawArrow(rDC, pts, rData, eSettings);
                                    }
                                    break;

        case wxHDR_SORT_ICON_DOWN : {
                                        wxPoint pts[3];
                                        int iRectRight = r.x + r.width - m_iMarginX - 2;
                                        int yMiddleLine = r.y + r.height / 2;
                                        pts[1].x = iRectRight;
                                        pts[1].y = yMiddleLine - iHalfHeight;

                                        pts[0].x = pts[1].x - iWidth;
                                        pts[0].y = pts[1].y;

                                        pts[2].x = iRectRight - iHalfWidth;
                                        pts[2].y = yMiddleLine + iHalfHeight;

                                        DoDrawArrow(rDC, pts, rData, eSettings);
                                    }
                                    break;

        case wxHDR_SORT_ICON_NONE :
        default                   : break;
    }
}

//--------------------- INTERFACE -----------------------------------//
/** Check if the engine is usable
  * \return true if the renderer is usable, false otherwise
  */
bool wxVirtualHeaderRendererCustom::IsOK(void) const
{
    return(true);
}

/** Get the header button height
  * \param rData [input] : additional data for rendering
  * \return the height of the header button
  */
wxSize wxVirtualHeaderRendererCustom::GetHeaderButtonSize(const THeaderData &rData)
{
    //get minimal height
    int iWidth  = 20;
    int iHeight = 10;

    //add text size
    int w, h;
    if (m_pWindow)
    {
        //get settings
        ESettings eSettings = DoGetCurrentSetting(rData);

        wxFont f = m_Settings[eSettings].m_Font;
        if (!f.IsOk()) f = m_pWindow->GetFont();
        m_pWindow->GetTextExtent(rData.m_sLabel, &w, &h, WX_VDV_NULL_PTR, WX_VDV_NULL_PTR, &f);

        w += 2 * m_iMarginX;
        h += 2 * m_iMarginY;
    }
    else
    {
        w = 0;
        h = 0;
    }
    if (w > iWidth)  iWidth  = w;
    if (h > iHeight) iHeight = h;

    //add bitmap size
    if (rData.m_Bitmap.IsOk())
    {
        w = rData.m_Bitmap.GetWidth()  + m_iMarginX; //already added with text
        h = rData.m_Bitmap.GetHeight() + 2 * m_iMarginY;
        if (w > iWidth)  iWidth  = w;
        if (h > iHeight) iHeight = h;
    }

    //return
    return(wxSize(iWidth, iHeight));
}

/** Draw the header
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererCustom::DrawHeader(wxDC &rDC, const wxRect &r, const THeaderData &rData)
{
    ESettings eSettings = DoGetCurrentSetting(rData);
    DoDrawHeaderBackground(rDC, r, eSettings, rData.m_SelectionColour);
    DoDrawHeaderContent(rDC, r, rData);
}

/** Draw the header content, without background
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  * \param rData    [input] : additional data for rendering
  */
void wxVirtualHeaderRendererCustom::DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                                       const THeaderData &rData)
{
    DoDrawHeaderContent(rDC, r, rData);
}

/** Draw the header background only
  * \param rDC      [input] : the DC to use for rendering
  * \param r        [input] : the clipping rectangle
  */
void wxVirtualHeaderRendererCustom::DrawHeaderBackground(wxDC &rDC, const wxRect &r)
{
    DoDrawHeaderBackground(rDC, r, WX_VHRC_NORMAL_STATE_SETTINGS, wxNullColour);
}

/** Prepare the DC for rendering
  * \param rDC [input]: the DC to prepare
  * The default implementation does nothing
  */
void wxVirtualHeaderRendererCustom::PrepareDC(wxDC &rDC)
{
    //text background colour
    wxColour cBackColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND);
    wxBrush b(cBackColour, wxBRUSHSTYLE_SOLID);
    wxPen p(cBackColour, wxPENSTYLE_SOLID);

    //text foreground colour
    wxColour cForeColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); //wxSYS_COLOUR_CAPTIONTEXT
    rDC.SetTextForeground(cForeColour);

    //font
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT); //wxSYS_SYSTEM_FONT
    rDC.SetFont(f);
}
