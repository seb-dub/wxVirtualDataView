/**********************************************************************/
/** FILE    : VirtualDataViewStdFilterEditor.cpp                     **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : standard impl of wxVirtualDataViewIFilterEditor        **/
/**********************************************************************/

#include <wx/VirtualDataView/FilterEditors/VirtualDataViewStdFilterEditor.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterComboCtrl.h>
#include <wx/textctrl.h>
#include <wx/app.h>
#include <wx/dc.h>
#include "FilterEditors/ComboboxState.hpp"

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  */
wxVirtualDataViewStdFilterEditor::wxVirtualDataViewStdFilterEditor(void)
    : wxVirtualDataViewIFilterEditor(),
      m_pComboBox(WX_VDV_NULL_PTR),
      m_pComboBoxState(new wxComboboxState)
{

}

/** Destructor
  */
wxVirtualDataViewStdFilterEditor::~wxVirtualDataViewStdFilterEditor(void)
{
    if (m_pComboBox) m_pComboBox->SetState(WX_VDV_NULL_PTR);
    DestroyEditor(m_pComboBox);
    if (m_pComboBoxState) delete(m_pComboBoxState);
    m_pComboBoxState = WX_VDV_NULL_PTR;
}

//--------------------- UI INTERFACE --------------------------------//
/** Clone the filter
  * \return a new filter editor, allocated with "new"
  */
wxVirtualDataViewIFilterEditor* wxVirtualDataViewStdFilterEditor::Clone(void) const
{
    wxVirtualDataViewStdFilterEditor *pNewEditor = new wxVirtualDataViewStdFilterEditor;
    if (pNewEditor) pNewEditor->Copy(*this);
    return(pNewEditor);
}

wxSize GetBestDefaultSize(wxWindow *pWin)
{
    static wxSize s_DefaultSize(-1, -1);
    if ((s_DefaultSize.GetWidth() < 0) || (s_DefaultSize.GetHeight() < 0))
    {
        //1 simple combo box
        int iWidth  = 16;
        int iHeight = 16;
        s_DefaultSize.Set(iWidth, iHeight);

        wxWindow *pWin2 = pWin;
        if (!pWin2) pWin2 = wxTheApp->GetTopWindow();
        if (pWin2)
        {
            wxVirtualDataViewFilterComboCtrl *pEditor = new wxVirtualDataViewFilterComboCtrl(pWin2);
            if (pEditor)
            {
                s_DefaultSize = pEditor->GetBestSize();
                pEditor->Destroy();
            }
        }
    }
    return(s_DefaultSize);
}

/** Get the size of the filter
  * \param pWin [input]: the window in which the filter editor is shown
  * \return the size of the filter
  */
wxSize wxVirtualDataViewStdFilterEditor::GetSize(wxWindow *pWin)
{
    if ((m_DefaultSize.GetWidth() < 0) || (m_DefaultSize.GetHeight() < 0))
    {
        m_DefaultSize = GetBestDefaultSize(pWin);
    }

    return(m_DefaultSize);
}

/** Draw the filter
  * \param pWin  [input]: the window receiving the drawing
  * \param rDC   [input]: the DC to use for drawing the filter
  * \param rRect [input]: the clipping rectangle
  */
void wxVirtualDataViewStdFilterEditor::Draw(wxWindow *pWin, wxDC &rDC, wxRect &rRect)
{
    if ((!m_pComboBox) && (m_pComboBoxState))
    {
        m_pComboBoxState->RecalculateSize(rRect);
        m_pComboBoxState->DrawComboBoxClientArea(pWin, rDC, rRect);
    }
    //wxRendererNative::Get().DrawComboBox(pWin, rDC, rRect, wxCONTROL_NONE);
}

/** Capture the editor to a bitmap
  * \param pEditor [input] : the editor to capture to bitmap
  */
void wxVirtualDataViewStdFilterEditor::CaptureToBitmap(wxWindow* pEditor)
{
    //we can draw it manually, so avoid using memory for a useless bitmap
}

/** Activate the filter editor
  * \param pParent [input]: the parent window
  * \param rRect   [input]: the clipping rectangle
  */
