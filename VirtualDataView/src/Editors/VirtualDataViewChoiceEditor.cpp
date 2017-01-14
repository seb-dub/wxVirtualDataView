/**********************************************************************/
/** FILE    : VirtualDataViewChoiceEditor.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view combo-box editor                           **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewChoiceEditor.h>
#include <wx/combobox.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewChoiceEditor::wxVirtualDataViewChoiceEditor(void)
    : wxVirtualDataViewEditor(),
      m_pComboBox(WX_VDV_NULL_PTR),
      m_bReadOnly(false),
      m_bSort(false)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewChoiceEditor::wxVirtualDataViewChoiceEditor(const wxVirtualDataViewChoiceEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pComboBox(WX_VDV_NULL_PTR),
      m_bReadOnly(false),
      m_bSort(false)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewChoiceEditor& wxVirtualDataViewChoiceEditor::operator=(const wxVirtualDataViewChoiceEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewChoiceEditor::~wxVirtualDataViewChoiceEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewChoiceEditor::Copy(const wxVirtualDataViewChoiceEditor &rhs)
{
    m_vChoices  = rhs.m_vChoices;
    m_bReadOnly = rhs.m_bReadOnly;
    m_bSort     = rhs.m_bSort;
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewChoiceEditor::Clone(void) const
{
    return(new wxVirtualDataViewChoiceEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewChoiceEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                   size_t uiField,
                                                   const wxVariant &rvInitialValue,
                                                   wxWindow *pParent,
                                                   wxRect rRect)
{
    wxString sValue = rvInitialValue.GetString();

    if (!m_pComboBox)
    {
        long lStyle = 0;
        if (m_bSort) lStyle |= wxCB_SORT;
        if (m_bReadOnly) lStyle |= wxCB_READONLY;
        m_pComboBox = new wxComboBox(pParent, wxNewId(), sValue,
                                     rRect.GetPosition(), rRect.GetSize(),
                                     0, WX_VDV_NULL_PTR, lStyle);
        if (!m_pComboBox) return(false);

        m_pComboBox->Bind(wxEVT_CHAR, &wxVirtualDataViewChoiceEditor::OnCharEvent, this);
        m_pComboBox->Bind(wxEVT_TEXT_ENTER, &wxVirtualDataViewChoiceEditor::OnTextEnterEvent, this);
    }
    m_pComboBox->Set(m_vChoices);
    m_pComboBox->SetValue(sValue);

    m_pComboBox->SetInsertionPointEnd();
    m_pComboBox->SelectAll();

    AdjustEditorSize();

    m_pComboBox->Show();
    return(true);
}

/** Adjust the editor size
  */
void wxVirtualDataViewChoiceEditor::AdjustEditorSize(void) const
{
    if (!m_pComboBox) return;

    //Adjust size of wxTextCtrl editor to fit text, even if it means being
    //wider than the corresponding column (this is how Explorer behaves).
    const wxSize sTextSize  =  m_pComboBox->GetTextExtent(m_pComboBox->GetStringSelection());
    const int iFittingW     =  m_pComboBox->GetSizeFromTextSize(sTextSize).x;
    const int iCurrentW     =  m_pComboBox->GetSize().x;
    const int iMaxWidth     =  m_pComboBox->GetParent()->GetSize().x
                             - m_pComboBox->GetPosition().x;

    //Adjust size so that it fits all content. Don't change anything if the
    //allocated space is already larger than needed and don't extend wxDVC's
    //boundaries.
    int iWidth = wxMin(wxMax(iCurrentW, iFittingW), iMaxWidth);

    if (iWidth != iCurrentW)
        m_pComboBox->SetSize(wxSize(iWidth, -1));
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
bool wxVirtualDataViewChoiceEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                             wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    //not activable
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewChoiceEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewChoiceEditor::FinishEdition(void)
{
    if (!m_pComboBox) return(true);
    DestroyEditor(m_pComboBox);
    m_pComboBox = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewChoiceEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pComboBox) return(false);

    wxString sValue = m_pComboBox->GetValue();
    rvValue = sValue;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewChoiceEditor::GetEditorWindow(void)
{
    return(m_pComboBox);
}

//----------------------- EVENT HANDLERS ----------------------------//
/** Char events handler
  * \param rEvent [input] : the event information
  */
void wxVirtualDataViewChoiceEditor::OnCharEvent(wxKeyEvent &rEvent)
{
    switch(rEvent.GetKeyCode())
    {
        case WXK_ESCAPE :   NotifyEditionFinished(true);
                            break;

        default         :   rEvent.Skip();
                            break;
    }
}

/** Process a return event
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewChoiceEditor::OnTextEnterEvent(wxCommandEvent &rEvent)
{
    NotifyEditionFinished(false);
}

//------------------ CHOICES ---------------------------------------//
/** Set the choices
  * \param rvChoices [input]: the new choices
  */
void wxVirtualDataViewChoiceEditor::SetChoices(const wxArrayString &rvChoices)
{
    m_vChoices = rvChoices;
}

/** Get the choices
  * \return the choices
  */
wxArrayString& wxVirtualDataViewChoiceEditor::GetChoices(void)
{
    return(m_vChoices);
}

/** Get the choices (const version)
  * \return the choices
  */
const wxArrayString& wxVirtualDataViewChoiceEditor::GetChoices(void) const
{
    return(m_vChoices);
}

//-------------------- OPTIONS --------------------------------------//
/** Return true if the editor is read-only
  * \return true if the editor is read-only, false if it is read-write
  */
bool wxVirtualDataViewChoiceEditor::IsReadOnly(void) const
{
    return(m_bReadOnly);
}

/** Return true if the editor is read-write
  * \return true if the editor is read-write, false if it is read-only
  */
bool wxVirtualDataViewChoiceEditor::IsReadWrite(void) const
{
    return(!m_bReadOnly);
}

/** Toggle read-only ON/OFF
  * \param bReadOnly [input]: true for read-only
  *                           false for read-write
  */
void wxVirtualDataViewChoiceEditor::SetReadOnly(bool bReadOnly)
{
    m_bReadOnly = bReadOnly;
}

/** Toggle read-write ON/OFF
  * \param bReadWrite [input]: true for read-write
  *                            false for read-only
  */
void wxVirtualDataViewChoiceEditor::SetReadWrite(bool bReadWrite)
{
    m_bReadOnly = !bReadWrite;
}
