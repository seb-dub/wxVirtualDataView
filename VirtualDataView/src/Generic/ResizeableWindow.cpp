/**********************************************************************/
/** FILE    : ResizeableWindow.cpp                                   **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxPanel which can be resized                         **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/Generic/ResizeableWindow.h>
#include <wx/dcclient.h>
#include <wx/settings.h>

//------------------------ CONSTANTS --------------------------------//
#define WX_RESIZE_BORDER_THICKNESS      5
#define WX_TOP_BORDER                   0
#define WX_BOTTOM_BORDER                1
#define WX_LEFT_BORDER                  2
#define WX_RIGHT_BORDER                 3

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//
wxIMPLEMENT_ABSTRACT_CLASS(wxResizeableWindow, wxPanel);
const char wxResizeableWindowNameStr[] = "wxResizeableWindow";

/** Default constructor. The method Create() must be called
  */
wxResizeableWindow::wxResizeableWindow(void)
    : wxPanel(),
      m_bNativeBorder(false),
      m_iResizing(WX_E_BORDER_NONE),
      m_bRespectMinimalSize(true),
      m_bForwardToParent(true)
{
}

/** Constructor
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param sName     [input]: the name of the object
  */
wxResizeableWindow::wxResizeableWindow(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                     const wxSize& size, long lStyle, wxString sName)

    : wxPanel(),
      m_bNativeBorder(false),
      m_iResizing(WX_E_BORDER_NONE),
      m_bRespectMinimalSize(true),
      m_bForwardToParent(true)
{
    CreateWin(pParent, id, pos, size, lStyle, sName);
}

/** Destructor (virtual)
  */
wxResizeableWindow::~wxResizeableWindow(void)
{

}

//----------------------- CREATION METHODS --------------------------//
/** Really creates the control
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent        [input]: parent of the window
  * \param id             [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos            [input]: initial position of the window. Default to wxDefaultPosition
  * \param size           [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle         [input]: window style. Default to 0
  * \param sName          [input]: the name of the object
  * \return true on success or false if the control couldn't be created
  */
bool wxResizeableWindow::CreateWin(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                   const wxSize& size, long lStyle, wxString sName)
{
    //create window
    if (!wxPanel::Create(pParent, id, pos, size, lStyle, sName))
    {
        return(false);
    }

    //default border sizes
    m_bNativeBorder  = false;
    m_BorderSizes[WX_TOP_BORDER   ] = WX_RESIZE_BORDER_THICKNESS;
    m_BorderSizes[WX_BOTTOM_BORDER] = WX_RESIZE_BORDER_THICKNESS;
    m_BorderSizes[WX_LEFT_BORDER  ] = WX_RESIZE_BORDER_THICKNESS;
    m_BorderSizes[WX_RIGHT_BORDER ] = WX_RESIZE_BORDER_THICKNESS;
    InvalidateBestSize();

    //binding events
    Bind(wxEVT_SIZE                 , &wxResizeableWindow::OnResize             , this);
    Bind(wxEVT_MOTION               , &wxResizeableWindow::OnMouseEvent         , this);
    Bind(wxEVT_LEFT_DOWN            , &wxResizeableWindow::OnMouseEvent         , this);
    Bind(wxEVT_LEFT_UP              , &wxResizeableWindow::OnMouseEvent         , this);
    Bind(wxEVT_LEAVE_WINDOW         , &wxResizeableWindow::OnMouseLeave         , this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST   , &wxResizeableWindow::OnMouseCaptureLost   , this);
    Bind(wxEVT_MOUSE_CAPTURE_CHANGED, &wxResizeableWindow::OnMouseCaptureChanged, this);
    Bind(wxEVT_PAINT                , &wxResizeableWindow::OnPaintEvent         , this);
    Bind(wxEVT_ERASE_BACKGROUND     , &wxResizeableWindow::OnEraseBackground    , this);

    return(true);
}

//------------------------------- BORDERS ---------------------------//
/** Set native resizing border
  * \param bNativeBorder [input]: true for native border, false for generic borders
  */
void wxResizeableWindow::SetNativeResizeBorder(bool bNativeBorder)
{
    m_bNativeBorder = bNativeBorder;
    InvalidateBestSize();
}

