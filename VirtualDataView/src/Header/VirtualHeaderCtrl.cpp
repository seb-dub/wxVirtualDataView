/**********************************************************************/
/** FILE    : VirtualHeaderCtrl.cpp                                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : generic header control with native look & feel         **/
/**********************************************************************/


#include <wx/VirtualDataView/Header/VirtualHeaderCtrl.h>
#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>
#include <wx/VirtualDataView/Header/VirtualHeaderRendererNative.h>
#include <wx/VirtualDataView/Header/VirtualHeaderRendererGeneric.h>
#include <wx/VirtualDataView/Header/VirtualHeaderRendererCustom.h>
#include <wx/VirtualDataView/Header/VirtualHeaderRearrangeDlg.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>
#include <wx/VirtualDataView/FilterEditors/VirtualDataViewIFilterEditor.h>

#include <wx/renderer.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/menu.h>
#include <iostream>

//---------------------- STATIC DATA --------------------------------//
const char wxVirtualHeaderCtrlNameStr[] = "wxVirtualHeaderCtrl";
const size_t wxVirtualHeaderCtrl::s_uiNotFound = size_t(-1);

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualHeaderCtrl::wxVirtualHeaderCtrl(void)
    : wxSystemThemedControl<wxControl>(),
      m_pRenderer(WX_VDV_NULL_PTR),
      m_pDragImage(WX_VDV_NULL_PTR),
      m_pActiveFilterEditor(WX_VDV_NULL_PTR),
      m_uiActiveFilteredItem(size_t(-1))
{
    Init();
}

/** Constructor
  * \param pParent [input]: the parent window
  * \param id      [input]: the window ID (default = wxID_ANY)
  * \param ptPos   [input]: the initial position (default = wxDefaultPosition)
  * \param size    [input]: the initial size (default = wxDefaultSize)
  * \param lStyle  [input]: the header style. See wxHeaderCtrl for more info.
  *                         default value = wxHD_DEFAULT_STYLE
  * \param sName   [input]: window name
  */
wxVirtualHeaderCtrl::wxVirtualHeaderCtrl(wxWindow *pParent,
                                                 wxWindowID id,
                                                 const wxPoint &ptPos,
                                                 const wxSize &size,
                                                 long lStyle,
                                                 const wxString &sName)
    : wxSystemThemedControl<wxControl>(),
      m_pRenderer(WX_VDV_NULL_PTR),
      m_pDragImage(WX_VDV_NULL_PTR),
      m_pActiveFilterEditor(WX_VDV_NULL_PTR),
      m_uiActiveFilteredItem(size_t(-1))
{
    Create(pParent, id, ptPos, size, lStyle);
    Init();
}

/** Destructor
  */
wxVirtualHeaderCtrl::~wxVirtualHeaderCtrl(void)
{
    if (m_pRenderer) delete(m_pRenderer);
    m_pRenderer = WX_VDV_NULL_PTR;

    if (m_pDragImage) delete(m_pDragImage);
    m_pDragImage = WX_VDV_NULL_PTR;
}

//------------------------ INTERNAL ---------------------------------//
/** Main initialization
  */
void wxVirtualHeaderCtrl::Init(void)
{
    //event handlers
    m_uiCurrentMouseSpot    = s_uiNotFound;
    m_uiCurrentSelection    = s_uiNotFound;
    m_bPushingButton        = false;
    m_bIsDragging           = false;
    m_uiResizeBorderSize    = 4;
    m_eOrientation          = WX_VDV_COL_HEADER;
    m_bShowFilters          = false;
    m_pActiveFilterEditor   = WX_VDV_NULL_PTR;
    m_uiActiveFilteredItem  = size_t(-1);

    //event handlers
    BindEventHandlers();

    //rendering
    EnableSystemTheme(true);
    SetNativeRenderer();
//    SetGenericRenderer();
//    SetCustomRenderer();

    m_BestSize.Set(-1, -1);
}

//--------------------- ORIENTATION ---------------------------------//
/** Get the orientation of the control
  * \return the orientation of the control
  */
wxVirtualHeaderCtrl::EOrientation wxVirtualHeaderCtrl::GetOrientation(void) const
{
    return(m_eOrientation);
}

/** Set the orientation of the control
  * \param eOrientation [input]: the new orientation of the header
  */
void wxVirtualHeaderCtrl::SetOrientation(EOrientation eOrientation)
{
    m_eOrientation = eOrientation;
}

//-------------------- RENDERER -------------------------------------//
/** Set the renderer for this header
  * \param pRenderer [input]: the new renderer. Ownership is taken.
  *                           It must have been allocated with "new"
  */
void wxVirtualHeaderCtrl::SetRenderer(wxVirtualHeaderIRenderer *pRenderer)
{
    if ((m_pRenderer == pRenderer)) return;
    if (m_pRenderer) delete(m_pRenderer);
    m_pRenderer = pRenderer;
    ComputeMinSize();
}

/** Get the renderer
  * \return the renderer.
  *         Do not delete it: it is still owned by the wxVirtualHeaderCtrl object
  */
wxVirtualHeaderIRenderer* wxVirtualHeaderCtrl::GetRenderer(void)
{
    return(m_pRenderer);
}

//------------ RENDERER : CONVENIENCE METHODS -----------------------//
/** Use a native implementation of wxVirtualHeaderIRenderer
  */
void wxVirtualHeaderCtrl::SetNativeRenderer(void)
{
    SetRenderer(new wxVirtualHeaderRendererNative(this));
}

/** Use wxVirtualHeaderRendererGeneric as renderer
  */
void wxVirtualHeaderCtrl::SetGenericRenderer(void)
{
    SetRenderer(new wxVirtualHeaderRendererGeneric(this));
}

/** Use wxVirtualHeaderRendererCustom as renderer
  */
void wxVirtualHeaderCtrl::SetCustomRenderer(void)
{
    SetRenderer(new wxVirtualHeaderRendererCustom(this));
}

//----------------------- SIZING ------------------------------------//
/** Get best size for the control
  * Best size = show all items
  */
wxSize wxVirtualHeaderCtrl::DoGetBestSize(void) const
{
    //check cached size
    if (  (m_BestSize.GetWidth() >= 0)
        &&(m_BestSize.GetHeight() >= 0)
       )
    {
        return(m_BestSize);
    }

    //compute
    size_t uiCount = GetItemCount();
    int iHeight = 0;
    int iWidth  = 0;
    if (HasFlag(WX_VDV_PERFECT_SIZE_CALCULATION))
    {
        //compute
        size_t i;
        wxSize s;

        if (m_eOrientation == WX_VDV_COL_HEADER)
        {
            //column header
            for(i=0;i<uiCount;i++)
            {
                if (!IsItemShown(i)) continue;
                s = GetItemSize(i);
                if (s.GetHeight() > iHeight) iHeight = s.GetHeight();
                iWidth += s.GetWidth();

                //filter
                if (!m_bShowFilters) continue;
                wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(i);
                if (!pFilterEditor) continue;

                wxVirtualHeaderCtrl *pWin = const_cast<wxVirtualHeaderCtrl*>(this);
                wxSize sFilterSize = pFilterEditor->GetSize(pWin);
                int iTotalHeight = s.GetHeight() + sFilterSize.GetHeight();
                if (iTotalHeight > iHeight) iHeight = iTotalHeight;
            }
        }
        else
        {
            //row header
            for(i=0;i<uiCount;i++)
            {
                if (!IsItemShown(i)) continue;
                s = GetItemSize(i);
                if (s.GetWidth() > iWidth) iWidth = s.GetWidth();
                iHeight += s.GetHeight();

                //filter
                if (!m_bShowFilters) continue;
                wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(i);
                if (!pFilterEditor) continue;

                wxVirtualHeaderCtrl *pWin = const_cast<wxVirtualHeaderCtrl*>(this);
                wxSize sFilterSize = pFilterEditor->GetSize(pWin);
                int iTotalWidth = s.GetWidth() + sFilterSize.GetWidth();
                if (iTotalWidth > iWidth) iWidth = iTotalWidth;
            }
        }
    }
    else
    {
        if (m_eOrientation == WX_VDV_COL_HEADER)
        {
            iWidth  = uiCount * m_MinSize.GetWidth();
            iHeight = m_MinSize.GetHeight();

            //filter
            if (m_bShowFilters)
            {
                wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(0);
                if (pFilterEditor)
                {
                    wxVirtualHeaderCtrl *pWin = const_cast<wxVirtualHeaderCtrl*>(this);
                    wxSize sFilterSize = pFilterEditor->GetSize(pWin);
                    iHeight += sFilterSize.GetHeight();
                }

            }
        }
        else
        {
            iWidth  = m_MinSize.GetWidth();
            iHeight = uiCount * m_MinSize.GetHeight();

            //filter
            if (m_bShowFilters)
            {
                wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(0);
                if (pFilterEditor)
                {
                    wxVirtualHeaderCtrl *pWin = const_cast<wxVirtualHeaderCtrl*>(this);
                    wxSize sFilterSize = pFilterEditor->GetSize(pWin);
                    iWidth += sFilterSize.GetWidth();
                }
            }
        }
    }

    //at least min size
    if (iWidth < m_MinSize.GetWidth()) iWidth = m_MinSize.GetWidth();
    if (iHeight < m_MinSize.GetHeight()) iHeight = m_MinSize.GetHeight();

    //cache & return
    wxSize &rSize = const_cast<wxSize &>(m_BestSize);
    rSize.Set(iWidth, iHeight);
    return(m_BestSize);
}

