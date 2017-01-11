/**********************************************************************/
/** FILE    : VirtualDataViewCellAttrProvider.cpp                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view cell attribute                             **/
/**********************************************************************/


#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttrProvider.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>

//---------------------- CONSTRUCTORS & DESTRUCTOR ------------------//
/** Default constructor
  * \param sType     [input]: the type name (identify a wxVariant type)
  *                           default = "string"
  * \param pRenderer [input]: the renderer (default = NULL)
  * \param pEditor   [input]: the editor (default = NULL)
  * \param pAttr     [input]: the default graphic attributes (default = NULL)
  */
wxVirtualDataViewCellAttrProvider::wxVirtualDataViewCellAttrProvider(void)
{
}

/** Destructor
  */
wxVirtualDataViewCellAttrProvider::~wxVirtualDataViewCellAttrProvider(void)
{
}

//--------------------- GET ATTRIBUTES ------------------------------//
/** Helper : find the attributes
  * \param rCoord [input] : the coordinate of the cell
  * \return a pointer to the attribute
  */
WX_VDV_INLINE wxVirtualDataViewCellAttr*
 wxVirtualDataViewCellAttrProvider::DoFind(const wxVirtualDataViewCellCoord &rCoord) const
{
    wxCellAttrHashMap::const_iterator it = m_MapOfCellsAttributes.find(rCoord);
    if (it != m_MapOfCellsAttributes.end()) return(it->second.get());
    return(WX_VDV_NULL_PTR);
}

/** Get the default attribute
  * \return a pointer to the attribute.
  *         Do not delete it : it is still owned by the provider object
  */
wxVirtualDataViewCellAttr* wxVirtualDataViewCellAttrProvider::GetDefaultAttribute(void) const
{
    wxVirtualDataViewCellCoord c(-1, -1);
    return(DoFind(c));
}

/** Get row attribute
  * \param lRow [input] : the index of the row
  * \return a pointer to the attribute.
  *         Do not delete it : it is still owned by the provider object
  */
wxVirtualDataViewCellAttr* wxVirtualDataViewCellAttrProvider::GetRowAttribute(long lRow) const
{
    wxVirtualDataViewCellCoord c(lRow, -1);
    return(DoFind(c));
}

/** Get column attribute
  * \param lCol [input] : the index of the column
  * \return a pointer to the attribute.
  *         Do not delete it : it is still owned by the provider object
  */
wxVirtualDataViewCellAttr* wxVirtualDataViewCellAttrProvider::GetColAttribute(long lCol) const
{
    wxVirtualDataViewCellCoord c(-1, lCol);
    return(DoFind(c));
}

/** Get cell attribute
  * \param lRow [input] : the index of the row
  * \param lCol [input] : the index of the column
  * \return a pointer to the attribute.
  *         Do not delete it : it is still owned by the provider object
  */
wxVirtualDataViewCellAttr* wxVirtualDataViewCellAttrProvider::GetCellAttribute(long lRow, long lCol) const
{
    wxVirtualDataViewCellCoord c(lRow, lCol);
    return(DoFind(c));
}

/** Get cell attribute
  * \param rCoord [input] : the cell coordinates
  * \return a pointer to the attribute.
  *         Do not delete it : it is still owned by the provider object
  */
wxVirtualDataViewCellAttr*
 wxVirtualDataViewCellAttrProvider::GetCellAttribute(const wxVirtualDataViewCellCoord &rCoord) const
{
    return(DoFind(rCoord));
}

//--------------- SET ATTRIBUTES ------------------------------------//
/** Helper : set the attributes
  * \param rCoord   [input]: the cell coordinates
  * \param pAttr  [input]: the attribute object.
  *                        Ownership is taken: it must have been allocated with "new"
  */
WX_VDV_INLINE void wxVirtualDataViewCellAttrProvider::DoSet(const wxVirtualDataViewCellCoord &rCoord,
                                                            wxVirtualDataViewCellAttr *pAttr)
{
    //special case : delete existing attribute
    if (!pAttr)
    {
        m_MapOfCellsAttributes.erase(rCoord);
        return;
    }

    wxCellAttrHashMap::iterator it = m_MapOfCellsAttributes.find(rCoord);
    if (it != m_MapOfCellsAttributes.end())
    {
        //already existing. Is it the same ?
        if (it->second.get() == pAttr) return;
        it->second.reset(pAttr);
    }
    else
    {
        wxSharedPtr<wxVirtualDataViewCellAttr> shp(pAttr);
        m_MapOfCellsAttributes.insert(wxCellAttrHashMap::value_type(rCoord, shp));
    }
}

