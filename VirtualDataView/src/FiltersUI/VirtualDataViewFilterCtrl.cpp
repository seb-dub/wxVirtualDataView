/**********************************************************************/
/** FILE    : VirtualDataViewFilterCtrl.cpp                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a list view for showing values to filter               **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterCtrl.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataFilterModel.h>
#include <wx/VirtualDataView/Columns/VirtualDataViewColumnsList.h>
#include <wx/VirtualDataView/Columns/VirtualDataViewColumn.h>
#include <wx/VirtualDataView/StateModels/VirtualListStateModel.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualListModelRenderer.h>
#include <wx/VirtualDataView/Types/VariantUtils.h>
#include <wx/hashset.h>
#include <wx/log.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewFilterCtrl, wxVirtualDataViewListCtrl);
const char wxVirtualDataViewFilterCtrlNameStr[] = "wxVirtualDataViewFilterCtrl";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewFilterCtrl::wxVirtualDataViewFilterCtrl(void)
    : wxVirtualDataViewListCtrl()
{
}

/** Constructor
  * Creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent   [input]: parent of the window
  * \param id        [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos       [input]: initial position of the window. Default to wxDefaultPosition
  * \param size      [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle    [input]: window style. Default to 0
  * \param validator [input]: validator
  * \param sName     [input]: the name of the object
  */
wxVirtualDataViewFilterCtrl::wxVirtualDataViewFilterCtrl(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                                     const wxSize& size, long lStyle,
                                                     const wxValidator& validator, wxString sName)

    : wxVirtualDataViewListCtrl()
{
    InitDataView();
    Create(pParent, id, pos, size, lStyle, validator, sName,
           new wxVirtualDataFilterModel(), new wxVirtualListStateModel(), WX_VDV_NULL_PTR);
}

/** Destructor (virtual)
  */
wxVirtualDataViewFilterCtrl::~wxVirtualDataViewFilterCtrl(void)
{

}

//----------------------- CREATION METHODS --------------------------//
/** Really creates the control and sets the initial number of items in it
  * (which may be changed later with SetItemCount())
  * the only special style which may be specified here is wxLB_MULTIPLE
  * \param pParent        [input]: parent of the window
  * \param id             [input]: id of the window. default to wxID_ANY (automatically allocated)
  * \param pos            [input]: initial position of the window. Default to wxDefaultPosition
  * \param size           [input]: initial size of the window. Default to wxDefaultSize
  * \param lStyle         [input]: window style. Default to 0
  * \param validator      [input]: validator
  * \param sName          [input]: the name of the object
  * \param pDataModel     [input]: the default data model. Can be NULL (the user needs to provide it later using SetDataModel)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pStateModel    [input]: the default state model. Can be NULL (a wxVirtualListStateModel is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \param pModelRenderer [input]: the default state model. Can be NULL (a wxVirtualTreeModelRenderer is used)
  *                                It must have been allocated with "new". Ownership is taken.
  * \return true on success or false if the control couldn't be created
  */
bool wxVirtualDataViewFilterCtrl::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                         const wxSize& size, long lStyle,
                                         const wxValidator& validator, wxString sName,
                                         wxVirtualIDataModel *pDataModel,
                                         wxVirtualIStateModel *pStateModel,
                                         wxVirtualIModelRenderer *pModelRenderer)
{
    //create control
    if (!wxVirtualDataViewListCtrl::Create(pParent, id, pos, size,
                                           lStyle, validator, sName,
                                           pDataModel,
                                           pStateModel,
                                           pModelRenderer))
    {
        return(false);
    }

    //model renderer
    wxVirtualListModelRenderer *pMR = reinterpret_cast<wxVirtualListModelRenderer*>(GetModelRenderer());
    if (pMR)
    {
        pMR->DrawHorizontalGridLines(false);
        pMR->DrawVerticalGridLines(false);
        pMR->SetAlternatingRowBackColours(false, false);
    }

    //hide headers
    HideColHeader();
    HideRowHeader();

    //add 1 column
    wxVirtualDataViewColumnsList &rvColumns = Columns();
    rvColumns.Clear();

    wxVirtualDataViewColumn oCol("Value", 0);
    oCol.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_STRING);
    oCol.SetFromType();
    oCol.SetMinWidth(100);
    oCol.SetEditable(false);
    oCol.SetActivatable(false);
    oCol.SetReadOnly(true);
    rvColumns.AppendColumn(oCol);

    return(true);
}

