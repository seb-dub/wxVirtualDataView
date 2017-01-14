/**********************************************************************/
/** FILE    : DataViewModelCompat.h                                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualIDataViewModel using a wxDataViewModel      **/
/**********************************************************************/


#ifndef _DATA_VIEW_MODEL_COMPAT_H_
#define _DATA_VIEW_MODEL_COMPAT_H_

#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0

class WXDLLIMPEXP_ADV   wxDataViewModel;
class WXDLLIMPEXP_ADV   wxDataViewItem;

//------------------------- DECLARATION -----------------------------//
/** \class wxDataViewModelCompat : a compatibility class for wxDataViewModel
  * It implements the wxVirtualIDataViewModel interface, and forwards all the calls
  * to the internal wxDataViewModel
  */
class WXDLLIMPEXP_VDV wxDataViewModelCompat : public wxVirtualIDataModel
{
    public:
        //constructors & destructor
        wxDataViewModelCompat(wxDataViewModel *pDataModel);                                 ///< \brief constructor
        ~wxDataViewModelCompat(void);                                                       ///< \brief destructor

        //interface: class type
        virtual int GetModelClass(void) const;                                              ///< \brief get the model class flags

        //interface : hierarchy
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID);           ///< \brief get the parent item ID
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex);                              ///< \brief get child item ID
        virtual size_t          GetAllChildren(wxVirtualItemIDs &vIDs,
                                               const wxVirtualItemID &rIDParent);           ///< \brief get all children at once
        virtual size_t          GetFieldCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of fields in the item

        //interface : item data
        virtual wxVariant GetItemData(const wxVirtualItemID &rID,
                                      size_t uiField = 0,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief get the item data
        virtual wxVirtualDataViewItemAttr* GetItemAttribute(const wxVirtualItemID &rID,
                                                            size_t uiField,
                                              const wxVirtualDataViewItemState &rState);    ///< \brief get the item graphic attributes
        virtual EFlags    GetItemFlags(const wxVirtualItemID &rID, size_t uiField);         ///< \brief get the item flags
        virtual bool      SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief set the item data

        //sorting
        virtual bool CanSort(const TSortFilters &vSortFilters) const;                       ///< \brief check if the field can be sorted by the data model
        virtual void Sort(const TSortFilters &vSortFilters);                                ///< \brief sort the values
        virtual void ResetSorting(void);                                                    ///< \brief remove all sort filters

        //sorting comparison functors
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVirtualItemID &rID2,
                                        size_t uiField);                                    ///< \brief compare 2 items
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVariant &rValue1,
                                        const wxVirtualItemID &rID2,
                                        const wxVariant &rValue2,
                                        size_t uiField);                                    ///< \brief compare 2 items values

    protected:
        //data
        wxDataViewModel*            m_pDataModel;                                           ///< \brief the data model
        wxVirtualDataViewItemAttr*  m_pAttribute;                                           ///< \brief graphic attributes

        //methods
        wxVirtualItemID ConvertItemID(const wxDataViewItem &rID);                           ///< \brief convert a wxDataViewItem to a wxVirtualItemID
        wxDataViewItem  ConvertVirtualItemID(const wxVirtualItemID &rID);                   ///< \brief convert a wxVirtualItemID to a wxDataViewItem

    private:
        //methods
        wxVirtualItemID DoConvertItemID(const wxDataViewItem &rID);                         ///< \brief convert a wxDataViewItem to a wxVirtualItemID
        wxDataViewItem  DoConvertVirtualItemID(const wxVirtualItemID &rID);                 ///< \brief convert a wxVirtualItemID to a wxDataViewItem

};

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
#endif

