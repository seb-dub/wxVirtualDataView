/**********************************************************************/
/** FILE    : wxVirtualItemID.cpp                                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view item ID                                    **/
/**********************************************************************/

#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

//---------- CONVENIENCE: PARENT, CHILD, SIBLING --------------------//
/** Get the parent item
  * \return the parent of this item
  */
wxVirtualItemID wxVirtualItemID::GetParent(void) const
{
    if (!m_pModel) return(wxVirtualItemID());
    return(m_pModel->GetParent(*this));
}

/** Get the child item
  * \param uiIndex [input]: the index of the child to get
  * \return the child of this item
  */
wxVirtualItemID wxVirtualItemID::GetChild(size_t uiIndex) const
{
    if (!m_pModel) return(wxVirtualItemID());
    return(m_pModel->GetChild(*this, uiIndex));
}

/** Get the amount of children of the item
  * \return the amount of children for this item
  */
size_t wxVirtualItemID::GetChildCount(void) const
{
    if (!m_pModel) return(0);
    return(m_pModel->GetChildCount(*this));
}

/** Get the index of the item (use the wxVirtualIDataModel)
  * \return the index of this item
  */
size_t wxVirtualItemID::FindChildIndex(void) const
{
    if (!m_pModel) return(size_t(-1));
    return(m_pModel->GetChildIndex(*this));
}

/** Get the depth of the item
  * \return the depth of this item
  */
size_t wxVirtualItemID::GetDepth(void) const
{
    if (!m_pModel) return(0);
    return(m_pModel->GetDepth(*this));
}

//-------------------------- MODEL ----------------------------------//
/** Get the model
  * \return the model of the item
  */
wxVirtualIDataModel* wxVirtualItemID::GetTopModel(void) const
{
    if (m_pModel) return(m_pModel->GetTopModel());
    return(m_pModel);
}

//------------------- ROOT ITEM -------------------------------------//
/** Check if the item is the root. (model OK + ID == 0)
  * \return true if the item is the root item, false otherwise
  */
bool wxVirtualItemID::IsRoot(void) const
{
    if (!m_pModel) return(false);
    return(m_pModel->IsRootItem(*this));
}