//-------------------------- DATA MODEL --------------------------------------------//
/** Set the current data model. Current proxies will be kept
  * \param pModel [input] : the new data model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewFilterCtrl::SetDataModel(wxVirtualIDataModel *pModel)
{
    //desactivated, to avoid setting a new model
//    if (!pModel) return;
//    int iModelClass = pModel->GetModelClass();
//    bool bIsFilter = iModelClass & wxVirtualIDataModel::WX_MODEL_CLASS_FILTER;
//    bool bIsProxy  = iModelClass & wxVirtualIDataModel::WX_MODEL_CLASS_PROXY;
//    if ((!bIsProxy) && (!bIsFilter)) return;
//    wxVirtualDataViewCtrl::SetDataModel(pModel);
}

/** Get the filter data model
  * \return the filter data model. Type is checked.
  */
wxVirtualDataFilterModel* wxVirtualDataViewFilterCtrl::GetFilterDataModel(void)
{
    wxVirtualIDataModel *pDataModel = GetDataModel();
    if (!pDataModel) return(WX_VDV_NULL_PTR);

    int iModelClass = pDataModel->GetModelClass();
    bool bIsFilter = iModelClass & wxVirtualIDataModel::WX_MODEL_CLASS_FILTER;
    bool bIsProxy  = iModelClass & wxVirtualIDataModel::WX_MODEL_CLASS_PROXY;
    if ((!bIsProxy) && (!bIsFilter)) return(WX_VDV_NULL_PTR);

    return((wxVirtualDataFilterModel *) pDataModel);
}


//---------------------- FILL FILTER --------------------------------//
/** Fill the filter with data from a source model
  * \param pSrcModel    [input]: the data model containing the value to display inside the filter
  * \param uiField      [input]: the field of the data model to consider
  * \param pStateModel  [input]: the state model to use
  */
void wxVirtualDataViewFilterCtrl::InitFilter(wxVirtualIDataModel *pSrcModel, size_t uiField,
                                             wxVirtualIStateModel *pStateModel)
{
    wxVector<wxVariant>& vValues = Values();
    vValues.clear();

    if (pSrcModel)
    {
        pSrcModel->GetAllValues(vValues, uiField, pStateModel);
    }

    wxVirtualDataFilterModel *pFilterModel = GetFilterDataModel();
    if (pFilterModel) pFilterModel->SortValues(wxVirtualIDataModel::WX_E_SORT_ASCENDING);

    OnDataModelChanged();
    AutoSizeAllColumns();
}

//------------------------- ARRAY OF VALUES -------------------------//
/** Direct access to the array of values
  * \return a reference to the list of values
  */
wxVector<wxVariant>& wxVirtualDataViewFilterCtrl::Values(void)
{
    wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    return(pModel->Values());
}

/** Direct access to the array of values - const version
  * \return a const reference to the list of values
  */
const wxVector<wxVariant>&  wxVirtualDataViewFilterCtrl::Values(void) const
{
    const wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    return(pModel->Values());
}

//--------------------- DISPLAY STRING OF THE FILTER ----------------//
/** Return the filter string to display
  * \return the filter string to display
  */
wxString wxVirtualDataViewFilterCtrl::GetFilterDisplayString(void) const
{
    wxVirtualItemID idFirst = GetFirstCheckedItem();
    wxVirtualItemID idNext = GetNextCheckedItem(idFirst);
    if (!idFirst.IsOK()) return(wxEmptyString);
    if (idNext.IsOK()) return("<custom>"); //more than 1 item.

    //only 1 item
    wxVariant v = GetItemData(idFirst, 0);
    return(GetStringValue(v));
}

