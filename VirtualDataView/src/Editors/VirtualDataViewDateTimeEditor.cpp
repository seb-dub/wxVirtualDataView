/**********************************************************************/
/** FILE    : VirtualDataViewDateTimeEditor.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view date/time editor                           **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewDateTimeEditor.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/panel.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param bDropDown [input]: true for drop-down style for wxDatePickerCtrl (wxDP_DROPDOWN)
  *                           false for spin-control style (wxDP_SPIN)
  * \param bPlatformDefault [input]: true for using default platform style (wxDP_DEFAULT)
  *                                  false for using wxDP_DROPDOWN or wxDP_SPIN
  */
wxVirtualDataViewDateTimeEditor::wxVirtualDataViewDateTimeEditor(bool bDropDown,
                                                                 bool bPlatformDefault)
    : wxVirtualDataViewEditor(),
      m_pPanel(WX_VDV_NULL_PTR),
      m_pDatePickerCtrl(WX_VDV_NULL_PTR),
      m_pTimePickerCtrl(WX_VDV_NULL_PTR),
      m_bDropDown(bDropDown),
      m_bPlatformDefault(bPlatformDefault)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewDateTimeEditor::wxVirtualDataViewDateTimeEditor(const wxVirtualDataViewDateTimeEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pPanel(WX_VDV_NULL_PTR),
      m_pDatePickerCtrl(WX_VDV_NULL_PTR),
      m_pTimePickerCtrl(WX_VDV_NULL_PTR),
      m_bDropDown(true),
      m_bPlatformDefault(false)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewDateTimeEditor& wxVirtualDataViewDateTimeEditor::operator=(const wxVirtualDataViewDateTimeEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewDateTimeEditor::~wxVirtualDataViewDateTimeEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewDateTimeEditor::Copy(const wxVirtualDataViewDateTimeEditor &rhs)
{
    m_bDropDown         = rhs.m_bDropDown;
    m_bPlatformDefault  = rhs.m_bPlatformDefault;
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewDateTimeEditor::Clone(void) const
{
    return(new wxVirtualDataViewDateTimeEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewDateTimeEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                     size_t uiField,
                                                     const wxVariant &rvInitialValue,
                                                     wxWindow *pParent,
                                                     wxRect rRect)
{
    wxDateTime dt = rvInitialValue.GetDateTime();

    //create panel
    if (!m_pPanel)
    {
        long lStyle = wxTAB_TRAVERSAL | wxBORDER_NONE;
        m_pPanel = new wxPanel(pParent, wxNewId(), rRect.GetPosition(), rRect.GetSize(), lStyle);
    }
    m_pPanel->SetSize(rRect);

    //compute size & pos of pickers
    wxSize s = rRect.GetSize();
    wxSize s1(s.GetWidth() / 2, s.GetHeight());
    wxSize s2(s.GetWidth() - s1.GetWidth(), s.GetHeight());
    wxPoint pt1(0, 0);
    wxPoint pt2(pt1.x + s1.GetWidth(), pt1.y);
    wxRect r1(pt1, s1);
    wxRect r2(pt2, s2);

    //create date picker ctrl
    if (!m_pDatePickerCtrl)
    {
        long lStyle = wxDP_SHOWCENTURY;
        if (m_bPlatformDefault) lStyle |= wxDP_DEFAULT;
        else if (m_bDropDown)   lStyle |= wxDP_DROPDOWN;
        else                    lStyle |= wxDP_SPIN;
        m_pDatePickerCtrl = new wxDatePickerCtrl(m_pPanel, wxNewId(), dt,
                                                 pt1,
                                                 s1, lStyle);
        if (!m_pDatePickerCtrl) return(false);
    }

    m_pDatePickerCtrl->SetValue(dt);
    m_pDatePickerCtrl->SetSize(r1);

    //create time picker ctrl
    if (!m_pTimePickerCtrl)
    {
        long lStyle = wxTP_DEFAULT;
        m_pTimePickerCtrl = new wxTimePickerCtrl(m_pPanel, wxNewId(), dt,
                                                 pt2,
                                                 s2, lStyle);
        if (!m_pTimePickerCtrl) return(false);
    }

    m_pTimePickerCtrl->SetValue(dt);
    m_pTimePickerCtrl->SetSize(r2);

    m_pPanel->Show();

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
bool wxVirtualDataViewDateTimeEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                               wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    //not activable
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewDateTimeEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewDateTimeEditor::FinishEdition(void)
{
    if (!m_pPanel) return(true);
    DestroyEditor(m_pPanel);
    m_pPanel = WX_VDV_NULL_PTR;
    m_pDatePickerCtrl = WX_VDV_NULL_PTR;
    m_pTimePickerCtrl = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewDateTimeEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pPanel) return(false);
    if (!m_pDatePickerCtrl) return(false);
    if (!m_pTimePickerCtrl) return(false);

    wxDateTime dtDate = m_pDatePickerCtrl->GetValue();
    wxDateTime dtTime = m_pTimePickerCtrl->GetValue();
    dtDate.SetHour(dtTime.GetHour());
    dtDate.SetMinute(dtTime.GetMinute());
    dtDate.SetSecond(dtTime.GetSecond());
    dtDate.SetMillisecond(dtTime.GetMillisecond());

    rvValue = dtDate;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewDateTimeEditor::GetEditorWindow(void)
{
    return(m_pPanel);
}

//----------------------- OPTIONS -----------------------------------//
/** Return true if the calendar control is drop-down
  * \return true if the calendar control is drop-down (wxDP_DROPDOWN)
  *         false if the calendar control is spin (wxDP_SPIN)
  */
bool wxVirtualDataViewDateTimeEditor::IsDropDown(void) const
{
    return(m_bDropDown);
}

/** Set drop-down / spin
  * \param bDropDown [input]: true if the calendar control is drop-down (wxDP_DROPDOWN)
  *                           false if the calendar control is spin (wxDP_SPIN)
  */
void wxVirtualDataViewDateTimeEditor::SetDropDown(bool bDropDown)
{
    m_bDropDown = bDropDown;
}

/** Set spin / drop-down
  * \param bSpin [input] : true if the calendar control is drop-down (wxDP_DROPDOWN)
  *                        false if the calendar control is spin (wxDP_SPIN)
  */
void wxVirtualDataViewDateTimeEditor::SetSpin(bool bSpin)
{
    m_bDropDown = !bSpin;
}

/** Set spin / drop-down which corresponds to the default of the platform
  * \param bDefault [input]: true for using platform default
  *                          false for using spin/dropdown
  */
void wxVirtualDataViewDateTimeEditor::SetPlatformDropDownDefault(bool bDefault)
{
    m_bPlatformDefault = bDefault;
}

/** Return true if platform default is used
  * \return true if platform default is used (wxDP_DEFAULT)
  *         false if user settings are used (wxDP_DROPDOWN/wxDP_SPIN)
  */
bool wxVirtualDataViewDateTimeEditor::IsPlatformDropDownDefaultUsed(void) const
{
    return(m_bPlatformDefault);
}
