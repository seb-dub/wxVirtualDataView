/**********************************************************************/
/** FILE    : VirtualDataViewEvent.cpp                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : an event class for wxVirtualDataViewCtrl               **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/DataView/VirtualDataViewEvent.h>

//----------------- MACRO IMPLEMENTATIONS ---------------------------//
wxIMPLEMENT_DYNAMIC_CLASS(wxVirtualDataViewEvent, wxNotifyEvent);

#define WX_DEFINE_VDV_EVENT(evt) wxDEFINE_EVENT(evt, wxVirtualDataViewEvent);

//selection & activation & focus
WX_DEFINE_VDV_EVENT(wxEVT_VDV_SELECTION_CHANGING);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_SELECTION_CHANGED);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_ACTIVATED);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_GET_FOCUS);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_LOSE_FOCUS);

//item check
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_CHECKING);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_CHECKED);

//expand/collapse
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_EXPANDING);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_COLLAPSING);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_EXPANDED);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_COLLAPSED);

//edition
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_START_EDITING);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_EDITING_STARTED);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_EDITING_DONE);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_VALUE_CHANGING);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_VALUE_CHANGED);

//context menu
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_CONTEXT_MENU);

//column headers
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_LEFT_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_MIDDLE_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_RIGHT_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_LEFT_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_MIDDLE_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_RIGHT_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_LEFT_DCLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_MIDDLE_DCLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_RIGHT_DCLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_SEPARATOR_LEFT_DCLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_SEPARATOR_MIDDLE_DCLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_HEADER_SEPARATOR_RIGHT_DCLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_SORTED);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_COLUMN_REORDERED);

//drag and drop
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_BEGIN_DRAG);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_DROP_POSSIBLE);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_DROP);

//mouse
WX_DEFINE_VDV_EVENT(wxEVT_VDV_MOUSE_EVENT);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_MOUSE_MOVE_EVENT);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_LEFT_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_LEFT_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_LEFT_DOUBLE_CLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_MIDDLE_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_MIDDLE_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_MIDDLE_DOUBLE_CLICK);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_RIGHT_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_RIGHT_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_ITEM_RIGHT_DOUBLE_CLICK);

//keyboard
WX_DEFINE_VDV_EVENT(wxEVT_VDV_KEYBOARD_EVENT);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_KEY_DOWN);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_KEY_UP);
WX_DEFINE_VDV_EVENT(wxEVT_VDV_CHAR);

#undef WX_DEFINE_VDV_EVENT

//-------------- CONSTRUCTORS & DESTRUCTOR --------------------------//
/** Default constructor
  * \param eType  [input]: the event type
  * \param iWinID [input]: the ID of the window emitting the event
  */
wxVirtualDataViewEvent::wxVirtualDataViewEvent(wxEventType eType,
                                               int iWinID)
    : wxNotifyEvent(eType, iWinID),
      m_ItemID(),
      m_uiField(0),
      m_uiColumn(0),
      m_pColumn(WX_VDV_NULL_PTR),
      m_vValue(wxNullVariant),
      m_ptPos(-1, -1),
      m_bEditCancelled(false),
      m_MouseEvent(),
      m_ItemRect(0, 0, -1, -1),
      m_uiHitTestFlags(0),
      m_ptRelativePos(-1, -1),
      m_KeyEvent(),
      m_vDeselectedItems(),
      m_vSelectedItems()
#if wxUSE_DRAG_AND_DROP
        , m_pDataObject(WX_VDV_NULL_PTR),
        m_DataFormat(),
        m_pDataBuffer(WX_VDV_NULL_PTR),
        m_uiDataSize(0),
        m_iDragFlags(wxDrag_CopyOnly),
        m_eDropEffect(wxDragNone)
#endif
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewEvent::wxVirtualDataViewEvent(const wxVirtualDataViewEvent &rhs)
    : wxNotifyEvent(rhs),
      m_ItemID(rhs.m_ItemID),
      m_uiField(rhs.m_uiField),
      m_uiColumn(rhs.m_uiColumn),
      m_pColumn(rhs.m_pColumn),
      m_vValue(rhs.m_vValue),
      m_ptPos(rhs.m_ptPos),
      m_bEditCancelled(rhs.m_bEditCancelled),
      m_MouseEvent(rhs.m_MouseEvent),
      m_ItemRect(rhs.m_ItemRect),
      m_uiHitTestFlags(rhs.m_uiHitTestFlags),
      m_ptRelativePos(rhs.m_ptRelativePos),
      m_KeyEvent(rhs.m_KeyEvent),
      m_vDeselectedItems(rhs.m_vDeselectedItems),
      m_vSelectedItems(rhs.m_vSelectedItems)