//-------------------------- SPECIAL ITEMS --------------------------//
/** Check/uncheck special items
  * \param eState [input]: the new state of the special items
  */
void wxVirtualDataViewFilterCtrl::CheckSpecialItems(ECheckState eState)
{
    const wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    if (!pModel) return;

    wxVirtualItemID id;
    id = pModel->GetBlankItemID();
    if (id.IsOK()) CheckItem(id, eState, false, true);

    id = pModel->GetNonBlankItemID();
    if (id.IsOK()) CheckItem(id, eState, false, true);
}

/** Set if <blanks> item is present
  * \param bHasBlanks [input]: true for adding the item, false for removing it
  */
void wxVirtualDataViewFilterCtrl::SetHasBlanks(bool bHasBlanks)
{
    wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    if (pModel) pModel->SetHasBlanks(bHasBlanks);
}

/** Set if <non-blanks> item is present
  * \param bHasNonBlanks [input]: true for adding the item, false for removing it
  */
void wxVirtualDataViewFilterCtrl::SetHasNonBlanks(bool bHasNonBlanks)
{
    wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    if (pModel) pModel->SetHasNonBlanks(bHasNonBlanks);
}

/** Check if the <blanks> item is present
  * \return true if the item is present, false otherwise
  */
bool wxVirtualDataViewFilterCtrl::HasBlanks(void) const
{
    const wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    if (pModel) return(pModel->HasBlanks());
    return(false);
}

/** Check if the <non-blanks> item is present
  * \return true if the item is present, false otherwise
  */
bool wxVirtualDataViewFilterCtrl::HasNonBlanks(void) const
{
    const wxVirtualDataFilterModel *pModel = reinterpret_cast<wxVirtualDataFilterModel*>(GetDataModel());
    if (pModel) return(pModel->HasNonBlanks());
    return(false);
}

//------------------- SAVE / RESTORE STATE --------------------------//
WX_DECLARE_HASH_SET(wxVirtualItemID, wxVirtualItemIDHash, wxVirtualItemIDEqual, TSetOfIDs);

/** Helper: convert an array to a hash set
  * \param oSetOfIDs [output]: the set of item IDs
  * \param vIDs      [input] : the array of item IDs
  */
WX_VDV_INLINE void VectorToHashSet(TSetOfIDs &oSetOfIDs, const wxVirtualItemIDs &vIDs)
{
    size_t i, uiCount;
    uiCount = vIDs.size();
    for(i = 0; i < uiCount; i++) oSetOfIDs.insert(vIDs[i]);
}

/** Save the current state
  */
void wxVirtualDataViewFilterCtrl::SaveState(void)
{
    GetCheckedItems(m_vSavedCheckedItems, WX_CHECKSTATE_CHECKED);
}

/** Restore the saved state
  */
void wxVirtualDataViewFilterCtrl::RestoreState(void)
{
    //step 1 : put all the checked items inside a set
    TSetOfIDs oSetOfIDs;
    VectorToHashSet(oSetOfIDs, m_vSavedCheckedItems);

    //step 2 : loop on all items
    wxVirtualIDataModel *pDataModel = GetDataModel();
    if (!pDataModel) return;

    wxVirtualIStateModel *pStateModel = GetStateModel();
    if (!pStateModel) return;

    TSetOfIDs::iterator itEnd = oSetOfIDs.end();
    wxVirtualItemID id = pDataModel->GetRootItem();
    while (!id.IsInvalid())
    {
        if (oSetOfIDs.find(id) != itEnd)
        {
            pStateModel->SetCheck(id, wxVirtualIStateModel::WX_CHECKSTATE_CHECKED, false);
        }
        else
        {
            pStateModel->SetCheck(id, wxVirtualIStateModel::WX_CHECKSTATE_UNCHECKED, false);
        }
        id = pDataModel->NextItem(id, WX_VDV_NULL_PTR, 1);
    }
}
