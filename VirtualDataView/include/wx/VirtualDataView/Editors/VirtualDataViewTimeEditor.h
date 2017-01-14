/**********************************************************************/
/** FILE    : VirtualDataViewTimeEditor.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view time editor                                **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_TIME_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_TIME_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

class WXDLLIMPEXP_ADV wxTimePickerCtrl;

/** \class wxVirtualDataViewTimeEditor : a time editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewTimeEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewTimeEditor(void);                                      ///< \brief constructor
        wxVirtualDataViewTimeEditor(const wxVirtualDataViewTimeEditor &rhs);    ///< \brief copy constructor
        wxVirtualDataViewTimeEditor&
            operator=(const wxVirtualDataViewTimeEditor &rhs);                  ///< \brief assignation operator
        virtual ~wxVirtualDataViewTimeEditor(void);                             ///< \brief destructor

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
        wxTimePickerCtrl*   m_pTimePickerCtrl;                                  ///< \brief the time picker control

        //method
        void Copy(const wxVirtualDataViewTimeEditor &rhs);                      ///< \brief copy an object
};

#endif

