/**********************************************************************/
/** FILE    : VirtualITableTreeDataModel.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data model exposed as a hierarchy of tables          **/
/**********************************************************************/

#include <wx/VirtualDataView/Table/Models/VirtualITableTreeDataModel.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttrProvider.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualITableTreeDataModel::wxVirtualITableTreeDataModel(void)
    : wxVirtualIDataModel(),
      m_pCellAttrProvider(new wxVirtualDataViewCellAttrProvider)
{

}

/** Destructor
  */
wxVirtualITableTreeDataModel::~wxVirtualITableTreeDataModel(void)
{
    if (m_pCellAttrProvider) delete(m_pCellAttrProvider);
    m_pCellAttrProvider = WX_VDV_NULL_PTR;
}

//--------------- INTERFACE FROM WXVIRTUALIDATAMODEL ----------------//
/** Get the amount of children
  * \param rID [input]: the item ID to query
  * \return the amount of children of rID
  */
size_t wxVirtualITableTreeDataModel::GetChildCount(const wxVirtualItemID &rID)
{
    return(GetColumnsCount(rID) * GetRowsCount(rID));
}

/** Get child item ID
  * \param rIDParent    [input]: the parent item ID to query
  * \param uiChildIndex [input]: the index of the child to get
  * \return the child index
  */
wxVirtualItemID wxVirtualITableTreeDataModel::GetChild(const wxVirtualItemID &rIDParent,
                                                       size_t uiChildIndex)
{
    size_t uiNbCols = GetColumnsCount(rIDParent);
    if (uiNbCols == 0) return(s_RootID);

    size_t uiRow = uiChildIndex / uiNbCols;
    size_t uiCol = uiChildIndex % uiNbCols;
    return(GetChild(uiRow, uiCol, rIDParent));
}

//--------------------- INTERFACE WITH COMMON IMPLEMENTATION --------//
/** Get the sibling item
  * The common implementation calls GetParent(rID), followed by GetChild(uiRow, uiCol, idParent)
  * \param rID      [input] : the item ID to query
  * \param uiRow    [input] : the row of the item to get
  * \param uiCol    [input] : the column of the item to get
  * \return the ID of the brother item
  */
wxVirtualItemID wxVirtualITableTreeDataModel::GetSibling(const wxVirtualItemID &rID,
                                                         size_t uiRow, size_t uiCol)
{
    wxVirtualItemID oParent = GetParent(rID);
    return(GetChild(uiRow, uiCol, oParent));
}

/** Get the item flags
  * \param rID [input]: the item ID to query
  * \return what the item can support
  * The default implementation returns WX_ITEM_FLAGS_ALL for valid ID,
  * and WX_ITEM_FLAGS_NONE for invalid IDs
  */
int wxVirtualITableTreeDataModel::GetFlags(const wxVirtualItemID &rID)
{
    if (rID.IsOK()) return(WX_ITEM_FLAGS_ALL);
    return(WX_ITEM_FLAGS_NONE);
}

//--------------------------- COMMON METHODS ------------------------//
/** Call SetValue, followed by ValueChanged
  * \param rVariant [input] : the new value of the item
  * \param rID      [input] : the ID of the item
  * \return true if the value actually changed, false otherwise
  */
bool wxVirtualITableTreeDataModel::ChangeValue(const wxVariant &rVariant,
                                               const wxVirtualItemID &rID)
{
    if (!SetValue(rVariant, rID)) return(false);
    return(ValueChanged(rID));
}

/** Notification method indicating that a value has changed
  * \param rID      [input] : the ID of the item
  * \return true if the value actually changed, false otherwise
  */

bool wxVirtualITableTreeDataModel::ValueChanged(const wxVirtualItemID &rID)
{
    bool bRet = true;

//    wxDataViewModelNotifiers::iterator iter;
//    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
//    {
//        wxDataViewModelNotifier* notifier = *iter;
//        if (!notifier->ValueChanged( item, col ))
//            ret = false;
//    }

    return(bRet);
}

//---------------- CELL ATTRIBUTES PROVIDER -------------------------//
/** Get the attributes provider
  * \return the attributes provider
  */
wxVirtualDataViewCellAttrProvider* wxVirtualITableTreeDataModel::GetAttributesProvider(void) const
{
    return(m_pCellAttrProvider);
}

/** Set the attributes provider
  * \param pProv [input]: the new provider. It must have been allocated with "new"
  *                       Ownership is taken
  */
void wxVirtualITableTreeDataModel::SetAttributesProvider(wxVirtualDataViewCellAttrProvider* pProv)
{
    if (m_pCellAttrProvider == pProv) return;
    if (m_pCellAttrProvider) delete(m_pCellAttrProvider);
    m_pCellAttrProvider = pProv;
}

