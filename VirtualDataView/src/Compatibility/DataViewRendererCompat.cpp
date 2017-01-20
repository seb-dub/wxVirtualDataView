/**********************************************************************/
/** FILE    : DataViewRendererCompat.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxDataViewRenderer using internally wxVirualDataView **/
/**********************************************************************/


#include <wx/VirtualDataView/Compatibility/DataViewRendererCompat.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/renderer.h>


//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  * \param rVariantType [input]: the variant type managed by this renderer
  * \param eMode        [input]: the current cell mode
  * \param iAlign       [input]: the alignment of the cells
  */
wxDataViewRendererCompat::wxDataViewRendererCompat(const wxString &rVariantType,
                                                   wxDataViewCellMode eMode,
                                                   int iAlign)
    : wxDataViewCustomRenderer(rVariantType, eMode, iAlign),
      m_pRenderer(WX_VDV_NULL_PTR),
      m_bOwnRenderer(true),
      m_pEditor(WX_VDV_NULL_PTR),
      m_bOwnEditor(true)
{
}

/** Destructor
  */
wxDataViewRendererCompat::~wxDataViewRendererCompat(void)
{
    ReleaseRenderer();
    ReleaseEditor();
}

//---------------------------- RENDERER -----------------------------//
/** Set the renderer
  * \param pRenderer [input]: the new renderer
  */
void wxDataViewRendererCompat::SetRenderer(wxVirtualDataViewRenderer *pRenderer)
{
    if (pRenderer == m_pRenderer) return;
    ReleaseRenderer();
    m_pRenderer = pRenderer;
}

/** Get the renderer
  * \return the current renderer
  */
wxVirtualDataViewRenderer* wxDataViewRendererCompat::GetRenderer(void)
{
    return(m_pRenderer);
}

/** Check if it has the renderer ownership
  * \return true if the renderer is owned by this object
  */
bool wxDataViewRendererCompat::HasRendererOwnership(void) const
{
    return(m_bOwnRenderer);
}

/** Take renderer ownership. The renderer will be released using "delete" by the destructor
  */
void wxDataViewRendererCompat::TakeRendererOwnership(void)
{
    m_bOwnRenderer = true;
}

/** Release renderer ownership
  */
void wxDataViewRendererCompat::ReleaseRendererOwnership(void)
{
    m_bOwnRenderer = false;
}

/** Release the renderer
  */
void wxDataViewRendererCompat::ReleaseRenderer(void)
{
    if ((m_pRenderer) && (m_bOwnRenderer)) delete(m_pRenderer);
    m_pRenderer = WX_VDV_NULL_PTR;
}

//---------------------- EDITOR -------------------------------------//
/** Set the editor
  * \param pEditor [input]: the new editor
  */
void wxDataViewRendererCompat::SetEditor(wxVirtualDataViewEditor *pEditor)
{
    if (pEditor == m_pEditor) return;
    ReleaseEditor();
    m_pEditor = pEditor;
}

/** Get the editor
  * \return the editor
  */
wxVirtualDataViewEditor* wxDataViewRendererCompat::GetEditor(void)
{
    return(m_pEditor);
}

/** Check if it has the editor ownership
  * \return true if the editor is owned by this object, false otherwise
  */
bool wxDataViewRendererCompat::HasEditorOwnership(void) const
{
    return(m_bOwnEditor);
}

/** Take editor ownership. The editor will be released with "delete" by the destructor
*/
void wxDataViewRendererCompat::TakeEditorOwnership(void)
{
    m_bOwnEditor = true;
}

/** Release editor ownership
  */
void wxDataViewRendererCompat::ReleaseEditorOwnership(void)
{
    m_bOwnEditor = false;
}

/** Release the editor
  */
void wxDataViewRendererCompat::ReleaseEditor(void)
{
    if ((m_pEditor) && (m_bOwnEditor)) delete(m_pEditor);
    m_pEditor = WX_VDV_NULL_PTR;
}

//-------------------- wxDataViewCustomRenderer INTERFACE -----------//
/** React to cell activation
  * \param rCell        [input]: rectangle of the cell which is activated
  * \param pDataModel   [input]: data model used
  * \param rID          [input]: the ID of the item to activate
  * \param uiCol        [input]: the index of the activated column
  * \param pMouseEvent  [input]: pointer to the mouse event, if any
  * \return true if the item was activated, false otherwise
  */
bool wxDataViewRendererCompat::ActivateCell(const wxRect &rCell, wxDataViewModel* pDataModel,
                                            const wxDataViewItem &rID, unsigned int uiCol,
                                            const wxMouseEvent *pMouseEvent)
{
    if (!m_pEditor) return(false);

    size_t uiField = GetOwner()->GetModelColumn();
    wxVariant v;
    pDataModel->GetValue(v, rID, uiCol);

    wxVirtualItemID id(rID.GetID(), 0, 0, WX_VDV_NULL_PTR);
    wxPoint ptPos(wxDefaultCoord, wxDefaultCoord);
    if (pMouseEvent) ptPos = pMouseEvent->GetPosition();
    if (m_pEditor->Activate(id, uiField, rCell, ptPos, v))
    {
        return(SetValue(v));
    }
    return(false);
}

