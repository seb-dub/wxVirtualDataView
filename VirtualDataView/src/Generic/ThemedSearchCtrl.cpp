/**********************************************************************/
/** FILE    : ThemedSearchCtrl.cpp                                   **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : the wxSearchCtrl, with correct theming enabled         **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/Generic/ThemedSearchCtrl.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

/** Default constructor. The method Create() must be called
  */
wxThemedSearchCtrl::wxThemedSearchCtrl(void)
    : wxSystemThemedControl<wxSearchCtrl>()
{
}

/** Constructor
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param value     [input]: the initial searched string displayed
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param validator [input]: the validator for the control
  * \param name      [input]: the name of the object
  */
wxThemedSearchCtrl::wxThemedSearchCtrl(wxWindow *pParent, wxWindowID id, const wxString &value,
                                       const wxPoint& pos, const wxSize& size, long lStyle,
                                       const wxValidator &validator, const wxString &name)

    : wxSystemThemedControl<wxSearchCtrl>()
{
    CreateWin(pParent, id, value, pos, size, lStyle, validator, name);
}

/** Destructor (virtual)
  */
wxThemedSearchCtrl::~wxThemedSearchCtrl(void)
{

}

//----------------------- CREATION METHODS --------------------------//
/** Really creates the control
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param value     [input]: the initial searched string displayed
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param validator [input]: the validator for the control
  * \param name      [input]: the name of the object
  */
bool wxThemedSearchCtrl::CreateWin(wxWindow *pParent, wxWindowID id, const wxString &value,
                                   const wxPoint& pos, const wxSize& size, long lStyle,
                                   const wxValidator &validator, const wxString &name)
{
    if (!wxSearchCtrl::Create(pParent, id, value,
                              pos, size, lStyle, validator, name))
    {
        return(false);
    }

    EnableSystemTheme();
//    SetBackgroundColour(wxNullColour);
    SetBackgroundStyle(wxBG_STYLE_SYSTEM);

    return(true);
}

//----------------------------- BITMAP RENDERING --------------------//
#if wxUSE_NATIVE_SEARCH_CONTROL == 0
// icons are rendered at 3-8 times larger than necessary and downscaled for
// antialiasing
static int GetMultiplier()
{
    int depth = ::wxDisplayDepth();

    if  ( depth >= 24 )
    {
        return 8;
    }
    return 6;
}

static void RescaleBitmap(wxBitmap& bmp, const wxSize& sizeNeeded)
{
    wxCHECK_RET( sizeNeeded.IsFullySpecified(), wxS("New size must be given") );

    wxBitmap newBmp(sizeNeeded, bmp.GetDepth());
#if defined(__WXMSW__) || defined(__WXOSX__)
    // wxBitmap::UseAlpha() is used only on wxMSW and wxOSX.
    newBmp.UseAlpha(bmp.HasAlpha());
#endif // __WXMSW__ || __WXOSX__
    {
        wxMemoryDC dc(newBmp);
        double scX = (double)sizeNeeded.GetWidth() / bmp.GetWidth();
        double scY = (double)sizeNeeded.GetHeight() / bmp.GetHeight();
        dc.SetUserScale(scX, scY);
        dc.DrawBitmap(bmp, 0, 0, true);
    }
    bmp = newBmp;
}

#define LIGHT_STEP 160
#define WXMAX(a,b) ((a)>(b)?(a):(b))

/** Render the search button
  * \param x [input]: x coordinate of the top left corner
  * \param y [input]: y coordinate of the top left corner
  * \param renderDrop [input]: true if a drop-down arrow should be rendered
  *                            (for custom menu)
  * \return the drawn bitmap
  */
