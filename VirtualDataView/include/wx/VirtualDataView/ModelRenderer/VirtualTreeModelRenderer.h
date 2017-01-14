/**********************************************************************/
/** FILE    : VirtualTreeModelRenderer.h                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : tree implementation of wxVirtualIModelRenderer         **/
/**********************************************************************/


#ifndef WX_VIRTUAL_TREE_MODEL_RENDERERER_H_
#define WX_VIRTUAL_TREE_MODEL_RENDERERER_H_

#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/VirtualDataView/DataView/VirtualTreePath.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualIModelRenderer.h>
#include <wx/vector.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewBase;
class WXDLLIMPEXP_VDV wxVirtualDataViewItemState;
class WXDLLIMPEXP_VDV wxVirtualItemID;
class WXDLLIMPEXP_VDV wxVirtualDataViewColumnsList;
class WXDLLIMPEXP_VDV wxVirtualDataViewEditor;

/** \class wxVirtualTreeModelRenderer : an implementation of wxVirtualIModelRenderer
  * This is used to render a tree, a tree list or a table / list
  */
class WXDLLIMPEXP_VDV wxVirtualTreeModelRenderer : public wxVirtualIModelRenderer
{
    public:
        //hit test flags

        //constructors & destructor
        wxVirtualTreeModelRenderer(wxVirtualDataViewBase* pClient,
                                   wxVirtualDataViewCtrl* pScroller);   ///< \brief constructor
        virtual ~wxVirtualTreeModelRenderer(void);                      ///< \brief destructor

        //interface implementation - event handling
        virtual bool PaintModel(wxDC &rDC);                             ///< \brief paint the model
        virtual bool EraseBackgroundEvent(wxEraseEvent &rEvent);        ///< \brief erase background event
        virtual bool SizeEvent(wxSizeEvent &rEvent);                    ///< \brief a size event occurred
        virtual bool FocusEvent(wxFocusEvent &rEvent);                  ///< \brief a focus event occurred
        virtual bool ScrollEvent(wxScrollWinEvent &rEvent);             ///< \brief a scroll event occurred
        virtual bool MouseEvent(wxMouseEvent &rEvent);                  ///< \brief a mouse event occurred
        virtual bool KeyboardEvent(wxKeyEvent &rEvent);                 ///< \brief a keyboard event occurred
        virtual bool ContextMenuEvent(wxContextMenuEvent &rEvent);      ///< \brief a context menu event occurred
        virtual bool HelpEvent(wxHelpEvent &rEvent);                    ///< \brief a help event occurred
        virtual bool StartEdition(const wxVirtualItemID &rID,
                                  size_t uiCol);                        ///< \brief start the item edition
        virtual bool FinishEdition(bool bCancelled);                    ///< \brief provokes the end of the current edition

        //interface implementation - sizing
        virtual wxSize GetBestSize(void) const;                         ///< \brief get the best size
        virtual wxSize GetBestSize(size_t uiColID) const;               ///< \brief get the best size of a column
        virtual bool HitTest(THitTest &rResult,
                             wxCoord x, wxCoord y,
                             bool bClientAreaOnly = true);              ///< \brief performs a hit test
        virtual bool GetItemRect(wxRect &rRect,
                                 const wxVirtualItemID &rID,
                                 size_t uiColIndex,
                                 bool bClientCoordinates = true);       ///< \brief get the rectangle of an item
        virtual bool Expand(const wxVirtualItemID &rID,
                            bool bRefresh = true,
                            bool bRecursive = false,
                            bool bUpdateScrollbars = true,
                            bool bSendEvent = true);                    ///< \brief expand an item
        virtual bool Collapse(const wxVirtualItemID &rID,
                              bool bRefresh = true,
                              bool bRecursive = false,
                              bool bUpdateScrollbars = true,
                              bool bSendEvent = true);                  ///< \brief collapse an item

        //interface implementation - misc
        virtual int GetFocusedField(void) const;                        ///< \brief get focused column ID
        virtual void SetFocusedField(int iField);                       ///< \brief set focused column ID
        virtual void UpdateScrollbars(void);                            ///< \brief update the scrollbars

