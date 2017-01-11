/**********************************************************************/
/** FILE    : VirtualTreeModelRenderer.cpp                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : partial implementation of wxVirtualIModelRenderer      **/
/**********************************************************************/

#include <wx/VirtualDataView/ModelRenderer/VirtualTreeModelRenderer.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewBase.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/Columns/VirtualDataViewColumnsList.h>
#include <wx/stack.h>
#include <wx/hashset.h>
#include <wx/settings.h>
#include <wx/log.h>
#include <wx/renderer.h>
#include <wx/dcmemory.h>
#include <iostream>


#define LOG_SCROLLBAR       0
#if LOG_SCROLLBAR != 0
    #define PRINT_SCROLLBAR(x)      x
#else
    #define PRINT_SCROLLBAR(x)
#endif // LOG_SCROLLBAR

#define LOG_PAINT_TIMER     0
#if LOG_PAINT_TIMER != 0

    #include <windows.h>
    static DWORD dwTicks[15];

    #define LOG_PAINT_TIME(iTimer)  dwTicks[iTimer] = ::GetTickCount();
    #define LOG_PAINT_TIME_PRINT(i1, i2, sMsg)  wxLogMessage("Time %s = %d ms", sMsg, dwTicks[i2] - dwTicks[i1]);

#else
    #define LOG_PAINT_TIME(iTimer)
    #define LOG_PAINT_TIME_PRINT(i1, i2, sMsg)
#endif

//-------------------- CONSTRUCTORS & DESTRUCTOR --------------------//
/** Constructor
  * \param pClient [input]: the client window
  * \param pScroller [input]: the window with the scrollbars
  */
wxVirtualTreeModelRenderer::wxVirtualTreeModelRenderer(wxVirtualDataViewBase* pClient,
                                                       wxVirtualDataViewCtrl* pScroller)
    : wxVirtualIModelRenderer(pClient, pScroller)
{
    Init();
}

/** Destructor
  */
wxVirtualTreeModelRenderer::~wxVirtualTreeModelRenderer(void)
{

}

//------- INTERFACE IMPLEMENTATION : EVENT HANDLING -----------------//
/** Paint the model
  * \param rDC [input] : the device context to use
  * \return true on success, false on failure (painting incomplete or not done at all)
  */
bool wxVirtualTreeModelRenderer::PaintModel(wxDC &rDC)
{
    //information : measuring items size and drawing them are both
    //              time consuming
    //  1st optimisation : uniform row height : it will allow to scan the tree very fast
    //                     store the 1st visible ROW (scrollbar position)
    //                     scan from root node
    //                     start to draw when current row = first row. End drawing when
    //                     client size height is drawn
    //
    //  2nd optimisation : store the tree path to the 1st visible item, and
    //                     update it at each scrolling / insertion / deletion / expand / collapse
    //                     Drawing can now start quickly without scanning from root node

    LOG_PAINT_TIME(0);

    //check & init
    if (!m_pClient) return(false);

    //background
    ComputeBackgroundColours();
    DrawBackground(rDC);
    LOG_PAINT_TIME(1);

    //get models
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //init dc
    rDC.SetDeviceOrigin(0, 0);

    //client size
    int w = 0;
    int h = 0;
    m_pClient->GetClientSize(&w, &h);
    wxCoord xMax = w;
    wxCoord wMax = w - m_iMarginLeft - m_iMarginRight;

    //compute vertical margin
    int iVerticalMargin = m_iMarginTop + m_iMarginBottom;
    int iPenWidth       = m_HorizontalRulesPen.GetWidth();
    int iPenWidthUp     = iPenWidth / 2;
    int iPenWidthDown   = iPenWidth - iPenWidthUp;
    if (!m_bDrawHoritontalRules) iPenWidth = 0;
    int iVerticalSize   = iVerticalMargin + iPenWidth;

    //item size
    int iHeight = -1;
    if (m_bUniformRowHeight)
    {
        iHeight = GetUniformRowHeight();
    }

    //1st item
    m_FirstVisibleItem.SetDataModel(pDataModel);
    wxVirtualItemID id = m_FirstVisibleItem.Get();
    size_t uiLevel     = m_FirstVisibleItem.GetLevel();
    wxCoord x = 0;
    wxCoord y = 0;

    //state
    wxVirtualDataViewItemState oState;
    if (!pStateModel) oState.SetDefault();

    //deal with root item
    if (m_bHideRootItem)
    {
        if (uiLevel > 0) uiLevel--;
        if (pDataModel->IsRootItem(id))
        {
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
            if (uiLevel > 0) uiLevel--;
        }
    }

    //save
    wxVirtualItemID idStart = id;
    size_t uiStartLevel = uiLevel;

    //horizontal scroll bar
    if (!m_pScroller) return(false);
    int x0 = m_pScroller->GetScrollPos(wxHORIZONTAL);
    LOG_PAINT_TIME(2);

    //draw items
    size_t uiCount = 0;
    size_t uiRank = m_pScroller->GetScrollPos(wxVERTICAL);
    while (y < h)
    {
        if (!id.IsOK()) break;

        //state
        if (pStateModel) pStateModel->GetState(oState, id);

        //paint the item
        if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();
        wxRect r(x   + m_iMarginLeft - x0, y + m_iMarginTop,
                 wMax + x0, iHeight);
        DrawItem(rDC, r, id, oState, pDataModel, uiLevel, uiRank, uiCount);

        //next
        uiRank++;
        uiCount++;
        y += iHeight;
        y += iVerticalSize; //margin + horizontal grid pen

        //next item
        id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
    }
    LOG_PAINT_TIME(3);

    //draw horizontal grid lines
    if (m_bDrawHoritontalRules)
    {
        id      = idStart;
        uiLevel = uiStartLevel;

        int iOffset = m_HorizontalRulesPen.GetWidth() - m_HorizontalRulesPen.GetWidth() / 2;
        y = -iOffset;

        rDC.SetPen(m_HorizontalRulesPen);

        while(y < h)
        {
            if (!id.IsOK()) break;

            //measure item
            if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();

            //compute
            y += iHeight + iVerticalMargin;

            //grid line
            y += iPenWidthUp;
            rDC.DrawLine(0, y, xMax + x0, y);
            y += iPenWidthDown;

            //next item
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
        }
    }
    LOG_PAINT_TIME(4);

    //draw vertical grid lines
    if (m_bDrawVerticalRules)
    {
        wxVirtualDataViewColumnsList &rvColumns = Columns();
        size_t uiNbCols = rvColumns.GetColumnsCount();
        size_t uiCol;

        rDC.SetPen(m_VerticalRulesPen);

        int iOffset = m_VerticalRulesPen.GetWidth() - m_VerticalRulesPen.GetWidth() / 2;
        int x = -x0 - iOffset;
        for(uiCol = 0; uiCol < uiNbCols; uiCol++)
        {
            wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(uiCol);
            if (!pCol) continue;
            if (pCol->IsHidden()) continue;

            x += pCol->GetWidth();
            rDC.DrawLine(x, 0, x, h);
        }
    }
    LOG_PAINT_TIME(5);

    //tree lines, tree buttons, checkboxes : clip to the expander column
    int iStartExpanderCol = 0;
    int iWidthExpanderCol = w;
    size_t uiNbCols = Columns().GetColumnsCount();
    size_t uiCol;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        if (IsExpanderColumn(uiCol))
        {
            iStartExpanderCol = Columns().GetColumnStart(uiCol);
            iWidthExpanderCol = Columns().GetColumnWidth(uiCol);
            break;
        }
    }
    wxRect rExpanderCol(iStartExpanderCol, 0, iWidthExpanderCol, h);
    wxDCClipper dcc(rDC, rExpanderCol);

    //draw tree lines
    if (m_eTreeLinesStyle != E_TREE_LINES_NONE)
    {
        y = 0;
        id      = idStart;
        uiLevel = uiStartLevel;

        rDC.SetPen(m_TreeLinesPen);

        while(y < h)
        {
            if (!id.IsOK()) break;

            //state
            if (pStateModel) pStateModel->GetState(oState, id);

            //paint the item
            if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();
            wxRect r(x   + m_iMarginLeft - x0, y + m_iMarginTop,
                     wMax + x0, iHeight);
            DrawTreeLines(rDC, r, id, pDataModel, uiLevel);

            //next
            y += iHeight;
            y += iVerticalSize; //margin + horizontal grid pen

            //next item
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
        }
    }
    LOG_PAINT_TIME(6);

    //draw tree buttons
    if (m_bDrawTreeButtons)
    {
        y = 0;
        id      = idStart;
        uiLevel = uiStartLevel;

        while(y < h)
        {
            if (!id.IsOK()) break;

            //state
            if (pStateModel) pStateModel->GetState(oState, id);

            //paint the item
            if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();
            wxRect r(x   + m_iMarginLeft - x0, y + m_iMarginTop,
                     wMax + x0, iHeight);
            r.x += uiLevel * m_iIndentation;

            if (pDataModel->HasExpander(id))
            {
                DrawTreeButton(rDC, r, oState.IsExpanded());
            }

            //next
            y += iHeight;
            y += iVerticalSize; //margin + horizontal grid pen

            //next item
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
        }
    }
    LOG_PAINT_TIME(7);

    //draw check boxes
    if (m_bDrawCheckbox)
    {
        y = 0;
        id      = idStart;
        uiLevel = uiStartLevel;

        while(y < h)
        {
            if (!id.IsOK()) break;

            //state
            if (pStateModel) pStateModel->GetState(oState, id);

            //paint the item
            if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();
            wxRect r(x   + m_iMarginLeft - x0, y + m_iMarginTop,
                     wMax + x0, iHeight);
            r.x += uiLevel * m_iIndentation;

            if (pDataModel->HasCheckbox(id))
            {
                DrawCheckBox(rDC, r, oState.GetCheckState());
            }

            //next
            y += iHeight;
            y += iVerticalSize; //margin + horizontal grid pen

            //next item
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
        }
    }
    LOG_PAINT_TIME(8);
    LOG_PAINT_TIME_PRINT(0, 1, "background");
    LOG_PAINT_TIME_PRINT(1, 2, "setup");
    LOG_PAINT_TIME_PRINT(2, 3, "items");
    LOG_PAINT_TIME_PRINT(3, 4, "horizontal lines");
    LOG_PAINT_TIME_PRINT(4, 5, "vertical lines");
    LOG_PAINT_TIME_PRINT(5, 6, "tree lines");
    LOG_PAINT_TIME_PRINT(6, 7, "tree buttons");
    LOG_PAINT_TIME_PRINT(7, 8, "checkboxes");
    LOG_PAINT_TIME_PRINT(0, 8, "total");
    return(true);
}

/** Erase background event
  * \param rEvent [input]: the event data
  */
bool wxVirtualTreeModelRenderer::EraseBackgroundEvent(wxEraseEvent &rEvent)
{
    return(true);
}

/** A size event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::SizeEvent(wxSizeEvent &rEvent)
{
    UpdateScrollbars();
    if (m_pClient) m_sCurrentClientSize = m_pClient->GetClientSize();
    else           m_sCurrentClientSize.Set(0, 0);

    //if there is only 1 column, make it the size of the whole client width
    wxVirtualDataViewColumnsList &rvColumns = Columns();
    if (rvColumns.GetColumnsCount() == 1)
    {
        if (rvColumns.GetTotalWidth() < m_sCurrentClientSize.GetWidth())
        {
            rvColumns.GetColumn(0)->SetWidth(m_sCurrentClientSize.GetWidth());
        }
    }

    return(true);
}

/** A focus event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::FocusEvent(wxFocusEvent &rEvent)
{
    m_bHasFocus = rEvent.GetEventType() == wxEVT_SET_FOCUS;
    if (!m_bHasFocus)
    {
        //when losing focus, de-focus current item
        //set focus item (here it is set to an invalid item. This is not a mistake. This
        //is to avoid edition of an item after clicking outside the window. Example :
        //      1 - select Item A => focused item = Item A
        //      2 - click outside the window => focused item is still Item A
        //      3 - select Item A again => edition of Item A will be triggered, which is not what the user expects
        // So we need to break Step 2)
        wxVirtualItemID id;
        SetItemFocus(id, 0, 0, 0, true);
    }

     //wxLogMessage("wxVirtualTreeModelRenderer::FocusEvent %s", (m_bHasFocus ? "Get" : "Lose"));
//    if ((m_bHasFocus) && (m_bIsEditing) && (m_pEditor))
//    {
//        wxWindow *pWin = m_pEditor->GetEditorWindow();
//        if (pWin)
//        {
//            wxLogMessage("wxVirtualTreeModelRenderer::FocusEvent on editor");
//            m_bFocusGuard = true; //to indicate that we just set the focus to the editor window
//            pWin->SetFocus(); //this will send a lost focus event -> the method wxVirtualTreeModelRenderer::FocusEvent will be called again
//        }
//    }
//    else if (!m_bHasFocus)
//    {
//        if (m_bFocusGuard)  m_bFocusGuard = false;  //do not end the edition: we just gave it the focus !
//        else                EndCurrentEdition(false);
//    }

    return(true);
}

/** A scroll event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::ScrollEvent(wxScrollWinEvent &rEvent)
{
    if (!m_pScroller) return(false);
    if (!m_pClient)   return(false);

    //get current orientation & position
    int iOrientation = rEvent.GetOrientation();
    int iPosition    = m_pScroller->GetScrollPos(iOrientation);
    int iPageSize    = m_pScroller->GetScrollThumb(iOrientation);

    //update scroll position
    if (  (rEvent.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK)
        ||(rEvent.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK))
    {
        iPosition = rEvent.GetPosition();
        m_pScroller->SetScrollPos(iOrientation, iPosition, true);
    }
    else if (rEvent.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN)
    {
        iPosition++;
        m_pScroller->SetScrollPos(iOrientation, iPosition, true);
    }
    else if (rEvent.GetEventType() == wxEVT_SCROLLWIN_LINEUP)
    {
        iPosition--;
        m_pScroller->SetScrollPos(iOrientation, iPosition, true);
    }
    else if (rEvent.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN)
    {
        iPosition += iPageSize;
        m_pScroller->SetScrollPos(iOrientation, iPosition, true);
    }
    else if (rEvent.GetEventType() == wxEVT_SCROLLWIN_PAGEUP)
    {
        iPosition -= iPageSize;
        m_pScroller->SetScrollPos(iOrientation, iPosition, true);
    }

    if (iOrientation == wxVERTICAL)
    {
        if (iPosition < 0) iPosition = 0;
        int iRange      = m_iNbVisiblesItems; //m_pScroller->GetScrollRange(wxVERTICAL);
        int iThumbSize  = m_pScroller->GetScrollThumb(wxVERTICAL);
        if (iPosition >= iRange - iThumbSize) iPosition = iRange - iThumbSize;
        ComputeFirstVisibleItem(iPosition);
        PRINT_SCROLLBAR(wxLogMessage("position = %d", iPosition));
    }

    ScrollEditorWindow();
    return(true);
}

/** A mouse event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::MouseEvent(wxMouseEvent &rEvent)
{
    //handled and not skipped in user code :
    //DO not do it because otherwise a stack overflow would occur
    //This method is user code
    if (!m_pClient) return(false);
    //if (m_pClient->GetEventHandler()->ProcessEvent(rEvent)) return(true);

    if (!m_pScroller) return(false);

    //right click : skip (context menu)
    if ((rEvent.RightIsDown()) || (rEvent.RightUp()))
    {
        return(false);
    }

    //mouse wheel
    if (rEvent.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        int iOrientation = wxVERTICAL;
        if (rEvent.GetWheelAxis() == wxMOUSE_WHEEL_HORIZONTAL)
        {
            iOrientation = wxHORIZONTAL;
        }
        int iPosition    = m_pScroller->GetScrollPos(iOrientation);

        int iPos = rEvent.GetWheelRotation() / rEvent.GetWheelDelta();
        if (iPos != 0)
        {
            iPosition -= iPos;
            if (iPosition < 0) iPosition = 0;
            m_pScroller->SetScrollPos(iOrientation, iPosition, true);
        }

        if (iOrientation == wxVERTICAL)
        {
            ComputeFirstVisibleItem(iPosition);
            wxScrollWinEvent event(wxEVT_SCROLLWIN_THUMBRELEASE, iPosition, wxVERTICAL);
            m_pClient->SendScrollEvent(event);
        }

        Refresh();
        return(true);
    }

    //basic check
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //hit test first - if nothing is hit, continue, because it can means de-select all
    THitTest ht;
    HitTest(ht, rEvent.GetX(), rEvent.GetY(), true);

    //mouse cursor
    if ((ht.m_uiFlags & E_HIT_TEST_ON_LEFT_EDGE) || (ht.m_uiFlags & E_HIT_TEST_ON_RIGHT_EDGE))
    {
        m_pClient->SetCursor(wxCURSOR_SIZEWE);
    }
    else
    {
        m_pClient->SetCursor(wxNullCursor);
    }

    //dragging column separator or double click on column separator
    if (ResizeColumn(ht, rEvent)) return(true);

    //tooltip & help text
    SetToolTip(ht);

    //process the event
    if (m_pClient->SendEventMouseEvent(ht, rEvent)) return(true);

    //shortcuts
    bool bLeftDown   = rEvent.LeftDown();
    bool bLeftDClick = rEvent.LeftDClick();
    bool bLeftClick  = bLeftDown || bLeftDClick;

    //set focus
    //if (bLeftDown) m_pClient->SetFocus();

    //expand / collapse
    //double click expand/collapse recursively
    if ((ht.m_uiFlags & E_HIT_TEST_ON_EXPANDER) && (bLeftClick))
    {
        bool bRecursive = bLeftDClick || rEvent.ShiftDown();
        bool bHasExpander = false;
        wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
        if (pDataModel) bHasExpander = pDataModel->HasExpander(ht.m_ID);
        if (bHasExpander)
        {
            bool bSendEvent = true;
            if (pStateModel->IsExpanded(ht.m_ID)) return(Collapse(ht.m_ID, true, bRecursive, true, ht.m_uiRank, bSendEvent));
            return(Expand(ht.m_ID, true, bRecursive, true, ht.m_uiRank, bSendEvent));
        }
    }

    //checkbox
    if (m_bDrawCheckbox)
    {
        if ((ht.m_uiFlags & E_HIT_TEST_ON_CHECKBOX) && (bLeftClick) && (m_bDrawCheckbox))
        {
            bool bHasCheckBox = false;
            wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
            if (pDataModel) bHasCheckBox =   (pDataModel->HasCheckbox(ht.m_ID))
                                          && (pDataModel->IsCheckable(ht.m_ID));
            if (bHasCheckBox)
            {
                if (!m_pClient) return(false);
                if (!m_pClient->SendEventChecking(ht.m_ID)) return(false);
                wxVirtualIStateModel::ECheckState eState = pStateModel->GetCheck(ht.m_ID);
                switch(eState)
                {
                    case wxVirtualIStateModel::WX_CHECKSTATE_CHECKED        :
                        eState = wxVirtualIStateModel::WX_CHECKSTATE_UNCHECKED;
                        break;

                    case wxVirtualIStateModel::WX_CHECKSTATE_UNCHECKED      :
                    case wxVirtualIStateModel::WX_CHECKSTATE_UNDETERMINED   :
                    default                                                 :
                        eState = wxVirtualIStateModel::WX_CHECKSTATE_CHECKED;
                        break;
                }
                pStateModel->SetCheck(ht.m_ID, eState, false);
                m_pClient->SendEventChecked(ht.m_ID);
                Refresh();
                return(true);
            }
        }
    }

    //activate item on double click
    if (bLeftDClick)
    {
        ActivateItem(ht.m_ID, ht.m_uiField, ht.m_uiColIndex, true, true);
        return(true);
    }

    //edit : allow edition on left click (but not double click) on the focused item
    //       end edition when a click outside of the focused cell has been detected
    if (bLeftDown)
    {
        //double click are not tested because they are already swallowed by the
        //item activation

        //test is sequence:
        //  - if edition is currently done, try to finish it
        //  - if edition of a new item can be started
        //  - update selection

        //edition
        wxPoint ptPos(ht.m_RelativeX, ht.m_RelativeY);
        if (EditItem(pStateModel, ht.m_ID, ht.m_uiField,
                     ht.m_uiColIndex, ptPos,
                     ht.m_uiRank, rEvent.ControlDown(), rEvent.ShiftDown()))
        {
            return(true);
        }

        //on button down, let the OS perform basic handling, like setting focus
        if (bLeftDown) rEvent.Skip();

        //selection / deselection
        bool bSendEvent = true;
        if (PerformSelection(ht.m_ID, ht.m_uiField, ht.m_uiColIndex, ht.m_uiRank,
                             rEvent.ControlDown(),
                             rEvent.ShiftDown(), bSendEvent));
        {
            return(true);
        }
    }

    return(true);
}

/** A keyboard event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::KeyboardEvent(wxKeyEvent &rEvent)
{
    //basic checks
    if (!m_pClient) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    wxVirtualIDataModel *pDataModel;

    //user code handling
    wxVirtualItemID idFocused = pStateModel->GetFocusedItem();
    if (m_pClient->SendKeyboardEvent(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem, rEvent))
    {
        return(true);
    }

    //invalid focused item : do nothing
    if (!idFocused.IsOK()) return(true);

    //flags
    bool bControlDown = rEvent.ControlDown();
    bool bShiftDown   = rEvent.ShiftDown();

    //handle only char events or char_hooks
    if (rEvent.GetEventType() == wxEVT_CHAR_HOOK) return(CharHookEvent(rEvent));
    if (rEvent.GetEventType() != wxEVT_CHAR) return(false);

    //vertical scrollbar data
    if (!m_pScroller) return(false);
    size_t uiScrollPos      = m_pScroller->GetScrollPos(wxVERTICAL);
    size_t uiScrollPageSize = m_pScroller->GetScrollThumb(wxVERTICAL);
    size_t uiLast           = m_iNbVisiblesItems; //m_pScroller->GetScrollRange(wxVERTICAL);
    size_t uiFirst          = 0;
    if (m_bHideRootItem) uiFirst++;
    if (uiScrollPageSize > 0) uiScrollPageSize--;

    size_t uiTopItemRank = uiScrollPos;
    if (m_bHideRootItem) uiTopItemRank++;

    //switch
    wxVirtualItemIDs vIDsOld, vIDsNew;
    size_t uiRank;
    switch(rEvent.GetKeyCode())
    {
        // '+': Expand subtree
        case '+'            :
        case WXK_ADD        :   if (!pStateModel->IsExpanded(idFocused))
                                {
                                    Expand(idFocused, true, false, true);
                                }
                                EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                break;

        // '-': collapse subtree
        case '-'            :
        case WXK_SUBTRACT   :   if (pStateModel->IsExpanded(idFocused))
                                {
                                    Collapse(idFocused, true, false, true);
                                }
                                EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                break;

        // space : toggle selection of the item
        case ' '            :   if (!ActivateCell(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem,
                                                  wxPoint(-1, -1), m_uiRankFocusedItem, bControlDown, bShiftDown))
                                {
                                    SelectItem(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem,
                                               true, false, true);
                                }
                                MakeItemVisible(idFocused, m_uiColFocusedItem, WX_VISIBLE_DONT_CARE);
                                break;

        //return: activate the item
        case WXK_RETURN     :   ActivateCell(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem,
                                             wxPoint(-1, -1), m_uiRankFocusedItem, bControlDown, bShiftDown);
                                ActivateItem(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem,
                                             true, true);
                                MakeItemVisible(idFocused, m_uiColFocusedItem, WX_VISIBLE_DONT_CARE);
                                break;

        //move up
        case WXK_HOME       :   MoveTo(uiFirst, bControlDown, bShiftDown); break;
        case WXK_UP         :   if (m_uiRankFocusedItem > 0)
                                {
                                    pDataModel = m_pClient->GetDataModel();
                                    if (!pDataModel) break;
                                    idFocused = pDataModel->PrevItem(idFocused, pStateModel, 1);
                                    if (idFocused.IsInvalid())
                                    {
                                        idFocused = pStateModel->GetFocusedItem();
                                        EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                        break;
                                    }
                                    MoveTo(idFocused, m_uiRankFocusedItem - 1, bControlDown, bShiftDown);
                                }
                                else
                                {
                                    EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                }
                                break;
        case WXK_PAGEUP     :   if (m_uiRankFocusedItem > uiScrollPageSize)
                                {
                                    if (m_uiRankFocusedItem <= uiTopItemRank)
                                    {
                                        //move 1 page up
                                        pDataModel = m_pClient->GetDataModel();
                                        if (!pDataModel) break;
                                        idFocused = pDataModel->PrevItem(idFocused, pStateModel, uiScrollPageSize);
                                        if (idFocused.IsInvalid())
                                        {
                                            idFocused = pStateModel->GetFocusedItem();
                                            EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                            break;
                                        }
                                        MoveTo(idFocused, m_uiRankFocusedItem - uiScrollPageSize,
                                               bControlDown, bShiftDown);
                                    }
                                    else
                                    {
                                        //move to the top of the page
                                        MoveTo(uiScrollPos, bControlDown, bShiftDown);
                                    }
                                }
                                else
                                {
                                    MoveTo(uiFirst, bControlDown, bShiftDown);
                                }
                                break;

        //move down
        case WXK_END        :   MoveTo(uiLast, bControlDown, bShiftDown); break;
        case WXK_DOWN       :   if (m_uiRankFocusedItem != uiLast)
                                {
                                    pDataModel = m_pClient->GetDataModel();
                                    if (!pDataModel) break;
                                    idFocused = pDataModel->NextItem(idFocused, pStateModel, 1);
                                    if (idFocused.IsInvalid())
                                    {
                                        idFocused = pStateModel->GetFocusedItem();
                                        EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                        break;
                                    }
                                    MoveTo(idFocused, m_uiRankFocusedItem + 1, bControlDown, bShiftDown);
                                }
                                else
                                {
                                    EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                }
                                break;
        case WXK_PAGEDOWN   :   if (m_uiRankFocusedItem >= uiLast)
                                {
                                    EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                    break;
                                }
                                if (m_uiRankFocusedItem + uiScrollPageSize >= uiLast)
                                {
                                    MoveTo(uiLast, bControlDown, bShiftDown);
                                }
                                else
                                {
                                    pDataModel = m_pClient->GetDataModel();
                                    if (!pDataModel) break;
                                    idFocused = pDataModel->NextItem(idFocused, pStateModel, uiScrollPageSize);
                                    if (idFocused.IsInvalid())
                                    {
                                        idFocused = pStateModel->GetFocusedItem();
                                        EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                        break;
                                    }
                                    MoveTo(idFocused, m_uiRankFocusedItem + uiScrollPageSize,
                                           bControlDown, bShiftDown);
                                }
                                break;

        //move up & collapse
        case WXK_BACK       :   //move to parent without collapsing
                                pDataModel = m_pClient->GetDataModel();
                                if (!pDataModel) break;

                                idFocused = pDataModel->GetParent(idFocused);
                                uiRank = pDataModel->GetItemRank(idFocused, pStateModel);
                                MoveTo(idFocused, uiRank, bControlDown, bShiftDown);
                                break;

        case WXK_LEFT       :   //collapse if possible, otherwise go up
                                if (pStateModel->IsExpanded(idFocused))
                                {
                                    Collapse(idFocused, true, false, true);
                                    EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                }
                                else
                                {
                                    //move to parent
                                    pDataModel = m_pClient->GetDataModel();
                                    if (!pDataModel) break;

                                    idFocused = pDataModel->GetParent(idFocused);
                                    uiRank = pDataModel->GetItemRank(idFocused, pStateModel);
                                    MoveTo(idFocused, uiRank, bControlDown, bShiftDown);
                                }
                                break;

        case WXK_RIGHT      :   //expand if possible, otherwise go down to 1st child
                                if (!pStateModel->IsExpanded(idFocused))
                                {
                                    Expand(idFocused, true, false, true);
                                    EnsureItemVisible(idFocused, m_uiColFocusedItem);
                                }
                                else
                                {
                                    pDataModel = m_pClient->GetDataModel();
                                    if (!pDataModel) break;
                                    if (pDataModel->HasExpander(idFocused))
                                    {
                                        idFocused = pDataModel->GetChild(idFocused, 0);
                                        MoveTo(idFocused, m_uiRankFocusedItem + 1, bControlDown, bShiftDown);
                                    }
                                }
                                break;

        //default: do nothing
        default             :   break;
    }

    return(true);
}

/** Handle a char hook event
  * \param rEvent [input]: event information
  */
