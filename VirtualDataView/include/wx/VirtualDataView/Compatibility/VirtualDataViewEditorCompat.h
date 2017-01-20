/**********************************************************************/
/** FILE    : VirtualDataViewEditorCompat.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualDataViewEditor wrapping a wxDVCRenderer     **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_EDITOR_COMPAT_H_
#define _VIRTUAL_DATA_VIEW_EDITOR_COMPAT_H_

#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
class WXDLLIMPEXP_ADV wxDataViewCustomRenderer;

/** \class wxVirtualDataViewEditorCompat : an editor which uses internally a wxDataViewRenderer
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewEditorCompat: public wxVirtualDataViewEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewEditorCompat(
                        wxDataViewCustomRenderer* pRenderer = WX_VDV_NULL_PTR);     ///< \brief default constructor
        virtual ~wxVirtualDataViewEditorCompat(void);                               ///< \brief destructor

        //interface implementation
        //interface
        virtual wxVirtualDataViewEditor* Clone(void) const;                         ///< \brief clone the editor
        virtual bool      DoStartEditing(const wxVirtualItemID &rID,
                                         size_t uiField,
                                         const wxVariant &rvInitialValue,
                                         wxWindow *pParent,
                                         wxRect rRect);                             ///< \brief start editing
        virtual bool      Activate(const wxVirtualItemID &rID,
                                   size_t uiField, wxRect rRect,
                                   wxPoint ptPos,
                                   wxVariant &rInOutValue);                         ///< \brief answer to a single click/space/return
        virtual bool      CanFinishEdition(void);                                   ///< \brief test if edition can be finished
        virtual bool      FinishEdition(void);                                      ///< \brief finish the edition
        virtual bool      GetValue(wxVariant &rvValue);                             ///< \brief get the value
        virtual wxWindow* GetEditorWindow(void);                                    ///< \brief get the editor window

    protected:
        //data
        wxDataViewCustomRenderer*   m_pRenderer;                                    ///< \brief the renderer
        wxWindow*                   m_pEditorWindow;                                ///< \brief the editor window

};

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
#endif

