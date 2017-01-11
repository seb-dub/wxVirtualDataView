/**********************************************************************/
/** FILE    : VirtualDataViewChoiceEditor.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view choice editor                              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_CHOICE_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_CHOICE_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

class WXDLLIMPEXP_CORE wxComboBox;

/** \class wxVirtualDataViewChoiceEditor : a choice editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewChoiceEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewChoiceEditor(void);                                    ///< \brief constructor
        wxVirtualDataViewChoiceEditor(const wxVirtualDataViewChoiceEditor &rhs);///< \brief copy constructor
        wxVirtualDataViewChoiceEditor&
            operator=(const wxVirtualDataViewChoiceEditor &rhs);                ///< \brief assignation operator
        virtual ~wxVirtualDataViewChoiceEditor(void);                           ///< \brief destructor

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

        //choices
        void SetChoices(const wxArrayString &rvChoices);                        ///< \brief set the choices
        wxArrayString& GetChoices(void);                                        ///< \brief get the choices
        const wxArrayString& GetChoices(void) const;                            ///< \brief get the choices (const version)

        //options
        bool IsSorted(void) const;                                              ///< \brief true if the control is sorted
        void SetSorted(bool bSorted = true);                                    ///< \brief toggle sorting ON/OFF

        bool IsReadOnly(void) const;                                            ///< \brief return true if the editor is read-only
        bool IsReadWrite(void) const;                                           ///< \brief return true if the editor is read-write
        void SetReadOnly(bool bReadOnly = true);                                ///< \brief toggle read-only ON/OFF
        void SetReadWrite(bool bReadWrite = true);                              ///< \brief toggle read-write ON/OFF

    protected:
        //data
        wxComboBox*     m_pComboBox;                                            ///< \brief the combo box
        wxArrayString   m_vChoices;                                             ///< \brief list of choices
        bool            m_bReadOnly;                                            ///< \brief true for read-only
        bool            m_bSort;                                                ///< \brief true for sorting the items

        //method
        void Copy(const wxVirtualDataViewChoiceEditor &rhs);                      ///< \brief copy an object
        void AdjustEditorSize(void) const;                                      ///< \brief adjust the editor size
        void OnCharEvent(wxKeyEvent &rEvent);                                   ///< \brief char events handler
        void OnTextEnterEvent(wxCommandEvent &rEvent);                          ///< \brief process a return event
};

#endif