/** Check if native resizing border is used
  * \return true if native borders are used, false otherwise
  */
bool wxResizeableWindow::IsNativeResizeBorder(void) const
{
    return(m_bNativeBorder);
}

/** Set resizing borders (non-native)
  * \param iUp    [input]: thickness of the top resizing border. a value <= 0 deactivate this border
  * \param iDown  [input]: thickness of the bottom resizing border. a value <= 0 deactivate this border
  * \param iLeft  [input]: thickness of the left resizing border. a value <= 0 deactivate this border
  * \param iRight [input]: thickness of the right resizing border. a value <= 0 deactivate this border
  */
void wxResizeableWindow::SetResizeBorders(int iUp, int iDown, int iLeft, int iRight)
{
    m_BorderSizes[WX_TOP_BORDER   ] = iUp;
    m_BorderSizes[WX_BOTTOM_BORDER] = iDown;
    m_BorderSizes[WX_LEFT_BORDER  ] = iLeft;
    m_BorderSizes[WX_RIGHT_BORDER ] = iRight;
    InvalidateBestSize();
}

/** Get resizing borders (non-native)
  * \param iUp    [output] : thickness of the top resizing border. A value <= 0 means that this border is deactivated
  * \param iDown  [output] : thickness of the bottom resizing border. A value <= 0 means that this border is deactivated
  * \param iLeft  [output] : thickness of the left resizing border. A value <= 0 means that this border is deactivated
  * \param iRight [output] : thickness of the right resizing border. A value <= 0 means that this border is deactivated
  */
void wxResizeableWindow::GetResizeBorders(int &iUp, int &iDown, int &iLeft, int &iRight) const
{
    iUp    = m_BorderSizes[WX_TOP_BORDER   ];
    iDown  = m_BorderSizes[WX_BOTTOM_BORDER];
    iLeft  = m_BorderSizes[WX_LEFT_BORDER  ];
    iRight = m_BorderSizes[WX_RIGHT_BORDER ];
}

/** Get borders size - negatives values are set to 0
  * \param iUp    [output] : thickness of the top resizing border. A value <= 0 means that this border is deactivated
  * \param iDown  [output] : thickness of the bottom resizing border. A value <= 0 means that this border is deactivated
  * \param iLeft  [output] : thickness of the left resizing border. A value <= 0 means that this border is deactivated
  * \param iRight [output] : thickness of the right resizing border. A value <= 0 means that this border is deactivated
  */
WX_VDV_INLINE void wxResizeableWindow::DoGetBorders(int &iUp, int &iDown, int &iLeft, int &iRight) const
{
    iUp    = m_BorderSizes[WX_TOP_BORDER   ]; if (iUp    < 0) iUp    = 0;
    iDown  = m_BorderSizes[WX_BOTTOM_BORDER]; if (iDown  < 0) iDown  = 0;
    iLeft  = m_BorderSizes[WX_LEFT_BORDER  ]; if (iLeft  < 0) iLeft  = 0;
    iRight = m_BorderSizes[WX_RIGHT_BORDER ]; if (iRight < 0) iRight = 0;
}

//-------------------- MINIMAL SIZE ---------------------------------//
/** Check if minimal size is respected
  * \return true if minimal size is respected
  *         false if the window can be resized smaller than the minimal size
  */
bool wxResizeableWindow::RespectMinimalSize(void) const
{
    return(m_bRespectMinimalSize);
}

/** Set if the minimal size is respected
  * \param bRespect [input]: true if minimal size is respected
  *         false if the window can be resized smaller than the minimal size
  */
void wxResizeableWindow::RespectMinimalSize(bool bRespect)
{
    m_bRespectMinimalSize = bRespect;
}

//--------------------------- FORWARD TO PARENT ---------------------//
/** Check if the sizing events are forwarded to the parent
  * Either this window or the parent window can be resized
  * \return true for resizing the parent window
  *         false for resizing this window
  */
bool wxResizeableWindow::ForwardToParent(void) const
{
    return(m_bForwardToParent);
}

/** Set if the sizing events are forwarded to the parent
  * Either this window or the parent window can be resized
  * \param bForward [input]: true for resizing the parent window
  *                          false for resizing this window
  */
