/**********************************************************************/
/** FILE    : VirtualDataViewFilterComboCtrl.cpp                     **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a custom combobox for filters User Interface           **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterComboCtrl.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterComboPopup.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPanel.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPopup.h>
#include <wx/VirtualDataView/Filters/VirtualDataViewStringFilter.h>
#include <wx/textctrl.h> //wxEVT_TEXT_ENTER
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include "FilterEditors/ComboboxState.hpp"

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewFilterComboCtrl, wxComboCtrl);
const char wxVirtualDataViewFilterComboCtrlNameStr[] = "wxVirtualDataViewFilterComboCtrl";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewFilterComboCtrl::wxVirtualDataViewFilterComboCtrl(void)
    : wxComboCtrl(),
      m_bCustomDialogPopup(true),
      m_pFilterDialog(WX_VDV_NULL_PTR),
      m_bGeneratedByTextEntry(false)
{
    InitPopupWindow();
    BindEvents();
}

/** Constructor
  * Creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param validator [input]: validator
  * \param sName     [input]: the name of the object
  */
wxVirtualDataViewFilterComboCtrl::wxVirtualDataViewFilterComboCtrl(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                                                   const wxSize& size, long lStyle,
                                                                   const wxValidator& validator, wxString sName)

    : wxComboCtrl(pParent, id, wxEmptyString, pos, size, lStyle, validator, sName),
      m_bCustomDialogPopup(true),
      m_pFilterDialog(WX_VDV_NULL_PTR),
      m_pComboboxState(WX_VDV_NULL_PTR)
{
    InitPopupWindow();
    BindEvents();
}

/** Destructor (virtual)
  */
wxVirtualDataViewFilterComboCtrl::~wxVirtualDataViewFilterComboCtrl(void)
{

}

//-------------------- DESTRUCTION ----------------------------------//
/** Notify that the control is going to be destroyed
  */
void wxVirtualDataViewFilterComboCtrl::AboutToBeDestroyed(void)
{
    if (!m_pComboboxState) return;
    m_pComboboxState->InitFromComboBox(this, m_btnArea, m_btnState,
                                       m_btnSpacingX, m_blankButtonBg,
                                       m_hasFgCol, true);
}

//----------------------- POPUP WINDOW ------------------------------//
/** Initialize the popup window
  */
void wxVirtualDataViewFilterComboCtrl::InitPopupWindow(void)
{
    if (m_bCustomDialogPopup)
    {
        wxComboPopup *pPopup = GetPopupControl();
        if (pPopup) delete(pPopup);
        SetPopupControl(WX_VDV_NULL_PTR);
        m_pFilterDialog = new wxVirtualDataViewFilterPopup(this);
    }
    else
    {
        if (m_pFilterDialog)
        {
            m_pFilterDialog->Close();
            m_pFilterDialog->Destroy();
            m_pFilterDialog = WX_VDV_NULL_PTR;
        }

        wxVirtualDataViewFilterComboPopup *pPopup = new wxVirtualDataViewFilterComboPopup();
        SetPopupControl(pPopup);
        EnablePopupAnimation(true);
    }
}

/** Bind the common events
  */
void wxVirtualDataViewFilterComboCtrl::BindEvents(void)
{
    Bind(wxEVT_PAINT, &wxVirtualDataViewFilterComboCtrl::OnPaintCombo, this);
    Bind(wxEVT_TEXT, &wxVirtualDataViewFilterComboCtrl::OnTextChanged, this);
    Bind(wxEVT_TEXT_ENTER, &wxVirtualDataViewFilterComboCtrl::OnTextChanged, this);
}

//---------------------- FILL FILTER --------------------------------//
/** Fill the filter with data from a source model
  * \param pSrcModel   [input]: the data model containing the value to display inside the filter
  * \param uiField     [input]: the field of the data model to consider
  * \param pStateModel [input]: the state model to use
  */
void wxVirtualDataViewFilterComboCtrl::InitFilter(wxVirtualIDataModel *pSrcModel, size_t uiField,
                                                  wxVirtualIStateModel *pStateModel)
{
    wxVirtualDataViewFilterPanel *pFilterPanel = GetFilterPanel();
    if (pFilterPanel) pFilterPanel->InitFilter(pSrcModel, uiField, pStateModel);
}

//---------------------- OVERRIDE -----------------------------------//
/** Custom button action
  * If a custom dialog box is used, then it is displayed in modal mode
  * Otherwise a standard popup is displayed
  */
