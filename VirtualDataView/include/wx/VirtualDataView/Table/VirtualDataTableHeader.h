/**********************************************************************/
/** FILE    : VirtualDataTableHeader.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header for row/columns for wxVirtualDataTableCtrl      **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_TABLE_HEADER_H_
#define _VIRTUAL_DATA_TABLE_HEADER_H_

#include <wx/VirtualDataView/Header/VirtualHeaderCtrl.h>

class WXDLLIMPEXP_VDV wxVirtualDataTableCtrl;
class WXDLLIMPEXP_VDV wxVirtualHeaderIRenderer;
extern const char wxVDVTableHeaderCtrlNameStr[];

/** \class wxVirtualDataViewHeader : header control for columns/rows for wxVirtualDataTableCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataTableHeader : public wxVirtualHeaderCtrl
{
    public:
        //constructors & destructor
        wxVirtualDataTableHeader(void);                                                 ///< \brief default constructor
        wxVirtualDataTableHeader(wxWindow *pParent,
                                wxWindowID id         = wxID_ANY,
                                const wxPoint &ptPos  = wxDefaultPosition,
                                const wxSize &size    = wxDefaultSize,
                                long lStyle           = WX_VDV_DEFAULT_STYLE,
                                const wxString &sName = wxVDVTableHeaderCtrlNameStr);   ///< \brief constructor
        virtual ~wxVirtualDataTableHeader(void);                                        ///< \brief destructor

        //owner
        wxVirtualDataTableCtrl*  GetOwner(void) const;                                  ///< \brief get the owner window
        void                    SetOwner(wxVirtualDataTableCtrl* pOwner);               ///< \brief set the owner window

    protected:
        //data
        wxVirtualDataTableCtrl*     m_pOwner;                                           ///< \brief owner window

        //interface implementation
        virtual size_t  GetItemCount(void) const;                                       ///< \brief get the amount of items in the header
        virtual wxSize  GetItemSize(size_t uiItemID) const;                             ///< \brief get the size of one item
        virtual bool    GetItemData(size_t uiItemID,
                                    TItemData &rData) const;                            ///< \brief get the item data (label, sorting arrow, styles)
        virtual size_t  GetFirstVisibleItem(void) const;                                ///< \brief get the index of the 1st visible item
        virtual size_t  GetNextVisibleItem(size_t uiItemID) const;                      ///< \brief get the index of the next visible item
        virtual size_t  GetLastVisibleItem(void) const;                                 ///< \brief get the last visible item
        virtual wxCoord GetFirstItemOrigin(void) const;                                 ///< \brief get the origin of the 1st visible item (unscrolled coordinate)
};

#endif //_VIRTUAL_DATA_TABLE_HEADER_H_

