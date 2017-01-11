/**********************************************************************/
/** FILE    : VirtualDataViewBase.cpp                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a base class for viewing a very large amount of items  **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/DataView/VirtualDataViewBase.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/Models/VirtualIProxyDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDefaultStateModel.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualIModelRenderer.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualTreeModelRenderer.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

#ifndef WX_PRECOMP
    #include <wx/settings.h>
    #include <wx/dcclient.h>
    #include <wx/listbox.h>
#endif //WX_PRECOMP

#include <wx/dcbuffer.h>
#include <wx/selstore.h>
#include <wx/renderer.h>
#include <wx/log.h>


#define WX_DEBUG_VIRTUAL_DATA_VIEW_EVENT      0
#if WX_DEBUG_VIRTUAL_DATA_VIEW_EVENT != 0
    #define PRINT_MSG(x)    LogMessage(x)
#else
    #define PRINT_MSG(x)
#endif // WX_DEBUG_VIRTUAL_DATA_VIEW_EVENT


//--------------------- STATIC DATA ---------------------------------//
const char wxsVirtualDataViewBaseName[] = "wxVirtualDataViewBase";


//-------------- CONSTRUCTORS & DESTRUCTOR --------------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewBase, wxControl);

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewBase::wxVirtualDataViewBase(void)
    : wxSystemThemedControl<wxControl>(),
      m_pDataModel(WX_VDV_NULL_PTR)
{
    Init(WX_VDV_NULL_PTR, WX_VDV_NULL_PTR, WX_VDV_NULL_PTR);
}

/** Constructor
  * Creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent        [input]: parent of the window
  * \param id             [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos            [input]: initial position of the window. Default to wxDefaultPosition
  * \param size           [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle         [input]: window style. Default to 0
  * \param pDataModel     [input]: the default data model. Can be NULL (the user needs to provide it later using SetDataModel)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pStateModel    [input]: the default state model. Can be NULL (a wxVirtualDefaultStateModel is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pModelRenderer [input]: the default state model. Can be NULL (a wxVirtualTreeModelRenderer is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param sName          [input]: the name of the object
  */
wxVirtualDataViewBase::wxVirtualDataViewBase(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                             const wxSize& size, long lStyle,
                                             wxVirtualIDataModel *pDataModel,
                                             wxVirtualIStateModel *pStateModel,
                                             wxVirtualIModelRenderer *pModelRenderer,
                                             wxString sName)

    : wxSystemThemedControl<wxControl>(),
      m_pDataModel(WX_VDV_NULL_PTR),
      m_pStateModel(WX_VDV_NULL_PTR),
      m_pModelRenderer(WX_VDV_NULL_PTR)
{
    Init(pDataModel, pStateModel, pModelRenderer);
    Create(pParent, id, pos, size, lStyle, sName);
}

/** Destructor (virtual)
  */
wxVirtualDataViewBase::~wxVirtualDataViewBase(void)
{
    Release();
}

//------------------ INTERNAL METHODS -------------------------------//
/** Common parts of all constructors
  * \param pDataModel     [input]: the default data model. Can be NULL (the user needs to provide it later using SetDataModel)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pStateModel    [input]: the default state model. Can be NULL (a wxVirtualDefaultStateModel is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pModelRenderer [input]: the default state model. Can be NULL (a wxVirtualTreeModelRenderer is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param sName          [input]: the name of the object
  */
void wxVirtualDataViewBase::Init(wxVirtualIDataModel *pDataModel,
                                 wxVirtualIStateModel *pStateModel,
                                 wxVirtualIModelRenderer *pModelRenderer)
{
    m_pOwner            = WX_VDV_NULL_PTR;
    m_pDataModel        = pDataModel;

    m_pStateModel       = pStateModel;
    if (!m_pStateModel) m_pStateModel = new wxVirtualDefaultStateModel();

    m_pModelRenderer    = pModelRenderer;
    if (!m_pModelRenderer) m_pModelRenderer = new wxVirtualTreeModelRenderer(this, NULL);
}

/** Release internal resources
  */
