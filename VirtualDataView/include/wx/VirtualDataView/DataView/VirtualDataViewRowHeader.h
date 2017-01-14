/**********************************************************************/
/** FILE    : VirtualDataViewRowHeader.h                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : header for rows for wxVirtualDataViewCtrl              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_ROW_HEADER_H_
#define _VIRTUAL_DATA_VIEW_ROW_HEADER_H_

#include <wx/VirtualDataView/Header/VirtualHeaderCtrl.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewCtrl;
class WXDLLIMPEXP_VDV wxVirtualHeaderIRenderer;
extern const char wxVDVRowHeaderCtrlNameStr[];

/** \class wxVirtualDataViewRowHeader : header control for rows for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewRowHeader : public wxVirtualHeaderCtrl
{
    public:
        //constructors & destructor
        wxVirtualDataViewRowHeader(void);                                                 ///< \brief default constructor
        wxVirtualDataViewRowHeader(wxWindow *pParent,
                                   wxWindowID id         = wxID_ANY,
                                   const wxPoint &ptPos  = wxDefaultPosition,
                                   const wxSize &size    = wxDefaultSize,
                                   long lStyle           = WX_VDV_DEFAULT_STYLE,
                                   const wxString &sName = wxVDVRowHeaderCtrlNameStr);  ///< \brief constructor
        virtual ~wxVirtualDataViewRowHeader(void);                                      ///< \brief destructor

        //owner
        wxVirtualDataViewCtrl*  GetOwner(void) const;                                   ///< \brief get the owner window
        void                    SetOwner(wxVirtualDataViewCtrl* pOwner);                ///< \brief set the owner window

    protected:
        //data
        wxVirtualDataViewCtrl*     m_pOwner;                                            ///< \brief owner window

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

#endif //_VIRTUAL_DATA_VIEW_ROW_HEADER_H_

