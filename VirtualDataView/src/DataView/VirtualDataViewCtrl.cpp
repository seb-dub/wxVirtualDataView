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

#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewBase.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewColHeader.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewRowHeader.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCornerHeader.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/Models/VirtualSortingDataModel.h>
#include <wx/VirtualDataView/Models/VirtualFilteringDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/FilterEditors/VirtualDataViewIFilterEditor.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewFilter.h>

#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
//#include <wx/log.h>
#include <wx/stack.h>
#include <wx/hashset.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewCtrl, wxControl);
const char wxVirtualDataViewCtrlNameStr[] = "wxVirtualDataViewCtrl";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewCtrl::wxVirtualDataViewCtrl(void)
    : wxSystemThemedControl<wxControl>(),
      m_pClientArea(WX_VDV_NULL_PTR),
      m_pColumnHeaderWindow(WX_VDV_NULL_PTR),
      m_pRowHeaderWindow(WX_VDV_NULL_PTR),
      m_pCornerHeaderWindow(WX_VDV_NULL_PTR),
      m_pSortingModel(WX_VDV_NULL_PTR),
      m_pFilteringModel(WX_VDV_NULL_PTR)
{
    InitDataView();
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
wxVirtualDataViewCtrl::wxVirtualDataViewCtrl(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                             const wxSize& size, long lStyle,
                                             const wxValidator& validator, wxString sName)

    : wxSystemThemedControl<wxControl>(),
      m_pClientArea(WX_VDV_NULL_PTR),
      m_pColumnHeaderWindow(WX_VDV_NULL_PTR),
      m_pRowHeaderWindow(WX_VDV_NULL_PTR),
      m_pCornerHeaderWindow(WX_VDV_NULL_PTR),
      m_pSortingModel(WX_VDV_NULL_PTR),
      m_pFilteringModel(WX_VDV_NULL_PTR)
{
    InitDataView();
    Create(pParent, id, pos, size, lStyle, validator, sName);
}

/** Destructor (virtual)
  */
wxVirtualDataViewCtrl::~wxVirtualDataViewCtrl(void)
{

}

//---------------------- CREATION METHODS ---------------------------//
/** Common parts of all constructors
  */
void wxVirtualDataViewCtrl::InitDataView(void)
{
    m_pClientArea         = WX_VDV_NULL_PTR;
    m_pColumnHeaderWindow = WX_VDV_NULL_PTR;
    m_pRowHeaderWindow    = WX_VDV_NULL_PTR;
    m_pCornerHeaderWindow = WX_VDV_NULL_PTR;
    m_pSortingModel       = WX_VDV_NULL_PTR;
    m_pFilteringModel     = WX_VDV_NULL_PTR;
}

/** Really creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent        [input]: parent of the window
  * \param id             [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos            [input]: initial position of the window. Default to wxDefaultPosition
  * \param size           [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle         [input]: window style. Default to 0
  * \param validator      [input]: validator
  * \param sName          [input]: the name of the object
  * \param pDataModel     [input]: the default data model. Can be NULL (the user needs to provide it later using SetDataModel)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pStateModel    [input]: the default state model. Can be NULL (a wxVirtualDefaultStateModel is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pModelRenderer [input]: the default state model. Can be NULL (a wxVirtualTreeModelRenderer is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \return true on success or false if the control couldn't be created
  */
bool wxVirtualDataViewCtrl::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                    const wxSize& size, long lStyle,
                                    const wxValidator& validator, wxString sName,
                                    wxVirtualIDataModel *pDataModel,
                                    wxVirtualIStateModel *pStateModel,
                                    wxVirtualIModelRenderer *pModelRenderer)
{
    InitDataView();

    //wxWidgets initialization
    if (!wxControl::Create(pParent, id, pos, size,
                           lStyle | wxHSCROLL | wxVSCROLL, validator, sName))
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
    m_pClientArea = new wxVirtualDataViewBase(this, wxID_ANY, pos, size, lStyleMain,
                                              pDataModel, pStateModel, pModelRenderer);
    if (m_pClientArea)
    {
        m_pClientArea->SetOwner(this);
        wxVirtualIModelRenderer *pModelRenderer = m_pClientArea->GetModelRenderer();
        if (pModelRenderer) pModelRenderer->SetScrollerWindow(this);
    }

    //default style for row/column header
    long lHeaderStyle = wxVirtualHeaderCtrl::WX_VDV_DEFAULT_STYLE | wxBORDER_NONE;

    //column header
    m_pColumnHeaderWindow = new wxVirtualDataViewColHeader(this, wxID_ANY,
                                                           wxDefaultPosition, wxDefaultSize,
                                                           lHeaderStyle);
    m_pColumnHeaderWindow->SetOwner(this);


    //row header
    lHeaderStyle = wxBORDER_NONE; //no reordering, no hiding, no filtering
    m_pRowHeaderWindow = new wxVirtualDataViewRowHeader(this, wxID_ANY,
                                                        wxDefaultPosition, wxDefaultSize,
                                                        lHeaderStyle);
    m_pRowHeaderWindow->SetOwner(this);


    //corner header
    if ((m_pColumnHeaderWindow) && (m_pRowHeaderWindow))
    {
        m_pCornerHeaderWindow = new wxVirtualDataViewCornerHeader(this, wxID_ANY,
                                                                  wxDefaultPosition, wxDefaultSize,
                                                                  wxBORDER_NONE);
    }
    else
    {
        m_pCornerHeaderWindow = WX_VDV_NULL_PTR;
    }

	//scrollbars
    Bind(wxEVT_SIZE                  , &wxVirtualDataViewCtrl::OnSize       , this);
    Bind(wxEVT_SCROLLWIN_TOP         , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_BOTTOM      , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_LINEUP      , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_LINEDOWN    , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_PAGEUP      , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_PAGEDOWN    , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK  , &wxVirtualDataViewCtrl::OnScrollEvent, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &wxVirtualDataViewCtrl::OnScrollEvent, this);

    //column header
    if (m_pColumnHeaderWindow)
    {
        //resizing
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_BEGIN_RESIZE,
                                    &wxVirtualDataViewCtrl::OnColHeaderBeginSize, this);
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_RESIZING,
                                    &wxVirtualDataViewCtrl::OnColHeaderSizing, this);
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_END_RESIZE,
                                    &wxVirtualDataViewCtrl::OnColHeaderSize, this);


        //auto resize
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_SEPARATOR_LEFT_DCLICK,
                                    &wxVirtualDataViewCtrl::OnColHeaderAutoSize, this);

        //reordering
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_BEGIN_DRAGGING,
                                    &wxVirtualDataViewCtrl::OnColHeaderStartReorder, this);
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_END_DRAGGING,
                                    &wxVirtualDataViewCtrl::OnColHeaderReorder, this);

        //sorting
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_LEFT_UP,
                                    &wxVirtualDataViewCtrl::OnColHeaderSort, this);

        //layout
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_LAYOUT_CHANGED,
                                    &wxVirtualDataViewCtrl::OnColHeaderLayout, this);

        //filtering
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_BEGIN_FILTER_EDIT,
                                    &wxVirtualDataViewCtrl::OnColHeaderFilterBegin, this);
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_FILTER_CHANGED,
                                    &wxVirtualDataViewCtrl::OnColHeaderFilterChanged, this);
        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_END_FILTER_EDIT,
                                    &wxVirtualDataViewCtrl::OnColHeaderFilterEnd, this);

        //forward other events
//        m_pColumnHeaderWindow->Bind(wxEVT_VIRTUAL_HEADER_SEPARATOR_LEFT_DCLICK,
//                                    &wxVirtualDataViewCtrl::OnColHeaderEvent, this);
    }

    return(true);
}

//--------------------- MAIN WINDOW ---------------------------------//
/** Get the main window
  * \return the main window
  */
wxVirtualDataViewBase* wxVirtualDataViewCtrl::GetMainWindow(void) const
{
    return(m_pClientArea);
}

//--------------------- HEADERS -------------------------------------//
/** Get the column header window
  * \return the column header window
  */
wxVirtualDataViewColHeader* wxVirtualDataViewCtrl::GetColumnHeaderWindow(void) const
{
    return(m_pColumnHeaderWindow);
}

/** Get the row header window
  * \return the row header window
  */
wxVirtualDataViewRowHeader* wxVirtualDataViewCtrl::GetRowHeaderWindow(void) const
{
    return(m_pRowHeaderWindow);
}

/** Get the corner header window
  * \return the corner header window
  */
wxVirtualDataViewCornerHeader* wxVirtualDataViewCtrl::GetCornerHeaderWindow(void) const
{
    return(m_pCornerHeaderWindow);
}

/** Show or hide the row header
  * \param bShow [input]: true for showing the header, false for hiding it
  */
void wxVirtualDataViewCtrl::ShowRowHeader(bool bShow)
{
    if (m_pRowHeaderWindow) m_pRowHeaderWindow->Show(bShow);

    if (!m_pCornerHeaderWindow) return;
    if ((IsRowHeaderShown()) && (IsColHeaderShown()))
    {
        m_pCornerHeaderWindow->Show(true);
    }
    else
    {
        m_pCornerHeaderWindow->Show(false);
    }
    ResizeChildren();
}

/** Hide the row header
  */
void wxVirtualDataViewCtrl::HideRowHeader(void)
{
    ShowRowHeader(false);
}

/** Check if the row header is shown
  * \return true if the row header is shown, false otherwise
  */
bool wxVirtualDataViewCtrl::IsRowHeaderShown(void) const
{
    if (!m_pRowHeaderWindow) return(false);
    return(m_pRowHeaderWindow->IsShown());
}

/** Show or hide the column header
  * \param bShow [input]: true for showing the header, false for hiding it
  */
void wxVirtualDataViewCtrl::ShowColHeader(bool bShow)
{
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->Show(bShow);

    if (!m_pCornerHeaderWindow) return;
    if ((IsRowHeaderShown()) && (IsColHeaderShown()))
    {
        m_pCornerHeaderWindow->Show(true);
    }
    else
    {
        m_pCornerHeaderWindow->Show(false);
    }
    ResizeChildren();
}

/** Hide the column header
  */
void wxVirtualDataViewCtrl::HideColHeader(void)
{
    ShowColHeader(false);
}

/** Check if the column header is shown
  * \return true if the column header is shown, false otherwise
  */
bool wxVirtualDataViewCtrl::IsColHeaderShown(void) const
{
    if (!m_pColumnHeaderWindow) return(false);
    return(m_pColumnHeaderWindow->IsShown());
}


//--------------- WINDOWS MANAGEMENT --------------------------------//
#ifdef __WXUNIVERSAL__
/** Check if the current window is the current one
  * \return true if the window is the current one, false otherwise
  */
bool wxVirtualDataViewCtrl::IsCurrent(void) const
{
    return(wxControl::IsCurrent());
}
#endif

//------------------- VISUAL ATTRIBUTES -----------------------------//
/** Get the default window attributes
  * \return the default visual attributes
  */
wxVisualAttributes wxVirtualDataViewCtrl::GetDefaultAttributes(void) const
{
    if (m_pClientArea) return(m_pClientArea->GetDefaultAttributes());
    return(wxVirtualDataViewCtrl::GetClassDefaultAttributes());
}

/** Get the default window attributes - static
  * \param variant [input] : what attribute to get. Default value = wxWINDOW_VARIANT_NORMAL
  * \return the visual attributes
  */
wxVisualAttributes wxVirtualDataViewCtrl::GetClassDefaultAttributes(wxWindowVariant variant)
{
    return(wxVirtualDataViewBase::GetClassDefaultAttributes(variant));
}

//------------------ DATA MODEL -------------------------------------//
/** Get the current data model used, without any proxies
  * \return the current data model without all the proxies filters
  */
wxVirtualIDataModel* wxVirtualDataViewCtrl::GetBaseDataModel(void) const
{
    if (!m_pClientArea) return(WX_VDV_NULL_PTR);
    return(m_pClientArea->GetBaseDataModel());
}

/** Get the current data model used, with all proxies if any
  * \return the current data model used.
  *         Do not delete the pointer : it is still owned by the control
  */
wxVirtualIDataModel* wxVirtualDataViewCtrl::GetDataModel(void) const
{
    if (!m_pClientArea) return(WX_VDV_NULL_PTR);
    return(m_pClientArea->GetDataModel());
}

/** Set the current data model. Current proxies will be kept
  * \param pModel [input] : the new data model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCtrl::SetDataModel(wxVirtualIDataModel *pModel)
{
    if (m_pClientArea) m_pClientArea->SetDataModel(pModel);
}

//--------------------- PROXY DATA MODELS ---------------------------//
/** Attach a proxy data model to the chain of models
  * \param pModel [input]: the proxy model to attach. Ownership is taken
  *                        It must have been allocated with "new"
  */
void wxVirtualDataViewCtrl::AttachProxyModel(wxVirtualIProxyDataModel *pModel)
{
    if (m_pClientArea) m_pClientArea->AttachProxyModel(pModel);
    else if (pModel) delete(pModel);
}

/** Remove the last proxy data model from the chain of models
  */
