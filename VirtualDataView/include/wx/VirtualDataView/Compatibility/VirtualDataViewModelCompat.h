/**********************************************************************/
/** FILE    : VirtualDataViewModelCompat.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualIDataViewModel using a wxDataViewModel      **/
/**********************************************************************/


#ifndef VIRTUAL_DATA_VIEW_MODEL_COMPAT_H_
#define VIRTUAL_DATA_VIEW_MODEL_COMPAT_H_

#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0

class WXDLLIMPEXP_ADV   wxDataViewModel;
class WXDLLIMPEXP_ADV   wxDataViewItem;

//------------------------- DECLARATION -----------------------------//
/** \class wxVirtualDataViewModelCompat : a compatibility class for wxDataViewModel
  * It implements the wxVirtualIDataViewModel interface, and forwards all the calls
  * to the internal wxDataViewModel
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewModelCompat : public wxVirtualIDataModel
{
    public:
        //constructors & destructor
        wxVirtualDataViewModelCompat(wxDataViewModel *pDataModel);                          ///< \brief constructor
        ~wxVirtualDataViewModelCompat(void);                                                ///< \brief destructor

        //get / set wxDataViewModel
        wxDataViewModel*        GetModel(void);                                             ///< \brief get the data model
        const wxDataViewModel*  GetModel(void) const;                                       ///< \brief get the data model - const version
        void                    SetModel(wxDataViewModel *pModel);                          ///< \brief set the data model

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

        //interface : sorting
        virtual bool CanSort(const TSortFilters &vSortFilters) const;                       ///< \brief check if the field can be sorted by the data model
        virtual void Sort(const TSortFilters &vSortFilters);                                ///< \brief sort the values
        virtual void ResetSorting(void);                                                    ///< \brief remove all sort filters

        //interface : sorting comparison functors
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVirtualItemID &rID2,
                                        size_t uiField);                                    ///< \brief compare 2 items
        virtual ECompareResult  Compare(const wxVirtualItemID &rID1,
                                        const wxVariant &rValue1,
                                        const wxVirtualItemID &rID2,
                                        const wxVariant &rValue2,
                                        size_t uiField);                                    ///< \brief compare 2 items values

        //interface : misc
        virtual bool            HasExpander(const wxVirtualItemID &rID);                    ///< \brief return true if an item expander should be drawn for this item

        //wxDataViewModel compatibility interface
        wxString GetColumnType(size_t uiField) const;                                       ///< \brief get column type
        bool     HasValue(const wxDataViewItem &rID, size_t uiField);                       ///< \brief check if an item has a value
        bool     IsListModel(void) const;                                                   ///< \brief check if the wxDataViewModel is a list model
        bool     IsVirtualListModel(void) const;                                            ///< \brief check if the wxDataViewModel is a virtual list model


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