//-------------------- CALLBACKS ------------------------------------//
/** One or more columns were added
  * The default implementation does nothing and returns true
  * \param uiColBefore [input] : the column index located immediately after the new column(s)
  * \param uiNbColumns [input] : amount of new columns
  * \param rID         [input] : the item ID which is modified
  * \return true on success, false on failure
  */
bool wxVirtualITableTreeDataModel::OnColAdded(size_t uiColBefore, size_t uiNbColumns,
                                              const wxVirtualItemID &rID)
{
    return(true);
}

/** One or more columns were deleted
  * The default implementation does nothing and returns true
  * \param uiColDeleted [input] : the index of the 1st column which is deleted
  * \param uiNbColumns  [input]  : amount of deleted columns
  * \param rID          [input] : the item ID which is modified
  * \return true on success, false on failure
  */
bool wxVirtualITableTreeDataModel::OnColDeleted(size_t uiColDeleted, size_t uiNbColumns,
                                                const wxVirtualItemID &rID)
{
    return(true);
}

/** One or more rows were added
  * The default implementation does nothing and returns true
  * \param uiRowBefore  [input] : the row index located immediately after the new row(s)
  * \param uiNbRows     [input] : amount of new rows
  * \param rID          [input] : the item ID which is modified
  * \return true on success, false on failure
  */
bool wxVirtualITableTreeDataModel::OnRowAdded(size_t uiRowBefore, size_t uiNbRows,
                                              const wxVirtualItemID &rID)
{
    return(true);
}

/** One or more rows were deleted
  * The default implementation does nothing and returns true
  * \param uiRowDeleted [input] : the index of the 1st row which is deleted
  * \param uiNbRows     [input]  : amount of deleted rows
  * \param rID          [input] : the item ID which is modified
  * \return true on success, false on failure
  */
bool wxVirtualITableTreeDataModel::OnRowDeleted(size_t uiRowDeleted, size_t uiNbRows,
                                                const wxVirtualItemID &rID)
{
    return(true);
}

//-------------------- INDEX CREATION METHODS -----------------------//
/** Create an item ID from a void pointer
  * \param uiRow [input]: the row index
  * \param uiCol [input]: the column index
  * \param pID   [input]: the ID of the model item ID
  * \return the model item ID
  */
wxVirtualItemID wxVirtualITableTreeDataModel::CreateItemID(size_t uiRow, size_t uiCol, void *pID)
{
    wxVirtualItemID id(pID, uiRow, uiCol, this);
    return(id);
}

/** Create an index from a 32 bits unsigned integer
  * \param uiRow [input]: the row index
  * \param uiCol [input]: the column index
  * \param id    [input]: the ID of the model item ID
  * \return the model item ID
  */
wxVirtualItemID wxVirtualITableTreeDataModel::CreateItemID(size_t uiRow, size_t uiCol, wxUint32 id)
{
    wxVirtualItemID idItem(id, uiRow, uiCol, this);
    return(idItem);
}

/** Create an index from a 32 bits signed integer
  * \param uiRow [input]: the row index
  * \param uiCol [input]: the column index
  * \param id    [input]: the ID of the model item ID
  * \return the model item ID
  */
wxVirtualItemID wxVirtualITableTreeDataModel::CreateItemID(size_t uiRow, size_t uiCol, wxInt32 id)
{
    wxVirtualItemID idItem(id, uiRow, uiCol, this);
    return(idItem);
}
#if WX_VDD_ALLOW_64BITS_ITEMID != 0

/** Create an index from a 64 bits unsigned integer
  * \param uiRow [input]: the row index
  * \param uiCol [input]: the column index
  * \param id    [input]: the ID of the model item ID
  * \return the model item ID
  */
wxVirtualItemID wxVirtualITableTreeDataModel::CreateItemID(size_t uiRow, size_t uiCol, wxUint64 id)
{
    wxVirtualItemID idItem(id, uiRow, uiCol, this);
    return(idItem);
}

/** Create an index from a 64 bits signed integer
  * \param uiRow [input]: the row index
  * \param uiCol [input]: the column index
  * \param id    [input]: the ID of the model item ID
  * \return the model item ID
  */
wxVirtualItemID wxVirtualITableTreeDataModel::CreateItemID(size_t uiRow, size_t uiCol, wxInt64 id)
{
    wxVirtualItemID idItem(id, uiRow, uiCol, this);
    return(idItem);
}

#endif // WX_VDD_ALLOW_64BITS_ITEMID
