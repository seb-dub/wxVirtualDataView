/**********************************************************************/
/** FILE    : VirtualDataViewEditor.h                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view editor                                     **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_EDITOR_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/gdicmn.h>
#include <wx/event.h>

//forward declarations
class WXDLLIMPEXP_BASE wxVariant;
class WXDLLIMPEXP_CORE wxWindow;
class WXDLLIMPEXP_BASE wxVariant;
class WXDLLIMPEXP_VDV  wxVirtualItemID;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_VDV, wxEVT_VDV_EDITION_FINISHED, wxCommandEvent);

class WXDLLIMPEXP_VDV wxVirtualDataViewEvtHandler;

/** \class wxVirtualDataViewEditor : a cell editor for the wxVirtualDataViewCtrl control
  * When the editor is finished internally, it sends a wxCommandEvent wxEVT_VDV_EDITION_FINISHED
  * with GetInt() == 0 when edition is cancelled, or GetInt() == 1 when edition is accepted
  *
  * This allow complex editors to be created, without coupling with the producer class
  *
  * The virtual protected method NotifyEditionFinished sends this message to the its immediate parent
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewEditor : public wxEvtHandler
{
    public:
        //constructors & destructor
        wxVirtualDataViewEditor(void);                                          ///< \brief constructor
        virtual ~wxVirtualDataViewEditor(void);                                 ///< \brief destructor

        bool      StartEditing(const wxVirtualItemID &rID,
                                       size_t uiField,
                                       const wxVariant &rvInitialValue,
                                       wxWindow *pParent,
                                       wxRect rRect);                           ///< \brief start editing
        bool        HasFocus(void);                                             ///< \brief check if the editor has the focus

        //interface
        virtual wxVirtualDataViewEditor* Clone(void) const          = 0;        ///< \brief clone the editor
        virtual bool      DoStartEditing(const wxVirtualItemID &rID,
                                         size_t uiField,
                                         const wxVariant &rvInitialValue,
                                         wxWindow *pParent,
                                         wxRect rRect)              = 0;        ///< \brief start editing
        virtual bool      Activate(const wxVirtualItemID &rID,
                                   size_t uiField, wxRect rRect,
                                   wxPoint ptPos,
                                   wxVariant &rInOutValue)          = 0;        ///< \brief answer to a single click/space/return
        virtual bool      CanFinishEdition(void)                    = 0;        ///< \brief test if edition can be finished
        virtual bool      FinishEdition(void)                       = 0;        ///< \brief finish the edition
        virtual bool      GetValue(wxVariant &rvValue)              = 0;        ///< \brief get the value
        virtual wxWindow* GetEditorWindow(void)                     = 0;        ///< \brief get the editor window

    protected:
        friend class wxVirtualDataViewEvtHandler;

        //methods
        void CopyBase(const wxVirtualDataViewEditor &rhs);                      ///< \brief copy base object
        void DestroyEditor(wxWindow* pEditor);                                  ///< \brief helper method for destroying an editor
        void OnLostFocus(wxFocusEvent &rEvent);                                 ///< \brief lost focus event handler

        void RegisterFocusHandler(wxWindow *pEditor);                           ///< \brief register the focus handler

        virtual void NotifyEditionFinished(bool bCancelled);                    ///< \brief notify the parent that the edition was finished
};

#endif

