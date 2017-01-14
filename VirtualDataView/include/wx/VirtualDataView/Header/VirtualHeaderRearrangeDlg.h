/**********************************************************************/
/** FILE    : VirtualHeaderRearrangeDlg.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : dlg box for reorder/show columns in wxVirtualHeaderCtrl**/
/**********************************************************************/

#ifndef _VIRTUAL_HEADER_REARRANGE_DIALOG_BOX_H_
#define _VIRTUAL_HEADER_REARRANGE_DIALOG_BOX_H_

#include <wx/rearrangectrl.h>

#if wxUSE_REARRANGECTRL

//------------------------- DECLARATION -----------------------------//
extern const wxString sVHRearrangeMsg;
extern const wxString sVHRearrangeTitle;

/** \class wxVirtualHeaderRearrangeDlg : a dialog box for reordering, showing or hiding columns
  * for wxVirtualHeaderCtrl
  */
class wxVirtualHeaderRearrangeDlg : public wxRearrangeDialog
{
    public:
        wxVirtualHeaderRearrangeDlg(wxWindow *pParent,
                                    const wxArrayInt &rvOrder,
                                    const wxArrayString &rvItemNames,
                                    wxString sMessage = sVHRearrangeMsg,
                                    wxString sTitle   = sVHRearrangeTitle); ///< \brief constructor
        ~wxVirtualHeaderRearrangeDlg(void);                                 ///< \brief destructor
};

#endif // wxUSE_REARRANGECTRL
#endif // _VIRTUAL_HEADER_REARRANGE_DIALOG_BOX_H_
