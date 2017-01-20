/**********************************************************************/
/** FILE    : VirtualDataViewColumnsList.cpp                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view columns list                               **/
/**********************************************************************/

#include <wx/VirtualDataView/Columns/VirtualDataViewColumnsList.h>

//----------------------- CONSTRUCTORS & DESTRUCTOR -----------------//
/** Default constructor
  */
wxVirtualDataViewColumnsList::wxVirtualDataViewColumnsList(void)
    : m_vColumns()
{
}

/** Destructor
  */
wxVirtualDataViewColumnsList::~wxVirtualDataViewColumnsList(void)
{
    Clear();
}

//--------------------- DELETE --------------------------------------//
/** Remove all columns
  */
void wxVirtualDataViewColumnsList::Clear(void)
{
    TArrayOfColumns::iterator it    = m_vColumns.begin();
    TArrayOfColumns::iterator itEnd = m_vColumns.end();
    while(it != itEnd)
    {
        wxVirtualDataViewColumn *pCol = *it;
        if (pCol) delete(pCol);
        ++it;
    }
    m_vColumns.clear();
}

/** Delete one column
  * \param uiColumn [input]: the index of the column to delete
  *                          all columns indices after this one are updated (uiColumn + N becomes uiColumn + N - 1)
  */
void wxVirtualDataViewColumnsList::DeleteColumn(size_t uiColumn)
{
    if (uiColumn >= m_vColumns.size()) return;

    TArrayOfColumns::iterator it = m_vColumns.begin();
    it += uiColumn;
    wxVirtualDataViewColumn *pCol = *it;
    if (pCol) delete(pCol);
    m_vColumns.erase(it);
}

/** Delete several columns
  * \param uiFirst [input]: the index of the 1st column to delete
  * \param uiCount [input]: the amount of columns to delete
  */
void wxVirtualDataViewColumnsList::DeleteColumns(size_t uiFirst, size_t uiCount)
{
    TArrayOfColumns::iterator itFirst = m_vColumns.begin();
    itFirst += uiFirst;

    TArrayOfColumns::iterator itLast = itFirst + uiCount;

    TArrayOfColumns::iterator it    = itFirst;
    TArrayOfColumns::iterator itEnd = m_vColumns.end();
    while ((it < itLast) && (it != itEnd))
    {
        wxVirtualDataViewColumn *pCol = *it;
        if (pCol) delete(pCol);
        ++it;
    }

    m_vColumns.erase(itFirst, itLast);
}

//---------------------- ADD ----------------------------------------//
/** Append a column
  * \param rCol [input]: the column to add. A copy is made
  */
void wxVirtualDataViewColumnsList::AppendColumn(const wxVirtualDataViewColumn &rCol)
{
    m_vColumns.push_back(new wxVirtualDataViewColumn(rCol));
}

/** Insert a column
  * \param uiBefore [input]: the index of the column which is located AFTER the new column
  * \param rCol [input]: the column to add. A copy is made
  */
void wxVirtualDataViewColumnsList::InsertColumn(size_t uiBefore,
                                                const wxVirtualDataViewColumn &rCol)
{
    TArrayOfColumns::iterator it = m_vColumns.begin() + uiBefore;
    m_vColumns.insert(it, new wxVirtualDataViewColumn(rCol));
}

//---------------------- GET ----------------------------------------//
/** Get the amount of columns
  * \return the amount of columns
  */
size_t wxVirtualDataViewColumnsList::GetColumnsCount(void) const
{
    return(m_vColumns.size());
}

/** Get the column
  * \param uiColumn [input]: the index of the column to get
  * \return a pointer to the column
  *         WX_VDV_NULL_PTR is the uiColumn is invalid
  * Do not delete the wxVirtualDataViewColumn object : it is still owned by this class
  */
wxVirtualDataViewColumn* wxVirtualDataViewColumnsList::GetColumn(size_t uiColumn) const
{
    if (uiColumn >= m_vColumns.size()) return(WX_VDV_NULL_PTR);
    return(const_cast<wxVirtualDataViewColumn*>(m_vColumns[uiColumn]));
}

//----------------------- REORDER -----------------------------------//
/** Swap 2 columns
  * \param uiCol1 [input]: the index of the 1st column
  * \param uiCol2 [input]: the index of the 2nd column
  * Nothing is done is one of the index is invalid
  */
void wxVirtualDataViewColumnsList::SwapColumns(size_t uiCol1, size_t uiCol2)
{
    //special case
    if (uiCol1 == uiCol2) return;
    if (uiCol1 >= m_vColumns.size()) return;
    if (uiCol2 >= m_vColumns.size()) return;

    //swap
    wxVirtualDataViewColumn *cSwap = m_vColumns[uiCol1];
    m_vColumns[uiCol1] = m_vColumns[uiCol2];
    m_vColumns[uiCol2] = cSwap;
}

