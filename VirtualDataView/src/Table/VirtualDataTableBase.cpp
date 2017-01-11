/**********************************************************************/
/** FILE    : wxVirtualDataTableBase.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a base class for viewing a very large amount of items  **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/Table/VirtualDataTableCtrl.h>
#include <wx/VirtualDataView/Table/VirtualDataTableBase.h>
#include <wx/VirtualDataView/Table/Models/VirtualITableTreeDataModel.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttrProvider.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/Mappers/VirtualDataViewCellMapper.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>

#ifndef WX_PRECOMP
    #include <wx/settings.h>
    #include <wx/dcclient.h>
    #include <wx/listbox.h>
#endif //WX_PRECOMP

#include <wx/dcbuffer.h>
#include <wx/selstore.h>
#include <wx/renderer.h>


//--------------------- STATIC DATA ---------------------------------//
static const size_t s_uiNotFound = (size_t) wxNOT_FOUND;
const char wxsVirtualDataTableBaseName[] = "wxVirtualDataTableBase";


//-------------- CONSTRUCTORS & DESTRUCTOR --------------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataTableBase, wxControl);

/** Default constructor. The method Create() must be called
  */
wxVirtualDataTableBase::wxVirtualDataTableBase(void)
    : wxSystemThemedControl<wxControl>(),
      m_pOwner(WX_VDV_NULL_PTR),
      m_pDataModel(WX_VDV_NULL_PTR),
      m_pRowMapper(WX_VDV_NULL_PTR),
      m_pColMapper(WX_VDV_NULL_PTR),
      m_iMarginX(0), m_iMarginY(0),
      m_iGridPenWidth(0)
{
    Init();
}

/** Constructor
  * Creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent [input]: parent of the window
  * \param id      [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos     [input]: initial position of the window. Default to wxDefaultPosition
  * \param size    [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle  [input]: window style. Default to 0
  * \param sName   [input]: the name of the object
  */
wxVirtualDataTableBase::wxVirtualDataTableBase(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                               const wxSize& size, long lStyle, wxString sName)

    : wxSystemThemedControl<wxControl>(),
      m_pOwner((wxVirtualDataTableCtrl*) pParent),
      m_pDataModel(WX_VDV_NULL_PTR),
      m_pRowMapper(WX_VDV_NULL_PTR),
      m_pColMapper(WX_VDV_NULL_PTR),
      m_iMarginX(0), m_iMarginY(0),
      m_iGridPenWidth(0)
{
    Init();
    Create(pParent, id, pos, size, lStyle, sName);
}

/** Destructor (virtual)
  */
wxVirtualDataTableBase::~wxVirtualDataTableBase(void)
{
    Release();
}

//------------------ INTERNAL METHODS -------------------------------//
/** Common parts of all constructors
  */
void wxVirtualDataTableBase::Init(void)
{
    m_pOwner            = WX_VDV_NULL_PTR;
    m_pDataModel        = WX_VDV_NULL_PTR;
    m_pRowMapper        = WX_VDV_NULL_PTR;
    m_pColMapper        = WX_VDV_NULL_PTR;
    m_iMarginX          = 0;
    m_iMarginY          = 0;
    m_iGridPenWidth     = 0;
}

/** Release internal resources
  */
void wxVirtualDataTableBase::Release(void)
{
    if (m_pDataModel) delete(m_pDataModel);
    m_pDataModel = WX_VDV_NULL_PTR;

    if (m_pRowMapper) delete(m_pRowMapper);
    m_pRowMapper = WX_VDV_NULL_PTR;

    if (m_pColMapper) delete(m_pColMapper);
    m_pColMapper = WX_VDV_NULL_PTR;
}

//------------------ CREATION METHODS -------------------------------//
/** Really creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * \param pParent [input]: parent of the window
  * \param id      [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos     [input]: initial position of the window. Default to wxDefaultPosition
  * \param size    [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle  [input]: window style. Default to 0
  * \param sName   [input]: the name of the object
  * \return true on success or false if the control couldn't be created
  */