void wxVirtualDataViewBase::Release(void)
{
    if (m_pDataModel) delete(m_pDataModel);
    m_pDataModel = WX_VDV_NULL_PTR;

    if (m_pStateModel) delete(m_pStateModel);
    m_pStateModel = WX_VDV_NULL_PTR;

    if (m_pModelRenderer) delete(m_pModelRenderer);
    m_pModelRenderer = WX_VDV_NULL_PTR;
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
bool wxVirtualDataViewBase::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                   const wxSize& size, long lStyle, wxString sName)
{
#ifdef __WXMSW__
    if ((lStyle & wxBORDER_MASK) == wxDEFAULT) lStyle |= wxBORDER_THEME;
#endif

    m_pOwner = (wxVirtualDataViewCtrl*) pParent;
    if (m_pModelRenderer) m_pModelRenderer->SetScrollerWindow(m_pOwner);

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
    //painting
    Bind(wxEVT_PAINT                 , &wxVirtualDataViewBase::OnPaintEvent    , this);
    Bind(wxEVT_ERASE_BACKGROUND      , &wxVirtualDataViewBase::OnEraseEvent    , this);
    //sizing & scrolling
    Bind(wxEVT_SIZE                  , &wxVirtualDataViewBase::OnSizeEvent     , this);
    Bind(wxEVT_SCROLLWIN_TOP         , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_BOTTOM      , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_LINEUP      , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_LINEDOWN    , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_PAGEUP      , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_PAGEDOWN    , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK  , &wxVirtualDataViewBase::OnScrollEvent   , this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &wxVirtualDataViewBase::OnScrollEvent   , this);
    //keyboard & focus
    Bind(wxEVT_KEY_DOWN              , &wxVirtualDataViewBase::OnKeyboardEvent , this);
    Bind(wxEVT_KEY_UP                , &wxVirtualDataViewBase::OnKeyboardEvent , this);
    Bind(wxEVT_CHAR                  , &wxVirtualDataViewBase::OnKeyboardEvent , this);
    Bind(wxEVT_CHAR_HOOK             , &wxVirtualDataViewBase::OnKeyboardEvent , this);
    Bind(wxEVT_SET_FOCUS             , &wxVirtualDataViewBase::OnSetOrKillFocus, this);
    Bind(wxEVT_KILL_FOCUS            , &wxVirtualDataViewBase::OnSetOrKillFocus, this);
    //mouse event
    Bind(wxEVT_LEFT_DOWN             , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_LEFT_UP               , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_LEFT_DCLICK           , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_MIDDLE_DOWN           , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_MIDDLE_UP             , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_MIDDLE_DCLICK         , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_RIGHT_DOWN            , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_RIGHT_UP              , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_RIGHT_DCLICK          , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_AUX1_DOWN             , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_AUX1_UP               , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_AUX1_DCLICK           , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_AUX2_DOWN             , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_AUX2_UP               , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_AUX2_DCLICK           , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_MOTION                , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_ENTER_WINDOW          , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_LEAVE_WINDOW          , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_MOUSEWHEEL            , &wxVirtualDataViewBase::OnMouseEvent    , this);
    Bind(wxEVT_MAGNIFY               , &wxVirtualDataViewBase::OnMouseEvent    , this);
    //editor
    Bind(wxEVT_VDV_EDITION_FINISHED  , &wxVirtualDataViewBase::OnEditionEnded  , this);
    //other events
    Bind(wxEVT_CONTEXT_MENU          , &wxVirtualDataViewBase::OnContextMenu   , this);
    Bind(wxEVT_HELP                  , &wxVirtualDataViewBase::OnHelpEvent     , this);

    return(true);
}

//--------------------- WINDOWS MANAGEMENT --------------------------//
#ifdef __WXUNIVERSAL__
/** Check if the current window is the current one
  * \return true if the window is the current one, false otherwise
  */
bool wxVirtualDataViewBase::IsCurrent(void) const
{
    return(wxVScrolledWindow::IsCurrent());
}
#endif

//------------------------ VISUAL ATTRIBUTES ------------------------//
/** Get the default window attributes
  * \return the default visual attributes
  */
wxVisualAttributes wxVirtualDataViewBase::GetDefaultAttributes(void) const
{
    return(DoGetDefaultAttributes());
}

/** Get the default window attributes - static
  * \param variant [input] : what attribute to get. Default value = wxWINDOW_VARIANT_NORMAL
  * \return the visual attributes
  */
wxVisualAttributes wxVirtualDataViewBase::GetClassDefaultAttributes(wxWindowVariant variant)
{
    return(wxListBox::GetClassDefaultAttributes(variant));
}

/** Get the default window attributes
  * \return the default window attributes
  */
wxVisualAttributes wxVirtualDataViewBase::DoGetDefaultAttributes(void) const
{
    return(GetClassDefaultAttributes(GetWindowVariant()));
}

//------------------- OWNER WINDOW ----------------------------------//
/** Get the owner window
  * \return the owner window
  */
wxVirtualDataViewCtrl* wxVirtualDataViewBase::GetOwner(void) const
{
    return(m_pOwner);
}

/** Set the owner window
  * \param pOwner [input]: the new owner window
  */
void wxVirtualDataViewBase::SetOwner(wxVirtualDataViewCtrl* pOwner)
{
    m_pOwner = pOwner;
}

//------------------ DATA MODEL -------------------------------------//
/** Get the current data model used, without any proxies
  * \return the current data model without all the proxies filters
  */
wxVirtualIDataModel* wxVirtualDataViewBase::GetBaseDataModel(void) const
{
    wxVirtualIDataModel *pDataModel = m_pDataModel;
    if (!pDataModel) return(WX_VDV_NULL_PTR);
    while (pDataModel->IsProxyDataModel())
    {
        wxVirtualIProxyDataModel *pProxyModel = reinterpret_cast<wxVirtualIProxyDataModel*>(pDataModel);
        pDataModel = pProxyModel->GetMasterModel();
        if (!pDataModel) return(WX_VDV_NULL_PTR);
    }

    return(pDataModel);
}