wxBitmap wxThemedSearchCtrl::RenderSearchBitmap(int x, int y, bool renderDrop)
{
    //return(wxSearchCtrl::RenderSearchBitmap(x, y, renderDrop));

    wxColour bg = GetBackgroundColour();
    wxColour fg = GetForegroundColour().ChangeLightness(LIGHT_STEP-20);

    //MAIN BITMAP
    // image stats
    // force width:height ratio
    if (14 * x > y * 20) x = y * 20 / 14;   // x is too big
    else                 y = x * 14 / 20;   // y is too big

    // glass 11x11, top left corner
    // handle (9,9)-(13,13)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();
    int penWidth   = multiplier * 2;

    penWidth = penWidth * x / 20;

    wxBitmap bitmap(multiplier * x, multiplier * y);
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    //clear background
    wxRect r(0, 0, bitmap.GetWidth(), bitmap.GetHeight());
    wxRendererNative::Get().DrawItemSelectionRect(this, mem, r, 0);

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int glassBase = 5 * x / 20;
    int glassFactor = 2*glassBase + 1;
    int radius = multiplier*glassFactor/2;
    mem.DrawCircle(radius,radius,radius);


    //draw handle
    int lineStart = radius + (radius-penWidth/2) * 707 / 1000; // 707 / 1000 = 0.707 = 1/sqrt(2);

    mem.SetPen( wxPen(fg) );
    mem.SetBrush( wxBrush(fg) );
    int handleCornerShift = penWidth * 707 / 1000 / 2; // 707 / 1000 = 0.707 = 1/sqrt(2);
    handleCornerShift = WXMAX( handleCornerShift, 1 );
    int handleBase = 4 * x / 20;
    int handleLength = 2*handleBase+1;
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*handleLength/2+handleCornerShift,multiplier*handleLength/2-handleCornerShift),
        wxPoint(multiplier*handleLength/2-handleCornerShift,multiplier*handleLength/2+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,lineStart,lineStart);

    // draw drop triangle
    int triangleX = 13 * x / 20;
    int triangleY = 5 * x / 20;
    int triangleBase = 3 * x / 20;
    int triangleFactor = triangleBase*2+1;
    wxPoint dropPolygon[] =
    {
        wxPoint(multiplier*0,multiplier*0), // triangle left
        wxPoint(multiplier*triangleFactor-1,multiplier*0), // triangle right
        wxPoint(multiplier*triangleFactor/2,multiplier*triangleFactor/2), // triangle bottom
    };
    if (renderDrop)
    {
        mem.DrawPolygon(WXSIZEOF(dropPolygon),dropPolygon,multiplier*triangleX,multiplier*triangleY);
    }
    mem.SelectObject(wxNullBitmap);

    //MASK BITMAP
    wxBitmap bitmapMask(multiplier * x, multiplier * y, 1); //monochrome bitmaps
    mem.SelectObject(bitmapMask);

    //clear background - all black = all transparent
    mem.SetBrush(*wxBLACK_BRUSH);
    mem.SetPen(*wxBLACK_PEN);
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    //drop glass visible part
    mem.SetBrush(*wxWHITE_BRUSH);
    mem.SetPen(*wxWHITE_PEN);
    mem.DrawCircle(radius,radius,radius);

    //handle visible part
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,lineStart,lineStart);

    //drop triangle visible part
    if (renderDrop)
    {
        mem.DrawPolygon(WXSIZEOF(dropPolygon),dropPolygon,multiplier*triangleX,multiplier*triangleY);
    }

    //drop glass hidden part
    mem.SetBrush(*wxBLACK_BRUSH);
    mem.SetPen(*wxBLACK_PEN);
    mem.DrawCircle(radius,radius,radius-penWidth);
    mem.SelectObject(wxNullBitmap);
    //END MASK BITMAP

    //SET MASK
    bitmap.SetMask(new wxMask(bitmapMask));

    //RESCALE
    if ( multiplier != 1 )
    {
        RescaleBitmap(bitmap, wxSize(x, y));
        RescaleBitmap(bitmapMask, wxSize(x, y));
        bitmap.SetMask(new wxMask(bitmapMask));
    }

    if ( !renderDrop )
    {
        // Trim the edge where the arrow would have gone
        bitmap = bitmap.GetSubBitmap(wxRect(0,0, y,y));
    }

    return bitmap;
}

/** Render the cancel button
  * \param x [input]: x coordinate of the top left corner
  * \param y [input]: y coordinate of the top left corner
  * \return the drawn bitmap
  */
wxBitmap wxThemedSearchCtrl::RenderCancelBitmap(int x, int y)
{
    //return(wxSearchCtrl::RenderCancelBitmap(x, y));
    wxColour bg = GetBackgroundColour();
    wxColour fg = GetForegroundColour().ChangeLightness(LIGHT_STEP-20);

    //MAIN BITMAP
    // image stats
    // total size 14x14
    // force 1:1 ratio
    if (x > y) x = y;   // x is too big
    else       y = x;   // y is too big

    // 14x14 circle
    // cross line starts (4,4)-(10,10)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();
    int penWidth = multiplier * x / 14;

    wxBitmap bitmap( multiplier*x, multiplier*y );
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    //clear background
    wxRect r(0, 0, bitmap.GetWidth(), bitmap.GetHeight());
    wxRendererNative::Get().DrawItemSelectionRect(this, mem, r, 0);

    //draw circle
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int radius = multiplier*x/2;
    mem.DrawCircle(radius,radius,radius);

    // draw cross
    int lineStartBase = 4 * x / 14;
    int lineLength = x - 2*lineStartBase;

    mem.SetPen( wxPen(bg) );
    mem.SetBrush( wxBrush(bg) );
    int handleCornerShift = penWidth/2;
    handleCornerShift = WXMAX( handleCornerShift, 1 );
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,multiplier*lineStartBase,multiplier*lineStartBase);
    wxPoint handlePolygon2[] =
    {
        wxPoint(+handleCornerShift,+handleCornerShift),
        wxPoint(-handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,-multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,-multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon2),handlePolygon2,multiplier*lineStartBase,multiplier*(x-lineStartBase));
    mem.SelectObject(wxNullBitmap);
    //END MAIN BITMAP

    //MASK BITMAP
    wxBitmap bitmapMask(multiplier * x, multiplier * y, 1); //monochrome bitmaps
    mem.SelectObject(bitmapMask);

    //clear background - all black = all transparent
    mem.SetBrush(*wxBLACK_BRUSH);
    mem.SetPen(*wxBLACK_PEN);
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    //circle visible part
    mem.SetBrush(*wxWHITE_BRUSH);
    mem.SetPen(*wxWHITE_PEN);
    mem.DrawCircle(radius,radius,radius);

    mem.SelectObject(wxNullBitmap);
    //END MASK BITMAP

    //SET MASK
    bitmap.SetMask(new wxMask(bitmapMask));

    //RESCALE
    if ( multiplier != 1 )
    {
        RescaleBitmap(bitmap, wxSize(x, y));
        RescaleBitmap(bitmapMask, wxSize(x, y));
        bitmap.SetMask(new wxMask(bitmapMask));
    }

    return bitmap;
}

#endif // wxUSE_NATIVE_SEARCH_CONTROL

