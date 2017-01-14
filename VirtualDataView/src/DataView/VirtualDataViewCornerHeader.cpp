/**********************************************************************/
/** FILE    : VirtualDataViewCornerHeader.cpp                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header control for corner for wxVirtualDataViewCtrl    **/
/**********************************************************************/

#include <wx/VirtualDataView/DataView/VirtualDataViewCornerHeader.h>


const char wxVDViewCornerHeaderNameStr[] = "wxVirtualDataViewCornerHeader";

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualDataViewCornerHeader::wxVirtualDataViewCornerHeader(void)
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
wxVirtualDataViewCornerHeader::wxVirtualDataViewCornerHeader(wxWindow *pParent,
                                                               wxWindowID id,
                                                               const wxPoint &ptPos,
                                                               const wxSize &size,
                                                               long lStyle,
                                                               const wxString &sName)
    : wxVirtualHeaderCtrl(pParent, id, ptPos, size, lStyle, sName),
      m_pOwner(WX_VDV_NULL_PTR)
{
    m_pOwner = (wxVirtualDataViewCtrl*) pParent;
}

/** Destructor
  */
wxVirtualDataViewCornerHeader::~wxVirtualDataViewCornerHeader(void)
{
}

//--------------------- OWNER ---------------------------------------//
/** Get the owner window
  * \return the owner window
  */
wxVirtualDataViewCtrl* wxVirtualDataViewCornerHeader::GetOwner(void) const
{
    return(m_pOwner);
}

/** Set the owner window
  * \param pOwner [input]: the new owner window
  */
void wxVirtualDataViewCornerHeader::SetOwner(wxVirtualDataViewCtrl* pOwner)
{
    m_pOwner = pOwner;
}

//---------------------- INTERFACE IMPLEMENTATION -------------------//
/** Get the amount of items in the header
  * \return the amount of items in the header
  */
size_t wxVirtualDataViewCornerHeader::GetItemCount(void) const
{
    return(0);
}

/** Get the size of one item
  * \param uiItemID [input]: the index of the header item to query
  * \return the size of the item
  */
wxSize wxVirtualDataViewCornerHeader::GetItemSize(size_t uiItemID) const
{
    return(GetClientSize());
}

/** Get the item data (label, sorting arrow, styles)
  * \param uiItemID [input] : the index of the header item to query
  * \param rData    [output]: the data to fill
  * \return true on success, false on failure
  */
bool wxVirtualDataViewCornerHeader::GetItemData(size_t uiItemID, TItemData &rData) const
{
    return(false);
}

/** Get the index of the 1st visible item
  * \return the index of the 1st visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewCornerHeader::GetFirstVisibleItem(void) const
{
    return(s_uiNotFound);
}

/** Get the index of the last visible item
  * \param uiItemID [input]: the index of the current item
  * \return the index of the item after uiItemID, if visible
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewCornerHeader::GetNextVisibleItem(size_t uiItemID) const
{
    return(s_uiNotFound);
}

/** Get the last visible item
  * \return the ID of the last visible item
  *         s_uiNotFound if there are no visible items
  */
size_t wxVirtualDataViewCornerHeader::GetLastVisibleItem(void) const
{
    return(s_uiNotFound);
}

/** Get the origin of the 1st visible item (unscrolled coordinate)
  * \return the vertical offset in pixels of the 1st visible item for row headers
  *         the horizontal offset in pixels of the 1st visible item for column header
  *         This should be a NEGATIVE value
  */
wxCoord wxVirtualDataViewCornerHeader::GetFirstItemOrigin(void) const
{
    return(0);
}