bool wxVirtualTreeModelRenderer::CharHookEvent(wxKeyEvent &rEvent)
{
    if ((!m_bIsEditing) || (!m_pEditor)) return(false);

    switch(rEvent.GetKeyCode())
    {
        case WXK_ESCAPE         : if (CancelCurrentEdition())
                                  {
                                        if (m_pClient) m_pClient->SetFocus();
                                        Refresh();
                                        return(true);
                                  }
                                  break;

        case WXK_TAB            : if (rEvent.AltDown()) break;      //ignore ALT-TAB
                                  if (EndCurrentEdition())
                                  {
                                        if (m_pClient) m_pClient->SetFocus();
                                        Refresh();
                                        return(true);
                                  }
                                  break;

        case WXK_RETURN         : if (rEvent.ControlDown()) break; //ignore CTRL+RETURN
                                  if (rEvent.AltDown()) break;     //ignore ALT+RETURN
        case WXK_NUMPAD_ENTER   : if (EndCurrentEdition())
                                  {
                                        if (m_pClient) m_pClient->SetFocus();
                                        Refresh();
                                        return(true);
                                  }
                                  break;

        default                 : break;
    }

    return(false);
}

/** Start the item edition
  * \param rID   [input]: the ID of the item to edit
  * \param uiCol [input]: the column of the item to edit
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::StartEdition(const wxVirtualItemID &rID, size_t uiCol)
{
    //cancel current edition
    if (!EndCurrentEdition(true)) return(false);
    Refresh();

    //basic check
    if (!m_pClient) return(false);
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //get field
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(uiCol);
    if (!pCol) return(false);
    size_t uiField = pCol->GetModelColumn();

    //get rank
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);
    size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);

    //set the item as focused
    SetItemFocus(rID, uiField, uiCol, uiRank, true);

    //edition
    if (EditFocusedItem(rID, uiField, uiCol))
    {
        PerformSelection(rID, uiField, uiCol, uiRank, false, false, true, true);
        return(true);
    }
    return(false);
}

/** Provokes the end of the current edition. This notification comes from the editor itself
  * \param bCancelled [input]: true if the edition was cancelled, false otherwise
  * \return true if the event was processed, false otherwise
  */
bool wxVirtualTreeModelRenderer::FinishEdition(bool bCancelled)
{
    if (EndCurrentEdition(bCancelled))
    {
        if (m_pClient) m_pClient->SetFocus();
        Refresh();
        return(true);
    }
    return(false);
}

/** A context menu event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::ContextMenuEvent(wxContextMenuEvent &rEvent)
{
    if (!m_pClient) return(false);

    int x = rEvent.GetPosition().x;
    int y = rEvent.GetPosition().y;
    m_pClient->ScreenToClient(&x, &y); //for context menu, coordinates are in screen coordinates !

    THitTest ht;
    HitTest(ht, x, y, true);

    if (!m_pClient->SendContextMenuEvent(ht.m_ID, ht.m_uiField, ht.m_uiColIndex, x, y)) return(false);
    return(true);
}

/** A help event occurred
  * \param rEvent [input]: the event information
  * \return true if the event was processed and not skipped, false otherwise
  */
bool wxVirtualTreeModelRenderer::HelpEvent(wxHelpEvent &rEvent)
{
    //basic checks
    if (!m_pClient) return(false);
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    //hit test
    int x = rEvent.GetPosition().x;
    int y = rEvent.GetPosition().y;

    THitTest ht;
    HitTest(ht, x, y, true);

    //set help data
    if (ht.m_uiField & E_HIT_TEST_OUTSIDE_ITEM)
    {
        m_pClient->SetHelpText("");
    }
    else
    {
        //get context help
        wxVariant vHelpText;
        vHelpText = pDataModel->GetItemData(ht.m_ID, ht.m_uiField,
                                            wxVirtualIDataModel::WX_ITEM_HELP_DATA
                                           );
        if (vHelpText.IsNull()) m_pClient->SetHelpText("");
        else                    m_pClient->SetHelpText(vHelpText.GetString());
    }

    return(true);
}

//----------------- INTERFACE IMPLEMENTATION : SIZING ---------------//
/** Get the best size
  * \return the best client size for showing the full model
  */
wxSize wxVirtualTreeModelRenderer::GetBestSize(void) const
{
    size_t uiNbItems = 0;
    return(ComputeSize(uiNbItems));
}

/** Get the best size of a column
  * \param uiColID [input] : the column ID to measure (view coord)
  * \return the best size of the column
  */
wxSize wxVirtualTreeModelRenderer::GetBestSize(size_t uiColID) const
{
    //check & init
    if (!m_pClient) return(wxSize(1, 1));

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(wxSize(1, 1));

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(wxSize(1, 1));

    //get margins
    int iHorizMargin = m_iMarginLeft + m_iMarginRight;
    int iVertMargin  = m_iMarginTop + m_iMarginBottom;
    if (m_bDrawHoritontalRules) iVertMargin += m_VerticalRulesPen.GetWidth();

    //compute size
    //if all sizes are uniform, the calculation is faster
    wxSize sResult(0, 0);
    long lChild, lNbChildren;
    wxVirtualItemID idRoot = pDataModel->GetRootItem();

    //brute force calculation unavoidable
    wxStack<wxVirtualItemID> StackOfNodes;
    StackOfNodes.push(idRoot);
    while(StackOfNodes.size() > 0)
    {
        //get item
        wxVirtualItemID id = StackOfNodes.top();
        StackOfNodes.pop();

        //compute size
        wxSize s = GetItemSize(id, uiColID);

        //add margins
        if (!pDataModel->IsRootItem(id))
        {
            s.x += iHorizMargin;
            s.y += iVertMargin;
        }

        //accumulate (max value horizontal + accumulate vertically)
        if (s.x > sResult.x) sResult.x = s.x;
        sResult.y += s.y;

        //recurse
        if (!pStateModel->IsExpanded(id)) continue;
        lNbChildren = pDataModel->GetChildCount(id);
        for(lChild = lNbChildren - 1; lChild >= 0; lChild--)
        {
            wxVirtualItemID idChild = pDataModel->GetChild(id, lChild);
            StackOfNodes.push(idChild);
        }
    }

    return(sResult);
}


/** Performs a hit test
  * \param rResult         [output]: the result of the hit test
  * \param x               [input] : the x coordinate of the point to test
  * \param y               [input] : the y coordinate of the point to test
  * \param bClientAreaOnly [input] : true for performing the test only on visible item
  *                                  false for performing the test on all items
  * \return true if an item was found, false otherwise
  */
bool wxVirtualTreeModelRenderer::HitTest(THitTest &rResult, wxCoord x, wxCoord y,
                                         bool bClientAreaOnly)
{
    //init
    rResult.m_uiFlags               = 0;
    rResult.m_x                     = x;
    rResult.m_y                     = y;
    rResult.m_RelativeX             = 0;
    rResult.m_RelativeY             = 0;
    rResult.m_ID.SetClientData(WX_VDV_NULL_PTR);
    rResult.m_uiField               = 0;
    rResult.m_uiColIndex            = 0;
    rResult.m_RectItem.x            = 0;
    rResult.m_RectItem.y            = 0;
    rResult.m_RectItem.width        = 0;
    rResult.m_RectItem.height       = 0;
    rResult.m_RectSubItem.x         = 0;
    rResult.m_RectSubItem.y         = 0;
    rResult.m_RectSubItem.width     = 0;
    rResult.m_RectSubItem.height    = 0;
    rResult.m_uiRank                = 0;

    //check
    if (!m_pClient) return(false);
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();

    //state
    wxVirtualDataViewItemState oState;
    if (!pStateModel) oState.SetDefault();

    //margins
    int iMargins = m_iMarginBottom + m_iMarginTop;
    if (m_bDrawHoritontalRules) iMargins += m_HorizontalRulesPen.GetWidth();

    //uniform row height
    int iHeightTotal = 0;
    int iHeight = 0;
    if (m_bUniformRowHeight)
    {
        iHeight = GetUniformRowHeight();
        iHeightTotal = iHeight + iMargins;
    }

    //1st item to scan
    m_FirstVisibleItem.SetDataModel(pDataModel);
    wxVirtualItemID id = m_FirstVisibleItem.Get();
    size_t uiLevel     = m_FirstVisibleItem.GetLevel();
    int w              = 0;
    int h              = 0;
    wxCoord yy         = 0;

    //client area or full
    if (bClientAreaOnly)
    {
        rResult.m_RelativeX = x;
        rResult.m_RectItem.x = 0;
        rResult.m_RectItem.width = w;

        id      = m_FirstVisibleItem.Get();
        uiLevel = m_FirstVisibleItem.GetLevel();
        m_pClient->GetClientSize(&w, &h);

        if (!m_pScroller) return(false);
        rResult.m_uiRank = m_pScroller->GetScrollPos(wxVERTICAL);
    }
    else
    {
        id      = pDataModel->GetRootItem();
        uiLevel = 0;
        h       = -1;
    }

    //deal with root item
    if (m_bHideRootItem)
    {
        if (uiLevel > 0) uiLevel--;
        if (pDataModel->IsRootItem(id))
        {
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
            if (uiLevel > 0) uiLevel--;
        }
        rResult.m_uiRank++;
    }

    //scan horizontally : compute column
    int x0 = 0;
    if ((bClientAreaOnly) && (m_pScroller)) x0 = m_pScroller->GetScrollPos(wxHORIZONTAL);
    wxVirtualDataViewColumnsList &rvColumns = Columns();
    size_t uiNbCols = rvColumns.GetColumnsCount();
    size_t uiCol;
    int ixCol = -x0;
    int iWidth = 0;
    int xx = 0;
    bool bFound = false;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(uiCol);
        if (!pCol) continue;
        if (pCol->IsHidden()) continue;

        iWidth = pCol->GetWidth();
        ixCol += iWidth;
        if (x <= ixCol)
        {
            rResult.m_uiColIndex = uiCol;
            rResult.m_RectSubItem.x     = ixCol - iWidth;
            rResult.m_RectSubItem.width = iWidth;
            bFound = true;
            break;
        }

        xx += iWidth;
    }
    if (!bFound)
    {
        //column not found : gives the first or the last column index
        if (x < 0)             rResult.m_uiColIndex = 0;
        else if (uiNbCols > 0) rResult.m_uiColIndex = uiNbCols - 1;
        else                   rResult.m_uiColIndex = 0;

        rResult.m_RectSubItem.x = rvColumns.GetColumnStart(rResult.m_uiColIndex) + x0;
        rResult.m_RectSubItem.width = rvColumns.GetColumnWidth(rResult.m_uiColIndex);
        rResult.m_uiFlags |= E_HIT_TEST_OUTSIDE_ITEM;
    }
    rResult.m_RelativeX -= xx; //relative to column start in virtual coordinate
    rResult.m_RelativeX += x0; //relative to column start in client coordinates

    //flags : on margins
    if (rResult.m_RelativeX < m_iMarginLeft)                  rResult.m_uiFlags |= E_HIT_TEST_ON_LEFT_MARGIN;
    if (rResult.m_RelativeX > iWidth - m_iMarginRight)        rResult.m_uiFlags |= E_HIT_TEST_ON_RIGHT_MARGIN;
    if (rResult.m_RelativeY < m_iMarginTop)                   rResult.m_uiFlags |= E_HIT_TEST_ON_TOP_MARGIN;
    if (rResult.m_RelativeY > iHeightTotal - m_iMarginBottom) rResult.m_uiFlags |= E_HIT_TEST_ON_BOTTOM_MARGIN;

    //flags : on vertical edges
    int iThickness = m_VerticalRulesPen.GetWidth();
    if (iThickness <= m_iColumnBorderThickness) iThickness = m_iColumnBorderThickness;
    if (iThickness <= 1) iThickness = 1;
    if (rResult.m_RelativeX < iThickness) rResult.m_uiFlags |= E_HIT_TEST_ON_LEFT_EDGE;
    if (rResult.m_RelativeX > iWidth - iThickness) rResult.m_uiFlags |= E_HIT_TEST_ON_RIGHT_EDGE;

    //flags : on horizontal edges
    iThickness = m_HorizontalRulesPen.GetWidth();
    if (iThickness <= m_iRowBorderThickness) iThickness = m_iRowBorderThickness;
    if (iThickness < 1) iThickness = 1;
    if (rResult.m_RelativeY < iThickness) rResult.m_uiFlags |= E_HIT_TEST_ON_TOP_EDGE;
    if (rResult.m_RelativeY > iHeightTotal - iThickness) rResult.m_uiFlags |= E_HIT_TEST_ON_BOTTOM_EDGE;

    //convert field to model column
    wxVirtualDataViewColumn *pCol2 = rvColumns.GetColumn(rResult.m_uiColIndex);
    if (pCol2)
    {
        rResult.m_uiField = pCol2->GetModelColumn();
    }

    //scan vertically
    while ((yy < h) || (h < 0))
    {
        if (!id.IsOK()) break;

        //get item size
        if (!m_bUniformRowHeight)
        {
            iHeight = GetItemSize(id).GetHeight();
            iHeightTotal = iHeight + iMargins;
        }

        //check y. We want yy <= y < yy + iHeightTotal
        if ((yy <= y) && (y < yy + iHeightTotal))
        {
            //hit
            rResult.m_ID              = id;
            rResult.m_RelativeY       = y - yy;
            rResult.m_x               = x;
            rResult.m_y               = y;

            //expander hit ?
            wxCoord lIndent = uiLevel * m_iIndentation + m_iMarginLeft;
            if (pDataModel->HasExpander(id))
            {
                wxCoord xMin    = lIndent - x0;
                wxCoord xMax    = xMin + m_iTreeButtonTotalWidth;
                if ((x >= xMin) && (x <= xMax)) rResult.m_uiFlags |= E_HIT_TEST_ON_EXPANDER;
            }

            //checkbox hit ?
            if ((pDataModel->HasCheckbox(id)) && (m_bDrawCheckbox))
            {
                wxCoord xMin = lIndent + m_iTreeButtonTotalWidth - x0;
                wxCoord xMax = xMin + m_iCheckboxTotalWidth;
                if ((x >= xMin) && (x <= xMax)) rResult.m_uiFlags = E_HIT_TEST_ON_CHECKBOX;
            }

            //fill rect for sub-item (column). X & Width already filled during horizontal scan
            rResult.m_RectSubItem.y = yy;
            rResult.m_RectSubItem.height = iHeightTotal;

            //fill rect for whole item
            wxSize sItemSize = GetItemSize(id);
            rResult.m_RectItem.x      = -x0;
            rResult.m_RectItem.width  = sItemSize.GetWidth();
            rResult.m_RectItem.y      = yy;
            rResult.m_RectItem.height = iHeightTotal;

            //PrintItemID(id, uiLevel, "Hittest at x", x, "y", y);

            return(true);
        }

        //next
        yy += iHeightTotal;
        id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel, 1);
        rResult.m_uiRank++;
    };

    return(false);
}