/** Compute minimal size
  */
void wxVirtualHeaderCtrl::ComputeMinSize(void)
{
    if (m_pRenderer)
    {
        wxVirtualHeaderIRenderer::THeaderData rData;
        rData.m_sLabel = "AgMf0";
        m_MinSize = m_pRenderer->GetHeaderButtonSize(rData);
    }
    else
    {
        m_MinSize.Set(20, 20);
    }
}

//---------------------------- CALLBACKS ----------------------------//
/** Notify that header items have changed
  */
void wxVirtualHeaderCtrl::OnItemsChanged(void)
{
    DoInvalidateBestSize();
}

//------------------------ EVENT HANDLERS ---------------------------//
/** Bind the event handlers
  */
void wxVirtualHeaderCtrl::BindEventHandlers(void)
{
    //draw events
    Bind(wxEVT_PAINT           , &wxVirtualHeaderCtrl::OnPaint         , this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxVirtualHeaderCtrl::OnDrawBackground, this);

    //mouse events
    Bind(wxEVT_LEFT_DOWN       , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_LEFT_UP         , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_LEFT_DCLICK     , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_MIDDLE_DOWN     , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_MIDDLE_UP       , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_MIDDLE_DCLICK   , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_RIGHT_DOWN      , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_RIGHT_UP        , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_RIGHT_DCLICK    , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_AUX1_DOWN       , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_AUX1_UP         , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_AUX1_DCLICK     , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_AUX2_DOWN       , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_AUX2_UP         , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_AUX2_DCLICK     , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_MOTION          , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_ENTER_WINDOW    , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_LEAVE_WINDOW    , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_MOUSEWHEEL      , &wxVirtualHeaderCtrl::OnMouse         , this);
    Bind(wxEVT_MAGNIFY         , &wxVirtualHeaderCtrl::OnMouse         , this);

    Bind(wxEVT_VIRTUAL_HEADER_RIGHT_UP, &wxVirtualHeaderCtrl::OnRightClick, this);

    //focus
    Bind(wxEVT_KILL_FOCUS     , &wxVirtualHeaderCtrl::OnLoseFocus      , this);

    //filter event - notifications sent by wxVirtualDataViewFilter, with
    //the parent window as object (parent window = this header control window)
    Bind(wxEVT_VDV_FILTER_EDITION_BEGIN  , &wxVirtualHeaderCtrl::OnFilterBegin  , this);
    Bind(wxEVT_VDV_FILTER_EDITION_CHANGED, &wxVirtualHeaderCtrl::OnFilterChanged, this);
    Bind(wxEVT_VDV_FILTER_EDITION_END    , &wxVirtualHeaderCtrl::OnFilterEnded  , this);

    //for theming on MSW
    Bind(wxEVT_SYS_COLOUR_CHANGED , &wxVirtualHeaderCtrl::OnSysColourChanged, this);
}

/** Erase background event handler
  */
void wxVirtualHeaderCtrl::OnDrawBackground(wxEraseEvent &rEvent)
{

}

/** Paint event handler
  */
void wxVirtualHeaderCtrl::OnPaint(wxPaintEvent &rEvent)
{
    wxAutoBufferedPaintDC dc(this);
//    wxPaintDC dc(this);
    DoPaint(dc);
}

/** Redraw the window in a specified DC
  * \param rDC [input]: the DC to use
  */
void wxVirtualHeaderCtrl::UpdateDisplay(wxDC &rDC)
{
    DoPaint(rDC);
}

/** Performs the painting
  * \param rDC [input] : the DC to use
  */
void wxVirtualHeaderCtrl::DoPaint(wxDC &rDC)
{
    if (!m_pRenderer) return;

    //get client size
    int w, h;
    GetClientSize(&w, &h);

    //set DC attributes for non-native painting
    m_pRenderer->PrepareDC(rDC);

    //scrolling offset
    wxCoord iOffset = GetFirstItemOrigin();

    //editor : put it in the correct position
    wxWindow *pWinEditor = WX_VDV_NULL_PTR;
    if (m_pActiveFilterEditor) pWinEditor = m_pActiveFilterEditor->GetEditorWindow();

    //paint labels
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        //paint all visible labels
        wxCoord x0 = iOffset;

        size_t uiCol = GetFirstVisibleItem();
        while ((uiCol != s_uiNotFound) && (x0 < w))
        {
            wxSize s = GetItemSize(uiCol);
            if (s.GetWidth() > 0)
            {
                //column is shown
                wxRect r(x0, 0, s.GetWidth(), s.GetHeight());
                PaintHeader(rDC, r, uiCol);

                //filter
                if (m_bShowFilters)
                {
                    bool bIsShownAndActive = (pWinEditor) && (uiCol == m_uiActiveFilteredItem);
                    wxRect rFilter(x0, s.GetHeight(), s.GetWidth(), h - s.GetHeight());
                    PaintBackground(rDC, rFilter);

                    wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(uiCol);
                    if ((pFilterEditor) && (!bIsShownAndActive))
                    {
                        wxSize sFilterSizer = pFilterEditor->GetSize(this);
                        wxRect rf(x0, s.GetHeight(), s.GetWidth(), sFilterSizer.GetHeight());
                        pFilterEditor->Draw(this, rDC, rf);
                    }

                    if (bIsShownAndActive)
                    {
                        pWinEditor->Move(x0, s.GetHeight());
                    }
                }


                x0 += s.GetWidth();
            }
            //else column is hidden

            uiCol = GetNextVisibleItem(uiCol);
        }

        //paint remaining data
        if (x0 < w)
        {
            wxRect r(x0, 0, w - x0, h);
            PaintBackground(rDC, r);
        }
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        //paint all visible labels
        wxCoord y0 = iOffset;

        size_t uiRow = GetFirstVisibleItem();
        while ((uiRow != s_uiNotFound) && (y0 < h))
        {
            wxSize s = GetItemSize(uiRow);
            if (s.GetHeight() >= 0)
            {
                //row is shown
                wxRect r(0, y0, s.GetWidth(), s.GetHeight());
                PaintHeader(rDC, r, uiRow);

                //filter
                if (m_bShowFilters)
                {
                    bool bIsShownAndActive = (pWinEditor) && (uiRow == m_uiActiveFilteredItem);

                    wxRect rFilter(s.GetWidth(), y0, w - s.GetWidth(), s.GetHeight());
                    PaintBackground(rDC, rFilter);

                    wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(uiRow);
                    if ((pFilterEditor) && (!bIsShownAndActive))
                    {
                        wxSize sFilterSizer = pFilterEditor->GetSize(this);
                        wxRect rf(s.GetWidth(), y0, sFilterSizer.GetWidth(), s.GetHeight());
                        pFilterEditor->Draw(this, rDC, rf);
                    }

                    if (bIsShownAndActive)
                    {
                        pWinEditor->Move(s.GetWidth(), y0);
                    }
                }


                y0 += s.GetHeight();
            }
            //else row is hidden

            uiRow = GetNextVisibleItem(uiRow);
        }

        //paint remaining data
        if (y0 < h)
        {
            wxRect r(0, y0, w, h - y0);
            PaintBackground(rDC, r);
        }
    }



    //dragging
    if (m_bIsDragging) PaintDragging(rDC);
}

