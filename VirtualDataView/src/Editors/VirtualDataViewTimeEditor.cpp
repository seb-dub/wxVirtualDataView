/**********************************************************************/
/** FILE    : VirtualDataViewTimeEditor.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view time editor                                **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewTimeEditor.h>
#include <wx/timectrl.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewTimeEditor::wxVirtualDataViewTimeEditor(void)
    : wxVirtualDataViewEditor(),
      m_pTimePickerCtrl(WX_VDV_NULL_PTR)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewTimeEditor::wxVirtualDataViewTimeEditor(const wxVirtualDataViewTimeEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pTimePickerCtrl(WX_VDV_NULL_PTR)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewTimeEditor& wxVirtualDataViewTimeEditor::operator=(const wxVirtualDataViewTimeEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewTimeEditor::~wxVirtualDataViewTimeEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewTimeEditor::Copy(const wxVirtualDataViewTimeEditor &rhs)
{
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewTimeEditor::Clone(void) const
{
    return(new wxVirtualDataViewTimeEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewTimeEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                 size_t uiField,
                                                 const wxVariant &rvInitialValue,
                                                 wxWindow *pParent,
                                                 wxRect rRect)
{
    wxDateTime dt = rvInitialValue.GetDateTime();

    if (!m_pTimePickerCtrl)
    {
        long lStyle = wxTP_DEFAULT;
        m_pTimePickerCtrl = new wxTimePickerCtrl(pParent, wxNewId(), dt,
                                                 rRect.GetPosition(),
                                                 rRect.GetSize(), lStyle);
        if (!m_pTimePickerCtrl) return(false);
    }

    m_pTimePickerCtrl->SetValue(dt);
    m_pTimePickerCtrl->SetSize(rRect);
    m_pTimePickerCtrl->Show();

    return(true);
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
bool wxVirtualDataViewTimeEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                           wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    //not activable
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewTimeEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewTimeEditor::FinishEdition(void)
{
    if (!m_pTimePickerCtrl) return(true);
    DestroyEditor(m_pTimePickerCtrl);
    m_pTimePickerCtrl = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewTimeEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pTimePickerCtrl) return(false);

    wxDateTime dt = m_pTimePickerCtrl->GetValue();
    rvValue = dt;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewTimeEditor::GetEditorWindow(void)
{
    return(m_pTimePickerCtrl);
}
