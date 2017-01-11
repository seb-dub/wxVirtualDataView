
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/bitmap.h>
#include <map>

class TestTreeDataModel : public wxVirtualIDataModel
{
    public:

        //constructors & destructor
        TestTreeDataModel(bool bEmpty = false);                                     ///< \brief default constructor
        virtual ~TestTreeDataModel(void);                                           ///< \brief destructor

        //interface
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rID = s_RootID);   ///< \brief get the parent item ID
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);          ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex);                      ///< \brief get child item ID
        virtual size_t          GetFieldCount(const wxVirtualItemID &rID);          ///< \brief get the amount of fields in the item

        virtual wxVariant GetItemData(const wxVirtualItemID &rID,
                                      size_t uiField = 0,
                                      EDataType eType = WX_ITEM_MAIN_DATA);         ///< \brief get the item data
        virtual bool      SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType = WX_ITEM_MAIN_DATA);         ///< \brief set the item data
        virtual wxVirtualDataViewItemAttr* GetItemAttribute(const wxVirtualItemID &rID,
                                                            size_t uiField,
                                              const wxVirtualDataViewItemState &rState);    ///< \brief get the item graphic attributes

    private:
        //structs
        struct TID
        {
            wxVirtualItemID     m_id;
            size_t              m_uiField;

            TID(wxVirtualItemID id, size_t uiField);
            bool operator<(const TID &rhs) const;
        };
        typedef std::map<TID, wxVariant> TMapOfValues;
        TMapOfValues m_MapOfValues;                                                 ///< \brief values edited by user

        wxVirtualDataViewItemAttr   m_attrOdd;
        wxVirtualDataViewItemAttr   m_attrEven;
        wxBitmap                    m_bmp1;
        wxBitmap                    m_bmp2;
        wxBitmap                    m_bmp3;
        wxDateTime                  m_dt;
        bool                        m_bEmpty;
        bool                        m_bSpeedOptimization;
};