#if wxUSE_DRAG_AND_DROP
        , m_pDataObject(rhs.m_pDataObject),
        m_DataFormat(rhs.m_DataFormat),
        m_pDataBuffer(rhs.m_pDataBuffer),
        m_uiDataSize(rhs.m_uiDataSize),
        m_iDragFlags(rhs.m_iDragFlags),
        m_eDropEffect(rhs.m_eDropEffect)
#endif
{
}

/** Destructor
  */
wxVirtualDataViewEvent::~wxVirtualDataViewEvent(void)
{
}

//----------------------- wxEVENT INTERFACE IMPLEMENTATION ----------//
/** Clone the event
  */
wxEvent* wxVirtualDataViewEvent::Clone(void) const
{
    return(new wxVirtualDataViewEvent(*this));
}

//--------------------------- ITEM ----------------------------------//
/** Set the item impacted by the event
  * Not valid for wxEVT_VDV_SELECTION_CHANGING or wxEVT_VDV_SELECTION_CHANGED
  * \param id [input]: the item ID
  */
void wxVirtualDataViewEvent::SetItem(wxVirtualItemID id)
{
    m_ItemID = id;
}

/** Get the item impacted by the event.
  * Not valid for wxEVT_VDV_SELECTION_CHANGING or wxEVT_VDV_SELECTION_CHANGED
  * \return the item ID
  */
wxVirtualItemID wxVirtualDataViewEvent::GetItem(void) const
{
    return(m_ItemID);
}

/** Set the item field index
  * \param uiField [input]: the new field index (model coord)
  */
void wxVirtualDataViewEvent::SetFieldIndex(size_t uiField)
{
    m_uiField = uiField;
}

/** Get the item field index
  * \return the field index (model coord)
  */
size_t wxVirtualDataViewEvent::GetFieldIndex(void) const
{
    return(m_uiField);
}


//-------------- SELECTION CHANGED / CHANGING -----------------------//
/** Set the de-selected items (copy performed).
  * Valid only for wxEVT_VDV_SELECTION_CHANGING and wxEVT_VDV_SELECTION_CHANGED
  * \param vIDs [input]: the list of de-selected items. A copy is performed
  */
void wxVirtualDataViewEvent::SetDeselectedItems(const wxVirtualItemIDs &vIDs)
{
    m_vDeselectedItems = vIDs;
}

/** Set the de-selected items (move performed)
  * Valid only for wxEVT_VDV_SELECTION_CHANGING and wxEVT_VDV_SELECTION_CHANGED
  * \param vIDs [input]: the list of de-selected items. A swap is performed
  */
void wxVirtualDataViewEvent::AssignDeselectedItems(wxVirtualItemIDs &vIDs)
{
    m_vDeselectedItems.swap(vIDs);
}

/** Get the de-selected items
  * Valid only for wxEVT_VDV_SELECTION_CHANGING and wxEVT_VDV_SELECTION_CHANGED
  * \return the list of de-selected items
  */
const wxVirtualItemIDs& wxVirtualDataViewEvent::GetDeselectedItems(void) const
{
    return(m_vDeselectedItems);
}

/** Set the selected items (copy performed).
  * Valid only for wxEVT_VDV_SELECTION_CHANGING and wxEVT_VDV_SELECTION_CHANGED
  * \param vIDs [input]: the list of selected items. A copy is performed
  */
void wxVirtualDataViewEvent::SetSelectedItems(const wxVirtualItemIDs &vIDs)
{
    m_vSelectedItems = vIDs;
}

/** Set the selected items (move performed)
  * Valid only for wxEVT_VDV_SELECTION_CHANGING and wxEVT_VDV_SELECTION_CHANGED
  * \param vIDs [input]: the list of selected items. A swap is performed
  */
void wxVirtualDataViewEvent::AssignSelectedItems(wxVirtualItemIDs &vIDs)
{
    m_vSelectedItems.swap(vIDs);
}

/** Get the selected items
  * Valid only for wxEVT_VDV_SELECTION_CHANGING and wxEVT_VDV_SELECTION_CHANGED
  * \return the list of selected items
  */
const wxVirtualItemIDs& wxVirtualDataViewEvent::GetSelectedItems(void) const
{
    return(m_vSelectedItems);
}

//--------------------------- COLUMN --------------------------------//
/** Set the view column
  * \param uiViewColumn [input]: index of the view column (not model column)
  */
void wxVirtualDataViewEvent::SetColumnIndex(size_t uiViewColumn)
{
    m_uiColumn = uiViewColumn;
}

/** Get the column index. Valid for all events.
  * \return the view column index (not model column index)
  */
size_t wxVirtualDataViewEvent::GetColumnIndex(void) const
{
    return(m_uiColumn);
}

/** Set the column
  * \param pCol [input]: the column object. Ownership is not taken
  */
void wxVirtualDataViewEvent::SetColumn(wxVirtualDataViewColumn *pCol)
{
    m_pColumn = pCol;
}

