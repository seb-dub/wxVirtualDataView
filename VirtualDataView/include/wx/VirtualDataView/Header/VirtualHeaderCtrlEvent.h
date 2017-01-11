/**********************************************************************/
/** FILE    : VirtualHeaderCtrlEvent.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : generic header control with native look & feel         **/
/**********************************************************************/


#ifndef _VIRTUAL_HEADER_CTRL_EVENT_H_
#define _VIRTUAL_HEADER_CTRL_EVENT_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/event.h>

/** \class wxVirtualHeaderCtrlEvent : event class for wxVirtualHeaderCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualHeaderCtrlEvent : public wxNotifyEvent
{
    public:
        //constructors & destructor
        wxVirtualHeaderCtrlEvent(wxEventType eCommandType = wxEVT_NULL,
                                 int iWindowID = 0);                        ///< \brief default constructor
        wxVirtualHeaderCtrlEvent(const wxVirtualHeaderCtrlEvent &rhs);      ///< \brief copy constructor
        virtual ~wxVirtualHeaderCtrlEvent(void);                            ///< \brief destructor

        //wxEvent interface implementation
        virtual wxEvent* Clone(void) const wxOVERRIDE;                      ///< \brief clone the event

        //item ID
        size_t GetItemID(void) const;                                       ///< \brief get the item ID
        void   SetItemID(size_t uiItemID);                                  ///< \brief set the item ID

        //size after resizing event
        int    GetSize(void) const;                                         ///< \brief get the new size for sizing events
        void   SetSize(int iNewSize);                                       ///< \brief set the new size for sizing events

        //new index for re-ordering event
        size_t GetNewOrder(void) const;                                     ///< \brief get the new index after a re-ordering event
        void   SetNewOrder(size_t uiNewOrder);                              ///< \brief set the new index after a re-ordering event

    protected:
        //data
        size_t      m_uiItemID;                                             ///< \brief item ID
        int         m_iSize;                                                ///< \brief new width/height for sizing events
        size_t      m_uiNewOrder;                                           ///< \brief new index for re-ordering event

    private:
        wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxVirtualHeaderCtrlEvent);
};

//event declarations
#define wxVIRTUAL_HEADER_DECLARE_EVENT(id)                                  \
    wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_VDV, id, wxVirtualHeaderCtrlEvent)

//mouse move: the item ID under mouse cursor changed
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_CHANGE);

//mouse buttons
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_LEFT_DOWN);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_LEFT_UP);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_MIDDLE_DOWN);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_MIDDLE_UP);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_RIGHT_DOWN);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_RIGHT_UP);

//mouse double clicks
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_LEFT_DCLICK);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_MIDDLE_DCLICK);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_RIGHT_DCLICK);

wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_SEPARATOR_LEFT_DCLICK);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_SEPARATOR_MIDDLE_DCLICK);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_SEPARATOR_RIGHT_DCLICK);

//resizing
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_BEGIN_RESIZE);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_RESIZING);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_END_RESIZE);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_RESIZING_CANCELLED);

//reordering
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_BEGIN_DRAGGING);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_DRAGGING);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_END_DRAGGING);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_DRAGGING_CANCELLED);

//filtering
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_SHOW_FILTERS);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_HIDE_FILTERS);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_BEGIN_FILTER_EDIT);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_END_FILTER_EDIT);
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_FILTER_CHANGED);

//layout
wxVIRTUAL_HEADER_DECLARE_EVENT(wxEVT_VIRTUAL_HEADER_LAYOUT_CHANGED);

#undef wxVIRTUAL_HEADER_DECLARE_EVENT

//event handler typedef
typedef void (wxEvtHandler::*wxVirtualHeaderCtrlEventFunction)(wxVirtualHeaderCtrlEvent&);

//event macro
#define wxVirtualHeaderCtrlEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxVirtualHeaderCtrlEventFunction, func)

#define wx__DECLARE_VIRTUAL_HEADER_EVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_VIRTUAL_HEADER_ ## evt, id, wxVirtualHeaderCtrlEventHandler(fn))

#define EVT_HEADER_CLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(CLICK, id, fn)
#define EVT_HEADER_RIGHT_CLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(RIGHT_CLICK, id, fn)
#define EVT_HEADER_MIDDLE_CLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(MIDDLE_CLICK, id, fn)

#define EVT_HEADER_DCLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(DCLICK, id, fn)
#define EVT_HEADER_RIGHT_DCLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(RIGHT_DCLICK, id, fn)
#define EVT_HEADER_MIDDLE_DCLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(MIDDLE_DCLICK, id, fn)

#define EVT_HEADER_SEPARATOR_DCLICK(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(SEPARATOR_DCLICK, id, fn)

#define EVT_HEADER_BEGIN_RESIZE(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(BEGIN_RESIZE, id, fn)
#define EVT_HEADER_RESIZING(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(RESIZING, id, fn)
#define EVT_HEADER_END_RESIZE(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(END_RESIZE, id, fn)

#define EVT_HEADER_BEGIN_REORDER(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(BEGIN_REORDER, id, fn)
#define EVT_HEADER_END_REORDER(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(END_REORDER, id, fn)

#define EVT_HEADER_DRAGGING_CANCELLED(id, fn) wx__DECLARE_VIRTUAL_HEADER_EVT(DRAGGING_CANCELLED, id, fn)


#endif