void wxVirtualDataViewCtrl::PopProxyModel(void)
{
    if (m_pClientArea)
    {
        wxVirtualIDataModel *pCurrentModel = m_pClientArea->GetDataModel();
        if (m_pSortingModel   == pCurrentModel) m_pSortingModel = WX_VDV_NULL_PTR;
        if (m_pFilteringModel == pCurrentModel) m_pFilteringModel = WX_VDV_NULL_PTR;
        m_pClientArea->PopProxyModel();
    }
}

/** Remove a proxy data model from the chain of models
  * \param pModel [input]: the model to remove. It will be also deleted
  */
void wxVirtualDataViewCtrl::DetachProxyModel(wxVirtualIProxyDataModel *pModel)
{
    if (m_pClientArea) m_pClientArea->DetachProxyModel(pModel);
    else if (pModel) delete(pModel);


    if (m_pSortingModel   == pModel) m_pSortingModel = WX_VDV_NULL_PTR;
    if (m_pFilteringModel == pModel) m_pFilteringModel = WX_VDV_NULL_PTR;
}

/** Remove all proxy data models from the chain of models
  */
void wxVirtualDataViewCtrl::DetachAllProxyModels(void)
{
    if (m_pClientArea) m_pClientArea->DetachAllProxyModels();
    m_pSortingModel = WX_VDV_NULL_PTR;
    m_pFilteringModel = WX_VDV_NULL_PTR;
}

//--------------------- STATE MODEL ---------------------------------//
/** Get the state model
  * \return the current data model used.
  *         Do not delete the pointer : it is still owned by the control
  */
wxVirtualIStateModel* wxVirtualDataViewCtrl::GetStateModel(void) const
{
    if (!m_pClientArea) return(WX_VDV_NULL_PTR);
    return(m_pClientArea->GetStateModel());
}

/** Set the state model
  * \param pStateModel [input] : the new state model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCtrl::SetStateModel(wxVirtualIStateModel *pStateModel)
{
    if (m_pClientArea) m_pClientArea->SetStateModel(pStateModel);
}

//---------------------- MODEL RENDERER -----------------------------//
/** Get the model renderer
  * \return the model renderer
  *          Do not delete the pointer : it is still owned by the control
  */
wxVirtualIModelRenderer* wxVirtualDataViewCtrl::GetModelRenderer(void) const
{
    if (!m_pClientArea) return(WX_VDV_NULL_PTR);
    return(m_pClientArea->GetModelRenderer());
}

/** Set the model renderer
  * \param pModelRenderer [input] : the new model renderer to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCtrl::SetModelRenderer(wxVirtualIModelRenderer *pModelRenderer)
{
    if (m_pClientArea) m_pClientArea->SetModelRenderer(pModelRenderer);
    if (pModelRenderer)
    {
        pModelRenderer->SetClientWindow(m_pClientArea);
        pModelRenderer->SetScrollerWindow(this);
    }
}

//---------------------- CALLBACKS FOR DATA CHANGE ------------------//
/** The model has changed its amount of row
  */
void wxVirtualDataViewCtrl::OnModelItemCountChanged(void)
{
    if (m_pClientArea) m_pClientArea->OnDataModelChanged();
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->SetOwner(this);
    if (m_pRowHeaderWindow)    m_pRowHeaderWindow->SetOwner(this);
    if (m_pCornerHeaderWindow) m_pCornerHeaderWindow->SetOwner(this);

    RefreshHeaders();
    if (m_pClientArea)
    {
        m_pClientArea->Refresh(true, WX_VDV_NULL_PTR);
        m_pClientArea->Update();
    }
}

/** Called by the client to notify that the data model has changed
  * This happens when the data model has changed (called automatically by SetDataModel)
  * or when the data inside the model has changed
  */
void wxVirtualDataViewCtrl::OnDataModelChanged(void)
{
    if (m_pClientArea) m_pClientArea->OnDataModelChanged();
    OnModelItemCountChanged();
}

/** Called by the client to notify changes in layout
  */
void wxVirtualDataViewCtrl::OnLayoutChanged(void)
{
    if (m_pClientArea) m_pClientArea->OnLayoutChanged();
    OnModelItemCountChanged();
}

//----------------------- COLUMNS -----------------------------------//
/** Get the list of columns
  * \return a reference to the list of columns
  */
wxVirtualDataViewColumnsList& wxVirtualDataViewCtrl::Columns(void)
{
    return(m_vColumns);
}

/** Get the list of columns - const version
  * \return a reference to the list of columns
  */
const wxVirtualDataViewColumnsList& wxVirtualDataViewCtrl::Columns(void) const
{
    return(m_vColumns);
}

/** Get column width
  * \param uiColumn [input]: the column index
  * \return the width of the column
  */
int wxVirtualDataViewCtrl::GetColumnWidth(size_t uiColumn) const
{
    return(m_vColumns.GetColumnWidth(uiColumn));
}

/** Get column minimal width
  * \param uiColumn [input]: the column index
  * \return the minimal width of the column
  */
int wxVirtualDataViewCtrl::GetColumnMinWidth(size_t uiColumn) const
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) return(pCol->GetMinWidth());
    return(-1);
}

/** Get column start offset
  * \param uiColumn [input]: the column index
  * \return the column start offset
  */
int wxVirtualDataViewCtrl::GetColumnStartOffset(size_t uiColumn) const
{
    return(m_vColumns.GetColumnStart(uiColumn));
}

/** Set column width
  * \param uiColumn [input]: the column index
  * \param iWidth   [input]: the new width
  */
void wxVirtualDataViewCtrl::SetColumnWidth(size_t uiColumn, int iWidth)
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) pCol->SetWidth(iWidth);
}

/** Set column minimal width
  * \param uiColumn     [input]: the column index
  * \param iMinWidth    [input]: the new minimal width
  */
void wxVirtualDataViewCtrl::SetColumnMinWidth(size_t uiColumn, int iMinWidth)
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) pCol->SetMinWidth(iMinWidth);
}

/** Resize the column so it fits its content
  * \param uiColumn     [input]: the column index
  */
void wxVirtualDataViewCtrl::AutoSizeColumn(size_t uiColumn)
{
    //get model renderer
    if (!m_pClientArea) return;
    wxVirtualIModelRenderer *pModelRenderer = m_pClientArea->GetModelRenderer();
    if (!pModelRenderer) return;

    //get column
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (!pCol) return;

    //compute best size
    int iNewSize = pModelRenderer->GetBestSize(uiColumn).GetWidth();
    //wxLogMessage("new size = %d", iNewSize);

    //clamp new size to minimal size
    if (iNewSize < pCol->GetMinWidth()) iNewSize = pCol->GetMinWidth();

    //resize column
    pCol->SetWidth(iNewSize);

    //adjust scrollbars
    wxSize sVirtualSize = m_pClientArea->GetVirtualSize();
    sVirtualSize.SetWidth(m_vColumns.GetTotalWidth());
    m_pClientArea->SetVirtualSize(sVirtualSize);
    m_pClientArea->SendSizeEvent();

    //redraw
    Refresh(true, WX_VDV_NULL_PTR);
    Update();
}

/** Resize all the columns so it fits the content
  */
void wxVirtualDataViewCtrl::AutoSizeAllColumns(void)
{
    //get model renderer
    if (!m_pClientArea) return;
    wxVirtualIModelRenderer *pModelRenderer = m_pClientArea->GetModelRenderer();
    if (!pModelRenderer) return;

    //resize the columns
    size_t uiNbCols = m_vColumns.GetColumnsCount();
    size_t uiCol;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        //get column
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) continue;

        //compute best size
        int iNewSize = pModelRenderer->GetBestSize(uiCol).GetWidth();

        //clamp new size to minimal size
        if (iNewSize < pCol->GetMinWidth()) iNewSize = pCol->GetMinWidth();

        //resize column
        pCol->SetWidth(iNewSize);
    }

    //adjust scrollbars
    wxSize sVirtualSize = m_pClientArea->GetVirtualSize();
    sVirtualSize.SetWidth(m_vColumns.GetTotalWidth());
    m_pClientArea->SetVirtualSize(sVirtualSize);
    m_pClientArea->SendSizeEvent();

    //redraw
    Refresh(true, WX_VDV_NULL_PTR);
    Update();
}

/** Show / hide a column
  * \param uiColumn [input]: the column index
  * \param bShow    [input]: true for showing the column, false for hiding it
  */
void wxVirtualDataViewCtrl::ShowColumn(size_t uiColumn, bool bShow)
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) pCol->Show(bShow);
}

/** Hide / show a column
  * \param uiColumn [input]: the column index
  * \param bHide    [input]: true for hiding the column, false for showing it
  */
void wxVirtualDataViewCtrl::HideColumn(size_t uiColumn, bool bHide)
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) pCol->Hide(bHide);
}

/** Hide / show a column (toggle state)
  * \param uiColumn [input]: the column index
  */
void wxVirtualDataViewCtrl::ToggleHideShowColumn(size_t uiColumn)
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) pCol->ToggleHideShow();
}

/** Check if a column is shown
  * \param uiColumn [input]: the column index
  * \return true if the column is shown, false it is hidden
  */
bool wxVirtualDataViewCtrl::IsColumnShown(size_t uiColumn) const
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) return(pCol->IsShown());
    return(false);
}

/** Check if a column is hidden
  * \param uiColumn [input]: the column index
  * \return true if the column is hidden, false it is shown
  */
bool wxVirtualDataViewCtrl::IsColumnHidden(size_t uiColumn) const
{
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumn);
    if (pCol) return(pCol->IsHidden());
    return(false);
}

//------------------ FOCUSED COLUMN ---------------------------------//
/** Refresh the headers windows
  */
void wxVirtualDataViewCtrl::RefreshHeaders(void)
{
    if (m_pColumnHeaderWindow)
    {
        m_pColumnHeaderWindow->Refresh(false, NULL);
        m_pColumnHeaderWindow->Update();
    }

    if (m_pRowHeaderWindow)
    {
        m_pRowHeaderWindow->Refresh(false, NULL);
        m_pRowHeaderWindow->Update();
    }

    if (m_pCornerHeaderWindow)
    {
        m_pCornerHeaderWindow->Refresh(false, NULL);
        m_pCornerHeaderWindow->Update();
    }
}

/** Refresh the data view only
  */
void wxVirtualDataViewCtrl::RefreshDataView(void)
{
    if (m_pClientArea)
    {
        m_pClientArea->Refresh(false, NULL);
        m_pClientArea->Update();
    }
}

/** Refresh the headers and the data view
  */
void wxVirtualDataViewCtrl::RefreshAll(void)
{
    Refresh(false, NULL);
    Update();
}


//------------------------- EVENTS ----------------------------------//
/** Send an event
  * \param rEvent [input]: the event to send
  * \return true if the event was processed and not skipped
  *         false it the event should continue to be processed
  */
bool wxVirtualDataViewCtrl::SendEvent(wxVirtualDataViewEvent &rEvent)
{
    rEvent.SetEventObject(this);
    rEvent.SetId(GetId());

    bool bResult = GetEventHandler()->ProcessEvent(rEvent); //true if processed and not skipped
    return(bResult);
}

//------------------------ EVENTS HANDLERS --------------------------//
/** Sizing event handlers
  */
void wxVirtualDataViewCtrl::OnSize(wxSizeEvent &rEvent)
{
    //since setting the scrollbars will make them appearing or disappearing,
    //it will change the client size of the window.
    //So we need a small loop here, and we quit when the client size has not changed
    //example:
    //start       : no scrollbar shown, expand of 1 item is done
    //iteration 1 : set the scrollbar normally. Vertical scrollbar appears, client width reduced
    //iteration 2 : now we need an horizontal scrollbar. horizontal scrollbar appears, client height reduced
    //iteration 3 : vertical scrollbar changes, client size does not change, end of loop
    int i;
    int iMax = 100; //3 should be sufficient, but to be safe
    for(i=0;i<iMax;i++)
    {
        wxSize sOldSize = GetClientSize();
        ResizeChildren(sOldSize);
        wxSize sNewSize = GetClientSize();

        if (sOldSize == sNewSize) break;
    }
}

/** Scrolling event handler
  */
void wxVirtualDataViewCtrl::OnScrollEvent(wxScrollWinEvent &rEvent)
{
//    wxScrollWinEvent *pEvent = new wxScrollWinEvent(event);
//    QueueEvent(pEvent);

    //process scroll event and redraw main client area
    if (m_pClientArea)
    {
        wxVirtualIModelRenderer *pModelRenderer = m_pClientArea->GetModelRenderer();
        if (pModelRenderer) pModelRenderer->ScrollEvent(rEvent);
    }

    RefreshAll();
    rEvent.Skip();
}