void wxResizeableWindow::ForwardToParent(bool bForward)
{
    m_bForwardToParent = bForward;
}

//---------------- wxWindow OVERRIDE --------------------------------//
/** Get best size
  * \return the best size
  */
wxSize wxResizeableWindow::DoGetBestSize(void) const
{
    //use default algorithm (takes borders into account)
    wxSize oBestSize = wxWindow::DoGetBestSize();
    const wxWindowList &vChildren = GetChildren();
    if (!vChildren.IsEmpty())
    {
        wxWindowList::const_iterator it = vChildren.begin();
        wxWindow *pChild = *it;
        if (pChild)
        {
            pChild->InvalidateBestSize();
            oBestSize = pChild->GetBestSize();
        }
    }

    //remove resizing borders thickness
    if (m_bNativeBorder) return(oBestSize);

    int iUp, iDown, iLeft, iRight;
    DoGetBorders(iUp, iDown, iLeft, iRight);

    //height
    int iHeight = oBestSize.GetHeight();
    if (iHeight > 0) iHeight += iUp + iDown;
    oBestSize.SetHeight(iHeight);

    //width
    int iWidth = oBestSize.GetWidth();
    if (iWidth > 0) iWidth += iLeft + iRight;
    oBestSize.SetWidth(iWidth);

    return(oBestSize);
}

//----------------------------------- EVENT HANDLERS ----------------//
/** Event handler on resize
  */
void wxResizeableWindow::OnResize(wxSizeEvent &rEvent)
{
    wxSize oSize(rEvent.GetSize());
    ResizeChildren(oSize);
    rEvent.Skip();
}

/** Event handler for mouse events
  */
void wxResizeableWindow::OnMouseEvent(wxMouseEvent &rEvent)
{
    if (!m_bNativeBorder)
    {
        //init
        int iUp, iDown, iLeft, iRight;
        DoGetBorders(iUp, iDown, iLeft, iRight);


        int x = rEvent.GetX();
        int y = rEvent.GetY();

        int w, h;
        GetClientSize(&w, &h);
        int iBottom = h - iDown;
            iRight  = w - iRight;

        //end resizing or resizing ?
        if (rEvent.LeftUp())
        {
            //end resizing
            m_iResizing = WX_E_BORDER_NONE;
            ReleaseMouse();
        }
        else if ((rEvent.Dragging()) && (rEvent.LeftIsDown()) && (m_iResizing != WX_E_BORDER_NONE))
        {
            //resizing

            //parent or this window
            wxWindow *pWin = this;
            if (m_bForwardToParent)
            {
                pWin = pWin->GetParent();
                if (!pWin) pWin = this;
            }

            //minimal size
            int iMinWidth = 1;
            int iMinHeight = 1;
            if (m_bRespectMinimalSize)
            {
                wxSize sMin = pWin->GetMinSize();
                iMinWidth = sMin.GetWidth();
                if (iMinWidth < 1) iMinWidth = 1;
                iMinHeight = sMin.GetHeight();
                if (iMinHeight < 1) iMinHeight = 1;
            }

            //resizing
            int xPos, yPos, iWinWidth, iWinHeight;
            pWin->GetPosition(&xPos, &yPos);
            pWin->GetSize(&iWinWidth, &iWinHeight);

            //translate to parent coordinates
            int x0 = 0, y0 = 0;
            pWin->GetPosition(&x0, &y0);
            x       += x0;
            y       += y0;
            iLeft   += x0;
            iRight  += x0;
            iUp     += y0;
            iBottom += y0;

            //resize left or right
                 if (m_iResizing & WX_E_BORDER_LEFT)    {iWinWidth -= x - xPos; xPos = x;}
            else if (m_iResizing & WX_E_BORDER_RIGHT)   {iWinWidth  = x - xPos;}
            if (iWinWidth < iMinWidth) iWinWidth = iMinWidth;

            //resize top or bottom
                 if (m_iResizing & WX_E_BORDER_TOP)     {iWinHeight -= y - yPos; yPos = y;}
            else if (m_iResizing & WX_E_BORDER_BOTTOM)  {iWinHeight = y - yPos;}
            if (iWinHeight < iMinHeight) iWinHeight = iMinHeight;

            pWin->SetSize(xPos, yPos, iWinWidth, iWinHeight, wxSIZE_ALLOW_MINUS_ONE);
            Refresh(true, WX_VDV_NULL_PTR);
            Update();
            return;
        }

        //check which border is hit
        int iBorder = WX_E_BORDER_NONE;
             if (x < iLeft )  iBorder |= WX_E_BORDER_LEFT;
        else if (x > iRight)  iBorder |= WX_E_BORDER_RIGHT;
             if (y < iUp)     iBorder |= WX_E_BORDER_TOP;
        else if (y > iBottom) iBorder |= WX_E_BORDER_BOTTOM;

        //change cursor if it is on one of the resizing border
        if (x < iLeft)
        {
            //left border or top left corner or bottom left corner
                 if (y < iUp)      SetCursor(wxCursor(wxCURSOR_SIZENWSE)); //top left corner
            else if (y > iBottom)  SetCursor(wxCursor(wxCURSOR_SIZENESW)); //bottom left corner
            else                   SetCursor(wxCursor(wxCURSOR_SIZEWE  )); //left border
        }
        else if (x > iRight)
        {
            //right border or top right corner or bottom right corner
                 if (y < iUp)      SetCursor(wxCursor(wxCURSOR_SIZENESW)); //top right corner
            else if (y > iBottom)  SetCursor(wxCursor(wxCURSOR_SIZENWSE)); //bottom right corner
            else                   SetCursor(wxCursor(wxCURSOR_SIZEWE  )); //right border
        }
        else if ((y < iUp) || (y > iBottom))
        {
            //top border or bottom border
            SetCursor(wxCursor(wxCURSOR_SIZENS));
        }
        else
        {
            //nothing specific
            SetCursor(wxCursor(wxCURSOR_ARROW));
            rEvent.Skip();
            return;
        }

        //now check if resizing is needed
        //note that if no borders are hit, then this code is not reached
        //so iBorder is guaranteed to be != WX_E_BORDER_NONE
        if (rEvent.LeftDown())
        {
            //start resizing
            m_iResizing = iBorder;
            CaptureMouse();
        }
    }

    rEvent.Skip();
}