bool wxVirtualDataViewStdFilterEditor::StartFilter(wxWindow *pParent, wxRect &rRect)
{
    CreateComboBox(pParent, rRect);
    if (!m_pComboBox) return(false);
    NotifyFilterBegin();
    return(true);
}

/** Return true if filter edition can be finished
  * \return true if filter edition can be finished
  */
bool wxVirtualDataViewStdFilterEditor::CanFinishFilterEdition(void)
{
    return(true);
}

/** End filter edition
  * \param bCancelled [input]: true if the filter was cancelled, false otherwise
  * \return true on success, false on failure
  */
bool wxVirtualDataViewStdFilterEditor::FinishFilterEdition(bool bCancelled)
{
    if (!m_pComboBox) return(true);
    NotifyFilterEnd(bCancelled);

    m_pComboBox->AboutToBeDestroyed();
    DestroyEditor(m_pComboBox);
    m_pComboBox = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewStdFilterEditor::GetEditorWindow(void)
{
    return(m_pComboBox);
}

/** Clear all items in the filter editor
  */
void wxVirtualDataViewStdFilterEditor::ClearFilter(void)
{
    if (m_pComboBox) m_pComboBox->Clear();
}

/** Initialize the filter
  * Fill the filter with data from a source model
  * \param pSrcModel [input]: the data model containing the value to display inside the filter
  * \param uiField   [input]: the field of the data model to consider
  * \param pStateModel [input]: the state model to use
  */
void wxVirtualDataViewStdFilterEditor::InitFilter(wxVirtualIDataModel *pSrcModel, size_t uiField,
                                                  wxVirtualIStateModel *pStateModel)
{
    if (m_pComboBox) m_pComboBox->InitFilter(pSrcModel, uiField, pStateModel);
}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewStdFilterEditor::Copy(const wxVirtualDataViewStdFilterEditor &rhs)
{
    CopyBase(rhs);
}

/** Create the combo box
  * \param pParent [input] : the parent window
  * \param rRect   [input] : the position & size of the window
  */
void wxVirtualDataViewStdFilterEditor::CreateComboBox(wxWindow* pParent, wxRect &rRect)
{
    if (!m_pComboBox)
    {
        m_pComboBox = new wxVirtualDataViewFilterComboCtrl(pParent);
        m_pComboBox->Bind(wxEVT_TEXT, &wxVirtualDataViewStdFilterEditor::OnFilterChanged, this);
        m_pComboBox->Bind(wxEVT_TEXT_ENTER, &wxVirtualDataViewStdFilterEditor::OnFilterChanged, this);
        m_pComboBox->Bind(wxEVT_COMBOBOX_CLOSEUP, &wxVirtualDataViewStdFilterEditor::OnFilterChanged, this);
        m_pComboBox->Bind(wxEVT_COMBOBOX_DROPDOWN, &wxVirtualDataViewStdFilterEditor::OnFilterInit, this);

        m_pComboBox->SetState(m_pComboBoxState);
        if (m_pComboBoxState) m_pComboBox->SetValue(m_pComboBoxState->GetValue());
        m_pComboBox->SetFocus();
    }
    SetComboBoxSize(rRect);
}

/** Set the combo box size
  * \param rRect   [input] : the position & size of the window
  */
void wxVirtualDataViewStdFilterEditor::SetComboBoxSize(wxRect &rRect)
{
    if (!m_pComboBox) return;
    m_pComboBox->SetSize(rRect);
}

//--------------------- EVENT HANDLERS ------------------------------//
/** Combo-box contents changed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewStdFilterEditor::OnFilterChanged(wxCommandEvent &rEvent)
{
    if (m_pComboBox)
    {
        wxVirtualDataViewFilter *pFilter = m_pComboBox->GenerateFilter();
        SetFilter(pFilter);
    }

    NotifyFilterChanged();
    rEvent.Skip();
}

/** Drop-down event for the combo-box
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewStdFilterEditor::OnFilterInit(wxCommandEvent &rEvent)
{
    NotifyFilterBegin();
    rEvent.Skip();
}