/** Get the rectangle of an item
  * \param rRect [output]: the bounding rectangle of the item
  * \param rID                [input] : the ID of the element to get
  * \param uiColIndex         [input] : the column index of the element to get (view coordinate)
  * \param bClientCoordinates [input] : if true, the bounding rectangle is in client coordinates
  *                                     (so scrolled out items may have negatives coordinates)
  *                                     if false, the root item starts at (0,0) (top left corner)
  * \return true if the item is displayed
  *         false otherwise (collapsed, hidden)
  */
bool wxVirtualTreeModelRenderer::GetItemRect(wxRect &rRect,
                                             const wxVirtualItemID &rID,
                                             size_t uiColIndex,
                                             bool bClientCoordinates)
{
    //init & check
    rRect.x = -1;
    rRect.y = -1;
    rRect.width = -1;
    rRect.height = -1;
    if (!m_pClient) return(false);
    if ((!m_pScroller) && (bClientCoordinates)) return(false);

    //get data model
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    //get state model
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //get y coords
    int iYOffset = 0;
    if (m_bUniformRowHeight)
    {
        //get item rank
        size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);
        if ((!pDataModel->IsRootItem(rID)) && (uiRank == 0)) return(false);
        if ((m_bHideRootItem) && (uiRank > 0)) uiRank--;

        //compute height
        int iHeight = GetUniformRowHeight();
        iHeight = GetRowHeightWithMargins(iHeight);
        rRect.height = iHeight;

        //compute y coord
        rRect.y = uiRank * iHeight;

        //Y offset for
        iYOffset = m_pScroller->GetScrollPos(wxVERTICAL) * iHeight;
    }
    else
    {
        //non-uniform row height : scan from root
        size_t uiLevel = pDataModel->GetDepth(rID);
        int iHeight = 0;
        int y = 0;
        wxVirtualItemID id = pDataModel->GetRootItem();

        bool bFoundFirstVisibleItem = false;
        m_FirstVisibleItem.SetDataModel(pDataModel);
        wxVirtualItemID idFirstVisible = m_FirstVisibleItem.Get();

        while (id != rID)
        {
            if (id == idFirstVisible)
            {
                bFoundFirstVisibleItem = true;
                iYOffset = y;
            }
            iHeight = GetItemSize(id).GetHeight();
            iHeight = GetRowHeightWithMargins(iHeight);
            y += iHeight;

            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel, 1);
            if (id.IsInvalid()) return(false);
        }

        //found
        iHeight = GetItemSize(rID).GetHeight();
        rRect.y = y;
        rRect.height = GetRowHeightWithMargins(iHeight);

        //offset of 1st visible item
        if ((!bFoundFirstVisibleItem) && (bClientCoordinates))
        {
            iYOffset = y;
            while(id != idFirstVisible)
            {
                iHeight = GetItemSize(id).GetHeight();
                iHeight = GetRowHeightWithMargins(iHeight);
                iYOffset += iHeight;

                id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel, 1);
                if (id.IsInvalid()) break;
            }
        }
    }

    //x coord :
    rRect.x = Columns().GetColumnStart(uiColIndex);
    rRect.width = Columns().GetColumnWidth(uiColIndex);

    //transform to client coordinates
    if (bClientCoordinates)
    {
        int iXOffset = m_pScroller->GetScrollPos(wxHORIZONTAL);
        rRect.x -= iXOffset;
        rRect.y -= iYOffset;
    }

    return(false);
}

/** Expand an item
  * \param rID                  [input]: the item to expand
  * \param bRefresh             [input]: true if the display must be redrawn after expanding the item
  * \param bRecursive           [input]: true if the change has to be applied recursively on all items
  * \param bUpdateScrollbars    [input]: true if scrollbars need to be update after the operation
  * \param bSendEvent           [input]: true for sending events
  * \return true if the item was expanded, false otherwise
  */
bool wxVirtualTreeModelRenderer::Expand(const wxVirtualItemID &rID, bool bRefresh, bool bRecursive,
                                        bool bUpdateScrollbars, bool bSendEvent)
{
    if (!m_pClient) return(false);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    if (!pDataModel->HasExpander(rID)) return(true);
    size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);
    return(Expand(rID, bRefresh, bRecursive, bUpdateScrollbars, uiRank, bSendEvent));
}

/** Collapse an item
  * \param rID                  [input]: the item to expand
  * \param bRefresh             [input]: true if the display must be redrawn after collapsing the item
  * \param bRecursive           [input]: true if the change has to be applied recursively on all items
  * \param bUpdateScrollbars    [input]: true if scrollbars need to be update after the operation
  * \param bSendEvent           [input]: true for sending events
  * \return true if the item was collapsed, false otherwise
  */
bool wxVirtualTreeModelRenderer::Collapse(const wxVirtualItemID &rID, bool bRefresh, bool bRecursive,
                                          bool bUpdateScrollbars, bool bSendEvent)
{
    if (!m_pClient) return(false);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);
    return(Collapse(rID, bRefresh, bRecursive, bUpdateScrollbars, uiRank, bSendEvent));
}

/** Expand an item
  * \param rID                  [input]: the item to expand
  * \param bRefresh             [input]: true if the display must be redrawn after expanding the item
  * \param bRecursive           [input]: true if the change has to be applied recursively on all items
  * \param bUpdateScrollbars    [input]: true if scrollbars need to be update after the operation
  * \param uiRank               [input]: rank of the item to expand
  * \param bSendEvent           [input]: true for sending events
  * \return true if the item was expanded, false otherwise
  */
bool wxVirtualTreeModelRenderer::Expand(const wxVirtualItemID &rID, bool bRefresh, bool bRecursive,
                                        bool bUpdateScrollbars, size_t uiRank, bool bSendEvent)
{
    if (!m_pClient) return(false);
    if (!CollapseOrExpand(rID, true, bRefresh, bRecursive, bUpdateScrollbars, uiRank, bSendEvent)) return(false);
    return(true);
}

/** Collapse an item
  * \param rID                  [input]: the item to expand
  * \param bRefresh             [input]: true if the display must be redrawn after collapsing the item
  * \param bRecursive           [input]: true if the change has to be applied recursively on all items
  * \param bUpdateScrollbars    [input]: true if scrollbars need to be update after the operation
  * \param uiRank               [input]: rank of the item to collapse
  * \param bSendEvent           [input]: true for sending events
  * \return true if the item was collapsed, false otherwise
  */
bool wxVirtualTreeModelRenderer::Collapse(const wxVirtualItemID &rID, bool bRefresh, bool bRecursive,
                                          bool bUpdateScrollbars, size_t uiRank, bool bSendEvent)
{
    if (!m_pClient) return(false);
    if (!CollapseOrExpand(rID, false, bRefresh, bRecursive, bUpdateScrollbars, uiRank, bSendEvent)) return(false);
    return(true);
}

/** Collapse or expand the node
  * \param rID                  [input]: the item to collapse or expand
  * \param bExpand              [input]: true for expanding the item, false for collapsing it
  * \param bRefresh             [input]: true if the display must be redrawn after collapsing the item
  * \param bRecursive           [input]: true if the change has to be applied recursively on all items
  * \param bUpdateScrollbars    [input]: true if scrollbars need to be update after the operation
  * \param uiRank               [input]: rank of the item to collapse or expand
  * \param bSendEvent           [input]: true for sending events
  * \return true if the item was collapsed, false otherwise
  */
bool wxVirtualTreeModelRenderer::CollapseOrExpand(const wxVirtualItemID &rID,
                                                  bool bExpand, bool bRefresh,
                                                  bool bRecursive, bool bUpdateScrollbars,
                                                  size_t uiRank, bool bSendEvent)
{
    //check
    if (!m_pClient) return(false);
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    if (!EndCurrentEdition()) return(false);

    wxVirtualIDataModel* pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    if (!m_pScroller) return(false);

    //collapse or expand
    if (bRecursive)
    {
        if (!bSendEvent)
        {
            pStateModel->Expand(rID, bExpand, true);
        }
        else
        {
            wxEventType eEventDoing, eEventDone;
            if (bExpand)
            {
                eEventDoing = wxEVT_VDV_ITEM_EXPANDING;
                eEventDone  = wxEVT_VDV_ITEM_EXPANDED;
            }
            else
            {
                eEventDoing = wxEVT_VDV_ITEM_COLLAPSING;
                eEventDone  = wxEVT_VDV_ITEM_COLLAPSED;
            }

            wxStack<wxVirtualItemID> oStackOfIDs;
            oStackOfIDs.push(rID);
            while(!oStackOfIDs.empty() > 0)
            {
                wxVirtualItemID id = oStackOfIDs.top();
                oStackOfIDs.pop();

                //process
                if (!m_pClient->SendEventAndCheck(eEventDoing, id)) continue;
                pStateModel->Expand(id, bExpand, false);
                m_pClient->SendEvent(eEventDone, id);

                //children
                wxVirtualItemID idChild = pDataModel->GetChild(id, 0);
                size_t uiChild = 0;
                while(!idChild.IsInvalid())
                {
                    oStackOfIDs.push(idChild);
                    uiChild++;
                    idChild = pDataModel->GetChild(id, uiChild);
                }
            }
        }


        if (bUpdateScrollbars)
        {
            InvalidateCurrentSize();
            InvalidateVirtualSize();
            UpdateScrollbars();
        }
    }
    else
    {
        bool bIsExpanded = pStateModel->IsExpanded(rID);
        if ((!bIsExpanded && bExpand) || (bIsExpanded && !bExpand))
        {
            //send event
            if (bSendEvent)
            {
                if (bExpand)
                {
                    if (!m_pClient->SendEventExpanding(rID)) return(false);
                }
                else
                {
                    if (!m_pClient->SendEventCollapsing(rID)) return(false);
                }
            }
            size_t uiOldSize = 0;
            size_t uiNewSize = 0;
            bool bUniform = m_bUniformRowHeight;
            if ((bUpdateScrollbars) && (bUniform))
            {
                uiOldSize = pDataModel->GetSubTreeSize(rID, pStateModel);
            }

            pStateModel->Expand(rID, bExpand, false);

            if ((bUpdateScrollbars) && (bUniform))
            {
                uiNewSize = pDataModel->GetSubTreeSize(rID, pStateModel);
            }

            if (bUpdateScrollbars)
            {
                if (bUniform)
                {
                    int iRange     = m_iNbVisiblesItems; //m_pScroller->GetScrollRange(wxVERTICAL);
                    int iDiff      = uiNewSize - uiOldSize;
                    iRange += iDiff;
                    SetAllScrollbars(m_sVirtualSize, iRange);

                    //effect on first visible item
                    if (m_iFirstVisibleLine > (int) uiRank)
                    {
                        ShiftFirstVisibleItem(iDiff);
                    }
                }
                else
                {
                    InvalidateCurrentSize();
                    InvalidateVirtualSize();
                    UpdateScrollbars();
                }
            }

            //send event
            if (bSendEvent)
            {
                if (bExpand) m_pClient->SendEventExpanded(rID);
                else         m_pClient->SendEventCollapsed(rID);
            }
        }
    }

    //effect on focused item:
    //  1 : if the expanded/collapsed item is located after or is the focused item,
    //      there is no effect : m_uiRankFocusedItem <= uiRank
    //  2 : if the expanded/collapsed item is located before the focused item,
    //      then the rank of the focused item is increased/decreased by
    //      the expanded/collapsed sub-tree size
    //  3 : if the focused item is included in the collapsed tree, then
    //      the collapsed node is the new focused item
    wxVirtualItemID idFocused = pStateModel->GetFocusedItem();
    if ((!bExpand) && (pDataModel->IsInSubTree(idFocused, rID)))
    {
        //case 3
        m_uiRankFocusedItem = uiRank;
        pStateModel->SetFocus(rID);
    }
    else if (m_uiRankFocusedItem > uiRank)
    {
        //case 2
        size_t uiSubTreeSize = pDataModel->GetSubTreeSize(rID, pStateModel);
        if (bExpand) m_uiRankFocusedItem += uiSubTreeSize;
        else if (m_uiRankFocusedItem >= uiSubTreeSize) m_uiRankFocusedItem -= uiSubTreeSize;
        else m_uiRankFocusedItem = 0;
    }
    //else case 1 : nothing to do

    if (bRefresh) Refresh();

    //debug
//    idFocused = pStateModel->GetFocusedItem();
//    wxLogMessage("Focused item id=%d row=%d col=%d rank=%d",
//                  idFocused.GetInt32(), idFocused.GetRow(),
//                  idFocused.GetColumn(), m_uiRankFocusedItem);

    return(true);
}

//------------------ INTERFACE IMPLEMENTATION : MISC ----------------//
/** Get focused column ID
  * \return the focused column ID
  */
int wxVirtualTreeModelRenderer::GetFocusedField(void) const
{
    return(m_uiColFocusedItem); //the column and not the field because a field can be visible in multiple columns
}

/** Set focused column ID
  * \param iField [input]: the new focused field
  */
void wxVirtualTreeModelRenderer::SetFocusedField(int iField)
{
    if (iField < 0) return;
    m_uiColFocusedItem = iField;

    wxVirtualDataViewColumn *pCol = Columns().GetColumn(iField);
    if (pCol) m_uiFieldFocusedItem = pCol->GetModelColumn();
}

//------------ INTERFACE IMPLEMENTATION : CALLBACKS -----------------//
/** Called by the client to notify that the data model has changed
  */
void wxVirtualTreeModelRenderer::OnDataModelChanged(void)
{
    InvalidateFirstVisibleItem();
    InvalidateCurrentSize();
    InvalidateVirtualSize();
    UpdateScrollbars();
    UpdateFocusedItemRank();
}

/** Called by the client to notify changes in layout
  */
void wxVirtualTreeModelRenderer::OnLayoutChanged(void)
{
    InvalidateFirstVisibleItem();
    InvalidateCurrentSize();
    InvalidateVirtualSize();
    UpdateScrollbars();
    UpdateFocusedItemRank();
}

//-------------- PROTECTED INTERFACE IMPLEMENTATION -----------------//
/** Draw the background
  * \param rDC [input] : the device context to use
  * \return true on success, false on failure (painting incomplete or not done at all)
  */
bool wxVirtualTreeModelRenderer::DrawBackground(wxDC &rDC)
{
    //fill it with background colour
    rDC.SetBackground(m_pClient->GetBackgroundColour());
    rDC.Clear();
    return(true);
}

/** Get the size of 1 item
  * \param rID [input]: the ID of the item
  * \return the size of the item
  */
wxSize wxVirtualTreeModelRenderer::GetItemSize(const wxVirtualItemID &rID) const
{
    //if (rID.IsInvalid()) return(wxSize(0, 0)); NO: let the model renderer performs
                                               //    a calculation, to return a valid size
                                               //    this is needed to avoid wrong row header in empty controls
                                               //    (this will not crash though, only looks strange)
    if (m_bHideRootItem)
    {
        wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
        if (pDataModel)
        {
            if (pDataModel->IsRootItem(rID)) return(wxSize(0, 0));
        }
    }

    //add each column size to the result
    wxSize sResult(0, 0);
    const wxVirtualDataViewColumnsList &rvList = Columns();
    size_t uiNbCols = rvList.GetColumnsCount();
    size_t uiCol;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        wxSize sSizeCol = GetItemSize(rID, uiCol);

        //width : add
        int w = sSizeCol.GetWidth();

        //add left margin for all columns except 1st one
        if (uiCol > 0) w += m_iMarginLeft;

        //add right margin for all columns except last one
        if (uiCol < uiNbCols - 1) w += m_iMarginRight;

        //add border thickness for all columns except 1st
        if ((uiCol > 0) && (m_bDrawVerticalRules)) w += m_iColumnBorderThickness;

        //height : get maximum
        int h = sSizeCol.GetHeight();
        if (h > sResult.GetHeight()) sResult.SetHeight(h);
        sResult.SetWidth(sResult.GetWidth() + w);
    }

    return(sResult);
}

/** Get the size of 1 item
  * \param rID      [input] : the ID of the item
  * \param uiColID  [input] : the index of the column
  * \return the size of the item
  */
wxSize wxVirtualTreeModelRenderer::GetItemSize(const wxVirtualItemID &rID,
                                               size_t uiColID) const
{
    //init
    wxSize sResult(0, 0);

    //get data model
    if (!m_pClient) return(sResult);
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(sResult);

    //get state model
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(sResult);

    //get column
    const wxVirtualDataViewColumnsList &rvList = Columns();
    wxVirtualDataViewColumn *pCol = rvList.GetColumn(uiColID);
    if (!pCol) return(sResult);
    if (pCol->IsHidden()) return(sResult);

    //get renderer
    wxVirtualDataViewRenderer *pRenderer = pCol->GetRenderer();
    if (!pRenderer) return(sResult);

    //get model column
    size_t uiFieldID = pCol->GetModelColumn();

    //get item value & state & attribute
    wxVariant vValue = pDataModel->GetItemData(rID, uiFieldID);
    wxVirtualDataViewItemState rState;
    pStateModel->GetState(rState, rID);
    wxVirtualDataViewItemAttr *pAttr = pDataModel->GetItemAttribute(rID, uiFieldID, rState);

    //get raw size
    sResult = pRenderer->GetSize(m_pClient, pAttr, rState, vValue);

    //add indent
    if (IsExpanderColumn(uiColID))
    {
        int lLevel       = pDataModel->GetDepth(rID);
        if ((m_bHideRootItem) && (lLevel > 0)) lLevel--;
        int lIndentation = lLevel * m_iIndentation + m_iTreeButtonTotalWidth;
        if ((pDataModel->HasCheckbox(rID)) && (m_bDrawCheckbox)) lIndentation += m_iCheckboxTotalWidth;
        int w = sResult.GetWidth();
        w += lIndentation;
        sResult.SetWidth(w);
    }

    //return
    return(sResult);
}