/** Get the current data model used, with all proxies if any
  * \return the current data model used.
  *         Do not delete the pointer : it is still owned by the control
  */
wxVirtualIDataModel* wxVirtualDataViewBase::GetDataModel(void) const
{
    return(m_pDataModel);
}

/** Set the current data model. Current proxies will be kept
  * \param pModel [input] : the new data model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewBase::SetDataModel(wxVirtualIDataModel *pModel)
{
    if (m_pDataModel == pModel) return;
    if (m_pDataModel) delete(m_pDataModel);
    m_pDataModel = pModel;

    if (m_pModelRenderer) m_pModelRenderer->OnDataModelChanged();
    if (m_pStateModel) m_pStateModel->OnDataModelChanged(m_pDataModel);
    InvalidateBestSize();
    if (m_pOwner) m_pOwner->InvalidateBestSize();
}


//--------------------- PROXY DATA MODELS ---------------------------//
/** Attach a proxy data model to the chain of models
  * \param pModel [input]: the proxy model to attach. Ownership is taken
  *                        It must have been allocated with "new"
  */
void wxVirtualDataViewBase::AttachProxyModel(wxVirtualIProxyDataModel *pModel)
{
    pModel->AttachMasterModel(m_pDataModel);
    m_pDataModel = pModel;

    if (m_pStateModel) m_pStateModel->OnDataModelChanged(m_pDataModel);
}

/** Remove the last proxy data model from the chain of models
  */
void wxVirtualDataViewBase::PopProxyModel(void)
{
    wxVirtualIDataModel *pDataModel = m_pDataModel;
    if (!pDataModel) return;
    if (pDataModel->IsProxyDataModel())
    {
        wxVirtualIProxyDataModel *pProxyModel = reinterpret_cast<wxVirtualIProxyDataModel*>(pDataModel);
        m_pDataModel = pProxyModel->DetachMasterModel();
        delete(pDataModel);
    }

    if (m_pStateModel) m_pStateModel->OnDataModelChanged(m_pDataModel);
}

/** Remove a proxy data model from the chain of models
  * \param pModel [input]: the model to remove. It will be also deleted
  */
void wxVirtualDataViewBase::DetachProxyModel(wxVirtualIProxyDataModel *pModel)
{
    if (!pModel) return;

    wxVirtualIDataModel *pDataModel = m_pDataModel;
    if (!pDataModel) return;

    wxVirtualIDataModel *pPrevious = pDataModel;
    while (pDataModel->IsProxyDataModel())
    {
        wxVirtualIProxyDataModel *pProxyModel = reinterpret_cast<wxVirtualIProxyDataModel*>(pDataModel);

        if (pDataModel == pModel)
        {
            //we have : pPrevious -> pDataModel -> pMasterModel
            //detach the master model of the found proxy model
            wxVirtualIDataModel *pMasterModel = pProxyModel->DetachMasterModel();

            //now plug the master model back in the chain
            //we have : pPrevious -> pDataModel -> NULL
            //we want : pPrevious -> pMasterModel
            //special case : m_pDataModel == pModel
            //               in that case, pPrevious == pModel == m_pDataModel
            if (pPrevious == m_pDataModel)
            {
                //top node
                m_pDataModel = pMasterModel;
            }
            else
            {
                pProxyModel = reinterpret_cast<wxVirtualIProxyDataModel*>(pPrevious);
                pProxyModel->AttachMasterModel(pMasterModel);

                //we now have : pPrevious -> pMasterModel
            }
            delete(pModel);
            return;
        }

        pPrevious = pDataModel;
        pDataModel = pProxyModel->GetMasterModel();
    }

    if (m_pStateModel) m_pStateModel->OnDataModelChanged(m_pDataModel);
}

/** Remove all proxy data models from the chain of models
  */
void wxVirtualDataViewBase::DetachAllProxyModels(void)
{
    wxVirtualIDataModel *pDataModel = m_pDataModel;
    if (!pDataModel) return;

    while (pDataModel->IsProxyDataModel())
    {
        wxVirtualIProxyDataModel *pProxyModel = reinterpret_cast<wxVirtualIProxyDataModel*>(pDataModel);
        m_pDataModel = pProxyModel->DetachMasterModel();
        delete(pDataModel);
        pDataModel = m_pDataModel;
    }

    if (m_pStateModel) m_pStateModel->OnDataModelChanged(m_pDataModel);
}

//--------------------- STATE MODEL ---------------------------------//
/** Get the state model
  * \return the current data model used.
  *         Do not delete the pointer : it is still owned by the control
  */
wxVirtualIStateModel* wxVirtualDataViewBase::GetStateModel(void) const
{
    return(m_pStateModel);
}

