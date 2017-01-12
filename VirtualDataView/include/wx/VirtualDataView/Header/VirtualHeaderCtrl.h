/**********************************************************************/
/** FILE    : VirtualHeaderCtrl.h                                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : generic header control with native look & feel         **/
/**********************************************************************/


#ifndef _VIRTUAL_HEADER_CTRL_H_
#define _VIRTUAL_HEADER_CTRL_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/VirtualDataView/Header/VirtualHeaderCtrlEvent.h>
#include <wx/control.h>
#include <wx/systhemectrl.h>
#include <wx/renderer.h>
#include <wx/dragimag.h>
#include <wx/vector.h>

class WXDLLIMPEXP_VDV wxVirtualHeaderIRenderer;
class WXDLLIMPEXP_VDV wxVirtualDataViewIFilterEditor;

extern const char wxVirtualHeaderCtrlNameStr[];

/** \class wxVirtualHeaderCtrl : generic header control with native look & feel
  * It can handles a very large amount of items in the header
  *
  * Note: on Windows, there is the WC_HEADER control class. This class is supposedly
  *       able to handle many items, using some virtual mode by setting a HDF_OWNERDRAW flag
  *       in the items. Alas, it still stores a large amount of data, and creating more than
  *       than 10000 items is memory intensive. On top of that, inserting an item must be done
  *       one by one, and each insertion is O(n) in time -> Adding n items is O(n^2), which is
  *       scary when ones wants to display a large amount of rows/columns (probably a linked list
  *       of items is used internally)
  *       This was the case on Windows 7 and Windows 10.
  *
  * For handling many items, a strict decoupling of the header data and the control is needed.
  * The decoupling is done by implementing protected virtual methods.
  * The following methods MUST be implemented:
  *     size_t  GetItemCount(void) const : return the amount of items in the control. Needed by GetBestSize()
  *     wxSize  GetItemSize(size_t uiItemID) const : return the size of an item. Needed by GetBestSize(), painting and mouse events
  *     bool    GetItemData(size_t uiItemID,
  *                         TItemData &rData) const : retrieve specific item data. For painting only
  *     size_t  GetFirstVisibleItem(void) const : find the 1st visible item. For painting and mouse events
  *     size_t  GetNextVisibleItem(size_t uiItemID) const : find the next visible item. Return size_t(-1) when no more items
  *     wxCoord GetFirstItemOrigin(void) const : get the offset of the 1st visible item.
  *                                              This is a horizontal offset in pixels for column header,
  *                                              and a vertical offset in pixels for row header.
  *                                              This is needed for implementing smooth scrolling
  *
  * Items hiding/showing can be done during implementation of GetItemSize: simply return
  * a width < 0 for hidden column, or a height < 0 for hidden rows.
  * It can be also done by skipping the hidden rows/columns in GetFirstVisibleItem() and
  * GetNextVisibleItem()
  *
  * Reordering is supported indirectly :
  *     1 - a first option is that the concrete implementation should map the uiItemID to
  *         the correct model column/row index.
  *         This should be done inside GetItemSize() and GetItemData()
  *         In that case GetFirstVisibleItem() and GetNextVisibleItem() should return 0 to n-1
  *     2 - a second option is to implement the reordering logic inside GetFirstVisibleItem()
  *         and GetNextVisibleItem() : always return the correct mapped item ID
  *
  * Note on scrolling : for avoiding overflow in scroll events with large number of items,
  *                     scrolling is implemented in a row-by-row or column-by-column basis.
  *                     wDC.SetDeviceOrigin is never called, and all painting starts at (0,0)
  *                     If smooth scrolling is needed, GetFirstItemOrigin should return a value != 0
  *
  * Rendering can be customized by several methods:
  *     - by setting a wxVirtualHeaderIRenderer object. This is the recommended method
  *       3 renderers are implemented by default on Windows, 2 on other platforms:
  *         1 - a generic one (use wxRendererNative internally)
  *         2 - a Windows one (use wxThemeEngine internally)
  *         3 - a custom one (use wxDC methods internally)
  *
  *     - by overriding item paint methods: these methods paint one item at the time
  *       This gives more flexibility for painting one item, but top level organization is
  *       still handled by the control.
  *         void PaintBackground(wxDC &rDC, wxRect &rRect)
  *         void PaintHeader(wxDC &rDC, wxRect &rRect, size_t uiViewCoord)
  *         void PaintDragging(wxDC &rDC)
  *
  *     - by overriding the top level paint methods.
  *       This gives the greatest flexibility (fully customizable) but requires more work
  *         void OnDrawBackground(wxEraseEvent &rEvent)
  *         void OnPaint(wxPaintEvent &rEvent)
  *
  * Flags:
  *     WX_VDV_ALLOW_REORDER : allow column or row reordering with dragging
  *     WX_VDV_ALLOW_RESIZE  : allow to resize labels. The resizing is indirect : only an event is sent
  *                            If the GetItemSize() method does not take it into account, then nothing happens
  *     WX_VDV_PERFECT_SIZE_CALCULATION : this is only used in GetBestSize() implementation. Do not use it if
  *                                       you have potentially millions of items
  */
