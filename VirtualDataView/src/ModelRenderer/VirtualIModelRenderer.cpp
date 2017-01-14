/**********************************************************************/
/** FILE    : VirtualIModelRenderer.cpp                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a model renderer                          **/
/**********************************************************************/

#include <wx/VirtualDataView/ModelRenderer/VirtualIModelRenderer.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewBase.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>

//-------------------- CONSTRUCTORS & DESTRUCTOR --------------------//
/** Constructor
  * \param pClient   [input]: the client window
  * \param pScroller [input]: the window with the scrollbars
  */
wxVirtualIModelRenderer::wxVirtualIModelRenderer(wxVirtualDataViewBase* pClient,
                                                 wxVirtualDataViewCtrl* pScroller)
    : m_pClient(pClient),
      m_pScroller((wxWindow*) pScroller)
{

}

/** Destructor
  */
wxVirtualIModelRenderer::~wxVirtualIModelRenderer(void)
{

}

//---------------------- CLIENT -------------------------------------//
/** Get the client window
  * \return the client window
  */
wxVirtualDataViewBase* wxVirtualIModelRenderer::GetClientWindow(void) const
{
    return(m_pClient);
}

/** Get the controlling window
  * \return the controlling window
  */
wxVirtualDataViewCtrl* wxVirtualIModelRenderer::GetControlWindow(void) const
{
    if (!m_pClient) return(WX_VDV_NULL_PTR);
    return(m_pClient->GetOwner());
}

/** Set the client window
  * \param pClient [input]: the new client window
  */
void wxVirtualIModelRenderer::SetClientWindow(wxVirtualDataViewBase *pClient)
{
    m_pClient = pClient;
}

//---------------------- SCROLLER WINDOW ----------------------------//
/** Get the window with the scrollbars
  * \return the window with the scrollbars
  */
wxWindow* wxVirtualIModelRenderer::GetScrollerWindow(void)
{
    return(m_pScroller);
}

/** Set the window with the scrollbars
  * \param pScroller [input]: the window with the scrollbars
  */
void wxVirtualIModelRenderer::SetScrollerWindow(wxWindow* pScroller)
{
    m_pScroller = pScroller;
}

//--------------------- INTERFACE -----------------------------------//
/** Refresh the display
  */
void wxVirtualIModelRenderer::Refresh(void)
{
//    if (!m_pClient) return;
//    m_pClient->Refresh(true, WX_VDV_NULL_PTR);
//    m_pClient->Update();
    wxClientDC dc(m_pClient);
    wxBufferedDC bdc(&dc);
    PaintModel(bdc);

    if (m_pClient) m_pClient->RefreshHeaders();
}

/** Get the best size of a column
  * \param uiColID [input] : the column ID to measure (view coord)
  * \return the best size of the column
  */
wxSize wxVirtualIModelRenderer::GetBestSize(size_t uiColID) const
{
    return(GetBestSize());
}

/** Get the 1st visible item
  * \return the 1st visible item
  */
wxVirtualItemID wxVirtualIModelRenderer::GetFirstVisibleItem(void) const
{
    wxVirtualItemID id;
    if (!m_pClient) return(id);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(id);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(id);

    id = pDataModel->NextItem(id, pStateModel, 1);
    while(id.IsOK())
    {
        if (IsItemVisible(id, 0, false)) return(id);
        id = pDataModel->NextItem(id, pStateModel, 1);
    }

    return(id);
}

/** Get the next visible item
  * \param rID [input]: the starting item (never returned)
  * \return the next visible item
  */
wxVirtualItemID wxVirtualIModelRenderer::GetNextVisibleItem(const wxVirtualItemID &rID) const
{
    wxVirtualItemID id;
    if (!m_pClient) return(id);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(id);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(id);

    id = pDataModel->NextItem(rID, pStateModel, 1);
    while(id.IsOK())
    {
        if (IsItemVisible(id, 0, false)) return(id);
        id = pDataModel->NextItem(id, pStateModel, 1);
    }

    return(id);
}

/** Get the previous visible item
  * \param rID [input]: the starting item (never returned)
  * \return the previous visible item
  */
wxVirtualItemID wxVirtualIModelRenderer::GetPrevVisibleItem(const wxVirtualItemID &rID) const
{
    wxVirtualItemID id;
    if (!m_pClient) return(id);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(id);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(id);

    id = pDataModel->PrevItem(rID, pStateModel, 1);
    while(id.IsOK())
    {
        if (IsItemVisible(id, 0, false)) return(id);
        id = pDataModel->PrevItem(id, pStateModel, 1);
    }

    return(id);
}

/** Get all visible items
  * \param vIDs [input]: the visible items. Previous content is lost
  * \return the amount of visible items in vIDs
  */
size_t wxVirtualIModelRenderer::GetAllVisibleItems(wxVirtualItemIDs &vIDs) const
{
    vIDs.clear();
    if (!m_pClient) return(0);

    wxVirtualIDataModel *pDataModel = m_pClient->GetDataModel();
    if (!pDataModel) return(0);

    wxVirtualIStateModel *pStateModel = m_pClient->GetStateModel();
    if (!pStateModel) return(0);

    wxVirtualItemID id = pDataModel->GetRootItem();
    id = pDataModel->NextItem(id, pStateModel, 1);
    while(id.IsOK())
    {
        if (IsItemVisible(id, 0, false)) vIDs.push_back(id);
        id = pDataModel->NextItem(id, pStateModel, 1);
    }

    return(vIDs.size());
}
