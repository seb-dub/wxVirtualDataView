/**********************************************************************/
/** FILE    : VirtualListModelRenderer.cpp                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : list implementation of wxVirtualIModelRenderer         **/
/**********************************************************************/

#include <wx/VirtualDataView/ModelRenderer/VirtualListModelRenderer.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/StateModels/VirtualDataViewItemState.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewBase.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/Columns/VirtualDataViewColumnsList.h>
#include <wx/stack.h>
#include <wx/hashset.h>
#include <wx/settings.h>
#include <wx/renderer.h>
#include <wx/dcmemory.h>
#include <iostream>

//-------------------- CONSTRUCTORS & DESTRUCTOR --------------------//
/** Constructor
  * \param pClient [input]: the client window
  * \param pScroller [input]: the window with the scrollbars
  */
wxVirtualListModelRenderer::wxVirtualListModelRenderer(wxVirtualDataViewBase* pClient,
                                                       wxVirtualDataViewCtrl* pScroller)
    : wxVirtualTreeModelRenderer(pClient, pScroller)
{
    Init();
    m_bHideRootItem = true;
    m_eTreeLinesStyle = E_TREE_LINES_NONE;
    m_TreeButtonSize.Set(0, 0);
    m_iTreeButtonMargins[0] = 0;
    m_iTreeButtonMargins[1] = 0;
    m_iTreeButtonMargins[2] = 0;
    m_iTreeButtonMargins[3] = 0;
    m_iTreeButtonTotalWidth = 0;
    m_bDrawTreeButtons = false;
    m_iIndentation = 0;
}

/** Destructor
  */
wxVirtualListModelRenderer::~wxVirtualListModelRenderer(void)
{

}

//------- INTERFACE IMPLEMENTATION : EVENT HANDLING -----------------//
/** Paint the model
  * \param rDC [input] : the device context to use
  * \return true on success, false on failure (painting incomplete or not done at all)
  */
bool wxVirtualListModelRenderer::PaintModel(wxDC &rDC)
{
//    //check & init
//    if (!m_pClient) return(false);
//    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
//    if (!pDataModel) return(false);
//    if (pDataModel->IsProxyDataModel())
//    {
//        int iDebug;
//        iDebug = 1;
//    }

    return(wxVirtualTreeModelRenderer::PaintModel(rDC));
}

//---------------------- SCROLLBARS ---------------------------------//
/** Convert a line number to a tree path
  * \param iLine [input] : the line number to convert
  * \return the tree path of the item
  */
wxVirtualTreePath wxVirtualListModelRenderer::ConvertRowToTreePath(int iLine) const
{
    wxVirtualTreePath oPath;
    if (!m_pClient) return(oPath);
    wxVirtualIDataModel  *pDataModel  = m_pClient->GetDataModel();
    oPath.Set(iLine + 1, pDataModel, WX_VDV_NULL_PTR);

    return(oPath);
}

