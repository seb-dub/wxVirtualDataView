/**********************************************************************/
/** FILE    : DataViewModelCompat.cpp                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualIDataViewModel using a wxDataViewModel      **/
/**********************************************************************/

#include <wx/VirtualDataView/Compatibility/DataViewModelCompat.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0

#include <wx/VirtualDataView/Compatibility/DataViewItemCompat.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/dataview.h>

//--------------- CONSTRUCTORS & DESTRUCTOR -------------------------//
/** Constructor
  * \param pDataModel [input]: the data model to wrap. IncRef is called by
  *                            constructor, DecRef by the destructor
  */
wxDataViewModelCompat::wxDataViewModelCompat(wxDataViewModel *pDataModel)
    : wxVirtualIDataModel(),
      m_pDataModel(pDataModel),
      m_pAttribute(new wxVirtualDataViewItemAttr)
{
    if (m_pDataModel) m_pDataModel->IncRef();
}

/** Destructor
  * DecRef() is called
  */
wxDataViewModelCompat::~wxDataViewModelCompat(void)
{
    if (m_pDataModel) m_pDataModel->DecRef();
    if (m_pAttribute) delete(m_pAttribute);
    m_pAttribute = WX_VDV_NULL_PTR;
}

//--------------------- INTERNAL METHODS ----------------------------//
/** Convert a wxDataViewItem to a wxVirtualItemID
  * \param rID [input]: the item ID to convert
  * \return the converted item ID
  */
wxVirtualItemID wxDataViewModelCompat::ConvertItemID(const wxDataViewItem &rID)
{
    return(DoConvertItemID(rID));
}

/** Convert a wxVirtualItemID to a wxDataViewItem
  * \param rID [input]: the item ID to convert
  * \return the converted ID
  */
wxDataViewItem  wxDataViewModelCompat::ConvertVirtualItemID(const wxVirtualItemID &rID)
{
    return(DoConvertVirtualItemID(rID));
}

/** Convert a wxDataViewItem to a wxVirtualItemID
  * \param rID [input]: the item ID to convert
  * \return the converted item ID
  */
WX_VDV_INLINE wxVirtualItemID wxDataViewModelCompat::DoConvertItemID(const wxDataViewItem &rID)
{
    if (!rID.IsOk()) return(CreateInvalidItemID());
    return(wxVirtualItemID(rID.GetID(), 0, 0, this));
}

/** Convert a wxVirtualItemID to a wxDataViewItem
  * \param rID [input]: the item ID to convert
  * \return the converted ID
  */
WX_VDV_INLINE wxDataViewItem  wxDataViewModelCompat::DoConvertVirtualItemID(const wxVirtualItemID &rID)
{
    return(wxDataViewItem(rID.GetID()));
}

//---------------------- INTERFACE : CLASS TYPE ---------------------//
/** Get the model class flags
  * This is an alternative to RTTI and dynamic_cast
  * This allows to refuse some kind of models for specialization of wxVirtualDataViewCtrl
  * \return a set of flags EClassType
  */
int wxDataViewModelCompat::GetModelClass(void) const
{
    int iResult = WX_MODEL_CLASS_BASE;
    if (m_pDataModel)
    {
        if (m_pDataModel->IsListModel()) iResult |= WX_MODEL_CLASS_LIST;
    }
    return(iResult);
}

//--------------------- INTERFACE: HIERARCHY ------------------------//
/** Get the parent item ID
  * \param rID [input]: the item identifier
  * \return the parent of the item.
  * For list model, it always return the root item, or invalid item for root
  */
wxVirtualItemID wxDataViewModelCompat::GetParent(const wxVirtualItemID &rID)
{
    wxDataViewItem id = DoConvertVirtualItemID(rID);
    wxDataViewItem idChild = m_pDataModel->GetParent(id);
    return(DoConvertItemID(idChild));
}


/** Get the amount of children
  * \param rID [input]: the item identifier
  * \return the amount of children for this item
  *
  * For list models, only the root item has children
  */
size_t wxDataViewModelCompat::GetChildCount(const wxVirtualItemID &rID)
{
    wxDataViewItem id = DoConvertVirtualItemID(rID);
    wxDataViewItemArray vChildren;
    m_pDataModel->GetChildren(id, vChildren);
    return(vChildren.Count());
}