/** Paint one button
  * \param rDC   [input]: the device context to use
  * \param rRect [input]: the clipping region
  * \param uiViewCoord [input]: the view coordinate of the row/column
  */
void wxVirtualHeaderCtrl::PaintHeader(wxDC &rDC, wxRect &rRect, size_t uiViewCoord)
{
    TItemData rData;
    rData.m_bIsVisible = true;

    //fill mouse tracking & pushed state
    //Can be overridden later by model
    if (m_bIsDragging)
    {
        //dragging
        if (uiViewCoord == m_uiDraggedItemID)
        {
            rData.m_bIsCurrent    = true;
            rData.m_bIsPushed     = true;
            rData.m_bIsHotTracked = false;
        }
        else if (uiViewCoord == m_uiTargetDropItemID)
        {
            rData.m_bIsCurrent    = false;
            rData.m_bIsPushed     = false;
            rData.m_bIsHotTracked = true;
        }
        else
        {
            rData.m_bIsCurrent    = false;
            rData.m_bIsPushed     = false;
            rData.m_bIsHotTracked = false;
        }
    }
    else
    {
        //not dragging
        if (uiViewCoord == m_uiCurrentMouseSpot)
        {
            rData.m_bIsHotTracked = true;
            if (m_bPushingButton) rData.m_bIsPushed = true;
            else                  rData.m_bIsPushed = false;
        }
        else
        {
            rData.m_bIsHotTracked = false;
            rData.m_bIsPushed     = false;
        }

        //fill current selection
        if (uiViewCoord == m_uiCurrentSelection)
        {
            rData.m_bIsCurrent = true;
        }
        else
        {
            rData.m_bIsCurrent = false;
        }
    }

    //get header data from model (may override some values - by design)
    GetItemData(uiViewCoord, rData);

    //draw
    m_pRenderer->DrawHeader(rDC, rRect, (const wxVirtualHeaderIRenderer::THeaderData&) rData);
}

/** Only paint background
  * \param rDC   [input]: the device context to use
  * \param rRect [input]: the clipping region
  */
void wxVirtualHeaderCtrl::PaintBackground(wxDC &rDC, wxRect &rRect)
{
    m_pRenderer->DrawHeaderBackground(rDC, rRect);
}

/** Paint dragged item
  * \param rDC [input]: the device context to use
  */
void wxVirtualHeaderCtrl::PaintDragging(wxDC &rDC)
{
    //draw a marker after the target element
    if (m_uiTargetDropItemID == s_uiNotFound) return;
    if (!m_pDragImage) return;

    wxRect rRect = GetItemRect(m_uiTargetDropItemID, true);
    if (rRect.GetWidth() <= 0)  return;
    if (rRect.GetHeight() <= 0) return;

    //wxRendererNative::Get().DrawFocusRect(this, rDC, rRect, wxCONTROL_SELECTED);
    wxPen oPen(*wxBLUE, 2, wxPENSTYLE_SOLID);
    wxDCPenChanger dcPen(rDC, oPen);

    wxCoord x0 = rRect.x;
    wxCoord y0 = rRect.y;
    wxCoord x1, y1;
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        x1 = x0;
        y1 = y0 + rRect.height;
    }
    else
    {
        x1 = x0 + rRect.width;
        y1 = y0;
    }

    rDC.DrawLine(x0, y0, x1, y1);
}

////---------------------------- MOUSE EVENTS -------------------------//
/** Handle all kinds of mouse event
  */
void wxVirtualHeaderCtrl::OnMouse(wxMouseEvent &rEvent)
{
    wxEventType eType = rEvent.GetEventType();

    int x = rEvent.GetX();
    int y = rEvent.GetY();

    ESeparator eSeparator = WX_VDV_NO_SEPARATOR;
    bool bHasCapture  = HasCapture();
    bool bOnFilter    = false;

    size_t uiOldMouseSpot = m_uiCurrentMouseSpot;
    if ((rEvent.Dragging()) && (rEvent.LeftIsDown()))
    {
        bool bProcessed =  false;
        if (m_bIsDragging) bProcessed = DragItem(x, y);
        else               bProcessed = StartDragging(x, y);

//        Refresh(false, WX_VDV_NULL_PTR);
//        Update();

        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if ((eType == wxEVT_MOTION) || (eType == wxEVT_ENTER_WINDOW))
    {
        //mouse is moving inside the window - find the header below it
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;

        //update mouse cursor
        ApplyMouseCursor(eSeparator);

        if (uiOldMouseSpot != m_uiCurrentMouseSpot)
        {
            bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_CHANGE, m_uiCurrentMouseSpot);
            Refresh(false, WX_VDV_NULL_PTR);
            Update();
            if (!bProcessed) rEvent.Skip(false);
        }
        return;
    }
    else if (eType == wxEVT_LEAVE_WINDOW)
    {
        m_bPushingButton     = false;
        m_uiCurrentMouseSpot = s_uiNotFound;

        if (uiOldMouseSpot != m_uiCurrentMouseSpot)
        {
            bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_CHANGE, size_t(-1));
            Refresh(false, WX_VDV_NULL_PTR);
            Update();
            if (!bProcessed) rEvent.Skip(false);
        }
        return;
    }
    else if (eType == wxEVT_LEFT_DOWN)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) {StartFilter(m_uiCurrentMouseSpot); return;}
        EndFilter();

        m_bPushingButton = true;

        ApplyMouseCursor(eSeparator);

        //bool bProcessed =  StartDragging(x, y);
        bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_LEFT_DOWN, m_uiCurrentMouseSpot);

        Refresh(false, WX_VDV_NULL_PTR);
        Update();

        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_LEFT_UP)
    {
        m_bPushingButton     = false;
        m_uiCurrentSelection = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = StopDragging(x, y, m_uiCurrentSelection, false);

        Refresh(false, WX_VDV_NULL_PTR);
        Update();

        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_MIDDLE_DOWN)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_MIDDLE_DOWN, m_uiCurrentMouseSpot);
        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_MIDDLE_UP)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_MIDDLE_UP, m_uiCurrentMouseSpot);
        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_RIGHT_DOWN)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_RIGHT_DOWN, m_uiCurrentMouseSpot);
        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_RIGHT_UP)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_RIGHT_UP, m_uiCurrentMouseSpot);
        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_LEFT_DCLICK)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = false;
        if (eSeparator == WX_VDV_NO_SEPARATOR)
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_LEFT_DCLICK, m_uiCurrentMouseSpot);
        }
        else
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_SEPARATOR_LEFT_DCLICK, m_uiCurrentMouseSpot);
        }

        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_MIDDLE_DCLICK)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = false;
        if (eSeparator == WX_VDV_NO_SEPARATOR)
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_MIDDLE_DCLICK, m_uiCurrentMouseSpot);
        }
        else
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_SEPARATOR_MIDDLE_DCLICK, m_uiCurrentMouseSpot);
        }

        if (!bProcessed) rEvent.Skip(false);
        return;
    }
    else if (eType == wxEVT_RIGHT_DCLICK)
    {
        m_uiCurrentMouseSpot = HitTest(x, y, eSeparator, bOnFilter, bHasCapture);
        if (bOnFilter) return;
        ApplyMouseCursor(eSeparator);

        bool bProcessed = false;
        if (eSeparator == WX_VDV_NO_SEPARATOR)
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_RIGHT_DCLICK, m_uiCurrentMouseSpot);
        }
        else
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_SEPARATOR_RIGHT_DCLICK, m_uiCurrentMouseSpot);
        }

        if (!bProcessed) rEvent.Skip(false);
        return;
    }

    //default behaviour : skip the event
    rEvent.Skip();
}

/** Mouse capture lost event handler
  */
void wxVirtualHeaderCtrl::OnCaptureLost(wxMouseCaptureLostEvent &rEvent)
{
    CancelDragging(0, 0, false);
}

/** Right click event
  * Default implementation shows the popup menu
  * \param rEvent [input]: the event information
  */
void wxVirtualHeaderCtrl::OnRightClick(wxVirtualHeaderCtrlEvent &rEvent)
{
    if (   (!HasFlag(WX_VDV_ALLOW_HIDING))
        && (!HasFlag(WX_VDV_ALLOW_FILTERING))
        && (!HasFlag(WX_VDV_ALLOW_REORDER))
       )
    {
        rEvent.Skip();
        return;
    }

    ShowItemsMenu(ScreenToClient(wxGetMousePosition()), wxEmptyString);
}

