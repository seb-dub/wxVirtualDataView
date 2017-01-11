/**********************************************************************/
/** FILE    : VirtualDataViewBase.h                                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a base class for viewing a very large amount of items  **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_BASE_H_
#define _WX_VIRTUAL_VIEW_BASE_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualIModelRenderer.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewEvent.h>
#include <wx/systhemectrl.h>
#include <wx/control.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewCtrl;
class WXDLLIMPEXP_VDV wxVirtualIDataModel;
class WXDLLIMPEXP_VDV wxVirtualIProxyDataModel;
class WXDLLIMPEXP_VDV wxVirtualIModelRenderer;
class WXDLLIMPEXP_VDV wxVirtualIStateModel;
class WXDLLIMPEXP_VDV wxVirtualItemID;

extern const char wxsVirtualDataViewBaseName[];

class WXDLLIMPEXP_VDV wxVirtualDataViewBase : public wxSystemThemedControl<wxControl>
{
    public:

        //constructors & destructor
        wxVirtualDataViewBase(void);                                      ///< \brief default constructor. Create() must be called manually
        wxVirtualDataViewBase(wxWindow *pParent, wxWindowID id,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long lStyle    = 0,
                    wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR,
                    wxVirtualIStateModel *pStateModel = WX_VDV_NULL_PTR,
                    wxVirtualIModelRenderer *pModelRenderer = WX_VDV_NULL_PTR,
                            wxString sName = wxsVirtualDataViewBaseName); ///< \brief constructor
        virtual ~wxVirtualDataViewBase(void);                             ///< \brief destructor

        //initialization
        bool Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                    const wxSize& size, long lStyle, wxString sName);       ///< \brief really creates the control and sets the initial number of items in it

        //windows management
        #ifdef __WXUNIVERSAL__
        bool IsCurrent(void) const;                                         ///< \brief check if the current window is the current one
        #endif

        //visual attributes
        wxVisualAttributes GetDefaultAttributes(void) const;                ///< \brief get the default window attributes
        static wxVisualAttributes
         GetClassDefaultAttributes(wxWindowVariant variant =
                                   wxWINDOW_VARIANT_NORMAL);                ///< \brief get the default window attributes

        //owner window
        wxVirtualDataViewCtrl*  GetOwner(void) const;                       ///< \brief get the owner window
        void                    SetOwner(wxVirtualDataViewCtrl* pOwner);    ///< \brief set the owner window

        //data model
        wxVirtualIDataModel*    GetBaseDataModel(void) const;               ///< \brief get the current data model used, without any proxies
        wxVirtualIDataModel*    GetDataModel(void) const;                   ///< \brief get the current data model used, with all proxies if any
        void                    SetDataModel(wxVirtualIDataModel *pModel);  ///< \brief set the current data model. Current proxies will be kept

        //proxy data models
        void    AttachProxyModel(wxVirtualIProxyDataModel *pModel);         ///< \brief attach a proxy data model to the chain of models
        void    PopProxyModel(void);                                        ///< \brief remove the last proxy data model from the chain of models
        void    DetachProxyModel(wxVirtualIProxyDataModel *pModel);         ///< \brief remove a proxy data model from the chain of models
        void    DetachAllProxyModels(void);                                 ///< \brief remove all proxy data models from the chain of models

        //state model
        wxVirtualIStateModel* GetStateModel(void) const;                    ///< \brief get the state model
        void SetStateModel(wxVirtualIStateModel *pStateModel);              ///< \brief set the state model

        //model renderer
        wxVirtualIModelRenderer* GetModelRenderer(void) const;              ///< \brief get the model renderer
        void SetModelRenderer(wxVirtualIModelRenderer *pModelRenderer);     ///< \brief set the model renderer

        //repaint
        void UpdateDisplay(void);                                           ///< \brief update the display
        void UpdateDisplay(wxDC &rDC);                                      ///< \brief update the display

        //item edition
        bool            StartItemEdition(const wxVirtualItemID &rID,
                                         int iCol);                         ///< \brief start the edition of an item
        wxVirtualItemID GetEditedItem(void) const;                          ///< \brief get the currently edited item
        int             GetEditedColumn(void) const;                        ///< \brief get the currently edited column
        bool            FinishEdition(bool bCancelled);                     ///< \brief provokes the end of the current edition

        //callbacks for data change
        void OnDataModelChanged(void);                                      ///< \brief called by the client to notify that the data model has changed
        void OnLayoutChanged(void);                                         ///< \brief called by the client to notify changes in layout

        //sending events
        bool SendEvent(wxEventType eType, const wxVirtualItemID &rID,
                       size_t uiField = 0, size_t uiCol = 0);               ///< \brief send an event
        bool SendEventAndCheck(wxEventType eType,
                               const wxVirtualItemID &rID,
                               size_t uiField = 0, size_t uiCol = 0);       ///< \brief send an event and check if it was vetoed
        bool SendEventExpanding(const wxVirtualItemID &rID);                ///< \brief send event when item is expanding
        bool SendEventCollapsing(const wxVirtualItemID &rID);               ///< \brief send event when item is collapsing
        void SendEventExpanded(const wxVirtualItemID &rID);                 ///< \brief send event when item is expanded
        void SendEventCollapsed(const wxVirtualItemID &rID);                ///< \brief send event when item is collapsed
        bool SendEventChecking(const wxVirtualItemID &rID);                 ///< \brief send event when item is checking
        void SendEventChecked(const wxVirtualItemID &rID);                  ///< \brief send event when item is checked
        bool SendEventMouseEvent(const wxVirtualIModelRenderer::THitTest &rHT,
                                 wxMouseEvent &rEvent);                     ///< \brief send mouse move event on item
        bool SendKeyboardEvent(const wxVirtualItemID &rID, size_t uiField,
                               size_t uiCol, wxKeyEvent &rEvent);           ///< \brief send a keyboard event on item
        bool SendScrollEvent(wxScrollWinEvent &rEvent);                     ///< \brief send a scrolling event
        bool SendSelectionChangingEvent(wxVirtualItemIDs &vOld,
                                        wxVirtualItemIDs &vNew);            ///< \brief send a selection changing event
        bool SendSelectionChangedEvent(wxVirtualItemIDs &vOld,
                                       wxVirtualItemIDs &vNew);             ///< \brief send a selection changed event
        bool SendSelectionChangingEventByCopy(const wxVirtualItemIDs &vOld,
                                              const wxVirtualItemIDs &vNew);///< \brief send a selection changing event
        bool SendSelectionChangedEventByCopy(const wxVirtualItemIDs &vOld,
                                             const wxVirtualItemIDs &vNew); ///< \brief send a selection changed event
        bool SendItemFocusedEvent(const wxVirtualItemID &rID,
                              size_t uiField, size_t uiCol, bool bFocused); ///< \brief send a get focus / lose focus event
        bool SendContextMenuEvent(const wxVirtualItemID &rID,
                              size_t uiField, size_t uiCol, int x, int y);  ///< \brief send a context menu event
        bool SendActivationEvent(const wxVirtualItemID &rID,
                                 size_t uiField, size_t uiCol);             ///< \brief activate the item
        bool SendStartEditingEvent(const wxVirtualItemID &rID,
                                   size_t uiField, size_t uiCol);           ///< \brief send an event when item edition starts
        bool SendEditionStartedEvent(const wxVirtualItemID &rID,
                                     size_t uiField, size_t uiCol);         ///< \brief send an event after item edition has started
        bool SendEditingEndEvent(const wxVirtualItemID &rID,
                                 size_t uiField, size_t uiCol,
                                 bool bCancel);                             ///< \brief send an event after item edition finished

        bool SendItemValueChangingEvent(const wxVirtualItemID &rID,
                                       size_t uiField, size_t uiCol,
                                       const wxVariant &rvValue);           ///< \brief send an event when item is changing value
        bool SendItemValueChangedEvent(const wxVirtualItemID &rID,
                                       size_t uiField, size_t uiCol,
                                       const wxVariant &rvValue);           ///< \brief send an event when item changed value

        //focused column
        int GetFocusedColumn(void) const;                                   ///< \brief get focused column ID
        void SetFocusedColumn(int iCol);                                    ///< \brief set focused column ID
        void RefreshHeaders(void);                                          ///< \brief refresh the headers windows

        //column resizing
        void StartResizingColumn(size_t uiCol, int iNewWidth);              ///< \brief start resizing a column
        void ResizingColumn(size_t uiCol, int iNewWidth);                   ///< \brief currently resizing a column
        void ResizeColumn(size_t uiCol, int iNewWidth);                     ///< \brief finish resizing a column
        void AutoSizeColumn(size_t uiColumn);                               ///< \brief resize the column

        //scrollbars
        void UpdateScrollbars(wxSize sNewClientSize);                       ///< \brief update scrollbars

    protected:
        //data
        wxVirtualDataViewCtrl*              m_pOwner;                       ///< \brief owner window
        wxVirtualIDataModel*                m_pDataModel;                   ///< \brief the data model, with proxies
        wxVirtualIStateModel*               m_pStateModel;                  ///< \brief the state model
        wxVirtualIModelRenderer*            m_pModelRenderer;               ///< \brief the model renderer

        //methods
        void Init(wxVirtualIDataModel *pDataModel,
                  wxVirtualIStateModel *pStateModel,
                  wxVirtualIModelRenderer *pModelRenderer);                 ///< \brief common part of all constructors
        void Release(void);                                                 ///< \brief release internal resources

        //interface: event handlers
        virtual void OnPaintEvent(wxPaintEvent &rEvent);                    ///< \brief paint handler
        virtual void OnEraseEvent(wxEraseEvent &rEvent);                    ///< \brief erase background event handler
        virtual void OnSizeEvent(wxSizeEvent &rEvent);                      ///< \brief sizing event handler
        virtual void OnScrollEvent(wxScrollWinEvent &rEvent);               ///< \brief a scroll event occurred
        virtual void OnSetOrKillFocus(wxFocusEvent &rEvent);                ///< \brief change focus handler
        virtual void OnKeyboardEvent(wxKeyEvent &rEvent);                   ///< \brief a keyboard event occurred
        virtual void OnMouseEvent(wxMouseEvent &rEvent);                    ///< \brief a mouse event occurred
        virtual void OnContextMenu(wxContextMenuEvent &rEvent);             ///< \brief a context menu event occurred
        virtual void OnHelpEvent(wxHelpEvent &rEvent);                      ///< \brief a help event occurred
        virtual void OnEditionEnded(wxCommandEvent &rEvent);                ///< \brief end edition of a cell

        //interface with default behaviour
        virtual wxVisualAttributes DoGetDefaultAttributes(void) const;      ///< \brief get the default window attributes

        //wxWindow sizing
        virtual wxSize DoGetBestSize(void) const;                           ///< \brief get the best size for the item

    private:

        //events
        void LogMessage(wxString sMsg);                                     ///< \brief print a log message
        void InitEvent(wxVirtualDataViewEvent &rEvent,
                       const wxVirtualItemID &rID, size_t uiField,
                       size_t uiCol);                                       ///< \brief initialize an event
        bool SendEvent(wxVirtualDataViewEvent &rEvent);                     ///< \brief send an event

        wxDECLARE_NO_COPY_CLASS(wxVirtualDataViewBase);
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataViewBase);
};

#endif // _WX_VIRTUAL_VIEW_BASE_H_