        //interface implementation - callbacks
        virtual void OnDataModelChanged(void);                          ///< \brief called by the client to notify that the data model has changed
        virtual void OnLayoutChanged(void);                             ///< \brief called by the client to notify changes in layout

        //margins
        int  GetLeftMargin(void) const;                                 ///< \brief get the left margin, in pixels
        int  GetRightMargin(void) const;                                ///< \brief get the right margin, in pixels
        int  GetTopMargin(void) const;                                  ///< \brief get the top margin, in pixels
        int  GetBottomMargin(void) const;                               ///< \brief get the bottom margin, in pixels
        void SetLeftMargin(int iMargin);                                ///< \brief set the left margin, in pixels
        void SetRightMargin(int iMargin);                               ///< \brief set the right margin, in pixels
        void SetTopMargin(int iMargin);                                 ///< \brief set the top margin, in pixels
        void SetBottomMargin(int iMargin);                              ///< \brief set the bottom margin, in pixels

        //row and column picking thickness (resizing)
        int   GetRowBorderThickness(void) const;                        ///< \brief get the row border thickness (for picking)
        void  SetRowBorderThickness(int iThickness);                    ///< \brief set the row border thickness (for picking)
        int   GetColumnBorderThickness(void) const;                     ///< \brief get the column border thickness (for picking)
        void  SetColumnBorderThickness(int iThickness);                 ///< \brief set the column border thickness (for picking)

        //grid lines
        bool  DrawHorizontalGridLines(void) const;                      ///< \brief return true if horizontal grid lines are drawn, false otherwise
        void  DrawHorizontalGridLines(bool bDraw);                      ///< \brief set/unset drawing of horizontal grid lines
        wxPen GetHorizontalGridLinesPen(void) const;                    ///< \brief get the pen used to draw the horizontal grid lines
        void  SetHoritontalGridLinesPen(wxPen pen);                     ///< \brief set the pen used to draw the horizontal grid lines
        bool  DrawVerticalGridLines(void) const;                        ///< \brief return true if vertical grid lines are drawn, false otherwise
        void  DrawVerticalGridLines(bool bDraw);                        ///< \brief set/unset drawing of vertical grid lines
        wxPen GetVerticalGridLinesPen(void) const;                      ///< \brief get the pen used to draw the vertical grid lines
        void  SetVerticalGridLinesPen(wxPen pen);                       ///< \brief set the pen used to draw the vertical grid lines

        //tree buttons & lines
        /// \enum ETreeLinesStyle : the kind of tree lines to draw
        enum ETreeLinesStyle
        {
            E_TREE_LINES_NONE        = 0,                               ///< \brief no tree lines
            E_TREE_LINES_AT_ITEM     = 1,                               ///< \brief tree lines go up to the parent item
            E_TREE_LINES_AT_EXPANDER = 2                                ///< \brief tree lines go up to the expander
        };

        bool  HideRootItem(void) const;                                 ///< \brief check if root item is hidden
        void  HideRootItem(bool bHide);                                 ///< \brief hide or show root item
        void  ShowRootItem(void);                                       ///< \brief show root item
        wxPen GetTreeLinesPen(void) const;                              ///< \brief get the pen used to draw lines connecting nodes
        void  SetTreeLinesPen(wxPen pen);                               ///< \brief set the pen used to draw lines connecting nodes
        ETreeLinesStyle  GetTreeLinesStyle(void) const;                 ///< \brief check how lines connecting nodes are drawn
        void  SetTreeLinesStyle(ETreeLinesStyle eTreeLinesStyle);       ///< \brief set how lines connecting nodes are drawn
        int   GetIdentationSize(void) const;                            ///< \brief get the indentation size
        void  SetIdentationSize(int iIndent);                           ///< \brief set the indentation size
        wxSize GetTreeButtonSize(void) const;                           ///< \brief get the tree button size
        void   SetTreeButtonSize(wxSize s);                             ///< \brief set the tree button size
        int    GetTreeButtonMargins(void) const;                        ///< \brief get the tree button margins
        void   SetTreeButtonMargins(int iMargin);                       ///< \brief set the tree button margins (left, right, top, bottom)