/** Set the state model
  * \param pStateModel [input] : the new state model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewBase::SetStateModel(wxVirtualIStateModel *pStateModel)
{
    if (m_pStateModel == pStateModel) return;
    if (m_pStateModel) delete(m_pStateModel);
    m_pStateModel = pStateModel;

    if (m_pModelRenderer) m_pModelRenderer->OnLayoutChanged();

    InvalidateBestSize();
    if (m_pOwner) m_pOwner->InvalidateBestSize();
}

//---------------------- MODEL RENDERER -----------------------------//
/** Get the model renderer
  * \return the model renderer
  *          Do not delete the pointer : it is still owned by the control
  */
wxVirtualIModelRenderer* wxVirtualDataViewBase::GetModelRenderer(void) const
{
    return(m_pModelRenderer);
}

/** Set the model renderer
  * \param pModelRenderer [input] : the new model renderer to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewBase::SetModelRenderer(wxVirtualIModelRenderer *pModelRenderer)
{
    if (m_pModelRenderer == pModelRenderer) return;
    if (m_pModelRenderer) delete(m_pModelRenderer);
    m_pModelRenderer = pModelRenderer;
    if (m_pModelRenderer)
    {
        m_pModelRenderer->SetClientWindow(this);
        m_pModelRenderer->OnDataModelChanged();
    }
}

//------------------- INTERFACE : EVENT HANDLING --------------------//
/** Paint handler
  * \param rEvent [input]: the event data
  */
void wxVirtualDataViewBase::OnPaintEvent(wxPaintEvent& WXUNUSED(rEvent))
{
    wxAutoBufferedPaintDC dc(this);
//    wxPaintDC dc(this);

    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->PaintModel(dc)) return;
    }

    //failed somehow
    //fill it with background colour
    dc.SetBackground(GetBackgroundColour());
    dc.Clear();
}

/** Erase background event handler
  * \param rEvent [input]: the event data
  */