/** Event handler for mouse leave events
  */
void wxResizeableWindow::OnMouseLeave(wxMouseEvent &rEvent)
{
    if (!m_bNativeBorder)
    {
        SetCursor(wxNullCursor);
    }
    rEvent.Skip();
}

/** Event handler for mouse capture lost
  */
void wxResizeableWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent &rEvent)
{
    m_iResizing = WX_E_BORDER_NONE;
    rEvent.Skip();
}

/** Event handler for mouse capture changed
  */
void wxResizeableWindow::OnMouseCaptureChanged(wxMouseCaptureChangedEvent &rEvent)
{
    m_iResizing = WX_E_BORDER_NONE;
    rEvent.Skip();
}

/** Event handler for erase background events
  */
void wxResizeableWindow::OnEraseBackground(wxEraseEvent &rEvent)
{
    //normal background drawing
    rEvent.Skip();
}

/** Event handler for paint events
  */
void wxResizeableWindow::OnPaintEvent(wxPaintEvent &rEvent)
{
    wxPaintDC dc(this);
    if (m_bNativeBorder) return;
    PaintBorders(dc);
}

//---------------------PAINT  METHODS -------------------------------//
#define WX_POINT_SIZE           3
/** Paint the borders
  * \param rDC [input]: the device context
  */