        //background colours
        bool    UseAlternatingRowBackColours(void) const;               ///< \brief return true if alternating row back colours are used
        bool    UseViewDependentRowBackColours(void) const;             ///< \brief return true if view dependent back colours are used
        void    SetAlternatingRowBackColours(bool bAlternate,
                                             bool bViewDependent);      ///< \brief set alternating row back colours style
        void    GetAlternatingBackColours(wxColour &cEven,
                                          wxColour &cOdd) const;        ///< \brief get the alternating row back colours
        void    SetAlternatingBackColours(wxColour cEven,
                                          wxColour cOdd);               ///< \brief set the alternating row back colours

        //selected item
        bool    AreSelectedItemsShownAsFocused(void) const;             ///< \brief check if selected items are always shown as focused
        void    SetSelectedItemsShownAsFocused(bool bAsFocused = true); ///< \brief set if selected items are always shown as focused
        wxColour GetSelectedItemsColour(void) const;                    ///< \brief get the selected items colour
        void     SetSelectedItemsColour(wxColour cColour);              ///< \brief set the selected items colour

        //checkbox
        bool   AreCheckBoxesDrawn(void) const;                          ///< \brief return true if checkboxes are drawn
        void   DrawCheckBoxes(bool bDraw = true);                       ///< \brief hide / show checkboxes
        void   HideCheckBoxes(void);                                    ///< \brief hide checkboxes
        wxSize GetCheckBoxSize(void) const;                             ///< \brief get the check box size
        void   SetCheckBoxSize(wxSize s);                               ///< \brief set the check box size
        int    GetCheckBoxMargins(void) const;                          ///< \brief get the check box margins
        void   SetCheckBoxMargins(int iMargin);                         ///< \brief set the check box margins (left, right, top, bottom)

        //uniform row heights
        bool HasUniformRowHeight(void) const;                           ///< \brief return true if the tree has uniform row heights
        void SetUniformRowHeight(bool bHasUniformRowHeight = true);     ///< \brief set/unset uniform row heights
        void SetVariableRowHeight(void);                                ///< \brief unset uniform row heights
        void SetUniformRowHeightValue(int iHeight = -1);                ///< \brief set the uniform row height value
        int  GetUniformRowHeight(void) const;                           ///< \brief get the uniform row height value
        int  GetRowHeightWithMargins(int iHeightWithoutMargins) const;  ///< \brief get the row height with margins

        //uniform column widths
        bool HasUniformColWidth(void) const;                            ///< \brief return true if the tree has uniform column widths
        void SetUniformColWidth(bool bHasUniformColWidth = true);       ///< \brief set/unset uniform column widths
        void SetVariableColWidth(void);                                 ///< \brief unset uniform column widths
        void SetUniformColWidthValue(int iWidth = -1);                  ///< \brief set the uniform column width value
        int  GetUniformColWidth(void) const;                            ///< \brief get the uniform column width value
        int  GetColWidthWithMargins(int iColWidthWithoutMargins) const; ///< \brief get the column width with margins

        //columns
        wxVirtualDataViewColumnsList& Columns(void);                    ///< \brief list of columns
        const wxVirtualDataViewColumnsList& Columns(void) const;        ///< \brief list of columns

        //visibility
        virtual bool IsItemVisible(const wxVirtualItemID &rID,
                                   size_t uiCol = 0,
                                   bool bCheckField = true) const;      ///< \brief check if the item is visible
        virtual bool MakeItemVisible(const wxVirtualItemID &rID,
                                     size_t uiCol,
                                     EVisibilityFlags eFlag);           ///< \brief make the item visible (without expanding)
        virtual bool EnsureItemVisible(const wxVirtualItemID &rID,
                                      size_t uiCol,
                        EVisibilityFlags eFlag = WX_VISIBLE_DONT_CARE); ///< \brief ensure the item is visible (with expanding when needed)
        virtual wxVirtualItemID GetFirstVisibleItem(void) const;        ///< \brief get the 1st visible item
        virtual size_t GetAllVisibleItems(wxVirtualItemIDs &vIDs) const;///< \brief get all visible items
        virtual bool ScrollTo(size_t uiVerticalPos, size_t uiCol,
                              bool bRefresh = true);                    ///< \brief update scrollbar positions
        virtual bool MoveTo(size_t uiRank, bool bControlDown,
                            bool bShiftDown);                           ///< \brief update vertical scrollbar position and select object

