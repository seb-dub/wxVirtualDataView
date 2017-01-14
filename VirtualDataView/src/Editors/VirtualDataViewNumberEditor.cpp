/**********************************************************************/
/** FILE    : VirtualDataViewNumberEditor.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view number editor                              **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewNumberEditor.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/valnum.h>
#include <float.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewNumberEditor::wxVirtualDataViewNumberEditor(void)
    : wxVirtualDataViewEditor(),
      m_bUseSpinControl(true),
      m_bIsFloatingPoint(false)
{
    m_Control.m_pTextControl = WX_VDV_NULL_PTR;
    m_Control.m_pSpinControl = WX_VDV_NULL_PTR;
    m_Control.m_pSpinControlDouble = WX_VDV_NULL_PTR;
    SetRangeAuto();
    m_fIncrement = 0.1f;
}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewNumberEditor::wxVirtualDataViewNumberEditor(const wxVirtualDataViewNumberEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_bUseSpinControl(true),
      m_bIsFloatingPoint(false)
{
    m_Control.m_pTextControl = WX_VDV_NULL_PTR;
    m_Control.m_pSpinControl = WX_VDV_NULL_PTR;
    m_Control.m_pSpinControlDouble = WX_VDV_NULL_PTR;
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewNumberEditor& wxVirtualDataViewNumberEditor::operator=(const wxVirtualDataViewNumberEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewNumberEditor::~wxVirtualDataViewNumberEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewNumberEditor::Copy(const wxVirtualDataViewNumberEditor &rhs)
{
    memcpy(&m_Control, &rhs.m_Control, sizeof(UControl));
    m_bIsFloatingPoint  = rhs.m_bIsFloatingPoint;
    m_bUseSpinControl   = rhs.m_bUseSpinControl;
    m_vMin              = rhs.m_vMin;
    m_vMax              = rhs.m_vMax;
    m_fIncrement        = rhs.m_fIncrement;
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewNumberEditor::Clone(void) const
{
    return(new wxVirtualDataViewNumberEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewNumberEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                   size_t uiField,
                                                   const wxVariant &rvInitialValue,
                                                   wxWindow *pParent,
                                                   wxRect rRect)
{
    if (m_bUseSpinControl)
    {
        if (m_bIsFloatingPoint)
        {
            return(CreateSpinDoubleEditor(rvInitialValue, pParent, rRect));
        }
        else
        {
            return(CreateSpinEditor(rvInitialValue, pParent, rRect));
        }
    }
    else
    {
        return(CreateTextEditor(rvInitialValue, pParent, rRect));
    }

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
bool wxVirtualDataViewNumberEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                             wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    //not activable
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewNumberEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewNumberEditor::FinishEdition(void)
{
    if (m_bUseSpinControl)
    {
        if (m_bIsFloatingPoint)
        {
            if (!m_Control.m_pSpinControlDouble) return(true);
            DestroyEditor(m_Control.m_pSpinControlDouble);
            m_Control.m_pSpinControlDouble = WX_VDV_NULL_PTR;
        }
        else
        {
            if (!m_Control.m_pSpinControl) return(true);
            DestroyEditor(m_Control.m_pSpinControl);
            m_Control.m_pSpinControl = WX_VDV_NULL_PTR;
        }
    }
    else
    {
        DestroyEditor(m_Control.m_pTextControl);
        m_Control.m_pTextControl = WX_VDV_NULL_PTR;
    }
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewNumberEditor::GetValue(wxVariant &rvValue)
{
    if (m_bUseSpinControl)
    {
        if (m_bIsFloatingPoint)
        {
            if (!m_Control.m_pSpinControlDouble) return(false);
            rvValue = m_Control.m_pSpinControlDouble->GetValue();
            return(true);
        }
        else
        {
            if (!m_Control.m_pSpinControl) return(false);
            rvValue = (long) m_Control.m_pSpinControl->GetValue();
            return(true);
        }
    }
    else
    {
        if (!m_Control.m_pTextControl) return(false);
        rvValue = m_Control.m_pTextControl->GetValue();
        return(true);
    }
    return(false);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewNumberEditor::GetEditorWindow(void)
{
    if (m_bUseSpinControl)
    {
        if (m_bIsFloatingPoint) return(m_Control.m_pSpinControlDouble);
        else                    return(m_Control.m_pSpinControl);
    }
    return(m_Control.m_pTextControl);
}

//----------------------- EVENT HANDLERS ----------------------------//
/** Char events handler
  * \param rEvent [input] : the event information
  */
void wxVirtualDataViewNumberEditor::OnCharEvent(wxKeyEvent &rEvent)
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
void wxVirtualDataViewNumberEditor::OnTextEnterEvent(wxCommandEvent &rEvent)
{
    NotifyEditionFinished(false);
}

//--------------------- INTERNAL METHODS ----------------------------//
/** Adjust the editor size
  * \param pTextControl [input]: the text control to adjust
  */