bool wxVirtualDataTableBase::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                 const wxSize& size, long lStyle, wxString sName)
{
#ifdef __WXMSW__
    if ((lStyle & wxBORDER_MASK) == wxDEFAULT) lStyle |= wxBORDER_THEME;
#endif

    lStyle |= wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE;
    if (!(wxControl::Create(pParent, id, pos, size, lStyle, wxDefaultValidator, sName))) return(false);
    EnableSystemTheme();

    //make sure the native widget has the right colour since we do transparent drawing by default
    //SetBackgroundColour(GetBackgroundColour());
    SetBackgroundColour(wxNullColour);

    //leave m_colBgSel in an invalid state: it means for OnDrawBackground()
    //to use wxRendererNative instead of painting selection bg ourselves
    //m_colBgSel = wxNullColour;

    //Flicker-free drawing requires this
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    //event connections
    Bind(wxEVT_PAINT      , &wxVirtualDataTableBase::OnPaint         , this);
    Bind(wxEVT_KEY_DOWN   , &wxVirtualDataTableBase::OnKeyDown       , this);
    Bind(wxEVT_LEFT_DOWN  , &wxVirtualDataTableBase::OnLeftDown      , this);
    Bind(wxEVT_LEFT_DCLICK, &wxVirtualDataTableBase::OnLeftDClick    , this);
    Bind(wxEVT_SET_FOCUS  , &wxVirtualDataTableBase::OnSetOrKillFocus, this);
    Bind(wxEVT_KILL_FOCUS , &wxVirtualDataTableBase::OnSetOrKillFocus, this);
    return(true);
}

//----------------------- OWNER WINDOW ------------------------------//
/** Get the owner window
  * \return the owner window
  */
wxVirtualDataTableCtrl*  wxVirtualDataTableBase::GetOwner(void) const
{
    return(m_pOwner);
}

/** Set the owner window
  * \param pOwner [input]: the new owner window
  */
void wxVirtualDataTableBase::SetOwner(wxVirtualDataTableCtrl *pOwner)
{
    m_pOwner = pOwner;
}

//--------------------- WINDOWS MANAGEMENT --------------------------//
#ifdef __WXUNIVERSAL__
/** Check if the current window is the current one
  * \return true if the window is the current one, false otherwise
  */
bool wxVirtualDataTableBase::IsCurrent(void) const
{
    return(wxVScrolledWindow::IsCurrent());
}
#endif


//------------------------ VISUAL ATTRIBUTES ------------------------//
/** Get the default window attributes
  * \return the default visual attributes
  */
wxVisualAttributes wxVirtualDataTableBase::GetDefaultAttributes(void) const
{
    return(DoGetDefaultAttributes());
}

/** Get the default window attributes - static
  * \param variant [input] : what attribute to get. Default value = wxWINDOW_VARIANT_NORMAL
  * \return the visual attributes
  */
wxVisualAttributes wxVirtualDataTableBase::GetClassDefaultAttributes(wxWindowVariant variant)
{
    return(wxListBox::GetClassDefaultAttributes(variant));
}

/** Get the default window attributes
  * \return the default window attributes
  */
wxVisualAttributes wxVirtualDataTableBase::DoGetDefaultAttributes(void) const
{
    return(GetClassDefaultAttributes(GetWindowVariant()));
}

//------------------ DATA MODEL -------------------------------------//
/** Get the current data model used
  * \return the current data model used. Do not delete the pointer : it is still owned by the control
  */
wxVirtualITableTreeDataModel* wxVirtualDataTableBase::GetDataModel(void) const
{
    return(m_pDataModel);
}

/** Set the current data model
  * \param pModel [input] : the new data model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataTableBase::SetDataModel(wxVirtualITableTreeDataModel *pModel)
{
    if (m_pDataModel == pModel) return;
    if (m_pDataModel) delete(m_pDataModel);
    m_pDataModel = pModel;
}

//---------------------- CALLBACKS FOR DATA CHANGE ------------------//
/** The model has changed its amount of row
  */
void wxVirtualDataTableBase::OnModelRowCountChanged(void)
{
    if (m_pRowMapper) m_pRowMapper->OnModelChanged(m_pDataModel);
}

/** The model has changed its amount of columns
  */
void wxVirtualDataTableBase::OnModelColCountChanged(void)
{
    if (m_pColMapper) m_pColMapper->OnModelChanged(m_pDataModel);
}

//------------------ ROWS MAPPING -----------------------------------//
/** Get the row mapper
  * \return the row mapper.
  *         Do not delete it : it is still owned by the wxVirtualDataTableBase object
  */
wxVirtualDataViewCellMapper* wxVirtualDataTableBase::GetRowMapper(void) const
{
    return(m_pRowMapper);
}

/** Set the row mapper
  * \param pMapper [input]: the new row mapper. Ownership is taken
  *                         it must have been allocated with "new"
  */
void wxVirtualDataTableBase::SetRowMapper(wxVirtualDataViewCellMapper *pMapper)
{
    if (m_pRowMapper == pMapper) return;
    if (m_pRowMapper) delete(m_pRowMapper);
    m_pRowMapper = pMapper;
}