//---------------------- COLUMN RESIZING ----------------------------//
/** Start resizing a column
  * \param uiCol     [input]: the impacted column
  * \param iNewWidth [input]: the new column width
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::StartResizingColumn(size_t uiCol, int iNewWidth)
{
    m_pClientArea->CaptureMouse();
    return(true);
}

/** Currently resizing a column
  * \param uiCol     [input]: the impacted column
  * \param iNewWidth [input]: the new column width
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::ResizingColumn(size_t uiCol, int iNewWidth)
{
    //clamp new size to minimal size
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
    if (!pCol) return(false);
    if (iNewWidth < pCol->GetMinWidth()) iNewWidth = pCol->GetMinWidth();

    //compute the new column x position
    int x = 0;
    size_t i;
    for(i=0;i<uiCol;i++)
    {
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(i);
        if (!pCol) continue;
        x += pCol->GetWidth();
    }
    x += iNewWidth;

    //adjust with horizontal scrollbar
    x -= GetScrollPos(wxHORIZONTAL);

    //draw a black line
    wxPen oBlackPen(wxColour(0, 0, 0), 3, wxPENSTYLE_SOLID);
    if (m_pClientArea)
    {
        //redraw client area
        wxClientDC dc(m_pClientArea);
        wxBufferedDC bdc(&dc);

        m_pClientArea->UpdateDisplay(bdc);

        //now draw the sizing line
        int w, h;
        m_pClientArea->GetClientSize(&w, &h);

        //too light - barely visible on white
        //wxRendererNative::Get().DrawSplitterSash(m_pClientArea, bdc, wxSize(w, h), x, wxVERTICAL);
        wxRasterOperationMode eSavedROP = bdc.GetLogicalFunction();
        bdc.SetLogicalFunction(wxINVERT);
        bdc.SetPen(oBlackPen);
        bdc.DrawLine(x, 0, x, h);
        bdc.SetLogicalFunction(eSavedROP);
    }

    if (m_pColumnHeaderWindow)
    {
        //redraw client area
        m_pColumnHeaderWindow->Refresh(true, WX_VDV_NULL_PTR);
        m_pColumnHeaderWindow->Update();

        wxClientDC dc(m_pColumnHeaderWindow);
        wxBufferedDC bdc(&dc);

        //now draw the sizing line
        int w, h;
        m_pColumnHeaderWindow->GetClientSize(&w, &h);

        wxRasterOperationMode eSavedROP = bdc.GetLogicalFunction();
        bdc.SetLogicalFunction(wxINVERT);
        bdc.SetPen(oBlackPen);
        bdc.DrawLine(x, 0, x, h);
        bdc.SetLogicalFunction(eSavedROP);
    }

    return(true);
}

/** Finish resizing a column
  * \param uiCol     [input]: the impacted column
  * \param iNewWidth [input]: the new column width
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::ResizeColumn(size_t uiCol, int iNewWidth)
{
    m_pClientArea->ReleaseMouse();

    //clamp new size to minimal size
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
    if (!pCol) return(false);
    if (iNewWidth < pCol->GetMinWidth()) iNewWidth = pCol->GetMinWidth();

    //resize column
    pCol->SetWidth(iNewWidth);

    //adjust scrollbars
    if (m_pClientArea)
    {
        wxSize sVirtualSize = m_pClientArea->GetVirtualSize();
        sVirtualSize.SetWidth(m_vColumns.GetTotalWidth());
        m_pClientArea->SetVirtualSize(sVirtualSize);
        m_pClientArea->SendSizeEvent();
    }

    //redraw
    Refresh(true, WX_VDV_NULL_PTR);
    Update();
    return(true);
}


//----- COLUMN HEADER EVENT HANDLERS --------------------------------//
/** A column header is starting to be resized
  */
void wxVirtualDataViewCtrl::OnColHeaderBeginSize(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiColumnID = rEvent.GetItemID();
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumnID);
    if (!pCol)
    {
        rEvent.Skip();
        return;
    }
    if (!pCol->CanResize()) rEvent.Veto();
    else                    rEvent.Skip();
}

/** A column header is resized
  */
void wxVirtualDataViewCtrl::OnColHeaderSize(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiColumnID = rEvent.GetItemID();
    int iNewSize = rEvent.GetSize();
    //wxLogMessage("new size = %d", iNewSize);

    if (!ResizeColumn(uiColumnID, iNewSize)) rEvent.Skip();
}

/** A column header is currently resizing itself
  */
void wxVirtualDataViewCtrl::OnColHeaderSizing(wxVirtualHeaderCtrlEvent &rEvent)
{
    //compute the new column x position
    size_t uiColumnID = rEvent.GetItemID();
    int iNewSize = rEvent.GetSize();

    if (!ResizingColumn(uiColumnID, iNewSize)) rEvent.Skip();
}

/** A column header separator has been double clicked
  */
void wxVirtualDataViewCtrl::OnColHeaderAutoSize(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiColumnID = rEvent.GetItemID();
    AutoSizeColumn(uiColumnID);
}


/** A column header is starting to be dragged
  */
void wxVirtualDataViewCtrl::OnColHeaderStartReorder(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiColumnID = rEvent.GetItemID();
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumnID);
    if (!pCol)
    {
        rEvent.Skip();
        return;
    }
    if (!pCol->CanReorder()) rEvent.Veto();
    else                     rEvent.Skip();
}

/** A column header has been dragged
  */
void wxVirtualDataViewCtrl::OnColHeaderReorder(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiDraggedCol = rEvent.GetItemID();
    size_t uiNewColPos  = rEvent.GetNewOrder();

    if (m_pClientArea) m_pClientArea->FinishEdition(true);
    m_vColumns.MoveColumn(uiDraggedCol, uiNewColPos);
    if (m_pClientArea) m_pClientArea->UpdateDisplay();

    //send event
    wxVirtualDataViewEvent e(wxEVT_VDV_COLUMN_REORDERED);
    SendEvent(e);
}

/** A column header has been left click
  */
void wxVirtualDataViewCtrl::OnColHeaderSort(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiColumnID = rEvent.GetItemID();
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumnID);
    if (!pCol)
    {
        rEvent.Skip();
        return;
    }
    if (!pCol->IsSortable())
    {
        rEvent.Veto();
        return;
    }

    //toggle state :
    //not sorting -> sort ascending -> sort descending -> not sorting -> ...
    if (pCol->IsSortKey())
    {
        bool bSortAscending = pCol->IsSortOrderAscending();
        if (bSortAscending) pCol->SetSortOrder(false); //sort descending
        else                pCol->SetSortKey(false);   //not sorting
    }
    else
    {
        //sort ascending
        pCol->SetSortKey(true);
        pCol->SetSortOrder(true);
    }

    //sort
    SortItems();

    //send event
    wxVirtualDataViewEvent e(wxEVT_VDV_COLUMN_SORTED);
    e.SetColumnIndex(uiColumnID);
    e.SetColumn(pCol);
    SendEvent(e);
}

/** A column header layout changed event has occurred
  */
void wxVirtualDataViewCtrl::OnColHeaderLayout(wxVirtualHeaderCtrlEvent &rEvent)
{
    ResizeChildren();
}

/** A column header filter begin event has occurred
  */
void wxVirtualDataViewCtrl::OnColHeaderFilterBegin(wxVirtualHeaderCtrlEvent &rEvent)
{
    RefreshAll();

    if (!m_pColumnHeaderWindow) return;
    size_t uiColID = rEvent.GetItemID();
    wxVirtualDataViewIFilterEditor *pFilterEditor = m_pColumnHeaderWindow->GetFilterEditor(uiColID);
    if (!pFilterEditor) return;

    //get column & model field
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(uiColID);
    if (!pCol) return;
    size_t uiField = pCol->GetModelColumn();

    //initialize filter: get a list of values
    wxVirtualIDataModel  *pDataModel = GetDataModel();  //filtered model, so other filters are applied
    if (!pDataModel) return;

    wxVirtualIStateModel *pStateModel; // = GetStateModel();
    pStateModel = WX_VDV_NULL_PTR; //get all values;

    pFilterEditor->InitFilter(pDataModel, uiField, pStateModel);
}

/** A column header filter changed event has occurred
  */
void wxVirtualDataViewCtrl::OnColHeaderFilterChanged(wxVirtualHeaderCtrlEvent &rEvent)
{
    //filter value changed : reapply filters
    ApplyFilters();
}

/** A column header filter end event has occurred
  */
void wxVirtualDataViewCtrl::OnColHeaderFilterEnd(wxVirtualHeaderCtrlEvent &rEvent)
{
    //filter edition ended : reapply filters
    ApplyFilters();
}


/** A column header event has occurred
  */
void wxVirtualDataViewCtrl::OnColHeaderEvent(wxVirtualHeaderCtrlEvent &rEvent)
{
    size_t uiColumnID = rEvent.GetItemID();
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiColumnID);
    if (!pCol)
    {
        rEvent.Skip();
        return;
    }

    //convert the event
    int iType = 0;
    wxEventType eType = rEvent.GetEventType();
    if (eType == wxEVT_VIRTUAL_HEADER_LEFT_DOWN)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_LEFT_DOWN;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_MIDDLE_DOWN)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_MIDDLE_DOWN;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_RIGHT_DOWN)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_RIGHT_DOWN;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_LEFT_UP)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_LEFT_UP;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_MIDDLE_UP)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_MIDDLE_UP;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_RIGHT_UP)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_RIGHT_UP;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_LEFT_DCLICK)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_LEFT_DCLICK;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_MIDDLE_DCLICK)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_MIDDLE_DCLICK;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_RIGHT_DCLICK)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_RIGHT_DCLICK;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_SEPARATOR_LEFT_DCLICK)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_SEPARATOR_LEFT_DCLICK;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_SEPARATOR_MIDDLE_DCLICK)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_SEPARATOR_MIDDLE_DCLICK;
    }
    else if (eType == wxEVT_VIRTUAL_HEADER_SEPARATOR_RIGHT_DCLICK)
    {
        iType = wxEVT_VDV_COLUMN_HEADER_SEPARATOR_RIGHT_DCLICK;
    }
    else
    {
        rEvent.Skip();
        return;
    }

    //send the event
    wxVirtualDataViewEvent e(iType);
    e.SetColumnIndex(uiColumnID);
    e.SetColumn(pCol);
    SendEvent(e);

    //skip the header event, so it can be further processed
    rEvent.Skip();
}

//--------------------- LAYOUT --------------------------------------//
/** Resize all the children
  */
void wxVirtualDataViewCtrl::ResizeChildren(void)
{
    wxSize sClientSize = GetClientSize();
    ResizeChildren(sClientSize);
}

/** Resize all the children
  * \param sClientSize [input]: the client size of the main window
  */
void wxVirtualDataViewCtrl::ResizeChildren(wxSize sClientSize)
{
    int iWidthLeft   = 0;
    int iHeightAbove = 0;

    //get minimal sizes
    bool bColHeader = IsColHeaderShown();
    if (bColHeader)
    {
        wxSize s = m_pColumnHeaderWindow->GetBestSize();
        if (s.GetHeight() > iHeightAbove) iHeightAbove = s.GetHeight();
    }

    bool bRowHeader = IsRowHeaderShown();
    if (bRowHeader)
    {
        wxSize s = m_pRowHeaderWindow->GetBestSize();
        if (s.GetWidth() > iWidthLeft) iWidthLeft = s.GetWidth();
    }

    bool bCornerHeader = (m_pCornerHeaderWindow) && (bRowHeader) && (bColHeader);
    if (bCornerHeader)
    {
        wxSize s = m_pCornerHeaderWindow->GetBestSize();
        if (s.GetWidth() > iWidthLeft) iWidthLeft = s.GetWidth();
        if (s.GetHeight() > iHeightAbove) iHeightAbove = s.GetHeight();
    }

    //layout
    int w = sClientSize.GetWidth()  - iWidthLeft;
    int h = sClientSize.GetHeight() - iHeightAbove;
    if (bCornerHeader)  m_pCornerHeaderWindow->SetSize(0, 0, iWidthLeft, iHeightAbove);
    if (bColHeader)     m_pColumnHeaderWindow->SetSize(iWidthLeft, 0, w, iHeightAbove);
    if (bRowHeader)     m_pRowHeaderWindow->SetSize(0, iHeightAbove, iWidthLeft, h);
    if (m_pClientArea)
    {
        m_pClientArea->SetSize(iWidthLeft, iHeightAbove, w, h);
        wxSize s(w, h);
        m_pClientArea->UpdateScrollbars(s);
    }

    //refresh
    RefreshHeaders();
    if (m_pClientArea) m_pClientArea->UpdateDisplay();
}

//------------------- wxWindow OVERRIDE -----------------------------//
/** Get the best size for the item
  * \return the best size for showing the whole model
  */
wxSize wxVirtualDataViewCtrl::DoGetBestSize(void) const
{
    if (!m_pClientArea) return(wxWindow::DoGetBestSize());
    return(m_pClientArea->GetBestSize());
}

//--------------------- INLINED METHODS -----------------------------//
/** Get the data model
  * \return the data model
  */
WX_VDV_INLINE wxVirtualIDataModel* wxVirtualDataViewCtrl::DoGetDataModel(void) const
{
    if (!m_pClientArea) return(WX_VDV_NULL_PTR);
    return(m_pClientArea->GetDataModel());
}

/** Get the state model
  * \return the state model
  */
WX_VDV_INLINE wxVirtualIStateModel* wxVirtualDataViewCtrl::DoGetStateModel(void) const
{
    if (!m_pClientArea) return(WX_VDV_NULL_PTR);
    return(m_pClientArea->GetStateModel());
}

//------------------- ITEM ITERATION : CHILDREN ---------------------//
#define DO_GET_DATA_MODEL
/** Get the root item
  * \return the root item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetRootItem(void) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetRootItem());
    return(wxVirtualItemID());
}

/** Get the amount of children item
  * \param rID [input]: the item to query
  * \return the amount of children of the item
  */
size_t wxVirtualDataViewCtrl::GetChildrenCount(const wxVirtualItemID &rID) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetChildCount(rID));
    return(0);
}

/** Check if an item has children
  * \param rID [input]: the item to check
  * \return true if an item has children, false otherwise
  */