    protected:
        //data
        bool                            m_bHasFocus;                    ///< \brief true if the window has the focus
        int                             m_iMarginLeft;                  ///< \brief left margin, in pixels
        int                             m_iMarginRight;                 ///< \brief right margin, in pixels
        int                             m_iMarginTop;                   ///< \brief top margin, in pixels
        int                             m_iMarginBottom;                ///< \brief bottom margin, in pixels
        int                             m_iRowBorderThickness;          ///< \brief row border thickness (for picking)
        int                             m_iColumnBorderThickness;       ///< \brief column border thickness (for picking)
        wxPen                           m_HorizontalRulesPen;           ///< \brief pen used for drawing the horizontal grid lines
        wxPen                           m_VerticalRulesPen;             ///< \brief pen used for drawing the vertical grid lines
        bool                            m_bDrawHoritontalRules;         ///< \brief true if horizontal grid lines are drawn
        bool                            m_bDrawVerticalRules;           ///< \brief true if vertical grid lines are drawn
        bool                            m_bHideRootItem;                ///< \brief true for hiding root item, false for showing it
        wxPen                           m_TreeLinesPen;                 ///< \brief the pen used to draw tree lines
        ETreeLinesStyle                 m_eTreeLinesStyle;              ///< \brief how tree lines are drawn
        wxSize                          m_TreeButtonSize;               ///< \brief tree button size
        int                             m_iTreeButtonMargins[4];        ///< \brief tree button margins (left, right, top, bottom)
        int                             m_iTreeButtonTotalWidth;        ///< \brief tree button total width, with margins
        bool                            m_bDrawTreeButtons;             ///< \brief true if tree buttons (expanders) are drawn
        wxSize                          m_CheckboxSize;                 ///< \brief size of the checkbox
        int                             m_iCheckboxMargins[4];          ///< \brief checkbox margins (left, right, top, bottom)
        int                             m_iCheckboxTotalWidth;          ///< \brief checkbox total width, with margins
        bool                            m_bDrawCheckbox;                ///< \brief true if check box are drawn
        int                             m_iIndentation;                 ///< \brief item indentation
        bool                            m_bUniformRowHeight;            ///< \brief true if each row has the same height
        int                             m_iRowHeight;                   ///< \brief for uniform row height, the height of the row
        bool                            m_bUniformColWidth;             ///< \brief true if each column has the same width
        int                             m_iColWidth;                    ///< \brief for uniform column width, the width of the column
        wxVirtualTreePath               m_FirstVisibleItem;             ///< \brief the 1st visible item
        int                             m_iFirstVisibleLine;            ///< \brief the 1st visible line
        bool                            m_bGuard;                       ///< \brief reentrency guard
        size_t                          m_uiFieldFocusedItem;           ///< \brief field of the focused item
        size_t                          m_uiColFocusedItem;             ///< \brief column index of the focused item
        size_t                          m_uiRankFocusedItem;            ///< \brief rank of the focused item
        bool                            m_bAlternatingBackColour;       ///< \brief alternating background colour
        bool                            m_bFixedAlternatingBackColour;  ///< \brief true if back colour is computed relative to top visible row
        wxColour                        m_cRowColourEven;               ///< \brief even row colours
        wxColour                        m_cRowColourOdd;                ///< \brief odd row colours
        wxColour                        m_cRowColourEvenReal;               ///< \brief even row colours
        wxColour                        m_cRowColourOddReal;                ///< \brief odd row colours
        bool                            m_bIsEditing;                   ///< \brief true if edition of cell is currently done
        wxVirtualDataViewEditor*        m_pEditor;                      ///< \brief current cell editor
        wxSize                          m_sVirtualSize;                 ///< \brief virtual size of the control
        int                             m_iNbVisiblesItems;             ///< \brief amount of visible items in the control (not invisible due to collapsed parent)
        wxSize                          m_sCurrentClientSize;           ///< \brief current client size
        bool                            m_bShowSelectedItemsAsFocused;  ///< \brief true for showing selected items as focused
        wxColour                        m_cSelectedItemsColour;         ///< \brief selected items colour
        bool                            m_bDraggingColumn;              ///< \brief true if a column separator is being dragged
        int                             m_iDraggedColumn;               ///< \brief the column being dragged
        bool                            m_bFocusGuard;                  ///< \brief focus event guard (see FocusEvent() and StartEdition() for more info)

