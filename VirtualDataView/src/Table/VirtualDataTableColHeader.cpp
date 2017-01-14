/**********************************************************************/
/** FILE    : VirtualDataTableColHeader.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header control for columns for wxVirtualDataTableCtrl  **/
/**********************************************************************/


#include <wx/VirtualDataView/Table/VirtualDataTableColHeader.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
wxVirtualDataTableColHeader::wxVirtualDataTableColHeader(void)
    : wxVirtualDataTableHeader()
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
wxVirtualDataTableColHeader::wxVirtualDataTableColHeader(wxWindow *pParent,
                                                         wxWindowID id,
                                                         const wxPoint &ptPos,
                                                         const wxSize &size,
                                                         long lStyle,
                                                         const wxString &sName)
    : wxVirtualDataTableHeader(pParent, id, ptPos, size, lStyle, sName)
{
    m_eOrientation = WX_VDV_COL_HEADER;
}

/** Destructor
  */
wxVirtualDataTableColHeader::~wxVirtualDataTableColHeader(void)
{
}
