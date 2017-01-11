/**********************************************************************/
/** FILE    : VirtualIModelRenderer.h                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a model renderer                          **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_MODEL_RENDERERER_H_
#define WX_VIRTUAL_I_MODEL_RENDERERER_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/event.h>
#include <wx/dc.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewBase;
class WXDLLIMPEXP_VDV wxVirtualDataViewCtrl;

/** \class wxVirtualIModelRenderer : a class for dealing with high-level model rendering
  * As a consequence, it also has to deal with hit test, sizing and scrolling
  */
class WXDLLIMPEXP_VDV wxVirtualIModelRenderer
{
    public:
        //enums
        enum EHitTestFlags
        {
            E_HIT_TEST_ON_EXPANDER      = 0x00000001,                   ///< \brief point is on expander
            E_HIT_TEST_ON_CHECKBOX      = 0x00000002,                   ///< \brief point is on checkbox
            E_HIT_TEST_ON_IMAGE         = 0x00000004,                   ///< \brief point is on image
            E_HIT_TEST_ON_TOP_MARGIN    = 0x00000008,                   ///< \brief point is inside the top margin of the item
            E_HIT_TEST_ON_BOTTOM_MARGIN = 0x00000010,                   ///< \brief point is inside the bottom margin of the item
            E_HIT_TEST_ON_LEFT_MARGIN   = 0x00000020,                   ///< \brief point is inside the left margin of the item
            E_HIT_TEST_ON_RIGHT_MARGIN  = 0x00000040,                   ///< \brief point is inside the right margin of the item
            E_HIT_TEST_ON_TOP_EDGE      = 0x00000080,                   ///< \brief point is on the top edge of the item
            E_HIT_TEST_ON_BOTTOM_EDGE   = 0x00000100,                   ///< \brief point is on the bottom edge of the item
            E_HIT_TEST_ON_LEFT_EDGE     = 0x00000200,                   ///< \brief point is on the left edge of the item
            E_HIT_TEST_ON_RIGHT_EDGE    = 0x00000400,                   ///< \brief point is on the right edge of the item
            E_HIT_TEST_OUTSIDE_ITEM     = 0x00000800,                   ///< \brief point is outside the item. In that case, the nearest item is returned
            E_HIT_TEST_ON_ITEM          = 0x00000000                    ///< \brief point is on the item directly
        };

        /// \enum EVisibilityFlags : where the item should be visible
        enum EVisibilityFlags
        {
            WX_VISIBLE_DONT_CARE    = 0,                                ///< \brief as long as the item is visible, nothing is done
            WX_VISIBLE_AT_TOP       = 1,                                ///< \brief the item must be the 1st visible
            WX_VISIBLE_AT_BOTTOM    = 2                                 ///< \brief the item must be the last visible
        };

        //structures
        struct THitTest
        {
            wxUint32        m_uiFlags;                                  ///< \brief additional flags of the hit test
            wxCoord         m_x;                                        ///< \brief the client coordinate X of the original point
            wxCoord         m_y;                                        ///< \brief the client coordinate Y of the original point
            wxVirtualItemID m_ID;                                       ///< \brief the item hit
            wxUint32        m_uiField;                                  ///< \brief field index of the item hit (model coord)
            wxUint32        m_uiColIndex;                               ///< \brief column index of the item hit (view coord)
            wxRect          m_RectItem;                                 ///< \brief bounding rectangle of the item hit (client coordinates)
            wxRect          m_RectSubItem;                              ///< \brief bounding rectangle of the sub-item hit (client coordinates)
            wxUint32        m_uiRank;                                   ///< \brief rank of the item
            wxCoord         m_RelativeX;                                ///< \brief the X coordinate of the point, relative to the hit sub-item rect top left corner
            wxCoord         m_RelativeY;                                ///< \brief the Y coordinate of the point, relative to the hit sub-item rect top left corner
        };

        //constructors & destructor
        wxVirtualIModelRenderer(wxVirtualDataViewBase* pClient,
                                wxVirtualDataViewCtrl* pScroller);      ///< \brief constructor
        virtual ~wxVirtualIModelRenderer(void);                         ///< \brief destructor

        //client
        wxVirtualDataViewBase* GetClientWindow(void) const;             ///< \brief get the client window
        wxVirtualDataViewCtrl* GetControlWindow(void) const;            ///< \brief get the controlling window
        virtual void SetClientWindow(wxVirtualDataViewBase *pClient);   ///< \brief set the client window

        //window with scrollbars
        wxWindow*  GetScrollerWindow(void);                             ///< \brief get the window with the scrollbars
        void    SetScrollerWindow(wxWindow* pScroller);                 ///< \brief set the window with the scrollbars

