/**********************************************************************/
/** FILE    : VirtualDataViewColourEditor.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view colour editor                              **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewColourEditor.h>
#include <wx/clrpicker.h>
#include <wx/colordlg.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewColourEditor::wxVirtualDataViewColourEditor(void)
    : wxVirtualDataViewEditor(),
      m_pColourPicker(WX_VDV_NULL_PTR)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewColourEditor::wxVirtualDataViewColourEditor(const wxVirtualDataViewColourEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pColourPicker(WX_VDV_NULL_PTR)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewColourEditor& wxVirtualDataViewColourEditor::operator=(const wxVirtualDataViewColourEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewColourEditor::~wxVirtualDataViewColourEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewColourEditor::Copy(const wxVirtualDataViewColourEditor &rhs)
{
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewColourEditor::Clone(void) const
{
    return(new wxVirtualDataViewColourEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewColourEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                   size_t uiField,
                                                   const wxVariant &rvInitialValue,
                                                   wxWindow *pParent,
                                                   wxRect rRect)
{
    if (rvInitialValue.GetType() != "wxColour") return(false);
    wxColour c;
    c << rvInitialValue;

    if (!m_pColourPicker)
    {
        m_pColourPicker = new wxColourPickerCtrl(pParent, wxNewId(), c,
                                                 rRect.GetLeftTop(), rRect.GetSize(), 0);
        if (!m_pColourPicker) return(false);
    }
    m_pColourPicker->SetSize(rRect.GetSize());
    m_pColourPicker->SetColour(c);
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
bool wxVirtualDataViewColourEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                             wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    if (rInOutValue.GetType() != "wxColour") return(false);
    wxColour c;
    c << rInOutValue;

    wxColourDialog d(NULL);
    d.GetColourData().SetColour(c);
    if (d.ShowModal() == wxID_OK)
    {
        c = d.GetColourData().GetColour();
        rInOutValue << c;
        return(true);
    }
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewColourEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewColourEditor::FinishEdition(void)
{
    if (!m_pColourPicker) return(true);
    DestroyEditor(m_pColourPicker);
    m_pColourPicker = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewColourEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pColourPicker) return(false);

    wxColour c = m_pColourPicker->GetColour();
    rvValue << c;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewColourEditor::GetEditorWindow(void)
{
    return(m_pColourPicker);
}

