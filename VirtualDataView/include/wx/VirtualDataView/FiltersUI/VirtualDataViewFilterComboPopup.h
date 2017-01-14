/**********************************************************************/
/** FILE    : VirtualDataViewFilterComboPopup.h                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : popup for a custom combobox for filters User Interface **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_FILTER_COMBO_POPUP_H_
#define _WX_VIRTUAL_VIEW_FILTER_COMBO_POPUP_H_


#define WX_COMBO_POPUP_RESIZEABLE       0

#if WX_COMBO_POPUP_RESIZEABLE != 0
    #include <wx/VirtualDataView/ResizeableWindow/ResizeableWindow.h>
    #define wxPARENT_POPUP_BASE     wxResizeableWindow
#else
    #define wxPARENT_POPUP_BASE     wxVirtualDataViewFilterPanel
#endif // WX_COMBO_POPUP_RESIZEABLE

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPanel.h>
#include <wx/combo.h>

extern const char wxVirtualDataViewFilterComboPopupNameStr[];

class WXDLLIMPEXP_VDV wxVirtualDataViewFilterPanel;

/** \class wxVirtualDataViewFilterComboPopup : a custom combobox for filters User Interface.
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFilterComboPopup : public wxPARENT_POPUP_BASE,
                                                          public wxComboPopup
{
    public:

        //constructors & destructor
        wxVirtualDataViewFilterComboPopup(void);                                    ///< \brief default constructor. Create() must be called manually
        virtual ~wxVirtualDataViewFilterComboPopup(void);                           ///< \brief destructor

        //wxComboPopup interface implementation
        virtual bool Create(wxWindow *pParent);                                     ///< \brief create the window
        virtual wxWindow* GetControl(void);                                         ///< \brief get the created control
        virtual void SetStringValue(const wxString &rsValue);                       ///< \brief set the current value
        virtual wxString GetStringValue(void) const;                                ///< \brief get the current value

        //main components
        wxVirtualDataViewFilterPanel*   GetFilterPanel(void) const;                 ///< \brief get the filter panel
};

#endif // _WX_VLBOX_H_

