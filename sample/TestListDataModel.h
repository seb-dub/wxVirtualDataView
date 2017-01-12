
#include <wx/VirtualDataView/Models/VirtualIListDataModel.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/bitmap.h>
#include <map>

class TestListDataModel : public wxVirtualIListDataModel
{
    public:

        //constructors & destructor
        TestListDataModel(void);                                                    ///< \brief default constructor
        virtual ~TestListDataModel(void);                                           ///< \brief destructor

        //interface
        //new interface
        virtual size_t GetItemCount(void);                                          ///< \brief get amount of items

        //new interface : item data
        virtual wxVariant GetListItemData(size_t uiItemID,
                                      size_t uiField = 0,
                                      EDataType eType = WX_ITEM_MAIN_DATA);                 ///< \brief get the item data
        virtual bool      SetListItemData(size_t uiItemID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType = WX_ITEM_MAIN_DATA);         ///< \brief set the item data
        virtual wxVirtualDataViewItemAttr* GetListItemAttribute(size_t uiItemID,
                                                            size_t uiField,
                                              const wxVirtualDataViewItemState &rState);    ///< \brief get the item graphic attributes

    private:
        //structs
        struct TID
        {
            size_t              m_uiID;
            size_t              m_uiField;

            TID(size_t id, size_t uiField);
            bool operator<(const TID &rhs) const;
        };
        typedef std::map<TID, wxVariant> TMapOfValues;
        TMapOfValues m_MapOfValues;                                                 ///< \brief values edited by user

        //data
        wxVirtualDataViewItemAttr   m_attrOdd;
        wxVirtualDataViewItemAttr   m_attrEven;
        wxBitmap                    m_bmp1;
        wxBitmap                    m_bmp2;
};