bool wxVirtualDataViewCtrl::HasChildren(const wxVirtualItemID &rID) const
{
    if (GetChildrenCount(rID) > 0) return(true);
    return(false);
}

/** Get the index of a child
  * \param rParent [input]: the parent item
  * \param rChild  [input]: the child item
  * \return the index of the child, or -1 if it is not found
  */
int wxVirtualDataViewCtrl::GetChildIndex(const wxVirtualItemID &rParent,
                                         const wxVirtualItemID &rChild) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetChildIndex(rParent, rChild));
    return(-1);
}

/** Get the index of a child
  * \param rChild  [input]: the child item
  * \return the index of the child, or -1 if it is not found
  */
int wxVirtualDataViewCtrl::GetChildIndex(const wxVirtualItemID &rChild) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetChildIndex(rChild));
    return(-1);
}

/** Get a child item
  * \param rID [input]: the ID of the parent
  * \param uiChild [input]: the index of the child (0 = first child)
  * \return the child item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetChildItem(const wxVirtualItemID &rID,
                                                    size_t uiChild) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetChild(rID, uiChild));
    return(wxVirtualItemID());
}

/** Get the first child
  * \param rID [input] : the parent item
  * \return the 1st child of rID
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstChild(const wxVirtualItemID &rID) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetChild(rID, 0));
    return(wxVirtualItemID());
}

/** Get the next child
  * \param rParent [input]: the parent item
  * \param rChild  [input]: the child item
  * \return the child item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextChild(const wxVirtualItemID &rParent,
                                                    const wxVirtualItemID &rChild) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel)
    {
        size_t uiIndex = pDataModel->GetChildIndex(rParent, rChild);
        return(pDataModel->GetChild(rParent, uiIndex + 1));
    }
    return(wxVirtualItemID());
}

/** Get the previous child
  * \param rParent [input]: the parent item
  * \param rChild  [input]: the child item
  * \return the child item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPreviousChild(const wxVirtualItemID &rParent,
                                                        const wxVirtualItemID &rChild) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel)
    {
        size_t uiIndex = pDataModel->GetChildIndex(rParent, rChild);
        if (uiIndex == 0) return(wxVirtualItemID());
        if (uiIndex == size_t(-1)) return(wxVirtualItemID());
        return(pDataModel->GetChild(rParent, uiIndex - 1));
    }
    return(wxVirtualItemID());
}

/** Get the last child
  * \param rID [input]: the parent item
  * \return the last child
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetLastChild(const wxVirtualItemID &rID) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel)
    {
        size_t uiCount = pDataModel->GetChildCount(rID);
        if (uiCount == 0) return(wxVirtualItemID());
        return(pDataModel->GetChild(rID, uiCount - 1));
    }
    return(wxVirtualItemID());
}

/** Get all the children items
  * \param vChildren [output]: the children items. Previous content is lost
  * \param rParent   [input] : the parent item
  * \return the amount of children items
  */
size_t wxVirtualDataViewCtrl::GetAllChildren(wxVirtualItemIDs &vChildren,
                                             const wxVirtualItemID &rParent,
                                             bool bRecursive) const
{
    vChildren.clear();
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel)
    {
        if (bRecursive)
        {
            wxStack<wxVirtualItemID> StackOfIDs;
            StackOfIDs.push(rParent);
            while(StackOfIDs.size() > 0)
            {
                wxVirtualItemID id = StackOfIDs.top();
                StackOfIDs.pop();
                if (id.IsInvalid()) continue;

                size_t uiCount = pDataModel->GetChildCount(id);
                size_t i;
                for(i=0;i<uiCount;i++)
                {
                    wxVirtualItemID idChild = pDataModel->GetChild(id, i);
                    if (!idChild.IsInvalid())
                    {
                        vChildren.push_back(idChild);
                        StackOfIDs.push(idChild);
                    }
                }
            }
        }
        else
        {
            pDataModel->GetAllChildren(vChildren, rParent);
        }

    }
    return(vChildren.size());
}

//------------------ ITEM ITERATION : PARENT & SIBLING --------------
/** Get the parent item
  * \param rID [input]: the ID of the item to query (never returned)
  * \return the parent item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetParentItem(const wxVirtualItemID &rID) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetParent(rID));
    return(wxVirtualItemID());
}

/** Get the next sibling item
  * \param rID [input]: the ID of the item to query (never returned)
  * \return the next sibling item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextSibling(const wxVirtualItemID &rID) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetNextSibling(rID));
    return(wxVirtualItemID());
}

/** Get the previous sibling item
  * \param rID [input]: the ID of the item to query (never returned)
  * \return the previous sibling item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPreviousSibling(const wxVirtualItemID &rID) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (pDataModel) return(pDataModel->GetPreviousSibling(rID));
    return(wxVirtualItemID());
}

//------------------ ITEM ITERATION : LINEAR ------------------------//
/** Get the total count of item
  * \return the total amount of items (O(n) !!)
  */
size_t wxVirtualDataViewCtrl::GetItemCount(bool bCountRootItem) const
{

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(0);

    wxVirtualItemID rID = pDataModel->GetRootItem();
    size_t uiResult = pDataModel->GetSubTreeSize(rID, WX_VDV_NULL_PTR);
    if ((!bCountRootItem) && (uiResult > 0)) uiResult--;
    return(uiResult);
}

/** Check if the control is empty (no items apart from root)
  * \return true if the item is empty, false otherwise
  */
bool wxVirtualDataViewCtrl::IsEmpty(void) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(true);

    wxVirtualItemID rID = pDataModel->GetRootItem();
    size_t uiResult = pDataModel->GetChildCount(rID);
    if (uiResult == 0) return(true);

    return(false);
}

/** Get the 1st item (1st child of root)
  * \return the 1st item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstItem(void) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(wxVirtualItemID());

    wxVirtualItemID rID = pDataModel->GetRootItem();
    return(pDataModel->GetChild(rID, 0));
}

/** Get the next item
  * \param rID [input]: the start item (never returned)
  * \param uiCount [input] : the amount of item to walk
  * \param bSkipCollapsedChildren [input] : if true, collapsed children are skipped
  *                                         if false, collapsed children are iterated
  * \return the next item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextItem(const wxVirtualItemID &rID,
                                                   size_t uiCount,
                                                   bool bSkipCollapsedChildren) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    wxVirtualIStateModel *pStateModel = WX_VDV_NULL_PTR;
    if (bSkipCollapsedChildren) pStateModel = DoGetStateModel();
    if (pDataModel) return(pDataModel->NextItem(rID, pStateModel, uiCount));
    return(wxVirtualItemID());
}

/** Get the previous item
  * \param rID [input]: the start item (never returned)
  * \param uiCount [input] : the amount of item to walk
  * \param bSkipCollapsedChildren [input] : if true, collapsed children are skipped
  *                                         if false, collapsed children are iterated
  * \return the previous item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPreviousItem(const wxVirtualItemID &rID,
                                                       size_t uiCount,
                                                       bool bSkipCollapsedChildren) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    wxVirtualIStateModel *pStateModel = WX_VDV_NULL_PTR;
    if (bSkipCollapsedChildren) pStateModel = DoGetStateModel();
    if (pDataModel) return(pDataModel->PrevItem(rID, pStateModel, uiCount));
    return(wxVirtualItemID());
}

//-------------------------- SELECTION ------------------------------//
/** Get all the selected items
  * \param vIDs [output]: the list of selected items. Previous content is lost
  * \return the amount of selected items.
  */
size_t wxVirtualDataViewCtrl::GetSelectedItems(wxVirtualItemIDs &vIDs) const
{
    vIDs.clear();
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) pStateModel->GetSelectedItems(vIDs);
    return(vIDs.size());
}

/** Get the 1st selected item
  * \return the first selected item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstSelectedItem(void) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    return(pStateModel->GetFirstSelectedItem());
}

/** Get the next selected item
  * \param rID [input]: the starting item (never returned)
  * \return the next selected item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextSelectedItem(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    wxVirtualIDataModel  *pDataModel  = DoGetDataModel();

    return(pStateModel->GetNextSelectedItem(rID, pDataModel, true));
}

/** Get the previous selected item
  * \param rID [input]: the starting item (never returned)
  * \return the previous selected item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevSelectedItem(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    wxVirtualIDataModel  *pDataModel  = DoGetDataModel();

    return(pStateModel->GetPrevSelectedItem(rID, pDataModel, true));
}

/** Get the 1st un-selected item
  * \return the first un-selected item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstUnselectedItem(void) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    wxVirtualIDataModel  *pDataModel  = DoGetDataModel();

    return(pStateModel->GetFirstSelectedItem(pDataModel, false));
}

/** Get the next un-selected item
  * \param rID [input]: the starting item (never returned)
  * \return the next un-selected item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextUnselectedItem(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    wxVirtualIDataModel  *pDataModel  = DoGetDataModel();

    return(pStateModel->GetNextSelectedItem(rID, pDataModel, false));
}

/** Get the previous un-selected item
  * \param rID [input]: the starting item (never returned)
  * \return the previous un-selected item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevUnselectedItem(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    wxVirtualIDataModel  *pDataModel  = DoGetDataModel();

    return(pStateModel->GetPrevSelectedItem(rID, pDataModel, false));
}

WX_DECLARE_HASH_SET(wxVirtualItemID, wxVirtualItemIDHash, wxVirtualItemIDEqual, TSetOfIDs);

/** Helper: convert an array to a hash set
  * \param oSetOfIDs [output]: the set of item IDs
  * \param vIDs      [input] : the array of item IDs
  */
WX_VDV_INLINE void VectorToHashSet(TSetOfIDs &oSetOfIDs, const wxVirtualItemIDs &vIDs,
                                   wxVirtualIDataModel *pDataModel)
{
    size_t i, uiCount;
    uiCount = vIDs.size();
    for(i = 0; i < uiCount; i++)
    {
        if (!pDataModel->IsSelectable(vIDs[i])) continue;
        oSetOfIDs.insert(vIDs[i]);
    }
}

/** Helper : convert a hash set to an array
  * \param vIDs      [output]: the array of item IDs
  * \param oSetOfIDs [input] : the set of item IDs
  */
WX_VDV_INLINE void HashSetToVector(wxVirtualItemIDs &vIDs, const TSetOfIDs &oSetOfIDs)
{
    vIDs.reserve(oSetOfIDs.size() + vIDs.size());
    TSetOfIDs::const_iterator it    = oSetOfIDs.begin();
    TSetOfIDs::const_iterator itEnd = oSetOfIDs.end();
    while (it != itEnd)
    {
        vIDs.push_back(*it);
        ++it;
    }
}

/** Select one item and optionally its children
  * \param rID [input]: the item to select/deselect
  * \param bSelect    [input]: true for selecting, false for deselecting
  * \param bRecursive [input]: true for recursively selecting its descendant
  * \param bSendEvent [input]: true for sending an event
  */
void wxVirtualDataViewCtrl::SelectItem(const wxVirtualItemID &rID, bool bSelect,
                                       bool bRecursive, bool bSendEvent)
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return;

    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    wxVirtualItemIDs vOlds;
    wxVirtualItemIDs vNews;
    if (bSendEvent)
    {
        //get old selection
        TSetOfIDs oSetOfIDs;
        pStateModel->GetSelectedItems(vOlds);
        VectorToHashSet(oSetOfIDs, vOlds, pDataModel);

        //add the items to the new selection
        if (bRecursive) GetAllChildren(vNews, rID, bRecursive);
        vNews.push_back(rID);
        VectorToHashSet(oSetOfIDs, vNews, pDataModel);
        HashSetToVector(vNews, oSetOfIDs);

        //send event selection changed. Can be vetoed
        if (!m_pClientArea->SendSelectionChangingEvent(vOlds, vNews)) return;
    }

    //performs selection
    pStateModel->Select(rID, bSelect, bRecursive);

    //send selection changed event
    if (bSendEvent) m_pClientArea->SendSelectionChangedEvent(vOlds, vNews);
}

/** Deselect one item and optionally its children
  * \param rID [input]: the item to deselect
  * \param bRecursive [input]: true for recursively selecting its descendant
  * \param bSendEvent [input]: true for sending an event
  */
void wxVirtualDataViewCtrl::DeselectItem(const wxVirtualItemID &rID, bool bRecursive,
                                         bool bSendEvent)
{
    SelectItem(rID, false, bRecursive, bSendEvent);
}

/** Toggle selection state of 1 or more items
  * \param rID [input]: the item to select/deselect
  * \param bRecursive [input]: true for recursively toggling its descendant
  * \param bSendEvent [input]: true for sending an event
  */