void wxResizeableWindow::PaintBorders(wxDC &rDC)
{
    //background already handled - just draw the resizing border
    wxColour oColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    wxBrush oBrush(oColour, wxBRUSHSTYLE_SOLID);

    int w, h, iUp, iDown, iLeft, iRight;
    GetClientSize(&w, &h);
    GetResizeBorders(iUp, iDown, iLeft, iRight);

    //middle of borders - draw 3 rectangles 2x2 in the middle of the border
    //corners : draw 2 points horizontally and 1 vertically: + +
    //                                                         +
    const int yUp   = iUp / 2 - 1;
    const int yDown = h - iDown / 2 - 1;
    const int xLeft = iLeft / 2 - 1;
    const int xRight = w - iRight / 2 - 1;
    const int xMiddle = w / 2;
    const int yMiddle = h / 2;
    const int iShift  = WX_POINT_SIZE + WX_POINT_SIZE / 2 + 1;
    rDC.SetBrush(oBrush);
    rDC.SetPen(*wxTRANSPARENT_PEN);
    if (iUp    > 0)
    {
        DoDrawPointsH(rDC, xMiddle, yUp, iUp); //middle top border
        if (iLeft  > 0) DoDraw2PointsH(rDC, 1 , yUp, iUp); //top left corner
        if (iRight > 0) DoDraw2PointsH(rDC, xRight - iShift + 1 , yUp, iUp); //top right corner
    }
    if (iDown  > 0)
    {
        DoDrawPointsH(rDC, xMiddle, yDown  , iDown); //middle bottom border
        if (iLeft  > 0) DoDraw2PointsH(rDC, 1 , yDown, iDown); //bottom left corner
        if (iRight > 0) DoDraw2PointsH(rDC, xRight - iShift + 1 , yDown, iDown); //bottom right corner
    }
    if (iLeft  > 0)
    {
        DoDrawPointsV(rDC, xLeft  , yMiddle, iLeft); //middle left border
        if (iUp   > 0) DoDraw1Point(rDC, xLeft, yUp + iShift, iLeft, h); //top left corner
        if (iDown > 0) DoDraw1Point(rDC, xLeft, yDown - iShift + 1, iLeft, h); //bottom left corner
    }
    if (iRight > 0)
    {
        DoDrawPointsV(rDC, xRight , yMiddle, iRight); //middle right border
        if (iUp   > 0) DoDraw1Point(rDC, xRight, yUp + iShift, iRight, h); //top right corner
        if (iDown > 0) DoDraw1Point(rDC, xRight, yDown - iShift + 1, iRight, h); //bottom right corner
    }
}

/** Draw 3 aligned points horizontally
  * \param rDC  [input]: the device context to use
  * \param x    [input]: X coordinate of the centre of the line defined by the 3 points
  * \param y    [input]: Y coordinate of the centre of the line defined by the 3 points
  * \param h    [input]: total height available for drawing
  */
void wxResizeableWindow::DoDrawPointsH(wxDC &rDC, int x, int y, int h)
{
    int iHeight = WX_POINT_SIZE;
    if (iHeight > h) iHeight = h;
    const int x1 = x - WX_POINT_SIZE - WX_POINT_SIZE / 2 - 1;
    const int x2 = x1 + WX_POINT_SIZE + 1;
    const int x3 = x2 + WX_POINT_SIZE + 1;
    const int y0 = y - WX_POINT_SIZE / 2 + 1;
    rDC.DrawRectangle(x1, y0, WX_POINT_SIZE, iHeight);
    rDC.DrawRectangle(x2, y0, WX_POINT_SIZE, iHeight);
    rDC.DrawRectangle(x3, y0, WX_POINT_SIZE, iHeight);
}

/** Draw 3 aligned points vertically
  * \param rDC  [input]: the device context to use
  * \param x    [input]: X coordinate of the centre of the line defined by the 3 points
  * \param y    [input]: Y coordinate of the centre of the line defined by the 3 points
  * \param w    [input]: total width available for drawing
  */
void wxResizeableWindow::DoDrawPointsV(wxDC &rDC, int x, int y, int w)
{
    int iWidth = WX_POINT_SIZE;
    if (iWidth > w) iWidth = w;
    const int x0 = x - WX_POINT_SIZE / 2 + 1;
    const int y1 = y - WX_POINT_SIZE - WX_POINT_SIZE / 2 - 1;
    const int y2 = y1 + WX_POINT_SIZE + 1;
    const int y3 = y2 + WX_POINT_SIZE + 1;
    rDC.DrawRectangle(x0, y1, iWidth, WX_POINT_SIZE);
    rDC.DrawRectangle(x0, y2, iWidth, WX_POINT_SIZE);
    rDC.DrawRectangle(x0, y3, iWidth, WX_POINT_SIZE);
}

