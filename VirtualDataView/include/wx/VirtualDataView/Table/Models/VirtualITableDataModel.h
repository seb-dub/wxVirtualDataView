/**********************************************************************/
/** FILE    : VirtualITableDataModel.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : specialization of VirtualIDataModel for tables/lists   **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_TABLE_DATA_MODEL_H_
#define WX_VIRTUAL_I_TABLE_DATA_MODEL_H_

#include <wx/VirtualDataView/Table/Models/VirtualITableTreeDataModel.h>
#include <wx/variant.h>

class wxVirtualDataViewItemAttr;
class wxVirtualDataViewCellAttrProvider;

/** \class wxVirtualITableDataModel : specialization of wxVirtualIDataModel
  * for table or list views
  * There is no hierarchy defined
  */
class WXDLLIMPEXP_VDV wxVirtualITableDataModel : public wxVirtualITableTreeDataModel
{
    public:
        //constructors & destructor
        wxVirtualITableDataModel(void);                                                     ///< \brief default constructor
        virtual ~wxVirtualITableDataModel(void);                                            ///< \brief destructor

        //wxVirtualIDataModel interface implementation
        virtual size_t GetColumnsCount(const wxVirtualItemID &rID = s_RootID) const;        ///< \brief get the amount of columns
        virtual size_t GetRowsCount(const wxVirtualItemID &rID = s_RootID) const;           ///< \brief get the amount of rows
        virtual bool   GetValue(wxVariant &rVariant,
                                const wxVirtualItemID &rID,
                                int iDataType = WX_ITEM_MAIN_DATA);                         ///< \brief get the value of an item
        virtual bool   SetValue(const wxVariant &rVariant,
                                const wxVirtualItemID &rID,
                                int iDataType = WX_ITEM_MAIN_DATA);                         ///< \brief set the value of an item
        virtual wxVirtualItemID GetChild(size_t uiRow, size_t uiCol,
                                  const wxVirtualItemID &rIDParent = s_RootID);             ///< \brief get the child item
        virtual wxVirtualItemID GetParent(const wxVirtualItemID &rIDChild = s_RootID);      ///< \brief get the parent item

        //new interface
        virtual size_t GetColumnsCountTable(void) const                         = 0;        ///< \brief get the amount of columns in the table
        virtual size_t GetRowsCountTable(void) const                            = 0;        ///< \brief get the amount of rows in the table
        virtual bool   GetValue(wxVariant &rVariant,
                                size_t uiRow, size_t uiCol,
                                int iDataType = WX_ITEM_MAIN_DATA)              = 0;        ///< \brief get the cell value
        virtual bool   SetValue(const wxVariant &rVariant,
                                size_t uiRow, size_t uiCol,
                                int iDataType = WX_ITEM_MAIN_DATA)              = 0;        ///< \brief set the cell value
};

#endif

