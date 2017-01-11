/**********************************************************************/
/** FILE    : VirtualDataViewRowHeader.cpp                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header for rows for wxVirtualDataViewCtrl              **/
/**********************************************************************/

#include <wx/VirtualDataView/DataView/VirtualDataViewRowHeader.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualTreeModelRenderer.h>

const char wxVDVRowHeaderCtrlNameStr[] = "wxVirtualDataViewRowHeader";

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualDataViewRowHeader::wxVirtualDataViewRowHeader(void)
    : wxVirtualHeaderCtrl(),
      m_pOwner(WX_VDV_NULL_PTR)
{
     m_eOrientation = WX_VDV_ROW_HEADER;
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
wxVirtualDataViewRowHeader::wxVirtualDataViewRowHeader(wxWindow *pParent,
                                                       wxWindowID id,
                                                       const wxPoint &ptPos,
                                                       const wxSize &size,
                                                       long lStyle,
                                                       const wxString &sName)
    : wxVirtualHeaderCtrl(pParent, id, ptPos, size, lStyle, sName),
      m_pOwner(WX_VDV_NULL_PTR)
{
    m_pOwner = (wxVirtualDataViewCtrl*) pParent;
    m_eOrientation = WX_VDV_ROW_HEADER;
}

/** Destructor
  */
wxVirtualDataViewRowHeader::~wxVirtualDataViewRowHeader(void)
{
}

//--------------------- OWNER ---------------------------------------//
/** Get the owner window
  * \return the owner window
  */
wxVirtualDataViewCtrl* wxVirtualDataViewRowHeader::GetOwner(void) const
{
    return(m_pOwner);
}

/** Set the owner window
  * \param pOwner [input]: the new owner window
  */
void wxVirtualDataViewRowHeader::SetOwner(wxVirtualDataViewCtrl* pOwner)
{
    m_pOwner = pOwner;
}

//---------------------- INTERFACE IMPLEMENTATION -------------------//
/** Get the amount of items in the header
  * \return the amount of items in the header
  */
size_t wxVirtualDataViewRowHeader::GetItemCount(void) const
{
    if (!m_pOwner) return(0);
    return(m_pOwner->GetScrollRange(wxVERTICAL));
}

/** Get the size of one item
  * \param uiItemID [input]: the index of the header item to query
  * \return the size of the item
  */
wxSize wxVirtualDataViewRowHeader::GetItemSize(size_t uiItemID) const
{
    wxVirtualHeaderIRenderer::THeaderData rData;
    GetItemData(uiItemID, (TItemData&) rData);

    wxSize s = m_MinSize;//m_pRenderer->GetHeaderButtonSize(rData); slow

    //ensure that header item track the size of the underlying column/row
    if (!m_pOwner) return(s);

    //int iHeight = m_pOwner->GetRowHeight(uiItemID);
    wxVirtualTreeModelRenderer *pRenderer = (wxVirtualTreeModelRenderer*) m_pOwner->GetModelRenderer();
    int iHeight = pRenderer->GetUniformRowHeight();
    iHeight = pRenderer->GetRowHeightWithMargins(iHeight);
    s.SetHeight(iHeight);

    return(s);
}

/** Get the item data (label, sorting arrow, styles)
  * \param uiItemID [input] : the index of the header item to query
  * \param rData    [output]: the data to fill
  * \return true on success, false on failure
  */
bool wxVirtualDataViewRowHeader::GetItemData(size_t uiItemID, TItemData &rData) const
{
    rData.m_sLabel = wxString::Format("%d", uiItemID);
    rData.m_iAlignment = wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL;
    return(true);
}

/** Get the index of the 1st visible item
  * \return the index of the 1st visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewRowHeader::GetFirstVisibleItem(void) const
{
    if (!m_pOwner) return(1);
    if (m_pOwner->IsEmpty()) return(1);
    return(m_pOwner->GetScrollPos(wxVERTICAL) + 1);
}

/** Get the index of the last visible item
  * \param uiItemID [input]: the index of the current item
  * \return the index of the item after uiItemID, if visible
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewRowHeader::GetNextVisibleItem(size_t uiItemID) const
{
    if (!m_pOwner) return(uiItemID + 1);
    if (m_pOwner->IsEmpty()) return(uiItemID + 1);
//    size_t uiLast = m_pOwner->GetScrollThumb(wxVERTICAL) + m_pOwner->GetScrollPos(wxVERTICAL) + 1;
//    if (uiItemID + 1 <= uiLast) return(uiItemID + 1);
    return(uiItemID + 1);
}

/** Get the last visible item
  * \return the ID of the last visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewRowHeader::GetLastVisibleItem(void) const
{
    if (!m_pOwner) return(s_uiNotFound);
    if (m_pOwner->IsEmpty()) return(s_uiNotFound);
    return(m_pOwner->GetScrollRange(wxVERTICAL));
}

/** Get the origin of the 1st visible item (unscrolled coordinate)
  * \return the vertical offset in pixels of the 1st visible item for row headers
  *         the horizontal offset in pixels of the 1st visible item for column header
  *         This should be a NEGATIVE value
  */
wxCoord wxVirtualDataViewRowHeader::GetFirstItemOrigin(void) const
{
    return(0);
}
