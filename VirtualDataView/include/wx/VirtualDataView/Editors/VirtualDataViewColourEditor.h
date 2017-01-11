/**********************************************************************/
/** FILE    : VirtualDataViewColourEditor.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view colour editor                              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_COLOUR_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_COLOUR_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

class WXDLLIMPEXP_CORE wxColourPickerCtrl;

/** \class wxVirtualDataViewColourEditor : a colour editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewColourEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewColourEditor(void);                                        ///< \brief constructor
        wxVirtualDataViewColourEditor(const wxVirtualDataViewColourEditor &rhs);    ///< \brief copy constructor
        wxVirtualDataViewColourEditor&
            operator=(const wxVirtualDataViewColourEditor &rhs);                    ///< \brief assignation operator
        virtual ~wxVirtualDataViewColourEditor(void);                               ///< \brief destructor

        //interface implementation
        virtual wxVirtualDataViewEditor* Clone(void) const;                         ///< \brief clone the editor
        virtual bool      DoStartEditing(const wxVirtualItemID &rID,
                                         size_t uiField,
                                         const wxVariant &rvInitialValue,
                                         wxWindow *pParent,
                                         wxRect rRect);                             ///< \brief start editing
        virtual bool      Activate(const wxVirtualItemID &rID,
                                   size_t uiField, wxRect rRect, wxPoint ptPos,
                                   wxVariant &rInOutValue);                         ///< \brief answer to a single click/space/return
        virtual bool      CanFinishEdition(void);                                   ///< \brief test if edition can be finished
        virtual bool      FinishEdition(void);                                      ///< \brief finish the edition
        virtual bool      GetValue(wxVariant &rvValue);                             ///< \brief get the value
        virtual wxWindow* GetEditorWindow(void);                                    ///< \brief get the editor window

    protected:
        //data
        wxColourPickerCtrl*     m_pColourPicker;                                    ///< \brief the colour dialog

        //method
        void Copy(const wxVirtualDataViewColourEditor &rhs);                        ///< \brief copy an object
};

#endif

