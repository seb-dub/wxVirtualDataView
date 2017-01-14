/**********************************************************************/
/** FILE    : VirtualDataViewCheckBoxEditor.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view checkbox editor                            **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewCheckBoxEditor.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#include <wx/checkbox.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  * \param bTriState   [input]: true if the check-box is tri-state
  */
wxVirtualDataViewCheckBoxEditor::wxVirtualDataViewCheckBoxEditor(bool bTriState)
    : wxVirtualDataViewEditor(),
      m_bTriState(bTriState),
      m_pEditor(WX_VDV_NULL_PTR),
      m_eLowerBoundBehaviour(WX_E_OUT_OF_BOUND_IS_UNCHECKED),
      m_eUpperBoundBehaviour(m_bTriState ? WX_E_OUT_OF_BOUND_IS_UNDETERMINED : WX_E_OUT_OF_BOUND_IS_CHECKED)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewCheckBoxEditor::wxVirtualDataViewCheckBoxEditor(const wxVirtualDataViewCheckBoxEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_bTriState(false),
      m_pEditor(WX_VDV_NULL_PTR),
      m_eLowerBoundBehaviour(WX_E_OUT_OF_BOUND_IS_UNCHECKED),
      m_eUpperBoundBehaviour(m_bTriState ? WX_E_OUT_OF_BOUND_IS_UNDETERMINED : WX_E_OUT_OF_BOUND_IS_CHECKED)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewCheckBoxEditor& wxVirtualDataViewCheckBoxEditor::operator=(const wxVirtualDataViewCheckBoxEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewCheckBoxEditor::~wxVirtualDataViewCheckBoxEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewCheckBoxEditor::Copy(const wxVirtualDataViewCheckBoxEditor &rhs)
{
    m_bTriState             = rhs.m_bTriState;
    m_eLowerBoundBehaviour  = rhs.m_eLowerBoundBehaviour;
    m_eUpperBoundBehaviour  = rhs.m_eUpperBoundBehaviour;
}

/** Clamp the value to the range [0, 2] for tri-state, or [0, 1] for bi-state
  * \param lValue [input] : the value to clamp
  * \return the clamped value
  */
long wxVirtualDataViewCheckBoxEditor::ClampValue(long lValue)
{
    if (lValue < 0)
    {
        switch (m_eLowerBoundBehaviour)
        {
            case WX_E_OUT_OF_BOUND_IS_UNCHECKED    : return(0);
            case WX_E_OUT_OF_BOUND_IS_CHECKED      : return(1);
            case WX_E_OUT_OF_BOUND_IS_UNDETERMINED :
            default                                : if (m_bTriState) return(2);
                                                     return(1);

        }
    }

    long lMax = 2; //undetermined
    if (!m_bTriState) lMax = 1; //checked
    if (lValue > lMax)
    {
        switch (m_eUpperBoundBehaviour)
        {
            case WX_E_OUT_OF_BOUND_IS_UNCHECKED    : return(0);
            case WX_E_OUT_OF_BOUND_IS_CHECKED      : return(1);
            case WX_E_OUT_OF_BOUND_IS_UNDETERMINED :
            default                                : if (m_bTriState) return(2);
                                                     return(1);

        }
    }
    return(lValue);

//    //lower bound
//    if (lValue < 0)
//    {
//        if (m_bTriState) return(2); //undetermined
//        return(1);                  //checked
//    }
//
//    //upper bound
//    long lMax = 2; //undetermined
//    if (!m_bTriState) lMax = 1; //checked
//    if (lValue > lMax) return(lMax); //undetermined or checked
//
//    return(lValue);
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewCheckBoxEditor::Clone(void) const
{
    return(new wxVirtualDataViewCheckBoxEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCheckBoxEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                     size_t uiField,
                                                     const wxVariant &rvInitialValue,
                                                     wxWindow *pParent,
                                                     wxRect rRect)
{
    if (!m_pEditor)
    {
        long lStyle = wxCHK_2STATE;
        if (m_bTriState) lStyle = wxCHK_3STATE;
        m_pEditor = new wxCheckBox(pParent, wxNewId(),
                                   wxEmptyString,
                                   rRect.GetPosition(),
                                   rRect.GetSize(), lStyle);
        if (!m_pEditor) return(false);
    }

    long lValue = rvInitialValue.GetInteger();
    lValue = ClampValue(lValue);
    if (m_bTriState)
    {
        m_pEditor->Set3StateValue((wxCheckBoxState) lValue);
    }
    else
    {
        bool bChecked = (lValue != 0);
        m_pEditor->SetValue(bChecked);
    }

    //centre the checkbox
    wxSize s = wxVirtualDataViewRenderer::GetCheckboxSize(pParent); //m_pEditor->GetSize();

    //vertically
    int h = s.GetHeight();
    if (h < rRect.height)
    {
        //checkbox smaller than rect
        int iOffset = (rRect.height - h) / 2;
        rRect.y     += iOffset;
        rRect.height = h;
    }

    //horizontally
    int w = s.GetWidth();
    if (w < rRect.width)
    {
        //checkbox smaller than rect
        int iOffset = (rRect.width - w) / 2;
        rRect.x += iOffset;
        rRect.width = w;
    }


    m_pEditor->SetSize(rRect);

    //show it
    m_pEditor->Show();
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
bool wxVirtualDataViewCheckBoxEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                               wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    long lValue = rInOutValue.GetInteger();
    lValue = ClampValue(lValue);

    if (m_bTriState)
    {
        lValue = rInOutValue.GetInteger();
        lValue = ClampValue(lValue);
        switch(lValue)
        {
            case 0: lValue = 1; break;
            case 1: lValue = 2; break;
            case 2: lValue = 0; break;
            default: lValue = 0; break;
        }

    }
    else
    {
        switch(lValue)
        {
            case 0: lValue = 1; break;
            case 1: lValue = 0; break;
            default: lValue = 1; break;
        }
    }

    rInOutValue = lValue;
    return(true);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewCheckBoxEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCheckBoxEditor::FinishEdition(void)
{
    if (!m_pEditor) return(true);
    DestroyEditor(m_pEditor);
    m_pEditor = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewCheckBoxEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pEditor) return(false);

    long lValue = 0;
    if (m_bTriState)
    {
        switch(m_pEditor->Get3StateValue())
        {
            case wxCHK_UNCHECKED    : lValue = 0; break;
            case wxCHK_CHECKED      : lValue = 1; break;
            case wxCHK_UNDETERMINED : lValue = 2; break;
            default                 : lValue = 0; break;
        }
    }
    else
    {
        if (m_pEditor->IsChecked()) lValue = 1;
        else                        lValue = 0;
    }

    rvValue = lValue;
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewCheckBoxEditor::GetEditorWindow(void)
{
    return(m_pEditor);
}

//---------------------- TRI-STATE ----------------------------------//
/** Check if the editor is tri-state
  * \return true if the editor is tri-state, false otherwise
  */
bool wxVirtualDataViewCheckBoxEditor::IsTriState(void) const
{
    return(m_bTriState);
}

/** Set the editor to tri-state
  * \param bTriState [input]: true for setting the editor to tri-state
  *                           false for bi-state (true/false)
  */
void wxVirtualDataViewCheckBoxEditor::SetTriState(bool bTriState)
{
    m_bTriState = bTriState;
}

//------------------------ BEHAVIOURS -------------------------------//
/** Get lower bound behaviour
  * \return the lower bound behaviour
  */
wxVirtualDataViewCheckBoxEditor::EBehaviour wxVirtualDataViewCheckBoxEditor::GetLowerBoundBehaviour(void) const
{
    return(m_eLowerBoundBehaviour);
}

/** Get upper bound behaviour
  * \return the upper bound behaviour
  */
wxVirtualDataViewCheckBoxEditor::EBehaviour wxVirtualDataViewCheckBoxEditor::GetUpperBoundBehaviour(void) const
{
    return(m_eUpperBoundBehaviour);
}

/** Set lower bound behaviour
  * \param eBehaviour [input]: the new behaviour
  */
void wxVirtualDataViewCheckBoxEditor::SetLowerBoundBehaviour(EBehaviour eBehaviour)
{
    m_eLowerBoundBehaviour = eBehaviour;
}

/** Set upper bound behaviour
  * \param eBehaviour [input]: the new behaviour
  */
void wxVirtualDataViewCheckBoxEditor::SetUpperBoundBehaviour(EBehaviour eBehaviour)
{
    m_eUpperBoundBehaviour = eBehaviour;
}