/** Lose focus event
  */
void wxVirtualHeaderCtrl::OnLoseFocus(wxFocusEvent &rEvent)
{

}

//------------------------- HIT TESTING -----------------------------//
/** Set the mouse cursor
  * \param eSeparator [input]: on which separator the mouse cursor is currently
  */
WX_VDV_INLINE void wxVirtualHeaderCtrl::ApplyMouseCursor(ESeparator eSeparator)
{
    if ((eSeparator != WX_VDV_NO_SEPARATOR) && (HasFlag(WX_VDV_ALLOW_RESIZE)))
    {
        if (m_eOrientation == WX_VDV_COL_HEADER) SetCursor(wxCURSOR_SIZEWE);
        else                                     SetCursor(wxCURSOR_SIZENS);
    }
    else
    {
        SetCursor(wxNullCursor);
    }
}

/** Set the mouse cursor
  * \param x, y [input]: the mouse coordinates
  */
WX_VDV_INLINE void wxVirtualHeaderCtrl::ApplyMouseCursor(int x, int y)
{
    ESeparator eSeparator = WX_VDV_NO_SEPARATOR;
    bool bOnFilter = false;
    DoHitTest(x, y, eSeparator, bOnFilter, HasCapture());
    ApplyMouseCursor(eSeparator);
}

/** Get item below the client coordinate x, y
  * \param x               [input] : the X client coordinate
  * \param y               [input] : the Y client coordinate
  * \param w               [input] : the width of the client area
  * \param h               [input] : the height of the client area
  * \param eSeparator      [output]: the separator hit, if any
  * \param bOnFilter       [output]: true if the filter is hit
  * \param bOutOfBoundsHit [input] : if true, when x/y are out of bounds,
  *                                  the hit test return the closest item
  * \return the index of the item, if found
  *         s_uiNotFound if not found
  */
WX_VDV_INLINE size_t wxVirtualHeaderCtrl::DoHitTest(int x, int y, int w, int h,
                                                    ESeparator &eSeparator,
                                                    bool &bOnFilter,
                                                    bool bOutOfBoundsHit) const
{
    //easy cases : the coord are not in the client area
    //return -1 if mouse is not captured, or 1st / last visible item if captured
    eSeparator = WX_VDV_NO_SEPARATOR;
    bOnFilter  = false;
    if (bOutOfBoundsHit)
    {
        if (m_eOrientation == WX_VDV_COL_HEADER)
        {
            if (x < 0)  return(GetFirstVisibleItem());
            if (x >= w) return(GetLastVisibleItem());
        }
        else
        {
            if (y < 0)  return(GetFirstVisibleItem());
            if (y >= h) return(GetLastVisibleItem());
        }
    }
    else
    {
        //mouse not captured: return(-1) if cursor outside the window
        //(does it ever happen ? Should not, but use defensive programming !)
        if (x < 0) return(s_uiNotFound);
        if (y < 0) return(s_uiNotFound);

        if (x > w) return(s_uiNotFound);
        if (y > h) return(s_uiNotFound);
    }

    //convert unsigned to signed, to avoid compiler warning
    wxCoord iBorderSize = m_uiResizeBorderSize;

    //scan the items
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        //scan all visible labels
        wxCoord x0 = GetFirstItemOrigin();
        wxCoord iWidth = 0;

        size_t uiCol = GetFirstVisibleItem();
        size_t uiLast = uiCol;
        size_t uiFirst = uiCol;
        while (uiCol != s_uiNotFound)
        {
            wxSize s = GetItemSize(uiCol);

            iWidth = s.GetWidth();
            if ((iWidth > 0) && (x < x0 + iWidth))
            {
                //Inside item. Check if we are on the separator or not
                if (x < x0 + iBorderSize)
                {
                    //on left separator. return cell on the left of the separator
                    eSeparator = WX_VDV_LEFT_SEPARATOR;

                }
                else if (x > x0 + iWidth - iBorderSize)
                {
                    //on right separator
                    eSeparator = WX_VDV_RIGHT_SEPARATOR;
                }

                //check if we are on filter
                if (m_bShowFilters)
                {
                    if (y >= s.GetHeight()) bOnFilter = true;
                }

                return(uiCol);
            }

            x0 += iWidth;

            uiLast = uiCol;
            uiCol = GetNextVisibleItem(uiCol);
        }

        //special cases : left separator of 1st column : return 1st column
        //                right separator of last column : return last column
        if (x < x0 + iBorderSize)
        {
            //right separator of last column
            eSeparator = WX_VDV_RIGHT_SEPARATOR;
            return(uiLast);
        }
        x0 = GetFirstItemOrigin();
        if ((x > x0 - iBorderSize) || (x <= x0))
        {
            //left separator of 1st column
            eSeparator = WX_VDV_LEFT_SEPARATOR;
            return(uiFirst);
        }
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        //scan all visible labels
        wxCoord y0      = GetFirstItemOrigin();
        wxCoord iHeight = 0;

        size_t uiRow = GetFirstVisibleItem();
        size_t uiLast = uiRow;
        size_t uiFirst = uiRow;
        while (uiRow != s_uiNotFound)
        {
            wxSize s = GetItemSize(uiRow);

            iHeight = s.GetHeight();
            if ((iHeight > 0) && (y < y0 + iHeight))
            {
                //Inside item. Check if we are on the separator or not
                if (y < y0 + iBorderSize)
                {
                    //on top separator. return cell above the separator
                    eSeparator = WX_VDV_TOP_SEPARATOR;
                }
                else if (y > y0 + iHeight - iBorderSize)
                {
                    //on bottom separator
                    eSeparator = WX_VDV_BOTTOM_SEPARATOR;
                }

                //check if we are on filter
                if (m_bShowFilters)
                {
                    if (x >= s.GetWidth()) bOnFilter = true;
                }

                return(uiRow);
            }
            y0 += iHeight;

            uiRow = GetNextVisibleItem(uiRow);
        }

        //special cases : top separator of 1st row : return 1st row
        //                bottom separator of last row : return last row
        if (y < y0 + iBorderSize)
        {
            //bottom separator of last row
            eSeparator = WX_VDV_BOTTOM_SEPARATOR;
            return(uiLast);
        }
        y0 = GetFirstItemOrigin();
        if ((y > y0 - iBorderSize) || (y <= y0))
        {
            //top separator of 1st row
            eSeparator = WX_VDV_TOP_SEPARATOR;
            return(uiFirst);
        }
    }

    //not found
    return(s_uiNotFound);
}

/** Get item below the client coordinate x, y
  * \param x               [input] : the X client coordinate
  * \param y               [input] : the Y client coordinate
  * \param eSeparator      [output]: the separator hit, if any
  * \param bOnFilter       [output]: true if the filter is hit
  * \param bOutOfBoundsHit [input] : if true, when x/y are out of bounds,
  *                                  the hit test return the closest item
  * \return the index of the item, if found
  *         s_uiNotFound if not found
  */
WX_VDV_INLINE size_t wxVirtualHeaderCtrl::DoHitTest(int x, int y, ESeparator &eSeparator,
                                                    bool &bOnFilter,
                                                    bool bOutOfBoundsHit) const
{
    int w, h;
    GetClientSize(&w, &h);
    return(DoHitTest(x, y, w, h, eSeparator, bOnFilter, bOutOfBoundsHit));
}

/** Get item below the client coordinate x, y
  * \param x               [input] : the X client coordinate
  * \param y               [input] : the Y client coordinate
  * \param eSeparator      [output]: the separator hit, if any
  * \param bOnFilter       [output]: true if the filter is hit
  * \param bOutOfBoundsHit [input] : if true, when x/y are out of bounds,
  *                                  the hit test return the closest item
  * \return the index of the item, if found
  *         s_uiNotFound if not found
  */
size_t wxVirtualHeaderCtrl::HitTest(int x, int y, ESeparator &eSeparator,
                                    bool &bOnFilter,
                                    bool bOutOfBoundsHit) const
{
    return(DoHitTest(x, y, eSeparator, bOnFilter, bOutOfBoundsHit));
}

