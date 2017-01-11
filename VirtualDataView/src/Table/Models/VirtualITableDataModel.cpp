/**********************************************************************/
/** FILE    : VirtualITableDataModel.cpp                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : specialization of VirtualIDataModel for tables/lists   **/
/**********************************************************************/

#include <wx/VirtualDataView/Table/Models/VirtualITableDataModel.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttrProvider.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Default constructor
  */
wxVirtualITableDataModel::wxVirtualITableDataModel(void)
    : wxVirtualITableTreeDataModel()
{

}

/** Destructor
  */
wxVirtualITableDataModel::~wxVirtualITableDataModel(void)
{
    if (m_pCellAttrProvider) delete(m_pCellAttrProvider);
    m_pCellAttrProvider = WX_VDV_NULL_PTR;
}

//-------- wxVirtualIDataModel INTERFACE IMPLEMENTATION -------------//
/** Get the amount of columns
  * \param rID [input] : the parent item
  * \return the amount of columns of the table if rID is a root item
  *         0 otherwise
  */
size_t wxVirtualITableDataModel::GetColumnsCount(const wxVirtualItemID &rID) const
{
    if (!rID.IsOK()) return(GetColumnsCountTable());
    return(0);
}

/** Get the amount of rows
  * \param rID [input] : the parent item
  * \return the amount of rows of the table if rID is a root item
  *         0 otherwise
  */
size_t wxVirtualITableDataModel::GetRowsCount(const wxVirtualItemID &rID) const
{
    if (!rID.IsOK()) return(GetRowsCountTable());
    return(0);
}

/** Get the value of an item
  * \param rVariant  [output]: the data. Filled only if the method returned true
  * \param rID       [input] : the ID of the data to find
  * \param iDataType [input] : the data type to find
  * \return true if rVariant was filled with data, false otherwise
  */
bool wxVirtualITableDataModel::GetValue(wxVariant &rVariant,
                                        const wxVirtualItemID &rID,
                                        int iDataType)
{
    if (IsRootItem(rID)) return(false);
    return(GetValue(rVariant, rID.GetRow(), rID.GetColumn(), iDataType));
}

/** Set the value of an item
  * \param rVariant  [input]: the data.
  * \param rID       [input] : the ID of the data to find
  * \param iDataType [input] : the data type to find
  * \return true if the model was updated, false otherwise
  */
bool wxVirtualITableDataModel::SetValue(const wxVariant &rVariant,
                                        const wxVirtualItemID &rID,
                                        int iDataType)
{
    if (IsRootItem(rID)) return(false);
    return(SetValue(rVariant, rID.GetRow(), rID.GetColumn(), iDataType));
}

/** Get the child item
  * \param uiRow     [input]: the row of the child item
  * \param uiCol     [input]: the column of the child item
  * \param rIDParent [input]: the parent item.
  * \return the item ID of the child item.
  *         It returns a valid item only if rIDParent is the root item
  */
wxVirtualItemID wxVirtualITableDataModel::GetChild(size_t uiRow,
                                                   size_t uiCol,
                                                   const wxVirtualItemID &rIDParent)
{
    //if (rIDParent.IsInvalid()) return(wxVirtualItemID());
    return(CreateItemID(uiRow, uiCol, (wxUint32) 1));
}

/** Get the parent item
  * \param rIDChild [input]: the child item
  * \return the parent of rIDChild.
  *         Parent of root returns itself
  *         So it always returns the root item for table models
  */
wxVirtualItemID wxVirtualITableDataModel::GetParent(const wxVirtualItemID &rIDChild)
{
    return(wxVirtualItemID());
}
