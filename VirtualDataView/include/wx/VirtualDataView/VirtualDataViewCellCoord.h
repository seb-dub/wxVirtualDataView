/**********************************************************************/
/** FILE    : wxVirtualDataViewCellCoord.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view item ID                                    **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_CELL_COORD_H_
#define _VIRTUAL_DATA_VIEW_CELL_COORD_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

//------------------------- DECLARATION -----------------------------//

/** \class wxVirtualDataViewCellCoord : define (row/col) coordinates
  * A negative row or column has a special meaning: it means "undefined"
  * and generally means all rows or all columns
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewCellCoord
{
    public:

        //constructors & destructor
        wxVirtualDataViewCellCoord(long lRow = 0, long lCol = 0);           ///< \brief default constructor
        wxVirtualDataViewCellCoord(const wxVirtualDataViewCellCoord &rhs);  ///< \brief copy constructor
        wxVirtualDataViewCellCoord&
            operator=(const wxVirtualDataViewCellCoord &rhs);               ///< \brief assignation operator
        ~wxVirtualDataViewCellCoord(void);                                  ///< \brief destructor

        //row, cols
        long GetRow(void) const;                                            ///< \brief get the row
        long GetColumn(void) const;                                         ///< \brief get the column
        void SetRow(long lRow);                                             ///< \brief set the row
        void SetColumn(long lCol);                                          ///< \brief set the column

        //comparison
        bool operator==(const wxVirtualDataViewCellCoord &rhs) const;       ///< \brief operator ==
        bool operator!=(const wxVirtualDataViewCellCoord &rhs) const;       ///< \brief operator !=
        bool operator>=(const wxVirtualDataViewCellCoord &rhs) const;       ///< \brief operator >=
        bool operator<=(const wxVirtualDataViewCellCoord &rhs) const;       ///< \brief operator <=
        bool operator>(const wxVirtualDataViewCellCoord &rhs) const;        ///< \brief operator <
        bool operator<(const wxVirtualDataViewCellCoord &rhs) const;        ///< \brief operator >

        //hashing
        unsigned long HashCode(void) const;                                 ///< \brief compute a hash code
    protected:
        //data
        long    m_lRow;                                                     ///< \brief the row
        long    m_lColumn;                                                  ///< \brief the column
};

//---------------- HASH FUNCTOR -------------------------------------//
class wxVirtualDataViewCellCoordHash
{
    public:
        wxVirtualDataViewCellCoordHash(void) {}

        unsigned long operator()(const wxVirtualDataViewCellCoord &key) const
        {
            return(key.HashCode());
        }

        wxVirtualDataViewCellCoordHash& operator=(const wxVirtualDataViewCellCoordHash&)
        {
            return(*this);
        }
};

//----------------- EQUAL FUNCTOR -----------------------------------//
class wxVirtualDataViewCellCoordEqual
{
    public:
        wxVirtualDataViewCellCoordEqual(void) {}

        unsigned long operator()(const wxVirtualDataViewCellCoord &a,
                                 const wxVirtualDataViewCellCoord &b) const
        {
            return(a == b);
        }

        wxVirtualDataViewCellCoordEqual& operator=(const wxVirtualDataViewCellCoordEqual&)
        {
            return(*this);
        }
};

//--------------------- INLINE IMPLEMENTATION -----------------------//
//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Default constructor
  * \param lRow [input]: the row index. A negative means all rows
  * \param lCol [input]: the column index. A negative value means all columns
  */
WX_VDV_INLINE wxVirtualDataViewCellCoord::wxVirtualDataViewCellCoord(long lRow , long lCol)
    : m_lRow(lRow),
      m_lColumn(lCol)
{
    if (m_lRow    < -1) m_lRow    = -1;
    if (m_lColumn < -1) m_lColumn = -1;
}

/** Copy constructor
  */
WX_VDV_INLINE wxVirtualDataViewCellCoord::wxVirtualDataViewCellCoord(const wxVirtualDataViewCellCoord &rhs)
    : m_lRow(rhs.m_lRow),
      m_lColumn(rhs.m_lColumn)
{
}

/** Assignation operator
  */
WX_VDV_INLINE wxVirtualDataViewCellCoord& wxVirtualDataViewCellCoord::operator=(const wxVirtualDataViewCellCoord &rhs)
{
    m_lRow    = rhs.m_lRow;
    m_lColumn = rhs.m_lColumn;
    return(*this);
}

/** Destructor
  */
WX_VDV_INLINE wxVirtualDataViewCellCoord::~wxVirtualDataViewCellCoord(void)
{
}

//------------------ ROW & COL ACCESS -------------------------------//
/** Get the row
  * \return the row
  */
WX_VDV_INLINE long wxVirtualDataViewCellCoord::GetRow(void) const
{
    return(m_lRow);
}

/** Get the column
  * \return the column
  */
WX_VDV_INLINE long wxVirtualDataViewCellCoord::GetColumn(void) const
{
    return(m_lColumn);
}

/** Set the row
  * \param lRow [input] : the new row
  */
WX_VDV_INLINE void wxVirtualDataViewCellCoord::SetRow(long lRow)
{
    m_lRow = lRow;
    if (m_lRow < -1) m_lRow = -1;
}

/** Set the column
  * \param lCol [input] : the new column
  */
WX_VDV_INLINE void wxVirtualDataViewCellCoord::SetColumn(long lCol)
{
    m_lColumn = lCol;
    if (m_lColumn < -1) m_lColumn = -1;
}

//-------------------- COMPARISON -----------------------------------//
/** Operator == */
WX_VDV_INLINE bool wxVirtualDataViewCellCoord::operator==(const wxVirtualDataViewCellCoord &rhs) const
{
    if (m_lRow    != rhs.m_lRow   ) return(false);
    if (m_lColumn != rhs.m_lColumn) return(false);
    return(true);
}

/** operator != **/
WX_VDV_INLINE bool wxVirtualDataViewCellCoord::operator!=(const wxVirtualDataViewCellCoord &rhs) const
{
    if (m_lRow    != rhs.m_lRow   ) return(true);
    if (m_lColumn != rhs.m_lColumn) return(true);
    return(false);
}

/** operator >= */
WX_VDV_INLINE bool wxVirtualDataViewCellCoord::operator>=(const wxVirtualDataViewCellCoord &rhs) const
{
    return(rhs <= *this);
}

/** operator <= */
WX_VDV_INLINE bool wxVirtualDataViewCellCoord::operator<=(const wxVirtualDataViewCellCoord &rhs) const
{
    if (*this < rhs) return(true);
    if (*this == rhs) return(true);
    return(false);
}

/** Operator > **/
WX_VDV_INLINE bool wxVirtualDataViewCellCoord::operator>(const wxVirtualDataViewCellCoord &rhs) const
{
    return(rhs < *this);
}

/** Operator < **/
WX_VDV_INLINE bool wxVirtualDataViewCellCoord::operator<(const wxVirtualDataViewCellCoord &rhs) const
{
    if (m_lRow < rhs.m_lRow) return(true);
    if (m_lRow > rhs.m_lRow) return(false);
    return(m_lColumn < rhs.m_lColumn);
}

//---------------------- HASHING ------------------------------------//
/** Compute a hash code
  * \return a hash code
  */
WX_VDV_INLINE unsigned long wxVirtualDataViewCellCoord::HashCode(void) const
{
    unsigned long ulResult;
    #if SIZEOF_LONG == 4
    {
        //32 bits
        ulResult = m_lRow;
        ulResult |= (m_lColumn << 16) & 0xFFFF0000;
    }
    #else
    {
        //64 bits
        ulResult = m_lColumn;
        ulResult <<= 32;
        ulResult |= m_lRow;
    }
    #endif // SIZEOF_VOID_P
    return(ulResult);
}

#endif