        //methods
        void Init(void);                                                ///< \brief initialize the renderer
        wxPen InitTreeLinesPen(void) const;                             ///< \brief create the tree lines pen
        void ComputeTreeButtonSize(void);                               ///< \brief compute the size of the tree button, with margins
        void ComputeCheckBoxSize(void);                                 ///< \brief compute the size of the check box, with margins

        //sizing & scrolling
        void InvalidateCurrentSize(void);                               ///< \brief invalidate the current size
        void InvalidateVirtualSize(void);                               ///< \brief invalidate the virtual size
        virtual wxSize ComputeSize(size_t &uiNbItemsVisible) const;     ///< \brief compute the full size & amount of items
        virtual void AdjustScrollbars(void);                            ///< \brief adjust the scrollbars
        virtual wxVirtualTreePath ConvertRowToTreePath(int iLine) const;///< \brief convert a line number to a tree path
        virtual void ComputeFirstVisibleItem(int iFirstLine);           ///< \brief compute the 1st visible item
        virtual void ShiftFirstVisibleItem(int iDiff);                  ///< \brief shift the 1st visible item up or down
        virtual void RecomputeFirstVisibleItem(void);                   ///< \brief recompute the 1st visible item
        virtual void InvalidateFirstVisibleItem(void);                  ///< \brief invalidate the 1st visible item
        virtual void ScrollEditorWindow(void);                          ///< \brief scroll the editor window
        virtual wxSize GetItemSize(const wxVirtualItemID &rID,
                                   size_t uiColID) const;               ///< \brief get the size of 1 item
        virtual bool MakeItemVisible(const wxVirtualItemID &rID,
                                     size_t uiRank, size_t uiCol,
                                     EVisibilityFlags eFlag,
                                     bool bRefresh);                    ///< \brief make the item visible (without expanding)
        virtual bool MoveTo(const wxVirtualItemID &rID, size_t uiRank,
                            bool bControlDown, bool bShiftDown);        ///< \brief update vertical scrollbar position and select object
        void SetScrollbars(int iHPos, int iHPage, int iHRange,
                           int iVPos, int iVPage, int iVRange);         ///< \brief set both horizontal and vertical scrollbars
        void SetOneScrollbar(int iPos, int iPage, int iRange,
                             int iOrient);                              ///< \brief set one scrollbar
        void SetAllScrollbars(wxSize sVirtualSize, size_t uiNbItems);   ///< \brief set all scrollbars
        wxSize GetTargetSize(void) const;                               ///< \brief get scrolled window client size
        wxSize GetScrollerVisibleSize(void) const;                      ///< \brief client size available if no scrollbars are shown

        //column resizing
        bool ResizeColumn(const THitTest &ht, wxMouseEvent &rEvent);    ///< \brief check and perform columns resizing when needed

        //expand/collapse
        bool Expand(const wxVirtualItemID &rID,
                            bool bRefresh,
                            bool bRecursive,
                            bool bUpdateScrollbars,
                            size_t uiRank, bool bSendEvent);            ///< \brief expand an item
        bool Collapse(const wxVirtualItemID &rID,
                              bool bRefresh,
                              bool bRecursive,
                              bool bUpdateScrollbars,
                              size_t uiRank, bool bSendEvent);          ///< \brief collapse an item
        bool CollapseOrExpand(const wxVirtualItemID &rID,
                              bool bExpand,
                              bool bRefresh, bool bRecursive,
                              bool bUpdateScrollbars,
                              size_t uiRank, bool bSendEvent);          ///< \brief collapse or expand the node

        //tooltip
        void SetToolTip(const THitTest &ht);                            ///< \brief set the tooltip data