void wxVirtualDataViewNumberEditor::AdjustEditorSize(wxTextCtrl *pTextControl) const
{
    if (!pTextControl) return;

    //Adjust size of wxTextCtrl editor to fit text, even if it means being
    //wider than the corresponding column (this is how Explorer behaves).
    const wxSize sTextSize  =  pTextControl->GetTextExtent(pTextControl->GetValue());
    const int iFittingW     =  pTextControl->GetSizeFromTextSize(sTextSize).x;
    const int iCurrentW     =  pTextControl->GetSize().x;
    const int iMaxWidth     =  pTextControl->GetParent()->GetSize().x
                             - pTextControl->GetPosition().x;

    //Adjust size so that it fits all content. Don't change anything if the
    //allocated space is already larger than needed and don't extend wxDVC's
    //boundaries.
    int iWidth = wxMin(wxMax(iCurrentW, iFittingW), iMaxWidth);

    if (iWidth != iCurrentW)
        pTextControl->SetSize(wxSize(iWidth, -1));
}

/** Create the text editor (wxTextCtrl)
  * \param rvInitialValue [input]: the initial value of the editor
  * \param pParent        [input]: the parent window
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewNumberEditor::CreateTextEditor(const wxVariant &rvInitialValue,
                                                     wxWindow *pParent, wxRect rRect)
{
    wxString sValue = rvInitialValue.GetString();
    if (!m_Control.m_pTextControl)
    {
        long lStyle = wxTE_PROCESS_ENTER | wxBORDER_SIMPLE ;//| wxTE_MULTILINE | wxTE_RICH;
        m_Control.m_pTextControl = new wxTextCtrl(pParent, wxNewId(),
                                                 sValue,
                                                 rRect.GetPosition(),
                                                 rRect.GetSize(), lStyle);
        if (!m_Control.m_pTextControl) return(false);

        m_Control.m_pTextControl->Bind(wxEVT_CHAR, &wxVirtualDataViewNumberEditor::OnCharEvent, this);
        m_Control.m_pTextControl->Bind(wxEVT_TEXT_ENTER, &wxVirtualDataViewNumberEditor::OnTextEnterEvent, this);
    }

    //create validator
    if (m_bIsFloatingPoint)
    {
        wxFloatingPointValidator<double> vVal(&m_fValue);
        vVal.SetMin(m_vMin.GetDouble());
        vVal.SetMax(m_vMax.GetDouble());
        m_Control.m_pTextControl->SetValidator(vVal);
    }
    else
    {
        wxIntegerValidator<long> vVal(&m_lValue);
        vVal.SetMin(m_vMin.GetInteger());
        vVal.SetMax(m_vMax.GetInteger());
        m_Control.m_pTextControl->SetValidator(vVal);
    }

    m_Control.m_pTextControl->SetValue(sValue);
    m_Control.m_pTextControl->SetSize(rRect);
    AdjustEditorSize(m_Control.m_pTextControl);
    m_Control.m_pTextControl->Show();

    //select the text in the control an place the cursor at the end
    m_Control.m_pTextControl->SetInsertionPointEnd();
    m_Control.m_pTextControl->SelectAll();

    return(true);
}

/** Create the spin editor (wxSpinCtrl)
  * \param rvInitialValue [input]: the initial value of the editor
  * \param pParent        [input]: the parent window
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewNumberEditor::CreateSpinEditor(const wxVariant &rvInitialValue,
                                                     wxWindow *pParent, wxRect rRect)
{
    long lMin = m_vMin.GetInteger();
    long lMax = m_vMax.GetInteger();
    long lValue = rvInitialValue.GetInteger();
    if (!m_Control.m_pSpinControl)
    {
        m_Control.m_pSpinControl = new wxSpinCtrl(pParent, wxNewId(), wxEmptyString,
                                                  rRect.GetPosition(), rRect.GetSize(),
                                                  wxSP_ARROW_KEYS|wxALIGN_RIGHT,
                                                  lMin, lMax, lValue);
        if (!m_Control.m_pSpinControl) return(false);
    }

    m_Control.m_pSpinControl->SetValue(lValue);
    m_Control.m_pSpinControl->SetRange(lMin, lMax);
    m_Control.m_pSpinControl->Show();
    return(true);

}

/** Create the spin double editor (wxSpinCtrlDouble)
  * \param rvInitialValue [input]: the initial value of the editor
  * \param pParent        [input]: the parent window
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewNumberEditor::CreateSpinDoubleEditor(const wxVariant &rvInitialValue,
                                                           wxWindow *pParent, wxRect rRect)
{
    double fMin = m_vMin.GetDouble();
    double fMax = m_vMax.GetDouble();
    double fInc = m_fIncrement;
    double fValue = rvInitialValue.GetDouble();
    if (!m_Control.m_pSpinControlDouble)
    {
        m_Control.m_pSpinControlDouble = new wxSpinCtrlDouble(pParent, wxNewId(), wxEmptyString,
                                                              rRect.GetPosition(), rRect.GetSize(),
                                                              wxSP_ARROW_KEYS|wxALIGN_RIGHT,
                                                              fMin, fMax, fValue, fInc);
        if (!m_Control.m_pSpinControlDouble) return(false);
    }

    m_Control.m_pSpinControlDouble->SetValue(fValue);
    m_Control.m_pSpinControlDouble->SetIncrement(fInc);
    m_Control.m_pSpinControlDouble->SetRange(fMin, fMax);
    m_Control.m_pSpinControlDouble->Show();
    return(true);
}

//---------------------- RANGE --------------------------------------//
/** Set the range
  * \param rvMin [input]: the minimal value (included)
  * \param rvMax [input]: the maximal value (included)
  */
