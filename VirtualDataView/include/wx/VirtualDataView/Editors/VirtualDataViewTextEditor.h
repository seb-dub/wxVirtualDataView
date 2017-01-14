/**********************************************************************/
/** FILE    : VirtualDataViewTextEditor.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view text editor                                **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_TEXT_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_TEXT_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/icon.h>

class WXDLLIMPEXP_CORE wxTextCtrl;

/** \class wxVirtualDataViewTextEditor : a text editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewTextEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewTextEditor(void);                                      ///< \brief constructor
        wxVirtualDataViewTextEditor(const wxVirtualDataViewTextEditor &rhs);    ///< \brief copy constructor
        wxVirtualDataViewTextEditor&
            operator=(const wxVirtualDataViewTextEditor &rhs);                  ///< \brief assignation operator
        virtual ~wxVirtualDataViewTextEditor(void);                             ///< \brief destructor

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

    protected:
        //data
        wxTextCtrl*     m_pTextControl;                                         ///< \brief the text control
        wxIcon          m_Icon;                                                 ///< \brief saved icon
        bool            m_bBitmapText;                                          ///< \brief true if the data should be converted to bitmap + text

        //method
        void Copy(const wxVirtualDataViewTextEditor &rhs);                      ///< \brief copy an object
        void AdjustEditorSize(void) const;                                      ///< \brief adjust the editor size
        void OnCharEvent(wxKeyEvent &rEvent);                                   ///< \brief char events handler
        void OnTextEnterEvent(wxCommandEvent &rEvent);                          ///< \brief process a return event
};

#endif

