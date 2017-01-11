/**********************************************************************/
/** FILE    : VirtualDataViewDateEditor.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view date editor                                **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_DATE_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_DATE_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

class WXDLLIMPEXP_ADV wxDatePickerCtrl;

/** \class wxVirtualDataViewDateEditor : a date editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewDateEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewDateEditor(bool bDropDown = true,
                                    bool bPlatformDefault = false);             ///< \brief constructor
        wxVirtualDataViewDateEditor(const wxVirtualDataViewDateEditor &rhs);    ///< \brief copy constructor
        wxVirtualDataViewDateEditor&
            operator=(const wxVirtualDataViewDateEditor &rhs);                  ///< \brief assignation operator
        virtual ~wxVirtualDataViewDateEditor(void);                             ///< \brief destructor

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

        //options
        bool IsDropDown(void) const;                                            ///< \brief true if the calendar control is drop-down
        void SetDropDown(bool bDropDown = true);                                ///< \brief set drop-down / spin
        void SetSpin(bool bSpin = true);                                        ///< \brief set spin / drop-down
        void SetPlatformDropDownDefault(bool bDefault = true);                  ///< \brief set spin / drop-down which corresponds to the default of the platform
        bool IsPlatformDropDownDefaultUsed(void) const;                         ///< \brief true if platform default is used

    protected:
        //data
        wxDatePickerCtrl*   m_pDatePickerCtrl;                                  ///< \brief the date picker control
        bool                m_bDropDown;                                        ///< \brief true for drop-down control
        bool                m_bPlatformDefault;                                 ///< \brief true if platform default is to be used

        //method
        void Copy(const wxVirtualDataViewDateEditor &rhs);                      ///< \brief copy an object
};

#endif

