/**********************************************************************/
/** FILE    : DataViewItemCompat.h                                   **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view item ID                                    **/
/**********************************************************************/


#ifndef _DATA_VIEW_ITEM_COMPAT_H_
#define _DATA_VIEW_ITEM_COMPAT_H_

#include <wx/VirtualDataView/VirtualItemID.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0

#include <wx/itemid.h>

//------------------------- DECLARATION -----------------------------//

/** \class wxDataViewItemCompat : a compatibility class for wxDataViewItem
  * It provides the same API as wxDataViewItem, and can be converted back & forth with
  * wxVirtualItemID
  */
class WXDLLIMPEXP_VDV wxDataViewItemCompat : public wxItemId<void*>
{
    public:
        //constructors & destructor
        wxDataViewItemCompat(void);                                                         ///< \brief default constructor
        wxEXPLICIT wxDataViewItemCompat(void *pData);                                       ///< \brief constructor from a void pointer
        wxDataViewItemCompat(const wxDataViewItemCompat &rhs);                              ///< \brief copy constructor
        wxDataViewItemCompat(const wxVirtualItemID &rID);                                   ///< \brief constructor from a wxVirtualItemID
        ~wxDataViewItemCompat(void);                                                        ///< \brief destructor

        //conversion
        wxVirtualItemID ToVirtualItemID(wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR); ///< \brief convert to a wxVirtualItemID
};

WX_DEFINE_ARRAY(wxDataViewItemCompat, wxDataViewItemCompatArray);

//--------------------- INLINE IMPLEMENTATION -----------------------//
//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Default constructor
  */
WX_VDV_INLINE wxDataViewItemCompat::wxDataViewItemCompat(void)
    : wxItemId<void*>()
{
}

/** Constructor from a void pointer
  * \param pData [input]: the item data
  */
WX_VDV_INLINE wxDataViewItemCompat::wxDataViewItemCompat(void *pData)
    : wxItemId<void*>(pData)
{
}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
WX_VDV_INLINE wxDataViewItemCompat::wxDataViewItemCompat(const wxDataViewItemCompat &rhs)
    : wxItemId<void*>(rhs)
{
}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
WX_VDV_INLINE wxDataViewItemCompat::wxDataViewItemCompat(const wxVirtualItemID &rhs)
    : wxItemId<void*>(rhs.GetID())
{

}

/** Destructor
  */
WX_VDV_INLINE wxDataViewItemCompat::~wxDataViewItemCompat(void)
{
}

//------------------------ CONVERSION -------------------------------//
/** Convert to a wxVirtualItemID
  * \return the equivalent wxVirtualItemID
  */
wxVirtualItemID wxDataViewItemCompat::ToVirtualItemID(wxVirtualIDataModel *pDataModel)
{
    return(wxVirtualItemID(GetID(), 0, 0, pDataModel));
}

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
#endif

