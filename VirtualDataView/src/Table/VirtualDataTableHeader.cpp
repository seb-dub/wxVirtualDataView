/**********************************************************************/
/** FILE    : VirtualDataTableHeader.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header for row/columns for wxVirtualDataTableCtrl      **/
/**********************************************************************/

#include <wx/VirtualDataView/Table/VirtualDataTableHeader.h>
#include <wx/VirtualDataView/Table/VirtualDataTableCtrl.h>
#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>

const char wxVDVTableHeaderCtrlNameStr[] = "wxVirtualDataTableHeader";

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualDataTableHeader::wxVirtualDataTableHeader(void)
    : wxVirtualHeaderCtrl(),
      m_pOwner(WX_VDV_NULL_PTR)
{
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
wxVirtualDataTableHeader::wxVirtualDataTableHeader(wxWindow *pParent,
                                                   wxWindowID id,
                                                   const wxPoint &ptPos,
                                                   const wxSize &size,
                                                   long lStyle,
                                                   const wxString &sName)
    : wxVirtualHeaderCtrl(pParent, id, ptPos, size, lStyle, sName),
      m_pOwner(WX_VDV_NULL_PTR)
{
    m_pOwner = (wxVirtualDataTableCtrl*) pParent;
}

/** Destructor
  */
wxVirtualDataTableHeader::~wxVirtualDataTableHeader(void)
{
}

//--------------------- OWNER ---------------------------------------//
/** Get the owner window
  * \return the owner window
  */
wxVirtualDataTableCtrl* wxVirtualDataTableHeader::GetOwner(void) const
{
    return(m_pOwner);
}

/** Set the owner window
  * \param pOwner [input]: the new owner window
  */
void wxVirtualDataTableHeader::SetOwner(wxVirtualDataTableCtrl* pOwner)
{
    m_pOwner = pOwner;
}

//---------------------- INTERFACE IMPLEMENTATION -------------------//
/** Get the amount of items in the header
  * \return the amount of items in the header
  */
size_t wxVirtualDataTableHeader::GetItemCount(void) const
{
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        return(m_pOwner->GetAmountOfViewCols());
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        return(m_pOwner->GetAmountOfViewRows());
    }
    return(0);
}

/** Get the size of one item
  * \param uiItemID [input]: the index of the header item to query
  * \return the size of the item
  */
wxSize wxVirtualDataTableHeader::GetItemSize(size_t uiItemID) const
{
    wxVirtualHeaderIRenderer::THeaderData rData;
    GetItemData(uiItemID, (TItemData&) rData);

    wxSize s = m_MinSize;//m_pRenderer->GetHeaderButtonSize(rData); slow

    //ensure that header item track the size of the underlying column/row
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        int iWidth = m_pOwner->GetColumnWidth(uiItemID);
        s.SetWidth(iWidth);
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        int iHeight = m_pOwner->GetRowHeight(uiItemID);
        s.SetHeight(iHeight);
    }

    return(s);
}

/** Get the item data (label, sorting arrow, styles)
  * \param uiItemID [input] : the index of the header item to query
  * \param rData    [output]: the data to fill
  * \return true on success, false on failure
  */
bool wxVirtualDataTableHeader::GetItemData(size_t uiItemID, TItemData &rData) const
{
    rData.m_sLabel = wxString::Format("%d", uiItemID);
    rData.m_iAlignment = wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL;
    return(true);
}

/** Get the index of the 1st visible item
  * \return the index of the 1st visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataTableHeader::GetFirstVisibleItem(void) const
{
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        return(m_pOwner->GetVisibleColumnsBegin());
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        return(m_pOwner->GetVisibleRowsBegin());
    }
    return(s_uiNotFound);
}

/** Get the index of the last visible item
  * \param uiItemID [input]: the index of the current item
  * \return the index of the item after uiItemID, if visible
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataTableHeader::GetNextVisibleItem(size_t uiItemID) const
{
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        size_t uiLast = m_pOwner->GetVisibleColumnsEnd();
        if (uiItemID + 1 <= uiLast) return(uiItemID + 1);
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        size_t uiLast = m_pOwner->GetVisibleRowsEnd();
        if (uiItemID + 1 <= uiLast) return(uiItemID + 1);
    }
    return(s_uiNotFound);
}

/** Get the last visible item
  * \return the ID of the last visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataTableHeader::GetLastVisibleItem(void) const
{
    if (m_eOrientation == WX_VDV_COL_HEADER)
    {
        return(m_pOwner->GetVisibleColumnsEnd() - 1);
    }
    else if (m_eOrientation == WX_VDV_ROW_HEADER)
    {
        return(m_pOwner->GetVisibleRowsEnd() - 1);
    }
    return(s_uiNotFound);
}

/** Get the origin of the 1st visible item (unscrolled coordinate)
  * \return the vertical offset in pixels of the 1st visible item for row headers
  *         the horizontal offset in pixels of the 1st visible item for column header
  *         This should be a NEGATIVE value
  */
wxCoord wxVirtualDataTableHeader::GetFirstItemOrigin(void) const
{
    return(0);
}
