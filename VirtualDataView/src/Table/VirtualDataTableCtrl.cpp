/**********************************************************************/
/** FILE    : VirtualDataViewCtrl.cpp                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a class for viewing a very large amount of items       **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/Table/VirtualDataTableCtrl.h>
#include <wx/VirtualDataView/Table/VirtualDataTableBase.h>
#include <wx/VirtualDataView/Mappers/VirtualDataViewCellMapper.h>
#include <wx/VirtualDataView/Table/VirtualDataTableColHeader.h>
#include <wx/VirtualDataView/Table/VirtualDataTableRowHeader.h>
#include <wx/VirtualDataView/Table/VirtualDataTableCornerHeader.h>

#include <wx/sizer.h>
#include <wx/dcclient.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataTableCtrl, wxHVScrolledWindow);
const char wxVirtualDataTableCtrlNameStr[] = "wxVirtualDataTableCtrl";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataTableCtrl::wxVirtualDataTableCtrl(void)
    : wxSystemThemedControl<wxHVScrolledWindow>(),
      m_pClientArea(WX_VDV_NULL_PTR),
      m_pColumnHeaderWindow(WX_VDV_NULL_PTR),
      m_pRowHeaderWindow(WX_VDV_NULL_PTR),
      m_pCornerHeaderWindow(WX_VDV_NULL_PTR)
{
    InitDataTable();
}

/** Constructor
  * Creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param validator [input]: validator
  * \param sName     [input]: the name of the object
  */
wxVirtualDataTableCtrl::wxVirtualDataTableCtrl(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                             const wxSize& size, long lStyle,
                                             const wxValidator& validator, wxString sName)

    : wxSystemThemedControl<wxHVScrolledWindow>(),
      m_pClientArea(WX_VDV_NULL_PTR),
      m_pColumnHeaderWindow(WX_VDV_NULL_PTR),
      m_pRowHeaderWindow(WX_VDV_NULL_PTR),
      m_pCornerHeaderWindow(WX_VDV_NULL_PTR)
{
    InitDataTable();
    Create(pParent, id, pos, size, lStyle, validator, sName);
}

/** Destructor (virtual)
  */
wxVirtualDataTableCtrl::~wxVirtualDataTableCtrl(void)
{

}

//---------------------- CREATION METHODS ---------------------------//
/** Common parts of all constructors
  */
void wxVirtualDataTableCtrl::InitDataTable(void)
{
    m_pClientArea         = WX_VDV_NULL_PTR;
    m_pColumnHeaderWindow = WX_VDV_NULL_PTR;
    m_pRowHeaderWindow    = WX_VDV_NULL_PTR;
    m_pCornerHeaderWindow = WX_VDV_NULL_PTR;
    m_dx = 0;
}

/** Really creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent [input]: parent of the window
  * \param id      [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos     [input]: initial position of the window. Default to wxDefaultPosition
  * \param size    [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle  [input]: window style. Default to 0
  * \param validator [input]: validator
  * \param sName   [input]: the name of the object
  * \return true on success or false if the control couldn't be created
  */
bool wxVirtualDataTableCtrl::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                    const wxSize& size, long lStyle,
                                    const wxValidator& validator, wxString sName)
{
    InitDataTable();

    //wxWidgets initialization
    if (!wxHVScrolledWindow::Create(pParent, id, pos, size,
                                    lStyle | wxScrolledWindowStyle, sName))
    {
        return(false);
    }
    SetInitialSize(size);

#ifdef __WXMAC__
    MacSetClipChildren( true );
#endif

    EnableSystemTheme();

    //child windows creation
    //main window
    long lStyleMain = lStyle;
    //remove border
    lStyleMain &= ~wxBORDER_MASK;
    lStyleMain |= wxBORDER_NONE;
    m_pClientArea = new wxVirtualDataTableBase(this, wxID_ANY, pos, size, lStyleMain);
    if (m_pClientArea) m_pClientArea->SetOwner(this);

    //default style for row/column header
    long lHeaderStyle = wxVirtualHeaderCtrl::WX_VDV_DEFAULT_STYLE | wxBORDER_NONE;

    //column header
    if (HasFlag(wxVDT_NO_COL_HEADER))
    {
        m_pColumnHeaderWindow = WX_VDV_NULL_PTR;
    }
    else
    {
        m_pColumnHeaderWindow = new wxVirtualDataTableColHeader(this, wxID_ANY,
                                                                wxDefaultPosition, wxDefaultSize,
                                                                lHeaderStyle);
        m_pColumnHeaderWindow->SetOwner(this);
    }

    //row header
    if (HasFlag(wxVDT_NO_ROW_HEADER))
    {
        m_pRowHeaderWindow = WX_VDV_NULL_PTR;
    }
    else
    {
        m_pRowHeaderWindow = new wxVirtualDataTableRowHeader(this, wxID_ANY,
                                                             wxDefaultPosition, wxDefaultSize,
                                                             lHeaderStyle);
        m_pRowHeaderWindow->SetOwner(this);
    }

    //corner header
    if ((m_pColumnHeaderWindow) && (m_pRowHeaderWindow))
    {
        m_pCornerHeaderWindow = new wxVirtualDataTableCornerHeader(this, wxID_ANY,
                                                                   wxDefaultPosition, wxDefaultSize,
                                                                   wxBORDER_NONE);
    }
    else
    {
        m_pCornerHeaderWindow = WX_VDV_NULL_PTR;
    }

//    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
//    if (m_pColumnHeaderWindow) pSizer->Add(m_pColumnHeaderWindow, 0, wxEXPAND);
//    pSizer->Add(m_pClientArea, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
//    SetSizer(pSizer);
//    pSizer->Fit(this);
//	pSizer->SetSizeHints(this);

	//scrollbars
//    wxVarHScrollHelper::SetTargetWindow(m_pClientArea);
//    wxVarVScrollHelper::SetTargetWindow(m_pClientArea);
    EnablePhysicalScrolling(false, false);
    Bind(wxEVT_SIZE                  , &wxVirtualDataTableCtrl::OnSize       , this);
    Bind(wxEVT_SCROLLWIN_TOP         , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_BOTTOM      , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_LINEUP      , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_LINEDOWN    , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_PAGEUP      , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_PAGEDOWN    , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK  , &wxVirtualDataTableCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &wxVirtualDataTableCtrl::OnScrollEvent, this);

    return(true);
}

