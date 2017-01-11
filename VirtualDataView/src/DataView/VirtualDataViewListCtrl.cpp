/**********************************************************************/
/** FILE    : VirtualDataViewListCtrl.cpp                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : specialization for list views                          **/
/**********************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/VirtualDataView/DataView/VirtualDataViewListCtrl.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualListStateModel.h>
#include <wx/VirtualDataView/ModelRenderer/VirtualListModelRenderer.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//

wxIMPLEMENT_ABSTRACT_CLASS(wxVirtualDataViewListCtrl, wxVirtualDataViewCtrl);
const char wxVirtualDataViewListCtrlNameStr[] = "wxVirtualDataViewListCtrl";

/** Default constructor. The method Create() must be called
  */
wxVirtualDataViewListCtrl::wxVirtualDataViewListCtrl(void)
    : wxVirtualDataViewCtrl()
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
wxVirtualDataViewListCtrl::wxVirtualDataViewListCtrl(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                                     const wxSize& size, long lStyle,
                                                     const wxValidator& validator, wxString sName)

    : wxVirtualDataViewCtrl()
{
    InitDataView();
    Create(pParent, id, pos, size, lStyle, validator, sName,
           WX_VDV_NULL_PTR, new wxVirtualListStateModel(), WX_VDV_NULL_PTR);
}

/** Destructor (virtual)
  */
wxVirtualDataViewListCtrl::~wxVirtualDataViewListCtrl(void)
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
bool wxVirtualDataViewListCtrl::Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                                       const wxSize& size, long lStyle,
                                       const wxValidator& validator, wxString sName,
                                       wxVirtualIDataModel *pDataModel,
                                       wxVirtualIStateModel *pStateModel,
                                       wxVirtualIModelRenderer *pModelRenderer)
{
    wxVirtualIStateModel *pStateModel2 = pStateModel;
    if (!pStateModel2) pStateModel2 = new wxVirtualListStateModel();

    if (!wxVirtualDataViewCtrl::Create(pParent, id, pos, size,
                                      lStyle, validator, sName,
                                      pDataModel,
                                      pStateModel2,
                                      pModelRenderer))
    {
        return(false);
    }

    if (!pModelRenderer)
    {
        wxVirtualIModelRenderer *pMR = new  wxVirtualListModelRenderer(m_pClientArea, this);
        SetModelRenderer(pMR);
    }

    return(true);
}

//-------------------------- DATA MODEL --------------------------------------------//
/** Set the current data model. Current proxies will be kept
  * \param pModel [input] : the new data model to associate with the control.
  *                         Ownership is taken: it must have been allocated with "new"
  */
void wxVirtualDataViewListCtrl::SetDataModel(wxVirtualIDataModel *pModel)
{
    if (!pModel) return;
    int iModelClass = pModel->GetModelClass();
    bool bIsList  = iModelClass & wxVirtualIDataModel::WX_MODEL_CLASS_LIST;
    bool bIsProxy = iModelClass & wxVirtualIDataModel::WX_MODEL_CLASS_PROXY;
    if ((!bIsProxy) && (!bIsList)) return;
    wxVirtualDataViewCtrl::SetDataModel(pModel);
}

//------------------- ITEM ITERATION : CHILDREN ---------------------//
/** Get the amount of children item
  * \return the amount of items in the control
  */
size_t wxVirtualDataViewListCtrl::GetAmountOfItems(void) const
{
    wxVirtualIDataModel *pDataModel = GetDataModel();
    if (!pDataModel) return(0);

    wxVirtualItemID idRoot = pDataModel->GetRootItem();
    return(pDataModel->GetChildCount(idRoot));
}
