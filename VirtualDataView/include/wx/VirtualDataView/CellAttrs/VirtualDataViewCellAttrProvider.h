/**********************************************************************/
/** FILE    : VirtualDataViewCellAttrProvider.h                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view cell attribute provider                    **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_CELL_ATTR_PROVIDER_H_
#define _VIRTUAL_DATA_VIEW_CELL_ATTR_PROVIDER_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/VirtualDataView/VirtualDataViewCellCoord.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewDefaultCellAttr.h>
#include <wx/hashmap.h>
#include <wx/sharedptr.h>

//------------------ DECLARATION ------------------------------------//
WX_DECLARE_HASH_MAP(wxVirtualDataViewCellCoord,
                    wxSharedPtr<wxVirtualDataViewCellAttr>,
                    wxVirtualDataViewCellCoordHash,
                    wxVirtualDataViewCellCoordEqual,
                    wxCellAttrHashMap);

/** \class wxVirtualDataViewCellAttrProvider : a class for storing cell attributes
  * The coordinates are models coordinates : reordering of rows/columns/item or sorting are
  * not (and should not) be dealt by this class.
  * The default implementation use internally a hash table
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewCellAttrProvider
{
    public:

        //constructors & destructor
        wxVirtualDataViewCellAttrProvider(void);                                ///< \brief default constructor
        virtual ~wxVirtualDataViewCellAttrProvider(void);                       ///< \brief destructor

        //get attributes
        virtual wxVirtualDataViewCellAttr* GetDefaultAttribute(void) const;     ///< \brief get the default attribute
        virtual wxVirtualDataViewCellAttr* GetRowAttribute(long lRow) const;    ///< \brief get row attribute
        virtual wxVirtualDataViewCellAttr* GetColAttribute(long lCol) const;    ///< \brief get column attribute
        virtual wxVirtualDataViewCellAttr* GetCellAttribute(long lRow,
                                                            long lCol) const;   ///< \brief get cell attribute
        virtual wxVirtualDataViewCellAttr*
            GetCellAttribute(const wxVirtualDataViewCellCoord &rCoord) const;   ///< \brief get cell attribute

        //set attribute
        virtual void SetDefaultAttribute(wxVirtualDataViewCellAttr *pAttr);     ///< \brief set the default attribute
        virtual void SetRowAttribute(long lRow,
                                     wxVirtualDataViewCellAttr *pAttr);         ///< \brief set the row attribute
        virtual void SetColAttribute(long lCol,
                                     wxVirtualDataViewCellAttr *pAttr);         ///< \brief set the column attribute
        virtual void SetCellAttribute(long lRow, long lCol,
                                      wxVirtualDataViewCellAttr *pAttr);        ///< \brief set the cell attribute
        virtual void SetCellAttribute(const wxVirtualDataViewCellCoord &rCoord,
                                      wxVirtualDataViewCellAttr *pAttr);        ///< \brief set the cell attribute

        //effectivity calculation
        virtual wxVirtualDataViewRenderer* GetEffectiveRenderer(long lRow,
                                                                long lCol);     ///< \brief get effective cell renderer
        virtual wxVirtualDataViewItemAttr
            GetEffectiveAttribute(long lRow, long lCol,
                                  const wxVirtualDataViewItemAttr &rBase);      ///< \brief get effective cell attribute
    protected:
        //data
        wxCellAttrHashMap                   m_MapOfCellsAttributes;             ///< \brief store all attributes
        wxVirtualDataViewDefaultCellAttr    m_DefaultAttribute;                 ///< \brief default attribute (when everything else failed)

    private:
        //method
        wxVirtualDataViewCellAttr*
            DoFind(const wxVirtualDataViewCellCoord &rCoord) const;             ///< \brief helper : find the attributes
        void DoSet(const wxVirtualDataViewCellCoord &rCoord,
                   wxVirtualDataViewCellAttr *pAttr);                           ///< \brief helper : set the attributes
};

#endif