/** Get item below the client coordinate x or y
  * \param iCoord          [input] : the x client coordinate for column header
  *                                   the y client coordinate for row header
  * \param eSeparator      [output]: the separator hit, if any
  * \param bOnFilter       [output]: true if the filter is hit
  * \param bOutOfBoundsHit [input] : if true, when x/y are out of bounds,
  *                                  the hit test return the closest item
  * \return the index of the item below the coordinate, or s_uiNotFound if not found
  */
size_t wxVirtualHeaderCtrl::GetItemAtCoord(int iCoord, ESeparator &eSeparator,
                                           bool &bOnFilter,
                                           bool bOutOfBoundsHit) const
{
    int w, h;
    GetClientSize(&w, &h);

    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        return(DoHitTest(iCoord, h/2, w, h,
                         eSeparator, bOnFilter, bOutOfBoundsHit));
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        return(DoHitTest(w/2, iCoord, w, h,
                         eSeparator, bOnFilter, bOutOfBoundsHit));
    }

    eSeparator = WX_VDV_NO_SEPARATOR;
    bOnFilter = false;
    return(s_uiNotFound);
}

//--------------------- RESIZE BORDER SIZE --------------------------//
/** Get the width/height of the sizing border between items
  * \return the amount of pixels inside a cell indicating if the mouse cursor
  *         should show a sizing operation or a drag operation
  *         This amount of pixels is located both on left and right of the cell
  *         for column header (or top and bottom for row header)
  */
size_t wxVirtualHeaderCtrl::GetResizeBorderSize(void) const
{
    return(m_uiResizeBorderSize);
}

/** Set the width/height of the sizing border between items
  * \param uiSizeInPixels [input] : the amount of pixels inside a cell indicating
  *                                 if the mouse cursor should show a sizing operation
  *                                 or a drag operation. This amount of pixels is located
  *                                 both on left and right of the cell for column header
  *                                 (or top and bottom for row header)
  */
void wxVirtualHeaderCtrl::SetResizeBorderSize(size_t uiSizeInPixels)
{
    m_uiResizeBorderSize = uiSizeInPixels;
}

//----------------------- SELECTION ---------------------------------//
/** Get the current selection
  * \return the ID of the currently selected item
  */
size_t wxVirtualHeaderCtrl::GetCurrentSelection(void) const
{
    return(m_uiCurrentSelection);
}

/** Set the current selection
  * \param uiSelection [input]: the ID of the selected column
  */
void wxVirtualHeaderCtrl::SetCurrentSelection(size_t uiSelection)
{
    m_uiCurrentSelection = uiSelection;
}

//--------------------------- DRAGGING ------------------------------//
/** Start dragging an item, or resizing it
  * It also sends the needed events:
  *     wxEVT_VIRTUAL_HEADER_BEGIN_RESIZE
  *     wxEVT_VIRTUAL_HEADER_BEGIN_REORDER
  *     wxEVT_VIRTUAL_HEADER_LEFT_DOWN
  * \param uiItemID [input]: the ID of the item to drag
  * \param x [input] : the X coordinate of the mouse cursor (client coordinates)
  * \param y [input] : the Y coordinate of the mouse cursor (client coordinates)
  * \param bSendEvent [input]: if true, events are sent
  * \return true if the sent event was processed, false otherwise
  */
bool wxVirtualHeaderCtrl::StartDragging(int x, int y, bool bSendEvent)
{
    //init
    bool bProcessed = false;

    //end filtering
    if (!EndFilter()) return(false);

    //hit test
    ESeparator eSeparator = WX_VDV_NO_SEPARATOR;
    bool bOnFilter = false;
    size_t uiItemID = HitTest(x, y, eSeparator, bOnFilter, HasCapture());
    if (bOnFilter) return(StartFilter(uiItemID));

    //special case : no hit
    if (uiItemID == s_uiNotFound)
    {
        //set state
        m_bIsDragging = false;
        m_uiDraggedItemID    = uiItemID;
        m_uiTargetDropItemID = uiItemID;
        m_uiCurrentMouseSpot = uiItemID;

        if (bSendEvent) bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_LEFT_DOWN,
                                               s_uiNotFound);
        return(bProcessed);
    }

    //remove separator value if resizing not allowed
    if (!HasFlag(WX_VDV_ALLOW_RESIZE)) eSeparator = WX_VDV_NO_SEPARATOR;
    ApplyMouseCursor(eSeparator);

    //3 possibilities : dragging, resizing, nothing
    if ((HasFlag(WX_VDV_ALLOW_REORDER)) && (eSeparator == WX_VDV_NO_SEPARATOR))
    {
        //dragging
        m_bIsDragging        = true;
        m_uiDraggedItemID    = uiItemID;
        m_uiTargetDropItemID = uiItemID;
        m_uiCurrentMouseSpot = uiItemID;

        m_bPushingButton = true;

        //paint the item to drag in a bitmap
        wxRect rItemRect = GetItemRect(uiItemID);
        int iWidth = rItemRect.GetWidth();
        int iHeight = rItemRect.GetHeight();
        if (iWidth <= 0) return(false);
        if (iHeight <= 0) return(false);

        wxClientDC dc(this);
        wxBitmap bmp(iWidth, iHeight, dc);
        wxMemoryDC dcMem(bmp);
        rItemRect.x = 0; rItemRect.y = 0;
        PaintHeader(dcMem, rItemRect, uiItemID);
        dcMem.SelectObject(wxNullBitmap);
        //bmp.SaveFile("test.bmp", wxBITMAP_TYPE_BMP);

        //cursor for dragging
        m_pDragImage = new wxDragImage(bmp);
        if (!m_pDragImage) return(false);
        wxPoint ptHotSpot(0, 0);
        m_pDragImage->BeginDrag(ptHotSpot, this, true);

        //show cursor
        int iCursorX = x;
        int iCursorY = y;
        DragCoordAdjust(x, y, iCursorX, iCursorY);

        m_pDragImage->Move(wxPoint(iCursorX, iCursorY));
        m_pDragImage->Show();

        bool bAllowed = true;
        if (bSendEvent) bProcessed = SendEventAndCheck(bAllowed, wxEVT_VIRTUAL_HEADER_BEGIN_DRAGGING,
                                                       m_uiDraggedItemID, 0, m_uiTargetDropItemID);
        if ((bProcessed) && (!bAllowed)) return(CancelDragging(x, y, bSendEvent));
    }
    else if ((HasFlag(WX_VDV_ALLOW_RESIZE)) && (eSeparator != WX_VDV_NO_SEPARATOR))
    {
        //resizing
        //adjust item ID : the item to resize is located
        //on the left or on top of the separator
        if (eSeparator == WX_VDV_LEFT_SEPARATOR) //WX_VDV_TOP_SEPARATOR
        {
            if (uiItemID > 0) uiItemID--;
            else
            {
                //no resizing
                m_bIsDragging = false;
                m_uiDraggedItemID    = uiItemID;
                m_uiTargetDropItemID = uiItemID;
                m_uiCurrentMouseSpot = uiItemID;
                if (bSendEvent) bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_LEFT_DOWN, uiItemID);
                return(bProcessed);
            }
        }

        //set state
        m_bIsDragging        = true;
        m_uiDraggedItemID    = uiItemID;
        m_uiTargetDropItemID = uiItemID;
        m_uiCurrentMouseSpot = uiItemID;

        m_bPushingButton = false; //sizing : we do not push the item
        CaptureMouse();
        if (bSendEvent)
        {
            int iNewSize = GetNewItemSize(uiItemID, x, y);
            bool bAllowed = true;
            bProcessed = SendEventAndCheck(bAllowed, wxEVT_VIRTUAL_HEADER_BEGIN_RESIZE, uiItemID, iNewSize);
            if ((bProcessed) && (!bAllowed)) return(CancelDragging(x, y, bSendEvent));
        }
    }
    else
    {
        //nothing
        m_bIsDragging = false;
        m_uiDraggedItemID    = uiItemID;
        m_uiTargetDropItemID = uiItemID;
        m_uiCurrentMouseSpot = uiItemID;
        if (bSendEvent) bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_LEFT_DOWN, uiItemID);
        return(bProcessed);
    }

    return(bProcessed);
}

/** Adjust coordinates of dragging point
  * \param x  [input] : the original x client coordinate (mouse cursor position)
  * \param y  [input] : the original y client coordinate (mouse cursor position)
  * \param cx [output]: the x position of the drag image
  * \param cy [output]: the y position of the drag image
  */