void wxVirtualDataViewBase::OnEraseEvent(wxEraseEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->EraseBackgroundEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** Sizing event handler
  * \param rEvent [input]: the event data
  */
void wxVirtualDataViewBase::OnSizeEvent(wxSizeEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->SizeEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** A scroll event occurred
  * \param rEvent [input]: the event data
  */
void wxVirtualDataViewBase::OnScrollEvent(wxScrollWinEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->ScrollEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** Focus changed handler
  */
void wxVirtualDataViewBase::OnSetOrKillFocus(wxFocusEvent& rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->FocusEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** Keyboard event
  */
void wxVirtualDataViewBase::OnKeyboardEvent(wxKeyEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->KeyboardEvent(rEvent)) return;
    }
    rEvent.Skip();
}


/** Mouse event
  */
void wxVirtualDataViewBase::OnMouseEvent(wxMouseEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->MouseEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** A context menu event occurred
  */
void wxVirtualDataViewBase::OnContextMenu(wxContextMenuEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->ContextMenuEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** A help event occurred
  */
void wxVirtualDataViewBase::OnHelpEvent(wxHelpEvent &rEvent)
{
    if (m_pModelRenderer)
    {
        if (m_pModelRenderer->HelpEvent(rEvent)) return;
    }
    rEvent.Skip();
}

/** End edition of a cell
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewBase::OnEditionEnded(wxCommandEvent &rEvent)
{
    if (!m_pModelRenderer)
    {
        rEvent.Skip();
        return;
    }

    int iCancelled = rEvent.GetInt(); //0 for cancelled
    bool bIsCancelled = (iCancelled == 0);
    if (m_pModelRenderer->FinishEdition(bIsCancelled)) return;
    rEvent.Skip();
}

//-------------------- REPAINT --------------------------------------//
/** Update the display
  */
void wxVirtualDataViewBase::UpdateDisplay(void)
{

    if (m_pModelRenderer)
    {
        wxClientDC dc(this);
        wxBufferedDC bdc(&dc);
        m_pModelRenderer->PaintModel(bdc);
    }
}

/** Update the display
  * \param rDC [input] : the DC to use
  */
void wxVirtualDataViewBase::UpdateDisplay(wxDC &rDC)
{
    if (m_pModelRenderer) m_pModelRenderer->PaintModel(rDC);
}

//---------------------- ITEM EDITION -------------------------------//
/** Start the edition of an item
  * \param rID  [input]: the item to edit
  * \param iCol [input]: the column to edit
  * \return true on success, false on failure
  */
bool wxVirtualDataViewBase::StartItemEdition(const wxVirtualItemID &rID, int iCol)
{
    if (m_pModelRenderer) return(m_pModelRenderer->StartEdition(rID, iCol));
    return(false);
}

/** Get the currently edited item
  * \return the ID of the currently edited item
  *         if the ID is invalid (IsInvalid() method), then no edition is currently active
  */
wxVirtualItemID wxVirtualDataViewBase::GetEditedItem(void) const
{
    if (!m_pStateModel) return(wxVirtualItemID());
    return(m_pStateModel->GetFocusedItem());
}

/** Get the currently edited column
  * \return the ID of the currently edited column
  */
int wxVirtualDataViewBase::GetEditedColumn(void) const
{
    return(GetFocusedColumn());
}

/** End the edition, if any
  * \param bCancel [input]: true for cancelling the edition, false for validating it
  * \return true on success, false on failure
  */
bool wxVirtualDataViewBase::FinishEdition(bool bCancel)
{
    if (m_pModelRenderer) return(m_pModelRenderer->FinishEdition(bCancel));
    return(false);
}

//-------------- CALLBACKS FOR DATA CHANGE --------------------------//
/** Called by the client to notify that the data model has changed
  * This happens when the data model has changed (called automatically by SetDataModel)
  * or when the data inside the model has changed
  */
void wxVirtualDataViewBase::OnDataModelChanged(void)
{
    if (m_pModelRenderer) m_pModelRenderer->OnDataModelChanged();
    if (m_pStateModel) m_pStateModel->OnDataModelChanged(m_pDataModel);
    InvalidateBestSize();
    if (m_pOwner) m_pOwner->InvalidateBestSize();
}

/** Called by the client to notify changes in layout
  */
void wxVirtualDataViewBase::OnLayoutChanged(void)
{
    if (m_pModelRenderer) m_pModelRenderer->OnLayoutChanged();
}

//----------------- SENDING EVENTS ----------------------------------//
/** Print a log message
  * \param sMsg [input]: the message to print
  */
void wxVirtualDataViewBase::LogMessage(wxString sMsg)
{
    wxLogMessage(sMsg);
}

/** Initialize an event
  * \param rEvent   [output]: the event to initialize
  * \param rID      [input] : the ID of the item impacted by the event
  * \param uiField  [input] : the field of the item impacted by the event
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  */
inline void wxVirtualDataViewBase::InitEvent(wxVirtualDataViewEvent &rEvent,
                                             const wxVirtualItemID &rID,
                                             size_t uiField, size_t uiCol)
{
    rEvent.SetItem(rID);
    rEvent.SetFieldIndex(uiField);
    rEvent.SetColumnIndex(uiCol);
    wxVirtualDataViewColumn *pCol = WX_VDV_NULL_PTR;
    if (m_pOwner) pCol = m_pOwner->Columns().GetColumn(uiCol);
    rEvent.SetColumn(pCol);
}

/** Send an event
  * \param rEvent     [input]: the event to send
  * \return true if the event has been processed (and no more propagation needed)
  *         false if the event has not been processed or has been skipped
  */
inline bool wxVirtualDataViewBase::SendEvent(wxVirtualDataViewEvent &rEvent)
{
    if (!m_pOwner) return(false);
    return(m_pOwner->SendEvent(rEvent)); //processed and not skipped
}

/** Send an event
  * \param eType    [input]: the kind of event to send
  * \param rID      [input]: the impacted item ID
  * \param uiField  [input]: the impacted field
  * \param uiCol    [input]: the impacted column
  * \return true if the event has been processed (and no more propagation needed)
  *         false if the event has not been processed or has been skipped
  */
bool wxVirtualDataViewBase::SendEvent(wxEventType eType, const wxVirtualItemID &rID,
                                      size_t uiField, size_t uiCol)
{
    wxVirtualDataViewEvent e(eType);
    InitEvent(e, rID, uiField, uiCol);
    return(SendEvent(e));
}

/** Send an event and check if it was vetoed
  * \param eType    [input]: the kind of event to send
  * \param rID      [input]: the impacted item ID
  * \param uiField  [input]: the impacted field
  * \param uiCol    [input]: the impacted column
  * \return true if the event has been processed and accepted (and no more propagation needed)
  *         false if the event has not been processed or has been skipped or has been vetoed
  */
bool wxVirtualDataViewBase::SendEventAndCheck(wxEventType eType, const wxVirtualItemID &rID,
                                              size_t uiField, size_t uiCol)
{
    wxVirtualDataViewEvent e(eType);
    InitEvent(e, rID, uiField, uiCol);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send event when item is expanding (sent before expansion occurs)
  * \param rID [input]: the item ID which is expanding
  * \return true if the event has been accepted (expanding allowed)
  *         false if the event has been rejected (expanding forbidden)
  */
bool wxVirtualDataViewBase::SendEventExpanding(const wxVirtualItemID &rID)
{
    PRINT_MSG(wxString::Format("Item expanding id = %d", rID.GetInt32()));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_EXPANDING);
    InitEvent(e, rID, 0, 0);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send event when item is collapsing (sent before collapse occurs)
  * \param rID [input]: the item ID which is collapsing
  * \return true if the event has been accepted (collapsing allowed)
  *         false if the event has been rejected (collapsing forbidden)
  */
bool wxVirtualDataViewBase::SendEventCollapsing(const wxVirtualItemID &rID)
{
    PRINT_MSG(wxString::Format("Item collapsing id = %d", rID.GetInt32()));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_COLLAPSING);
    InitEvent(e, rID, 0, 0);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send event when item is expanded (sent after expansion occurs)
  * \param rID [input]: the item ID which is expanded
  */
void wxVirtualDataViewBase::SendEventExpanded(const wxVirtualItemID &rID)
{
    PRINT_MSG(wxString::Format("Item expanded id = %d", rID.GetInt32()));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_EXPANDED);
    InitEvent(e, rID, 0, 0);
    SendEvent(e);
}

/** Send event when item is collapsed (sent after collapse occurs)
  * \param rID [input]: the item ID which is collapsing
  */
void wxVirtualDataViewBase::SendEventCollapsed(const wxVirtualItemID &rID)
{
    PRINT_MSG(wxString::Format("Item collapsed id = %d", rID.GetInt32()));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_COLLAPSED);
    InitEvent(e, rID, 0, 0);
    SendEvent(e);
}

/** Send event when item is checking (sent before check state changes)
  * \param rID [input]: the item ID which is checking
  * \return true if the event has been accepted (check state change allowed)
  *         false if the event has been rejected (check state change forbidden)
  */
bool wxVirtualDataViewBase::SendEventChecking(const wxVirtualItemID &rID)
{
    PRINT_MSG(wxString::Format("Item checking id = %d", rID.GetInt32()));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_CHECKING);
    InitEvent(e, rID, 0, 0);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send event when item is checked (sent after check state change occurs)
  * \param rID [input]: the item ID which is checked
  */
void wxVirtualDataViewBase::SendEventChecked(const wxVirtualItemID &rID)
{
    PRINT_MSG(wxString::Format("Item collapsed id = %d", rID.GetInt32()));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_CHECKED);
    InitEvent(e, rID, 0, 0);
    SendEvent(e);
}

/** Send mouse move event on item
  * \param rID     [input]: the item receiving the mouse event
  * \param rHT [input]: hit test information
  * \param rEvent     [input]: full mouse event information
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendEventMouseEvent(const wxVirtualIModelRenderer::THitTest &rHT,
                                                  wxMouseEvent &rEvent)
{
    PRINT_MSG(wxString::Format("Mouse event on item id = %d field = %d", rHT.m_ID.GetInt32(), rHT.m_uiField));

    wxVirtualDataViewEvent e(wxEVT_VDV_MOUSE_EVENT);
    InitEvent(e, rHT.m_ID, rHT.m_uiField, rHT.m_uiColIndex);
    e.SetPosition(rEvent.GetX(), rEvent.GetY());
    e.SetMouseEvent(rEvent);

    e.SetItemRect(rHT.m_RectItem);
    e.SetHitTestFlags(rHT.m_uiFlags);
    e.SetRelativePosition(rHT.m_RelativeX, rHT.m_RelativeY);

    return(SendEvent(e));
}

/** Send a keyboard event on item
  * \param rID     [input]: the item receiving the mouse event
  * \param uiField [input]: the field of the item receiving the mouse event
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \param rEvent     [input]: full keyboard event information
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendKeyboardEvent(const wxVirtualItemID &rID, size_t uiField,
                                              size_t uiCol, wxKeyEvent &rEvent)
{
    PRINT_MSG(wxString::Format("Key event id = %d field = %d", rID.GetInt32(), uiField));

    wxVirtualDataViewEvent e(wxEVT_VDV_KEYBOARD_EVENT);
    InitEvent(e, rID, uiField, uiCol);
    e.SetPosition(rEvent.GetX(), rEvent.GetY());
    return(SendEvent(e));
}

/** Send scrolling event
  * \param rEvent [input]: the full event information
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendScrollEvent(wxScrollWinEvent &rEvent)
{
    PRINT_MSG(wxString::Format("Scroll event"));

    wxWindow* pWin = m_pOwner;
    if (!pWin) pWin = this;
    rEvent.SetEventObject(pWin);
    rEvent.SetId(pWin->GetId());

    bool bResult = pWin->GetEventHandler()->ProcessEvent(rEvent); //true if processed and not skipped
    return(bResult);
}

/** Send a selection changing event
  * \param vOld [input / output]: the old selection. Array is moved to the event object
  * \param vNew [input / output]: the new selection. Array is moved to the event object
  * \return true if the event has been accepted (selection changed allowed)
  *         false if the event has been rejected (selection changed forbidden)
  */
bool wxVirtualDataViewBase::SendSelectionChangingEvent(wxVirtualItemIDs &vOld,
                                                       wxVirtualItemIDs &vNew)
{
    PRINT_MSG(wxString::Format("Selection changing event"));

    wxVirtualDataViewEvent e(wxEVT_VDV_SELECTION_CHANGING);
    wxVirtualItemID id;
    InitEvent(e, id, 0, 0);
    e.AssignDeselectedItems(vOld);
    e.AssignSelectedItems(vNew);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send a selection changed event
  * \param vOld [input / output]: the old selection. Array is moved to the event object
  * \param vNew [input / output]: the new selection. Array is moved to the event object
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendSelectionChangedEvent(wxVirtualItemIDs &vOld,
                                                      wxVirtualItemIDs &vNew)
{
    PRINT_MSG(wxString::Format("Selection changed event"));

    wxVirtualDataViewEvent e(wxEVT_VDV_SELECTION_CHANGED);
    wxVirtualItemID id;
    InitEvent(e, id, 0, 0);
    e.AssignDeselectedItems(vOld);
    e.AssignSelectedItems(vNew);
    return(SendEvent(e));
}

/** Send a selection changing event
  * \param vOld [input]: the old selection. Array is copied to the event object
  * \param vNew [input]: the new selection. Array is copied to the event object
  * \return true if the event has been accepted (selection changed allowed)
  *         false if the event has been rejected (selection changed forbidden)
  */
bool wxVirtualDataViewBase::SendSelectionChangingEventByCopy(const wxVirtualItemIDs &vOld,
                                                             const wxVirtualItemIDs &vNew)
{
    PRINT_MSG(wxString::Format("Selection changing event"));

    wxVirtualDataViewEvent e(wxEVT_VDV_SELECTION_CHANGING);
    wxVirtualItemID id;
    InitEvent(e, id, 0, 0);
    e.SetDeselectedItems(vOld);
    e.SetSelectedItems(vNew);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send a selection changed event
  * \param vOld [input]: the old selection. Array is copied to the event object
  * \param vNew [input]: the new selection. Array is copied to the event object
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendSelectionChangedEventByCopy(const wxVirtualItemIDs &vOld,
                                                            const wxVirtualItemIDs &vNew)
{
    PRINT_MSG(wxString::Format("Selection changed event"));

    wxVirtualDataViewEvent e(wxEVT_VDV_SELECTION_CHANGED);
    wxVirtualItemID id;
    InitEvent(e, id, 0, 0);
    e.SetDeselectedItems(vOld);
    e.SetSelectedItems(vNew);
    return(SendEvent(e));
}

/** Send a get focus / lose focus event
  * \param rID [input]: the item getting or losing the focus
  * \param uiField [input]: the field of the item getting or losing the focus
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \param bFocused [input]: true if the item is getting the focus
  *                          false if the item is losing the focus
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendItemFocusedEvent(const wxVirtualItemID &rID,
                                                   size_t uiField, size_t uiCol, bool bFocused)
{
    PRINT_MSG(wxString::Format("Focus event id = %d field = %d", rID.GetInt32(), uiField));

    int iType = wxEVT_VDV_ITEM_GET_FOCUS;
    if (!bFocused) iType = wxEVT_VDV_ITEM_LOSE_FOCUS;
    wxVirtualDataViewEvent e(iType);
    InitEvent(e, rID, uiField, uiCol);
    return(SendEvent(e));
}

/** Send a context menu event
  * \param rID [input]: the item used for context menu. Can be invalid
  * \param uiField [input]: the field of the item.
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \param x   [input]: the x coordinate of the event
  * \param y   [input]: the y coordinate of the event
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendContextMenuEvent(const wxVirtualItemID &rID,
                                                   size_t uiField, size_t uiCol, int x, int y)
{
    PRINT_MSG(wxString::Format("Context menu event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_CONTEXT_MENU);
    InitEvent(e, rID, uiField, uiCol);
    e.SetPosition(x, y);
    return(SendEvent(e));
}

/** Activate the item
  * \param rID     [input]: the item to activate
  * \param uiField [input]: the field of the item to activate
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendActivationEvent(const wxVirtualItemID &rID,
                                                  size_t uiField, size_t uiCol)
{
    PRINT_MSG(wxString::Format("Activation event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_ACTIVATED);
    InitEvent(e, rID, uiField, uiCol);
    return(SendEvent(e));
}

/** Send an event when item edition starts
  * \param rID     [input]: the ID of the edited item
  * \param uiField [input]: the field of the edited item
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \return true if the event has been accepted (item edition allowed)
  *         false if the event has been rejected (item edition forbidden)
  */
bool wxVirtualDataViewBase::SendStartEditingEvent(const wxVirtualItemID &rID,
                                                  size_t uiField, size_t uiCol)
{
    PRINT_MSG(wxString::Format("Start editing event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_START_EDITING);
    InitEvent(e, rID, uiField, uiCol);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send an event after item edition has started
  * \param rID     [input]: the ID of the edited item
  * \param uiField [input]: the field of the edited item
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendEditionStartedEvent(const wxVirtualItemID &rID,
                                                    size_t uiField, size_t uiCol)
{
    PRINT_MSG(wxString::Format("Edition started event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_EDITING_STARTED);
    InitEvent(e, rID, uiField, uiCol);
    return(SendEvent(e));
}

/** Send an event after item edition finished
  * \param rID     [input]: the ID of the edited item
  * \param uiField [input]: the field of the edited item
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendEditingEndEvent(const wxVirtualItemID &rID,
                                                size_t uiField, size_t uiCol, bool bCancel)
{
    PRINT_MSG(wxString::Format("Edition done event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_EDITING_DONE);
    InitEvent(e, rID, uiField, uiCol);
    e.SetEditCanceled(bCancel);
    return(SendEvent(e));
}

/** Send an event when item value is changing
  * \param rID     [input]: the ID of the modified item
  * \param uiField [input]: the field of the modified item
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \param rvValue [input]: the new value of the item
  * \return true if the event has been accepted (item modification allowed)
  *         false if the event has been rejected (item modification forbidden)
  */
bool wxVirtualDataViewBase::SendItemValueChangingEvent(const wxVirtualItemID &rID,
                                                       size_t uiField, size_t uiCol,
                                                       const wxVariant &rvValue)
{
    PRINT_MSG(wxString::Format("Changing item value event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_VALUE_CHANGING);
    InitEvent(e, rID, uiField, uiCol);
    e.SetValue(rvValue);
    if (!SendEvent(e)) return(true); //not processed or not vetoed
    return(e.IsAllowed());
}

/** Send an event when item changed value
  * \param rID     [input]: the ID of the modified item
  * \param uiField [input]: the field of the modified item
  * \param uiCol   [input]: the column index of the modified item (for column reordering)
  * \param rvValue [input]: the new value of the item
  * \return true if the event has been processed (no further processing)
  *         false if the event has not been processed
  */
bool wxVirtualDataViewBase::SendItemValueChangedEvent(const wxVirtualItemID &rID,
                                                      size_t uiField, size_t uiCol,
                                                      const wxVariant &rvValue)
{
    UpdateDisplay();

    PRINT_MSG(wxString::Format("Change item value event id = %d field = %d", rID.GetInt32(), uiField));
    wxVirtualDataViewEvent e(wxEVT_VDV_ITEM_VALUE_CHANGED);
    InitEvent(e, rID, uiField, uiCol);
    e.SetValue(rvValue);
    return(SendEvent(e));
}

//------------------ FOCUSED COLUMN ---------------------------------//
/** Get focused column ID
  * \return the focused column ID
  */
int wxVirtualDataViewBase::GetFocusedColumn(void) const
{
    if (!m_pModelRenderer) return(0);
    return(m_pModelRenderer->GetFocusedField());
}

/** Get focused column ID
  * \param iCol [input] the focused column ID
  */
void wxVirtualDataViewBase::SetFocusedColumn(int iCol)
{
    if (!m_pModelRenderer) return;
    m_pModelRenderer->SetFocusedField(iCol);
}


/** Refresh the headers windows
  */
void wxVirtualDataViewBase::RefreshHeaders(void)
{
    if (m_pOwner) m_pOwner->RefreshHeaders();
}

//---------------------- COLUMN RESIZING ----------------------------//
/** Start resizing a column
  * \param uiCol     [input]: the impacted column
  * \param iNewWidth [input]: the new column width
  */
void wxVirtualDataViewBase::StartResizingColumn(size_t uiCol, int iNewWidth)
{
    if (m_pOwner) m_pOwner->StartResizingColumn(uiCol, iNewWidth);
}

/** Currently resizing a column
  * \param uiCol     [input]: the impacted column
  * \param iNewWidth [input]: the new column width
  */
void wxVirtualDataViewBase::ResizingColumn(size_t uiCol, int iNewWidth)
{
    if (m_pOwner) m_pOwner->ResizingColumn(uiCol, iNewWidth);
}

/** Finish resizing a column
  * \param uiCol     [input]: the impacted column
  * \param iNewWidth [input]: the new column width
  */
void wxVirtualDataViewBase::ResizeColumn(size_t uiCol, int iNewWidth)
{
    if (m_pOwner) m_pOwner->ResizeColumn(uiCol, iNewWidth);
}

/** Resize the column so it fits its content
  * \param uiColumn [input]: the column ID
  */
void wxVirtualDataViewBase::AutoSizeColumn(size_t uiColumn)
{
    if (m_pOwner) m_pOwner->AutoSizeColumn(uiColumn);
}

//---------------------- SCROLLBARS ---------------------------------//
/** Update scrollbars
  * \param sNewClientSize [input]: the new client size of this window
  *                                This size might not be yet effective due to pending events
  */
void wxVirtualDataViewBase::UpdateScrollbars(wxSize sNewClientSize)
{
    //this method is needed because setting up scrollbars inside wxVirtualDataViewCtrl is tricky:
    //  1 - setting the client size of wxVirtualDataViewBase will call wxWindow::SetSize
    //      but the new size will not be immediately effective.
    //  2 - setting up the scrollbars will modify client size of the parent wxVirtualDataViewCtrl
    //      It updates client size of wxVirtualDataViewBase, but see point 1
    if (!m_pModelRenderer) return;
    m_pModelRenderer->UpdateScrollbars();
}

//------------------- wxWindow OVERRIDE -----------------------------//
/** Get the best size for the item
  * \return the best size for showing the whole model
  */
wxSize wxVirtualDataViewBase::DoGetBestSize(void) const
{
    if (!m_pModelRenderer) return(wxWindow::DoGetBestSize());
    return(m_pModelRenderer->GetBestSize());
}
