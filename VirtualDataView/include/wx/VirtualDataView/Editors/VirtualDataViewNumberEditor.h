/**********************************************************************/
/** FILE    : VirtualDataViewNumberEditor.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view number editor                              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_NUMBER_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_NUMBER_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

class WXDLLIMPEXP_CORE wxTextCtrl;
class WXDLLIMPEXP_CORE wxSpinCtrl;
class WXDLLIMPEXP_CORE wxSpinCtrlDouble;

/** \class wxVirtualDataViewNumberEditor : a number editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewNumberEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewNumberEditor(void);                                    ///< \brief constructor
        wxVirtualDataViewNumberEditor(const wxVirtualDataViewNumberEditor &rhs);///< \brief copy constructor
        wxVirtualDataViewNumberEditor&
            operator=(const wxVirtualDataViewNumberEditor &rhs);                ///< \brief assignation operator
        virtual ~wxVirtualDataViewNumberEditor(void);                           ///< \brief destructor

        //interface implementation
        virtual wxVirtualDataViewEditor* Clone(void) const;                     ///< \brief clone the editor
        virtual bool      DoStartEditing(const wxVirtualItemID &rID,
                                         size_t uiField,
                                         const wxVariant &rvInitialValue,
                                         wxWindow *pParent,
                                         wxRect rRect);                         ///< \brief start editing
        virtual bool      Activate(const wxVirtualItemID &rID,
                                   size_t uiField, wxRect rRect, wxPoint ptPos,
                                   wxVariant &rInOutValue);                     ///< \brief answer to a single click/space/return
        virtual bool      CanFinishEdition(void);                               ///< \brief test if edition can be finished
        virtual bool      FinishEdition(void);                                  ///< \brief finish the edition
        virtual bool      GetValue(wxVariant &rvValue);                         ///< \brief get the value
        virtual wxWindow* GetEditorWindow(void);                                ///< \brief get the editor window

        //range
        void SetRange(const wxVariant &rvMin, const wxVariant &rvMax);          ///< \brief set the range
        void SetRange(double fMin, double fMax);                                ///< \brief set the range
        void SetRange(long lMin, long lMax);                                    ///< \brief set the range
        void SetRangeAuto(void);                                                ///< \brief compute automatically the range
        void GetRange(wxVariant &rvMin, wxVariant &rvMax) const;                ///< \brief get the range
        void GetRange(double &fMin, double &fMax) const;                        ///< \brief get the range
        void GetRange(long &lMin, long &lMax) const;                            ///< \brief get the range

        //increment
        void SetIncrement(double fIncrement);                                   ///< \brief set the increment
        double GetIncrement(void) const;                                        ///< \brief get the increment

        //floating point / integer
        bool    IsFloatingPoint(void) const;                                    ///< \brief return true if the format is floating point
        bool    IsInteger(void) const;                                          ///< \brief return true if the format is integer
        void    SetFloatingPoint(bool bFloat = true);                           ///< \brief set floating point / integer
        void    SetInteger(bool bInteger = true);                               ///< \brief set integer / floating point

        //spin control or text control
        bool    UseTextControl(void) const;                                     ///< \brief return true if a text control is used
        bool    UseSpinControl(void) const;                                     ///< \brief return true if a spin control is used
        void    SetTextControl(bool bTextCtrl = true);                          ///< \brief toggle the use of text/spin control
        void    SetSpinControl(bool bSpinCtrl = true);                          ///< \brief toggle the use of spin/text control

    protected:
        //union
        union UControl
        {
            wxTextCtrl*             m_pTextControl;                             ///< \brief the text control
            wxSpinCtrl*             m_pSpinControl;                             ///< \brief the spin control
            wxSpinCtrlDouble*       m_pSpinControlDouble;                       ///< \brief the floating point spin control
        };

        //data
        UControl                m_Control;                                      ///< \brief the editor control
        wxUint8                 m_bUseSpinControl   : 1;                        ///< \brief true for using spin control
        wxUint8                 m_bIsFloatingPoint  : 1;                        ///< \brief true for floating point format
        wxVariant               m_vMin;                                         ///< \brief minimal value
        wxVariant               m_vMax;                                         ///< \brief maximal value
        double                  m_fIncrement;                                   ///< \brief increment value for double spin control
        double                  m_fValue;                                       ///< \brief value for validator
        long                    m_lValue;                                       ///< \brief value for validator

        //method
        void Copy(const wxVirtualDataViewNumberEditor &rhs);                    ///< \brief copy an object
        bool CreateTextEditor(const wxVariant &rvInitialValue,
                              wxWindow *pParent, wxRect rRect);                 ///< \brief create the text editor
        bool CreateSpinEditor(const wxVariant &rvInitialValue,
                              wxWindow *pParent, wxRect rRect);                 ///< \brief create the spin editor
        bool CreateSpinDoubleEditor(const wxVariant &rvInitialValue,
                                    wxWindow *pParent, wxRect rRect);           ///< \brief create the spin double editor
        void AdjustEditorSize(wxTextCtrl *pTextControl) const;                  ///< \brief adjust the editor size
        void OnCharEvent(wxKeyEvent &rEvent);                                   ///< \brief char events handler
        void OnTextEnterEvent(wxCommandEvent &rEvent);                          ///< \brief process a return event
};

#endif