void wxVirtualDataViewCtrl::ToggleSelectedState(const wxVirtualItemID &rID,
                                                bool bRecursive,
                                                bool bSendEvent)
{
    //get state model
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    //get data model
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return;

    //get the list of items to change
    wxVirtualItemIDs vItems;
    if (bRecursive) GetAllChildren(vItems, rID, true);
    vItems.push_back(rID);
    size_t uiItem, uiNbItems;
    uiNbItems = vItems.size();

    //get old & new selection
    wxVirtualItemIDs vOlds;
    wxVirtualItemIDs vNews;
    if (bSendEvent)
    {
        TSetOfIDs oSetOfIDs;
        pStateModel->GetSelectedItems(vOlds);
        VectorToHashSet(oSetOfIDs, vOlds, pDataModel);

        for(uiItem=0; uiItem<uiNbItems; uiItem++)
        {
            wxVirtualItemID &id = vItems[uiItem];
            if (pStateModel->IsSelected(id)) oSetOfIDs.erase(id);
            else if (pDataModel->IsSelectable(id)) oSetOfIDs.insert(id);
        }
        HashSetToVector(vNews, oSetOfIDs);

        if (!m_pClientArea->SendSelectionChangingEvent(vOlds, vNews)) return; //vetoed
    }

    //perform state change
    for(uiItem=0; uiItem<uiNbItems; uiItem++)
    {
        wxVirtualItemID &id = vItems[uiItem];
        bool bSelected = pStateModel->IsSelected(id);
        if (!bSelected)
        {
            if (!pDataModel->IsSelectable(id)) continue;
        }
        pStateModel->Select(id, !bSelected, false);
    }

    //send selection changed event
    if (bSendEvent) m_pClientArea->SendSelectionChangedEvent(vOlds, vNews);
}

/** Select/deselect all items
  * \param bSelect    [input]: true for selecting, false for deselecting
  * \param bSendEvent [input]: true for sending an event
  */
void wxVirtualDataViewCtrl::SelectAll(bool bSelect, bool bSendEvent)
{
    //get state model
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    //selection changing event
    wxVirtualItemIDs vOldSelection;
    wxVirtualItemIDs vIDs;
    if (bSendEvent)
    {
        GetSelectedItems(vOldSelection);
        wxVirtualItemID idRoot = GetRootItem();
        GetAllChildren(vIDs, idRoot, true);
        if (!m_pClientArea->SendSelectionChangingEventByCopy(vOldSelection, vIDs)) return; //vetoed
    }

    //select
    pStateModel->SelectAll();

    //send selection changed event
    if (bSendEvent) m_pClientArea->SendSelectionChangedEvent(vOldSelection, vIDs);

}

/** Deselect all items
  * \param bSelect    [input]: true for selecting, false for deselecting
  * \param bSendEvent [input]: true for sending an event
  */
void wxVirtualDataViewCtrl::DeselectAll(bool bSendEvent)
{
    //get state model
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    //selection changing event
    wxVirtualItemIDs vOldSelection;
    wxVirtualItemIDs vIDs;
    if (bSendEvent)
    {
        GetSelectedItems(vOldSelection);
        if (!m_pClientArea->SendSelectionChangingEventByCopy(vOldSelection, vIDs)) return; //vetoed
    }

    //deselect
    pStateModel->DeselectAll();

    //send selection changed event
    if (bSendEvent) m_pClientArea->SendSelectionChangedEvent(vOldSelection, vIDs);
}

/** Select one or more items
  * \param vIDs       [input] : the list of items to select
  * \param bSendEvent [input] : if true, events are sent (selection changing and selection changed
  */
void wxVirtualDataViewCtrl::SelectItems(const wxVirtualItemIDs &vIDs, bool bSendEvent)
{
    //get state model
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    //selection changing event
    wxVirtualItemIDs vOldSelection;
    wxVirtualItemIDs vIDsCopy; //to move to the event
    if (bSendEvent)
    {
        GetSelectedItems(vOldSelection);
        if (!m_pClientArea->SendSelectionChangingEventByCopy(vOldSelection, vIDs)) return; //vetoed
    }

    //perform state change
    pStateModel->SelectList(vIDs, true);

    //send selection changed event
    if (bSendEvent) m_pClientArea->SendSelectionChangedEventByCopy(vOldSelection, vIDs);
}

/** Check if an item is selected
  * \param rID [input]: the item to check
  * \return true if the item is selected, false otherwise
  */
bool wxVirtualDataViewCtrl::IsSelected(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) return(pStateModel->IsSelected(rID));
    return(false);
}

/** Check if an item is deselected
  * \param rID [input]: the item to check
  * \return true if the item is deselected, false otherwise
  */
bool wxVirtualDataViewCtrl::IsDeselected(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) return(!pStateModel->IsSelected(rID));
    return(false);
}

//------------------------------- CHECK STATE -----------------------//
/** Get check state of an item
  * \param rID [input]: the item to query
  * \return the check state of the item
  */
wxVirtualDataViewCtrl::ECheckState wxVirtualDataViewCtrl::GetCheckState(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(WX_CHECKSTATE_UNDETERMINED);
    return((wxVirtualDataViewCtrl::ECheckState) pStateModel->GetCheck(rID));
}

/** Test if an item is checked
  * \param rID [input]: the item to query
  * \return true if the item is checked, false otherwise
  */
bool wxVirtualDataViewCtrl::IsChecked(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(false);
    return(pStateModel->GetCheck(rID) == wxVirtualIStateModel::WX_CHECKSTATE_CHECKED);
}

/** Test if an item is unchecked
  * \param rID [input]: the item to query
  * \return true if the item is unchecked, false otherwise
  */
bool wxVirtualDataViewCtrl::IsUnchecked(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(false);
    return(pStateModel->GetCheck(rID) == wxVirtualIStateModel::WX_CHECKSTATE_UNCHECKED);
}

/** Test if an item is in unknown check state
  * \param rID [input]: the item to query
  * \return true if the item is in unknown check state, false otherwise
  */
bool wxVirtualDataViewCtrl::IsCheckStateUndetermined(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(false);
    return(pStateModel->GetCheck(rID) == wxVirtualIStateModel::WX_CHECKSTATE_UNDETERMINED);
}

/** Check / uncheck items
  * \param rID        [input]: the item to check/uncheck
  * \param eState     [input]: the new check-state
  * \param bRecursive [input]: true for changing the state of all children as well
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::CheckItem(const wxVirtualItemID &rID,
                                      ECheckState eState,
                                      bool bRecursive, bool bSendEvent)
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return;

    //get list of items
    wxVirtualItemIDs vIDs;
    if (bRecursive) GetAllChildren(vIDs, rID, true);
    vIDs.insert(vIDs.begin(), rID);

    //check
    size_t uiNbItems = vIDs.size();
    size_t uiItem;
    for(uiItem = 0; uiItem < uiNbItems; uiItem++)
    {
        wxVirtualItemID &id = vIDs[uiItem];
        if (!pDataModel->IsCheckable(id)) continue;
        if (bSendEvent)
        {
            if (!m_pClientArea->SendEventChecking(id)) continue;
        }
        pStateModel->SetCheck(id, (wxVirtualIStateModel::ECheckState) eState, false);
        if (bSendEvent) m_pClientArea->SendEventChecked(id);
    }
}

/** Uncheck items
  * \param rID        [input]: the item to uncheck
  * \param bRecursive [input]: true for changing the state of all children as well
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::UncheckItem(const wxVirtualItemID &rID,
                                        bool bRecursive, bool bSendEvent)
{
    CheckItem(rID, WX_CHECKSTATE_UNCHECKED, bRecursive, bSendEvent);
}

/** Set items in undetermined check state
  * \param rID        [input]: the item to uncheck
  * \param bRecursive [input]: true for changing the state of all children as well
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::SetCheckUndetermined(const wxVirtualItemID &rID,
                                                 bool bRecursive, bool bSendEvent)
{

    CheckItem(rID, WX_CHECKSTATE_UNDETERMINED, bRecursive, bSendEvent);
}

/** Get all the checked items
  * \param vIDs [output]: the items. Previous content is lost
  * \return the amount of items in vIDs
  */
size_t wxVirtualDataViewCtrl::GetCheckedItems(wxVirtualItemIDs &vIDs,
                                              ECheckState eState) const
{
    vIDs.clear();
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(0);

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(0);

    wxVirtualIStateModel::ECheckState eState2 = (wxVirtualIStateModel::ECheckState) eState;
    wxVirtualItemID id = pDataModel->GetRootItem();
    id = pStateModel->GetFirstCheckedItem(pDataModel, eState2);
    while(!id.IsInvalid())
    {
        vIDs.push_back(id);
        id = pStateModel->GetNextCheckedItem(id, pDataModel, eState2);
    }

    return(vIDs.size());
}

/** Get all the unchecked items
  * \param vIDs [output]: the items. Previous content is lost
  * \return the amount of items in vIDs
  */
size_t wxVirtualDataViewCtrl::GetUncheckedItems(wxVirtualItemIDs &vIDs) const
{
    return(GetCheckedItems(vIDs, WX_CHECKSTATE_UNCHECKED));
}

/** Get all the items which are in an undetermined check state
  * \param vIDs [output]: the items. Previous content is lost
  * \return the amount of items in vIDs
  */
size_t wxVirtualDataViewCtrl::GetUndeterminedCheckedItems(wxVirtualItemIDs &vIDs) const
{
    return(GetCheckedItems(vIDs, WX_CHECKSTATE_UNDETERMINED));
}

/** Get the 1st checked item
  * \param eState [input]: the check state
  * \return the 1st item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstCheckedItem(ECheckState eState) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    wxVirtualIStateModel::ECheckState eState2 = (wxVirtualIStateModel::ECheckState) eState;
    return(pStateModel->GetFirstCheckedItem(pDataModel, eState2));
}

/** Get the next checked item
  * \param rID    [input]: the start item (never returned)
  * \param eState [input]: the check state
  * \return the next item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextCheckedItem(const wxVirtualItemID &rID,
                                                          ECheckState eState) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    wxVirtualIStateModel::ECheckState eState2 = (wxVirtualIStateModel::ECheckState) eState;
    return(pStateModel->GetNextCheckedItem(rID, pDataModel, eState2));
}

/** Get the previous checked item
  * \param rID    [input]: the start item (never returned)
  * \param eState [input]: the check state
  * \return the previous item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevCheckedItem(const wxVirtualItemID &rID,
                                                          ECheckState eState) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    wxVirtualIStateModel::ECheckState eState2 = (wxVirtualIStateModel::ECheckState) eState;
    return(pStateModel->GetPrevCheckedItem(rID, pDataModel, eState2));
}

/** Get the 1st unchecked item
  * \return the 1st item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstUncheckedItem(void) const
{
    return(GetFirstCheckedItem(WX_CHECKSTATE_UNCHECKED));
}

/** Get the next unchecked item
  * \param rID    [input]: the start item (never returned)
  * \return the next item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextUncheckedItem(const wxVirtualItemID &rID) const
{
    return(GetNextCheckedItem(rID, WX_CHECKSTATE_UNCHECKED));
}

/** Get the previous unchecked item
  * \param rID    [input]: the start item (never returned)
  * \return the previous item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevUncheckedItem(const wxVirtualItemID &rID) const
{
    return(GetPrevCheckedItem(rID, WX_CHECKSTATE_UNCHECKED));
}

/** Get the 1st undetermined item
  * \return the 1st item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstUndeterminedItem(void) const
{
    return(GetFirstCheckedItem(WX_CHECKSTATE_UNDETERMINED));
}

/** Get the next undetermined item
  * \param rID    [input]: the start item (never returned)
  * \return the next item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextUndeterminedItem(const wxVirtualItemID &rID) const
{
    return(GetNextCheckedItem(rID, WX_CHECKSTATE_UNDETERMINED));
}

/** Get the previous undetermined item
  * \param rID    [input]: the start item (never returned)
  * \return the previous item matching the state
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevUndeterminedItem(const wxVirtualItemID &rID) const
{
    return(GetPrevCheckedItem(rID, WX_CHECKSTATE_UNDETERMINED));
}

/** Check all items
  * \param eState     [input]: the new check state
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::CheckAll(ECheckState eState, bool bSendEvent)
{
    wxVirtualItemID id = GetRootItem();
    CheckItem(id, eState, true, bSendEvent);
}

/** Un-check all items
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::UncheckAll(bool bSendEvent)
{
    wxVirtualItemID id = GetRootItem();
    CheckItem(id, WX_CHECKSTATE_UNCHECKED, true, bSendEvent);
}

/** Set all items in undetermined check state
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::SetAllUndeterminedCheckState(bool bSendEvent)
{
    wxVirtualItemID id = GetRootItem();
    CheckItem(id, WX_CHECKSTATE_UNDETERMINED, true, bSendEvent);
}

/** Toggle check state of 1 or more item (undetermined becomes checked)
  * \param rID [input]: the item to toggle
  * \param bRecursive [input]: true for processing children as well (and their children...)
  * \param bSendEvent [input]: true for sending an event for each item
  */
void wxVirtualDataViewCtrl::ToggleCheckState(const wxVirtualItemID &rID,
                                             bool bRecursive, bool bSendEvent)
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return;

    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    //get list of items
    wxVirtualItemIDs vIDs;
    if (bRecursive) GetAllChildren(vIDs, rID, true);
    vIDs.insert(vIDs.begin(), rID);

    //process
    size_t uiNbItems = vIDs.size();
    size_t uiItem;
    for(uiItem = 0; uiItem < uiNbItems; uiItem++)
    {
        wxVirtualItemID &id = vIDs[uiItem];
        if (!pDataModel->IsCheckable(id)) continue;
        if (bSendEvent)
        {
            if (!m_pClientArea->SendEventChecking(id)) continue;
        }

        wxVirtualIStateModel::ECheckState eState = pStateModel->GetCheck(id);
        switch(eState)
        {
            case wxVirtualIStateModel::WX_CHECKSTATE_CHECKED     :
                eState = wxVirtualIStateModel::WX_CHECKSTATE_UNCHECKED;
                break;

            case wxVirtualIStateModel::WX_CHECKSTATE_UNCHECKED   :
            case wxVirtualIStateModel::WX_CHECKSTATE_UNDETERMINED:
            default                                              :
                eState = wxVirtualIStateModel::WX_CHECKSTATE_CHECKED;
                break;
        }
        pStateModel->SetCheck(id, eState, false);
        if (bSendEvent) m_pClientArea->SendEventChecked(id);
    }
}

