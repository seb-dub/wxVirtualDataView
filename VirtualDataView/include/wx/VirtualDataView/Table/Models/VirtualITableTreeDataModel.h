/**********************************************************************/
/** FILE    : VirtualITableTreeDataModel.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data model exposed as a hierarchy of tables          **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_TREE_TABLE_DATA_MODEL_H_
#define WX_VIRTUAL_I_TREE_TABLE_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/variant.h>

class wxVirtualDataViewItemAttr;
class wxVirtualDataViewCellAttrProvider;

/** \class wxVirtualITableTreeDataModel : a modified interface for a data model.
  * The model is exposed as a hierarchy of tables.
  *     - each item is identified by a wxVirtualItemID
  *     - each item has an amount of rows and columns: it defines a table size
  *     - each item has a list of children: each cell in the table is a child
  *     - top level items are represented by invalid wxVirtualItemID, constructed using default constructor
  *
  * So a standard models can be defined :
  *     - list model : top level node has N rows x 1 column.
  *                    Each sub-item has no rows and no columns
  *     - table model: top level node has N rows x P columns
  *                    Each sub-item has no rows and no columns
  *     - tree model : top level node has N rows x 1 column
  *                    Each sub-item has P rows x 1 column, and same for each hierarchy level
  *     - tree list model: top level node has N rows x P columns
  *                        Each sub-item has Q rows x P columns, and same for each hierarchy level
  *
  * Subclassing:
  *     - models implementation must implement the pure-virtual methods
  *     - either derive directly from this class, or use pre-defined models for
  *       list, table, tree or tree list
  *     - it is recommended to use one of the CreateItemID method internally for creating the
  *       wxVirtualItemID
  */
class WXDLLIMPEXP_VDV wxVirtualITableTreeDataModel : public wxVirtualIDataModel
{
    public:
        //constructors & destructor
        wxVirtualITableTreeDataModel(void);                                                 ///< \brief default constructor
        virtual ~wxVirtualITableTreeDataModel(void);                                        ///< \brief destructor

        //interface from wxVirtualIDataModel
        virtual size_t          GetChildCount(const wxVirtualItemID &rID);                  ///< \brief get the amount of children
        virtual wxVirtualItemID GetChild(const wxVirtualItemID &rIDParent,
                                         size_t uiChildIndex);                              ///< \brief get child item ID

        //new interface
        virtual size_t GetColumnsCount(const wxVirtualItemID &rID = s_RootID) const  = 0;   ///< \brief get the amount of columns
        virtual size_t GetRowsCount(const wxVirtualItemID &rID = s_RootID) const     = 0;   ///< \brief get the amount of rows
        virtual bool   GetValue(wxVariant &rVariant,
                                const wxVirtualItemID &rID,
                                int iDataType = WX_ITEM_MAIN_DATA)                   = 0;   ///< \brief get the value of an item
        virtual bool   SetValue(const wxVariant &rVariant,
                                const wxVirtualItemID &rID,
                                int iDataType = WX_ITEM_MAIN_DATA)                   = 0;   ///< \brief set the value of an item
        virtual wxVirtualItemID GetChild(size_t uiRow, size_t uiCol,
                                  const wxVirtualItemID &rIDParent = s_RootID)       = 0;   ///< \brief get the child item

        //interface with common implementation
        virtual wxVirtualItemID GetSibling(const wxVirtualItemID &rID,
                                           size_t uiRow, size_t uiCol);                     ///< \brief get the sibling item
        virtual int GetFlags(const wxVirtualItemID &rID);                                   ///< \brief get the item flags

        //common methods
        bool ChangeValue(const wxVariant &rVariant,
                         const wxVirtualItemID &rID);                                       ///< \brief call SetValue, followed by ValueChanged
        virtual bool ValueChanged(const wxVirtualItemID &rID);                              ///< \brief notification method indicating that a value has changed

        //attributes provider
        wxVirtualDataViewCellAttrProvider* GetAttributesProvider(void) const;               ///< \brief get the attributes provider
        void SetAttributesProvider(wxVirtualDataViewCellAttrProvider* pProv);               ///< \brief set the attributes provider)

        //callbacks: called by the control driving the model
        virtual bool OnColAdded(size_t uiColBefore, size_t uiNbColumns = 1,
                                const wxVirtualItemID &rID = s_RootID);                     ///< \brief one or more columns were added
        virtual bool OnColDeleted(size_t uiColDeleted, size_t uiNbColumns = 1,
                                  const wxVirtualItemID &rID = s_RootID);                   ///< \brief one or more columns were deleted
        virtual bool OnRowAdded(size_t uiRowBefore, size_t uiNbRows = 1,
                                const wxVirtualItemID &rID = s_RootID);                     ///< \brief one or more rows were added
        virtual bool OnRowDeleted(size_t uiRowDeleted, size_t uiNbRows = 1,
                                  const wxVirtualItemID &rID = s_RootID);                   ///< \brief one or more rows were deleted

    protected:
        //data
        wxVirtualDataViewCellAttrProvider*  m_pCellAttrProvider;                            ///< \brief cell attributes provider

        //methods
        wxVirtualItemID CreateItemID(size_t uiRow, size_t uiCol, void *pID);                ///< \brief create an item ID from a void pointer
        wxVirtualItemID CreateItemID(size_t uiRow, size_t uiCol, wxUint32 id);              ///< \brief create an item ID from a 32 bits unsigned integer
        wxVirtualItemID CreateItemID(size_t uiRow, size_t uiCol, wxInt32 id);               ///< \brief create an item ID from a 32 bits signed integer
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
        wxVirtualItemID CreateItemID(size_t uiRow, size_t uiCol, wxUint64 id);              ///< \brief create an item ID from a 64 bits unsigned integer
        wxVirtualItemID CreateItemID(size_t uiRow, size_t uiCol, wxInt64 id);               ///< \brief create an item ID from a 64 bits signed integer
#endif // WX_VDD_ALLOW_64BITS_ITEMID
};

#endif