/** Compute background colours
  */
void wxVirtualTreeModelRenderer::ComputeBackgroundColours(void)
{
    m_cRowColourOddReal = m_cRowColourOdd;
    m_cRowColourEvenReal = m_cRowColourEven;

    if ((m_cRowColourEvenReal.IsOk()) && (m_cRowColourOddReal.IsOk())) return;

    //Compute row colours from background colour
    if (!m_pClient)
    {
        m_cRowColourEvenReal.Set(200, 200, 200);
        m_cRowColourOddReal.Set(255, 255, 255);
        return;
    }
    const wxColour cBackColour = m_pClient->GetBackgroundColour();

    //even row = back colour. odd row = make lighter or darker based on current colour
    m_cRowColourEvenReal = cBackColour;
    int iAlpha = 125;
    if (cBackColour.GetRGB() > 0x808080) iAlpha = 75;
    m_cRowColourOddReal  = cBackColour.ChangeLightness(iAlpha);

    //even row = back colour. odd row = invert
//    m_cRowColourEvenReal = cBackColour;
//    unsigned char r = cBackColour.Red();
//    unsigned char g = cBackColour.Green();
//    unsigned char b = cBackColour.Blue();
//    r = 255 - r; g = 255 - g; b = 255 - b;
//    m_cRowColourOddReal  = wxColour(r, g, b);
}

/** Draw one item
  * \param rDC    [input]: the device context
  * \param rRect  [input]: the clipping rectangle. The left and right margins are already
  *                        taken into account
  * \param rID    [input]: the ID of the item to draw
  * \param rState [input]: the state of the item to draw
  * \param pModel [input]: the data model of the item
  * \param lLevel [input]: the hierarchy level of the item
  * \param uiRank [input]: the rank of the current item
  * \param uiCount [input]: how many items were drawn before this one
  */
void wxVirtualTreeModelRenderer::DrawItem(wxDC &rDC, wxRect &rRect,
                                          const wxVirtualItemID &rID,
                                          const wxVirtualDataViewItemState &rState,
                                          wxVirtualIDataModel *pModel,
                                          long lLevel, size_t uiRank, size_t uiCount)
{
    //indent
    wxRect r = rRect;
    int lIndentation = lLevel * m_iIndentation + m_iTreeButtonTotalWidth;
    if ((pModel->HasCheckbox(rID)) && (m_bDrawCheckbox)) lIndentation += m_iCheckboxTotalWidth;
    r.x     += lIndentation;
    r.width -= lIndentation;

    //take into account vertical lines
    int iVerticalLineWidth = 0;
    if (m_bDrawVerticalRules) iVerticalLineWidth = m_VerticalRulesPen.GetWidth();
    int iTotalRightMargin = m_iMarginRight + iVerticalLineWidth;

    //draw background
    if (m_bAlternatingBackColour)
    {
        wxColour c(m_cRowColourOddReal);
        if (m_bFixedAlternatingBackColour)
        {
            if (uiCount % 2 == 0) c = m_cRowColourEvenReal;
        }
        else
        {
            if (uiRank % 2 == 0) c = m_cRowColourEvenReal;
        }

        wxRect r2 = rRect;
        r2.x -= m_iMarginLeft;
        r2.width += m_iMarginLeft + m_iMarginRight;
        r2.y -= m_iMarginTop;
        r2.height += m_iMarginTop + m_iMarginBottom;

        if (m_bDrawHoritontalRules)
        {
            int iHeightPen = m_HorizontalRulesPen.GetWidth();
            r2.y -= iHeightPen;
            r2.height += iHeightPen;
        }

        wxBrush oBrush(c, wxBRUSHSTYLE_SOLID);
        wxPen oPen(c, 1, wxPENSTYLE_SOLID);
        rDC.SetBrush(oBrush);
        rDC.SetPen(oPen);
        rDC.DrawRectangle(r2);
    }

    //draw selection rectangle
    wxVirtualDataViewItemState oState(rState);
    bool bSelected = rState.IsSelected();
    if (bSelected)
    {
        wxRect r2 = rRect;
        r2.x -= m_iMarginLeft;
        r2.width += m_iMarginLeft + m_iMarginRight;
        r2.y -= m_iMarginTop;
        r2.height += m_iMarginTop + m_iMarginBottom;

        if (m_cSelectedItemsColour.IsOk())
        {
            wxBrush oBrush(m_cSelectedItemsColour, wxBRUSHSTYLE_SOLID);
            wxPen   oPen(m_cSelectedItemsColour, 1, wxPENSTYLE_SOLID);
            rDC.SetBrush(oBrush);
            rDC.SetPen(oPen);
            rDC.DrawRectangle(r2);
        }
        else
        {
            int iFlags = wxCONTROL_SELECTED;
            if ((m_bHasFocus) || (m_bShowSelectedItemsAsFocused)) iFlags |= wxCONTROL_FOCUSED;
            wxRendererNative::GetDefault().DrawItemSelectionRect(m_pClient, rDC, r2, iFlags);
        }
        oState.Select(false);
    }

    //temp graphic attributes
    wxVirtualDataViewItemAttr oTempAttr;

    //draw each columns
    wxVirtualDataViewColumnsList &rvColumns = Columns();
    size_t uiNbCols = rvColumns.GetColumnsCount();
    size_t uiCol;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        //get column
        wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(uiCol);
        if (!pCol) continue;
        if (pCol->IsHidden()) continue;

        //indentation
        int lIndent = 0;
        if (IsExpanderColumn(uiCol)) lIndent = lIndentation;
        lIndent += m_iMarginLeft;

        //get width
        int iWidth = pCol->GetWidth();

        //special case : only 1 field, the item takes the whole length
        size_t uiNbFields = pModel->GetFieldCount(rID);
        if (uiNbFields == size_t(-1)) iWidth = rvColumns.GetTotalWidth();

        r.width = iWidth;
        r.width -= lIndent;
        r.width -= iTotalRightMargin; //grid line + right margin

        //clip
        int iXLeft = r.x;
        int iXRight = r.x + r.width;
        if ((iXRight < rRect.x) || (iXLeft > rRect.x + rRect.width))
        {
            //column is scrolled out
            r.x -= lIndent;
            r.x += iWidth + m_iMarginLeft;
            continue;
        }

        //get attributes
        size_t uiModelCol = pCol->GetModelColumn();
        wxVirtualDataViewCellAttr *pAttr = pCol->GetAttribute();
        if (!pAttr)
        {
            r.x -= lIndent;
            r.x += iWidth + m_iMarginLeft;
            continue;
        }

        //get item attributes
        wxVirtualDataViewItemAttr *pItemAttr = pModel->GetItemAttribute(rID, uiModelCol, rState);

        //combine attributes
        wxVirtualDataViewItemAttr *pEffectiveAttr = pItemAttr;
        if (pItemAttr)
        {
            if (pAttr)
            {
                oTempAttr = *pItemAttr;
                oTempAttr.Combine(pAttr->GetGraphicAttributes());
                pEffectiveAttr = &oTempAttr;
            }
        }
        else if (pAttr)
        {
            pEffectiveAttr = pAttr->GetGraphicAttributes();
        }

        //get renderer
        wxVirtualDataViewRenderer *pRenderer = pAttr->GetRenderer();
        if (!pRenderer)
        {
            r.x -= lIndent;
            r.x += iWidth + m_iMarginLeft;
            continue;
        }

        //get data
        wxVariant vValue = pModel->GetItemData(rID, uiModelCol,
                                               wxVirtualIDataModel::WX_ITEM_MAIN_DATA
                                              );

        //clip
        wxDCClipper oDCClipper(rDC, r);

        //draw
        pRenderer->Render(m_pClient, r, rDC, pEffectiveAttr, oState, vValue);

        //next
        if (uiNbFields == size_t(-1)) break;
        r.x -= lIndent;
        r.x += iWidth + m_iMarginLeft;
    }

    //draw focused rect
    if (rState.IsFocused())
    {
        wxRendererNative::GetDefault().DrawFocusRect(m_pClient, rDC, rRect, 0);
    }
}

/** Check if the column is the expander column
  * \param uiViewCol [input]: the column index (view index) of the column to check
  * \return true if the column is the expander column
  *         false otherwise
  */
bool wxVirtualTreeModelRenderer::IsExpanderColumn(size_t uiViewCol) const
{
    if (uiViewCol == 0) return(true);
    return(false);
}

/** Draw tree lines for one item
  * \param rDC       [input]: the device context
  * \param rRect     [input]: the clipping rectangle
  * \param rID       [input]: the ID of the item to draw
  * \param rState    [input]: the state of the item to draw
  * \param pModel    [input]: the data model of the item
  * \param lLevel    [input]: the hierarchy level of the item
  */
void wxVirtualTreeModelRenderer::DrawTreeLines(wxDC &rDC, wxRect &rRect,
                                               const wxVirtualItemID &rID,
                                               wxVirtualIDataModel *pModel,
                                               long lLevel)
{
    //calculation X
    int iButtonWidth     = m_TreeButtonSize.GetWidth();
    int iHalfButtonWidth = iButtonWidth / 2;
    int iItemIndent      = lLevel * m_iIndentation;

    //calculation Y
    int iYTop            = rRect.GetTop() - m_iMarginTop - 1; //+/- 1 : last point is normally not included in line
    int iYBottom         = rRect.GetBottom() + m_iMarginBottom + 1;
    int iYMiddle         = (rRect.GetTop() + rRect.GetBottom()) / 2;

    //more calculations
    bool bStyle1 = true;
    if (m_eTreeLinesStyle == E_TREE_LINES_AT_EXPANDER) bStyle1 = false;
    int iXStart = rRect.x + iItemIndent;
    int iXLeft  = iXStart + m_iTreeButtonMargins[0] + iHalfButtonWidth;
    int iXRight = iXStart + m_iTreeButtonMargins[0] + iButtonWidth + m_iTreeButtonMargins[1];

    if ((!bStyle1) && (lLevel > 0)) iXLeft -= m_iIndentation;

    //horizontal
    rDC.DrawLine(iXLeft, iYMiddle, iXRight, iYMiddle);
    if ((!bStyle1) && (lLevel == 0)) iXLeft -= m_iIndentation;

    //vertical : loop up at each level
    wxVirtualItemID id = rID;

    //this level
    long l = lLevel;
    if (bStyle1)
    {
        //STYLE 1 : lines at items, classic Windows style
        //up: draw, unless it is the 1st child of root item
        wxVirtualItemID idParent = pModel->GetParent(id);
        size_t uiIndex = pModel->GetChildIndex(idParent, id);
        if ((uiIndex > 0) || (l > 0))
//        if (!pModel->IsLastChild(id))
        {
            rDC.DrawLine(iXLeft, iYMiddle, iXLeft, iYTop);
        }

        //down: draw, unless it is the last child
        if (!pModel->IsLastChild(id))
        {
            rDC.DrawLine(iXLeft, iYMiddle, iXLeft, iYBottom);
        }

        //other levels
        l--;
        id = pModel->GetParent(id);
        iXLeft -= m_iIndentation;
        while(l >= 0)
        {
            //up: draw, unless it is the last child
            wxVirtualItemID idParent = pModel->GetParent(id);
            if (!pModel->IsLastChild(id))
            {
                rDC.DrawLine(iXLeft, iYMiddle, iXLeft, iYTop);
            }

            //down: draw, unless it is the last child
            if (!pModel->IsLastChild(id))
            {
                rDC.DrawLine(iXLeft, iYMiddle, iXLeft, iYBottom);
            }

            //next level
            l--;
            id = idParent;
            iXLeft -= m_iIndentation;
        }
    }
    else
    {
        //STYLE 2 : lines at expander
        wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
        bool bExpanded = pStateModel->IsExpanded(id);
        if ((l == lLevel) && (pModel->GetChildCount(id) > 0) && (bExpanded))
        {
            rDC.DrawLine(iXLeft + m_iIndentation, iYMiddle, iXLeft + m_iIndentation, iYBottom);
        }

        while(l >= 0)
        {
            //calculations
            wxVirtualItemID idParent = pModel->GetParent(id);
            size_t uiIndex = pModel->GetChildIndex(idParent, id);
            size_t uiNbChildren = pModel->GetChildCount(idParent);
            bool bIsLastChild   = uiIndex == uiNbChildren - 1; //pModel->IsLastChild(id) uiIndex == uiNbChildren - 1

            //up: draw, unless it is the 1st child of root item, or unless it is last child of current level
            bool bFirstChildOfRoot = (uiIndex == 0) && (l == 0);
            bool bLastChildOfLevel = bIsLastChild && (l != lLevel);
            if ((!bFirstChildOfRoot) && (!bLastChildOfLevel))
            {
                rDC.DrawLine(iXLeft, iYMiddle, iXLeft, iYTop);
            }

            //down: draw, unless it is the last child
            if (!bIsLastChild)
            {
                rDC.DrawLine(iXLeft, iYMiddle, iXLeft, iYBottom);
            }

            //next level
            l--;
            id = idParent;
            iXLeft -= m_iIndentation;
        }
    }
}

/** Draw a tree button
  * \param rDC       [input]: the device context
  * \param rRect     [input]: the clipping rectangle of the whole item
  * \param bExpanded [input]: true if the item is expanded, false otherwise
  */
void wxVirtualTreeModelRenderer::DrawTreeButton(wxDC &rDC, wxRect &rRect, bool bExpanded)
{
    //compute rectangle
    int iHeight = m_TreeButtonSize.GetHeight();
    if (iHeight > rRect.GetHeight()) iHeight =  rRect.GetHeight()
                                               - m_iTreeButtonMargins[2]  //top margin
                                               - m_iTreeButtonMargins[3]; //bottom margin
    int iMargin = (rRect.GetHeight() - iHeight) / 2;

    wxRect rButton(rRect.x + m_iTreeButtonMargins[0],
                   rRect.y + iMargin,                   //center vertically
                   m_TreeButtonSize.GetWidth(),
                   iHeight);

    //draw
    int iFlags = 0;
    if (bExpanded) iFlags |= wxCONTROL_EXPANDED;
//    wxRendererNative &r = wxRendererNative::GetDefault(); //draw some kind of arrow when themed are enabled :-(
    wxRendererNative &r = wxRendererNative::GetGeneric();
    r.DrawTreeItemButton(m_pClient, rDC, rButton, iFlags);
}

/** Draw the checkbox
  * \param rDC       [input]: the device context
  * \param rRect     [input]: the clipping rectangle of the whole item
  * \param iState    [input]: the state of the checkbox.
  *                           0 : unchecked
  *                           1 : checked
  *                           2 : unknown
  */
void wxVirtualTreeModelRenderer::DrawCheckBox(wxDC &rDC, wxRect &rRect, int iState)
{
    //compute rectangle
    int iHeight = m_CheckboxSize.GetHeight();
    if (iHeight > rRect.GetHeight()) iHeight =  rRect.GetHeight()
                                               - m_iCheckboxMargins[2]  //top margin
                                               - m_iCheckboxMargins[3]; //bottom margin
    int iMargin = (rRect.GetHeight() - iHeight) / 2;

    wxRect rButton(rRect.x + m_iTreeButtonTotalWidth + m_iCheckboxMargins[0],
                   rRect.y + iMargin,                   //center vertically
                   m_CheckboxSize.GetWidth(),
                   iHeight);

    //draw
    int iFlags = 0;
         if (iState == 1) iFlags |= wxCONTROL_CHECKED;
    else if (iState == 2) iFlags |= wxCONTROL_UNDETERMINED;
    wxRendererNative &r = wxRendererNative::GetDefault();
    r.DrawCheckBox(m_pClient, rDC, rButton, iFlags);
}

//---------------------- SCROLLBARS ---------------------------------//
/** Invalidate the current size
  */
void wxVirtualTreeModelRenderer::InvalidateCurrentSize(void)
{
    m_sCurrentClientSize.Set(-1, -1);
}

/** Invalidate the virtual size
  */
void wxVirtualTreeModelRenderer::InvalidateVirtualSize(void)
{
    m_sVirtualSize.Set(-1, -1);
    m_iNbVisiblesItems = -1;
}

/** Compute the full size & amount of items
  * \param uiNbItemsVisible [output]: amount of visible items
  * \return size of the virtual client area necessary for showing all the items
  */
wxSize wxVirtualTreeModelRenderer::ComputeSize(size_t &uiNbItemsVisible) const
{
    //check & init
    uiNbItemsVisible = 0;
    if (!m_pClient) return(wxSize(1, 1));

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(wxSize(1, 1));

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(wxSize(1, 1));

    //get margins
    int iHorizMargin = m_iMarginLeft + m_iMarginRight;
    int iVertMargin  = m_iMarginTop + m_iMarginBottom;
    if (m_bDrawHoritontalRules) iVertMargin += m_VerticalRulesPen.GetWidth();

    //uniform height
    bool bUniformHeight = false;
    int iHeight = -1;
    if (m_bUniformRowHeight) iHeight = GetUniformRowHeight();
    if (iHeight > 0)
    {
        iHeight += iVertMargin;
        bUniformHeight = true;
    }

    //do we need to recompute vertical size ?
    //recompute if a change of layout has occurred (amount of visible items has changed)
    bool bRecomputeVSize = false;
    wxSize sCurrentSize = m_pClient->GetClientSize();
    if (sCurrentSize.GetHeight() != m_sCurrentClientSize.GetHeight()) bRecomputeVSize = true;
    if (bRecomputeVSize)
    {
        //do not recompute if virtual size is already OK
        if (m_sVirtualSize.GetHeight() > 0) bRecomputeVSize = false;
    }

    //compute size
    //if all sizes are uniform, the calculation is faster
    wxSize sResult(0, 0);
    if (bRecomputeVSize)
    {
        long lChild, lNbChildren;
        wxVirtualItemID idRoot = pDataModel->GetRootItem();
        if (bUniformHeight)
        {
            //uniform case : only compute amount of items
            uiNbItemsVisible = pDataModel->GetSubTreeSize(idRoot, pStateModel);
            if (m_bHideRootItem) uiNbItemsVisible--;
            sResult.SetHeight(uiNbItemsVisible * iHeight);
        }
        else
        {
            //non - uniform case - brute force calculation unavoidable
            wxStack<wxVirtualItemID> StackOfNodes;
            StackOfNodes.push(idRoot);

            while(StackOfNodes.size() > 0)
            {
                //get item
                wxVirtualItemID id = StackOfNodes.top();
                StackOfNodes.pop();

                //compute size
                wxSize s = GetItemSize(id);

                //add margins
                if ((id.IsOK()) || ((pDataModel->IsRootItem(id)) && (!m_bHideRootItem)))
                {
                    s.x += iHorizMargin;
                    s.y += iVertMargin;
                    uiNbItemsVisible++;
                }

                //accumulate (max value horizontal + accumulate vertically)
                //if (s.x > sResult.x) sResult.x += s.x;
                sResult.y += s.y;

                //recurse
                if (!pStateModel->IsExpanded(id)) continue;
                lNbChildren = pDataModel->GetChildCount(id);
                for(lChild = lNbChildren - 1; lChild >= 0; lChild--)
                {
                    wxVirtualItemID idChild = pDataModel->GetChild(id, lChild);
                    StackOfNodes.push(idChild);
                }
            }
        }
    }
    else
    {
        //already done
        uiNbItemsVisible = m_iNbVisiblesItems;
        sResult.SetHeight(m_sVirtualSize.GetHeight());
    }

    //horizontal width : size of all columns
    sResult.SetWidth(Columns().GetTotalWidth());

    return(sResult);
}

/** Set both horizontal and vertical scrollbars
  * \param iHPos   [input]: position of horizonzal scrollbar
  * \param iHPage  [input]: page size of horizontal scrollbar
  * \param iHRange [input]: range of horizontal scrollbar
  * \param iVPos   [input]: position of vertical scrollbar
  * \param iVPage  [input]: page size of vertical scrollbar
  * \param iVRange [input]: range of vertical scrollbar
  */