void wxVirtualHeaderCtrl::DragCoordAdjust(int x, int y, int &cx, int &cy) const
{
    //show cursor
    cx = x;
    cy = y;
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        cy = 0;
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        cx = 0;
    }
}

/** Perform the dragging operation
  * Send the following events:
  *     wxEVT_VIRTUAL_HEADER_DRAGGING
  *     wxEVT_VIRTUAL_HEADER_RESIZING
  * \param x [input] : the X coordinate of the mouse cursor (client coordinates)
  * \param y [input] : the Y coordinate of the mouse cursor (client coordinates)
  * \param bSendEvent [input]: if true, events are sent
  * \return true if the sent event was processed, false otherwise
  */
bool wxVirtualHeaderCtrl::DragItem(int x, int y, bool bSendEvent)
{
    if (m_pDragImage)
    {
        //compute item under mouse cursor
        size_t uiOldMouseSpot = m_uiTargetDropItemID;
        ESeparator eSeparator = WX_VDV_NO_SEPARATOR;
        bool bOnFilter = false;
        m_uiTargetDropItemID = HitTest(x, y, eSeparator, bOnFilter, HasCapture());

        //update
        if (uiOldMouseSpot != m_uiTargetDropItemID)
        {
            Refresh(false, WX_VDV_NULL_PTR);
            Update();
        }

        //show cursor
        int iCursorX = x;
        int iCursorY = y;
        DragCoordAdjust(x, y, iCursorX, iCursorY);

        //this is a real dragging event
//        m_pDragImage->Hide();

        Refresh();
        Update();

        m_pDragImage->Move(wxPoint(iCursorX, iCursorY));
//        m_pDragImage->Show();

        if (bSendEvent)
        {
            bool bAllowed = true;
            bool bProcessed = SendEventAndCheck(bAllowed, wxEVT_VIRTUAL_HEADER_DRAGGING,
                                                m_uiDraggedItemID, 0, m_uiTargetDropItemID);
            if ((bProcessed) && (!bAllowed)) return(CancelDragging(x, y, bSendEvent));
            return(bProcessed);
        }
    }
    else
    {
        //this is a resizing event
        if (bSendEvent)
        {
            int iNewSize = GetNewItemSize(m_uiDraggedItemID, x, y);
            bool bAllowed = true;
            bool bProcessed = SendEventAndCheck(bAllowed,
                                                wxEVT_VIRTUAL_HEADER_RESIZING,
                                                m_uiDraggedItemID, iNewSize);
            if ((bProcessed) && (!bAllowed)) return(CancelDragging(x, y, bSendEvent));
            return(bProcessed);
        }
    }
    return(false);
}

/** Stop dragging an item
  * It also sends the following events:
  *     wxEVT_VIRTUAL_HEADER_END_DRAGGING: to end a dragging operation
  *     wxEVT_VIRTUAL_HEADER_END_RESIZE : to end a resizing operation
  *     wxEVT_VIRTUAL_HEADER_LEFT_UP
  * \param x [input] : the X coordinate of the mouse cursor (client coordinates)
  * \param y [input] : the Y coordinate of the mouse cursor (client coordinates)
  * \param uiItemID [input]: the ID of the item where the dragging is finishing
  * \param bSendEvent [input]: if true, events are sent
  * \return true if the sent event was processed, false otherwise
  */
bool wxVirtualHeaderCtrl::StopDragging(int x, int y, size_t uiItemID, bool bCancel, bool bSendEvent)
{
    //reset state
    bool bProcessed = false;
    m_uiTargetDropItemID = s_uiNotFound;
    m_uiCurrentMouseSpot = uiItemID;
    m_bPushingButton     = false;

    if (m_pDragImage)
    {
        //stop dragging operation
        m_pDragImage->EndDrag();
        delete(m_pDragImage);
        m_pDragImage = WX_VDV_NULL_PTR;

        if (!m_bIsDragging) return(false);
        m_bIsDragging    = false;
        m_bPushingButton = false;

        if (uiItemID == m_uiDraggedItemID) return(false); //nothing to do

        //send event
        wxEventType eType = wxEVT_VIRTUAL_HEADER_END_DRAGGING;
        if (bCancel) eType = wxEVT_VIRTUAL_HEADER_DRAGGING_CANCELLED;
        if (bSendEvent) bProcessed = SendEvent(eType, m_uiDraggedItemID, 0, uiItemID);
        m_uiDraggedItemID    = s_uiNotFound;
    }
    else if (m_bIsDragging)
    {
        //stop resizing operation
        ReleaseMouse();
        int iNewSize = GetNewItemSize(m_uiDraggedItemID, x, y);
        wxEventType eType = wxEVT_VIRTUAL_HEADER_END_RESIZE;
        if (bCancel) eType = wxEVT_VIRTUAL_HEADER_RESIZING_CANCELLED;
        if (bSendEvent) bProcessed = SendEvent(eType, m_uiDraggedItemID, iNewSize);
        m_uiDraggedItemID    = s_uiNotFound;
        m_bIsDragging = false;
    }
    else
    {
        //simple left up operation
        if (bSendEvent)
        {
            bProcessed = SendEvent(wxEVT_VIRTUAL_HEADER_LEFT_UP, m_uiCurrentMouseSpot);
        }
    }

    return(bProcessed);
}

/** Compute the new item size
  * \param uiItemID [input]: the ID of the item to compute
  * \param x [input] : the X coordinate of the mouse cursor (client coordinates)
  * \param y [input] : the Y coordinate of the mouse cursor (client coordinates)
  * \return the new width or height of the item
  */
int wxVirtualHeaderCtrl::GetNewItemSize(size_t uiItemID, int x, int y) const
{
    wxRect rItemRect = GetItemRect(uiItemID, true);
    if (m_eOrientation == WX_VDV_COL_HEADER) return(x - rItemRect.x);
    return(y- rItemRect.y);
}

/** Cancel dragging
  * It also sends the following event:
  *     wxEVT_VIRTUAL_HEADER_DRAGGING_CANCELLED
  * \param bSendEvent [input]: if true, events are sent
  * \return true if the sent event was processed, false otherwise
  */
bool wxVirtualHeaderCtrl::CancelDragging(int x, int y, bool bSendEvent)
{
    return(StopDragging(x, y, true, bSendEvent));
}

//--------------------- INTERFACE WITH DEFAULT IMPLEMENTATION -------//
/** Get the last visible item
  * \return the ID of the last visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualHeaderCtrl::GetLastVisibleItem(void) const
{
    size_t uiResult = GetFirstVisibleItem();
    size_t uiNext   = uiResult;
    while(uiNext != s_uiNotFound)
    {
        uiResult = uiNext;
        uiNext = GetNextVisibleItem(uiResult);
    }
    return(uiResult);
}

/** Get the bounding rectangle of one item
  * \param uiItemID     [input]: the ID of the item to query
  * \param bVisibleOnly [input]: if true, only the visible items are scanned (fast)
  *                              if false, all items are scanned (slow)
  * \return the bounding rectangle of the item if found
  *         a rectangle with width = height = 0 if not found or not visible
  */
wxRect wxVirtualHeaderCtrl::GetItemRect(size_t uiItemID, bool bVisibleOnly) const
{
    int w, h;
    GetClientSize(&w, &h);

    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        wxCoord x0 = GetFirstItemOrigin();
        if (bVisibleOnly)
        {
            //scan all visible labels
            size_t uiCol = GetFirstVisibleItem();
            while (uiCol != s_uiNotFound)
            {
                wxSize s = GetItemSize(uiCol);
                if (uiCol == uiItemID)
                {
                    //found
                    return(wxRect(x0, 0, s.GetWidth(), h));
                }

                x0 += s.GetWidth();
                uiCol = GetNextVisibleItem(uiCol);
            }
        }
        else
        {
            //scan all items
            size_t uiCol, uiCount;
            uiCount = GetItemCount();
            for(uiCol = 0; uiCol < uiCount; uiCol++)
            {
                wxSize s = GetItemSize(uiCol);
                if (uiCol == uiItemID)
                {
                    //found
                    return(wxRect(x0, 0, s.GetWidth(), h));
                }

                x0 += s.GetWidth();
            }
        }

    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        wxCoord y0 = GetFirstItemOrigin();

        if (bVisibleOnly)
        {
            //scan all visible labels
            size_t uiRow = GetFirstVisibleItem();
            while (uiRow != s_uiNotFound)
            {
                wxSize s = GetItemSize(uiRow);
                if (uiRow == uiItemID)
                {
                    //found
                    return(wxRect(0, y0, w, s.GetHeight()));
                }

                y0 += s.GetHeight();
                uiRow = GetNextVisibleItem(uiRow);
            }
        }
        else
        {
            //scan all items
            size_t uiRow, uiCount;
            uiCount = GetItemCount();
            for(uiRow = 0; uiRow < uiCount; uiRow++)
            {
                wxSize s = GetItemSize(uiRow);
                if (uiRow == uiItemID)
                {
                    //found
                    return(wxRect(0, y0, w, s.GetHeight()));
                }

                y0 += s.GetHeight();
            }
        }
    }

    //not found
    return(wxRect(0, 0, 0, 0));
}