//---------------------- FOCUSED STATE ------------------------------//
/** Get the focused item
  * \return the focused item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFocusedItem(void) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());
    return(pStateModel->GetFocusedItem());
}

/** Set the focused item
  * \param rID        [input]: the ID of the item to focus
  * \param bSendEvent [input]: true if an event must be sent
  */
void wxVirtualDataViewCtrl::SetFocusedItem(const wxVirtualItemID &rID, bool bSendEvent)
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    if (bSendEvent)
    {
        size_t uiCol = GetFocusedColumn();
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) return;
        size_t uiField = pCol->GetModelColumn();

        wxVirtualItemID idPrev = pStateModel->GetFocusedItem();
        m_pClientArea->SendItemFocusedEvent(idPrev, uiField, uiCol, false);
        pStateModel->SetFocus(rID);
        m_pClientArea->SendItemFocusedEvent(rID, uiField, uiCol, true);
    }
    else
    {
        pStateModel->SetFocus(rID);
    }
}

/** Get focused column ID
  * \return the focused column ID
  */
int wxVirtualDataViewCtrl::GetFocusedColumn(void) const
{
    if (!m_pClientArea) return(0);
    return(m_pClientArea->GetFocusedColumn());
}

/** Set the focused column ID
  * \param iCol [input]: the new focused column
  * \param bSendEvent [input]: true if an event must be sent
  */
void wxVirtualDataViewCtrl::SetFocusedColumn(int iCol, bool bSendEvent)
{
    if (bSendEvent)
    {
        wxVirtualIStateModel *pStateModel = DoGetStateModel();
        if (!pStateModel) return;

        size_t uiCol = GetFocusedColumn();
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) return;
        size_t uiField = pCol->GetModelColumn();

        wxVirtualItemID id = pStateModel->GetFocusedItem();
        m_pClientArea->SendItemFocusedEvent(id, uiField, uiCol, false);

        pCol = m_vColumns.GetColumn(iCol);
        if (!pCol) return;
        uiField = pCol->GetModelColumn();

        m_pClientArea->SetFocusedColumn(iCol);
        m_pClientArea->SendItemFocusedEvent(id, uiField, iCol, true);
    }
    else
    {
        m_pClientArea->SetFocusedColumn(iCol);
    }
}

//------------- ENABLED / DISABLED STATE ----------------------------//
/** Check if an item is enabled
  * \param rID [input]: the item to check
  * \return true if the item is enabled (can take input), false otherwise
  */
bool wxVirtualDataViewCtrl::IsItemEnabled(const wxVirtualItemID &rID) const
{
    //enable if enabled : state model, data model, column state
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(false);
    if (!pStateModel->IsEnabled(rID)) return(false);

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(false);
    if (!pDataModel->IsEditable(rID)) return(false);

    int iCol = GetFocusedColumn();
    wxVirtualDataViewColumn* pCol = Columns().GetColumn(iCol);
    if (!pCol) return(false);
    if (pCol->IsReadOnly()) return(false);

    return(true);
}

/** Check if an item is disabled
  * \param rID [input]: the item to check
  * \return true if the item is enabled (can take input), false otherwise
  */
bool wxVirtualDataViewCtrl::IsItemDisabled(const wxVirtualItemID &rID) const
{
    return(!IsItemEnabled(rID));
}

/** Enable an item
  * \param rID     [input]: the item to enable
  * \param bEnable [input]: true for enabling the item, false otherwise
  * \param bRecursive [input]: true for processing the children as well
  */
void wxVirtualDataViewCtrl::EnableItem(const wxVirtualItemID &rID, bool bEnable,
                                       bool bRecursive)
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) pStateModel->Enable(rID, bEnable, bRecursive);
}

/** Disable an item
  * \param rID     [input]: the item to enable
  * \param bRecursive [input]: true for processing the children as well
  */
void wxVirtualDataViewCtrl::DisableItem(const wxVirtualItemID &rID, bool bRecursive)
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) pStateModel->Enable(rID, false, bRecursive);
}

/** Get first enabled item
  * \param bEnabled [input]: true for finding enabled items, false for finding disabled items
  * \return the item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstEnabledItem(bool bEnabled) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    return(pStateModel->GetFirstEnabledItem(pDataModel, bEnabled));
}

/** Get next enabled item
  * \param rID [input]: the start item (never returned)
  * \param bEnabled [input]: true for finding enabled items, false for finding disabled items
  * \return the item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextEnabledItem(const wxVirtualItemID &rID,
                                                          bool bEnabled) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    return(pStateModel->GetNextEnabledItem(rID, pDataModel, bEnabled));
}

/** Get previous enabled item
  * \param rID [input]: the start item (never returned)
  * \param bEnabled [input]: true for finding enabled items, false for finding disabled items
  * \return the item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevEnabledItem(const wxVirtualItemID &rID,
                                                          bool bEnabled) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    return(pStateModel->GetPrevEnabledItem(rID, pDataModel, bEnabled));
}

/** Get all enabled items
  * \param vIDs [output] : the found items. Previous content is lost
  * \param bEnabled [input]: true for finding enabled items, false for finding disabled items
  * \return the amount of found items
  */
size_t wxVirtualDataViewCtrl::GetAllEnabledItems(wxVirtualItemIDs &vIDs, bool bEnabled) const
{
    vIDs.clear();
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(0);

    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(0);

    wxVirtualItemID id = pDataModel->GetRootItem();
    id = pStateModel->GetFirstEnabledItem(pDataModel, bEnabled);
    while(!id.IsInvalid())
    {
        vIDs.push_back(id);
        id = pStateModel->GetNextEnabledItem(id, pDataModel, bEnabled);
    }

    return(vIDs.size());
}

/** Get first disabled item
  * \return the 1st disabled item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstDisabledItem(void) const
{
    return(GetFirstEnabledItem(false));
}

/** Get next disabled item
  * \param rID [input]: the start item (never returned)
  * \return the next disabled item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextDisabledItem(const wxVirtualItemID &rID) const
{
    return(GetNextEnabledItem(rID, false));
}

/** Get previous disabled item
  * \param rID [input]: the start item (never returned)
  * \return the previous disabled item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevDisabledItem(const wxVirtualItemID &rID) const
{
    return(GetPrevEnabledItem(rID, false));
}

/** Get all disabled items
  * \param vIDs [output] : the found items. Previous content is lost
  * \return the amount of found items
  */
size_t wxVirtualDataViewCtrl::GetAllDisabledItems(wxVirtualItemIDs &vIDs) const
{
    return(GetAllEnabledItems(vIDs, false));
}

//---------------------------- EXPAND / COLLAPSE --------------------//
/** Check if an item is expanded
  * \param rID [input]: the item to check
  * \return true if the item is expanded, false it is collapsed
  */
bool wxVirtualDataViewCtrl::IsExpanded(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) return(pStateModel->IsExpanded(rID));
    return(false);
}

/** Check if an item is collapsed
  * \param rID [input]: the item to check
  * \return true if the item is collapsed, false it is expanded
  */
bool wxVirtualDataViewCtrl::IsCollapsed(const wxVirtualItemID &rID) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (pStateModel) return(!pStateModel->IsExpanded(rID));
    return(false);
}

/** Expand or collapse an item
  * \param rID [input]: the item to expand / collapse
  * \param bExpand [input]: true for expanding the item, false for collapsing it
  * \param bRecursive [input]: true for recursive expansion / collapse
  * \param bSendEvent [input]: true if event are sent
  */
void wxVirtualDataViewCtrl::Expand(const wxVirtualItemID &rID, bool bExpand,
                                   bool bRecursive, bool bSendEvent)
{
    wxVirtualIModelRenderer *pRenderer = GetModelRenderer();
    if (!pRenderer) return;
    if (bExpand) pRenderer->Expand(rID, false, bRecursive, true, bSendEvent);
    else         pRenderer->Collapse(rID, false, bRecursive, true, bSendEvent);
}

/** Collapse an item
  * \param rID [input]: the item to  collapse
  * \param bRecursive [input]: true for recursive  collapse
  * \param bSendEvent [input]: true if event are sent
  */
void wxVirtualDataViewCtrl::Collapse(const wxVirtualItemID &rID,
                                     bool bRecursive, bool bSendEvent)
{
    Expand(rID, false, bRecursive, bSendEvent);
}

/** Get all expanded / collapsed items
  * \param vIDs [output]: the list of items. Previous content is lost
  * \param bExpanded [input]: true for finding expanded items, false for finding collapsed items
  * \return the amount of items
  */
size_t wxVirtualDataViewCtrl::GetExpandedItems(wxVirtualItemIDs &vIDs,
                                               bool bExpanded) const
{
    vIDs.clear();
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(0);

    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(0);

    wxVirtualItemID id = pDataModel->GetRootItem();
    id = pStateModel->GetFirstExpandedItem(pDataModel, bExpanded);
    while(!id.IsInvalid())
    {
        vIDs.push_back(id);
        id = pStateModel->GetNextExpandedItem(id, pDataModel, bExpanded);
    }

    return(vIDs.size());
}

/** Get all collapsed items
  * \param vIDs [output]: the list of items. Previous content is lost
  * \return the amount of items
  */
size_t wxVirtualDataViewCtrl::GetCollapsedItems(wxVirtualItemIDs &vIDs) const
{
    return(GetExpandedItems(vIDs, false));
}

/** Get the 1st expanded / collapsed item
  * \param pExpanded [input]: true for finding expanded items, false otherwise
  * \return the 1st expanded / collapsed item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstExpandedItem(bool bExpanded) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    return(pStateModel->GetFirstExpandedItem(pDataModel, bExpanded));
}

/** Get the next expanded / collapsed item
  * \param rID [input]: the starting item (never returned)
  * \param pExpanded [input]: true for finding expanded items, false otherwise
  * \return the next expanded / collapsed item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextExpandedItem(const wxVirtualItemID &rID,
                                                           bool bExpanded) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    return(pStateModel->GetNextExpandedItem(rID, pDataModel, bExpanded));
}

/** Get the previous expanded / collapsed item
  * \param rID [input]: the starting item (never returned)
  * \param pExpanded [input]: true for finding expanded items, false otherwise
  * \return the previous expanded / collapsed item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevExpandedItem(const wxVirtualItemID &rID,
                                                           bool bExpanded) const
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(wxVirtualItemID());

    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    return(pStateModel->GetPrevExpandedItem(rID, pDataModel, bExpanded));
}

/** Get the 1st collapsed item
  * \return the first collapsed item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstCollapsedItem(void) const
{
    return(GetFirstExpandedItem(false));
}

/** Get the next collapsed item
  * \param rID [input]: the starting item (never returned)
  * \param pExpanded [input]: true for finding collapsed items, false otherwise
  * \return the next collapsed item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextCollapsedItem(const wxVirtualItemID &rID) const
{
    return(GetNextExpandedItem(rID, false));
}

/** Get the previous collapsed item
  * \param rID [input]: the starting item (never returned)
  * \param pExpanded [input]: true for finding collapsed items, false otherwise
  * \return the previous collapsed item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevCollapsedItem(const wxVirtualItemID &rID) const
{
    return(GetPrevExpandedItem(rID, false));
}

/** Expand / collapse all items
  * \param bExpand [input]: true for expanding, false for collapsing
  * \param bEvent  [input]: true for sending event
  */
void wxVirtualDataViewCtrl::ExpandAll(bool bExpand, bool bSendEvent)
{
    wxVirtualItemID idRoot = GetRootItem();
    Expand(idRoot, bExpand, true, bSendEvent);
}

/** Collapse all items
  * \param bEvent  [input]: true for sending event
  */
void wxVirtualDataViewCtrl::CollapseAll(bool bSendEvent)
{
    ExpandAll(false, bSendEvent);
}

/** Toggle expanded / collapsed state
  * \param rID [input]: the item to process
  * \param bRecursive [input]: true for processing children as well
  * \param bEvent  [input]: true for sending event
  */
void wxVirtualDataViewCtrl::ToggleExpandCollapseState(const wxVirtualItemID &rID,
                                                      bool bRecursive,
                                                      bool bSendEvent)
{
    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return;

    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return;

    //get items
    wxVirtualItemIDs vIDs;
    if (bRecursive) GetAllChildren(vIDs, rID, true);
    vIDs.insert(vIDs.begin(), rID);
    size_t uiNbItems = vIDs.size();
    size_t uiItem;

    //process
    for(uiItem = 0; uiItem < uiNbItems; uiItem++)
    {
        wxVirtualItemID &id = vIDs[uiItem];
        if (pStateModel->IsExpanded(id))
        {
            pModelRenderer->Collapse(id, false, false, true, bSendEvent);
        }
        else
        {
            pModelRenderer->Expand(id, false, false, true, bSendEvent);
        }
    }
}

