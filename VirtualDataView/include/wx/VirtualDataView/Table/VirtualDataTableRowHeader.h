/**********************************************************************/
/** FILE    : VirtualDataTableRowHeader.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header control for rows for wxVirtualDataTableCtrl     **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_TABLE_ROW_HEADER_H_
#define _VIRTUAL_DATA_TABLE_ROW_HEADER_H_

#include <wx/VirtualDataView/Table/VirtualDataTableHeader.h>

/** \class wxVirtualDataTableRowHeader : header control for rows for wxVirtualDataView
  */
class WXDLLIMPEXP_VDV wxVirtualDataTableRowHeader : public wxVirtualDataTableHeader
{
    public:
        //constructors & destructor
        wxVirtualDataTableRowHeader(void);                                              ///< \brief default constructor
        wxVirtualDataTableRowHeader(wxWindow *pParent,
                                    wxWindowID id         = wxID_ANY,
                                    const wxPoint &ptPos  = wxDefaultPosition,
                                    const wxSize &size    = wxDefaultSize,
                                    long lStyle           = WX_VDV_DEFAULT_STYLE,
                               const wxString &sName = wxVDVTableHeaderCtrlNameStr);    ///< \brief constructor
        virtual ~wxVirtualDataTableRowHeader(void);                                     ///< \brief destructor
};

#endif //_VIRTUAL_DATA_TABLE_ROW_HEADER_H_

