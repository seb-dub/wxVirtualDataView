/**********************************************************************/
/** FILE    : VirtualDataTableRowHeader.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header control for rows for wxVirtualDataTableCtrl     **/
/**********************************************************************/


#include <wx/VirtualDataView/Table/VirtualDataTableRowHeader.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualDataTableRowHeader::wxVirtualDataTableRowHeader(void)
    : wxVirtualDataTableHeader()
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
wxVirtualDataTableRowHeader::wxVirtualDataTableRowHeader(wxWindow *pParent,
                                                         wxWindowID id,
                                                         const wxPoint &ptPos,
                                                         const wxSize &size,
                                                         long lStyle,
                                                         const wxString &sName)
    : wxVirtualDataTableHeader(pParent, id, ptPos, size, lStyle, sName)
{
    m_eOrientation = WX_VDV_ROW_HEADER;
}

/** Destructor
  */
wxVirtualDataTableRowHeader::~wxVirtualDataTableRowHeader(void)
{

}
