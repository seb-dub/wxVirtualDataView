/**********************************************************************/
/** FILE    : VirtualHeaderCtrlEvent.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : generic header control with native look & feel         **/
/**********************************************************************/


#include <wx/VirtualDataView/Header/VirtualHeaderCtrlEvent.h>

//---------------------- EVENT IMPLEMENTATIONS ----------------------//
#define wxVIRTUAL_HEADER_DEFINE_EVENT(id)               \
    wxDEFINE_EVENT(id, wxVirtualHeaderCtrlEvent);

//mouse move / hot tracking
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_CHANGE);

//mouse buttons
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_LEFT_DOWN);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_LEFT_UP);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_MIDDLE_DOWN);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_MIDDLE_UP);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_RIGHT_DOWN);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_RIGHT_UP);

//mouse double clicks
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_LEFT_DCLICK);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_MIDDLE_DCLICK);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_RIGHT_DCLICK);

wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_SEPARATOR_LEFT_DCLICK);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_SEPARATOR_MIDDLE_DCLICK);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_SEPARATOR_RIGHT_DCLICK);

//resizing
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_BEGIN_RESIZE);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_RESIZING);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_END_RESIZE);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_RESIZING_CANCELLED);

//reordering
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_BEGIN_DRAGGING);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_DRAGGING);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_END_DRAGGING);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_DRAGGING_CANCELLED);

//filtering
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_SHOW_FILTERS);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_HIDE_FILTERS);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_BEGIN_FILTER_EDIT);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_END_FILTER_EDIT);
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_FILTER_CHANGED);

//layout
wxVIRTUAL_HEADER_DEFINE_EVENT(wxEVT_VIRTUAL_HEADER_LAYOUT_CHANGED);


#undef wxVIRTUAL_HEADER_DEFINE_EVENT

//-------------------- CONSTRUCTORS & DESTRUCTOR --------------------//
wxIMPLEMENT_DYNAMIC_CLASS(wxVirtualHeaderCtrlEvent, wxNotifyEvent);

/** Default constructor
  * \param eCommandType [input]: the event type
  * \param iWindowID    [input]: the ID of the window generating the event
  */
wxVirtualHeaderCtrlEvent::wxVirtualHeaderCtrlEvent(wxEventType eCommandType,
                                                  int iWindowID)
    : wxNotifyEvent(eCommandType, iWindowID),
      m_uiItemID(size_t(-1)),
      m_iSize(0),
      m_uiNewOrder(size_t(-1))
{
}

/** Copy constructor
  * \param rhs [input] : the object to copy
  */
wxVirtualHeaderCtrlEvent::wxVirtualHeaderCtrlEvent(const wxVirtualHeaderCtrlEvent &rhs)
    : wxNotifyEvent(rhs),
      m_uiItemID(rhs.m_uiItemID),
      m_iSize(rhs.m_iSize),
      m_uiNewOrder(rhs.m_uiNewOrder)
{
}

/** Destructor
  */
wxVirtualHeaderCtrlEvent::~wxVirtualHeaderCtrlEvent(void)
{
}

//----------- WXEVENT INTERFACE IMPLEMENTATION ----------------------//
/** Clone the event
  * \return a copy of this event, allocated with "new"
  */
wxEvent* wxVirtualHeaderCtrlEvent::Clone(void) const wxOVERRIDE
{
    return(new wxVirtualHeaderCtrlEvent(*this));
}

//----------------- ITEM ID -----------------------------------------//
/** Get the item ID
  * \return the Item ID
  */
size_t wxVirtualHeaderCtrlEvent::GetItemID(void) const
{
    return(m_uiItemID);
}

/** Set the item ID
  * \param uiItemID [input]: the new item ID
  */
void wxVirtualHeaderCtrlEvent::SetItemID(size_t uiItemID)
{
    m_uiItemID = uiItemID;
}

//----------- SIZE AFTER RESIZING EVENT -----------------------------//
/** Get the new size for sizing events
  * \return the size of the row/column after a sizing event
  */
int wxVirtualHeaderCtrlEvent::GetSize(void) const
{
    return(m_iSize);
}

/** Set the new size for sizing events
  * \param iNewSize [input]: the new size
  */
void wxVirtualHeaderCtrlEvent::SetSize(int iNewSize)
{
    m_iSize = iNewSize;
}

//-------- NEW INDEX FOR RE-ORDERING EVENT --------------------------//
/** Get the new index after a re-ordering event
  * \return the new index after a re-ordering event
  */
size_t wxVirtualHeaderCtrlEvent::GetNewOrder(void) const
{
    return(m_uiNewOrder);
}

/** Set the new index after a re-ordering event
  * \param uiNewOrder [input]: the new index of the row/column
  */
void wxVirtualHeaderCtrlEvent::SetNewOrder(size_t uiNewOrder)
{
    m_uiNewOrder = uiNewOrder;
}