/** Create the actual editor control once editing is about to start
  * \param pParent      [input]: the parent window
  * \param rLabelRect   [input]: the rect receiving the window
  * \param rValue       [input]: the initial value
  */
wxWindow* wxDataViewRendererCompat::CreateEditorCtrl(wxWindow *pParent,
                                                     wxRect rLabelRect, const wxVariant &rValue)
{
    if (!m_pEditor) return(WX_VDV_NULL_PTR);
    wxVirtualItemID id;
    wxVariant vValue;
    GetValue(vValue);
    if (m_pEditor->StartEditing(id, 0, vValue, pParent, rLabelRect))
    {
        return(m_pEditor->GetEditorWindow());
    }
    return(WX_VDV_NULL_PTR);
}

/** Return size required to show content
  * \return size required to show content
  */
wxSize wxDataViewRendererCompat::GetSize(void) const
{
    wxVirtualDataViewItemState oState;
    wxVariant v;
    wxVirtualDataViewItemAttr oAttr(GetAttr());
    if (m_pRenderer) return(m_pRenderer->GetSize(GetOwner()->GetOwner(),
                                                 &oAttr, oState, v));
    return(wxDefaultSize);
}

/** Get the value from the editor control (pointed to by editor)
  * \param pEditor [input] : the editor window
  * \param rvValue [output]: the value of the editor
  * \return true if the value was retrieved, false otherwise
  */
bool wxDataViewRendererCompat::GetValueFromEditorCtrl(wxWindow *pEditor, wxVariant &rValue)
{
    if (!pEditor) return(false);
    if (!m_pEditor) return(false);
    return(m_pEditor->GetValue(rValue));
}

/** Return true if this renderer supports in-place editing
  * \return true if this renderer supports in-place editing
  */
bool wxDataViewRendererCompat::HasEditorCtrl(void) const
{
    if (m_pEditor) return(true);
    return(false);
}

/** React to an activation of a cell
  * \param rCell        [input]: rectangle of the cell which is activated
  * \param pDataModel   [input]: data model used
  * \param rID          [input]: the ID of the item to activate
  * \param uiCol        [input]: the index of the activated column
  * \return true if the item was activated, false otherwise
  */
bool wxDataViewRendererCompat::Activate(wxRect rCell, wxDataViewModel *pDataModel,
                                        const wxDataViewItem &rID, unsigned int uiCol)
{
    if (!m_pEditor) return(false);

    size_t uiField = GetOwner()->GetModelColumn();
    wxVariant v;
    pDataModel->GetValue(v, rID, uiCol);

    wxVirtualItemID id(rID.GetID(), 0, 0, WX_VDV_NULL_PTR);
    wxPoint ptPos(wxDefaultCoord, wxDefaultCoord);
    if (m_pEditor->Activate(id, uiField, rCell, ptPos, v))
    {
        return(SetValue(v));
    }
    return(false);
}

/** React to a left click
  * \param ptCursor     [input]: point of the click
  * \param rCell        [input]: rectangle of the cell which is activated
  * \param pDataModel   [input]: data model used
  * \param rID          [input]: the ID of the item to activate
  * \param uiCol        [input]: the index of the activated column
  * \return true if the item was activated, false otherwise
  */
bool wxDataViewRendererCompat::LeftClick(wxPoint ptCursor, wxRect rCell,
                                         wxDataViewModel *pDataModel,
                                         const wxDataViewItem &rID, unsigned int uiCol)
{
    if (!m_pEditor) return(false);

    size_t uiField = GetOwner()->GetModelColumn();
    wxVariant v;
    pDataModel->GetValue(v, rID, uiCol);

    wxVirtualItemID id(rID.GetID(), 0, 0, WX_VDV_NULL_PTR);
    if (m_pEditor->Activate(id, uiField, rCell, ptCursor, v))
    {
        return(SetValue(v));
    }
    return(false);
}

/** Render the cell
  * \param rCell        [input]: rectangle of the cell which is rendered
  * \param pDC          [input]: Device context of the cell
  * \param iState       [input]: cell state
  * \return true on success, false on failure
  */
bool wxDataViewRendererCompat::Render(wxRect rCell, wxDC *pDC, int iState)
{
    wxVirtualDataViewItemState oState(iState & wxDATAVIEW_CELL_SELECTED,
                                      iState & wxDATAVIEW_CELL_FOCUSED,
                                      !(iState & wxDATAVIEW_CELL_INSENSITIVE),
                                      false,
                                      0);
    wxVariant v;
    if (!GetValue(v)) return(false);
    wxVirtualDataViewItemAttr oAttr(GetAttr());
    m_pRenderer->Render(GetOwner()->GetOwner(), rCell, *pDC,
                        &oAttr, oState, v);
    return(true);
}

/** Start a drag operation
  * \param ptCursor     [input]: the position of the mouse cursor
  * \param rCell        [input]: the rectangle of the cell to drag
  * \param pDataModel   [input]: the data model used
  * \param rID          [input]: the ID of the item to activate
  * \param uiCol        [input]: the index of the activated column
  * \return true if the dragging operation succeeded, false otherwise
  */
bool wxDataViewRendererCompat::StartDrag(const wxPoint &ptCursor, const wxRect &rCell,
                                         wxDataViewModel *pDataModel,
                                         const wxDataViewItem &rID, unsigned int uiCol)
{
    return(false);
}