class WXDLLIMPEXP_VDV wxVirtualHeaderCtrl : public wxSystemThemedControl<wxControl>
{
    public:
        //enums
        /// \enum EOrientation : the orientation of the header
        enum EOrientation
        {
            WX_VDV_HORIZONTAL       = 0,                                                ///< \brief horizontal header (column header)
            WX_VDV_COL_HEADER       = WX_VDV_HORIZONTAL,                                ///< \brief horizontal header (column header)
            WX_VDV_VERTICAL         = 1,                                                ///< \brief vertical header (row header)
            WX_VDV_ROW_HEADER       = WX_VDV_VERTICAL                                   ///< \brief vertical header (row header)
        };

        /// \enum EFlags : the available style flags
        enum EFlags
        {
            WX_VDV_ALLOW_REORDER            = 0x01,                                     ///< \brief can reorder labels
            WX_VDV_ALLOW_RESIZE             = 0x02,                                     ///< \brief can resize labels
            WX_VDV_PERFECT_SIZE_CALCULATION = 0x04,                                     ///< \brief force accurate size calculation
            WX_VDV_ALLOW_FILTERING          = 0x08,                                     ///< \brief allow filtering
            WX_VDV_ALLOW_HIDING             = 0x10,                                     ///< \brief can show/hide items
            WX_VDV_DEFAULT_STYLE            = WX_VDV_ALLOW_RESIZE |
                                              WX_VDV_ALLOW_REORDER|
                                              WX_VDV_ALLOW_FILTERING|
                                              WX_VDV_ALLOW_HIDING                       ///< \brief can resize and reorder labels
        };

        /// \enum ESeparator : the separator hit during a hit test
        enum ESeparator
        {
            WX_VDV_NO_SEPARATOR             = 0,                                        ///< \brief hit test on the item
            WX_VDV_LEFT_SEPARATOR           = 1,                                        ///< \brief hit test on the left separator
            WX_VDV_TOP_SEPARATOR            = WX_VDV_LEFT_SEPARATOR,                    ///< \brief hit test on the top separator
            WX_VDV_RIGHT_SEPARATOR          = 2,                                        ///< \brief hit test on the right separator
            WX_VDV_BOTTOM_SEPARATOR         = WX_VDV_RIGHT_SEPARATOR                    ///< \brief hit test on the bottom separator
        };

