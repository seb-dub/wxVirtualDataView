/**********************************************************************/
/** FILE    : VirtualHeaderRearrangeDlg.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : dlg box for reorder/show columns in wxVirtualHeaderCtrl**/
/**********************************************************************/

#include <wx/VirtualDataView/Header/VirtualHeaderRearrangeDlg.h>

#if wxUSE_REARRANGECTRL

//-------------------- STATIC MEMBERS -------------------------------//
const wxString sVHRearrangeMsg   = _("Please select the columns to show and define their order:");
const wxString sVHRearrangeTitle = _("Customize Columns");

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Constructor
  * \param pParent      [input]: parent window
  * \param rvOrder      [input]: current order of the items
  * \param rvItemNames  [input]: name of each item (reordered)
  * \param sMessage     [input]: the message to print
  * \param sTitle       [input]: the title to print
  */
wxVirtualHeaderRearrangeDlg::wxVirtualHeaderRearrangeDlg(wxWindow *pParent,
                                                         const wxArrayInt &rvOrder,
                                                         const wxArrayString &rvItemNames,
                                                         wxString sMessage,
                                                         wxString sTitle)
        : wxRearrangeDialog(pParent,
                            sMessage, sTitle,
                            rvOrder, rvItemNames)
{

}

/** Destructor
  */
wxVirtualHeaderRearrangeDlg::~wxVirtualHeaderRearrangeDlg(void)
{
}

#endif // wxUSE_REARRANGECTRL