//------------------ ACTIVATE AN ITEM -------------------------------//
/** Activate an item
  * \param rID        [input]: the ID of the item to activate
  * \param iCol       [input]: which column to activate
  * \param bSendEvent [input]: true for sending activation event, false otherwise
  */
void wxVirtualDataViewCtrl::ActivateItem(const wxVirtualItemID &rID, int iCol,
                                         bool bSendEvent)
{
    //select, focus
    SelectItem(rID, true, false, bSendEvent);
    SetFocusedColumn(iCol, false);
    SetFocusedItem(rID, bSendEvent);

    //send activation event
    if (!bSendEvent) return;
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
    size_t uiField = 0;
    if (pCol) uiField = pCol->GetModelColumn();
    m_pClientArea->SendActivationEvent(rID, uiField, iCol);
}

//-------------- HIT TEST & BOUNDING RECT ---------------------------//
/** Performs a hit test
  * \param rResult         [output]: the result of the hit test
  * \param x               [input] : the x coordinate of the point to test
  * \param y               [input] : the y coordinate of the point to test
  * \param bClientAreaOnly [input] : true for performing the test only on visible item
  *                                  false for performing the test on all items
  * \return true if an item was found, false otherwise
  */
bool wxVirtualDataViewCtrl::HitTest(THitTest &rResult, wxCoord x, wxCoord y,
                                    bool bClientAreaOnly)
{
    if (!m_pClientArea) return(false);

    //adjust to client area coordinates due to header
    int x0 = 0, y0 = 0;
    m_pClientArea->GetPosition(&x0, &y0);
    x0 = x - x0;
    y0 = y - y0;

    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(false);
    return(pModelRenderer->HitTest((wxVirtualIModelRenderer::THitTest&) rResult,
                                   x0, y0, bClientAreaOnly));
}

/** Get the rectangle of an item
  * \param rRect [output]: the bounding rectangle of the item
  * \param rID                [input] : the ID of the element to get
  * \param uiColIndex         [input] : the column index of the element to get (view coordinate)
  * \param bClientCoordinates [input] : if true, the bounding rectangle is in client coordinates
  *                                     (so scrolled out items may have negatives coordinates)
  *                                     if false, the root item starts at (0,0) (top left corner)
  * \return true if the item is displayed
  *         false otherwise (collapsed, hidden)
  */
bool wxVirtualDataViewCtrl::GetColItemRect(wxRect &rRect, const wxVirtualItemID &rID,
                                           size_t uiColIndex, bool bClientCoordinates)
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(false);
    if (!pModelRenderer->GetItemRect(rRect, rID, uiColIndex, bClientCoordinates)) return(false);

    //item rect is relative to the client window.
    int x0 = 0, y0 = 0;
    if (m_pClientArea) m_pClientArea->GetPosition(&x0, &y0);
    rRect.Offset(x0, y0);
    return(true);
}

/** Get the rectangle of an item
  * \param rRect [output]: the bounding rectangle of the item
  * \param rID                [input] : the ID of the element to get
  * \param bClientCoordinates [input] : if true, the bounding rectangle is in client coordinates
  *                                     (so scrolled out items may have negatives coordinates)
  *                                     if false, the root item starts at (0,0) (top left corner)
  * \return true if the item is displayed
  *         false otherwise (collapsed, hidden)
  */
bool wxVirtualDataViewCtrl::GetItemRect(wxRect &rRect, const wxVirtualItemID &rID,
                                        bool bClientCoordinates)
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(false);

    //get rect for field 0
    if (!pModelRenderer->GetItemRect(rRect, rID, 0, bClientCoordinates))
    {
        return(false);
    }

    //expand to left x and right x
    rRect.x = 0;
    rRect.width = Columns().GetTotalWidth();

    //transform to client coordinates
    if (bClientCoordinates)
    {
        int iXOffset = GetScrollPos(wxHORIZONTAL);
        rRect.x -= iXOffset;
    }

    //item rect is relative to the client window.
    int x0 = 0, y0 = 0;
    if (m_pClientArea) m_pClientArea->GetPosition(&x0, &y0);
    rRect.Offset(x0, y0);

    return(true);
}

//-------------------------- VISIBILITY -----------------------------//
/** Check if the item is visible
  * \param rID   [input]: the item to check
  * \param uiCol [input]: the index of the column
  * \param bCheckCol [input]: true if the column visibility must be checked
  * \return true if the item is visible, false otherwise
  */
bool wxVirtualDataViewCtrl::IsItemVisible(const wxVirtualItemID &rID, size_t uiCol,
                                          bool bCheckCol)
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(false);
    return(pModelRenderer->IsItemVisible(rID, uiCol, bCheckCol));
}

/** Make the item visible (without expanding)
  * \param rID     [input]: the item to make visible
  * \param uiCol   [input]: the view column of the item to make visible
  * \param eFlag   [input]: how to make the item visible
  * \param bSelect [input]: true for selecting the item in addition
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::MakeItemVisible(const wxVirtualItemID &rID, size_t uiCol,
                                            EVisibilityFlags eFlag, bool bSelect)
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(false);
    if (!pModelRenderer->MakeItemVisible(rID, uiCol,
                                         (wxVirtualIModelRenderer::EVisibilityFlags) eFlag)) return(false);
    if (bSelect) SelectItem(rID, true, false, true);
    return(true);
}


/** Ensure the item is visible (with expanding when needed)
  * \param rID     [input]: the item to make visible
  * \param uiCol   [input]: the view column of the item to make visible
  * \param eFlag   [input]: how to make the item visible
  * \param bSelect [input]: true for selecting the item in addition
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::EnsureItemVisible(const wxVirtualItemID &rID, size_t uiCol,
                                              EVisibilityFlags eFlag, bool bSelect)
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(false);
    if (!pModelRenderer->EnsureItemVisible(rID, uiCol,
                                          (wxVirtualIModelRenderer::EVisibilityFlags) eFlag)) return(false);
    if (bSelect) SelectItem(rID, true, false, true);
    return(true);
}

/** Get the 1st visible item
  * \return the 1st visible item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetFirstVisibleItem(void) const
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(wxVirtualItemID());
    return(pModelRenderer->GetFirstVisibleItem());
}

/** Get the next visible item
  * \param rID [input]: the starting item (never returned)
  * \return the next visible item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetNextVisibleItem(const wxVirtualItemID &rID) const
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(wxVirtualItemID());
    return(pModelRenderer->GetNextVisibleItem(rID));
}

/** Get the previous visible item
  * \param rID [input]: the starting item (never returned)
  * \return the previous visible item
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetPrevVisibleItem(const wxVirtualItemID &rID) const
{
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (!pModelRenderer) return(wxVirtualItemID());
    return(pModelRenderer->GetPrevVisibleItem(rID));
}

/** Get all visible items
  * \param vIDs [input]: the visible items. Previous content is lost
  * \return the amount of visible items in vIDs
  */
size_t wxVirtualDataViewCtrl::GetAllVisibleItems(wxVirtualItemIDs &vIDs) const
{
    vIDs.clear();
    wxVirtualIModelRenderer *pModelRenderer = GetModelRenderer();
    if (pModelRenderer) pModelRenderer->GetAllVisibleItems(vIDs);
    return(vIDs.size());
}

//----------------------- GET / SET VALUE ---------------------------//
/** Get item data
  * \param rID  [input]: the item to query
  * \param iCol [input]: the column to query
  * \param eType [input]: the kind of data to set
  * \return the item data
  */
wxVariant wxVirtualDataViewCtrl::GetItemData(const wxVirtualItemID &rID, int iCol, EDataType eType) const
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(wxVariant());

    wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
    if (!pCol) return(wxVariant());
    size_t uiField = pCol->GetModelColumn();

    return(pDataModel->GetItemData(rID, uiField, (wxVirtualIDataModel::EDataType) eType));
}

/** Set item data
  * \param rID        [input]: the item to query
  * \param iCol       [input]: the column to query
  * \param vValue     [input]: the new value
  * \param bSendEvent [input]: true for sending an event
  * \param eType      [input]: the kind of data to set
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::SetItemData(const wxVirtualItemID &rID, int iCol,
                                        wxVariant vValue, bool bSendEvent, EDataType eType)
{
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(false);

    wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
    if (!pCol) return(false);
    size_t uiField = pCol->GetModelColumn();

    if (bSendEvent)
    {
        if (!m_pClientArea->SendItemValueChangingEvent(rID, uiField, iCol, vValue)) return(false);
    }
    if (!pDataModel->SetItemData(rID, uiField, vValue, (wxVirtualIDataModel::EDataType) eType)) return(false);
    if (bSendEvent) m_pClientArea->SendItemValueChangedEvent(rID, uiField, iCol, vValue);
    return(true);
}

//------------------------ ITEM EDITION -----------------------------//
/** Start the edition of an item
  * \param rID  [input]: the item to edit
  * \param iCol [input]: the column to edit
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::StartItemEdition(const wxVirtualItemID &rID, int iCol)
{
    if (!m_pClientArea) return(false);
    return(!m_pClientArea->StartItemEdition(rID, iCol));
}

/** Get the currently edited item
  * \return the ID of the currently edited item
  *         if the ID is invalid (IsInvalid() method), then no edition is currently active
  */
wxVirtualItemID wxVirtualDataViewCtrl::GetEditedItem(void) const
{
    if (!m_pClientArea) return(wxVirtualItemID());
    return(m_pClientArea->GetEditedItem());
}

/** Get the currently edited column
  * \return the ID of the currently edited column
  */
int wxVirtualDataViewCtrl::GetEditedColumn(void) const
{
    if (!m_pClientArea) return(-1);
    return(m_pClientArea->GetEditedColumn());
}

/** Provokes the end of the current edition
  * \param bCancelled [input] : true for cancelling the edition, false for accepting the new value
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCtrl::FinishEdition(bool bCancelled)
{
    if (!m_pClientArea) return(false);
    return(m_pClientArea->FinishEdition(bCancelled));
}

//----------------- GRAPHIC ATTRIBUTES ------------------------------//
/** Get the item graphic attributes
  * \param rID  [input]: the item to query
  * \param iCol [input]: the column to query
  * \param bMergeWithColAttrib [input]: if true, the result is a combination of
  *                                     column attribute + item attribute
  *                                     if false, the result is the item attribute only
  * \return the graphic attribute
  */
wxVirtualDataViewItemAttr wxVirtualDataViewCtrl::GetItemAttribute(const wxVirtualItemID &rID,
                                                                  int iCol,
                                                                  bool bMergeWithColAttrib)
{
    wxVirtualDataViewItemAttr oResult;

    //get models
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(oResult);

    wxVirtualIStateModel *pStateModel = DoGetStateModel();
    if (!pStateModel) return(oResult);

    //get state
    wxVirtualDataViewItemState oState;
    pStateModel->GetState(oState, rID);

    //get model column
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
    if (!pCol) return(oResult);
    size_t uiField = pCol->GetModelColumn();

    //get model attribute
    wxVirtualDataViewItemAttr* pAttr = pDataModel->GetItemAttribute(rID, uiField, oState);
    if (pAttr) oResult = *pAttr;

    if (bMergeWithColAttrib)
    {
        if (pCol->GetAttribute())
        {
            wxVirtualDataViewItemAttr *pColAttr = pCol->GetAttribute()->GetGraphicAttributes();
            if (pColAttr) oResult.Combine(*pColAttr);
        }
    }

    return(oResult);
}

//------------------------------ SEARCHING --------------------------//
/** Find the 1st item matching a criteria
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param bSelect        [input]: true if the matched item should be selected
  * \param bMakeVisible   [input]: true if the matched item should be scrolled into view
  * \param iCol           [input]: the column to examine. If <0 then all fields must be examined
  * \return the 1st item matching the criteria
  */
wxVirtualItemID wxVirtualDataViewCtrl::FindFirst(const wxVariant &rvValue,
                                                 bool bFullMatch, bool bCaseSensitive,
                                                 bool bRegEx,
                                                 bool bSelect, bool bMakeVisible, int iCol)
{
    //check
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(wxVirtualItemID());

    //search
    wxVirtualItemID id;
    if (iCol >= 0)
    {
        wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
        if (!pCol) return(wxVirtualItemID());
        size_t uiField = pCol->GetModelColumn();

        id = pDataModel->FindFirst(rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField);
    }
    else
    {
        id = pDataModel->FindFirst(rvValue, bFullMatch, bCaseSensitive, bRegEx);
    }

    //select
    if (bSelect)
    {
        DeselectAll(true);
        SelectItem(id, true, false, true);
    }

    //scroll into view
    if (bMakeVisible)
    {
        size_t uiCol = 0;
        if (iCol >= 0) uiCol = iCol;
        EnsureItemVisible(id, uiCol, WX_VISIBLE_DONT_CARE, false);
    }

    return(id);
}

/** Find the next item matching a criteria
  * \param rID            [input]: the starting item. It is never returned as a search result
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param bSelect        [input]: true if the matched item should be selected
  * \param bMakeVisible   [input]: true if the matched item should be scrolled into view
  * \param iCol           [input]: the column to examine. If <0 then all fields must be examined
  * \return the next item matching the criteria
  */
