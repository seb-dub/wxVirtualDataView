/**********************************************************************/
/** FILE    : ComboboxState.hpp                                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : state of a combobox for static painting                **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_COMBOBOX_STATE_H_
#define _VIRTUAL_DATA_VIEW_COMBOBOX_STATE_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/gdicmn.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/bitmap.h>

class WXDLLIMPEXP_CORE  wxComboCtrl;
class WXDLLIMPEXP_CORE  wxComboPopup;
class WXDLLIMPEXP_CORE  wxWindow;
class WXDLLIMPEXP_CORE  wxDC;
class WXDLLIMPEXP_VDV   wxVirtualDataViewFilterComboCtrl;

/** \class wxComboboxState : state of a combobox for static painting
  */
class WXDLLIMPEXP_VDV wxComboboxState
{
    public:
        //constructors & destructor
        wxComboboxState(void);                                      ///< \brief constructor
        ~wxComboboxState(void);                                     ///< \brief destructor

        //initialization
        void InitFromComboBox(wxComboCtrl *pComboBox,
                              const wxRect &rButtonRect,
                              int iButtonState,
                              int iButtonSpacingX,
                              bool bBlankButtonBG,
                              bool bHasForegroundColour,
                              bool bIsDestroying);                  ///< \brief initialize from a combo box
        void Init(void);                                            ///< \brief initialize to a default state
        void Init(const wxRect &rRect);                             ///< \brief initialize to a default state

        //resize
        void RecalculateSize(const wxRect &rRect);                  ///< \brief recalculate sizes

        //drawing
        void DrawComboBoxClientArea(wxWindow *pWin, wxDC &rDC,
                                    const wxRect &rRect);           ///< \brief paint the combo box

        //text
        wxString GetValue(void) const;                              ///< \brief get the string value
        void     SetValue(wxString sValue);                         ///< \brief set the string value

    protected:
        //enums
        /// \enum EButtonDrawFlags for modifying behaviour of DrawButton
        enum EButtonDrawFlags
        {
            Button_PaintBackground             = 0x0001,            ///< \brief Paints control background below the button
            Button_BitmapOnly                  = 0x0002             ///< \brief Only paints the bitmap
        };

        //data - generic
        int             m_iStyle;                                   ///< \brief window style
        int             m_iFlags;                                   ///< \brief internal flags
        bool            m_bIsEnabled;                               ///< \brief true if the control is enabled
        bool            m_bFocus;                                   ///< \brief true if the focus should be drawn

        //data - boundaries
        int             m_iWidthBorder;                             ///< \brief border width of the control
        int             m_iWidthCustomPaint;                        ///< \brief for text rendering right of the selection

        //data - background
        bool            m_bHasTransparentBackground;                ///< \brief true if the background is transparent
        wxColour        m_cBackColour;                              ///< \brief background colour of the control
        wxColour        m_cParentBackColour;                        ///< \brief background colour of the parent

        //data - text field
        bool            m_bHasText;                                 ///< \brief true if a text area is present
        wxRect          m_rRectTextArea;                            ///< \brief size of text area
        wxFont          m_oFont;                                    ///< \brief font to use for text rendering
        wxString        m_sText;                                    ///< \brief text to render
        int             m_iMarginLeft;                              ///< \brief left margin
        bool            m_bHasFgCol;                                ///< \brief true if a foreground colour is defined
        wxColour        m_cForegroundCol;                           ///< \brief foreground colour

        //data - button
        bool            m_bHasButton;                               ///< \brief true if a button is present
        wxRect          m_rRectButton;                              ///< \brief button position
        int             m_iButtonState;                             ///< \brief state of the button (pushed, focused, ...)
        int             m_iButtonSpacingX;                          ///< \brief spacing around the button
        wxSize          m_ButtonSize;                               ///< \brief button size
        wxBitmap        m_bmpNormal;                                ///< \brief button bitmap for normal state
        wxBitmap        m_bmpPressed;                               ///< \brief button bitmap for pressed state
        wxBitmap        m_bmpHover;                                 ///< \brief button bitmap for hovering state
        wxBitmap        m_bmpDisabled;                              ///< \brief button bitmap for disabled state
        bool            m_bBlankButtonBg;                           ///< \brief true if the button has a blank background
        int             m_iButtonWidth;                             ///< \brief user defined button width
        int             m_iButtonHeight;                            ///< \brief user defined button height
        int             m_iDefaultButtonWidth;                      ///< \brief default button width

        //popup
        wxComboPopup*   m_pPopupInterface;                          ///< \brief pointer to the popup interface
        int             m_iPopupState;                              ///< \brief popup state

        //methods
        int  GetNativeTextIndent(void) const;                       ///< \brief get native text indent
        void InitTextAndButtonAreas(const wxRect &rRect);           ///< \brief calculate text and button rectangle
        void ComputeButtonSize(int iBtnWidth,
                               const wxSize &rsClientSize,
                               bool bButtonOnRightSide);            ///< \brief compute button size
        void DrawComboBoxClientAreaGeneric(wxWindow *pWin,
                                           wxDC &rDC,
                                           const wxRect &rRect);    ///< \brief paint the combo box - generic implementation
        void DrawButton(wxWindow *pWin, wxDC &rDC,
                        int iDrawFlags, int x, int y);              ///< \brief draw the button
        void DefaultPaintComboControl(wxDC &rDC,
                                      const wxRect &rRectText);     ///< \brief draw the background + text of the text entry
        void PrepareBackground(wxDC& rDC, const wxRect& rRect,
                               int iFlags);                         ///< \brief prepare combo box background on area in a way typical on platform
        bool HasTransparentBG(wxWindow *pWin);                      ///< \brief check if the window has a transparent background
};

#endif