        //structs
        /// \struct TItemData : the data for one item
        struct TItemData
        {
            //same as wxHeaderButtonParams
            wxColour                m_ArrowColour;                                      ///< \brief colour of the arrow
            wxColour                m_SelectionColour;                                  ///< \brief background colour of the selection
            wxString                m_sLabel;                                           ///< \brief text to be displayed
            wxFont                  m_FontLabel;                                        ///< \brief font to use for the text
            wxColour                m_LabelColour;                                      ///< \brief colour to use for the text
            wxBitmap                m_Bitmap;                                           ///< \brief bitmap to display on the label
            int                     m_iAlignment;                                       ///< \brief content alignment (wxAlignment combination)
            //additional data
            wxHeaderSortIconType    m_eSortIconType;                                    ///< \brief sort arrow to display
            bool                    m_bIsCurrent;                                       ///< \brief true if the item is the current one (clicked/selected)
            bool                    m_bIsHotTracked;                                    ///< \brief true if the item is hot-tracked (mouse hovering on it)
            bool                    m_bIsPushed;                                        ///< \brief true if the item is pushed (mouse button down)
            bool                    m_bIsVisible;                                       ///< \brief true if the item is visible
            bool                    m_bIsFiltering;                                     ///< \brief true if a filter is currently applied on the item
        };

        //constructors & destructor
        wxVirtualHeaderCtrl(void);                                                      ///< \brief default constructor
        wxVirtualHeaderCtrl(wxWindow *pParent,
                                   wxWindowID id         = wxID_ANY,
                                   const wxPoint &ptPos  = wxDefaultPosition,
                                   const wxSize &size    = wxDefaultSize,
                                   long lStyle           = WX_VDV_DEFAULT_STYLE,
                                   const wxString &sName = wxVirtualHeaderCtrlNameStr); ///< \brief constructor
        virtual ~wxVirtualHeaderCtrl(void);                                             ///< \brief destructor

        //orientation
        EOrientation GetOrientation(void) const;                                        ///< \brief get the orientation of the control
        void SetOrientation(EOrientation eOrientation);                                 ///< \brief set the orientation of the control

        //rendering
        void SetRenderer(wxVirtualHeaderIRenderer *pRenderer);                          ///< \brief set the renderer for this header
        wxVirtualHeaderIRenderer*   GetRenderer(void);                                  ///< \brief get the renderer

        //rendering - convenience methods
        void SetNativeRenderer(void);                                                   ///< \brief use a native implementation of wxVirtualHeaderIRenderer
        void SetGenericRenderer(void);                                                  ///< \brief use wxVirtualHeaderRendererGeneric as renderer
        void SetCustomRenderer(void);                                                   ///< \brief use wxVirtualHeaderRendererCustom as renderer

        //callbacks to call when underlying data have changed
        void OnItemsChanged(void);                                                      ///< \brief notify that header items have changed

        //hit test
        size_t HitTest(int x, int y, ESeparator &eSeparator,
                       bool &bOnFilter,
                       bool bOutOfBoundsHit = false) const;                             ///< \brief get item below the client coordinate x, y
        size_t GetItemAtCoord(int iCoord, ESeparator &eSeparator,
                              bool &bOnFilter,
                              bool bOutOfBoundsHit = false) const;                      ///< \brief get item below the client coordinate x or y

        //resize border size
        size_t GetResizeBorderSize(void) const;                                         ///< \brief get the width/height of the sizing border between items
        void   SetResizeBorderSize(size_t uiSizeInPixels);                              ///< \brief set the width/height of the sizing border between items

        //selection
        size_t GetCurrentSelection(void) const;                                         ///< \brief get the current selection
        void   SetCurrentSelection(size_t uiSelection);                                 ///< \brief set the current selection

        //pure interface
        virtual size_t  GetItemCount(void) const                    = 0;                ///< \brief get the amount of items in the header
        virtual wxSize  GetItemSize(size_t uiItemID) const          = 0;                ///< \brief get the size of one item
        virtual bool    GetItemData(size_t uiItemID,
                                    TItemData &rData) const         = 0;                ///< \brief get the item data (label, sorting arrow, styles)
        virtual size_t  GetFirstVisibleItem(void) const             = 0;                ///< \brief get the index of the 1st visible item
        virtual size_t  GetNextVisibleItem(size_t uiItemID) const   = 0;                ///< \brief get the index of the next visible item
        virtual wxCoord GetFirstItemOrigin(void) const              = 0;                ///< \brief get the origin of the 1st visible item (unscrolled coordinate)

