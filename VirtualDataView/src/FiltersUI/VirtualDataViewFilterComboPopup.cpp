/**********************************************************************/
/** FILE    : VirtualDataViewFilterComboPopup.cpp                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : popup for a custom combobox for filters User Interface **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterComboPopup.h>


//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//
const char wxVirtualDataViewFilterComboPopupNameStr[] = "wxVirtualDataViewFilterComboPopup";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewFilterComboPopup::wxVirtualDataViewFilterComboPopup(void)
    : wxPARENT_POPUP_BASE(),
      wxComboPopup()
{
}

/** Destructor (virtual)
  */
wxVirtualDataViewFilterComboPopup::~wxVirtualDataViewFilterComboPopup(void)
{

}

//------------------ wxComboPopup INTERFACE IMPLEMENTATION ----------//
/** Create the window
  * \param pParent   [input]: parent of the window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewFilterComboPopup::Create(wxWindow *pParent)
{
    long lStyle = wxTAB_TRAVERSAL | wxBORDER_THEME;

    #if WX_COMBO_POPUP_RESIZEABLE != 0
        bool bResult = wxResizeableWindow::Create(pParent, wxNewId());
        if (!bResult) return(false);
        SetNativeResizeBorder(false);
        SetResizeBorders(-1, 5, -1, 5);

        wxVirtualDataViewFilterPanel *pPanel;
        pPanel = new wxVirtualDataViewFilterPanel(this, wxNewId(),
                                                  wxDefaultPosition, wxDefaultSize, lStyle);
    #else

        //create filtering panel
        bool bResult = wxVirtualDataViewFilterPanel::Create(pParent, wxNewId(),
                                                            wxDefaultPosition, wxDefaultSize, lStyle);
    #endif // WX_COMBO_POPUP_RESIZEABLE

    return(bResult);
}

/** Get the created control
  * \return a pointer to the created control
  */
wxWindow* wxVirtualDataViewFilterComboPopup::GetControl(void)
{
    return(this);
}

/** Set the current value
  * \param rsValue [input] : the new current value
  */
void wxVirtualDataViewFilterComboPopup::SetStringValue(const wxString &rsValue)
{

}

/** Get the current value
  * \return the current value
  */
wxString wxVirtualDataViewFilterComboPopup::GetStringValue(void) const
{
    wxVirtualDataViewFilterPanel* pFilterPanel = GetFilterPanel();
    if (!pFilterPanel) return(wxEmptyString);

    wxVirtualDataViewFilterCtrl *pFilterCtrl = pFilterPanel->GetFilterControl();
    if (!pFilterCtrl) return(wxEmptyString);

    return(pFilterCtrl->GetFilterDisplayString());
}

//--------------------- MAIN COMPONENTS -----------------------------//
/** Get the filter panel
  * \return the filter panel
  */
wxVirtualDataViewFilterPanel* wxVirtualDataViewFilterComboPopup::GetFilterPanel(void) const
{
    #if WX_COMBO_POPUP_RESIZEABLE != 0
        wxWindowList &vChildren = GetChildren();
        wxWindowList::iterator it = vChildren.begin();
        if (it == vChildren.end()) return(WX_VDV_NULL_PTR);

        return((wxVirtualDataViewFilterPanel*) *it);

    #else
        const wxVirtualDataViewFilterPanel *pPanel = this;
        return(const_cast<wxVirtualDataViewFilterPanel*>(pPanel));
    #endif // WX_COMBO_POPUP_RESIZEABLE
}