//--------------------- MAIN WINDOW ---------------------------------//
/** Get the main window
  * \return the main window
  */
wxVirtualDataTableBase* wxVirtualDataTableCtrl::GetMainWindow(void) const
{
    return(m_pClientArea);
}

//--------------------- HEADERS -------------------------------------//
/** Get the column header window
  * \return the column header window
  */
wxVirtualDataTableHeader* wxVirtualDataTableCtrl::GetColumnHeaderWindow(void) const
{
    return(m_pColumnHeaderWindow);
}

/** Get the row header window
  * \return the row header window
  */
wxVirtualDataTableHeader* wxVirtualDataTableCtrl::GetRowHeaderWindow(void) const
{
    return(m_pRowHeaderWindow);
}

/** Get the corner header window
  * \return the corner header window
  */
wxVirtualDataTableCornerHeader* wxVirtualDataTableCtrl::GetCornerHeaderWindow(void) const
{
    return(m_pCornerHeaderWindow);
}

//--------------- WINDOWS MANAGEMENT --------------------------------//
#ifdef __WXUNIVERSAL__
/** Check if the current window is the current one
  * \return true if the window is the current one, false otherwise
  */
bool wxVirtualDataTableCtrl::IsCurrent(void) const
{
    return(wxControl::IsCurrent());
}
#endif

//------------------- VISUAL ATTRIBUTES -----------------------------//
/** Get the default window attributes
  * \return the default visual attributes
  */
wxVisualAttributes wxVirtualDataTableCtrl::GetDefaultAttributes(void) const
{
    if (m_pClientArea) return(m_pClientArea->GetDefaultAttributes());
    return(wxVirtualDataTableCtrl::GetClassDefaultAttributes());
}

/** Get the default window attributes - static
  * \param variant [input] : what attribute to get. Default value = wxWINDOW_VARIANT_NORMAL
  * \return the visual attributes
  */
wxVisualAttributes wxVirtualDataTableCtrl::GetClassDefaultAttributes(wxWindowVariant variant)
{
    return(wxVirtualDataTableBase::GetClassDefaultAttributes(variant));
}

//----------- SELECTION MODEL ---------------------------------------//
///** Get the selection storage
//  * \return the selection storage. It is still owned by the class itself
//  */
//ISelectionStorage* wxVirtualDataTableCtrl::GetSelectionStorage(void)
//{
//    if (m_pClientArea) return(m_pClientArea->GetSelectionStorage());
//    return(WX_VDV_NULL_PTR);
//}
//
///** Set the selection storage
//  * \param pSS [input] : the new selection storage.
//  *                      Ownership is taken. It must have been allocated with "new"
//  */
//void wxVirtualDataTableCtrl::SetSelectionStorage(ISelectionStorage *pSS)
//{
//    if (m_pClientArea) m_pClientArea->SetSelectionStorage(pSS);
//}

//--------------- DATA MODEL ----------------------------------------//
/** Get the current data model used
  * \return the current data model used. Do not delete the pointer : it is still owned by the control
  */
