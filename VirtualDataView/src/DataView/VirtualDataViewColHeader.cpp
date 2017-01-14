/**********************************************************************/
/** FILE    : VirtualDataViewColHeader.cpp                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header for columns for wxVirtualDataViewCtrl           **/
/**********************************************************************/

#include <wx/VirtualDataView/DataView/VirtualDataViewColHeader.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>
#include <wx/menu.h>
//#include <wx/log.h>

const char wxVDVColHeaderCtrlNameStr[] = "wxVirtualDataViewColHeader";

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualDataViewColHeader::wxVirtualDataViewColHeader(void)
    : wxVirtualHeaderCtrl(),
      m_pOwner(WX_VDV_NULL_PTR)
{
     m_eOrientation = WX_VDV_COL_HEADER;
}

/** Constructor
  * \param pParent [input]: the parent window
  * \param id      [input]: the window ID (default = wxID_ANY)
  * \param ptPos   [input]: the initial position (default = wxDefaultPosition)
  * \param size    [input]: the initial size (default = wxDefaultSize)
  * \param lStyle  [input]: the header style. See wxHeaderCtrl for more info.
  *                         default value = wxHD_DEFAULT_STYLE
  * \param sName   [input]: window name
  */
wxVirtualDataViewColHeader::wxVirtualDataViewColHeader(wxWindow *pParent,
                                                       wxWindowID id,
                                                       const wxPoint &ptPos,
                                                       const wxSize &size,
                                                       long lStyle,
                                                       const wxString &sName)
    : wxVirtualHeaderCtrl(pParent, id, ptPos, size, lStyle, sName),
      m_pOwner(WX_VDV_NULL_PTR)
{
    m_pOwner = (wxVirtualDataViewCtrl*) pParent;
    m_eOrientation = WX_VDV_COL_HEADER;
}

/** Destructor
  */
wxVirtualDataViewColHeader::~wxVirtualDataViewColHeader(void)
{
}

//--------------------- OWNER ---------------------------------------//
/** Get the owner window
  * \return the owner window
  */
wxVirtualDataViewCtrl* wxVirtualDataViewColHeader::GetOwner(void) const
{
    return(m_pOwner);
}

/** Set the owner window
  * \param pOwner [input]: the new owner window
  */
void wxVirtualDataViewColHeader::SetOwner(wxVirtualDataViewCtrl* pOwner)
{
    m_pOwner = pOwner;
}

//---------------------- INTERFACE IMPLEMENTATION -------------------//
/** Get the amount of items in the header
  * \return the amount of items in the header
  */
size_t wxVirtualDataViewColHeader::GetItemCount(void) const
{
    if (!m_pOwner) return(1);
    return(m_pOwner->Columns().GetColumnsCount());
}

/** Get the size of one item
  * \param uiItemID [input]: the index of the header item to query
  * \return the size of the item
  */
wxSize wxVirtualDataViewColHeader::GetItemSize(size_t uiItemID) const
{
    wxVirtualHeaderIRenderer::THeaderData rData;
    GetItemData(uiItemID, (TItemData&) rData);

    wxSize s = m_MinSize;//m_pRenderer->GetHeaderButtonSize(rData); slow
    if (!m_pOwner) return(s);

    //hidden column ?
    wxVirtualDataViewColumn *pCol = m_pOwner->Columns().GetColumn(uiItemID);
    if (pCol)
    {
        if (pCol->IsHidden()) return(wxSize(0, m_MinSize.GetHeight()));
    }

    //ensure that header item track the size of the underlying column/row
    int iWidth = m_pOwner->Columns().GetColumnWidth(uiItemID);
    s.SetWidth(iWidth);

    return(s);
}

/** Get the item data (label, sorting arrow, styles)
  * \param uiItemID [input] : the index of the header item to query
  * \param rData    [output]: the data to fill
  * \return true on success, false on failure
  */
bool wxVirtualDataViewColHeader::GetItemData(size_t uiItemID, TItemData &rData) const
{
    rData.m_sLabel = wxString::Format("%d", uiItemID);
    rData.m_iAlignment = wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL;

    if (m_pOwner)
    {
        wxVirtualDataViewColumn *pCol = m_pOwner->Columns().GetColumn(uiItemID);
        if (pCol)
        {
            rData.m_sLabel = pCol->GetTitle();
            bool bSort = pCol->IsSortKey();
            bool bAscending = pCol->IsSortOrderAscending();

            if (bSort)
            {
                if (bAscending) rData.m_eSortIconType = wxHDR_SORT_ICON_UP;
                else            rData.m_eSortIconType = wxHDR_SORT_ICON_DOWN;
            }
            else
            {
                rData.m_eSortIconType = wxHDR_SORT_ICON_NONE;
            }

            rData.m_bIsVisible = pCol->IsShown();
        }

        size_t uiSelectedCol = m_pOwner->GetFocusedColumn();
        if (uiSelectedCol == uiItemID) rData.m_bIsCurrent = true;
        else                           rData.m_bIsCurrent = false;


        rData.m_bIsFiltering = m_pOwner->IsFiltering(uiItemID);
    }
    return(true);
}

