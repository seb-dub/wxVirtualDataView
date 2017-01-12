/**********************************************************************/
/** FILE    : VirtualDataViewCtrl.h                                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a class for viewing a very large amount of items       **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_CTRL_H_
#define _WX_VIRTUAL_VIEW_CTRL_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/control.h>
#include <wx/systhemectrl.h>
#include <wx/VirtualDataView/Columns/VirtualDataViewColumnsList.h>
#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>

class WXDLLIMPEXP_VDV wxVirtualIDataModel;
class WXDLLIMPEXP_VDV wxVirtualIProxyDataModel;
class WXDLLIMPEXP_VDV wxVirtualSortingDataModel;
class WXDLLIMPEXP_VDV wxVirtualFilteringDataModel;
class WXDLLIMPEXP_VDV wxVirtualIStateModel;
class WXDLLIMPEXP_VDV wxVirtualIModelRenderer;
class WXDLLIMPEXP_VDV wxVirtualDataViewBase;
class WXDLLIMPEXP_VDV wxVirtualDataViewColHeader;
class WXDLLIMPEXP_VDV wxVirtualDataViewRowHeader;
class WXDLLIMPEXP_VDV wxVirtualDataViewCornerHeader;
class WXDLLIMPEXP_VDV wxVirtualHeaderCtrlEvent;
class WXDLLIMPEXP_VDV wxVirtualDataViewEvent;
class WXDLLIMPEXP_VDV wxVirtualDataViewFilter;

extern const char wxVirtualDataViewCtrlNameStr[];

//styles
#define wxVDV_SINGLE                  0x0000     // for convenience
#define wxVDV_MULTIPLE                0x0001     // can select multiple items

#define wxVDV_HORIZ_RULES             0x0002     // light horizontal rules between rows
#define wxVDV_VERT_RULES              0x0004     // light vertical rules between columns

#define wxVDV_ROW_LINES               0x0008     // alternating colour in rows
#define wxVDV_VARIABLE_LINE_HEIGHT    0x0010     // variable line height

/// \class wxVirtualDataViewCtrl : the main virtual data view control
class WXDLLIMPEXP_VDV wxVirtualDataViewCtrl : public wxSystemThemedControl<wxControl>
{
    public:

        //constructors & destructor
        wxVirtualDataViewCtrl(void);                                            ///< \brief default constructor. Create() must be called manually
        wxVirtualDataViewCtrl(wxWindow *pParent, wxWindowID id,
                              const wxPoint& pos, const wxSize& size,
                              long lStyle,
                              const wxValidator& validator = wxDefaultValidator,
                              wxString sName = wxVirtualDataViewCtrlNameStr);   ///< \brief constructor
        virtual ~wxVirtualDataViewCtrl(void);                                   ///< \brief destructor

        //main window
        wxVirtualDataViewBase* GetMainWindow(void) const;                       ///< \brief get the main window

        //headers
        wxVirtualDataViewColHeader*     GetColumnHeaderWindow(void) const;      ///< \brief get the column header window
        wxVirtualDataViewRowHeader*     GetRowHeaderWindow(void) const;         ///< \brief get the row header window
        wxVirtualDataViewCornerHeader*  GetCornerHeaderWindow(void) const;      ///< \brief get the corner header window

        void ShowRowHeader(bool bShow = true);                                  ///< \brief show or hide the row header
        void HideRowHeader(void);                                               ///< \brief hide the row header
        bool IsRowHeaderShown(void) const;                                      ///< \brief check if the row header is shown

        void ShowColHeader(bool bShow = true);                                  ///< \brief show or hide the column header
        void HideColHeader(void);                                               ///< \brief hide the column header
        bool IsColHeaderShown(void) const;                                      ///< \brief check if the column header is shown

        //initialization
        virtual bool Create(wxWindow *pParent, wxWindowID id,
                            const wxPoint& pos,  const wxSize& size,
                            long lStyle,
                            const wxValidator& validator, wxString sName,
                            wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR,
                            wxVirtualIStateModel *pStateModel = WX_VDV_NULL_PTR,
                    wxVirtualIModelRenderer *pModelRenderer = WX_VDV_NULL_PTR); ///< \brief really creates the control and sets the initial number of items in it

        //windows management
        #ifdef __WXUNIVERSAL__
        bool IsCurrent(void) const;                                             ///< \brief check if the current window is the current one
        #endif

        //visual attributes
        wxVisualAttributes GetDefaultAttributes(void) const;                    ///< \brief get the default window attributes
        static wxVisualAttributes
         GetClassDefaultAttributes(wxWindowVariant variant =
                                   wxWINDOW_VARIANT_NORMAL);                    ///< \brief get the default window attributes

        //data model
        wxVirtualIDataModel*    GetBaseDataModel(void) const;                   ///< \brief get the current data model used, without any proxies
        wxVirtualIDataModel*    GetDataModel(void) const;                       ///< \brief get the current data model used, with all proxies if any
        virtual void            SetDataModel(wxVirtualIDataModel *pModel);      ///< \brief set the current data model. Current proxies will be kept

        //proxy data models
        void    AttachProxyModel(wxVirtualIProxyDataModel *pModel);             ///< \brief attach a proxy data model to the chain of models
        void    PopProxyModel(void);                                            ///< \brief remove the last proxy data model from the chain of models
        void    DetachProxyModel(wxVirtualIProxyDataModel *pModel);             ///< \brief remove a proxy data model from the chain of models
        void    DetachAllProxyModels(void);                                     ///< \brief remove all proxy data models from the chain of models

        //state model
        wxVirtualIStateModel* GetStateModel(void) const;                        ///< \brief get the state model
        void SetStateModel(wxVirtualIStateModel *pStateModel);                  ///< \brief set the state model

        //model renderer
        wxVirtualIModelRenderer* GetModelRenderer(void) const;                  ///< \brief get the model renderer
        void SetModelRenderer(wxVirtualIModelRenderer *pModelRenderer);         ///< \brief set the model renderer

        //callbacks for data change
        void OnModelItemCountChanged(void);                                     ///< \brief called by the client to notify that the data model has changed
        void OnDataModelChanged(void);                                          ///< \brief called by the client to notify that the data model has changed
        void OnLayoutChanged(void);                                             ///< \brief called by the client to notify changes in layout

        //columns
        wxVirtualDataViewColumnsList& Columns(void);                            ///< \brief list of columns
        const wxVirtualDataViewColumnsList& Columns(void) const;                ///< \brief list of columns - const version
        int GetColumnWidth(size_t uiColumn) const;                              ///< \brief get column width
        int GetColumnMinWidth(size_t uiColumn) const;                           ///< \brief get column minimal width
        int GetColumnStartOffset(size_t uiColumn) const;                        ///< \brief get column start offset
        void SetColumnWidth(size_t uiColumn, int iWidth);                       ///< \brief set column width
        void SetColumnMinWidth(size_t uiColumn, int iMinWidth);                 ///< \brief set column minimal width
        void AutoSizeColumn(size_t uiColumn);                                   ///< \brief resize the column
        void AutoSizeAllColumns(void);                                          ///< \brief resize all the columns
        void ShowColumn(size_t uiColumn, bool bShow = true);                    ///< \brief show / hide a column
        void HideColumn(size_t uiColumn, bool bHide = true);                    ///< \brief hide / show a column
        void ToggleHideShowColumn(size_t uiColumn);                             ///< \brief hide / show a column
        bool IsColumnShown(size_t uiColumn) const;                              ///< \brief check if a column is shown
        bool IsColumnHidden(size_t uiColumn) const;                             ///< \brief check if a column is hidden

        //refresh
        void RefreshHeaders(void);                                              ///< \brief refresh the headers windows
        void RefreshDataView(void);                                             ///< \brief refresh the data view only
        void RefreshAll(void);                                                  ///< \brief refresh the headers and the data view

        //events
        bool SendEvent(wxVirtualDataViewEvent &rEvent);                         ///< \brief send an event

        //item iteration : children
        wxVirtualItemID GetRootItem(void) const;                                ///< \brief get the root item
        size_t          GetChildrenCount(const wxVirtualItemID &rID) const;     ///< \brief get the amount of children item
        bool            HasChildren(const wxVirtualItemID &rID) const;          ///< \brief check if an item has children
        int             GetChildIndex(const wxVirtualItemID &rParent,
                                      const wxVirtualItemID &rChild) const;     ///< \brief get the index of a child
        int             GetChildIndex(const wxVirtualItemID &rChild) const;     ///< \brief get the index of a child
        wxVirtualItemID GetChildItem(const wxVirtualItemID &rID,
                                     size_t uiChild) const;                     ///< \brief get a child item
        wxVirtualItemID GetFirstChild(const wxVirtualItemID &rID) const;        ///< \brief get the first child
        wxVirtualItemID GetNextChild(const wxVirtualItemID &rParent,
                                     const wxVirtualItemID &rChild) const;      ///< \brief get the next child
        wxVirtualItemID GetPreviousChild(const wxVirtualItemID &rParent,
                                         const wxVirtualItemID &rChild) const;  ///< \brief get the previous child
        wxVirtualItemID GetLastChild(const wxVirtualItemID &rID) const;         ///< \brief get the last child
        size_t          GetAllChildren(wxVirtualItemIDs &vChildren,
                                       const wxVirtualItemID &rParent,
                                       bool bRecursive = false) const;          ///< \brief get all the children items

        //item iteration : parent & sibling
        wxVirtualItemID GetParentItem(const wxVirtualItemID &rID) const;        ///< \brief get the parent item
        wxVirtualItemID GetNextSibling(const wxVirtualItemID &rID) const;       ///< \brief get the next sibling item
        wxVirtualItemID GetPreviousSibling(const wxVirtualItemID &rID) const;   ///< \brief get the previous sibling item

        //item iteration : linear
        size_t GetItemCount(bool bCountRootItem = false) const;                 ///< \brief get the total count of item
        bool   IsEmpty(void) const;                                             ///< \brief check if the control is empty (no items apart from root)
        wxVirtualItemID GetFirstItem(void) const;                               ///< \brief get the 1st item (1st child of root)
        wxVirtualItemID GetNextItem(const wxVirtualItemID &rID,
                                    size_t uiCount = 1,
                                   bool bSkipCollapsedChildren = false) const;  ///< \brief get the next item
        wxVirtualItemID GetPreviousItem(const wxVirtualItemID &rID,
                                        size_t uiCount = 1,
                                   bool bSkipCollapsedChildren = false) const;  ///< \brief get the previous item

        //selection
        size_t GetSelectedItems(wxVirtualItemIDs &vIDs) const;                  ///< \brief get all the selected items
        wxVirtualItemID GetFirstSelectedItem(void) const;                       ///< \brief get the 1st selected item
        wxVirtualItemID GetNextSelectedItem(const wxVirtualItemID &rID) const;  ///< \brief get the next selected item
        wxVirtualItemID GetPrevSelectedItem(const wxVirtualItemID &rID) const;  ///< \brief get the previous selected item
        wxVirtualItemID GetFirstUnselectedItem(void) const;                     ///< \brief get the 1st un-selected item
        wxVirtualItemID GetNextUnselectedItem(const wxVirtualItemID &rID) const;///< \brief get the next un-selected item
        wxVirtualItemID GetPrevUnselectedItem(const wxVirtualItemID &rID) const;///< \brief get the previous un-selected item
        void SelectItem(const wxVirtualItemID &rID, bool bSelect = true,
                        bool bRecursive = false, bool bSendEvent = true);       ///< \brief select one item and optionally its children
        void DeselectItem(const wxVirtualItemID &rID, bool bRecursive = false,
                          bool bSendEvent = true);                              ///< \brief deselect an item and optionally its children
        void ToggleSelectedState(const wxVirtualItemID &rID,
                                 bool bRecursive = false,
                                 bool bSendEvent = true);                       ///< \brief toggle selection state of 1 or more items
        void SelectAll(bool bSelect = true, bool bSendEvent = true);            ///< \brief select all items
        void DeselectAll(bool bSendEvent = true);                               ///< \brief de-select all items
        void SelectItems(const wxVirtualItemIDs &vIDs, bool bSendEvent = true); ///< \brief select one or more items
        bool IsSelected(const wxVirtualItemID &rID) const;                      ///< \brief check if an item is selected
        bool IsDeselected(const wxVirtualItemID &rID) const;                    ///< \brief check if an item is deselected

        //check state
        /// \enum ECheckState : check state of the item
        enum ECheckState
        {
            WX_CHECKSTATE_UNCHECKED         = 0,                                ///< \brief the item is un-checked
            WX_CHECKSTATE_CHECKED           = 1,                                ///< \brief the item is checked
            WX_CHECKSTATE_UNDETERMINED      = 2                                 ///< \brief the item state is mixed (or unknown)
        };
        ECheckState GetCheckState(const wxVirtualItemID &rID) const;            ///< \brief get check state of an item
        bool        IsChecked(const wxVirtualItemID &rID) const;                ///< \brief test if an item is checked
        bool        IsUnchecked(const wxVirtualItemID &rID) const;              ///< \brief test if an item is unchecked
        bool        IsCheckStateUndetermined(const wxVirtualItemID &rID) const; ///< \brief test if an item is in unknown check state
        void        CheckItem(const wxVirtualItemID &rID,
                              ECheckState eState = WX_CHECKSTATE_CHECKED,
                              bool bRecursive = false, bool bSendEvent = true); ///< \brief check / uncheck items
        void        UncheckItem(const wxVirtualItemID &rID,
                                bool bRecursive = false,
                                bool bSendEvent = true);                        ///< \brief uncheck items
        void        SetCheckUndetermined(const wxVirtualItemID &rID,
                                         bool bRecursive = false,
                                         bool bSendEvent = true);               ///< \brief set items in undetermined check state
        size_t      GetCheckedItems(wxVirtualItemIDs &vIDs,
                        ECheckState eState = WX_CHECKSTATE_CHECKED) const;      ///< \brief get all the checked items
        size_t      GetUncheckedItems(wxVirtualItemIDs &vIDs) const;            ///< \brief get all the unchecked items
        size_t      GetUndeterminedCheckedItems(wxVirtualItemIDs &vIDs) const;  ///< \brief get all the items which are in an undetermined check state
        wxVirtualItemID GetFirstCheckedItem(ECheckState
                                        eState = WX_CHECKSTATE_CHECKED) const;  ///< \brief get the 1st checked item
        wxVirtualItemID GetNextCheckedItem(const wxVirtualItemID &rID,
                             ECheckState eState = WX_CHECKSTATE_CHECKED) const; ///< \brief get the next checked item
        wxVirtualItemID GetPrevCheckedItem(const wxVirtualItemID &rID,
                             ECheckState eState = WX_CHECKSTATE_CHECKED) const; ///< \brief get the previous checked item
        wxVirtualItemID GetFirstUncheckedItem(void) const;                      ///< \brief get the 1st unchecked item
        wxVirtualItemID GetNextUncheckedItem(const wxVirtualItemID &rID) const; ///< \brief get the next unchecked item
        wxVirtualItemID GetPrevUncheckedItem(const wxVirtualItemID &rID) const; ///< \brief get the previous unchecked item
        wxVirtualItemID GetFirstUndeterminedItem(void) const;                   ///< \brief get the 1st undetermined item
        wxVirtualItemID GetNextUndeterminedItem(
                                            const wxVirtualItemID &rID) const;  ///< \brief get the next undetermined item
        wxVirtualItemID GetPrevUndeterminedItem(
                                            const wxVirtualItemID &rID) const;  ///< \brief get the previous undetermined item
        void CheckAll(ECheckState eState = WX_CHECKSTATE_CHECKED,
                      bool bSendEvent = true);                                  ///< \brief check all items
        void UncheckAll(bool bSendEvent = true);                                ///< \brief un-check all items
        void SetAllUndeterminedCheckState(bool bSendEvent = true);              ///< \brief set all items in undetermined check state
        void ToggleCheckState(const wxVirtualItemID &rID,
                              bool bRecursive = false, bool bSendEvent = true); ///< \brief toggle check state of 1 or more item (undetermined becomes checked)

        //focused state
        wxVirtualItemID GetFocusedItem(void) const;                             ///< \brief get the focused item
        void SetFocusedItem(const wxVirtualItemID &rID, bool bSendEvent = true);///< \brief set the focused item
        int GetFocusedColumn(void) const;                                       ///< \brief get focused column ID
        void SetFocusedColumn(int iCol, bool bSendEvent = true);                ///< \brief set the focused column ID

        //enabled / disabled state
        bool IsItemEnabled(const wxVirtualItemID &rID) const;                   ///< \brief check if an item is enabled
        bool IsItemDisabled(const wxVirtualItemID &rID) const;                  ///< \brief check if an item is disabled
        void EnableItem(const wxVirtualItemID &rID, bool bEnable = true,
                        bool bRecursive = false);                               ///< \brief enable an item
        void DisableItem(const wxVirtualItemID &rID, bool bRecursive = false);  ///< \brief disable an item
        wxVirtualItemID GetFirstEnabledItem(bool bEnabled = true) const;        ///< \brief get first enabled item
        wxVirtualItemID GetNextEnabledItem(const wxVirtualItemID &rID,
                                           bool bEnabled = true) const;         ///< \brief get next enabled item
        wxVirtualItemID GetPrevEnabledItem(const wxVirtualItemID &rID,
                                           bool bEnabled = true) const;         ///< \brief get previous enabled item
        size_t          GetAllEnabledItems(wxVirtualItemIDs &vIDs,
                                           bool bEnabled = true) const;         ///< \brief get all enabled items
        wxVirtualItemID GetFirstDisabledItem(void) const;                       ///< \brief get first disabled item
        wxVirtualItemID GetNextDisabledItem(const wxVirtualItemID &rID) const;  ///< \brief get next disabled item
        wxVirtualItemID GetPrevDisabledItem(const wxVirtualItemID &rID) const;  ///< \brief get previous disabled item
        size_t          GetAllDisabledItems(wxVirtualItemIDs &vIDs) const;      ///< \brief get all disabled items

        //expand / collapse
        bool IsExpanded(const wxVirtualItemID &rID) const;                      ///< \brief check if an item is expanded
        bool IsCollapsed(const wxVirtualItemID &rID) const;                     ///< \brief check if an item is collapsed
        void Expand(const wxVirtualItemID &rID, bool bExpand = true,
                    bool bRecursive = false,
                    bool bSendEvent = true);                                    ///< \brief expand an item
        void Collapse(const wxVirtualItemID &rID, bool bRecursive = false,
                      bool bSendEvent = true);                                  ///< \brief collapse an item
        size_t GetExpandedItems(wxVirtualItemIDs &vIDs,
                                bool bExpanded = true) const;                   ///< \brief get all expanded items
        size_t GetCollapsedItems(wxVirtualItemIDs &vIDs) const;                 ///< \brief get all collapsed items
        wxVirtualItemID GetFirstExpandedItem(bool bExpanded = true) const;      ///< \brief get the 1st expanded item
        wxVirtualItemID GetNextExpandedItem(const wxVirtualItemID &rID,
                                            bool bExpanded = true) const;       ///< \brief get the next expanded item
        wxVirtualItemID GetPrevExpandedItem(const wxVirtualItemID &rID,
                                            bool bExpanded = true) const;       ///< \brief get the previous expanded item
        wxVirtualItemID GetFirstCollapsedItem(void) const;                      ///< \brief get the 1st collapsed item
        wxVirtualItemID GetNextCollapsedItem(const wxVirtualItemID &rID) const; ///< \brief get the next collapsed item
        wxVirtualItemID GetPrevCollapsedItem(const wxVirtualItemID &rID) const; ///< \brief get the previous collapsed item
        void ExpandAll(bool bExpand = true, bool bSendEvent = true);            ///< \brief expand all items
        void CollapseAll(bool bSendEvent = true);                               ///< \brief collapse all items
        void ToggleExpandCollapseState(const wxVirtualItemID &rID,
                                       bool bRecursive = false,
                                       bool bSendEvent = true);                 ///< \brief toggle expanded / collapsed state

        //activate an item
        void ActivateItem(const wxVirtualItemID &rID, int iCol,
                          bool bSendEvent = true);                              ///< \brief activate an item

        //hit test & bounding rect
        /// \struct THitTest : hit testing information
        struct THitTest
        {
            wxUint32        m_uiFlags;                                          ///< \brief additional flags of the hit test
            wxCoord         m_x;                                                ///< \brief the client coordinate X of the original point
            wxCoord         m_y;                                                ///< \brief the client coordinate Y of the original point
            wxVirtualItemID m_ID;                                               ///< \brief the item hit
            wxUint32        m_uiField;                                          ///< \brief field index of the item hit (model coord)
            wxUint32        m_uiColIndex;                                       ///< \brief column index of the item hit (view coord)
            wxRect          m_RectItem;                                         ///< \brief bounding rectangle of the item hit (virtual coordinates)
            wxUint32        m_uiRank;                                           ///< \brief rank of the item
            wxCoord         m_RelativeX;                                        ///< \brief the X coordinate of the point, relative to the hit item rect top left corner
            wxCoord         m_RelativeY;                                        ///< \brief the Y coordinate of the point, relative to the hit item rect top left corner
        };
        bool HitTest(THitTest &rResult, wxCoord x, wxCoord y,
                     bool bClientAreaOnly = true);                              ///< \brief performs a hit test
        bool GetColItemRect(wxRect &rRect,
                            const wxVirtualItemID &rID,
                            size_t uiColIndex,
                            bool bClientCoordinates = true);                    ///< \brief get the rectangle of an item
        bool GetItemRect(wxRect &rRect,
                         const wxVirtualItemID &rID,
                         bool bClientCoordinates = true);                       ///< \brief get the rectangle of an item

        //visibility
        /// \enum EVisibilityFlags : where the item should be visible
        enum EVisibilityFlags
        {
            WX_VISIBLE_DONT_CARE    = 0,                                        ///< \brief as long as the item is visible, nothing is done
            WX_VISIBLE_AT_TOP       = 1,                                        ///< \brief the item must be the 1st visible
            WX_VISIBLE_AT_BOTTOM    = 2                                         ///< \brief the item must be the last visible
        };
        bool IsItemVisible(const wxVirtualItemID &rID, size_t uiCol = 0,
                           bool bCheckCol = true);                              ///< \brief check if the item is visible
        bool MakeItemVisible(const wxVirtualItemID &rID, size_t uiCol,
                             EVisibilityFlags eFlag = WX_VISIBLE_DONT_CARE,
                             bool bSelect = true);                              ///< \brief make the item visible (without expanding)
        bool EnsureItemVisible(const wxVirtualItemID &rID, size_t uiCol,
                               EVisibilityFlags eFlag = WX_VISIBLE_DONT_CARE,
                               bool bSelect = true);                            ///< \brief ensure the item is visible (with expanding when needed)
        wxVirtualItemID  GetFirstVisibleItem(void) const;                       ///< \brief get the 1st visible item
        wxVirtualItemID  GetNextVisibleItem(const wxVirtualItemID &rID) const;  ///< \brief get the next visible item
        wxVirtualItemID  GetPrevVisibleItem(const wxVirtualItemID &rID) const;  ///< \brief get the previous visible item
        size_t           GetAllVisibleItems(wxVirtualItemIDs &vIDs) const;      ///< \brief get all visible items

        //get / set value
        /// \enum EDataType : the kind of data to get
        enum EDataType
        {
            WX_ITEM_MAIN_DATA           = 0,                                    ///< \brief main data to retrieve
            WX_ITEM_TOOLTIP_DATA        = 1,                                    ///< \brief the tool-tip data
            WX_ITEM_HELP_DATA           = 2                                     ///< \brief the help data
        };
        wxVariant GetItemData(const wxVirtualItemID &rID, int iCol,
                              EDataType eType = WX_ITEM_MAIN_DATA) const;       ///< \brief get item data
        bool      SetItemData(const wxVirtualItemID &rID, int iCol,
                              wxVariant vValue,
                              bool bSendEvent = true,
                              EDataType eType = WX_ITEM_MAIN_DATA);             ///< \brief set item data

        //item edition
        bool            StartItemEdition(const wxVirtualItemID &rID, int iCol); ///< \brief start the edition of an item
        wxVirtualItemID GetEditedItem(void) const;                              ///< \brief get the currently edited item
        int             GetEditedColumn(void) const;                            ///< \brief get the currently edited column
        bool            FinishEdition(bool bCancelled);                         ///< \brief provokes the end of the current edition


        //graphic attributes
        wxVirtualDataViewItemAttr GetItemAttribute(const wxVirtualItemID &rID,
                                                   int iCol,
                                            bool bMergeWithColAttrib = true);   ///< \brief get the item graphic attributes

        //searching
        wxVirtualItemID FindFirst(const wxVariant &rvValue,
                                  bool bFullMatch, bool bCaseSensitive,
                                  bool bRegEx,
                                  bool bSelect = true,
                                  bool bMakeVisible = true, int iCol = -1);     ///< \brief find the 1st item matching a criteria
        wxVirtualItemID FindNext(const wxVirtualItemID &rID,
                                 const wxVariant &rvValue,
                                 bool bFullMatch, bool bCaseSensitive,
                                 bool bRegEx,
                                 bool bSelect = true,
                                 bool bMakeVisible = true, int iCol = -1);      ///< \brief find the next item matching a criteria
        wxVirtualItemID FindPrev(const wxVirtualItemID &rID,
                                 const wxVariant &rvValue,
                                 bool bFullMatch, bool bCaseSensitive,
                                 bool bRegEx,
                                 bool bSelect = true,
                                 bool bMakeVisible = true, int iCol = -1);      ///< \brief find the previous item matching a criteria

        //sorting
        void SortItems(void);                                                   ///< \brief sort all the items according to the current sort filter
        void ClearSortFilters(void);                                            ///< \brief clear all sorting filters

        //filtering
        wxVirtualDataViewIFilterEditor* GetFilterEditor(size_t uiCol);          ///< \brief get the filter editor for a column
        bool SetFilterEditor(size_t uiCol,
                             wxVirtualDataViewIFilterEditor *pFilterEditor);    ///< \brief set the filter editor for a column
        void                        ClearAllFilters(void);                      ///< \brief clear all the filters
        void                        ApplyFilters(void);                         ///< \brief apply the current filters

        bool AreFiltersShown(void) const;                                       ///< \brief return true if filters are shown
        bool AreFiltersHidden(void) const;                                      ///< \brief return true if filters are hidden
        void ShowFilters(bool bShow = true);                                    ///< \brief show / hide filters
        void HideFilters(bool bHide = true);                                    ///< \brief hide / show filters
        void ToggleFilters(void);                                               ///< \brief show filters ON/OFF
        virtual bool StartFilter(size_t uiCol);                                 ///< \brief start edition of a filter
        virtual bool EndFilter(void);                                           ///< \brief finish edition of a filter
        bool IsFiltering(size_t uiCol);                                         ///< \brief check if the filter is active and not empty
        bool IsFiltering(void);                                                 ///< \brief check if at least one filter is active and not empty

        //column resizing
        bool StartResizingColumn(size_t uiCol, int iNewWidth);                  ///< \brief start resizing a column
        bool ResizingColumn(size_t uiCol, int iNewWidth);                       ///< \brief currently resizing a column
        bool ResizeColumn(size_t uiCol, int iNewWidth);                         ///< \brief finish resizing a column

    protected:
        //data
        wxVirtualDataViewBase*          m_pClientArea;                          ///< \brief the window showing the data
        wxVirtualDataViewColHeader*     m_pColumnHeaderWindow;                  ///< \brief column header window
        wxVirtualDataViewRowHeader*     m_pRowHeaderWindow;                     ///< \brief row header window
        wxVirtualDataViewCornerHeader*  m_pCornerHeaderWindow;                  ///< \brief corner header window
        wxVirtualDataViewColumnsList    m_vColumns;                             ///< \brief list of columns
        wxVirtualSortingDataModel*      m_pSortingModel;                        ///< \brief sorting data model
        wxVirtualFilteringDataModel*    m_pFilteringModel;                      ///< \brief filtering data model

        //methods
        void InitDataView(void);                                                ///< \brief initialize the control

        //event handlers
        virtual void OnSize(wxSizeEvent &rEvent);                               ///< \brief sizing event handlers
        virtual void OnScrollEvent(wxScrollWinEvent &rEvent);                   ///< \brief scrolling event handler

        //column header event handlers
        virtual void OnColHeaderBeginSize(wxVirtualHeaderCtrlEvent &rEvent);    ///< \brief a column header is starting to be resized
        virtual void OnColHeaderSize(wxVirtualHeaderCtrlEvent &rEvent);         ///< \brief a column header is resized
        virtual void OnColHeaderSizing(wxVirtualHeaderCtrlEvent &rEvent);       ///< \brief a column header is currently resizing itself
        virtual void OnColHeaderAutoSize(wxVirtualHeaderCtrlEvent &rEvent);     ///< \brief a column header separator has been double clicked
        virtual void OnColHeaderStartReorder(wxVirtualHeaderCtrlEvent &rEvent); ///< \brief a column header is starting to be dragged
        virtual void OnColHeaderReorder(wxVirtualHeaderCtrlEvent &rEvent);      ///< \brief a column header has been dragged
        virtual void OnColHeaderSort(wxVirtualHeaderCtrlEvent &rEvent);         ///< \brief a column header has been left click
        virtual void OnColHeaderEvent(wxVirtualHeaderCtrlEvent &rEvent);        ///< \brief a column header event has occurred
        virtual void OnColHeaderLayout(wxVirtualHeaderCtrlEvent &rEvent);       ///< \brief a column header layout changed event has occurred
        virtual void OnColHeaderFilterBegin(wxVirtualHeaderCtrlEvent &rEvent);  ///< \brief a column header filter begin event has occurred
        virtual void OnColHeaderFilterChanged(wxVirtualHeaderCtrlEvent &rEvent);///< \brief a column header filter changed event has occurred
        virtual void OnColHeaderFilterEnd(wxVirtualHeaderCtrlEvent &rEvent);    ///< \brief a column header filter end event has occurred

        //layout
        void ResizeChildren(void);                                              ///< \brief resize all the children
        void ResizeChildren(wxSize sClientSize);                                ///< \brief resize all the children
        virtual wxSize DoGetBestSize(void) const;                               ///< \brief wxWindow override

    private:
        //inlined
        wxVirtualIDataModel*    DoGetDataModel(void) const;                     ///< \brief get the data model
        wxVirtualIStateModel*   DoGetStateModel(void) const;                    ///< \brief get the state model

        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataViewCtrl);
};

#endif // _WX_VLBOX_H_