void wxVirtualDataViewFilterComboCtrl::OnButtonClick(void)
{
    if (m_bCustomDialogPopup)
    {
        wxCommandEvent oEvent(wxEVT_COMBOBOX_DROPDOWN, GetId());
        oEvent.SetEventObject(this);
        HandleWindowEvent(oEvent);

        if (!m_pFilterDialog) m_pFilterDialog = new wxVirtualDataViewFilterPopup(this);
        int w, h, x, y;
        GetSize(&w, &h);
        x = 0; y += h;
        ClientToScreen(&x, &y);

        int x0, y0;
        GetPosition(&x0, &y0);
        ClientToScreen(&x0, &y0);

        //get screen size, for adjusting popup size
        int iScreenWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X, this);
        int iScreenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y, this);
        iScreenHeight -= y;
        iScreenHeight -= y0;

        //get best popup size
        wxSize sBestSize = m_pFilterDialog->GetBestSize();
        int iBestWidth = sBestSize.GetWidth();
        int iBestHeight = sBestSize.GetHeight();

        //limits allowed
        int iMinWidth  = m_widthMinPopup;
        int iMaxHeight = m_heightPopup;
        int iLeft      = x - m_extLeft;
        //int iRight     = x + w + m_extRight;

        //adjust width : at least iMinWidth, at most screen width - iLeft
        if (iMinWidth > iBestWidth) iBestWidth = iMinWidth;
        if (iBestWidth > iScreenWidth - iLeft) iBestWidth = iScreenWidth - iLeft;

        //adjust height: at least iMinHeight, at most screen height - y
        if (iMaxHeight <= 0) iMaxHeight = iScreenHeight;
        if (iMaxHeight > iScreenHeight) iMaxHeight = iScreenHeight;
        if (iMaxHeight < iBestHeight) iBestHeight = iMaxHeight;

        if (iMinWidth < sBestSize.GetWidth()) SetPopupMinWidth(sBestSize.GetWidth());

        m_pFilterDialog->SetSize(iLeft, y, iBestWidth, iBestHeight);

        //set current filter
        wxVirtualDataViewFilterPanel *pPanel = GetFilterPanel();
        if (pPanel)
        {
            //pPanel->ApplySearchString(GetValue(), !m_bGeneratedByTextEntry);
        }

        //show
        m_pFilterDialog->SaveState();
        m_pFilterDialog->Show();
        m_pFilterDialog->SetFocus();
    }
    else
    {
        wxComboCtrl::OnButtonClick();
    }
}

/** Set to null if dialog popup is used
  */
void wxVirtualDataViewFilterComboCtrl::DoSetPopupControl(wxComboPopup* pPopup)
{
    if (!m_bCustomDialogPopup) wxComboCtrl::DoSetPopupControl(pPopup);
}

/** Show the popup
  */
void wxVirtualDataViewFilterComboCtrl::Popup(void)
{
    //ensure that popup is large enough
    wxComboPopup *pPopup = GetPopupControl();
    if (pPopup)
    {
        wxWindow *pWin = pPopup->GetControl();
        if (pWin)
        {

            wxSize sPopupBestSize = pWin->GetBestSize();
            wxSize sComboBoxSize  = GetSize();

            if (sComboBoxSize.GetWidth() < sPopupBestSize.GetWidth())
            {
                SetPopupMinWidth(sPopupBestSize.GetWidth());
            }
        }
    }

    //show
    wxComboCtrl::Popup();
}

/** Dismiss the popup
  */
void wxVirtualDataViewFilterComboCtrl::Dismiss(void)
{
    if (m_bCustomDialogPopup)
    {
        SetBackgroundColour(GetDisplayBackColour());
        SetForegroundColour(GetDisplayTextColour());
        m_bGeneratedByTextEntry = false;

        wxCommandEvent oEvent(wxEVT_COMBOBOX_CLOSEUP, GetId());
        oEvent.SetEventObject(this);
        HandleWindowEvent(oEvent);
    }
    else
    {
        wxComboCtrl::Dismiss();
    }
}

//---------------------- PAINTING -----------------------------------//
/** Set the combobox state
  * \param pState [input] : the new combobox state. The object is not owned,
  *                         it must therefore lives longer than the combobox
  */
void wxVirtualDataViewFilterComboCtrl::SetState(wxComboboxState *pState)
{
    m_pComboboxState = pState;
}

/** Get the combobox state
  * \return the combobox state. The object is not owned by the wxVirtualDataViewFilterComboCtrl class.
  */
wxComboboxState* wxVirtualDataViewFilterComboCtrl::GetState(void)
{
    return(m_pComboboxState);
}

/** Override to allow paint inside different DC
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterComboCtrl::OnPaintCombo(wxPaintEvent &rEvent)
{
    //Determine wxDC to use based on need to double-buffer or
    //use system-generated transparent background portions
    int w, h;
    GetClientSize(&w, &h);
    wxRect rRect(0, 0, w, h);
    #ifdef __WXMSW__
    if (HasTransparentBackground())
    {
        wxPaintDC dc(this);
        PaintComboBox(dc, rRect);
    }
    else
    #endif
    {
        wxAutoBufferedPaintDC dc(this);
        PaintComboBox(dc, rRect);
    }
}

/** Paint the combobox
  * \param rDC   [input]: the device context to use
  * \param rRect [input]: the clipping rectangle
  */
