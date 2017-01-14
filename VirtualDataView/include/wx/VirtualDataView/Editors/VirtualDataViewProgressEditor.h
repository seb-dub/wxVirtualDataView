/**********************************************************************/
/** FILE    : VirtualDataViewProgressEditor.h                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view progress editor                            **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_PROGRESS_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_PROGRESS_EDITOR_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

/** \class wxVirtualDataViewProgressEditor : a progress editor for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewProgressEditor : public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewProgressEditor(void);                                          ///< \brief constructor
        wxVirtualDataViewProgressEditor(const wxVirtualDataViewProgressEditor &rhs);    ///< \brief copy constructor
        wxVirtualDataViewProgressEditor&
            operator=(const wxVirtualDataViewProgressEditor &rhs);                      ///< \brief assignation operator
        virtual ~wxVirtualDataViewProgressEditor(void);                                 ///< \brief destructor

        //interface implementation
        virtual wxVirtualDataViewEditor* Clone(void) const;                             ///< \brief clone the editor
        virtual bool      DoStartEditing(const wxVirtualItemID &rID,
                                         size_t uiField,
                                         const wxVariant &rvInitialValue,
                                         wxWindow *pParent,
                                         wxRect rRect);                                 ///< \brief start editing
        virtual bool      Activate(const wxVirtualItemID &rID,
                                   size_t uiField, wxRect rRect, wxPoint ptPos,
                                   wxVariant &rInOutValue);                             ///< \brief answer to a single click/space/return
        virtual bool      CanFinishEdition(void);                                       ///< \brief test if edition can be finished
        virtual bool      FinishEdition(void);                                          ///< \brief finish the edition
        virtual bool      GetValue(wxVariant &rvValue);                                 ///< \brief get the value
        virtual wxWindow* GetEditorWindow(void);                                        ///< \brief get the editor window

        //range
        void SetRange(int iMin, int iMax);                                              ///< \brief set the valid range of the progress bar
        void GetRange(int &iMin, int &iMax) const;                                      ///< \brief get the valid range of the progress bar

        //draw value
        bool IsValueDrawn(void) const;                                                  ///< \brief check if the value is drawn as text
        void SetValueIsDrawn(bool bIsDrawn);                                            ///< \brief set if the value is drawn as text

    protected:
        //data
        wxWindow*       m_pEditor;                                                      ///< \brief the slider
        int             m_iMin;                                                         ///< \brief minimal value
        int             m_iMax;                                                         ///< \brief maximal value
        bool            m_bDrawValue;                                                   ///< \brief draw the text value of the item

        //method
        void Copy(const wxVirtualDataViewProgressEditor &rhs);                          ///< \brief copy an object
};

#endif