void wxVirtualTreeModelRenderer::SetScrollbars(int iHPos, int iHPage, int iHRange,
                                               int iVPos, int iVPage, int iVRange)
{
    if (!m_pScroller) return;
    SetOneScrollbar(iVPos, iVPage, iVRange, wxVERTICAL);
    SetOneScrollbar(iHPos, iHPage, iHRange, wxHORIZONTAL);
}

/** Set one scrollbar
  * \param iPos   [input]: position of horizonzal scrollbar
  * \param iPage  [input]: page size of horizontal scrollbar
  * \param iRange [input]: range of horizontal scrollbar
  * \param iOrient [input]: which scrollbar to set (wxVERTICAL or wxHORIZONTAL)
  */
void wxVirtualTreeModelRenderer::SetOneScrollbar(int iPos, int iPage, int iRange, int iOrient)
{
    //adjust values for meaningfulness
    //iPage must be in the range [0, iRange + 1]
    //iPos must be in the range [0,  iRange - max( iPage - 1, 0)]
    int iPo = iPos;
    int iRa = iRange;
    int iPa = iPage;

    //adjust page : must be <= range
//    if (iPa > iRa) iPa = iRa;
//
//    //adjust position
//    int iMax = iPa - 1;
//    if (iMax < 0) iMax = 0;
//    int iMaxPos = iRa - iMax;
//    if (iPo < 0) iPo = 0;
//    if (iPo >= iMaxPos) iPo = iMaxPos;

    //get current settings
    int iCurrentPos   = m_pScroller->GetScrollPos(iOrient);
    int iCurrentPage  = m_pScroller->GetScrollThumb(iOrient);
    int iCurrentRange = m_pScroller->GetScrollRange(iOrient);

    //set scrollbars only if 1 of the parameter has changed
    if ((iCurrentPos == iPo) && (iCurrentPage == iPa) && (iCurrentRange == iRa)) return;

    //set the scrollbar
    m_pScroller->SetScrollbar(iOrient, iPo, iPa, iRa, true);
}

/** Get scrolled window client size
  * \return the scrolled window client size
  */
wxSize wxVirtualTreeModelRenderer::GetTargetSize(void) const
{
    if (m_pClient) return(m_pClient->GetClientSize());
    return(wxSize(0, 0));
}

/** Client size available if no scrollbars are shown
  * \return the available client size if no scroll bars are shown
  */
wxSize wxVirtualTreeModelRenderer::GetScrollerVisibleSize(void) const
{
    if (m_pScroller)
    {
        wxSize sWinSize = m_pScroller->GetSize();
        wxSize sBorder  = m_pScroller->GetWindowBorderSize();
        return(sWinSize - sBorder);
    }
    return(wxSize(0, 0));
}

/** Set all scrollbars
  * \param sVirtualSize     [input]: the virtual size of the client area
  * \param uiNbItems        [input]: the amount of items visible
  */
void wxVirtualTreeModelRenderer::SetAllScrollbars(wxSize sVirtualSize, size_t uiNbItems)
{
    //since setting the scrollbars will make them appearing or disappearing,
    //it will change the client size of the window.
    //So we need a small loop here, and we quit when the client size has not changed
    //example:
    //start       : no scrollbar shown, expand of 1 item is done
    //iteration 1 : set the scrollbar normally. Vertical scrollbar appears, client width reduced
    //iteration 2 : now we need an horizontal scrollbar. horizontal scrollbar appears, client height reduced
    //iteration 3 : vertical scrollbar changes, client size does not change, end of loop

    //check
    if (!m_pClient) return;
    if (!m_pScroller) return;

    //rentrency. This is needed because the following can happen:
    //  - the scrollbar is adjusted : it appears or disappears, leading to a change in client size
    //  -> the change in client size generate a sizing event
    //  -> the sizing event generate a call to AdjustScrollbars...
    if (m_bGuard) return;
    m_bGuard = true;

    //compute
    //vertical scrolling   : 1 line per scroll unit
    int iHeight = GetUniformRowHeight();
    iHeight = GetRowHeightWithMargins(iHeight);
    if (iHeight == 0) iHeight = 1; //prevent division by 0

    //horizontal scrolling : 1 pixel per scroll unit
    int iVirtualWidth = sVirtualSize.GetWidth();
    int iHPos   = 0;
    int iHPage  = 0;
    int iHRange = 0;

    //loop for vertical and horizontal scrollbars
    int i;
    int iMax = 100; //3 should be sufficient, but to be safe
    for(i=0;i<iMax;i++)
    {
        //get current client size of SCROLLER window
        wxSize sOldSize = GetTargetSize();

        //adjust target size for best case scenario : no scrollbars shown at all
        wxSize sIdealSize = GetScrollerVisibleSize();
        if (sIdealSize != sOldSize)
        {
            if (sIdealSize.GetWidth() > sVirtualSize.GetWidth()) sOldSize.SetWidth(sIdealSize.GetWidth());
        }

        //vertical scrolling : update page size
        int iVPos   = m_pScroller->GetScrollPos(wxVERTICAL);
        int iVRange = uiNbItems;
        int iVPage  = sOldSize.GetHeight() / iHeight; //fully visible
        if (iVPage >= iVRange)
        {
            //no scrolling needed
            iVPos   = 0;
            iVPage  = 0;
            iVRange = 0;
        }
        else
        {
            //vertical scrolling needed
            if (iVPage < 1) iVPage = 1;
            int iVPosMax = iVRange - iVPage;
            if (iVPos > iVPosMax) iVPos = iVPosMax;
            else if (iVPos < 0) iVPos = 0;
        }

        //horizontal scrolling : update range & page size
        int iClientWidth = sOldSize.GetWidth();
        if (iClientWidth < iVirtualWidth)
        {
            //client size too small - needs scrolling
            iHPos   = m_pScroller->GetScrollPos(wxHORIZONTAL);
            iHPage  = iClientWidth;
            iHRange = iVirtualWidth;
        }
        else
        {
            //client size big enough - no scrolling necessary
            iHPos   = 0;
            iHPage  = 0;
            iHRange = 0;
        }

        //set the scrollbars
        SetScrollbars(iHPos, iHPage, iHRange, iVPos, iVPage, iVRange);

        //get new client size of SCROLLER window
        wxSize sNewSize = GetTargetSize();

        //no change in client size : we are done
        if (sOldSize == sNewSize) break;
    }

    //store virtual size
    m_sVirtualSize = sVirtualSize;
    m_iNbVisiblesItems = uiNbItems;

    //editor
    ScrollEditorWindow();

    //reentrency
    m_bGuard = false;
}

/** Adjust the scrollbars
  */
void wxVirtualTreeModelRenderer::AdjustScrollbars(void)
{
    //vertical scrollbars : count the amount of items visible
    size_t uiNbItems = 0;
    wxSize sVirtualSize = ComputeSize(uiNbItems);
    SetAllScrollbars(sVirtualSize, uiNbItems);
    RecomputeFirstVisibleItem();
}

/** Update the scrollbars
  */
void wxVirtualTreeModelRenderer::UpdateScrollbars(void)
{
    AdjustScrollbars();
}

/** Convert a line number to a tree path
  * \param iLine [input] : the line number to convert
  * \return the tree path of the item
  */
wxVirtualTreePath wxVirtualTreeModelRenderer::ConvertRowToTreePath(int iLine) const
{
    wxVirtualTreePath oPath;
    if (!m_pClient) return(oPath);
    wxVirtualIDataModel  *pDataModel  = m_pClient->GetDataModel();
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();

    //it seems to be faster to compute the item using next/previous than absolute calculation,
    //even for big differences
    int iDiff = iLine - m_iFirstVisibleLine; //m_iFirstVisibleLine == -1 has a special meaning : not initialized
    if (iDiff < 0) iDiff = -iDiff; //abs
    const int uiDiffThreshold = 10000000;
    if ((iDiff > uiDiffThreshold) && (m_iFirstVisibleLine >= 0))
    {
        //METHOD 1 : can be slow if number of direct children of an item is big (> 10000)
        //root has rank 0, and is hidden
        //so line 0 is actually rank 1
        if (m_bHideRootItem) oPath.Set(iLine + 1, pDataModel, pStateModel);
        else                 oPath.Set(iLine, pDataModel, pStateModel);
    }
    else
    {
        //METHOD 2 : use temporal coherence to avoid worst case scenario when amount of direct children
        //is big (> 10000). This assumes that the previous computed item is close to the one we are looking for
        //If this is not the case, this is equivalent to METHOD 1 in term of speed
        size_t uiRank      = iLine;
        size_t uiStartRank = m_iFirstVisibleLine;

        //root has rank 0, and is hidden so line 0 is actually rank 1
        if (m_bHideRootItem)
        {
            uiRank++;
            uiStartRank++;
        }

        wxVirtualItemID idStartItem = m_FirstVisibleItem.Get(pDataModel);

        if (m_iFirstVisibleLine == -1)
        {
            uiStartRank = 0;
            if (pDataModel) idStartItem = pDataModel->GetRootItem();
            if (m_bHideRootItem)
            {
                uiStartRank++;
                if (pDataModel) idStartItem = pDataModel->GetChild(idStartItem, 0);
            }
        }
        idStartItem.SetModel(pDataModel);

        bool bIsRootItem = idStartItem.IsRootItem();
        if (pDataModel) bIsRootItem = pDataModel->IsRootItem(idStartItem);

        if ((bIsRootItem) && (m_iFirstVisibleLine <= 0) && (m_bHideRootItem))
        {
            if (pDataModel) idStartItem = pDataModel->GetChild(idStartItem, 0);
        }

        oPath.Set(uiRank, pDataModel, pStateModel, idStartItem, uiStartRank);
    }

    return(oPath);
}

/** Compute the 1st visible item
  * \param iFirstLine [input]: the index of the 1st visible line
  */
void wxVirtualTreeModelRenderer::ComputeFirstVisibleItem(int iFirstLine)
{
    m_FirstVisibleItem  = ConvertRowToTreePath(iFirstLine);
    m_iFirstVisibleLine = iFirstLine;

    //PrintItemID(m_FirstVisibleItem.Get(), m_FirstVisibleItem.GetLevel(), "1st line", iFirstLine);
}

/** Shift the 1st visible item up or down
  * \param iDiff [input] : the amount of items which have been hidden or shown above the 1st visible item
  *                        < 0 means some items have been collapsed / hidden
  *                        > 0 means some items have been expanded / shown
  */
void wxVirtualTreeModelRenderer::ShiftFirstVisibleItem(int iDiff)
{
    if (m_iFirstVisibleLine >= 0) m_iFirstVisibleLine += iDiff;
    if (m_iFirstVisibleLine < -1) m_iFirstVisibleLine  = -1; //protection against out of bound
    //no need to re-compute the m_FirstVisibleItem because it is rank independent
}

/** Recompute the 1st visible item
  */
void wxVirtualTreeModelRenderer::RecomputeFirstVisibleItem(void)
{
    //check
    if (!m_pClient)
    {
        InvalidateFirstVisibleItem();
        return;
    }

    //get models
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if ((!pDataModel) || (!pStateModel))
    {
        InvalidateFirstVisibleItem();
        return;
    }

    //get 1st current item
    m_FirstVisibleItem.SetDataModel(pDataModel);
    wxVirtualItemID idFirst = m_FirstVisibleItem.Get();
    if (idFirst.IsInvalid())
    {
        InvalidateFirstVisibleItem();
        return;
    }

    //recompute rank
    m_iFirstVisibleLine = pDataModel->GetItemRank(idFirst, pStateModel);
    if ((m_bHideRootItem) && (m_iFirstVisibleLine > 0)) m_iFirstVisibleLine--;
}

/** Invalidate the 1st visible item
  */
void wxVirtualTreeModelRenderer::InvalidateFirstVisibleItem(void)
{
    m_iFirstVisibleLine = -1;
    ComputeFirstVisibleItem(0);
}

/** Scroll the editor window
  */
void wxVirtualTreeModelRenderer::ScrollEditorWindow(void)
{
    //for editor : it is faster to hide it, compute the rect and show it again
    if (!m_bIsEditing) return;
    if (!m_pEditor)    return;

    wxWindow *pEditor = m_pEditor->GetEditorWindow();
    if (!pEditor) return;

    bool bIsShown = pEditor->IsShown();
    pEditor->Hide();

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return;

    wxRect rRect;
    wxVirtualItemID idFocused = pStateModel->GetFocusedItem();
    GetItemRect(rRect, idFocused, m_uiColFocusedItem);
    AdjustItemRect(rRect, idFocused);

    pEditor->SetSize(rRect);

    if (bIsShown) pEditor->Show();
}

/** Check if the item is visible
  * \param rID     [input]: the item to check
  * \param uiCol   [input]: the view column of the item to check
  * \param bCheckField [input]: if true, the horizontal visibility of the field is checked
  * \return true if the item is visible, false otherwise
  */
bool wxVirtualTreeModelRenderer::IsItemVisible(const wxVirtualItemID &rID,
                                               size_t uiCol, bool bCheckField) const
{
    //check & init
    if (!m_pClient) return(false);

    //get models
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //client size
    int w = 0;
    int h = 0;
    m_pClient->GetClientSize(&w, &h);

    //compute vertical margin
    int iVerticalMargin = m_iMarginTop + m_iMarginBottom;
    int iPenWidth       = m_HorizontalRulesPen.GetWidth();
    if (!m_bDrawHoritontalRules) iPenWidth = 0;
    int iVerticalSize   = iVerticalMargin + iPenWidth;

    //item size
    int iHeight = -1;
    if (m_bUniformRowHeight)
    {
        iHeight = GetUniformRowHeight();
    }

    //1st item
    wxVirtualItemID id = m_FirstVisibleItem.Get(pDataModel);
    size_t uiLevel     = m_FirstVisibleItem.GetLevel();
    wxCoord y = 0;

    //state
    wxVirtualDataViewItemState oState;
    if (!pStateModel) oState.SetDefault();

    //deal with root item
    if (m_bHideRootItem)
    {
        if (uiLevel > 0) uiLevel--;
        if (pDataModel->IsRootItem(id))
        {
            id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
            if (uiLevel > 0) uiLevel--;
        }
    }

    //scan visible items
    while (y < h)
    {
        if (!id.IsOK()) break;

        //is item COMPLETLY visible in height ?
        if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();

        y += iHeight;
        y += iVerticalSize; //margin + horizontal grid pen
        if (y >= h) break;

        //check the item
        if (id == rID)
        {
            //the item itself is visible. now check the field
            if (!bCheckField) return(true);


            const wxVirtualDataViewColumnsList &rvColumns = Columns();
            size_t uiNbCols = rvColumns.GetColumnsCount();
            size_t i;
            wxCoord x = 0;
            wxCoord xMax;
            for(i = 0; i < uiNbCols; i++)
            {
                //get column
                wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(i);
                if (!pCol) continue;
                if (pCol->IsHidden()) continue;

                //get width & bounds
                int iWidth = pCol->GetWidth();
                xMax = x + iWidth;

                //is it the correct field ?
                if (pCol->GetModelColumn() == uiCol)
                {
                    //check range
                    if ((x > 0) && (x <= h)) return(true);
                    if ((xMax > 0) && (xMax <= h)) return(true);
                    return(false);
                }

                //next
                x = xMax;
            }
            return(false); //field not found
        }

        //next item
        id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel);
    }

    return(false);
}

/** Ensure the item is visible (with expanding when needed)
  * \param rID     [input]: the item to make visible
  * \param uiCol   [input]: the view column of the item to make visible
  * \param eFlag   [input]: how to make the item visible
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::EnsureItemVisible(const wxVirtualItemID &rID,
                                                   size_t uiCol,
                                                   EVisibilityFlags eFlag)
{
    //expand all parents if needed
    if (!m_pClient) return(false);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //we have to expand from top to bottom, otherwise item ranks will be computed incorrectly
    wxStack<wxVirtualItemID> oStackOfNodes;
    wxVirtualItemID id = pDataModel->GetParent(rID);
    while (!id.IsRootOrInvalid())
    {
        if (!pStateModel->IsExpanded(id)) oStackOfNodes.push(id);
        id = pDataModel->GetParent(id);
    }

    //expand
    while (oStackOfNodes.size() > 0)
    {
        id = oStackOfNodes.top();
        oStackOfNodes.pop();

        size_t uiRank = pDataModel->GetItemRank(id, pStateModel);
        Expand(id, false, false, true, uiRank);
    }

    //make visible
    return(MakeItemVisible(rID, uiCol, eFlag));
}

/** Get the 1st visible item
  * \return the 1st visible item
  */
wxVirtualItemID wxVirtualTreeModelRenderer::GetFirstVisibleItem(void) const
{
    wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR;
    if (m_pClient)
    {
        pDataModel = m_pClient->GetDataModel();
    }
    return(m_FirstVisibleItem.Get(pDataModel));
}

/** Get all visible items
  * \param vIDs [input]: the visible items. Previous content is lost
  * \return the amount of visible items in vIDs
  */
size_t wxVirtualTreeModelRenderer::GetAllVisibleItems(wxVirtualItemIDs &vIDs) const
{
    vIDs.clear();
    if (!m_pClient) return(0);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(0);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(0);

    //client size
    int w = 0;
    int h = 0;
    m_pClient->GetClientSize(&w, &h);

    //item size
    int iHeight = -1;
    if (m_bUniformRowHeight)
    {
        iHeight = GetUniformRowHeight();
    }

    //compute vertical margin
    int iVerticalMargin = m_iMarginTop + m_iMarginBottom;
    int iPenWidth       = m_HorizontalRulesPen.GetWidth();
    if (!m_bDrawHoritontalRules) iPenWidth = 0;
    int iVerticalSize   = iVerticalMargin + iPenWidth;

    wxVirtualItemID id = m_FirstVisibleItem.Get(pDataModel);
    int y = 0;
    while (y < h)
    {
        if (!id.IsOK()) break;

        //add
        vIDs.push_back(id);

        //get size
        if (!m_bUniformRowHeight) iHeight = GetItemSize(id).GetHeight();

        //next
        y += iHeight;
        y += iVerticalSize; //margin + horizontal grid pen

        //next item
        id = pDataModel->NextItem(id, pStateModel);
    }

    return(vIDs.size());
}