/** Draw 2 points horizontally in a corner
  * \param rDC  [input]: the device context to use
  * \param x    [input]: X coordinate of the left of the line defined by the 2 points
  * \param y    [input]: Y coordinate of the centre of the line defined by the 2 points
  * \param h    [input]: total height available for drawing
  */
void wxResizeableWindow::DoDraw2PointsH(wxDC &rDC, int x, int y, int h)
{
    int iHeight = WX_POINT_SIZE;
    if (iHeight > h) iHeight = h;
    const int x1 = x;
    const int x2 = x1 + WX_POINT_SIZE + 1;
    const int y0 = y - WX_POINT_SIZE / 2 + 1;
    rDC.DrawRectangle(x1, y0, WX_POINT_SIZE, iHeight);
    rDC.DrawRectangle(x2, y0, WX_POINT_SIZE, iHeight);
}

/** Draw 1 point in a corner
  * \param rDC  [input]: the device context to use
  * \param x    [input]: X coordinate of the middle of the point
  * \param y    [input]: Y coordinate of the middle of the point
  * \param w    [input]: total width available for drawing
  * \param h    [input]: total height available for drawing
  */
void wxResizeableWindow::DoDraw1Point(wxDC &rDC, int x, int y, int w, int h)
{
    int iWidth = WX_POINT_SIZE;
    if (iWidth > w) iWidth = w;

    int iHeight = WX_POINT_SIZE;
    if (iHeight > h) iHeight = h;

    const int x0 = x - WX_POINT_SIZE / 2 + 1;
    const int y0 = y - WX_POINT_SIZE / 2 + 1;
    rDC.DrawRectangle(x0, y0, iWidth, iHeight);
}

//---------------------- SIZING METHODS -----------------------------//
/** Check if the child window should be expanded
  * \return true if expand is needed, false otherwise
  * Expand is needed if:
  *     - only 1 child window is present
  *     - no sizers are set
  *     - no constraints are set
  */
bool wxResizeableWindow::ShouldExpandChildWindow(void) const
{
    //no constraints are set
    #if wxUSE_CONSTRAINTS
        if (GetConstraints()) return(false);
    #endif // wxUSE_CONSTRAINTS

    //no sizers
    if (GetSizer()) return(false);

    //only 1 child window is present
    const wxWindowList &vChildren = GetChildren();
    if (vChildren.size() != 1) return(false);

    return(true);
}

/** Resize the children
  * \param rSize [input]: the new window size (whole size of the window, including borders
  */
void wxResizeableWindow::ResizeChildren(const wxSize &rSize)
{
    //get non-native borders size
    int l, r, t, b;
    DoGetBorders(t, b, l, r);

    //layout - shift all children and clip them
    wxSize sClientArea = GetClientSize();
    wxWindowList &vChildren = GetChildren();
    int iWidth  = sClientArea.GetWidth();
    int iHeight = sClientArea.GetHeight();

    //special case : only 1 child
    wxWindowList::iterator it;
    if (ShouldExpandChildWindow())
    {
        it = vChildren.begin();
        wxWindow *pChild = *it;
        if (!pChild) return;

        //expand
        wxRect rChildRect(l, t, iWidth - l - r, iHeight - t - b);
        pChild->SetSize(rChildRect);

        return;
    }

    //more than 1 children
    for(it = vChildren.begin(); it != vChildren.end(); ++it)
    {
        wxWindow *pChild = *it;
        if (!pChild) continue;

        //shift top left corner
        wxRect rChildRect = pChild->GetRect();
        int iLeft = rChildRect.GetLeft();
        iLeft += l;
        rChildRect.SetLeft(iLeft);

        int iTop = rChildRect.GetTop();
        iTop += t;
        rChildRect.SetTop(iTop);

        //clip width
        int iRight = rChildRect.GetRight();
        if (iRight > iWidth) iRight = iWidth - r;
        rChildRect.SetRight(iRight);

        //clip height
        int iBottom = rChildRect.GetBottom();
        if (iBottom > iHeight) iBottom = iHeight - b;
        rChildRect.SetBottom(iBottom);

        //set size
        pChild->SetSize(rChildRect);
    }
}


