
#include <TestDataModel.h>

//----------------- CONSTRUCTORS & DESTRUCTOR -----------------------//
/** Default constructor
  */
TestDataModel::TestDataModel(void)
    : wxVirtualITableDataModel()
{

}

/** Destructor
  */
TestDataModel::~TestDataModel(void)
{

}

//---------------- INTERFACE IMPLEMENTATION -------------------------//
/** Get the amount of columns in the table
  * \return the amount of columns in the table
  */
size_t TestDataModel::GetColumnsCountTable(void) const
{
    return(2000000000);
}

/** Get the amount of rows in the table
  * \return the amount of rows in the table
  */
size_t TestDataModel::GetRowsCountTable(void) const
{
    return(2000000000);
}

/** Get the cell value
  * \param rVariant [output]: the value of the cell (modified only if true is returned)
  * \param uiRow    [input] : the row index
  * \param uiCol    [input] : the column index
  * \param iDataType [input] : the data type to get
  * \return true if the value has been modified, false otherwise
  */
bool TestDataModel::GetValue(wxVariant &rVariant, size_t uiRow, size_t uiCol, int iDataType)
{
    if (iDataType != iDataType) return(false);
    wxString sValue = wxString::Format("%d_%d", uiRow, uiCol);
    rVariant = sValue;
    return(true);
}

/** Set the cell value
  * \param rVariant  [input] : the new value of the item
  * \param uiRow    [input] : the row index
  * \param uiCol    [input] : the column index
  * \param iDataType [input] : the data type to set
  * \return true if the value has been modified, false otherwise
  */
bool TestDataModel::SetValue(const wxVariant &rVariant, size_t uiRow, size_t uiCol, int iDataType)
{
    return(false);
}