/** Get the index of the 1st visible item
  * \return the index of the 1st visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewColHeader::GetFirstVisibleItem(void) const
{
    if (!m_pOwner) return(s_uiNotFound);
    return(0);
}

/** Get the index of the last visible item
  * \param uiItemID [input]: the index of the current item
  * \return the index of the item after uiItemID, if visible
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewColHeader::GetNextVisibleItem(size_t uiItemID) const
{
    if (!m_pOwner) return(s_uiNotFound);
    size_t uiLast = m_pOwner->Columns().GetColumnsCount();
    if (uiItemID + 1 < uiLast) return(uiItemID + 1);
    return(s_uiNotFound);
}

/** Get the last visible item
  * \return the ID of the last visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewColHeader::GetLastVisibleItem(void) const
{
    if (!m_pOwner) return(s_uiNotFound);
    return(m_pOwner->Columns().GetColumnsCount() - 1);
}

/** Get the origin of the 1st visible item (unscrolled coordinate)
  * \return the vertical offset in pixels of the 1st visible item for row headers
  *         the horizontal offset in pixels of the 1st visible item for column header
  *         This should be a NEGATIVE value
  */
wxCoord wxVirtualDataViewColHeader::GetFirstItemOrigin(void) const
{
    if (!m_pOwner) return(0);
    int x0 = m_pOwner->GetScrollPos(wxHORIZONTAL);
    return(-x0);
}

//--------------------- UI FOR REORDERING / SHOWING / HIDING --------//
/** Update items order
  * \param rvOrder [input]: the new item order
  * \return true if reorder was successfull, false otherwise
  */
bool wxVirtualDataViewColHeader::UpdateOrder(const wxArrayInt &rvOrder)
{
    size_t uiCount = rvOrder.GetCount();
    if (uiCount != GetItemCount()) return(false);

    size_t i;
    wxVector<size_t> vNewOrder;
    vNewOrder.reserve(uiCount);
    for(i=0;i<uiCount;i++)
    {
        if (rvOrder[i] < 0) return(false);
        if (rvOrder[i] >= (int) uiCount) return(false);
        vNewOrder.push_back(rvOrder[i]);
    }

    m_pOwner->Columns().ReorderColumns(&vNewOrder.front());
    return(true);
}

/** Update item visibility
  * \param uiItemID  [input]: the item to update
  * \param bVisible  [input]: the new item visibility. True if the item is shown, false if it is hidden
  * \return true if visibility update was successfull, false otherwise
  */
bool wxVirtualDataViewColHeader::UpdateVisibility(size_t uiItemID, bool bVisible)
{
    size_t uiCount = GetItemCount();
    if (uiItemID >= uiCount) return(false);

    wxVirtualDataViewColumnsList &rvColumns = m_pOwner->Columns();
    wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(uiItemID);
    if (!pCol) return(false);
    pCol->Show(bVisible);

    return(true);
}

/** Check if an item is shown
  * \param uiItemID [input]: the ID of the item to check
  * \return true if the item is shown, false otherwise
  */
bool wxVirtualDataViewColHeader::IsItemShown(size_t uiItemID) const
{
    size_t uiCount = GetItemCount();
    if (uiItemID >= uiCount) return(false);

    wxVirtualDataViewColumnsList &rvColumns = m_pOwner->Columns();
    wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(uiItemID);
    if (!pCol) return(false);

    return(pCol->IsShown());
}

//-------------------------- FILTERING ------------------------------//
/** Get the filter editor for a specific item
  * \param uiItemID [input]: the ID of the item to query
  * \return a pointer to the filter. The filter is not owned by the header control
  */
wxVirtualDataViewIFilterEditor* wxVirtualDataViewColHeader::GetFilterEditor(size_t uiItemID) const
{
    size_t uiCount = GetItemCount();
    if (uiItemID >= uiCount) return(WX_VDV_NULL_PTR);

    wxVirtualDataViewColumnsList &rvColumns = m_pOwner->Columns();
    wxVirtualDataViewColumn *pCol = rvColumns.GetColumn(uiItemID);
    if (!pCol) return(WX_VDV_NULL_PTR);

    return(pCol->GetFilterEditor());
}

//------------------------- CONTEXTUAL MENU -------------------------//
static size_t s_uiID_RowHeaders = wxNewId() + 5000;

/** Append menu items to the contextual menu
  * \param rMenu [in/out] : the menu to modify
  */
void wxVirtualDataViewColHeader::AppendContextualMenuItems(wxMenu &rMenu)
{
    //show / hide row header
    rMenu.AppendSeparator();
    rMenu.Append(s_uiID_RowHeaders, _("&Show/hide rows header"));
}

/** Process a menu command
  * \param rMenu [input]: the contextual menu
  * \param iSelectedItem [input]: the selected item
  * \return true if the item was processed, false otherwise
  */
bool wxVirtualDataViewColHeader::ProcessMenuCommand(const wxMenu &rMenu, int iSelectedItem)
{
    //wxLogMessage("wxVirtualDataViewColHeader::ProcessMenuCommand 1");
    if (static_cast<size_t>(iSelectedItem) == s_uiID_RowHeaders)
    {
        //wxLogMessage("wxVirtualDataViewColHeader::ProcessMenuCommand 2");
        m_pOwner->ShowRowHeader(!m_pOwner->IsRowHeaderShown());
        return(true);
    }
    return(false);
}