/** Make the item visible (without expanding)
  * \param rID     [input]: the item to make visible
  * \param uiCol   [input]: the view column of the item to make visible
  * \param eFlag   [input]: how to make the item visible
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::MakeItemVisible(const wxVirtualItemID &rID,
                                                 size_t uiCol,
                                                 EVisibilityFlags eFlag)
{
    //get models
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //get rank
    size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);

    //make visible
    return(MakeItemVisible(rID, uiRank, uiCol, eFlag, true));
}

/** Make the item visible (without expanding)
  * \param rID      [input]: the item to make visible
  * \param uiRank   [input]: the rank of the item
  * \param uiCol    [input]: the view column of the item to make visible
  * \param eFlag    [input]: how to make the item visible
  * \param bRefresh [input]: true for refreshing the display after scrolling
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::MakeItemVisible(const wxVirtualItemID &rID,
                                                 size_t uiRank, size_t uiCol,
                                                 EVisibilityFlags eFlag, bool bRefresh)
{
    //check
    if (!m_pClient) return(false);
    if (!m_pScroller) return(false);

    //get scrollbars ranges
    size_t uiStartPos = m_pScroller->GetScrollPos(wxVERTICAL);
    size_t uiRange    = m_iNbVisiblesItems; //m_pScroller->GetScrollRange(wxVERTICAL);
    size_t uiPage     = m_pScroller->GetScrollThumb(wxVERTICAL);

    if ((m_bHideRootItem) && (uiRank > 0)) uiRank--;
    switch(eFlag)
    {
        case WX_VISIBLE_AT_TOP      :   //make it the 1st item
                                        uiStartPos = uiRank;
                                        if (uiStartPos + uiPage > uiRange) uiStartPos = uiRange - uiPage;
                                        ScrollTo(uiStartPos, uiCol, bRefresh);
                                        break;

        case WX_VISIBLE_AT_BOTTOM   :   //make it the last item
                                        if (uiRank >= uiPage) ScrollTo(uiRank - uiPage + 1, uiCol, bRefresh);
                                        else                 ScrollTo(0, uiCol, bRefresh);
                                        break;

        case WX_VISIBLE_DONT_CARE   :   //make it visible
        default                     :   if (IsItemVisible(rID, uiCol)) return(true);
                                        if (uiRank <= uiStartPos)  ScrollTo(uiRank, uiCol, bRefresh); //make it to the top
                                        else if (uiRank >= uiPage) ScrollTo(uiRank - uiPage + 1, uiCol, bRefresh); //make it to the bottom
                                        else                       ScrollTo(0, uiCol, bRefresh); //make it somewhere near the top
                                        break;
    }

    return(true);
}

/** Update scrollbar positions
  * \param uiVerticalPos [input]: the new vertical bar position
  * \param uiCol         [input]: the view column which must be visible (horizontal bar is adjusted)
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::ScrollTo(size_t uiVerticalPos, size_t uiCol,
                                          bool bRefresh)
{
    if (!m_pClient) return(false);
    if (!m_pScroller) return(false);

    //vertical scrollbar
    size_t uiPageSize = m_pScroller->GetScrollThumb(wxVERTICAL);
    size_t uiRange    = m_iNbVisiblesItems; //m_pScroller->GetScrollRange(wxVERTICAL);
    size_t uiLast     = uiRange - uiPageSize;
    if (uiPageSize > uiRange) uiLast = uiRange;

    size_t uiVPos = uiVerticalPos;
    if (uiVPos > uiLast) uiVPos = uiLast;

    m_pScroller->SetScrollPos(wxVERTICAL, uiVPos, true);
    ComputeFirstVisibleItem(uiVPos);

    wxScrollWinEvent rVertEvent(wxEVT_SCROLLWIN_THUMBRELEASE, uiVPos, wxVERTICAL);
    m_pClient->SendScrollEvent(rVertEvent);
    m_pClient->UpdateDisplay();
    m_pClient->RefreshHeaders();

    //horizontal scrollbar
    wxVirtualDataViewColumnsList &rvColumns = Columns();
    size_t uiNbCols = rvColumns.GetColumnsCount();
    size_t i;
    wxCoord x = 0;
    wxCoord xMax = 0;
    for(i = 0; i < uiNbCols; i++)
    {
        wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(i);
        if (!pCol) continue;
        //if (pCol->IsHidden()) continue;

        int iWidth = pCol->GetWidth();
        xMax = x + iWidth;

        if (pCol->GetModelColumn() == uiCol)
        {
            //field is between x and xMax. Now check the horizontal view
            int w = 0, h = 0;
            m_pClient->GetClientSize(&w, &h);
            int iHPos = m_pScroller->GetScrollPos(wxHORIZONTAL);
            if ((iHPos + w < x) || (iHPos > x + w))
            {
                iHPos = x;
                wxScrollWinEvent rHorzEvent(wxEVT_SCROLLWIN_THUMBRELEASE, iHPos, wxHORIZONTAL);
                m_pClient->SendScrollEvent(rHorzEvent);
            }
            break;
        }

        x = xMax;
    }

    //refresh
    if (bRefresh) Refresh();
    return(true);
}

/** Update vertical scrollbar position and select object
  * \param uiRank        [input]: the rank of the item to make visible
  * \param bControlDown  [input]: true if the control key is held down
  * \param bShiftDown    [input]: true if the shift key is held down
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::MoveTo(size_t uiRank,
                                        bool bControlDown,
                                        bool bShiftDown)
{
    //get models
    if (!m_pClient) return(false);
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //get item ID
    wxVirtualItemID id = pDataModel->FindItem(uiRank, pStateModel);

    //scroll
    return(MoveTo(id, uiRank, bControlDown, bShiftDown));
}

/** Update vertical scrollbar position and select object
  * \param uiRank        [input]: the rank of the item to make visible
  * \param bControlDown  [input]: true if the control key is held down
  * \param bShiftDown    [input]: true if the shift key is held down
  * \return true on success, false on failure
  */
bool wxVirtualTreeModelRenderer::MoveTo(const wxVirtualItemID &rID, size_t uiRank,
                                        bool bControlDown,
                                        bool bShiftDown)
{
    //check bounds
    if ((m_bHideRootItem) && (uiRank == 0)) return(true);
    if (!m_pScroller) return(false);
    //if ((int) uiRank > m_pScroller->GetScrollRange(wxVERTICAL)) return(true);
    if ((int) uiRank > m_iNbVisiblesItems) return(true);

    //scroll
    MakeItemVisible(rID, uiRank, m_uiColFocusedItem, WX_VISIBLE_DONT_CARE, false);

    //select and make focused
    PerformSelection(rID, m_uiFieldFocusedItem, m_uiColFocusedItem, uiRank, bControlDown, bShiftDown, true, true);

    //refresh
    //Refresh();
    return(true);
}

//-------------- RESIZING COLUMNS -----------------------------------//

/** Check and perform columns resizing when needed
  * \param ht [input] : hit test result of the mouse event
  * \param rEvent [input]: the mouse event information
  * \return true if the event was processed and not skipped
  *         false if further processing is needed
  */
bool wxVirtualTreeModelRenderer::ResizeColumn(const THitTest &ht, wxMouseEvent &rEvent)
{
    //double click on column separator
    if (rEvent.LeftDClick())
    {
        if (ht.m_uiFlags & E_HIT_TEST_ON_LEFT_EDGE)
        {
            size_t uiCol = ht.m_uiColIndex;
            if (uiCol > 0) uiCol--;
            m_pClient->AutoSizeColumn(uiCol);
            return(true);
        }
        else if (ht.m_uiFlags & E_HIT_TEST_ON_RIGHT_EDGE)
        {
            size_t uiCol = ht.m_uiColIndex;
            m_pClient->AutoSizeColumn(uiCol);
            return(true);
        }
    }

    //if ((rEvent.Dragging()) && (rEvent.LeftDown()))
    if (m_bDraggingColumn)
    {
        if (rEvent.LeftUp())
        {
            m_bDraggingColumn = false;

            size_t uiCol  = m_iDraggedColumn;
            int iColStart = Columns().GetColumnStart(uiCol) - m_pScroller->GetScrollPos(wxHORIZONTAL);
            int iNewWidth = ht.m_x - iColStart;
            m_pClient->ResizeColumn(uiCol, iNewWidth);
        }
        else
        {
            size_t uiCol  = m_iDraggedColumn;
            int iColStart = Columns().GetColumnStart(uiCol) - m_pScroller->GetScrollPos(wxHORIZONTAL);
            int iNewWidth = ht.m_x - iColStart;
            m_pClient->ResizingColumn(uiCol, iNewWidth);
        }
        return(true);

    }
    else if (rEvent.Dragging())
    {
        if (ht.m_uiFlags & E_HIT_TEST_ON_LEFT_EDGE)
        {
            size_t uiCol = ht.m_uiColIndex;
            if (uiCol > 0) uiCol--;
            int iColStart = Columns().GetColumnStart(uiCol) - m_pScroller->GetScrollPos(wxHORIZONTAL);
            int iNewWidth = ht.m_x - iColStart;

            m_bDraggingColumn = true;
            m_iDraggedColumn  = uiCol;

            m_pClient->StartResizingColumn(uiCol, iNewWidth);
            return(true);
        }
        else if (ht.m_uiFlags & E_HIT_TEST_ON_RIGHT_EDGE)
        {
            size_t uiCol = ht.m_uiColIndex;
            int iColStart = Columns().GetColumnStart(uiCol) - m_pScroller->GetScrollPos(wxHORIZONTAL);
            int iNewWidth = ht.m_x - iColStart;

            m_bDraggingColumn = true;
            m_iDraggedColumn  = uiCol;
            m_pClient->StartResizingColumn(uiCol, iNewWidth);
            return(true);
        }
    }
    return(false);
}

//--------------------------- INTERNAL METHODS ----------------------//
/** Initialize the renderer
  */
void wxVirtualTreeModelRenderer::Init(void)
{
    m_bGuard = false;

    m_bHasFocus = false;

    m_iMarginLeft   = 1;
    m_iMarginRight  = 1;
    m_iMarginTop    = 1;
    m_iMarginBottom = 1;

    m_iRowBorderThickness   = 5;
    m_iColumnBorderThickness = 5;

    m_bAlternatingBackColour = true;
    m_bFixedAlternatingBackColour = true;
    m_cRowColourEven.Set(200, 200, 200);
    m_cRowColourOdd.Set(255, 255, 255);

    m_bShowSelectedItemsAsFocused = true;
    m_cSelectedItemsColour = wxNullColour;

    m_bDrawHoritontalRules = true;
    m_bDrawVerticalRules   = true;

    wxColour cColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
    m_bHideRootItem  = true;
    m_TreeLinesPen   = InitTreeLinesPen();
    m_eTreeLinesStyle = E_TREE_LINES_AT_EXPANDER; //E_TREE_LINES_AT_ITEM E_TREE_LINES_AT_EXPANDER
    m_iIndentation    = 15;
    //m_TreeButtonSize  = wxSize(9, 9);
    SetTreeButtonSize(wxSize(-1, -1));
    m_iTreeButtonMargins[0] = 1;
    m_iTreeButtonMargins[1] = 1;
    m_iTreeButtonMargins[2] = 1;
    m_iTreeButtonMargins[3] = 1;
    m_bDrawTreeButtons = true;
    ComputeTreeButtonSize();

    SetCheckBoxSize(wxSize(-1, -1));
    m_iCheckboxMargins[0] = 1;
    m_iCheckboxMargins[1] = 1;
    m_iCheckboxMargins[2] = 1;
    m_iCheckboxMargins[3] = 1;
    m_bDrawCheckbox = true;
    ComputeCheckBoxSize();

    m_HorizontalRulesPen = wxPen(cColour, 1, wxPENSTYLE_SOLID);
    m_VerticalRulesPen   = wxPen(cColour, 1, wxPENSTYLE_SOLID);

    m_bUniformRowHeight = true;
    m_iRowHeight        = -1;

    m_bUniformColWidth  = true;
    m_iColWidth         = -1;

    m_uiRankFocusedItem = 0;
    m_uiColFocusedItem  = 0;
    m_uiFieldFocusedItem = 0;
    InvalidateFirstVisibleItem();
    InvalidateVirtualSize();

    m_bIsEditing = false;
    m_pEditor    = WX_VDV_NULL_PTR;

    if (m_pClient) m_sCurrentClientSize = m_pClient->GetClientSize();
    else           m_sCurrentClientSize.Set(0, 0);

    m_bDraggingColumn = false;
}

/** Create the tree lines pen
  * \return the tree lines pen
  */
wxPen wxVirtualTreeModelRenderer::InitTreeLinesPen(void) const
{
    #ifdef __WXMSW__
    {
        int i, j;
        wxBitmap bmp(8, 8);
        wxMemoryDC bdc;
        bdc.SelectObject(bmp);
        bdc.SetPen(*wxGREY_PEN);
        bdc.DrawRectangle(-1, -1, 10, 10);
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (!((i + j) & 1))
                {
                    bdc.DrawPoint(i, j);
                }
            }
        }

        return(wxPen(bmp, 1));
    }
    #else
    wxColour cColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
    return(wxPen(cColour, 1, wxPENSTYLE_SOLID));
    #endif // __WXMSW__
}

//-------------------------- MARGINS --------------------------------//
/** Get the left margin, in pixels
  * \return the left margin, in pixels
  */
int wxVirtualTreeModelRenderer::GetLeftMargin(void) const
{
    return(m_iMarginLeft);
}

/** Get the right margin, in pixels
  * \return the right margin, in pixels
  */
int wxVirtualTreeModelRenderer::GetRightMargin(void) const
{
    return(m_iMarginRight);
}

/** Get the top margin, in pixels
  * \return the top margin, in pixels
  */
int wxVirtualTreeModelRenderer::GetTopMargin(void) const
{
    return(m_iMarginTop);
}


/** Get the bottom margin, in pixels
  * \return the bottom margin, in pixels
  */
int wxVirtualTreeModelRenderer::GetBottomMargin(void) const
{
    return(m_iMarginBottom);
}

/** Set the left margin, in pixels
  * \param iMargin [input]: the new margin, in pixels
  */
void wxVirtualTreeModelRenderer::SetLeftMargin(int iMargin)
{
    m_iMarginLeft = iMargin;
    if (m_iMarginLeft < 0) m_iMarginLeft = 0;
    OnLayoutChanged();
}

/** Set the left margin, in pixels
  * \param iMargin [input]: the new margin, in pixels
  */
void wxVirtualTreeModelRenderer::SetRightMargin(int iMargin)
{
    m_iMarginRight = iMargin;
    if (m_iMarginRight < 0) m_iMarginRight = 0;
    OnLayoutChanged();
}

/** Set the left margin, in pixels
  * \param iMargin [input]: the new margin, in pixels
  */
void wxVirtualTreeModelRenderer::SetTopMargin(int iMargin)
{
    m_iMarginTop = iMargin;
    if (m_iMarginTop < 0) m_iMarginTop = 0;
    OnLayoutChanged();
}

/** Set the left margin, in pixels
  * \param iMargin [input]: the new margin, in pixels
  */
void wxVirtualTreeModelRenderer::SetBottomMargin(int iMargin)
{
    m_iMarginBottom = iMargin;
    if (m_iMarginBottom < 0) m_iMarginBottom = 0;
    OnLayoutChanged();
}

//------------------ BORDER THICKNESS -------------------------------//
/** Get the row border thickness (for picking)
  * \return the size of the row border in pixels (on each side of the border)
  */
int wxVirtualTreeModelRenderer::GetRowBorderThickness(void) const
{
    return(m_iRowBorderThickness);
}

/** Set the row border thickness (for picking)
  * \param iThickness [input]: the size of the row border in pixels (on each side of the border)
  */
void wxVirtualTreeModelRenderer::SetRowBorderThickness(int iThickness)
{
    m_iRowBorderThickness = iThickness;
    if (m_iRowBorderThickness < 0) m_iRowBorderThickness = 0;
}

/** Get the column border thickness (for picking)
  * \return the size of the column border in pixels (on each side of the border)
  */
int wxVirtualTreeModelRenderer::GetColumnBorderThickness(void) const
{
    return(m_iColumnBorderThickness);
}

/** Set the column border thickness (for picking)
  * \param iThickness [input]: the size of the column border in pixels (on each side of the border)
  */
void wxVirtualTreeModelRenderer::SetColumnBorderThickness(int iThickness)
{
    m_iColumnBorderThickness = iThickness;
    if (m_iColumnBorderThickness < 0) m_iColumnBorderThickness = 0;
}

//---------------------- GRID LINES ---------------------------------//
/** Return true if horizontal grid lines are drawn, false otherwise
  * \return true if horizontal grid lines are drawn, false otherwise
  */
bool wxVirtualTreeModelRenderer::DrawHorizontalGridLines(void) const
{
    return(m_bDrawHoritontalRules);
}

/** Set/unset drawing of horizontal grid lines
  * \param bDraw [input]: true for drawing the lines, false for hiding them
  */
void wxVirtualTreeModelRenderer::DrawHorizontalGridLines(bool bDraw)
{
    m_bDrawHoritontalRules = bDraw;
    OnLayoutChanged();
}

/** Get the pen used to draw the horizontal grid lines
  * \return the pen used to draw the horizontal grid lines
  */
wxPen wxVirtualTreeModelRenderer::GetHorizontalGridLinesPen(void) const
{
    return(m_HorizontalRulesPen);
}

/** Set the pen used to draw the horizontal grid lines
  * \param pen [input] : the new pen used to draw the horizontal grid lines
  */
void wxVirtualTreeModelRenderer::SetHoritontalGridLinesPen(wxPen pen)
{
    m_HorizontalRulesPen = pen;
    OnLayoutChanged();
}

/** Return true if vertical grid lines are drawn, false otherwise
  * \return true if vertical grid lines are drawn, false otherwise
  */
bool wxVirtualTreeModelRenderer::DrawVerticalGridLines(void) const
{
    return(m_bDrawVerticalRules);
}

/** Set/unset drawing of vertical grid lines
  * \param bDraw [input]: true for drawing the lines, false for hiding them
  */
void wxVirtualTreeModelRenderer::DrawVerticalGridLines(bool bDraw)
{
    m_bDrawVerticalRules = bDraw;
    OnLayoutChanged();
}

/** Get the pen used to draw the vertical grid lines
  * \return the pen used to draw the vertical grid lines
  */
wxPen wxVirtualTreeModelRenderer::GetVerticalGridLinesPen(void) const
{
    return(m_VerticalRulesPen);
}

/** Set the pen used to draw the vertical grid lines
  * \param pen [input] : the new pen used to draw the vertical grid lines
  */
void wxVirtualTreeModelRenderer::SetVerticalGridLinesPen(wxPen pen)
{
    m_VerticalRulesPen = pen;
    OnLayoutChanged();
}

//--------------- TREE BUTTONS & LINES ------------------------------//
/** Check if root item is hidden
  * \return true if the root item is hidden, false otherwise
  */
bool wxVirtualTreeModelRenderer::HideRootItem(void) const
{
    return(m_bHideRootItem);
}

/** Hide or show root item
  * \param bHide [input]: true for hiding the root item, false to show it
  */
void wxVirtualTreeModelRenderer::HideRootItem(bool bHide)
{
    m_bHideRootItem = bHide;
}

/** Show root item
  */
void wxVirtualTreeModelRenderer::ShowRootItem(void)
{
    m_bHideRootItem = false;
}

/** Get the pen used to draw lines connecting nodes
  * \return the pen used to draw the tree lines
  */
wxPen wxVirtualTreeModelRenderer::GetTreeLinesPen(void) const
{
    return(m_TreeLinesPen);
}

/** Set the pen used to draw lines connecting nodes
  * \param pen [input]: the new pen used to draw the tree lines
  */
void wxVirtualTreeModelRenderer::SetTreeLinesPen(wxPen pen)
{
    m_TreeLinesPen = pen;
}

/** Check if lines connecting nodes are drawn
  * \return true if tree lines are drawn, false otherwise
  */
wxVirtualTreeModelRenderer::ETreeLinesStyle wxVirtualTreeModelRenderer::GetTreeLinesStyle(void) const
{
    return(m_eTreeLinesStyle);
}

/** Set if lines connecting nodes are drawn
  * E_TREE_LINES_AT_ITEM : horizontal line goes from middle of button to left of item. Classical, Win32
  *  example: Indent = 3
  *  LEVEL
  *    0       +--ITEM 1
  *    1       |  |--CHILD 1
  *    1       |  +--CHILD 2
  *    2       |     |-- CHILD 3
  *    0       +--ITEM 2
  *    1          |--CHILD 4
  *
  * E_TREE_LINES_AT_EXPANDER: horizontal line goes from middle of PARENT button to left of item.
  *  example: indent = 3
  *   LEVEL
  *    0       +--ITEM 1
  *    1       |--+--CHILD 1
  *    1       |--+--CHILD 2
  *    2       |  |--+--CHILD 3
  *    0       +--+--ITEM 2
  *    1          |--CHILD 4
  * \param bDraw [input]: true for drawing the tree lines, false for hiding them
  */
void wxVirtualTreeModelRenderer::SetTreeLinesStyle(ETreeLinesStyle eTreeLinesStyle)
{
    m_eTreeLinesStyle = eTreeLinesStyle;
}

/** Get the indentation size
  * \return the amount of pixels to indent per tree level
  */
int wxVirtualTreeModelRenderer::GetIdentationSize(void) const
{
    return(m_iIndentation);
}

/** Set the indentation size
  * \param iIndent [input]: the amount of pixels to indent per tree level
  */
void wxVirtualTreeModelRenderer::SetIdentationSize(int iIndent)
{
    m_iIndentation = iIndent;
    OnLayoutChanged();
}

/** Get the tree button size
  * \return the size of the tree button (without margins)
  */
wxSize wxVirtualTreeModelRenderer::GetTreeButtonSize(void) const
{
    return(m_TreeButtonSize);
}

/** Set the tree button size
  * \param s [input]: the size of the tree button (without margins)
  */
void wxVirtualTreeModelRenderer::SetTreeButtonSize(wxSize s)
{
    m_TreeButtonSize = s;
    if ((m_TreeButtonSize.GetWidth() < 0) || (m_TreeButtonSize.GetHeight() < 0))
    {
        if (m_TreeButtonSize.GetWidth() < 0) m_TreeButtonSize.SetWidth(9);
        if (m_TreeButtonSize.GetHeight() < 0) m_TreeButtonSize.SetHeight(9);
    }

    ComputeTreeButtonSize();
}

/** Get the tree button margins
  * \return the tree button margins (left, right, top & bottom)
  */