        //interface with default implementation
        virtual size_t  GetLastVisibleItem(void) const;                                 ///< \brief get the last visible item
        virtual wxRect  GetItemRect(size_t uiItemID,
                                    bool bVisibleOnly = true) const;                    ///< \brief get the bounding rectangle of one item
        virtual bool    IsEmpty(void) const;                                            ///< \brief check if the control has some rows / columns

        //redraw
        void            UpdateDisplay(wxDC &rDC);                                       ///< \brief redraw the window in a specified DC

        //UI for reordering / visibility
        virtual bool    ShowCustomizeDialog(void);                                      ///< \brief show rearrange dialog
        virtual bool    UpdateOrder(const wxArrayInt &rvOrder);                         ///< \brief update items order
        virtual bool    UpdateVisibility(size_t uiItemID, bool bVisible);               ///< \brief update items visibility
        virtual bool    ShowItemsMenu(const wxPoint& ptPoint, const wxString& sTitle);  ///< \brief show the menu for columns reordering / hiding / showing
        virtual bool    IsItemShown(size_t uiItemID) const;                             ///< \brief check if an item is shown

        //filtering
        bool AreFiltersShown(void) const;                                               ///< \brief return true if filters are shown
        bool AreFiltersHidden(void) const;                                              ///< \brief return true if filters are hidden
        void ShowFilters(bool bShow = true);                                            ///< \brief show / hide filters
        void HideFilters(bool bHide = true);                                            ///< \brief hide / show filters
        void ToggleFilters(void);                                                       ///< \brief show filters ON/OFF
        virtual wxVirtualDataViewIFilterEditor* GetFilterEditor(size_t uiItemID) const; ///< \brief get the filter editor for a specific item
        virtual bool StartFilter(size_t uiItemID);                                      ///< \brief start edition of a filter
        virtual bool EndFilter(void);                                                   ///< \brief finish edition of a filter

        //send event
        bool SendLayoutChangedEvent(void);                                              ///< \brief send an event when layout has changed (items hidden/shown, filters ON/OFF)

    protected:
        //constants
        static const size_t         s_uiNotFound;                                       ///< \brief special constant for GetFirstVisibleItem and GetNextVisibleItem

        //data
        EOrientation                    m_eOrientation;                                 ///< \brief the orientation of the control
        size_t                          m_uiCurrentMouseSpot;                           ///< \brief indicates which header the mouse is pointing
        size_t                          m_uiCurrentSelection;                           ///< \brief current selected item
        bool                            m_bPushingButton;                               ///< \brief true if a header is currently pushed
        wxVirtualHeaderIRenderer*       m_pRenderer;                                    ///< \brief renderer of the header
        wxSize                          m_BestSize;                                     ///< \brief cached best size
        wxSize                          m_MinSize;                                      ///< \brief cached min size
        bool                            m_bIsDragging;                                  ///< \brief true if one item is currently being dragged
        size_t                          m_uiDraggedItemID;                              ///< \brief dragged item ID
        size_t                          m_uiTargetDropItemID;                           ///< \brief the item currently below the mouse cursor during drag
        wxDragImage*                    m_pDragImage;                                   ///< \brief for displaying a dragging cursor
        size_t                          m_uiResizeBorderSize;                           ///< \brief resize border size
        bool                            m_bShowFilters;                                 ///< \brief true if filters are shown, false otherwise
        wxVirtualDataViewIFilterEditor* m_pActiveFilterEditor;                          ///< \brief the active filter
        size_t                          m_uiActiveFilteredItem;                         ///< \brief currently filtered item

        //methods
        void Init(void);                                                                ///< \brief main initialization
        void ComputeMinSize(void);                                                      ///< \brief compute minimal size

        //wxWindow override
        virtual wxSize DoGetBestSize(void) const;                                       ///< \brief get best size for the control
        void DoInvalidateBestSize(void);                                                ///< \brief force a recalculation of best size

