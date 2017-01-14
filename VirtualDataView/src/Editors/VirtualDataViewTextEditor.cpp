/**********************************************************************/
/** FILE    : VirtualDataViewTextEditor.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view text editor                                **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewTextEditor.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/textctrl.h>

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewTextEditor::wxVirtualDataViewTextEditor(void)
    : wxVirtualDataViewEditor(),
      m_pTextControl(WX_VDV_NULL_PTR),
      m_Icon(wxNullIcon),
      m_bBitmapText(false)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewTextEditor::wxVirtualDataViewTextEditor(const wxVirtualDataViewTextEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pTextControl(WX_VDV_NULL_PTR),
      m_Icon(wxNullIcon),
      m_bBitmapText(false)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewTextEditor& wxVirtualDataViewTextEditor::operator=(const wxVirtualDataViewTextEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewTextEditor::~wxVirtualDataViewTextEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewTextEditor::Copy(const wxVirtualDataViewTextEditor &rhs)
{
    m_Icon          = rhs.m_Icon;
    m_bBitmapText   = rhs.m_bBitmapText;
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewTextEditor::Clone(void) const
{
    return(new wxVirtualDataViewTextEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewTextEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                 size_t uiField,
                                                 const wxVariant &rvInitialValue,
                                                 wxWindow *pParent,
                                                 wxRect rRect)
{
    wxString sValue;
    if (rvInitialValue.IsType("wxBitmapText"))
    {
        wxBitmapText bt;
        bt << rvInitialValue;
        sValue = bt.GetText();
        m_Icon = bt.GetIcon();
        m_bBitmapText = true;
    }
    else
    {
        m_bBitmapText = false;
        sValue = rvInitialValue.GetString();
    }

    if (m_pTextControl)
    {
        m_pTextControl->SetValue(sValue);
        m_pTextControl->SetSize(rRect);
        AdjustEditorSize();
        m_pTextControl->Show();
        return(true);
    }

    long lStyle = wxTE_PROCESS_ENTER | wxBORDER_SIMPLE ;//| wxTE_MULTILINE | wxTE_RICH;
    wxTextCtrl *pTextCtrl = new wxTextCtrl(pParent, wxNewId(),
                                           sValue,
                                           rRect.GetPosition(),
                                           rRect.GetSize(), lStyle);
    if (!pTextCtrl) return(false);
    pTextCtrl->Bind(wxEVT_CHAR, &wxVirtualDataViewTextEditor::OnCharEvent, this);
    pTextCtrl->Bind(wxEVT_TEXT_ENTER, &wxVirtualDataViewTextEditor::OnTextEnterEvent, this);

    //select the text in the control an place the cursor at the end
    pTextCtrl->SetInsertionPointEnd();
    pTextCtrl->SelectAll();

    m_pTextControl = pTextCtrl;
    AdjustEditorSize();
    m_pTextControl->Show();
    return(true);
}

/** Adjust the editor size
  */
void wxVirtualDataViewTextEditor::AdjustEditorSize(void) const
{
    if (!m_pTextControl) return;

    //shift text control in case of bitmap
    if (m_bBitmapText)
    {
        int x, y;
        m_pTextControl->GetPosition(&x, &y);
        int iWidthIcon = m_Icon.GetWidth() + 2;
        m_pTextControl->Move(x + iWidthIcon, y);

        int w, h;
        m_pTextControl->GetSize(&w, &h);
        w -= iWidthIcon;
        if (w < 1) w = 1;
        m_pTextControl->SetSize(w, h);
    }

    //Adjust size of wxTextCtrl editor to fit text, even if it means being
    //wider than the corresponding column (this is how Explorer behaves).
    const wxSize sTextSize  =  m_pTextControl->GetTextExtent(m_pTextControl->GetValue());
    const int iFittingW     =  m_pTextControl->GetSizeFromTextSize(sTextSize).x;
    const int iCurrentW     =  m_pTextControl->GetSize().x;
    const int iMaxWidth     =  m_pTextControl->GetParent()->GetSize().x
                             - m_pTextControl->GetPosition().x;

    //Adjust size so that it fits all content. Don't change anything if the
    //allocated space is already larger than needed and don't extend wxDVC's
    //boundaries.
    int iWidth = wxMin(wxMax(iCurrentW, iFittingW), iMaxWidth);

    if (iWidth != iCurrentW)
        m_pTextControl->SetSize(wxSize(iWidth, -1));
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
bool wxVirtualDataViewTextEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                           wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
    //not activable
    return(false);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewTextEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewTextEditor::FinishEdition(void)
{
    if (!m_pTextControl) return(true);
    DestroyEditor(m_pTextControl);
    m_pTextControl = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewTextEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pTextControl) return(false);

    wxString sText = m_pTextControl->GetValue();
    if (m_bBitmapText)
    {
        wxBitmapText bt(sText, m_Icon);
        rvValue << bt;
    }
    else
    {
        rvValue = sText;
    }
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewTextEditor::GetEditorWindow(void)
{
    return(m_pTextControl);
}

//----------------------- EVENT HANDLERS ----------------------------//
/** Char events handler
  * \param rEvent [input] : the event information
  */
void wxVirtualDataViewTextEditor::OnCharEvent(wxKeyEvent &rEvent)
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
void wxVirtualDataViewTextEditor::OnTextEnterEvent(wxCommandEvent &rEvent)
{
    NotifyEditionFinished(false);
}