/** Convert a view row coord to a model row coord
  * \param uiViewRow [input]: the view row
  * \return the model row
  */
size_t wxVirtualDataTableBase::GetModelRow(size_t uiViewRow) const
{
    return(DoGetModelRow(uiViewRow));
}

/** Convert a view model coord to a view row coord
  * \param uiModelRow [input]: the model row
  * \return the view row
  */
size_t wxVirtualDataTableBase::GetViewRow(size_t uiModelRow) const
{
    return(DoGetViewRow(uiModelRow));
}

/** Convert a view row coord to a model row coord
  * \param uiViewRow [input]: the view row
  * \return the model row
  */
WX_VDV_INLINE size_t wxVirtualDataTableBase::DoGetModelRow(size_t uiViewRow) const
{
    if (!m_pRowMapper) return(uiViewRow);
    return(m_pRowMapper->GetModelCoord(uiViewRow));
}

/** Convert a view model coord to a view row coord
  * \param uiModelRow [input]: the model row
  * \return the view row
  */
WX_VDV_INLINE size_t wxVirtualDataTableBase::DoGetViewRow(size_t uiModelRow) const
{
    if (!m_pRowMapper) return(uiModelRow);
    return(m_pRowMapper->GetViewCoord(uiModelRow));
}

/** Get the amount of view rows
  * \return the amount of view rows
  */
size_t wxVirtualDataTableBase::GetAmountOfViewRows(void) const
{
    if (m_pRowMapper) return(m_pRowMapper->GetAmountOfViewCoords());
    if (m_pDataModel) return(m_pDataModel->GetRowsCount());
    return(1);
}

//------------------- COLUMNS MAPPING -------------------------------//
/** Get the column mapper
  * \return the column mapper.
  *         Do not delete it : it is still owned by the wxVirtualDataTableBase object
  */
wxVirtualDataViewCellMapper* wxVirtualDataTableBase::GetColMapper(void) const
{
    return(m_pColMapper);
}

/** Set the column mapper
  * \param pMapper [input]: the new column mapper. Ownership is taken
  *                         it must have been allocated with "new"
  */
void wxVirtualDataTableBase::SetColMapper(wxVirtualDataViewCellMapper *pMapper)
{
    if (m_pColMapper == pMapper) return;
    if (m_pColMapper) delete(m_pColMapper);
    m_pColMapper = pMapper;
}

/** Convert a view column coord to a model column coord
  * \param uiViewCol [input]: the view column
  * \return the model column
  */
size_t wxVirtualDataTableBase::GetModelCol(size_t uiViewCol) const
{
    return(DoGetModelCol(uiViewCol));
}

/** Convert a view model coord to a view column coord
  * \param uiModelCol [input]: the model column
  * \return the view column
  */
size_t wxVirtualDataTableBase::GetViewCol(size_t uiModelCol) const
{
    return(DoGetViewCol(uiModelCol));
}


/** Convert a view column coord to a model column coord
  * \param uiViewCol [input]: the view column
  * \return the model column
  */
WX_VDV_INLINE size_t wxVirtualDataTableBase::DoGetModelCol(size_t uiViewCol) const
{
    if (!m_pColMapper) return(uiViewCol);
    return(m_pColMapper->GetModelCoord(uiViewCol));
}

/** Convert a view model coord to a view column coord
  * \param uiModelCol [input]: the model column
  * \return the view column
  */
WX_VDV_INLINE size_t wxVirtualDataTableBase::DoGetViewCol(size_t uiModelCol) const
{
    if (!m_pColMapper) return(uiModelCol);
    return(m_pColMapper->GetViewCoord(uiModelCol));
}

/** Get the amount of view columns
  * \return the amount of view columns
  */
size_t wxVirtualDataTableBase::GetAmountOfViewCols(void) const
{
    if (m_pColMapper) return(m_pColMapper->GetAmountOfViewCoords());
    if (m_pDataModel) return(m_pDataModel->GetColumnsCount());
    return(1);
}

//------------------- INTERFACE : EVENT HANDLING --------------------//
/** Paint handler
  */