wxVirtualItemID wxVirtualDataViewCtrl::FindNext(const wxVirtualItemID &rID,
                                                const wxVariant &rvValue,
                                                bool bFullMatch, bool bCaseSensitive,
                                                bool bRegEx,
                                                bool bSelect, bool bMakeVisible, int iCol)
{
    //check
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(wxVirtualItemID());

    //search
    wxVirtualItemID id;
    if (iCol >= 0)
    {
        wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
        if (!pCol) return(wxVirtualItemID());
        size_t uiField = pCol->GetModelColumn();

        id = pDataModel->FindNext(rID, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField);
    }
    else
    {
        id = pDataModel->FindNext(rID, rvValue, bFullMatch, bCaseSensitive, bRegEx);
    }

    //select
    if (bSelect)
    {
        DeselectAll(true);
        SelectItem(id, true, false, true);
    }

    //scroll into view
    if (bMakeVisible)
    {
        size_t uiCol = 0;
        if (iCol >= 0) uiCol = iCol;
        EnsureItemVisible(id, uiCol, WX_VISIBLE_DONT_CARE, false);
    }

    return(id);
}

/** Find the previous item matching a criteria
  * \param rID            [input]: the starting item. It is never returned as a search result
  * \param rvValue        [input]: the criteria to match
  * \param bFullMatch     [input]: true for exact matching, false for partial matching
  * \param bCaseSensitive [input]: true for case sensitive matching, false otherwise
  * \param bRegEx         [input]: true for using regular expressions
  * \param bSelect        [input]: true if the matched item should be selected
  * \param bMakeVisible   [input]: true if the matched item should be scrolled into view
  * \param iCol           [input]: the column to examine. If <0 then all fields must be examined
  * \return the previous item matching the criteria
  */
wxVirtualItemID wxVirtualDataViewCtrl::FindPrev(const wxVirtualItemID &rID,
                                                const wxVariant &rvValue,
                                                bool bFullMatch, bool bCaseSensitive,
                                                bool bRegEx,
                                                bool bSelect, bool bMakeVisible, int iCol)
{
    //check
    wxVirtualIDataModel *pDataModel = DoGetDataModel();
    if (!pDataModel) return(wxVirtualItemID());

    //search
    wxVirtualItemID id;
    if (iCol >= 0)
    {
        wxVirtualDataViewColumn *pCol = Columns().GetColumn(iCol);
        if (!pCol) return(wxVirtualItemID());
        size_t uiField = pCol->GetModelColumn();

        id = pDataModel->FindPrev(rID, rvValue, bFullMatch, bCaseSensitive, bRegEx, uiField);
    }
    else
    {
        id = pDataModel->FindPrev(rID, rvValue, bFullMatch, bCaseSensitive, bRegEx);
    }

    //select
    if (bSelect)
    {
        DeselectAll(true);
        SelectItem(id, true, false, true);
    }

    //scroll into view
    if (bMakeVisible)
    {
        size_t uiCol = 0;
        if (iCol >= 0) uiCol = iCol;
        EnsureItemVisible(id, uiCol, WX_VISIBLE_DONT_CARE, false);
    }

    return(id);
}

//------------------------ SORTING ----------------------------------//
/** Sort all the items according to the current sort filter
  */
void wxVirtualDataViewCtrl::SortItems(void)
{
    if (!m_pClientArea) return;

    //special case : no more sorting
    wxVirtualIDataModel::TSortFilters vSortFilters;
    bool bIsSorting = false;
    size_t uiNbCols = m_vColumns.GetColumnsCount();
    size_t uiCol;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) continue;
        if (!pCol->IsSortKey()) continue;
        bIsSorting = true;

        size_t uiField = pCol->GetModelColumn();
        wxVirtualIDataModel::ESorting eSortOrder = wxVirtualIDataModel::WX_E_SORT_NOT_SORTING;
        if (pCol->IsSortOrderAscending())
        {
            eSortOrder = wxVirtualIDataModel::WX_E_SORT_ASCENDING;
        }
        else
        {
            eSortOrder = wxVirtualIDataModel::WX_E_SORT_DESCENDING;
        }
        wxVirtualIDataModel::TSort oSort(uiField, eSortOrder);
        vSortFilters.push_back(oSort);
    }

    //special case : not sorting at all
    if (!bIsSorting)
    {
        ClearSortFilters();
        return;
    }

    //can the data model sort on its own ?
    wxVirtualIDataModel *pDataModel = GetBaseDataModel();
    if (!pDataModel) return;
    if (pDataModel->CanSort(vSortFilters))
    {
        //the data model can sort
        DetachProxyModel(m_pSortingModel);
        m_pSortingModel = WX_VDV_NULL_PTR;
        pDataModel->Sort(vSortFilters);

        //filtering will give another order: clear cache
        if (m_pFilteringModel) m_pFilteringModel->ClearCache();
    }
    else
    {
        //the data model cannot sort -> use a proxy model
        //ensure that the sorting model has been allocated
        if (!m_pSortingModel)
        {
            m_pSortingModel = new wxVirtualSortingDataModel;
            AttachProxyModel(m_pSortingModel);
        }
        if (!m_pSortingModel) return;

        //if (m_pFilteringModel) m_pFilteringModel->ClearCache();
        m_pSortingModel->Sort(vSortFilters);
    }

    //refresh
    m_pClientArea->OnLayoutChanged();
    RefreshDataView();
}

/** Clear all sorting filters
  */
void wxVirtualDataViewCtrl::ClearSortFilters(void)
{
    if (!m_pSortingModel) return;

    //clear the sorting filters & refresh

    //remove the sorting proxy model if any
    DetachProxyModel(m_pSortingModel);
    m_pSortingModel = WX_VDV_NULL_PTR;

    //bottom data model: notify to remove sorting filters
    wxVirtualIDataModel *pDataModel = GetBaseDataModel();
    if (pDataModel) pDataModel->ResetSorting();

    //filtering will give another order: clear cache
    if (m_pFilteringModel) m_pFilteringModel->ClearCache();

    //refresh
    m_pClientArea->OnLayoutChanged();
    RefreshDataView();
}

//--------------------------- FILTERING -----------------------------//
/** Get the filter editor for a column
  * \param uiCol [input]: the column to query
  * \return a pointer to the column filter. Do not delete it : it still belongs to the column object
  *         If no filters are attached to this column, then NULL is returned
  */
wxVirtualDataViewIFilterEditor* wxVirtualDataViewCtrl::GetFilterEditor(size_t uiCol)
{
    size_t uiNbCols = m_vColumns.GetColumnsCount();
    if (uiCol >= uiNbCols) return(WX_VDV_NULL_PTR);

    //get column
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
    if (!pCol) return(WX_VDV_NULL_PTR);

    //get filter
    wxVirtualDataViewIFilterEditor *pFilterEditor = pCol->GetFilterEditor();
    return(pFilterEditor);
}

/** Set the filter editor for a column. You need to call ApplyFilter to put it in action
  * \param uiCol   [input]: the column to modify
  * \param pFilterEditor [input]: the new filter editor to set. Ownership is taken : it must have been allocated with "new"
  *                               the previous filter editor is replaced
  * \return true on success (ownership of the filter is taken)
  *         false on failure
  */
bool wxVirtualDataViewCtrl::SetFilterEditor(size_t uiCol, wxVirtualDataViewIFilterEditor *pFilterEditor)
{
    //if (!pFilterEditor) return(false); NULL is valid (remove editor)

    size_t uiNbCols = m_vColumns.GetColumnsCount();
    if (uiCol >= uiNbCols) return(false);

    //get column
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
    if (!pCol) return(false);

    //attributes
    wxVirtualDataViewCellAttr *pAttr = pCol->GetAttribute();
    if (!pAttr) return(false);

    //set filter
    pAttr->SetFilterEditor(pFilterEditor);
    return(true);
}

/** Clear all the filters
  */
void wxVirtualDataViewCtrl::ClearAllFilters(void)
{
    if (!m_pFilteringModel) return;

    //clear the sorting filters & refresh
    DetachProxyModel(m_pFilteringModel);
    m_pFilteringModel = WX_VDV_NULL_PTR;
    if (m_pSortingModel) m_pSortingModel->ClearCache();

    m_pClientArea->OnLayoutChanged();
    RefreshDataView();
}

/** Apply the current filters
  */
void wxVirtualDataViewCtrl::ApplyFilters(void)
{
    //special case : no more filtering
    bool bIsFiltering = false;
    size_t uiNbCols = m_vColumns.GetColumnsCount();
    size_t uiCol;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        //get column
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) continue;

        //get filter
        wxVirtualDataViewFilter *pFilter = pCol->GetFilter();
        if (!pFilter) continue;
        if (pFilter->IsActive())
        {
            bIsFiltering = true;
            break;
        }
    }

    if (!bIsFiltering)
    {
        ClearAllFilters();
        return;
    }

    //ensure that the filtering model has been allocated
    if (!m_pClientArea) return;
    if (!m_pFilteringModel)
    {
        m_pFilteringModel = new wxVirtualFilteringDataModel;
        AttachProxyModel(m_pFilteringModel);
    }
    if (!m_pFilteringModel) return;

    //set the filters
    m_pFilteringModel->ClearFilters();
    if (m_pSortingModel) m_pSortingModel->ClearCache();
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        //get column
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) continue;

        //get filter
        wxVirtualDataViewFilter *pFilter = pCol->GetFilter();
        if (!pFilter) continue;
        if (!pFilter->IsActive()) continue;

        size_t uiField = pCol->GetModelColumn();
        m_pFilteringModel->PushFilter(uiField, pFilter);
    }

    //refresh
    m_pClientArea->OnDataModelChanged();
    RefreshDataView();
}

/** Return true if filters are shown
  * \return true if filters are shown, false otherwise
  */
bool wxVirtualDataViewCtrl::AreFiltersShown(void) const
{
    if (m_pColumnHeaderWindow) return(m_pColumnHeaderWindow->AreFiltersShown());
    return(false);
}

/** Return true if filters are hidden
  * \return true if filters are hidden, false otherwise
  */
bool wxVirtualDataViewCtrl::AreFiltersHidden(void) const
{
    if (m_pColumnHeaderWindow) return(m_pColumnHeaderWindow->AreFiltersHidden());
    return(false);
}


/** Show / hide filters
  * \param bShow [input]: true for showing the filters, false for hiding them
  * Showing / hiding the filters will probably change the window size, so a re-layouting of the parent is
  * probably necessary. This task has to be done by the caller
  */
void wxVirtualDataViewCtrl::ShowFilters(bool bShow)
{
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->ShowFilters(bShow);
}

/** Hide / show filters
  * \param bHide [input]: true for hiding the filters, false for showing them
  * Showing / hiding the filters will probably change the window size, so a re-layouting of the parent is
  * probably necessary. This task has to be
  */
void wxVirtualDataViewCtrl::HideFilters(bool bHide)
{
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->HideFilters(bHide);
}

/** Show filters ON/OFF
  * Showing / hiding the filters will probably change the window size, so a re-layouting of the parent is
  * probably necessary. This task has to be
  */
void wxVirtualDataViewCtrl::ToggleFilters(void)
{
    if (m_pColumnHeaderWindow) m_pColumnHeaderWindow->ToggleFilters();
}

/** Start edition of a filter
  * \param uiCol [input]: the filter to activate
  * \return true if the filter was activated, false otherwise
  */
bool wxVirtualDataViewCtrl::StartFilter(size_t uiCol)
{
    if (m_pColumnHeaderWindow) return(m_pColumnHeaderWindow->StartFilter(uiCol));
    return(false);
}

/** Finish edition of a filter
  * \return true if the filter was deactivated, false otherwise
  */
bool wxVirtualDataViewCtrl::EndFilter(void)
{
    if (m_pColumnHeaderWindow) return(m_pColumnHeaderWindow->EndFilter());
    return(true);
}

/** Check if the filter is active and not empty
  * \param uiCol [input] : the index of the column to check
  * \return true if a filter is active and not empty
  *         false otherwise
  */
bool wxVirtualDataViewCtrl::IsFiltering(size_t uiCol)
{
    size_t uiNbCols = m_vColumns.GetColumnsCount();
    if (uiCol >= uiNbCols) return(false);

    //get column
    wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
    if (!pCol) return(false);

    //get filter editor
    wxVirtualDataViewIFilterEditor *pFilterEditor = pCol->GetFilterEditor();
    if (!pFilterEditor) return(false);

    //check
    if (!pFilterEditor->IsFiltering()) return(false);

    //get filter
    wxVirtualDataViewFilter* pFilter = pFilterEditor->GetFilter();
    if (!pFilter) return(false);

    //check
    if (!pFilter->IsActive()) return(false);
    return(true);

}

/** Check if at least one filter is active and not empty
  * \return true if at least 1 filter is active and not empty
  *         false otherwise
  */
bool wxVirtualDataViewCtrl::IsFiltering(void)
{
    size_t uiCol;
    size_t uiNbCols = m_vColumns.GetColumnsCount();
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        //get column
        wxVirtualDataViewColumn *pCol = m_vColumns.GetColumn(uiCol);
        if (!pCol) continue;

        //get filter editor
        wxVirtualDataViewIFilterEditor *pFilterEditor = pCol->GetFilterEditor();
        if (!pFilterEditor) continue;

        //check
        if (!pFilterEditor->IsFiltering()) continue;

        //get filter
        wxVirtualDataViewFilter* pFilter = pFilterEditor->GetFilter();
        if (!pFilter) continue;

        //check
        if (pFilter->IsActive()) return(true);
    }
    return(false);
}
