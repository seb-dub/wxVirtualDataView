/**********************************************************************/
/** FILE    : VirtualDataViewEvent.h                                 **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : an event class for wxVirtualDataViewCtrl               **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_EVENT_H_
#define _WX_VIRTUAL_VIEW_EVENT_H_

#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/event.h>

#if wxUSE_DRAG_AND_DROP
    #include <wx/dnd.h>
    #include <wx/dataobj.h>
#endif // wxUSE_DRAG_AND_DROP

class WXDLLIMPEXP_VDV   wxVirtualDataViewColumn;

/** \class wxVirtualDataViewEvent : an event class for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewEvent : public wxNotifyEvent
{
    public:

        //enum
        enum EHitTestFlags
        {
            E_HIT_TEST_ON_EXPANDER      = 0x00000001,                       ///< \brief point is on expander
            E_HIT_TEST_ON_CHECKBOX      = 0x00000002,                       ///< \brief point is on checkbox
            E_HIT_TEST_ON_IMAGE         = 0x00000004,                       ///< \brief point is on image
            E_HIT_TEST_ON_TOP_MARGIN    = 0x00000008,                       ///< \brief point is inside the top margin of the item
            E_HIT_TEST_ON_BOTTOM_MARGIN = 0x00000010,                       ///< \brief point is inside the bottom margin of the item
            E_HIT_TEST_ON_LEFT_MARGIN   = 0x00000020,                       ///< \brief point is inside the left margin of the item
            E_HIT_TEST_ON_RIGHT_MARGIN  = 0x00000040,                       ///< \brief point is inside the right margin of the item
            E_HIT_TEST_ON_TOP_EDGE      = 0x00000080,                       ///< \brief point is on the top edge of the item
            E_HIT_TEST_ON_BOTTOM_EDGE   = 0x00000100,                       ///< \brief point is on the bottom edge of the item
            E_HIT_TEST_ON_LEFT_EDGE     = 0x00000200,                       ///< \brief point is on the left edge of the item
            E_HIT_TEST_ON_RIGHT_EDGE    = 0x00000400,                       ///< \brief point is on the right edge of the item
            E_HIT_TEST_OUTSIDE_ITEM     = 0x00000800,                       ///< \brief point is outside the item. In that case, the nearest item is returned
            E_HIT_TEST_ON_ITEM          = 0x00000000                        ///< \brief point is on the item directly
        };

        //constructors & destructor
        wxVirtualDataViewEvent(wxEventType eType = wxEVT_NULL,
                               int iWinID = 0);                             ///< \brief default constructor
        wxVirtualDataViewEvent(const wxVirtualDataViewEvent &rhs);          ///< \brief copy constructor
        virtual ~wxVirtualDataViewEvent(void);                              ///< \brief destructor

        //interface
        virtual wxEvent* Clone(void) const wxOVERRIDE;                      ///< \brief clone

        //item
        void SetItem(wxVirtualItemID id);                                   ///< \brief set the item
        wxVirtualItemID GetItem(void) const;                                ///< \brief get the item
        void SetFieldIndex(size_t uiField);                                 ///< \brief set the item field index
        size_t GetFieldIndex(void) const;                                   ///< \brief get the item field index

        //selection changing / changed
        void SetDeselectedItems(const wxVirtualItemIDs &vIDs);              ///< \brief set the de-selected items (copy performed)
        void AssignDeselectedItems(wxVirtualItemIDs &vIDs);                 ///< \brief set the de-selected items (move performed)
        const wxVirtualItemIDs& GetDeselectedItems(void) const;             ///< \brief get the de-selected items
        void SetSelectedItems(const wxVirtualItemIDs &vIDs);                ///< \brief set the selected items
        void AssignSelectedItems(wxVirtualItemIDs &vIDs);                   ///< \brief set the selected items (move performed)
        const wxVirtualItemIDs& GetSelectedItems(void) const;               ///< \brief get the selected items

        //column
        void SetColumnIndex(size_t uiViewColumn);                           ///< \brief set the view column
        size_t GetColumnIndex(void) const;                                  ///< \brief get the column index
        void SetColumn(wxVirtualDataViewColumn *pCol);                      ///< \brief set the column
        wxVirtualDataViewColumn* GetColumn(void) const;                     ///< \brief get the column

        //value
        void SetValue(const wxVariant &rValue);                             ///< \brief set the value of the item
        const wxVariant& GetValue(void) const;                              ///< \brief get the value of the item

        //point
        void SetPosition(int x, int y);                                     ///< \brief set the event position
        void SetPosition(wxPoint pt);                                       ///< \brief set the event position
        wxPoint GetPosition(void) const;                                    ///< \brief get the event position

        //edition
        bool IsEditCancelled(void) const;                                   ///< \brief true if edition has been cancelled
        void SetEditCanceled(bool bEditCancelled);                          ///< \brief set if edition has been cancelled

        //mouse event
        void SetMouseEvent(const wxMouseEvent &rEvent);                     ///< \brief set the mouse event
        const wxMouseEvent& GetMouseEvent(void) const;                      ///< \brief get the mouse event
        void SetItemRect(wxRect r);                                         ///< \brief set the item bounding rectangle
        wxRect GetItemRect(void) const;                                     ///< \brief get the item bounding rectangle
        void SetHitTestFlags(wxUint32 uiFlags);                             ///< \brief set the hit test flags
        wxUint32 GetHitTestFlags(void) const;                               ///< \brief get the hit test flags
        void SetRelativePosition(int x, int y);                             ///< \brief set the position relative to top left corner of item rect
        void SetRelativePosition(wxPoint pt);                               ///< \brief set the position relative to top left corner of item rect
        wxPoint GetRelativePosition(void) const;                            ///< \brief get the position relative to top left corner of item rect

        //key event
        void SetKeyEvent(const wxKeyEvent &rEvent);                         ///< \brief set the key event
        const wxKeyEvent& GetKeyEvent(void) const;                          ///< \brief get the key event

    #if wxUSE_DRAG_AND_DROP
        //For drag operations
        void SetDataObject(wxDataObject* pObj);                             ///< \brief set the data object
        wxDataObject* GetDataObject(void) const;                            ///< \brief get the data object

        //For drop operations
        void SetDataFormat(const wxDataFormat &rFormat);                    ///< \brief set the data format
        wxDataFormat GetDataFormat(void) const;                             ///< \brief get the data format
        void SetDataSize(size_t uiSize);                                    ///< \brief set the data size
        size_t GetDataSize(void) const;                                     ///< \brief get the data size
        void SetDataBuffer(void *pBuffer);                                  ///< \brief set the data buffer
        void* GetDataBuffer(void) const;                                    ///< \brief get the data buffer
        void SetDragFlags(int iFlags);                                      ///< \brief set the drag flags
        int GetDragFlags(void) const;                                       ///< \brief get the drag flags
        void SetDropEffect(wxDragResult eEffect);                           ///< \brief set the drop effect
        wxDragResult GetDropEffect(void) const;                             ///< \brief get the drop effect
    #endif // wxUSE_DRAG_AND_DROP

    protected:
        //data
        wxVirtualItemID             m_ItemID;                               ///< \brief item impacted by the event
        size_t                      m_uiField;                              ///< \brief the item field index
        size_t                      m_uiColumn;                             ///< \brief the data view column index
        wxVirtualDataViewColumn*    m_pColumn;                              ///< \brief the data view column
        wxVariant                   m_vValue;                               ///< \brief item value
        wxPoint                     m_ptPos;                                ///< \brief position of the event
        bool                        m_bEditCancelled;                       ///< \brief edit of item has been cancelled
        wxMouseEvent                m_MouseEvent;                           ///< \brief mouse event
        wxRect                      m_ItemRect;                             ///< \brief item bounding rect (for mouse events)
        wxUint32                    m_uiHitTestFlags;                       ///< \brief hit test flags (for mouse events)
        wxPoint                     m_ptRelativePos;                        ///< \brief position relative to item bounding rectangle (for mouse events)
        wxKeyEvent                  m_KeyEvent;                             ///< \brief keyboard event
        wxVirtualItemIDs            m_vDeselectedItems;                     ///< \brief de-selected items
        wxVirtualItemIDs            m_vSelectedItems;                       ///< \brief selected items

    #if wxUSE_DRAG_AND_DROP
        //drag operations
        wxDataObject*               m_pDataObject;                          ///< \brief the data object being dragged
        //drop operations
        wxDataFormat                m_DataFormat;                           ///< \brief format of the data object
        void*                       m_pDataBuffer;                          ///< \brief data buffer
        size_t                      m_uiDataSize;                           ///< \brief size of data in m_pDataBuffer, in bytes
        int                         m_iDragFlags;                           ///< \brief drag flags (wxDrag_CopyOnly, wxDrag_AllowMove, wxDrag_DefaultMove)
        wxDragResult                m_eDropEffect;                          ///< \brief drop effects
    #endif // wxUSE_DRAG_AND_DROP

    private:
        //no copy class
        wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxVirtualDataViewEvent);
};

//------------------- EVENT DECLARATIONS ----------------------------//
#define WX_DECLARE_VDV_EVENT(eEvent)  wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_VDV, eEvent, wxVirtualDataViewEvent);

//selection & activation & focus
WX_DECLARE_VDV_EVENT(wxEVT_VDV_SELECTION_CHANGING);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_SELECTION_CHANGED);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_ACTIVATED);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_GET_FOCUS);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_LOSE_FOCUS);

//item check
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_CHECKING);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_CHECKED);

//expand/collapse
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_EXPANDING);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_COLLAPSING);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_EXPANDED);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_COLLAPSED);

//edition
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_START_EDITING);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_EDITING_STARTED);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_EDITING_DONE);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_VALUE_CHANGING);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_VALUE_CHANGED);

//context menu
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_CONTEXT_MENU);

//column headers
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_LEFT_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_MIDDLE_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_RIGHT_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_LEFT_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_MIDDLE_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_RIGHT_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_LEFT_DCLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_MIDDLE_DCLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_RIGHT_DCLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_SEPARATOR_LEFT_DCLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_SEPARATOR_MIDDLE_DCLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_SEPARATOR_RIGHT_DCLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_SORTED);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_COLUMN_REORDERED);

//drag and drop
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_BEGIN_DRAG);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_DROP_POSSIBLE);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_DROP);

//mouse
WX_DECLARE_VDV_EVENT(wxEVT_VDV_MOUSE_EVENT);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_MOUSE_MOVE_EVENT);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_LEFT_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_LEFT_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_LEFT_DOUBLE_CLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_MIDDLE_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_MIDDLE_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_MIDDLE_DOUBLE_CLICK);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_RIGHT_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_RIGHT_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_ITEM_RIGHT_DOUBLE_CLICK);

//keyboard
WX_DECLARE_VDV_EVENT(wxEVT_VDV_KEYBOARD_EVENT);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_KEY_DOWN);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_KEY_UP);
WX_DECLARE_VDV_EVENT(wxEVT_VDV_CHAR);

#undef WX_DECLARE_VDV_EVENT

//------------------------ EVENT HANDLERS DECLARATION ---------------//
typedef void (wxEvtHandler::*wxVirtualDataViewEventFunction)(wxVirtualDataViewEvent&);
#define wxVirtualDataViewEventHandler(func)  wxEVENT_HANDLER_CAST(wxVirtualDataViewEventFunction, func)

#define wx__DECLARE_VIRTUALDATAVIEWEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_VDV_ ## evt, id, wxDataViewEventHandler(fn))

//selection & activation & focus
#define EVT_VDV_SELECTION_CHANGING(id, fn)      wx__DECLARE_VIRTUALDATAVIEWEVT(SELECTION_CHANGING, id, fn)
#define EVT_VDV_SELECTION_CHANGED(id, fn)       wx__DECLARE_VIRTUALDATAVIEWEVT(SELECTION_CHANGED, id, fn)
#define EVT_VDV_ITEM_ACTIVATED(id, fn)          wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_ACTIVATED, id, fn)
#define EVT_VDV_ITEM_GET_FOCUS(id, fn)          wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_GET_FOCUS, id, fn)
#define EVT_VDV_ITEM_LOSE_FOCUS(id, fn)         wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_LOSE_FOCUS, id, fn)

//expand/collapse
#define EVT_VDV_ITEM_EXPANDING(id, fn)          wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_EXPANDING, id, fn)
#define EVT_VDV_ITEM_COLLAPSING(id, fn)         wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_COLLAPSING, id, fn)
#define EVT_VDV_ITEM_EXPANDED(id, fn)           wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_EXPANDED, id, fn)
#define EVT_VDV_ITEM_COLLAPSED(id, fn)          wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_COLLAPSED, id, fn)

//edition
#define EVT_VDV_ITEM_START_EDITING(id, fn)      wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_START_EDITING, id, fn)
#define EVT_VDV_ITEM_EDITING_STARTED(id, fn)    wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_EDITING_STARTED, id, fn)
#define EVT_VDV_ITEM_EDITING_DONE(id, fn)       wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_EDITING_DONE, id, fn)
#define EVT_VDV_ITEM_VALUE_CHANGED(id, fn)      wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_VALUE_CHANGED, id, fn)

//context menu
#define EVT_VDV_ITEM_CONTEXT_MENU(id, fn)       wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_CONTEXT_MENU, id, fn)

//column headers
#define EVT_VDV_COLUMN_HEADER_LEFT_CLICK(id, fn) \
    wx__DECLARE_VIRTUALDATAVIEWEVT(COLUMN_HEADER_LEFT_CLICK, id, fn)
#define EVT_VDV_COLUMN_HEADER_RIGHT_CLICK(id, fn) \
    wx__DECLARE_VIRTUALDATAVIEWEVT(COLUMN_HEADER_RIGHT_CLICK, id, fn)
#define EVT_VDV_COLUMN_SORTED(id, fn)           wx__DECLARE_VIRTUALDATAVIEWEVT(COLUMN_SORTED, id, fn)
#define EVT_VDV_COLUMN_REORDERED(id, fn)        wx__DECLARE_VIRTUALDATAVIEWEVT(COLUMN_REORDERED, id, fn)

//drag and drop
#define EVT_VDV_ITEM_BEGIN_DRAG(id, fn)         wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_BEGIN_DRAG, id, fn)
#define EVT_VDV_ITEM_DROP_POSSIBLE(id, fn)      wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_DROP_POSSIBLE, id, fn)
#define EVT_VDV_ITEM_DROP(id, fn)               wx__DECLARE_VIRTUALDATAVIEWEVT(ITEM_DROP, id, fn)

//mouse
#define EVT_VDV_MOUSE_EVENT(id, fn)             wx__DECLARE_VIRTUALDATAVIEWEVT(MOUSE_EVENT, id, fn)

//keyboard
#define EVT_VDV_(id, fn)                        wx__DECLARE_VIRTUALDATAVIEWEVT(KEYBOARD_EVENT, id, fn)


#endif //_WX_VIRTUAL_VIEW_EVENT_H_