void wxVirtualDataTableBase::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc(this);
//    wxPaintDC dc(this);

    // fill it with background colour
    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    //if data model or attribute provider is invalid, simply quit.
    //only the background will be drawn
    if (!m_pDataModel) return;
    wxVirtualDataViewCellAttrProvider *pAttrProvider = m_pDataModel->GetAttributesProvider();
    if (!pAttrProvider) return;

    //default values
    wxVirtualDataViewItemState oDefaultState;
    wxVirtualDataViewItemAttr oBaseAttr;
    GetBaseItemAttr(oBaseAttr);

    //iterate over all visible cells
    wxHVScrolledWindow *pOwner = m_pOwner;
    if (!pOwner) return;
    size_t uiFirstRow = pOwner->GetVisibleRowsBegin();
    size_t uiLastRow  = pOwner->GetVisibleRowsEnd();
    size_t uiFirstCol = pOwner->GetVisibleColumnsBegin();
    size_t uiLastCol  = pOwner->GetVisibleColumnsEnd();
    size_t uiModelCol;
    size_t uiModelRow;
    size_t uiRow, uiCol;
    wxCoord x0, y0;

    wxCoord dx, dy;
    dx = m_iMarginX + m_iGridPenWidth;
    dy = m_iMarginY + m_iGridPenWidth;

    x0 = m_iMarginX;
    for(uiCol = uiFirstCol; uiCol < uiLastCol; uiCol++)
    {
        uiModelCol = DoGetModelCol(uiCol);
        wxCoord iColWidth = DoGetColumnWidth(uiModelCol);

        if (iColWidth <= 0)
        {
            //column is hidden
            continue;
        }

        y0 = m_iMarginY;
        for(uiRow = uiFirstRow; uiRow < uiLastRow; uiRow++)
        {
            //cell id
            wxVirtualItemID idCell = m_pDataModel->GetChild(uiRow, uiCol);

            //get cell size
            uiModelRow = DoGetModelRow(uiRow);
            wxCoord iRowHeight = DoGetRowHeight(uiModelRow);
            if (iRowHeight <= 0)
            {
                //row is hidden
                continue;
            }

            //cell rect
            wxRect rCellRect(x0 + m_iMarginX, y0 + m_iMarginY, iColWidth - dx, iRowHeight - dy);

            //cell value
            wxVariant vValue;
            wxVirtualDataViewItemAttr rAttr = pAttrProvider->GetEffectiveAttribute(uiModelRow, uiModelCol, oBaseAttr);
            if (m_pDataModel->GetValue(vValue, idCell))
            {
                wxVirtualDataViewRenderer *pRenderer = pAttrProvider->GetEffectiveRenderer(uiModelRow, uiModelCol);
                if (pRenderer)
                {
                    pRenderer->Render(this, rCellRect, dc,
                                      &rAttr, oDefaultState, vValue);
                }
                else
                {
                    //draw only background
                    wxVirtualDataViewRenderer::RenderBackground(this, rCellRect, dc, &rAttr);
                }
            }
            else
            {
                //draw background
                wxVirtualDataViewRenderer::RenderBackground(this, rCellRect, dc, &rAttr);
            }

            //next row
            y0 += iRowHeight;
        }

        //next column
        x0 += iColWidth;
    }
}

/** Get base item attributes
  * \param rBaseAttr [output]: the baseline for item graphic attributes
  */
void wxVirtualDataTableBase::GetBaseItemAttr(wxVirtualDataViewItemAttr &rBaseAttr) const
{
    rBaseAttr.SetBackgroundColour(GetBackgroundColour());
    rBaseAttr.SetColour(GetForegroundColour());
    rBaseAttr.SetFont(GetFont());
    rBaseAttr.SetBold(false);
    rBaseAttr.SetItalic(false);
}

/** Focus changed handler
  */
void wxVirtualDataTableBase::OnSetOrKillFocus(wxFocusEvent& WXUNUSED(event))
{
    // we need to repaint the selection when we get the focus since
    // wxRendererNative in general draws the focused selection differently
    // from the unfocused selection (see OnDrawItem):
    //RefreshSelected();
}

/** Key down event
  */
