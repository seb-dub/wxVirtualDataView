/**********************************************************************/
/** FILE    : VirtualDataTableColHeader.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header control for columns for wxVirtualDataTableCtrl  **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_TABLE_COLUMN_HEADER_H_
#define _VIRTUAL_DATA_TABLE_COLUMN_HEADER_H_

#include <wx/VirtualDataView/Table/VirtualDataTableHeader.h>

/** \class wxVirtualDataTableColHeader : header control for columns for wxVirtualDataTableCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataTableColHeader : public wxVirtualDataTableHeader
{
    public:
        //constructors & destructor
        wxVirtualDataTableColHeader(void);                                              ///< \brief default constructor
        wxVirtualDataTableColHeader(wxWindow *pParent,
                                    wxWindowID id         = wxID_ANY,
                                    const wxPoint &ptPos  = wxDefaultPosition,
                                    const wxSize &size    = wxDefaultSize,
                                    long lStyle           = WX_VDV_DEFAULT_STYLE,
                                const wxString &sName = wxVDVTableHeaderCtrlNameStr);   ///< \brief constructor
        virtual ~wxVirtualDataTableColHeader(void);                                     ///< \brief destructor
};

#endif //_VIRTUAL_DATA_TABLE_COLUMN_HEADER_H_