        //interface - event handling
        virtual bool PaintModel(wxDC &rDC)                        = 0;  ///< \brief paint the model
        virtual bool EraseBackgroundEvent(wxEraseEvent &rEvent)   = 0;  ///< \brief erase background event
        virtual bool SizeEvent(wxSizeEvent &rEvent)               = 0;  ///< \brief a size event occurred
        virtual bool FocusEvent(wxFocusEvent &rEvent)             = 0;  ///< \brief a focus event occurred
        virtual bool ScrollEvent(wxScrollWinEvent &rEvent)        = 0;  ///< \brief a scroll event occurred
        virtual bool MouseEvent(wxMouseEvent &rEvent)             = 0;  ///< \brief a mouse event occurred
        virtual bool KeyboardEvent(wxKeyEvent &rEvent)            = 0;  ///< \brief a keyboard event occurred
        virtual bool ContextMenuEvent(wxContextMenuEvent &rEvent) = 0;  ///< \brief a context menu event occurred
        virtual bool HelpEvent(wxHelpEvent &rEvent)               = 0;  ///< \brief a help event occurred
        virtual bool StartEdition(const wxVirtualItemID &rID,
                                  size_t uiCol)                   = 0;  ///< \brief start the item edition
        virtual bool FinishEdition(bool bCancelled)               = 0;  ///< \brief provokes the end of the current edition

        //interface - sizing
        virtual wxSize GetBestSize(void) const                    = 0;  ///< \brief get the best size
        virtual wxSize GetBestSize(size_t uiColID) const;               ///< \brief get the best size of a column
        virtual bool HitTest(THitTest &rResult,
                             wxCoord x, wxCoord y,
                             bool bClientAreaOnly = true)         = 0;  ///< \brief performs a hit test
        virtual bool GetItemRect(wxRect &rRect,
                                 const wxVirtualItemID &rID,
                                 size_t uiField,
                                 bool bClientCoordinates = true)  = 0;  ///< \brief get the rectangle of an item
        virtual bool Expand(const wxVirtualItemID &rID,
                            bool bRefresh = true,
                            bool bRecursive = false,
                            bool bUpdateScrollbars = true,
                            bool bSendEvent = true)               = 0;  ///< \brief expand an item
        virtual bool Collapse(const wxVirtualItemID &rID,
                              bool bRefresh = true,
                              bool bRecursive = false,
                              bool bUpdateScrollbars = true,
                            bool bSendEvent = true)               = 0;  ///< \brief collapse an item

        //interface - misc
        virtual void Refresh(void);                                     ///< \brief refresh the display//focused column
        virtual int GetFocusedField(void) const                   = 0;  ///< \brief get focused column ID
        virtual void SetFocusedField(int iField)                  = 0;  ///< \brief set focused column ID
        virtual void UpdateScrollbars(void)                       = 0;  ///< \brief update the scrollbar for a given client size

        //interface - visibility
        virtual bool IsItemVisible(const wxVirtualItemID &rID,
                                   size_t uiCol = 0,
                                   bool bCheckField = true) const = 0;  ///< \brief check if the item is visible

        virtual bool MakeItemVisible(const wxVirtualItemID &rID,
                                     size_t uiCol,
                                     EVisibilityFlags eFlag)      = 0;  ///< \brief make the item visible (without expanding)
        virtual bool EnsureItemVisible(const wxVirtualItemID &rID,
                                      size_t uiCol,
                EVisibilityFlags eFlag = WX_VISIBLE_DONT_CARE)    = 0;  ///< \brief ensure the item is visible (with expanding when needed)
        virtual wxVirtualItemID GetFirstVisibleItem(void) const;        ///< \brief get the 1st visible item
        virtual wxVirtualItemID GetNextVisibleItem(
                                    const wxVirtualItemID &rID) const;  ///< \brief get the next visible item
        virtual wxVirtualItemID GetPrevVisibleItem(
                                    const wxVirtualItemID &rID) const;  ///< \brief get the previous visible item
        virtual size_t GetAllVisibleItems(wxVirtualItemIDs &vIDs) const;///< \brief get all visible items

        //interface - callbacks
        virtual void OnDataModelChanged(void)                     = 0;  ///< \brief called by the client to notify that the data model has changed
        virtual void OnLayoutChanged(void)                        = 0;  ///< \brief called by the client to notify changes in layout

    protected:
        //data
        wxVirtualDataViewBase*    m_pClient;                            ///< \brief the client window
        wxWindow*                 m_pScroller;                          ///< \brief the window with the scrollbars
};

#endif