/** Get child item ID
  * \param rIDParent [input]: the ID of the parent item
  * \param uiChildIndex [input]: index of the child item to get
  * \return the ID of the child item
  *
  * For list models, only the root item has children
  */
wxVirtualItemID wxDataViewModelCompat::GetChild(const wxVirtualItemID &rIDParent, size_t uiChildIndex)
{
    wxDataViewItem id = DoConvertVirtualItemID(rIDParent);
    wxDataViewItemArray vChildren;
    m_pDataModel->GetChildren(id, vChildren);
    if (uiChildIndex >= vChildren.Count()) return(CreateInvalidItemID());

    wxDataViewItem idChild = vChildren[uiChildIndex];
    wxVirtualItemID idVChild = DoConvertItemID(idChild);
    idVChild.SetChildIndex(uiChildIndex);
    return(idVChild);
}

/** Get all children at once
  * \param vIDs      [output]: the list of children item. Previous content is erased
  * \param rIDParent [input] : the ID of the parent item
  * \return the amount of children in vIDs
  *
  * The default implementation calls GetChildCount, followed by N calls to GetChild
  * O(k) time (k == amount of children), O(1) space
  */
size_t wxDataViewModelCompat::GetAllChildren(wxVirtualItemIDs &vIDs,
                                             const wxVirtualItemID &rIDParent)
{
    wxDataViewItem id = DoConvertVirtualItemID(rIDParent);
    wxDataViewItemArray vChildren;
    m_pDataModel->GetChildren(id, vChildren);

    size_t i, uiCount;
    uiCount = vChildren.Count();
    vIDs.reserve(uiCount);
    for(i = 0; i < uiCount; i++)
    {
        wxDataViewItem idChild = vChildren[i];
        wxVirtualItemID idVChild = DoConvertItemID(idChild);
        idVChild.SetChildIndex(i);
        vIDs.push_back(idVChild);
    }
    return(uiCount);
}

/** Get the amount of fields in the item
  * The meaning of a field is model dependent. For example, in a tree-list model, it can be the
  * the amount of columns.
  * \return the amount of sub-field in the item.
  *         If the value is size_t(-1), then the item will be drawn on all columns
  * The default implementation returns 1
  *
  * Reimplementation is strongly encouraged
  */
size_t wxDataViewModelCompat::GetFieldCount(const wxVirtualItemID &rID)
{
    return(m_pDataModel->GetColumnCount());
}

//-------------------- INTERFACE : ITEM DATA ------------------------//
/** Get the item data
  * \param rID   [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  *
  * Default implementation return wxVariant().
  * Reimplementation is strongly encouraged
  */
wxVariant wxDataViewModelCompat::GetItemData(const wxVirtualItemID &rID,
                                             size_t uiField, EDataType eType)
{
    if (eType != WX_ITEM_MAIN_DATA) return(wxVariant());

    wxDataViewItem id = DoConvertVirtualItemID(rID);
    wxVariant v;
    m_pDataModel->GetValue(v, id, uiField);
    return(v);
}

/** Get the item graphic attributes
  * \param rID [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param rState [input] : the state of the item
  * \return a pointer to the graphic attributes for this item. The data is owned by the model.
  *         returning a NULL Pointer is valid: the default attributes will be used
  *
  * Default implementation return NULL
  * Reimplementation recommended for visually appealing controls
  */
wxVirtualDataViewItemAttr* wxDataViewModelCompat::GetItemAttribute(const wxVirtualItemID &rID,
                                                                   size_t uiField,
                                                                   const wxVirtualDataViewItemState &rState)
{
    wxDataViewItem id = DoConvertVirtualItemID(rID);
    wxDataViewItemAttr oAttr;
    if (m_pDataModel->GetAttr(id, uiField, oAttr))
    {
        m_pAttribute->Clear();
        if (oAttr.HasColour()) m_pAttribute->SetColour(oAttr.GetColour());
        if (oAttr.HasBackgroundColour()) m_pAttribute->SetBackgroundColour(oAttr.GetBackgroundColour());

        m_pAttribute->SetBold(oAttr.GetBold());
        m_pAttribute->SetItalic(oAttr.GetItalic());

        return(m_pAttribute);
    }
    return(WX_VDV_NULL_PTR);
}

/** Get the item flags
  * \param rID     [input] : the ID of the item to query
  * \param uiField [input] : the field of the item to query
  * \return the item flags.
  *
  * Default implementation returns always WX_ITEM_FLAGS_ALL.
  * Reimplementation recommended for complex usage scenario
  */