/** Reorder the columns
  * \param pNewOrder [input]: an array of indices. It must be large enough
  *                           (GetColumnsCount() elements)
  *                           pNewOrder[0] = index of the new 1st column
  *                           pNewOrder[1] = index of the new 2nd column
  *                           ...
  *
  * Example: we have a list of 3 columns [0, 1, 2]
  *          To reverse the columns order, we need to pass pNewOrder = [2, 1, 0]
  *          To swap the last 2 columns, we need to pass pNewOrder = [0, 2, 1]
  */
void wxVirtualDataViewColumnsList::ReorderColumns(size_t *pNewOrder)
{
    //check
    if (!pNewOrder) return;

    //create new vector
    size_t uiMax = m_vColumns.size();
    TArrayOfColumns vTemp;
    vTemp.clear();
    vTemp.reserve(uiMax);

    //reorder
    size_t i, uiIndex;
    for(i=0;i<uiMax;i++)
    {
        uiIndex = pNewOrder[i];
        if (uiIndex >= uiMax) return; //invalid index
        vTemp.push_back(m_vColumns[uiIndex]);
    }

    //end
    m_vColumns.swap(vTemp);
}

/** Move a column
  * If one of the index is out of bound, nothing is done
  * \param uiCol    [input]: the index of the column to move
  * \param uiMoveAt [input]: the new index of the column to move.
  *                          Columns located after uiMoveAt are shifted to the right
  */
void wxVirtualDataViewColumnsList::MoveColumn(size_t uiCol, size_t uiMoveAt)
{
    //check
    size_t uiMax = m_vColumns.size();
    if (uiCol >= uiMax) return;
    if (uiMoveAt >= uiMax) return;

    //create new vector
    TArrayOfColumns vTemp;
    vTemp.clear();
    vTemp.reserve(uiMax);

    //reorder
    size_t i;
    for(i=0;i<uiMoveAt;i++)
    {
        if (i == uiCol) continue;
        vTemp.push_back(m_vColumns[i]);
    }
    vTemp.push_back(m_vColumns[uiCol]);
    for(i=uiMoveAt;i<uiMax;i++)
    {
        if (i == uiCol) continue;
        vTemp.push_back(m_vColumns[i]);
    }

    //end
    m_vColumns.swap(vTemp);
}

/** Search the column associated with a model field index
  * \param uiModelField [input]: the model field index (used in wxVirtualIDataModel interface)
  * \return the index of the 1st column associated with this field
  *         size_t(-1) is returned if not found
  */
size_t wxVirtualDataViewColumnsList::GetColumnIndex(size_t uiModelField)
{
    size_t uiMax = m_vColumns.size();
    size_t i;
    for(i=0;i<uiMax;i++)
    {
        wxVirtualDataViewColumn *pCol = m_vColumns[i];
        if (!pCol) continue;
        if (pCol->GetModelColumn() == uiModelField) return(i);
    }

    return(size_t(-1));
}

//------------------------- WIDTH & OFFSETS -------------------------//
/** Get the start offset of a column
  * \param uiCol [input]: the index of the column to get
  * \return the offset of the column
  *         -1 if not found
  */
int wxVirtualDataViewColumnsList::GetColumnStart(size_t uiCol) const
{
    if (uiCol >= m_vColumns.size()) return(-1);
    size_t i;
    int iOffset = 0;
    for(i=0;i<uiCol;i++)
    {
        const wxVirtualDataViewColumn *pCol = m_vColumns[i];
        if (!pCol) continue;
        iOffset += pCol->GetWidth();
    }
    return(iOffset);
}

/** Get a column width
  * \param uiCol [input]: the index of the column to get
  * \return the width of the column, if found
  *         -1, if not found
  */
int wxVirtualDataViewColumnsList::GetColumnWidth(size_t uiCol) const
{
    if (uiCol >= m_vColumns.size()) return(-1);
    const wxVirtualDataViewColumn *pCol = m_vColumns[uiCol];
    if (!pCol) return(0);
    return(pCol->GetWidth());
}

/** Get the total width of the columns
  * \return the cumulated width of all the columns
  */
int wxVirtualDataViewColumnsList::GetTotalWidth(void) const
{
    size_t uiCol, uiNbCols;
    uiNbCols = m_vColumns.size();
    int iWidth = 0;
    for(uiCol = 0; uiCol < uiNbCols; uiCol++)
    {
        const wxVirtualDataViewColumn *pCol = m_vColumns[uiCol];
        if (!pCol) continue;
        iWidth += pCol->GetWidth();
    }
    return(iWidth);
}