        //reordering & resizing
        bool StartDragging(int x, int y, bool bSendEvent = true);                       ///< \brief start dragging an item
        bool DragItem(int x, int y, bool bSendEvent = true);                            ///< \brief perform the dragging operation
        bool StopDragging(int x, int y, size_t uiItemID,
                          bool bCancel = false, bool bSendEvent = true);                ///< \brief stop dragging an item
        int  GetNewItemSize(size_t uiItemID, int x, int y) const;                       ///< \brief compute the new item size
        bool CancelDragging(int x, int y, bool bSendEvent = true);                      ///< \brief cancel dragging

        //event handler
        void BindEventHandlers(void);                                                   ///< \brief bind the event handlers
        virtual void    OnDrawBackground(wxEraseEvent &rEvent);                         ///< \brief erase background event
                void    OnPaint(wxPaintEvent &rEvent);                                  ///< \brief paint event handler
        virtual void    DoPaint(wxDC &rDC);                                             ///< \brief performs the painting
        virtual void    OnMouse(wxMouseEvent &rEvent);                                  ///< \brief handle all kinds of mouse event
        virtual void    OnCaptureLost(wxMouseCaptureLostEvent &rEvent);                 ///< \brief mouse capture lost
        virtual void    OnRightClick(wxVirtualHeaderCtrlEvent &rEvent);                 ///< \brief right click event
        virtual void    OnLoseFocus(wxFocusEvent &rEvent);                              ///< \brief lose focus event

        //event handler for filter
        virtual void    OnFilterBegin(wxCommandEvent &rEvent);                          ///< \brief filter begin event
        virtual void    OnFilterChanged(wxCommandEvent &rEvent);                        ///< \brief filter changed event
        virtual void    OnFilterEnded(wxCommandEvent &rEvent);                          ///< \brief filter ended event

        //event handler for theming
        virtual void    OnSysColourChanged(wxSysColourChangedEvent &rEvent);            ///< \brief system colour changed - useful for theming

        //interface
        virtual void    PaintBackground(wxDC &rDC, wxRect &rRect);                      ///< \brief only paint background
        virtual void    PaintHeader(wxDC &rDC, wxRect &rRect, size_t uiViewCoord);      ///< \brief paint one button
        virtual void    PaintDragging(wxDC &rDC);                                       ///< \brief paint dragged item

        virtual void    AddMenuItems(wxMenu &rMenu);                                    ///< \brief add menu items for all items

    private:
        //methods
        void   DragCoordAdjust(int x, int y, int &cx, int &cy) const;                   ///< \brief adjust coordinates of dragging point
        size_t DoHitTest(int x, int y, int w, int h,
                         ESeparator &eSeparator, bool &bOnFilter,
                         bool bOutOfBoundsHit = false) const;                           ///< \brief get item below the client coordinate x, y
        size_t DoHitTest(int x, int y,
                         ESeparator &eSeparator, bool &bOnFilter,
                         bool bOutOfBoundsHit = false) const;                           ///< \brief get item below the client coordinate x, y
        void   ApplyMouseCursor(ESeparator eSeparator);                                 ///< \brief set the mouse cursor
        void   ApplyMouseCursor(int x, int y);                                          ///< \brief apply mouse cursor
        bool   SendEvent(wxEventType eType, size_t uiItemID,
                         int iNewSize = 0,
                         size_t uiNewOrder = size_t(-1));                               ///< \brief send an event
        bool   SendEventAndCheck(bool &rbAllowed,
                                 wxEventType eType, size_t uiItemID,
                                 int iNewSize = 0,
                                 size_t uiNewOrder = size_t(-1));                       ///< \brief send an event and check if it was vetoed
        bool   SendFilterShowHideEvent(void);                                           ///< \brief send a show/hide filter event

        virtual void AppendContextualMenuItems(wxMenu &rMenu);                          ///< \brief append menu items to the contextual menu
        virtual bool ProcessMenuCommand(const wxMenu &rMenu, int iSelectedItem);        ///< \brief process a menu command
};

#endif