/** Check if the control has some rows / columns
  * \return true if the control has some items, false otherwise
  */
bool wxVirtualHeaderCtrl::IsEmpty(void) const
{
    if (GetItemCount() == 0) return(true);
    return(false);
}

//------------------------ UI FOR UPDATING ORDER / VISIBILITY -------//
/** Show rearrange dialog
  * \return true if the reordering / visibility update has been successful, false otherwise
  * This method show the modal dialog box.
  * If the user click "OK", then it calls in turn UpdateOrder() and UpdateVisibility()
  */
bool wxVirtualHeaderCtrl::ShowCustomizeDialog(void)
{
    //fill arrays
    size_t i, uiCount;
    wxArrayInt    vOrder;
    wxArrayString vItemNames;
    TItemData tData;

    uiCount = GetItemCount();
    if (uiCount == 0) return(false);
    vOrder.reserve(uiCount);
    vItemNames.reserve(uiCount);
    for(i=0;i<uiCount;i++)
    {
        if (!GetItemData(i, tData)) continue;
        if (tData.m_bIsVisible) vOrder.push_back(i);
        else                    vOrder.push_back(-i);
        vItemNames.push_back(tData.m_sLabel);
    }

    //show dialog box
    wxVirtualHeaderRearrangeDlg dlg(this, vOrder, vItemNames);
    if (dlg.ShowModal() == wxID_OK)
    {
        //user clicked OK
        //get order & visibility

        wxArrayInt vOrder2 = dlg.GetOrder();
        uiCount = vOrder2.Count();

        vOrder.clear();

        for(i = 0; i < uiCount; i++)
        {
            if (vOrder2[i] >= 0)
            {
                if (!UpdateVisibility(vOrder2[i], true)) return(false);
                vOrder.push_back(vOrder2[i]);
            }
            else
            {
                if (!UpdateVisibility(~(vOrder2[i]), false)) return(false);
                vOrder.push_back(~(vOrder2[i]));
            }
        }

        if (!UpdateOrder(vOrder)) return(false);
        SendLayoutChangedEvent();
        return(true);
    }
    return(false);
}

/** Update items order
  * \param rvOrder [input]: the new item order
  * \return true if reorder was successfull, false otherwise
  * The default implementation does nothing and return true
  */
bool wxVirtualHeaderCtrl::UpdateOrder(const wxArrayInt &rvOrder)
{
    return(true);
}

/** Update item visibility
  * \param uiItemID  [input]: the item to update
  * \param bVisible  [input]: the new item visibility. True if the item is shown, false if it is hidden
  * \return true if visibility update was successfull, false otherwise
  * The default implementation does nothing and return true
  */
bool wxVirtualHeaderCtrl::UpdateVisibility(size_t uiItemID, bool bVisible)
{
    return(true);
}

/** Show the menu for columns reordering / hiding / showing
  * \param ptPoint [input]: The position of the menu, in the header window coordinates.
  * \param sTitle  [input]: The title for the menu if not empty.
  * \return true if something was changed, false otherwise
  */
bool  wxVirtualHeaderCtrl::ShowItemsMenu(const wxPoint& ptPoint, const wxString& sTitle)
{
    wxMenu oMenu;
    if (!sTitle.empty()) oMenu.SetTitle(sTitle);

    //items reordering
    if (HasFlag(WX_VDV_ALLOW_HIDING))
    {
        AddMenuItems(oMenu);
    }

    size_t uiID_Customize = oMenu.GetMenuItemCount();
    if (HasFlag(WX_VDV_ALLOW_REORDER))
    {
        oMenu.AppendSeparator();
        oMenu.Append(uiID_Customize, _("&Customize..."));
    }

    //filtering
    size_t uiID_Filtering = oMenu.GetMenuItemCount();
    if (HasFlag(WX_VDV_ALLOW_FILTERING))
    {
        oMenu.AppendSeparator();
        oMenu.Append(uiID_Filtering, _("&Show/hide filters"));
    }

    //append other items
    AppendContextualMenuItems(oMenu);

    //show the menu and get the user selection
    const int iSelectedItem = GetPopupMenuSelectionFromUser(oMenu, ptPoint);
    if (iSelectedItem == wxID_NONE) return(false); //menu cancelled

    if (ProcessMenuCommand(oMenu, iSelectedItem)) return(true);

    //filtering
    if (HasFlag(WX_VDV_ALLOW_FILTERING))
    {
        if (static_cast<size_t>(iSelectedItem) == uiID_Filtering)
        {
            ToggleFilters();
            return(true);
        }
    }

    //reordering
    if (HasFlag(WX_VDV_ALLOW_REORDER))
    {
        if (static_cast<size_t>(iSelectedItem) == uiID_Customize)
        {
            return(ShowCustomizeDialog());
        }
        else
        {
            //an item selected from the menu
            TItemData tData;
            if (!GetItemData(iSelectedItem, tData)) return(false);
            if (!UpdateVisibility(iSelectedItem, !tData.m_bIsVisible)) return(false);
            SendLayoutChangedEvent();
        }
    }

    return(true);
}

/** Append menu items to the contextual menu
  * \param rMenu [in/out] : the menu to modify
  */
void wxVirtualHeaderCtrl::AppendContextualMenuItems(wxMenu &rMenu)
{
}

/** Process a menu command
  * \param rMenu [input]: the contextual menu
  * \param iSelectedItem [input]: the selected item
  * \return true if the item was processed, false otherwise
  */
bool wxVirtualHeaderCtrl::ProcessMenuCommand(const wxMenu &rMenu, int iSelectedItem)
{
    return(false);
}

/** Check if an item is shown
  * \param uiItemID [input]: the ID of the item to check
  * \return true if the item is shown, false otherwise
  * The default implementation calls GetItemData and return TItemData::m_bIsVisible
  */
bool wxVirtualHeaderCtrl::IsItemShown(size_t uiItemID) const
{
    TItemData tData;
    if (!GetItemData(uiItemID, tData)) return(true);
    return(tData.m_bIsVisible);
}


/** Add menu items for all items
  * \param rMenu [output]: the menu to modify
  */
void wxVirtualHeaderCtrl::AddMenuItems(wxMenu &rMenu)
{
    size_t uiNbItems = GetItemCount();
    size_t i;
    TItemData tData;
    for(i = 0; i < uiNbItems; i++)
    {
        GetItemData(i, tData);
        rMenu.AppendCheckItem(i, tData.m_sLabel);
        rMenu.Check(i, tData.m_bIsVisible);
    }
}

//---------------------- FILTERING ----------------------------------//
/** Return true if filters are shown
  * \return true if filters are shown, false otherwise
  */
bool wxVirtualHeaderCtrl::AreFiltersShown(void) const
{
    return(m_bShowFilters);
}

/** Return true if filters are hidden
  * \return true if filters are hidden, false otherwise
  */
bool wxVirtualHeaderCtrl::AreFiltersHidden(void) const
{
    return(!m_bShowFilters);
}

/** Send a show/hide filter event
  * \return true if the event was processed, and Skip was not called
  *         false if the event should continue to be processed
  */
bool wxVirtualHeaderCtrl::SendFilterShowHideEvent(void)
{
    wxEventType eType;
    if (m_bShowFilters) eType = wxEVT_VIRTUAL_HEADER_SHOW_FILTERS;
    else                eType = wxEVT_VIRTUAL_HEADER_HIDE_FILTERS;

    size_t uiItemID = size_t(-1);
    return(SendEvent(eType, uiItemID));
}

/** Show / hide filters
  * \param bShow [input]: true for showing the filters, false for hiding them
  * Showing / hiding the filters will probably change the window size, so a re-layouting of the parent is
  * probably necessary. This task has to be done by the caller
  */