wxVirtualITableTreeDataModel* wxVirtualDataTableCtrl::GetDataModel(void)
{
    if (m_pClientArea) return(m_pClientArea->GetDataModel());
    return(WX_VDV_NULL_PTR);
}

/** Set the current data model
  * \param pModel [input] : the new data model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataTableCtrl::SetDataModel(wxVirtualITableTreeDataModel *pModel)
{
    if (m_pClientArea) m_pClientArea->SetDataModel(pModel);
    OnModelRowCountChanged();
    OnModelColCountChanged();
}

//---------------------- CALLBACKS FOR DATA CHANGE ------------------//
/** The model has changed its amount of row
  */
void wxVirtualDataTableCtrl::OnModelRowCountChanged(void)
{
    if (m_pClientArea) m_pClientArea->OnModelRowCountChanged();
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->SetOwner(this);
    if (m_pRowHeaderWindow)    m_pRowHeaderWindow->SetOwner(this);
    if (m_pCornerHeaderWindow) m_pCornerHeaderWindow->SetOwner(this);
    SetRowCount(GetAmountOfViewRows());
}

/** The model has changed its amount of columns
  */
void wxVirtualDataTableCtrl::OnModelColCountChanged(void)
{
    if (m_pClientArea) m_pClientArea->OnModelColCountChanged();
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->SetOwner(this);
    if (m_pRowHeaderWindow)    m_pRowHeaderWindow->SetOwner(this);
    if (m_pCornerHeaderWindow) m_pCornerHeaderWindow->SetOwner(this);
    SetColumnCount(GetAmountOfViewCols());
}

//------------------ ROWS MAPPING -----------------------------------//
/** Get the row mapper
  * \return the row mapper.
  *         Do not delete it : it is still owned by the wxVirtualDataTableCtrl object
  */
wxVirtualDataViewCellMapper* wxVirtualDataTableCtrl::GetRowMapper(void) const
{
    if (m_pClientArea) return(m_pClientArea->GetRowMapper());
    return(WX_VDV_NULL_PTR);
}

/** Set the row mapper
  * \param pMapper [input]: the new row mapper. Ownership is taken
  *                         it must have been allocated with "new"
  */
void wxVirtualDataTableCtrl::SetRowMapper(wxVirtualDataViewCellMapper *pMapper)
{
    if (m_pClientArea) m_pClientArea->SetRowMapper(pMapper);
    else               delete(pMapper);
    OnModelRowCountChanged();
}

/** Convert a view row coord to a model row coord
  * \param uiViewRow [input]: the view row
  * \return the model row
  */
size_t wxVirtualDataTableCtrl::GetModelRow(size_t uiViewRow) const
{
    if (m_pClientArea) return(m_pClientArea->GetModelRow(uiViewRow));
    return(uiViewRow);
}

/** Convert a view model coord to a view row coord
  * \param uiModelRow [input]: the model row
  * \return the view row
  */
size_t wxVirtualDataTableCtrl::GetViewRow(size_t uiModelRow) const
{
    if (m_pClientArea) return(m_pClientArea->GetViewRow(uiModelRow));
    return(uiModelRow);
}

/** Get the amount of view rows
  * \return the amount of view rows
  */
size_t wxVirtualDataTableCtrl::GetAmountOfViewRows(void) const
{
    if (m_pClientArea) return(m_pClientArea->GetAmountOfViewRows());
    return(1);
}

//------------------- COLUMNS MAPPING -------------------------------//
/** Get the column mapper
  * \return the column mapper.
  *         Do not delete it : it is still owned by the wxVirtualDataTableCtrl object
  */
wxVirtualDataViewCellMapper* wxVirtualDataTableCtrl::GetColMapper(void) const
{
    if (m_pClientArea) return(m_pClientArea->GetColMapper());
    return(WX_VDV_NULL_PTR);
}

/** Set the column mapper
  * \param pMapper [input]: the new column mapper. Ownership is taken
  *                         it must have been allocated with "new"
  */
void wxVirtualDataTableCtrl::SetColMapper(wxVirtualDataViewCellMapper *pMapper)
{
    if (m_pClientArea) m_pClientArea->SetColMapper(pMapper);
    else               delete(pMapper);
    OnModelColCountChanged();
}

/** Convert a view column coord to a model column coord
  * \param uiViewCol [input]: the view column
  * \return the model column
  */
size_t wxVirtualDataTableCtrl::GetModelCol(size_t uiViewCol) const
{
    if (m_pClientArea) return(m_pClientArea->GetModelCol(uiViewCol));
    return(uiViewCol);
}

/** Convert a view model coord to a view column coord
  * \param uiModelCol [input]: the model column
  * \return the view column
  */
size_t wxVirtualDataTableCtrl::GetViewCol(size_t uiModelCol) const
{
    if (m_pClientArea) return(m_pClientArea->GetViewCol(uiModelCol));
    return(uiModelCol);
}