/** Get the column. Valid for wxEVT_VDV_COLUMN_HEADER_CLICK.
  * \return the column object. Object is not owned
  */
wxVirtualDataViewColumn* wxVirtualDataViewEvent::GetColumn(void) const
{
    return(m_pColumn);
}

//---------------------- VALUE --------------------------------------//
/** Set the value of the item
  * \param rValue [input]: the value of the item
  */
void wxVirtualDataViewEvent::SetValue(const wxVariant &rValue)
{
    m_vValue = rValue;
}

/** Get the value of the item. Valid for all events.
  * \return the value of the item.
  */
const wxVariant& wxVirtualDataViewEvent::GetValue(void) const
{
    return(m_vValue);
}

//------------------------- POSITION --------------------------------//
/** Set the event position
  * \param x [input]: the x position
  * \param y [input]: the y position
  */
void wxVirtualDataViewEvent::SetPosition(int x, int y)
{
    m_ptPos.x = x;
    m_ptPos.y = y;
}

/** Set the event position
  * \param pt [input]: the position
  */
void wxVirtualDataViewEvent::SetPosition(wxPoint pt)
{
    m_ptPos = pt;
}

/** Get the event position
  * \return the event position
  */
wxPoint wxVirtualDataViewEvent::GetPosition(void) const
{
    return(m_ptPos);
}

//---------------------- EDITION ------------------------------------//
/** True if edition has been cancelled
  * \return true if edition has been cancelled
  */
bool wxVirtualDataViewEvent::IsEditCancelled(void) const
{
    return(m_bEditCancelled);
}

/** Set if edition has been cancelled
  * \param bEditCancelled [input]: true if the edition has been cancelled
  *                                false if the edition has been allowed
  */
void wxVirtualDataViewEvent::SetEditCanceled(bool bEditCancelled)
{
    m_bEditCancelled = bEditCancelled;
}

//--------------------- MOUSE EVENT ---------------------------------//
/** Set the mouse event
  * \param rEvent [input]: the event to set
  */
void wxVirtualDataViewEvent::SetMouseEvent(const wxMouseEvent &rEvent)
{
    m_MouseEvent = rEvent;

    //convert the mouse event
    //button down
         if (rEvent.LeftDown())     SetEventType(wxEVT_VDV_ITEM_LEFT_DOWN);
    else if (rEvent.MiddleDown())   SetEventType(wxEVT_VDV_ITEM_MIDDLE_DOWN);
    else if (rEvent.RightDown())    SetEventType(wxEVT_VDV_ITEM_RIGHT_DOWN);
    //button up
    else if (rEvent.LeftUp())       SetEventType(wxEVT_VDV_ITEM_LEFT_UP);
    else if (rEvent.MiddleUp())     SetEventType(wxEVT_VDV_ITEM_MIDDLE_UP);
    else if (rEvent.RightUp())      SetEventType(wxEVT_VDV_ITEM_RIGHT_UP);
    //button double-click
    else if (rEvent.LeftDClick())   SetEventType(wxEVT_VDV_ITEM_LEFT_DOUBLE_CLICK);
    else if (rEvent.MiddleDClick()) SetEventType(wxEVT_VDV_ITEM_MIDDLE_DOUBLE_CLICK);
    else if (rEvent.RightDClick())  SetEventType(wxEVT_VDV_ITEM_RIGHT_DOUBLE_CLICK);
    //mouse move
    if ((!rEvent.Dragging()) && (rEvent.GetButton() == wxMOUSE_BTN_NONE))
    {
        SetEventType(wxEVT_VDV_ITEM_MOUSE_MOVE_EVENT);
    }
    //other
    else                            SetEventType(wxEVT_VDV_MOUSE_EVENT);

}

/** Get the mouse event
  * \return the mouse event.
  *         Do not delete it: it still belongs to the wxVirtualDataViewEvent object
  */
const wxMouseEvent& wxVirtualDataViewEvent::GetMouseEvent(void) const
{
    return(m_MouseEvent);
}

/** Set the item bounding rectangle (for mouse events)
  * \param r [input]: the item bounding rectangle
  */
void wxVirtualDataViewEvent::SetItemRect(wxRect r)
{
    m_ItemRect = r;
}

/** Get the item bounding rectangle (for mouse events)
  * \return the item bounding rectangle
  */
wxRect wxVirtualDataViewEvent::GetItemRect(void) const
{
    return(m_ItemRect);
}

/** Set the hit test flags (for mouse events)
  * \param uiFlags [input]: the hit test flags. Combination of EHitTestFlags
  */
void wxVirtualDataViewEvent::SetHitTestFlags(wxUint32 uiFlags)
{
    m_uiHitTestFlags = uiFlags;
}

/** Get the hit test flags (for mouse events)
  * \return the hit test flags. Combination of EHitTestFlags
  */
wxUint32 wxVirtualDataViewEvent::GetHitTestFlags(void) const
{
    return(m_uiHitTestFlags);
}

/** Set the position relative to top left corner of item rect (for mouse events)
  * \param x [input]: the relative X coordinate
  * \param y [input]: the relative Y coordinate
  */
void wxVirtualDataViewEvent::SetRelativePosition(int x, int y)
{
    m_ptRelativePos.x = x;
    m_ptRelativePos.y = y;
}

/** Set the position relative to top left corner of item rect (for mouse events)
  * \param pt [input]: the relative position
  */
void wxVirtualDataViewEvent::SetRelativePosition(wxPoint pt)
{
    m_ptRelativePos = pt;
}

/** Get the position relative to top left corner of item rect (for mouse events)
  * \return the relative position
  */
wxPoint wxVirtualDataViewEvent::GetRelativePosition(void) const
{
    return(m_ptRelativePos);
}

//----------------------- KEY EVENT ---------------------------------//
/** Set the key event
  * \param rEvent [input]: the event to set
  */
void wxVirtualDataViewEvent::SetKeyEvent(const wxKeyEvent &rEvent)
{
    m_KeyEvent = rEvent;

    if (m_KeyEvent.GetEventType() == wxEVT_KEY_DOWN)
        SetEventType(wxEVT_VDV_KEY_DOWN);
    else if (m_KeyEvent.GetEventType() == wxEVT_KEY_UP)
        SetEventType(wxEVT_VDV_KEY_UP);
    else if (m_KeyEvent.GetEventType() == wxEVT_CHAR)
        SetEventType(wxEVT_VDV_CHAR);
}

/** Get the key event
  * \return the key event.
  *         Do not delete it: it still belongs to the wxVirtualDataViewEvent object
  */
const wxKeyEvent& wxVirtualDataViewEvent::GetKeyEvent(void) const
{
    return(m_KeyEvent);
}

//-------------------- DRAG OPERATIONS ------------------------------//
#if wxUSE_DRAG_AND_DROP
/** Set the data object
  * \param pObj [input]: the data object. Ownership is not taken
  */
void wxVirtualDataViewEvent::SetDataObject(wxDataObject* pObj)
{
    m_pDataObject = pObj;
}

/** Get the data object
  * \return the data object. The object is not owned by the event class
  */
wxDataObject* wxVirtualDataViewEvent::GetDataObject(void) const
{
    return(m_pDataObject);
}

//--------------------- DROP OPERATIONS -----------------------------//
/** Set the data format
  * \param rFormat [input]:  the data format
  */
void wxVirtualDataViewEvent::SetDataFormat(const wxDataFormat &rFormat)
{
    m_DataFormat = rFormat;
}

/** Get the data format
  * \return the data format
  */
wxDataFormat wxVirtualDataViewEvent::GetDataFormat(void) const
{
    return(m_DataFormat);
}

/** Set the data size
  * \param uiSize [input]: the new data size in bytes
  */
void wxVirtualDataViewEvent::SetDataSize(size_t uiSize)
{
    m_uiDataSize = uiSize;
}

/** Get the data size
  * \return the data size, in bytes
  */
size_t wxVirtualDataViewEvent::GetDataSize(void) const
{
    return(m_uiDataSize);
}

/** Set the data buffer
  * \param pBuffer [input]: the new data buffer. Ownership is not taken
  */
void wxVirtualDataViewEvent::SetDataBuffer(void *pBuffer)
{
    m_pDataBuffer = pBuffer;
}

/** Get the data buffer
  * \return the data buffer. The buffer is not owned by the object
  */
void* wxVirtualDataViewEvent::GetDataBuffer(void) const
{
    return(m_pDataBuffer);
}

/** Set the drag flags
  * \param iFlags [input]: the new drag flags. Can be:
  *                         wxDrag_CopyOnly
  *                         wxDrag_AllowMove
  *                         wxDrag_DefaultMove
  */
void wxVirtualDataViewEvent::SetDragFlags(int iFlags)
{
    m_iDragFlags = iFlags;
}

/** Get the drag flags.
  * \return the drag flags. Can be:
  *                         wxDrag_CopyOnly
  *                         wxDrag_AllowMove
  *                         wxDrag_DefaultMove
  */
int wxVirtualDataViewEvent::GetDragFlags(void) const
{
    return(m_iDragFlags);
}

/** Set the drop effect
  * \param eEffect [input]: the new drop effect
  */
void wxVirtualDataViewEvent::SetDropEffect(wxDragResult eEffect)
{
    m_eDropEffect = eEffect;
}

/** Get the drop effect
  * \return the drop effect
  */
wxDragResult wxVirtualDataViewEvent::GetDropEffect(void) const
{
    return(m_eDropEffect);
}

#endif //wxUSE_DRAG_AND_DROP