void wxVirtualDataViewNumberEditor::SetRange(const wxVariant &rvMin, const wxVariant &rvMax)
{
    m_vMin = rvMin;
    m_vMax = rvMax;
}

/** Set the range
  * \param fMin [input]: the minimal value (included)
  * \param fMax [input]: the maximal value (included)
  */
void wxVirtualDataViewNumberEditor::SetRange(double fMin, double fMax)
{
    m_vMin = fMin;
    m_vMax = fMax;
}

/** Set the range
  * \param lMin [input]: the minimal value (included)
  * \param lMax [input]: the maximal value (included)
  */
void wxVirtualDataViewNumberEditor::SetRange(long lMin, long lMax)
{
    m_vMin = lMin;
    m_vMax = lMax;
}

/** Compute automatically the range
  */
void wxVirtualDataViewNumberEditor::SetRangeAuto(void)
{
    if (m_bIsFloatingPoint)
    {
        m_vMin = -DBL_MAX;
        m_vMax = DBL_MAX;
    }
    else
    {
        m_vMin = LONG_MIN;
        m_vMax = LONG_MAX;
    }
}

/** Get the range
  * \param rvMin [output]: the minimal value (included)
  * \param rvMax [output]: the maximal value (included)
  */
void wxVirtualDataViewNumberEditor::GetRange(wxVariant &rvMin, wxVariant &rvMax) const
{
    rvMin = m_vMin;
    rvMax = m_vMax;
}

/** Get the range
  * \param lMin [output]: the minimal value (included)
  * \param lMax [output]: the maximal value (included)
  */
void wxVirtualDataViewNumberEditor::GetRange(double &fMin, double &fMax) const
{
    fMin = m_vMin.GetDouble();
    fMax = m_vMax.GetDouble();
}

/** Get the range
  * \param lMin [output]: the minimal value (included)
  * \param lMax [output]: the maximal value (included)
  */
void wxVirtualDataViewNumberEditor::GetRange(long &lMin, long &lMax) const
{
    lMin = m_vMin.GetInteger();
    lMax = m_vMax.GetInteger();
}

//------------------------ INCREMENT --------------------------------//
/** Set the increment (used only for floating point spin control)
  * \param fIncrement [input]: the new increment
  */
void wxVirtualDataViewNumberEditor::SetIncrement(double fIncrement)
{
    if (fIncrement == 0.0f) m_fIncrement = 0.1f;
    else                    m_fIncrement = fIncrement;
}

/** Get the increment (used only for floating point spin control)
  * \return the increment
  */
double wxVirtualDataViewNumberEditor::GetIncrement(void) const
{
    return(m_fIncrement);
}

//--------------------- FLOATING POINT / INTEGER --------------------//
/** Return true if the format is floating point
  * \return true if the format is floating point
  *         false if the format is integer
  */
bool wxVirtualDataViewNumberEditor::IsFloatingPoint(void) const
{
    return(m_bIsFloatingPoint);
}

/** Return true if the format is integer
  * \return true if the format is integer
  *         false if the format is floating point
  */
bool wxVirtualDataViewNumberEditor::IsInteger(void) const
{
    return(!m_bIsFloatingPoint);
}

/** Set floating point / integer
  * \param bFloat [input]: true for floating point format
  *                        false for integer format
  */
void wxVirtualDataViewNumberEditor::SetFloatingPoint(bool bFloat)
{
    m_bIsFloatingPoint = bFloat;
}

/** Set integer / floating point
  * \param bInteger [input]: true for integer format
  *                          false for floating point format
  */
void wxVirtualDataViewNumberEditor::SetInteger(bool bInteger)
{
    m_bIsFloatingPoint = !bInteger;
}

//--------------------- SPIN CONTROL OR TEXT CONTROL ----------------//
/** Return true if a text control is used
  * \return true if a text control is used
  *         false if a spin control is used
  */
bool wxVirtualDataViewNumberEditor::UseTextControl(void) const
{
    return(m_bUseSpinControl);
}

/** Return true if a spin control is used
  * \return true if a spin control is used
  *         false if a text control is used
  */
bool wxVirtualDataViewNumberEditor::UseSpinControl(void) const
{
    return(!m_bUseSpinControl);
}

/** Toggle the use of text/spin control
  * \param bTextCtrl [input]: true for using text control
  *                           false for using spin control
  */
void wxVirtualDataViewNumberEditor::SetTextControl(bool bTextCtrl)
{
    m_bUseSpinControl = !bTextCtrl;
}

/** Toggle the use of spin/text control
  * \param bSpinCtrl [input]: true for using spin control
  *                           false for using text control
  */
void wxVirtualDataViewNumberEditor::SetSpinControl(bool bSpinCtrl)
{
    m_bUseSpinControl = bSpinCtrl;
}