/** Set the default attributes
  * \param pAttr  [input]: the attribute object.
  *                        Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCellAttrProvider::SetDefaultAttribute(wxVirtualDataViewCellAttr *pAttr)
{
    wxVirtualDataViewCellCoord c(-1, -1);
    DoSet(c, pAttr);
}

/** Set the row attribute
  * \param lRow   [input]: the index of the row to set
  * \param pAttr  [input]: the attribute object.
  *                        Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCellAttrProvider::SetRowAttribute(long lRow, wxVirtualDataViewCellAttr *pAttr)
{
    wxVirtualDataViewCellCoord c(lRow, -1);
    DoSet(c, pAttr);
}

/** Set the column attribute
  * \param lCol   [input]: the index of the column to set
  * \param pAttr  [input]: the attribute object.
  *                        Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCellAttrProvider::SetColAttribute(long lCol, wxVirtualDataViewCellAttr *pAttr)
{
    wxVirtualDataViewCellCoord c(-1, lCol);
    DoSet(c, pAttr);
}

/** Set the cell attribute
  * \param lRow   [input]: the index of the row to set
  * \param lCol   [input]: the index of the column to set
  * \param pAttr  [input]: the attribute object.
  *                        Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCellAttrProvider::SetCellAttribute(long lRow, long lCol, wxVirtualDataViewCellAttr *pAttr)
{
    wxVirtualDataViewCellCoord c(lRow, lCol);
    DoSet(c, pAttr);
}

/** Set the cell attribute
  * \param rCoord [input]: the cell coordinates
  * \param pAttr  [input]: the attribute object.
  *                        Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewCellAttrProvider::SetCellAttribute(const wxVirtualDataViewCellCoord &rCoord,
                                                         wxVirtualDataViewCellAttr *pAttr)
{
    DoSet(rCoord, pAttr);
}

//---------------------- EFFECTIVITY CALCULATION --------------------//
/** Get effective cell renderer
  * The default precedence is :
  *     get renderer for this cell.
  *     If it does not exist, get renderer for this row
  *     If it does not exist, get renderer for this column
  *     If it does not exist, get default renderer
  * \param lRow [input]: the model row index
  * \param lCol [input]: the model column index
  * \return a pointer to the cell renderer to use
  *         Do not delete the object: it is still owned by the renderer
  */
wxVirtualDataViewRenderer* wxVirtualDataViewCellAttrProvider::GetEffectiveRenderer(long lRow,
                                                                                   long lCol)
{
    //precedence : cell then row then col then default
    wxVirtualDataViewCellAttr *pAttr = WX_VDV_NULL_PTR;

    //cell
    wxVirtualDataViewCellCoord cCell(lRow, lCol);
    pAttr = DoFind(cCell);
    if (pAttr)
    {
        if (pAttr->GetRenderer()) return(pAttr->GetRenderer());
    }

    //row
    wxVirtualDataViewCellCoord cRow(lRow, -1);
    pAttr = DoFind(cRow);
    if (pAttr)
    {
        if (pAttr->GetRenderer()) return(pAttr->GetRenderer());
    }

    //col
    wxVirtualDataViewCellCoord cCol(-1, lCol);
    pAttr = DoFind(cCol);
    if (pAttr)
    {
        if (pAttr->GetRenderer()) return(pAttr->GetRenderer());
    }

    //default
    wxVirtualDataViewCellCoord cDefault(-1, -1);
    pAttr = DoFind(cDefault);
    if (pAttr)
    {
        if (pAttr->GetRenderer()) return(pAttr->GetRenderer());
    }

    //base default
    return(m_DefaultAttribute.GetRenderer());
}

/** Get effective cell attribute
  * The default precedence is :
  *     get attributes for this cell.
  *     If it is not complete, add row attributes
  *     If it is not complete, add column attributes
  *     If it is not complete, add default attributes
  *     If it is not complete, add attributes from base
  * \param lRow  [input]: the model row index
  * \param lCol  [input]: the model column index
  * \param rBase [input]: the base attribute
  * \return a pointer to the cell attribute to use
  *         Do not delete the object: it is still owned by the renderer
  */
wxVirtualDataViewItemAttr wxVirtualDataViewCellAttrProvider::GetEffectiveAttribute(long lRow,
                                                                                    long lCol,
                                                                                    const wxVirtualDataViewItemAttr &rBase)
{
    wxVirtualDataViewItemAttr oResult;
    wxVirtualDataViewCellAttr *pAttr = WX_VDV_NULL_PTR;

    //from cell
    wxVirtualDataViewCellCoord cCell(lRow, lCol);
    pAttr = DoFind(cCell);
    if (pAttr)
    {
        if (pAttr->GetGraphicAttributes())
        {
            oResult += pAttr->GetGraphicAttributes();
        }
    }

    //from row
    wxVirtualDataViewCellCoord cRow(lRow, -1);
    pAttr = DoFind(cRow);
    if (pAttr)
    {
        if (pAttr->GetGraphicAttributes())
        {
            oResult += pAttr->GetGraphicAttributes();
        }
    }

    //from col
    wxVirtualDataViewCellCoord cCol(-1, lCol);
    pAttr = DoFind(cCol);
    if (pAttr)
    {
        if (pAttr->GetGraphicAttributes())
        {
            oResult += pAttr->GetGraphicAttributes();
        }
    }

    //from  default
    wxVirtualDataViewCellCoord cDefault(-1, -1);
    pAttr = DoFind(cDefault);
    if (pAttr)
    {
        if (pAttr->GetGraphicAttributes())
        {
            oResult += pAttr->GetGraphicAttributes();
        }
    }

    //base default
    if (m_DefaultAttribute.GetGraphicAttributes())
    {
        oResult += m_DefaultAttribute.GetGraphicAttributes();
    }

    //from base
    oResult += rBase;

    //change font
    wxFont fFont = oResult.GetFont();
    fFont = oResult.GetEffectiveFont(fFont);
    oResult.SetFont(fFont);
    return(oResult);
}
