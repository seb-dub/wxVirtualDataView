/**********************************************************************/
/** FILE    : VirtualDataViewCheckBoxEditor.h                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view checkbox editor                            **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_CHECK_BOX_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_CHECK_BOX_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

class WXDLLIMPEXP_CORE wxCheckBox;

/** \class wxVirtualDataViewCheckBoxEditor : a check box for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewCheckBoxEditor : public wxVirtualDataViewEditor
{
    public:
        //enum
        /// \enum EBehaviour : how to handle out of bounds value
        enum EBehaviour
        {
            WX_E_OUT_OF_BOUND_IS_UNCHECKED      = 0,                            ///< \brief out of bound values are considered un-checked
            WX_E_OUT_OF_BOUND_IS_CHECKED        = 1,                            ///< \brief out of bound values are considered checked
            WX_E_OUT_OF_BOUND_IS_UNDETERMINED   = 2                             ///< \brief out of bound values are considered undetermined
        };

        //constructors & destructor
        wxVirtualDataViewCheckBoxEditor(bool bTriState = false);                ///< \brief constructor
        wxVirtualDataViewCheckBoxEditor(
            const wxVirtualDataViewCheckBoxEditor &rhs);                        ///< \brief copy constructor
        wxVirtualDataViewCheckBoxEditor&
            operator=(const wxVirtualDataViewCheckBoxEditor &rhs);              ///< \brief assignation operator
        virtual ~wxVirtualDataViewCheckBoxEditor(void);                         ///< \brief destructor

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

        //tri-state
        bool IsTriState(void) const;                                            ///< \brief check if the editor is tri-state
        void SetTriState(bool bTriState);                                       ///< \brief set the editor to tri-state

        //behaviours
        EBehaviour GetLowerBoundBehaviour(void) const;                          ///< \brief get lower bound behaviour
        EBehaviour GetUpperBoundBehaviour(void) const;                          ///< \brief get upper bound behaviour
        void       SetLowerBoundBehaviour(EBehaviour eBehaviour);               ///< \brief set lower bound behaviour
        void       SetUpperBoundBehaviour(EBehaviour eBehaviour);               ///< \brief set upper bound behaviour

    protected:
        //data
        bool            m_bTriState;                                            ///< \brief true if the editor is tristate
        wxCheckBox*     m_pEditor;                                              ///< \brief the checkbox control
        EBehaviour      m_eLowerBoundBehaviour;                                 ///< \brief how to behave for values below 0
        EBehaviour      m_eUpperBoundBehaviour;                                 ///< \brief how to behave for values above 0

        //method
        void Copy(const wxVirtualDataViewCheckBoxEditor &rhs);                  ///< \brief copy an object
        long ClampValue(long iValue);                                           ///< \brief clamp the value
};

#endif