        //selection
        virtual bool PerformSelection(const wxVirtualItemID &rID,
                                      size_t uiField, size_t uiCol,
                                      size_t uiRank,
                                      bool bAddOrRemove,
                                      bool bSelectRange,
                                      bool bSendEvent = true,
                                      bool bSelect = true);             ///< \brief do the selection
        virtual bool SelectItem(const wxVirtualItemID &rID,
                                size_t uiField, size_t uiCol,
                                bool bToggle, bool bSelect,
                                bool bSendEvent = true);                ///< \brief shortcut for toggling one item
        virtual bool ActivateItem(const wxVirtualItemID &rID,
                                  size_t uiField, size_t uiCol,
                                  bool bSelect,
                                  bool bSendEvent = true);              ///< \brief activate an item
        virtual bool SetItemFocus(const wxVirtualItemID &rID,
                                  size_t uiField, size_t uiCol,
                                  size_t uiRank,
                                  bool bSendEvent = true);              ///< \brief change the focused item
        virtual void UpdateFocusedItemRank(void);                       ///< \brief update the rank of the focused item

        //edition
        void AdjustItemRect(wxRect &r, const wxVirtualItemID &rID);     ///< \brief adjust item rect for taking into account indentation
        bool EditItem(wxVirtualIStateModel *pStateModel,
                      const wxVirtualItemID &rID, size_t uiField,
                      size_t uiCol, wxPoint ptPos,
                      size_t uiRank, bool bAddOrRemove,
                      bool bSelectRange);                               ///< \brief edit the focused item
        bool CancelCurrentEdition(void);                                ///< \brief cancel the current edition
        bool EndCurrentEdition(bool bCancel = false);                   ///< \brief finish the current edition
        bool EndCurrentEdition(const wxVirtualItemID &rID,
                               size_t uiField, size_t uiCol,
                               bool bCancel = false);                   ///< \brief finish the current edition
        bool EditFocusedItem(const wxVirtualItemID &rID,
                             size_t uiField, size_t uiCol);             ///< \brief edit the focus item
        virtual wxVirtualDataViewEditor*
                    StartEdition(const wxVirtualItemID &rID,
                                  size_t uiField, size_t uiCol);        ///< \brief start editing an item
        virtual bool EndEdition(wxVirtualDataViewEditor *pEditor,
                                const wxVirtualItemID &rID,
                                size_t uiField, size_t uiCol,
                                bool bCancel = false);                  ///< \brief end editing an item
        virtual bool ChangeItemValue(const wxVirtualItemID &rID,
                                     size_t uiField, size_t uiCol,
                                     const wxVariant &rvValue);         ///< \brief change the value of an item
        virtual bool ActivateCell(const wxVirtualItemID &rID,
                                  size_t uiField, size_t uiCol,
                                  wxPoint ptPos,
                                  size_t uiRank, bool bAddOrRemove,
                                  bool bSelectRange);                   ///< \brief activate a cell
        virtual bool CharHookEvent(wxKeyEvent &rEvent);                 ///< \brief handle a char hook event

        //protected interface
        virtual bool DrawBackground(wxDC &rDC);                         ///< \brief draw the background
        virtual void DrawItem(wxDC &rDC, wxRect &rRect,
                              const wxVirtualItemID &rID,
                              const wxVirtualDataViewItemState &rState,
                              wxVirtualIDataModel *pModel,
                              long lLevel, size_t uiRank,
                              size_t uiCount);                          ///< \brief draw one item
        virtual void DrawTreeLines(wxDC &rDC, wxRect &rRect,
                                   const wxVirtualItemID &rID,
                                   wxVirtualIDataModel *pModel,
                                   long lLevel);                        ///< \brief draw tree lines
        virtual void DrawTreeButton(wxDC &rDC, wxRect &rRect,
                                    bool bExpanded);                    ///< \brief draw a tree button
        virtual void DrawCheckBox(wxDC &rDC, wxRect &rRect, int iState);///< \brief draw the checkbox
        virtual wxSize GetItemSize(const wxVirtualItemID &rID) const;   ///< \brief get the size of 1 item
        virtual bool IsExpanderColumn(size_t uiViewCol) const;          ///< \brief check if the column is the expander column

        //painting
        void ComputeBackgroundColours(void);                            ///< \brief compute background colours


        //debug
        void PrintItemID(const wxVirtualItemID &rID, size_t uiLevel,
                         const char *szMsg1, int iData1,
                         const char *szMsg2 = WX_VDV_NULL_PTR,
                         int iData2 = 0) const;                         ///< \brief debug method (print item ID)
};

#endif