void wxVirtualDataTableBase::OnKeyDown(wxKeyEvent& event)
{
//    // flags for DoHandleItemClick()
//    int flags = ItemClick_Kbd;
//
//    size_t uiCurrent;
//    switch (event.GetKeyCode())
//    {
//        case WXK_HOME           :
//        case WXK_NUMPAD_HOME    :   uiCurrent = 0;
//                                    break;
//
//        case WXK_END            :
//        case WXK_NUMPAD_END     :   uiCurrent = GetRowCount() - 1;
//                                    break;
//
//        case WXK_DOWN           :
//        case WXK_NUMPAD_DOWN    :   if (m_uiCurrent ==  GetRowCount() - 1) return;
//                                    uiCurrent = m_uiCurrent + 1;
//                                    break;
//
//        case WXK_UP             :
//        case WXK_NUMPAD_UP      :        if (m_uiCurrent == s_uiNotFound)    uiCurrent = GetRowCount() - 1;
//                                    else if (m_uiCurrent != 0)              uiCurrent = m_uiCurrent - 1;
//                                    else                                    return; // m_uiCurrent == 0
//                                    break;
//
//        case WXK_PAGEDOWN       :
//        case WXK_NUMPAD_PAGEDOWN:   PageDown();
//                                    uiCurrent = GetVisibleRowsBegin();
//                                    break;
//
//        case WXK_PAGEUP         :
//        case WXK_NUMPAD_PAGEUP  :   if (m_uiCurrent == GetVisibleRowsBegin()) PageUp();
//                                    uiCurrent = GetVisibleRowsBegin();
//                                    break;
//
//        case WXK_SPACE          :   //hack: pressing space should work like a mouse click rather than
//                                    //like a keyboard arrow press, so trick DoHandleItemClick() in thinking we were clicked
//                                    flags &= ~ItemClick_Kbd;
//                                    uiCurrent = m_uiCurrent;
//                                    break;
//
//#ifdef __WXMSW__
//        case WXK_TAB            :   //Since we are using wxWANTS_CHARS we need to send navigation events for the tabs on MSW
//                                    HandleAsNavigationKey(event);
//                                    //fall through to default
//#endif
//
//        default                 :   event.Skip();
//                                    uiCurrent = 0; // just to silent the stupid compiler warnings
//                                    wxUnusedVar(uiCurrent);
//                                    return;
//    }
//
//    if (event.ShiftDown())      flags |= ItemClick_Shift;
//    if (event.ControlDown())    flags |= ItemClick_Ctrl;
//
//    DoHandleItemClick(uiCurrent, flags);
}


/** Mouse left mouse button down
  */
void wxVirtualDataTableBase::OnLeftDown(wxMouseEvent& event)
{
    SetFocus();

//    size_t uiItem = wxVarVScrollHelper::VirtualHitTest(event.GetPosition().y);
//
//    if (uiItem != s_uiNotFound )
//    {
//        int flags = 0;
//        if (event.ShiftDown())      flags |= ItemClick_Shift;
//        if (event.ControlDown())    flags |= ItemClick_Ctrl;
//
//        DoHandleItemClick(uiItem, flags);
//    }
}

/** Mouse double click
  */
void wxVirtualDataTableBase::OnLeftDClick(wxMouseEvent& eventMouse)
{
//    size_t uiItem = wxVarVScrollHelper::VirtualHitTest(eventMouse.GetPosition().y);
//    if (uiItem != s_uiNotFound )
//    {
//        //if uiItem double-clicked was not yet selected, then treat this event as a left-click instead
//        if (uiItem == m_uiCurrent)
//        {
//            wxCommandEvent event(wxEVT_LISTBOX_DCLICK, GetId());
//            InitEvent(event, uiItem);
//            GetEventHandler()->ProcessEvent(event);
//        }
//        else
//        {
//            OnLeftDown(eventMouse);
//        }
//    }
}

//---------- ROW HEIGHT AND COLUMN WIDTH ----------------------------//
/** Get the height of a row
  * \param uiViewRow [input]: the row index (view coordinate)
  * \return the row height
  */
wxCoord wxVirtualDataTableBase::GetRowHeight(size_t uiViewRow) const
{
    size_t uiModelRow = DoGetModelRow(uiViewRow);
    return(DoGetRowHeight(uiModelRow));
}

/** Get the column width
  * \param uiViewCol [input]: the column index (view coordinate)
  * \return the column height
  */
wxCoord wxVirtualDataTableBase::GetColumnWidth(size_t uiViewCol) const
{
    size_t uiModelCol = DoGetModelCol(uiViewCol);
    return(DoGetColumnWidth(uiModelCol));
}

/** Get the height of a row
  * \param uiModelRow [input]: the row index (model coordinate)
  * \return the row height
  */
WX_VDV_INLINE wxCoord wxVirtualDataTableBase::DoGetRowHeight(size_t uiModelRow) const
{
    return(20 + m_iMarginY + m_iGridPenWidth);
}

/** Get the column width
  * \param uiViewCol [input]: the column index (model coordinate)
  * \return the column height
  */
WX_VDV_INLINE wxCoord wxVirtualDataTableBase::DoGetColumnWidth(size_t uiViewCol) const
{
    return(100 + m_iMarginX + m_iGridPenWidth);
}