int wxVirtualTreeModelRenderer::GetTreeButtonMargins(void) const
{
    return(m_iTreeButtonMargins[0]);
}

/** Set the tree button margins (left, right, top, bottom)
  * \param iMargin [input] : the tree button margins (left, right, top & bottom)
  */
void wxVirtualTreeModelRenderer::SetTreeButtonMargins(int iMargin)
{
    m_iTreeButtonMargins[0] = iMargin;
    m_iTreeButtonMargins[1] = iMargin;
    m_iTreeButtonMargins[2] = iMargin;
    m_iTreeButtonMargins[3] = iMargin;
    ComputeTreeButtonSize();
}

/** Compute the width of the tree button, with margins
  */
void wxVirtualTreeModelRenderer::ComputeTreeButtonSize(void)
{
    m_iTreeButtonTotalWidth  = m_iTreeButtonMargins[0];
    m_iTreeButtonTotalWidth += m_TreeButtonSize.GetWidth();
    m_iTreeButtonTotalWidth += m_iTreeButtonMargins[1];

    if (!m_bDrawTreeButtons) m_iTreeButtonTotalWidth = 0;
}

//----------------------- BACKGROUND COLOURS ------------------------//
/** return true if alternating row back colours are used
  * \return true if alternating row back colours are used
  */
bool wxVirtualTreeModelRenderer::UseAlternatingRowBackColours(void) const
{
    return(m_bAlternatingBackColour);
}

/** Return true if view dependent back colours are used
  * \return true if view dependent back colours are used
  *         false if row dependent back colours are used
  * In "view dependent" mode, the first VISIBLE row is always the same colour
  * then the 2nd VISIBLE row has the 2nd colour, ...
  * In "row dependent" mode, the top row has always the same colour, regardless if
  * it is visible or not.
  * So when scrolling vertically, the colours do not move in "view dependent" mode,
  * but they do move in the "row dependent" mode
  */
bool wxVirtualTreeModelRenderer::UseViewDependentRowBackColours(void) const
{
    return(m_bFixedAlternatingBackColour);
}

/** Set alternating row back colours style
  * \param bAlternate [input]: true for alternating back row colours
  *                            false for disabling it
  * \param bViewDependent [input]: true for "view dependent" rows colours
  *                                false for "row dependent" rows colours
  *                                Ignored if bAlternate is false.
  *                                See UseViewDependentRowBackColours() doc for more info
  */
void wxVirtualTreeModelRenderer::SetAlternatingRowBackColours(bool bAlternate,
                                                              bool bViewDependent)
{
    m_bAlternatingBackColour = bAlternate;
    m_bFixedAlternatingBackColour = bViewDependent;
}

/** Get the alternating row back colours
  * \param cEven [output]: the colour for "Even" rows
  * \param cOdd  [output]: the colour for "Odd" rows
  */
void wxVirtualTreeModelRenderer::GetAlternatingBackColours(wxColour &cEven,
                                                           wxColour &cOdd) const
{
    cEven = m_cRowColourEven;
    cOdd  = m_cRowColourOdd;
}

/** Set the alternating row back colours
  * \param cEven [output]: the colour for "Even" rows
  * \param cOdd  [output]: the colour for "Odd" rows
  */
void wxVirtualTreeModelRenderer::SetAlternatingBackColours(wxColour cEven,
                                                           wxColour cOdd)
{
    m_cRowColourEven = cEven;
    m_cRowColourOdd  = cOdd;
}

//--------------------- SELECTED ITEMS ------------------------------//
/** Check if selected items are always shown as focused
  * \return true if selected items are always shown as focused
  *         false if the actual focused state is used to compute the colour
  */
bool wxVirtualTreeModelRenderer::AreSelectedItemsShownAsFocused(void) const
{
    return(m_bShowSelectedItemsAsFocused);
}

/** Set if selected items are always shown as focused
  * \param bAsFocused [input]: true if selected items are always shown as focused
  *                            false if the actual focused state is used to compute the colour
  */
void wxVirtualTreeModelRenderer::SetSelectedItemsShownAsFocused(bool bAsFocused)
{
    m_bShowSelectedItemsAsFocused = bAsFocused;
}

/** Get the selected items colour
  * \return the selected items colour. If invalid, the native toolkit decides it
  */
wxColour wxVirtualTreeModelRenderer::GetSelectedItemsColour(void) const
{
    return(m_cSelectedItemsColour);
}

/** Set the selected items colour
  * \param cColour [input]: the new selected items colour. If valid (IsOk() == true)
  *                         then this colour will be used.
  *                         if IsOk() == false, then the default colour is used
  */
void wxVirtualTreeModelRenderer::SetSelectedItemsColour(wxColour cColour)
{
    m_cSelectedItemsColour = cColour;
}

//--------------------- CHECKBOX ------------------------------------//
/** Return true if checkboxes are drawn
  * \return true if checkboxes are drawn
  */
bool wxVirtualTreeModelRenderer::AreCheckBoxesDrawn(void) const
{
    return(m_bDrawCheckbox);
}

/** Hide / show checkboxes. No refresh of display is done
  * \param bDraw [input]: true for showing checkboxes
  *                       false for hiding them
  */
void wxVirtualTreeModelRenderer::DrawCheckBoxes(bool bDraw)
{
    m_bDrawCheckbox = bDraw;
}

/** Hide checkboxes. No refresh of display is done
  */
void wxVirtualTreeModelRenderer::HideCheckBoxes(void)
{
    m_bDrawCheckbox = false;
}

/** Get the check box size
  * \return the checkbox size
  */
wxSize wxVirtualTreeModelRenderer::GetCheckBoxSize(void) const
{
    return(m_CheckboxSize);
}

/** Set the check box size
  * \param s [input]: the checkbox size
  *                   If one of the member is <0 then it is computed automatically
  */
void wxVirtualTreeModelRenderer::SetCheckBoxSize(wxSize s)
{
    m_CheckboxSize = s;
    if ((m_CheckboxSize.GetWidth() < 0) || (m_CheckboxSize.GetHeight() < 0))
    {
        wxRendererNative &r = wxRendererNative::GetDefault();
        wxSize sDefault = r.GetCheckBoxSize(m_pClient);
        if (m_CheckboxSize.GetWidth() < 0) m_CheckboxSize.SetWidth(sDefault.GetWidth());
        if (m_CheckboxSize.GetHeight() < 0) m_CheckboxSize.SetHeight(sDefault.GetHeight());
    }
    ComputeCheckBoxSize();
}

/** Get the check box margins
  * \return the checkbox margins (left, right, top & bottom)
  */
int wxVirtualTreeModelRenderer::GetCheckBoxMargins(void) const
{
    return(m_iCheckboxMargins[0]);
}

/** Set the check box margins (left, right, top, bottom)
  * \param iMargin [input]: the new margins
  */
void wxVirtualTreeModelRenderer::SetCheckBoxMargins(int iMargin)
{
    m_iCheckboxMargins[0] = iMargin;
    m_iCheckboxMargins[1] = iMargin;
    m_iCheckboxMargins[2] = iMargin;
    m_iCheckboxMargins[3] = iMargin;
    ComputeCheckBoxSize();
}

/** Compute the size of the check box, with margins
  */
void wxVirtualTreeModelRenderer::ComputeCheckBoxSize(void)
{
    m_iCheckboxTotalWidth  = m_iCheckboxMargins[0];
    m_iCheckboxTotalWidth += m_CheckboxSize.GetWidth();
    m_iCheckboxTotalWidth += m_iCheckboxMargins[1];

    if (!m_bDrawCheckbox) m_iCheckboxTotalWidth = 0;
}

//-------------------- UNIFORM ROW HEIGHT ---------------------------//
/** Return true if the tree has uniform row heights
  * \return true if the tree has uniform row heights
  *         false if the rows have variable sizes
  */
bool wxVirtualTreeModelRenderer::HasUniformRowHeight(void) const
{
    return(m_bUniformRowHeight);
}

/** Set/unset uniform row heights
  * \param bHasUniformRowHeight [input]: true if the tree has uniform row heights
  *                                      false if the rows have variable sizes
  */
void wxVirtualTreeModelRenderer::SetUniformRowHeight(bool bHasUniformRowHeight)
{
    m_bUniformRowHeight = bHasUniformRowHeight;
}

/** Unset uniform row heights
  */
void wxVirtualTreeModelRenderer::SetVariableRowHeight(void)
{
    m_bUniformRowHeight = false;
}

/** Set the uniform row height value (without margins)
  * \param iHeight [input]: the new height, in pixels.
  *                         a negative value means that the row height will be automatically computed
  */
void wxVirtualTreeModelRenderer::SetUniformRowHeightValue(int iHeight)
{
    m_iRowHeight = iHeight;
}

/** Get the uniform row height value
  * \return the uniform row height value (without margins).
  */
int wxVirtualTreeModelRenderer::GetUniformRowHeight(void) const
{
    //set by user
    if (m_iRowHeight >= 0) return(m_iRowHeight);

    //automatic calculation : get the 1st model item
    wxVirtualItemID id;
    if (m_pClient)
    {
        wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
        if (pDataModel)
        {
            wxVirtualItemID idRoot = pDataModel->GetRootItem();
            if (idRoot.IsOK()) id = idRoot;
            wxVirtualItemID idFirst = pDataModel->GetChild(idRoot, 0);
            if (idFirst.IsOK()) id = idFirst;
        }
    }

    //compute the size of the 1st model item
    int iHeight = GetItemSize(id).GetHeight();
    return(iHeight);
}

/** Get the row height with margins
  * \param iHeightWithoutMargins [input]: the row height without margins
  * \return the row height with the margins and grid lines width
  */
int wxVirtualTreeModelRenderer::GetRowHeightWithMargins(int iHeightWithoutMargins) const
{
    int iResult = iHeightWithoutMargins;
    iResult += m_iMarginTop + m_iMarginBottom;
    if (m_bDrawHoritontalRules) iResult += m_HorizontalRulesPen.GetWidth();
    return(iResult);
}

//-------------------- UNIFORM COLUMN WIDTHS ------------------------//
/** Return true if the tree has uniform column widths
  * \return true if the tree has uniform column widths
  *         false if the columns have variable sizes
  */
bool wxVirtualTreeModelRenderer::HasUniformColWidth(void) const
{
    return(m_bUniformColWidth);
}

/** Set/unset uniform column widths
  * \param bHasUniformColWidth [input]: true if the tree has uniform column widths
  *                                      false if the columns have variable sizes
  */
void wxVirtualTreeModelRenderer::SetUniformColWidth(bool bHasUniformColWidth)
{
    m_bUniformColWidth = bHasUniformColWidth;
}

/** Unset uniform column widths
  */
void wxVirtualTreeModelRenderer::SetVariableColWidth(void)
{
    m_bUniformColWidth = false;
}

/** Set the uniform column width value (without margins)
  * \param iWidth [input]: the new width, in pixels.
  *                        a negative value means that the column width will be automatically computed
  */
void wxVirtualTreeModelRenderer::SetUniformColWidthValue(int iWidth)
{
    m_iColWidth = iWidth;
}

/** Get the uniform column width value
  * \return the uniform column width value (without margins).
  */
int wxVirtualTreeModelRenderer::GetUniformColWidth(void) const
{
    //set by user
    if (m_iColWidth >= 0) return(m_iColWidth);

//    //automatic calculation : get the 1st model item
//    if (!m_pClient) return(0);
//    wxVirtualIDataModel *pModel = m_pClient->GetDataModel();
//    if (!pModel) return(0);
//
//    wxVirtualItemID idRoot = pModel->GetRootItem();
//    wxVirtualItemID idFirst = pModel->GetChild(idRoot, 0);
//    if (!idFirst.IsOK()) return(0);
//
//    //compute the size of the 1st model item
//    int iWidth = GetItemSize(idFirst).GetWidth();

    //automatic calculation : get the size of all the columns
    const wxVirtualDataViewColumnsList &rColumns = GetControlWindow()->Columns();
    int iWidth = rColumns.GetTotalWidth();
    return(iWidth);
}

/** Get the column width with margins
  * \param iColWidthWithoutMargins [input]: the column width without margins
  * \return the column height width the margins and grid lines width
  */
int wxVirtualTreeModelRenderer::GetColWidthWithMargins(int iColWidthWithoutMargins) const
{
    int iResult = iColWidthWithoutMargins;
    iResult += m_iMarginLeft + m_iMarginRight;
    if (m_bDrawVerticalRules) iResult += m_VerticalRulesPen.GetWidth();
    return(iResult);
}

//----------------------- COLUMNS -----------------------------------//
/** Get the list of columns
  * \return a reference to the list of columns
  */
wxVirtualDataViewColumnsList& wxVirtualTreeModelRenderer::Columns(void)
{
    if (!GetControlWindow())
    {
        static wxVirtualDataViewColumnsList s_list;
        return(s_list);
    }
    return(GetControlWindow()->Columns());
}

/** Get the list of columns
  * \return a const reference to the list of columns
  */
const wxVirtualDataViewColumnsList& wxVirtualTreeModelRenderer::Columns(void) const
{
    if (!GetControlWindow())
    {
        static wxVirtualDataViewColumnsList s_list;
        return(s_list);
    }
    return(GetControlWindow()->Columns());
}

//---------------------- SELECTION ----------------------------------//
WX_DECLARE_HASH_SET(wxVirtualItemID, wxVirtualItemIDHash, wxVirtualItemIDEqual, TSetOfIDs);

/** Helper: convert an array to a hash set
  * \param oSetOfIDs [output]: the set of item IDs
  * \param vIDs      [input] : the array of item IDs
  */
WX_VDV_INLINE void VectorToHashSet(TSetOfIDs &oSetOfIDs, const wxVirtualItemIDs &vIDs)
{
    oSetOfIDs.clear();
    size_t i, uiCount;
    uiCount = vIDs.size();
    for(i = 0; i < uiCount; i++)
    {
        oSetOfIDs.insert(vIDs[i]);
    }
}

/** Helper : convert a hash set to an array
  * \param vIDs      [output]: the array of item IDs
  * \param oSetOfIDs [input] : the set of item IDs
  */
WX_VDV_INLINE void HashSetToVector(wxVirtualItemIDs &vIDs, const TSetOfIDs &oSetOfIDs)
{
    vIDs.clear();
    vIDs.reserve(oSetOfIDs.size());
    TSetOfIDs::const_iterator it    = oSetOfIDs.begin();
    TSetOfIDs::const_iterator itEnd = oSetOfIDs.end();
    while (it != itEnd)
    {
        vIDs.push_back(*it);
        ++it;
    }
}

/** Helper : get a range of selected items
  */
WX_VDV_INLINE void GetRangeOfItem(wxVirtualItemIDs &vIDs, wxVirtualIStateModel *pStateModel,
                           const wxVirtualItemID &rID1, const wxVirtualItemID &rID2)
{
    vIDs.clear();

    if (!pStateModel) return;
    wxVirtualIDataModel *pDataModel = rID1.GetTopModel();
    if (!pDataModel) return;

    wxVirtualItemID id = rID1;
    size_t uiLevel = 1;
    do
    {
        if (!id.IsOK()) return;
        vIDs.push_back(id);
        id = pDataModel->NextItem(id, uiLevel, uiLevel, pStateModel, 1);

    } while(id != rID2);
}

/** Do the selection
  * \param rID              [input]: ID of the item to select / deselect
  * \param uiField          [input]: the field of the item receiving the focus (model coord)
  * \param uiCol            [input]: the view column of the item receiving the focus
  * \param uiRank           [input]: rank of the item to select / deselect
  * \param bAddOrRemove     [input]: true if the item is selected / deselected without touching the other items
  * \param bSelectRange     [input]: true if a range of items is selected. Ignored if bAddOrRemove is true
  * \param bSendEvent       [input]: true if events are sent
  * \param bSelect          [input]: true for selecting, false for deselecting. Ignored if bAddOrRemove is true
  * \return true if successful, false otherwise
  *         true may be returned even if the selection has not changed at all
  */
bool wxVirtualTreeModelRenderer::PerformSelection(const wxVirtualItemID &rID,
                                                  size_t uiField,
                                                  size_t uiCol,
                                                  size_t uiRank,
                                                  bool bAddOrRemove,
                                                  bool bSelectRange,
                                                  bool bSendEvent,
                                                  bool bSelect)
{
    //get state model
    if (!m_pClient) return(false);
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //get old selection
    wxVirtualItemIDs vIDsOld;
    TSetOfIDs oSetOfIDs;
    if (bSendEvent) pStateModel->GetSelectedItems(vIDsOld);

    //compute new selection
    wxVirtualItemIDs vIDsNew;

    bool bChanged = false;
    if ((!rID.IsOK()) || (rID.IsInvalid()))
    {
        //send message
        if (bSendEvent)
        {
            if (!m_pClient->SendSelectionChangingEventByCopy(vIDsOld, vIDsNew)) return(true);
        }

        //deselect all
        pStateModel->DeselectAll();

        if (bSendEvent) m_pClient->SendSelectionChangedEvent(vIDsOld, vIDsNew);
        bChanged = true;

        //set focus item (here it is set to an invalid item. This is not a mistake. This
        //is to avoid edition of an item after deselecting it. Example :
        //      1 - select Item A => focused item = Item A
        //      2 - select invalid item => focused item is still Item A
        //      3 - select Item A again => edition of Item A will be triggered, which is not what the user expects
        // So we need to break Step 2)
        SetItemFocus(rID, uiField, uiCol, uiRank, bSendEvent);
    }
    else if (bAddOrRemove)
    {
        //add to / remove from current selection
        bool bIsSelected = pStateModel->IsSelected(rID);

        //send message
        if (bSendEvent)
        {
            VectorToHashSet(oSetOfIDs, vIDsOld);
            if (bIsSelected) oSetOfIDs.erase(rID); else oSetOfIDs.insert(rID);
            HashSetToVector(vIDsNew, oSetOfIDs);

            if (!m_pClient->SendSelectionChangingEventByCopy(vIDsOld, vIDsNew)) return(true);
        }

        //set selection and send message
        if (pStateModel->Select(rID, !bIsSelected, false)) bChanged = true;
        if (bSendEvent) m_pClient->SendSelectionChangedEvent(vIDsOld, vIDsNew);

        //set focus item
        SetItemFocus(rID, uiField, uiCol, uiRank, bSendEvent);
    }
    else if (bSelectRange)
    {
        //change range of selected elements
        wxVirtualItemID idFocused = pStateModel->GetFocusedItem();
        pStateModel->DeselectAll();
        if (idFocused.IsOK())
        {
            //change the range of selected element but do not change the list of selected item
            size_t uiRankFocused = m_uiRankFocusedItem; //idFocused.GetModel()->GetItemRank(idFocused, pStateModel);
            if ((m_bHideRootItem) && (uiRankFocused > 0)) uiRankFocused--;
            if (uiRankFocused <= uiRank)
            {
                if (bSendEvent)
                {
                    GetRangeOfItem(vIDsNew, pStateModel, idFocused, rID);
                    if (!m_pClient->SendSelectionChangingEventByCopy(vIDsOld, vIDsNew)) return(true);
                }

                pStateModel->SelectRange(idFocused, rID, bSelect);
                if (bSendEvent) m_pClient->SendSelectionChangedEvent(vIDsOld, vIDsNew);
            }
            else
            {
                if (bSendEvent)
                {
                    GetRangeOfItem(vIDsNew, pStateModel, rID, idFocused);
                    if (!m_pClient->SendSelectionChangingEventByCopy(vIDsOld, vIDsNew)) return(true);
                }

                pStateModel->SelectRange(rID, idFocused, bSelect);
            }
            bChanged = true;
        }
        else
        {
            //select the item and focus it

            //send selection changing event
            if (bSendEvent)
            {
                vIDsNew.push_back(rID);
                if (!m_pClient->SendSelectionChangingEventByCopy(vIDsOld, vIDsNew)) return(true);
            }

            //select and send event
            if (pStateModel->Select(rID, bSelect, false)) bChanged = true;
            if (bSendEvent) m_pClient->SendSelectionChangedEvent(vIDsOld, vIDsNew);

            //set focus item
            SetItemFocus(rID, uiField, uiCol, uiRank, bSendEvent);
        }
    }
    else
    {
        //set new selection

        //send selection changing event
        if (bSendEvent)
        {
            vIDsNew.push_back(rID);
            if (!m_pClient->SendSelectionChangingEventByCopy(vIDsOld, vIDsNew)) return(true);
        }

        //set new selection and send event
        if (pStateModel->DeselectAll()) bChanged = true;
        if (pStateModel->Select(rID, bSelect, false)) bChanged = true; //select this item
        if (bSendEvent) m_pClient->SendSelectionChangedEvent(vIDsOld, vIDsNew);

        //set focus item
        SetItemFocus(rID, uiField, uiCol, uiRank, bSendEvent);
    }

    if (bChanged) Refresh();
    return(true);
}