wxVirtualIDataModel::EFlags wxDataViewModelCompat::GetItemFlags(const wxVirtualItemID &rID, size_t uiField)
{
    return(WX_ITEM_FLAGS_ALL);
}

/** Set the item data
  * \param rID     [input]: the ID of the item to modify
  * \param uiField [input]: the field of the data to modify
  * \param vValue  [input]: the new value
  * \param eType   [input]: the kind of value to modify
  * \return true if the data was modified, false otherwise
  *
  * Default implementation does nothing and returns true.
  * Reimplement for read/write models
  */
bool wxDataViewModelCompat::SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                        const wxVariant &vValue,
                                        EDataType eType)
{
    if (eType != WX_ITEM_MAIN_DATA) return(false);

    wxDataViewItem id = DoConvertVirtualItemID(rID);
    return(m_pDataModel->SetValue(vValue, id, uiField));
}

//------------------------- SORTING ---------------------------------//
/** Check if the field can be sorted by the data model
  * \param vSortFilters [input]: the sort filters to apply
  * \return true if the model can sort the items according to the filters, false otherwise
  *         Note that true is returned only if ALL THE SORT FILTERS can be taken into account
  *
  * The default implementation returns always false.
  */
bool wxDataViewModelCompat::CanSort(const TSortFilters &vSortFilters) const
{
//    if (m_pDataModel->HasDefaultCompare()) return(false);
//    return(true);
    return(false);
}

/** Sort the values
  * \param vSortFilters [input]: the sort filters to apply
  *
  * The default implementation does nothing
  */
void wxDataViewModelCompat::Sort(const TSortFilters &vSortFilters)
{

}

/** Remove all sort filters
  * The default implementation does the following
  * \code
  *     TSortFilters vSortFilters;
        Sort(vSortFilters);
  * \endcode
  */
void wxDataViewModelCompat::ResetSorting(void)
{
    TSortFilters vSortFilters;
    Sort(vSortFilters);
}

//------------------------- SORTING COMPARISON ----------------------//
/** Compare 2 items
  * \param rID1       [input]: the 1st item to compare
  * \param rID2       [input]: the 2nd item to compare
  * \param uiField    [input]: the field to compare
  * \return WX_E_LESS_THAN    if rID1 < rID2
  *         WX_E_EQUAL        if rID1 == rID2
  *         WX_E_GREATER_THAN if rID1 > rID2
  */
wxVirtualIDataModel::ECompareResult wxDataViewModelCompat::Compare(const wxVirtualItemID &rID1,
                                                                   const wxVirtualItemID &rID2,
                                                                   size_t uiField)
{
    wxDataViewItem id1 = DoConvertVirtualItemID(rID1);
    wxDataViewItem id2 = DoConvertVirtualItemID(rID2);
    int iRes = m_pDataModel->Compare(id1, id2, uiField, true);
    if (iRes < 0) return(WX_E_LESS_THAN);
    if (iRes > 0) return(WX_E_GREATER_THAN);
    return(WX_E_EQUAL);
}

/** Compare 2 items values
  * \param rID1       [input]: the 1st item to compare
  * \param rValue1    [input]: the value of the 1st item to compare
  * \param rID2       [input]: the 2nd item to compare
  * \param rValue2    [input]: the value of the 2nd item to compare
  * \return WX_E_LESS_THAN    if rID1 < rID2
  *         WX_E_EQUAL        if rID1 == rID2
  *         WX_E_GREATER_THAN if rID1 > rID2
  */
wxVirtualIDataModel::ECompareResult wxDataViewModelCompat::Compare(const wxVirtualItemID &rID1,
                                                                   const wxVariant &rValue1,
                                                                   const wxVirtualItemID &rID2,
                                                                   const wxVariant &rValue2,
                                                                   size_t uiField)
{
    wxDataViewItem id1 = DoConvertVirtualItemID(rID1);
    wxDataViewItem id2 = DoConvertVirtualItemID(rID2);
    int iRes = m_pDataModel->Compare(id1, id2, uiField, true);
    if (iRes < 0) return(WX_E_LESS_THAN);
    if (iRes > 0) return(WX_E_GREATER_THAN);
    return(WX_E_EQUAL);
}

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC