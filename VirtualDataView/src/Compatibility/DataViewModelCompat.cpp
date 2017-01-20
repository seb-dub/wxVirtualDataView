/**********************************************************************/
/** FILE    : DataViewModelCompat.cpp                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : an implementation of wxDataViewModel using a wxVDV mod **/
/**********************************************************************/

#include <wx/VirtualDataView/Compatibility/DataViewModelCompat.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  * \param pDataModel [input]: the data model to use. Ownership is not taken
  */
wxDataViewModelCompat::wxDataViewModelCompat(wxVirtualIDataModel *pDataModel)
    : wxDataViewModel(),
      m_pDataModel(pDataModel)
{

}

/** Destructor
  */
wxDataViewModelCompat::~wxDataViewModelCompat(void)
{
    m_pDataModel = WX_VDV_NULL_PTR;
}


//---------------- GET / SET INTERNAL MODEL -------------------------//
/** Get the internal data model
  * \return the internal data model
  */
wxVirtualIDataModel* wxDataViewModelCompat::GetModel(void)
{
    return(m_pDataModel);
}

/** Set the internal data model
  * \param pDataModel [input]: the new internal data model. Ownership not taken
  */
void wxDataViewModelCompat::SetModel(wxVirtualIDataModel *pDataModel)
{
    m_pDataModel = pDataModel;
}

//--------- wxDataViewModel INTERFACE IMPLEMENTATION ----------------//
/** Get amount of columns
  * return the amount of columns
  */
unsigned int wxDataViewModelCompat::GetColumnCount(void) const
{
    if (!m_pDataModel) return(0);
    wxVirtualItemID idRoot = m_pDataModel->GetRootItem();
    return(m_pDataModel->GetFieldCount(idRoot));
}

/** Get the type of the column
  * \param uiCol [input]: the column index to query
  * \return the type of the column
  */
wxString wxDataViewModelCompat::GetColumnType(unsigned int uiCol) const
{
    return("string");
}

/** Get the value of an item
  * \param rvVariant        [output]: the value of the item
  * \param rID              [input] : the ID of the item to query
  * \param uiCol            [input] : the index of the column to query
  */
void wxDataViewModelCompat::GetValue(wxVariant &rvVariant,
                                     const wxDataViewItem &rID, unsigned int uiCol) const
{
    if (!m_pDataModel)
    {
        rvVariant = wxVariant();
        return;
    }
    wxVirtualItemID id(rID.GetID(), 0, 0, m_pDataModel);
    m_pDataModel->GetItemData(id, uiCol, wxVirtualIDataModel::WX_ITEM_MAIN_DATA);
}

/** Set the item value
  * \param rvValue      [input]: the new value of the item
  * \param rID          [input]: the ID of the item to modify
  * \param uiCol        [input]: the index of the column to modify
  * \return true if the item was modified, false otherwise
  */
bool wxDataViewModelCompat::SetValue(const wxVariant &rvVariant,
                                     const wxDataViewItem &rID,
                                     unsigned int uiCol)
{
    if (!m_pDataModel) return(false);
    wxVirtualItemID id(rID.GetID(), 0, 0, m_pDataModel);
    return(m_pDataModel->SetItemData(id, uiCol, rvVariant, wxVirtualIDataModel::WX_ITEM_MAIN_DATA));
}

/** Get the parent of the item
  * \param rID [input]: the ID of the item to query
  * \return the parent of the item
  */
wxDataViewItem wxDataViewModelCompat::GetParent(const wxDataViewItem &rID) const
{
    if (!m_pDataModel) return(wxDataViewItem());
    wxVirtualItemID id(rID.GetID(), 0, 0, m_pDataModel);
    wxVirtualItemID idParent = m_pDataModel->GetParent(id);
    return(wxDataViewItem(idParent.GetID()));
}

/** Check if the item is a container
  * \param rID [input]: the item to query
  * \return true if the item is a container, false otherwise
  */
bool wxDataViewModelCompat::IsContainer(const wxDataViewItem &rID) const
{
    if (!m_pDataModel) return(false);
    wxVirtualItemID id(rID.GetID(), 0, 0, m_pDataModel);
    return(m_pDataModel->GetChildCount(id) > 0);
}

/** Get the list of children
  * \param rID          [input]: the ID of the item to query
  * \param vChildren    [output]: the list of children
  * \return the amount of children
  */
unsigned int wxDataViewModelCompat::GetChildren(const wxDataViewItem &rID,
                                                wxDataViewItemArray &vChildren) const
{
    vChildren.clear();
    if (!m_pDataModel) return(0);

    wxVirtualItemIDs vIDs;
    wxVirtualItemID idParent(rID.GetID(), 0, 0, m_pDataModel);
    m_pDataModel->GetAllChildren(vIDs, idParent);

    size_t i, uiSize;
    uiSize = vIDs.size();
    vChildren.reserve(uiSize);
    for(i=0;i<uiSize;i++)
    {
        wxVirtualItemID &rChild = vIDs[i];
        wxDataViewItem id(rChild.GetID());
        vChildren.push_back(id);
    }

    return(vChildren.size());
}

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

