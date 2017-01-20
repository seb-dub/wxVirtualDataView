/**********************************************************************/
/** FILE    : VirtualDataViewColumnsList.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view columns list                               **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_COLUMNS_LIST_H_
#define _VIRTUAL_DATA_VIEW_COLUMNS_LIST_H_

#include <wx/VirtualDataView/Columns/VirtualDataViewColumn.h>
#include <wx/vector.h>

/** \class wxVirtualDataViewColumnsList : a list of columns
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewColumnsList
{
    public:
        //constructors & destructor
        wxVirtualDataViewColumnsList(void);                             ///< \brief default constructor
        ~wxVirtualDataViewColumnsList(void);                            ///< \brief destructor

        //delete
        void Clear(void);                                               ///< \brief remove all columns
        void DeleteColumn(size_t uiColumn);                             ///< \brief delete one column
        void DeleteColumns(size_t uiFirst, size_t uiCount);             ///< \brief delete several columns

        //add
        void AppendColumn(const wxVirtualDataViewColumn &rCol);         ///< \brief append a column
        void InsertColumn(size_t uiBefore,
                          const wxVirtualDataViewColumn &rCol);         ///< \brief insert a column

        //get
        size_t GetColumnsCount(void) const;                             ///< \brief get the amount of columns
        wxVirtualDataViewColumn* GetColumn(size_t uiColumn) const;      ///< \brief get the column

        //reorder
        void  SwapColumns(size_t uiCol1, size_t uiCol2);                ///< \brief swap 2 columns
        void  ReorderColumns(size_t *pNewOrder);                        ///< \brief reorder the columns

        //width
        int   GetColumnStart(size_t uiCol) const;                       ///< \brief get the start offset of a column
        int   GetColumnWidth(size_t uiCol) const;                       ///< \brief get a column width
        int   GetTotalWidth(void) const;                                ///< \brief get the total width of the columns
        void  MoveColumn(size_t uiCol, size_t uiMoveAt);                ///< \brief move a column

        //search
        size_t GetColumnIndex(size_t uiModelField);                     ///< \brief search the column associated with a model field index

    protected:
        //typedef
        typedef wxVector<wxVirtualDataViewColumn*>   TArrayOfColumns;   ///< \brief array of columns

        //data
        TArrayOfColumns     m_vColumns;                                 ///< \brief array of columnson
};

#endif