void wxVirtualHeaderCtrl::ShowFilters(bool bShow)
{
    if ((m_bShowFilters) && (bShow)) return;
    if ((!m_bShowFilters) && (!bShow)) return;
    m_bShowFilters = bShow;

    DoInvalidateBestSize();
    SendFilterShowHideEvent();
    SendLayoutChangedEvent();
}

/** Hide / show filters
  * \param bHide [input]: true for hiding the filters, false for showing them
  * Showing / hiding the filters will probably change the window size, so a re-layouting of the parent is
  * probably necessary. This task has to be
  */
void wxVirtualHeaderCtrl::HideFilters(bool bHide)
{
    if ((m_bShowFilters) && (!bHide)) return;
    if ((!m_bShowFilters) && (bHide)) return;
    m_bShowFilters = !bHide;

    DoInvalidateBestSize();
    SendFilterShowHideEvent();
    SendLayoutChangedEvent();
}

/** Show filters ON/OFF
  * Showing / hiding the filters will probably change the window size, so a re-layouting of the parent is
  * probably necessary. This task has to be
  */
void wxVirtualHeaderCtrl::ToggleFilters(void)
{
    m_bShowFilters = !m_bShowFilters;

    DoInvalidateBestSize();
    SendFilterShowHideEvent();
    SendLayoutChangedEvent();
}

/** Get the filter for a specific item
  * \param uiItemID [input]: the ID of the item to query
  * \return a pointer to the filter. The filter is not owned by the header control
  * The default implementation return always NULL
  */
wxVirtualDataViewIFilterEditor* wxVirtualHeaderCtrl::GetFilterEditor(size_t uiItemID) const
{
    return(WX_VDV_NULL_PTR);
}

/** Force a recalculation of best size
  */
void  wxVirtualHeaderCtrl::DoInvalidateBestSize(void)
{
    m_BestSize.Set(-1, -1);
    InvalidateBestSize();
}

/** Start edition of a filter
  * \param uiItemID [input]: the filter to activate
  * \return true if the filter was activated, false otherwise
  */
bool wxVirtualHeaderCtrl::StartFilter(size_t uiItemID)
{
    if (!m_bShowFilters) return(false);
    wxVirtualDataViewIFilterEditor *pFilterEditor = GetFilterEditor(uiItemID);
    if (!pFilterEditor) return(false);

    if (m_pActiveFilterEditor == pFilterEditor) return(true);
    if (m_pActiveFilterEditor)
    {
        if (!EndFilter()) return(false);
    }

    //compute rect for window
    wxSize sItemSize = GetItemSize(uiItemID);
    wxSize sFilterSize = pFilterEditor->GetSize(this);
    int x0 = 0, y0 = 0, w = 0, h = 0;
    size_t i;
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        x0 = GetFirstItemOrigin();
        for(i=0;i<uiItemID;i++)
        {
            x0 += GetItemSize(i).GetWidth();
        }
        y0 = sItemSize.GetHeight();

        w = sItemSize.GetWidth();
        h = sFilterSize.GetHeight();
    }
    else
    {
        x0 = sItemSize.GetWidth();
        y0 = GetFirstItemOrigin();
        for(i=0;i<uiItemID;i++)
        {
            y0 += GetItemSize(i).GetHeight();
        }

        w  = sFilterSize.GetWidth();
        h  = sItemSize.GetHeight();

    }
    wxRect rFilterRect(x0, y0, w, h);

    //start filter window
    m_pActiveFilterEditor = pFilterEditor;
    m_uiActiveFilteredItem = uiItemID;
    if (pFilterEditor->StartFilter(this, rFilterRect)) return(true);

    m_pActiveFilterEditor = WX_VDV_NULL_PTR;
    m_uiActiveFilteredItem = size_t(-1);
    return(false);
}

/** Finish edition of a filter
  * \return true if the filter was deactivated, false otherwise
  */
bool wxVirtualHeaderCtrl::EndFilter(void)
{
    if (m_pActiveFilterEditor)
    {
        if (!m_pActiveFilterEditor->CanFinishFilterEdition()) return(false);
        if (!m_pActiveFilterEditor->FinishFilterEdition()) return(false);

        m_uiActiveFilteredItem = size_t(-1);
        m_pActiveFilterEditor = WX_VDV_NULL_PTR;
        return(true);
    }
    return(true);
}

/** Filter begin event
  * \param rEvent [input]: the event information
  **/
void wxVirtualHeaderCtrl::OnFilterBegin(wxCommandEvent &rEvent)
{
    SendEvent(wxEVT_VIRTUAL_HEADER_BEGIN_FILTER_EDIT, m_uiActiveFilteredItem);
}

/** Filter changed event
  * \param rEvent [input]: the event information
  **/
void wxVirtualHeaderCtrl::OnFilterChanged(wxCommandEvent &rEvent)
{
    SendEvent(wxEVT_VIRTUAL_HEADER_FILTER_CHANGED, m_uiActiveFilteredItem);
}

/** Filter ended event
  * \param rEvent [input]: the event information
  **/
void wxVirtualHeaderCtrl::OnFilterEnded(wxCommandEvent &rEvent)
{
    SendEvent(wxEVT_VIRTUAL_HEADER_END_FILTER_EDIT, m_uiActiveFilteredItem);
    m_uiActiveFilteredItem = size_t(-1);
    m_pActiveFilterEditor = WX_VDV_NULL_PTR;
}


//------------------- EVENT HANDLER FOR THEMING ---------------------//
/** System colour changed - useful for theming
  * \param rEvent [input]: the event information
  */
void wxVirtualHeaderCtrl::OnSysColourChanged(wxSysColourChangedEvent &rEvent)
{
    if (m_pRenderer) m_pRenderer->ResetTheme();
    rEvent.Skip();
}

//------------------------- EVENT -----------------------------------//
/** Send an event
  * \param eType      [input]: the type of event to send
  * \param uiItemID   [input]: the item involved in the event
  *                          (which one is clicked, dragged, resized, ...)
  * \param iNewSize   [input]: for sizing event, the new row/column size
  * \param uiNewOrder [input]: for dragging event, the ID of the current drop target
  * \return true if the event was processed, and Skip was not called
  *         false if the event should continue to be processed
  */
bool wxVirtualHeaderCtrl::SendEvent(wxEventType eType, size_t uiItemID,
                                    int iNewSize, size_t uiNewOrder)
{
    if (eType == wxEVT_NULL) return(false);

    wxVirtualHeaderCtrlEvent event(eType, GetId());
    event.SetEventObject(this);

    event.SetItemID(uiItemID);
    event.SetSize(iNewSize);
    event.SetNewOrder(uiNewOrder);

    return(GetEventHandler()->ProcessEvent(event));
}

/** Send an event and check if it was vetoed
  * \param rbAllowed  [output]: true if the event was allowed (not vetoed), false otherwise
  * \param eType      [input]: the type of event to send
  * \param uiItemID   [input]: the item involved in the event
  *                          (which one is clicked, dragged, resized, ...)
  * \param iNewSize   [input]: for sizing event, the new row/column size
  * \param uiNewOrder [input]: for dragging event, the ID of the current drop target
  * \return true if the event was processed, and Skip was not called
  *         false if the event should continue to be processed
  */
bool wxVirtualHeaderCtrl::SendEventAndCheck(bool &rbAllowed,
                                            wxEventType eType, size_t uiItemID,
                                            int iNewSize, size_t uiNewOrder)
{
    rbAllowed = true;
    if (eType == wxEVT_NULL) return(false);

    wxVirtualHeaderCtrlEvent event(eType, GetId());
    event.SetEventObject(this);

    event.SetItemID(uiItemID);
    event.SetSize(iNewSize);
    event.SetNewOrder(uiNewOrder);

    bool bResult = GetEventHandler()->ProcessEvent(event);
    rbAllowed = event.IsAllowed();
    return(bResult);
}

/** Send an event when layout has changed (items hidden/shown, filters ON/OFF)
  * \return true if the event was processed, and Skip was not called
  *         false if the event should continue to be processed
  */
bool wxVirtualHeaderCtrl::SendLayoutChangedEvent(void)
{
    wxEventType eType = wxEVT_VIRTUAL_HEADER_LAYOUT_CHANGED;
    size_t uiItemID = size_t(-1);
    return(SendEvent(eType, uiItemID));
}

