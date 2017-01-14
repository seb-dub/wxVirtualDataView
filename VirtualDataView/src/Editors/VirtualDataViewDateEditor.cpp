/**********************************************************************/
/** FILE    : VirtualDataViewDateEditor.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view date editor                                **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewDateEditor.h>
#include <wx/datectrl.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param bDropDown [input]: true for drop-down style for wxDatePickerCtrl (wxDP_DROPDOWN)
  *                           false for spin-control style (wxDP_SPIN)
  * \param bPlatformDefault [input]: true for using default platform style (wxDP_DEFAULT)
  *                                  false for using wxDP_DROPDOWN or wxDP_SPIN
  */
wxVirtualDataViewDateEditor::wxVirtualDataViewDateEditor(bool bDropDown,
                                                         bool bPlatformDefault)
    : wxVirtualDataViewEditor(),
      m_pDatePickerCtrl(WX_VDV_NULL_PTR),
      m_bDropDown(bDropDown),
      m_bPlatformDefault(bPlatformDefault)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewDateEditor::wxVirtualDataViewDateEditor(const wxVirtualDataViewDateEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pDatePickerCtrl(WX_VDV_NULL_PTR),
      m_bDropDown(true),
      m_bPlatformDefault(false)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewDateEditor& wxVirtualDataViewDateEditor::operator=(const wxVirtualDataViewDateEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewDateEditor::~wxVirtualDataViewDateEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewDateEditor::Copy(const wxVirtualDataViewDateEditor &rhs)
{
    m_bDropDown         = rhs.m_bDropDown;
    m_bPlatformDefault  = rhs.m_bPlatformDefault;
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewDateEditor::Clone(void) const
{
    return(new wxVirtualDataViewDateEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewDateEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                 size_t uiField,
                                                 const wxVariant &rvInitialValue,
                                                 wxWindow *pParent,
                                                 wxRect rRect)
{
    wxDateTime dt = rvInitialValue.GetDateTime();

    if (!m_pDatePickerCtrl)
    {
        long lStyle = wxDP_SHOWCENTURY;
        if (m_bPlatformDefault) lStyle |= wxDP_DEFAULT;
        else if (m_bDropDown)   lStyle |= wxDP_DROPDOWN;
        else                    lStyle |= wxDP_SPIN;
        m_pDatePickerCtrl = new wxDatePickerCtrl(pParent, wxNewId(), dt,
                                                 rRect.GetPosition(),
                                                 rRect.GetSize(), lStyle);
        if (!m_pDatePickerCtrl) return(false);
    }

    m_pDatePickerCtrl->SetValue(dt);
    m_pDatePickerCtrl->SetSize(rRect);
    m_pDatePickerCtrl->Show();

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
bool wxVirtualDataViewDateEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                           wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    //not activable
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewDateEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewDateEditor::FinishEdition(void)
{
    if (!m_pDatePickerCtrl) return(true);
    DestroyEditor(m_pDatePickerCtrl);
    m_pDatePickerCtrl = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewDateEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pDatePickerCtrl) return(false);

    wxDateTime dt = m_pDatePickerCtrl->GetValue();
    rvValue = dt;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewDateEditor::GetEditorWindow(void)
{
    return(m_pDatePickerCtrl);
}

//----------------------- OPTIONS -----------------------------------//
/** Return true if the calendar control is drop-down
  * \return true if the calendar control is drop-down (wxDP_DROPDOWN)
  *         false if the calendar control is spin (wxDP_SPIN)
  */
bool wxVirtualDataViewDateEditor::IsDropDown(void) const
{
    return(m_bDropDown);
}

/** Set drop-down / spin
  * \param bDropDown [input]: true if the calendar control is drop-down (wxDP_DROPDOWN)
  *                           false if the calendar control is spin (wxDP_SPIN)
  */
void wxVirtualDataViewDateEditor::SetDropDown(bool bDropDown)
{
    m_bDropDown = bDropDown;
}

/** Set spin / drop-down
  * \param bSpin [input] : true if the calendar control is drop-down (wxDP_DROPDOWN)
  *                        false if the calendar control is spin (wxDP_SPIN)
  */
void wxVirtualDataViewDateEditor::SetSpin(bool bSpin)
{
    m_bDropDown = !bSpin;
}

/** Set spin / drop-down which corresponds to the default of the platform
  * \param bDefault [input]: true for using platform default
  *                          false for using spin/dropdown
  */
void wxVirtualDataViewDateEditor::SetPlatformDropDownDefault(bool bDefault)
{
    m_bPlatformDefault = bDefault;
}

/** Return true if platform default is used
  * \return true if platform default is used (wxDP_DEFAULT)
  *         false if user settings are used (wxDP_DROPDOWN/wxDP_SPIN)
  */
bool wxVirtualDataViewDateEditor::IsPlatformDropDownDefaultUsed(void) const
{
    return(m_bPlatformDefault);
}