void wxVirtualDataViewFilterComboCtrl::PaintComboBox(wxDC &rDC, const wxRect &rRect)
{
    if (m_pComboboxState)
    {
        m_pComboboxState->InitFromComboBox(this, m_btnArea, m_btnState,
                                           m_btnSpacingX, m_blankButtonBg, m_hasFgCol, false);
        m_pComboboxState->DrawComboBoxClientArea(this, rDC, rRect);
    }
    else
    {
        wxComboboxState oState;
        oState.InitFromComboBox(this, m_btnArea, m_btnState,
                                m_btnSpacingX, m_blankButtonBg, m_hasFgCol, false);
        oState.DrawComboBoxClientArea(this, rDC, rRect);
    }
}

//----------------------- MAIN COMPONENTS ---------------------------//
/** Access to the filter panel
  * \return the filter panel
  */
wxVirtualDataViewFilterPanel* wxVirtualDataViewFilterComboCtrl::GetFilterPanel(void)
{
    if (m_bCustomDialogPopup)
    {
        if (m_pFilterDialog) return(m_pFilterDialog->GetFilterPanel());
    }
    else
    {

        wxVirtualDataViewFilterComboPopup *pPopup = (wxVirtualDataViewFilterComboPopup*) GetPopupControl();
        if (pPopup) return(pPopup->GetFilterPanel());
    }
    return(WX_VDV_NULL_PTR);
}

/** Access to the filter control
  * \return the filter control
  */
wxVirtualDataViewFilterCtrl* wxVirtualDataViewFilterComboCtrl::GetFilterControl(void)
{
    wxVirtualDataViewFilterCtrl *pFilterCtrl = WX_VDV_NULL_PTR;
    wxVirtualDataViewFilterPanel *pPanel = GetFilterPanel();
    if (pPanel) pFilterCtrl = pPanel->GetFilterControl();
    return(pFilterCtrl);
}

//------------------- EVENT HANDLER ---------------------------------//
/** Text in the combobox has changed
  * \param rEvent [input]: the event information
  */
void wxVirtualDataViewFilterComboCtrl::OnTextChanged(wxCommandEvent &rEvent)
{
    wxVirtualDataViewFilterPanel *pPanel = WX_VDV_NULL_PTR;

    if (m_bCustomDialogPopup)
    {
        if (m_pFilterDialog) pPanel = m_pFilterDialog->GetFilterPanel();
    }
    else
    {

        wxVirtualDataViewFilterComboPopup *pPopup = (wxVirtualDataViewFilterComboPopup*) GetPopupControl();
        if (pPopup) pPanel = pPopup->GetFilterPanel();
    }

    if (pPanel)
    {
        //if (!m_bCustomDialogPopup)
        {
            pPanel->ApplySearchString(rEvent.GetString(), !m_bGeneratedByTextEntry);
        }
        SetBackgroundColour(GetDisplayBackColour());
        SetForegroundColour(GetDisplayTextColour());
    }

    m_bGeneratedByTextEntry = true;
    rEvent.Skip();
}

//------------------- CUSTOMIZATION ---------------------------------//
/** Return the filter back colour
  * \return the filter back colour. If invalid, the default colour as defined
  *         by the current theme / toolkit is used
  */
wxColour wxVirtualDataViewFilterComboCtrl::GetDisplayBackColour(void) const
{
    wxString sDisplayStr = GetValue();
    if (sDisplayStr == wxEmptyString) return(wxColour()); //invalid colour
    return(wxTheColourDatabase->Find("PINK"));
}

/** Return the filter text colour
  * \return the filter text colour. If invalid, the default colour as defined
  *         by the current theme / toolkit is used
  */
wxColour wxVirtualDataViewFilterComboCtrl::GetDisplayTextColour(void) const
{
    return(wxColour()); //invalid colour
}

//----------------------- GENERATE FILTER ---------------------------//
/** Generate the filter
  * \return the filter to apply. The filter is allocated using "new"
  *         a NULL Pointer can be returned and is perfectly valid: it means that no filters
  *         should be applied
  */
wxVirtualDataViewFilter* wxVirtualDataViewFilterComboCtrl::GenerateFilter(void)
{
    //special case
    wxString sValue = GetValue();
    if (sValue == wxEmptyString) return(WX_VDV_NULL_PTR);

    //if the text has been generated by a text entry, then use this one
    //(partial match, no regex, case insensitive)
    if (m_bGeneratedByTextEntry)
    {
        wxVirtualDataViewStringFilter *pFilter = new wxVirtualDataViewStringFilter();
        if (!pFilter) return(pFilter);
        pFilter->SetActive(true);
        pFilter->SetCaseInsensitive(true);
        pFilter->SetRecursive(true);
        pFilter->UseRegEx(false);
        pFilter->UsePartialMatch(true);
        pFilter->SetReferenceValue(sValue);

        return(pFilter);
    }

    //generated by the panel
    wxVirtualDataViewFilterPanel *pFilterPanel = GetFilterPanel();
    if (!pFilterPanel) return(WX_VDV_NULL_PTR);
    return(pFilterPanel->GenerateFilter());
}