/** Shortcut for toggling one item
  * \param rID [input] : the item to toggle
  * \param uiField [input]: the field of the item to select
  * \param uiCol   [input]: the view column index of the item to select
  * \param bToggle [input]: true for toggling the item state, false for selecting it
  * \param bSelect [input]: true for selecting the item, false for deselecting it
  *                         ignored if bToggle is true
  * \param bSendEvent [input] : true for sending events
  * \return true if successful, false otherwise
  *         true may be returned even if the selection has not changed at all (vetoed)
  */
bool wxVirtualTreeModelRenderer::SelectItem(const wxVirtualItemID &rID, size_t uiField,
                                            size_t uiCol,
                                            bool bToggle, bool bSelect, bool bSendEvent)
{
    if (!m_pClient) return(false);
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);
    return(PerformSelection(rID, uiField, uiCol, uiRank, bToggle, false, bSendEvent, bSelect));
}

/** Activate an item
  * \param rID     [input]: the ID of the item to activate
  * \param uiField [input]: the field of the item to activate
  * \param uiCol   [input]: the view column index of the item to select
  * \param bSelect [input]: true if the item should be selected as well
  * \param bSendEvent [input] : true for sending events
  * \return true if successful, false otherwise
  *         true may be returned even if the selection has not changed at all (vetoed)
  */
bool wxVirtualTreeModelRenderer::ActivateItem(const wxVirtualItemID &rID, size_t uiField,
                                              size_t uiCol, bool bSelect,
                                              bool bSendEvent)
{
    //basic check
    if (!m_pClient) return(false);
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    //get rank of item
    size_t uiRank = pDataModel->GetItemRank(rID, pStateModel);

    //select if necessary or just set focus
    if (bSelect)
    {
        //select and set focus
        PerformSelection(rID, uiField, uiCol, uiRank, false, false, bSendEvent, true);
    }
    else
    {
        //set focus only
        SetItemFocus(rID, uiField, uiCol, uiRank, bSendEvent);
    }

    //send activation event
    if (bSendEvent) m_pClient->SendActivationEvent(rID, uiField, uiCol);
    return(true);
}

/** Change the focused item
  * \param rID        [input]: the item receiving the focus
  * \param uiField    [input]: the field of the item receiving the focus (model coord)
  * \param uiCol      [input]: the column index of the item receiving the focus (view coord)
  * \param uiRank     [input]: the rank of the item receiving the focus
  * \param bSendEvent [input]: true if events are sent
  * \return true if processed, false otherwise
  */
bool wxVirtualTreeModelRenderer::SetItemFocus(const wxVirtualItemID &rID, size_t uiField,
                                              size_t uiCol, size_t uiRank, bool bSendEvent)
{
    //get state model
    if (!m_pClient) return(false);
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);

    if (bSendEvent)
    {
        m_pClient->SendItemFocusedEvent(pStateModel->GetFocusedItem(),
                                        m_uiFieldFocusedItem, m_uiColFocusedItem,
                                        false);
    }

    pStateModel->SetFocus(rID);
    m_uiRankFocusedItem = uiRank;
    m_uiFieldFocusedItem = uiField;
    m_uiColFocusedItem = uiCol;

    if (bSendEvent) m_pClient->SendItemFocusedEvent(rID, uiField, uiCol, true);
    Refresh();
    return(true);
}

/** Update the rank of the focused item
  */
void wxVirtualTreeModelRenderer::UpdateFocusedItemRank(void)
{
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return;
    wxVirtualItemID idFocused = pStateModel->GetFocusedItem();

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return;
    m_uiRankFocusedItem = pDataModel->GetItemRank(idFocused, pStateModel);
}

//---------------------- TOOLTIP ------------------------------------//
/** Set the tooltip data
  * \param ht [input]: the mouse event hit test information
  */
void wxVirtualTreeModelRenderer::SetToolTip(const THitTest &ht)
{
    if ((ht.m_ID.IsOK() && ((ht.m_uiFlags & E_HIT_TEST_OUTSIDE_ITEM) == 0)))
    {
        wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
        if (pDataModel)
        {
            wxVariant vToolTip;
            vToolTip = pDataModel->GetItemData(ht.m_ID, ht.m_uiField,
                                               wxVirtualIDataModel::WX_ITEM_TOOLTIP_DATA);
            if (vToolTip.IsNull()) m_pClient->SetToolTip((wxToolTip *) WX_VDV_NULL_PTR);
            else                   m_pClient->SetToolTip(vToolTip.GetString());

            //get context help
            wxVariant vHelpText;
            vHelpText = pDataModel->GetItemData(ht.m_ID, ht.m_uiField,
                                                wxVirtualIDataModel::WX_ITEM_HELP_DATA);
            if (vHelpText.IsNull()) m_pClient->SetHelpText("");
            else                    m_pClient->SetHelpText(vHelpText.GetString());
        }
        else
        {
            m_pClient->SetToolTip((wxToolTip *) WX_VDV_NULL_PTR);
        }
    }
    else
    {
        m_pClient->SetToolTip((wxToolTip *) WX_VDV_NULL_PTR);
    }
}

//------------------------- ITEM EDITION ----------------------------//
/** Adjust item rect for taking into account indentation
  * \param r    [input/output]: the rectangle to modify
  * \param rID  [input]       : the ID of the item
  */
void wxVirtualTreeModelRenderer::AdjustItemRect(wxRect &r, const wxVirtualItemID &rID)
{
    if (!IsExpanderColumn(m_uiColFocusedItem)) return;

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    size_t uiLevel = pDataModel->GetDepth(rID);

    if ((m_bHideRootItem) && (uiLevel > 0)) uiLevel--;
    long   lIndent = uiLevel * m_iIndentation + m_iTreeButtonTotalWidth;

    if ((pDataModel->HasCheckbox(rID)) && (m_bDrawCheckbox)) lIndent += m_iCheckboxTotalWidth;

    r.x += lIndent;
    r.width -= lIndent;
}

/** Edit the focused item
  * \param pStateModel  [input]: the current state model
  * \param rID          [input]: the item which was clicked / activated
  * \param uiField      [input]: the field which was clicked / activated (model coord)
  * \param uiCol        [input]: the column which was clicked / activated (view coord)
  * \param ptPos        [input]: point where the edition occurred (click)
  * \param uiRank       [input]: rank of the item to select / deselect
  * \param bAddOrRemove [input]: true if the item is selected / deselected without touching the other items
  * \param bSelectRange [input]: true if a range of items is selected. Ignored if bAddOrRemove is true
  * \return true if edition of an item was performed, false otherwise
  *         true is returned in the following scenarios:
  *             1 - there was an item currently edited
  *             2 - the cell clicked / activated is the same as the focused one
  *                 and edition started
  */
bool wxVirtualTreeModelRenderer::EditItem(wxVirtualIStateModel *pStateModel,
                                          const wxVirtualItemID &rID, size_t uiField, size_t uiCol,
                                          wxPoint ptPos, size_t uiRank, bool bAddOrRemove,
                                          bool bSelectRange)
{
    //end edition of current item
    //check if an item is currently edited, and if yes, try to finish it
    wxVirtualItemID idFocused = pStateModel->GetFocusedItem();
    if (EndCurrentEdition(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem)) return(true);

    //activation first
    if (ActivateCell(rID, uiField, uiCol, ptPos, uiRank, bAddOrRemove, bSelectRange)) return(true);

    //there was no edition. Try to edit the focused item
    //start edition if click on focused cell
    bool bSameCell =   (idFocused == rID)
                    && (m_uiColFocusedItem == uiCol)
                    && (idFocused.IsOK());
    if (!bSameCell) return(false);

    if (EditFocusedItem(rID, uiField, uiCol))
    {
        PerformSelection(rID, uiField, uiCol, uiRank, bAddOrRemove, bSelectRange, true, true);
        return(true);
    }

    //no edition to do
    return(false);
}

/** Cancel the current edition
  * \return true there are no more editions performed
  *         false if an editor is still opened
  */
bool wxVirtualTreeModelRenderer::CancelCurrentEdition(void)
{
    return(EndCurrentEdition(true));
}

/** Finish the current edition
  * \param bCancel [input]: true for cancelling current edition
  * \return true there are no more editions performed
  *         false if an editor is still opened
  */
bool wxVirtualTreeModelRenderer::EndCurrentEdition(bool bCancel)
{
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    wxVirtualItemID idFocused = pStateModel->GetFocusedItem();

    EndCurrentEdition(idFocused, m_uiFieldFocusedItem, m_uiColFocusedItem, bCancel);
    if (m_pEditor) return(false);
    return(!m_bIsEditing);
}

/** Finish the current edition
  * \param rID         [input]: the item which is currently edited
  * \param uiField     [input]: the field which is currently edited (model coord)
  * \param uiCol       [input]: the column which is currently edited (view coord)
  * \param bCancel [input]: true for cancelling current edition
  * \return true if there was an item currently being edited
  *         false otherwise
  */
bool wxVirtualTreeModelRenderer::EndCurrentEdition(const wxVirtualItemID &rID, size_t uiField, size_t uiCol,
                                                   bool bCancel)
{
    if (!m_bIsEditing) return(false);

    if (EndEdition(m_pEditor, rID, uiField, uiCol, bCancel))
    {
        //end edition allowed
        m_pEditor = WX_VDV_NULL_PTR;
        m_bIsEditing = false;
    }
    //else : end edition forbidden.

    return(true);
}

/** Edit the focus item
  * \param rID         [input]: the item to edit
  * \param uiField     [input]: the field of the item to edit (model coord)
  * \param uiCol       [input]: the column to edit (view coord)
  * \return true if edition was started successfully
  *         false otherwise
  */
bool wxVirtualTreeModelRenderer::EditFocusedItem(const wxVirtualItemID &rID, size_t uiField, size_t uiCol)
{
    //standard edition
    m_pEditor = StartEdition(rID, uiField, uiCol);
    if (m_pEditor)
    {
        m_bIsEditing = true;
        return(true);
    }

    m_bIsEditing = false;
    return(false);
}

/** Start editing an item
  * \param rID     [input]: the ID of the item to edit
  * \param uiField [input]: the field of the item to edit (model coord)
  * \param uiCol  [input]: the column to edit (view coord)
  * \return the editor if edition is allowed
  *         a null pointer if the edition is forbidden
  */
wxVirtualDataViewEditor* wxVirtualTreeModelRenderer::StartEdition(const wxVirtualItemID &rID,
                                                                  size_t uiField, size_t uiCol)
{
    //is column editable ?
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(uiCol);
    if (!pCol) return(WX_VDV_NULL_PTR);
    if (pCol->IsHidden()) return(WX_VDV_NULL_PTR);
    if (!pCol->IsEditable()) return(WX_VDV_NULL_PTR);

    //is item disabled ?
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(WX_VDV_NULL_PTR);
    if (!pStateModel->IsEnabled(rID)) return(WX_VDV_NULL_PTR);

    //is item editable
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(WX_VDV_NULL_PTR);
    wxVirtualIDataModel::EFlags eItemFlags = pDataModel->GetItemFlags(rID, uiField);
    bool bItemEditable = eItemFlags & wxVirtualIDataModel::WX_ITEM_FLAGS_EDITABLE;
    if (!bItemEditable) return(WX_VDV_NULL_PTR);

    //edition allowed ?
    bool bEditionAllowed;
    bEditionAllowed = m_pClient->SendStartEditingEvent(rID, uiField, uiCol);
    if (!bEditionAllowed) return(WX_VDV_NULL_PTR);

    //start editing
    //1 : get current cell value
    wxVariant vValue;
    vValue = pDataModel->GetItemData(rID, uiField,
                                     wxVirtualIDataModel::WX_ITEM_MAIN_DATA);

    //2 : get editor
    wxVirtualDataViewEditor* pEditor = pCol->GetEditor();
    if (!pEditor)  return(WX_VDV_NULL_PTR);

    //3: start editor
    wxRect r;
    GetItemRect(r, rID, uiCol);
    AdjustItemRect(r, rID);

    //the pEditor->StartEditing will send a SetFocus event on the editor.
    //That means the current window having the focus (likely this one) will receive a wxEVT_KILL_FOCUS event
    //BUT the wxEVT_KILL_FOCUS will be processed in the next event loop (this is a POST event - asynchronous)
    //SO the kill focus event will be processed right after the editor has been shown - which is not what we want
    if (m_pClient->HasFocus()) m_bFocusGuard = true;
    bool bEditingStarted = pEditor->StartEditing(rID, uiField,
                                                 vValue, m_pClient, r);
    if (!bEditingStarted) return(WX_VDV_NULL_PTR);

    //send event
    m_pClient->SendEditionStartedEvent(rID, uiField, uiCol);
    return(pEditor);
}

/** End editing an item
  * \param pEditor      [input]: the editor used to edit the cell
  * \param rID          [input]: the ID of the edited item
  * \param uiField      [input]: the field of the edited item (model coord)
  * \param uiCol        [input]: the view column index of the edited item (view coord)
  * \param bCancel      [input]: true for cancelling current edition
  * \return true if edition of the item succeeded
  *         false otherwise
  */
bool wxVirtualTreeModelRenderer::EndEdition(wxVirtualDataViewEditor *pEditor,
                                            const wxVirtualItemID &rID,
                                            size_t uiField, size_t uiCol,
                                            bool bCancel)
{
    //end edition
    if (!pEditor) return(true);

    //check if we can finish the current edition
    if (!pEditor->CanFinishEdition())
    {
        //editor prevented to finish the edition
        return(false);
    }

    //send end of edition event
    m_pClient->SendEditingEndEvent(rID, uiField, uiCol, bCancel);

    if (!bCancel)
    {
        //get the value
        wxVariant vValue;
        if (!pEditor->GetValue(vValue)) return(false);

        //change the value
        ChangeItemValue(rID, uiField, uiCol, vValue);
    }

    pEditor->FinishEdition();
    return(true);
}

/** Change the value of an item
  * \param rID      [input]: the ID of the item to modify
  * \param uiField  [input]: the field of the item to modify (model coord)
  * \param uiCol    [input]: the column of the item to modify (view coord)
  * \param rvValue  [input]: the new value
  * \return true if the value was changed successfully, false otherwise
  */
bool wxVirtualTreeModelRenderer::ChangeItemValue(const wxVirtualItemID &rID,
                                                 size_t uiField, size_t uiCol,
                                                 const wxVariant &rvValue)
{
    //is column editable ?
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(uiCol);
    if (!pCol) return(false);
    if (pCol->IsHidden()) return(false);
    if ((!pCol->IsEditable()) && (!pCol->IsActivatable())) return(false);

    //is item disabled ?
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);
    if (!pStateModel->IsEnabled(rID)) return(false);

    //is item editable
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);
    wxVirtualIDataModel::EFlags eItemFlags = pDataModel->GetItemFlags(rID, uiField);
    bool bItemEditable = eItemFlags & wxVirtualIDataModel::WX_ITEM_FLAGS_EDITABLE;
    if (!bItemEditable) return(false);

    //edition allowed ?
    bool bChangeValueAllowed;
    bChangeValueAllowed = m_pClient->SendItemValueChangingEvent(rID,
                                                               uiField,
                                                               uiCol,
                                                               rvValue);
    if (!bChangeValueAllowed) return(false);

    //set the new value
    wxVirtualIDataModel::EFlags eFlags = pDataModel->GetItemFlags(rID, uiField);
    bool bEditable = eFlags & wxVirtualIDataModel::WX_ITEM_FLAGS_EDITABLE;
    if (!bEditable) return(false);

    if (!pDataModel->SetItemData(rID, uiField, rvValue)) return(false);

    m_pClient->SendItemValueChangedEvent(rID,  uiField, uiCol, rvValue);
    return(true);
}

/** Activate a cell
  * \param rID             [input]: the ID of the item to modify
  * \param uiField          [input]: the field of the item to modify (model coord)
  * \param uiCol            [input]: the column of the item to modify (view coord)
  * \param ptPos            [input]: in case of mouse event, relative position of mouse event
  * \param uiRank           [input]: rank of the item to select / deselect
  * \param bAddOrRemove     [input]: true if the item is selected / deselected without touching the other items
  * \param bSelectRange     [input]: true if a range of items is selected. Ignored if bAddOrRemove is true
  * \return true if the item was edited by simple activation, false otherwise
  */
bool wxVirtualTreeModelRenderer::ActivateCell(const wxVirtualItemID &rID,
                                              size_t uiField, size_t uiCol,
                                              wxPoint ptPos,
                                              size_t uiRank, bool bAddOrRemove,
                                              bool bSelectRange)
{
    //end current edition
    if (!EndCurrentEdition()) return(false);

    //is column activatable ?
    wxVirtualDataViewColumn *pCol = Columns().GetColumn(uiCol);
    if (!pCol) return(false);
    if (!pCol->IsActivatable()) return(false);

    //is item disabled ?
    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(false);
    if (!pStateModel->IsEnabled(rID)) return(false);

    //from model : is item editable ?
    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(false);
    wxVirtualIDataModel::EFlags eFlags = pDataModel->GetItemFlags(rID, uiField);
    bool bEditable = eFlags & wxVirtualIDataModel::WX_ITEM_FLAGS_EDITABLE;
    if (!bEditable) return(false);

    //get editor
    wxVirtualDataViewEditor *pEditor = pCol->GetEditor();
    if (!pEditor) return(false);

    //activate
    wxRect r;
    GetItemRect(r, rID, uiCol);
    AdjustItemRect(r, rID);

    wxVariant vValue = pDataModel->GetItemData(rID, uiField);
    if (pEditor->Activate(rID, uiField, r, ptPos, vValue))
    {
        ChangeItemValue(rID, uiField, uiCol, vValue);
        m_pClient->SetFocus();
        PerformSelection(rID, uiField, uiCol, uiRank, bAddOrRemove, bSelectRange);
        return(true);
    }
    else
    {
        //cell can be activated, but editor does not support it. Just simulate edition
        EndCurrentEdition(false);
        SetItemFocus(rID, uiField, uiCol, uiRank, true);
        if (!EditFocusedItem(rID, uiField, uiCol)) return(false);
        PerformSelection(rID, uiField, uiCol, uiRank, bAddOrRemove, bSelectRange);
        return(true);
    }
    return(false);
}

//------------------------ DEBUG ------------------------------------//
/** Debug method (print item ID)
  * \param rID [input] : the item to log
  * \param uiLevel [input]: the level of the item
  * \param szMsg1  [input]: 1st informative message
  * \param iData1  [input]: data associated to 1st message
  * \param szMsg2  [input]: 2nd informative message
  * \param iData2  [input]: data associated to 2nd message
  */
void wxVirtualTreeModelRenderer::PrintItemID(const wxVirtualItemID &rID, size_t uiLevel,
                                             const char *szMsg1, int iData1,
                                             const char *szMsg2, int iData2) const
{
    int iID  = rID.GetUInt32();
    int iRow = rID.GetRow();
    int iCol = rID.GetColumn();

    const char *sz1 = szMsg1;
    int id1         = iData1;
    const char *sz2 = WX_VDV_NULL_PTR;
    int id2         = 0;
    if (!sz1) {sz1 = szMsg2; id1 = iData2;}
    else      {sz2 = szMsg2; id2 = iData2;}

    if (sz1)
    {
        if (sz2)
        {
            wxLogMessage("%s = %d %s = %d level = %d id = %d row = %d col = %d",
                         sz1, id1, sz2, id2, uiLevel, iID, iRow, iCol);
        }
        else
        {
            wxLogMessage("%s = %d level = %d id = %d row = %d col = %d",
                         sz1, id1, uiLevel, iID, iRow, iCol);
        }
    }
    else
    {
        wxLogMessage("level = %d id = %d row = %d col = %d",
                     uiLevel, iID, iRow, iCol);
    }
}
