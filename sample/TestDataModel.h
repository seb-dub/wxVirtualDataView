
#include <wx/VirtualDataView/Table/Models/VirtualITableDataModel.h>

/** \class wxVirtualIDataModel : a class for providing cell data and graphic attributes
  * The coordinates are models coordinates : reordering of rows/columns/item or sorting are
  * not (and should not) be dealt by this class.
  * The default implementation use internally a hash table
  */
class TestDataModel : public wxVirtualITableDataModel
{
    public:

        //constructors & destructor
        TestDataModel(void);                                                    ///< \brief default constructor
        virtual ~TestDataModel(void);                                           ///< \brief destructor

        //interface
        virtual size_t GetColumnsCountTable(void) const;                        ///< \brief get the amount of columns in the table
        virtual size_t GetRowsCountTable(void) const;                           ///< \brief get the amount of rows in the table
        virtual bool   GetValue(wxVariant &rVariant,
                                size_t uiRow, size_t uiCol,
                                int iDataType = WX_ITEM_MAIN_DATA);             ///< \brief get the cell value
        virtual bool   SetValue(const wxVariant &rVariant,
                                size_t uiRow, size_t uiCol,
                                int iDataType = WX_ITEM_MAIN_DATA);             ///< \brief set the cell value
};