/** Get the amount of view columns
  * \return the amount of view columns
  */
size_t wxVirtualDataTableCtrl::GetAmountOfViewCols(void) const
{
    if (m_pClientArea) return(m_pClientArea->GetAmountOfViewCols());
    return(1);
}

//---------- wxHVScrolledWindow INTERFACE IMPLEMENTATION ------------//
/** Get the height of a row
  * \param uiViewRow [input]: the row index (view coordinate)
  * \return the row height
  */
wxCoord wxVirtualDataTableCtrl::OnGetRowHeight(size_t uiViewRow) const
{
    if (m_pClientArea) return(m_pClientArea->GetRowHeight(uiViewRow));
    return(1);
}

/** Get the column width
  * \param uiViewCol [input]: the column index (view coordinate)
  * \return the column height
  */
wxCoord wxVirtualDataTableCtrl::OnGetColumnWidth(size_t uiViewCol) const
{
    if (m_pClientArea) return(m_pClientArea->GetColumnWidth(uiViewCol));
    return(1);
}

//------------------------ SCROLLING EVENTS HANDLERS ----------------//
/** Sizing event handlers
  */
void wxVirtualDataTableCtrl::OnSize(wxSizeEvent &event)
{
    wxSize sClientSize = event.GetSize();

    int iWidthLeft   = 0;
    int iHeightAbove = 0;

    //get minimal sizes
    if (m_pColumnHeaderWindow)
    {
        wxSize s = m_pColumnHeaderWindow->GetBestSize();
        if (s.GetHeight() > iHeightAbove) iHeightAbove = s.GetHeight();
    }

    if (m_pRowHeaderWindow)
    {
        wxSize s = m_pRowHeaderWindow->GetBestSize();
        if (s.GetWidth() > iWidthLeft) iWidthLeft = s.GetWidth();
    }

    if (m_pCornerHeaderWindow)
    {
        wxSize s = m_pCornerHeaderWindow->GetBestSize();
        if (s.GetWidth() > iWidthLeft) iWidthLeft = s.GetWidth();
        if (s.GetHeight() > iHeightAbove) iHeightAbove = s.GetHeight();
    }

    //layout
    int w = sClientSize.GetWidth()  - iWidthLeft;
    int h = sClientSize.GetHeight() - iHeightAbove;
    if (m_pCornerHeaderWindow) m_pCornerHeaderWindow->SetSize(0, 0, iWidthLeft, iHeightAbove);
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->SetSize(iWidthLeft, 0, w, iHeightAbove);
    if (m_pRowHeaderWindow)    m_pRowHeaderWindow->SetSize(0, iHeightAbove, iWidthLeft, h);
    if (m_pClientArea)         m_pClientArea->SetSize(iWidthLeft, iHeightAbove, w, h);
}

/** Scrolling event handler
  */
void wxVirtualDataTableCtrl::OnScrollEvent(wxScrollWinEvent &event)
{
//    wxScrollWinEvent *pEvent = new wxScrollWinEvent(event);
//    QueueEvent(pEvent);

    //redraw children windows
    if (m_pCornerHeaderWindow)
    {
        m_pCornerHeaderWindow->Refresh(false, WX_VDV_NULL_PTR);
        m_pCornerHeaderWindow->Update();
    }

    if (m_pRowHeaderWindow)
    {
        m_pRowHeaderWindow->Refresh(false, WX_VDV_NULL_PTR);
        m_pRowHeaderWindow->Update();
    }

    if (m_pColumnHeaderWindow)
    {
        m_pColumnHeaderWindow->Refresh(false, WX_VDV_NULL_PTR);
        m_pColumnHeaderWindow->Update();
    }

    if (m_pClientArea)
    {
        m_pClientArea->Refresh(false, WX_VDV_NULL_PTR);
        m_pClientArea->Update();
    }

    event.Skip();
}

//-------------------- COLUMNS --------------------------------------//


//---------------------- SELECTION ----------------------------------//


//----------------------- CURRENT ITEM ------------------------------//



//----------------- SELECTION COLOUR --------------------------------//

//---------- ROW HEIGHT AND COLUMN WIDTH ----------------------------//
/** Get the height of a row
  * \param uiViewRow [input]: the row index (view coordinate)
  * \return the row height
  */
wxCoord wxVirtualDataTableCtrl::GetRowHeight(size_t uiViewRow) const
{
    return(m_pClientArea->GetRowHeight(uiViewRow));
}

/** Get the column width
  * \param uiViewCol [input]: the column index (view coordinate)
  * \return the column height
  */
wxCoord wxVirtualDataTableCtrl::GetColumnWidth(size_t uiViewCol) const
{
    return(m_pClientArea->GetColumnWidth(uiViewCol));
}
