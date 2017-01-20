/**********************************************************************/
/** FILE    : VirtualDataViewEditorCompat.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualDataViewEditor wrapping a wxDVCRenderer     **/
/**********************************************************************/

#include <wx/VirtualDataView/Compatibility/VirtualDataViewEditorCompat.h>
#include <wx/VirtualDataView/Compatibility/DataViewModelCompat.h>
#include <wx/VirtualDataView/VirtualItemID.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
    #include <wx/dataview.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param eMode        [input]: the ellipsization mode
  * \param eHAlign      [input]: the horizontal alignment
  * \param eVAlign      [input]: the vertical alignment
  * \param pRenderer    [input]: the renderer to wrap
  */
wxVirtualDataViewEditorCompat::wxVirtualDataViewEditorCompat(wxDataViewCustomRenderer* pRenderer)
    : wxVirtualDataViewEditor(),
      m_pRenderer(pRenderer),
      m_pEditorWindow(WX_VDV_NULL_PTR)
{

}

/** Destructor - virtual
  */
wxVirtualDataViewEditorCompat::~wxVirtualDataViewEditorCompat(void)
{

}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the renderer
  * \return a new renderer, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewEditorCompat::Clone(void) const
{
    wxVirtualDataViewEditorCompat *pResult;
    pResult = new wxVirtualDataViewEditorCompat(m_pRenderer);
    if (pResult) pResult->CopyBase(*this);
    return(pResult);
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewEditorCompat::DoStartEditing(const wxVirtualItemID &rID,
                                                   size_t uiField,
                                                   const wxVariant &rvInitialValue,
                                                   wxWindow *pParent,
                                                   wxRect rRect)
{
    if (!m_pRenderer) return(false);
    if (!m_pEditorWindow)
    {
        m_pEditorWindow = m_pRenderer->CreateEditorCtrl(pParent, rRect, rvInitialValue);
        if (!m_pEditorWindow) return(false);
    }

    wxDataViewItem id(rID.GetID());
    return(m_pRenderer->StartEditing(id, rRect));
}

/** Answer to a single click/space/return
  * \param rID            [input]: the ID of the item to activate
  * \param uiField        [input]: the item field to activate
  * \param rRect          [input]: rectangle of the cell
  * \param ptPos          [input]: the relative position of the mouse event.
  *                                Negative coord indicates keyboard event.
  * \param rInOutValue    [input/output]: on input, the current value of the item
  *                                       on output, the new value of the item
  *                                       The item is modified only if true is returned
  * \return true if the item was modified, false otherwise
  */
bool wxVirtualDataViewEditorCompat::Activate(const wxVirtualItemID &rID, size_t uiField,
                                             wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    if (!m_pRenderer) return(false);
    if (m_pRenderer->GetMode() != wxDATAVIEW_CELL_ACTIVATABLE) return(false);
    wxDataViewItem id(rID.GetID());
    wxDataViewModelCompat *pModel = new wxDataViewModelCompat(rID.GetTopModel());
    wxMouseEvent e(wxEVT_LEFT_UP);
    e.SetPosition(ptPos);
    return(m_pRenderer->ActivateCell(rRect, pModel, id, uiField, &e));
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewEditorCompat::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewEditorCompat::FinishEdition(void)
{
    if (!m_pRenderer) return(true);
    m_pRenderer->FinishEditing();
    m_pEditorWindow = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewEditorCompat::GetValue(wxVariant &rvValue)
{
    if (!m_pRenderer) return(false);
    return(m_pRenderer->GetValue(rvValue));
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewEditorCompat::GetEditorWindow(void)
{
    return(m_pEditorWindow);
}

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
