/**********************************************************************/
/** FILE    : VirtualTreePath.cpp                                    **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : stores a tree path in a persistent manner              **/
/**********************************************************************/


#include <wx/VirtualDataView/DataView/VirtualTreePath.h>
#include <wx/VirtualDataView/VirtualItemID.h>
#include <wx/VirtualDataView/Models/VirtualIDataModel.h>

//--------------------- CONSTRUCTORS & DESTRUCTOR -------------------//
/** Default constructor
  */
wxVirtualTreePath::wxVirtualTreePath(void)
{
    Init();
}

/** Constructor from an ItemID and a data model
  * \param rID    [input]: the item ID to store
  */
wxVirtualTreePath::wxVirtualTreePath(const wxVirtualItemID &rID)
{
    Init();
    Build(rID);
}

/** Copy constructor
  * \param rhs [input]: the item to copy
  */
wxVirtualTreePath::wxVirtualTreePath(const wxVirtualTreePath &rhs)
{
    Init();
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualTreePath& wxVirtualTreePath::operator=(const wxVirtualTreePath &rhs)
{
    if (this == &rhs) return(*this);
    Release();
    Copy(rhs);
    return(*this);
}

/** Destructor
  */
wxVirtualTreePath::~wxVirtualTreePath(void)
{
    Release();
}


//----------------- INTERNAL METHODS --------------------------------//
/** Initialize the object
  */
void wxVirtualTreePath::Init(void)
{
    m_Path.m_pPath = WX_VDV_NULL_PTR;
    m_uiDepth      = 0;
    m_pDataModel   = WX_VDV_NULL_PTR;
}

/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualTreePath::Copy(const wxVirtualTreePath &rhs)
{
    m_uiDepth       = rhs.m_uiDepth;
    m_pDataModel    = rhs.m_pDataModel;

    //allocate memory and get pointers
          size_t *pPathDest = m_Path.m_vPath;
    const size_t *pPathSrc  = rhs.m_Path.m_vPath;
    if (m_uiDepth >= s_uiStaticSize)
    {
        //dynamic
        m_Path.m_pPath = new size_t[m_uiDepth];
        if (!m_Path.m_pPath)
        {
            Init();
            return;
        }

        pPathDest = m_Path.m_pPath;
        pPathSrc  = rhs.m_Path.m_pPath;
    }

    //copy
    size_t i;
    for(i=0;i<m_uiDepth;i++)
    {
        pPathDest[i] = pPathSrc[i];
    }
}

/** Release an object
  */
void wxVirtualTreePath::Release(void)
{
    if (m_uiDepth >= s_uiStaticSize)
    {
        if (m_Path.m_pPath) delete[] m_Path.m_pPath;
    }
    Init();
}

/** Set the item ID stored
  * \param rID        [input]: the item ID to store
  * \param pDataModel [input]: the data model to use. If NULL, the data model stored in rID is used
  */
void wxVirtualTreePath::Build(const wxVirtualItemID &rID, wxVirtualIDataModel *pDataModel)
{
    //compute the tree depth
    m_pDataModel = pDataModel;
    if (!m_pDataModel) m_pDataModel = rID.GetTopModel();
    if (!m_pDataModel)
    {
        Init();
        return;
    }
    m_uiDepth = m_pDataModel->GetDepth(rID);

    //memory allocation
    size_t *pPath = m_Path.m_vPath;
    if (m_uiDepth >= s_uiStaticSize)
    {
        m_Path.m_pPath = new size_t[m_uiDepth];
        if (!m_Path.m_pPath)
        {
            Init();
            return;
        }
        pPath = m_Path.m_pPath;
    }

    //store - get path from bottom to top
    size_t i;
    wxVirtualItemID id = rID;
    for(i=0;i<m_uiDepth;i++)
    {
        pPath[m_uiDepth - i - 1] = m_pDataModel->GetChildIndex(id);
        id = m_pDataModel->GetParent(id);
    }
}

//------------------------ SET / GET --------------------------------//
/** Retrieve the item ID stored
  * \param pDataModel [input]: the data model to use. If NULL, the stored data model is used
  * \return the item ID stored.
  */
wxVirtualItemID wxVirtualTreePath::Get(wxVirtualIDataModel *pDataModel) const
{
    return(GetItemAtLevel(m_uiDepth - 1, pDataModel));
}

/** Retrieve the item ID at a specific level
  * \param lLevel [input]: the level to retrieve (0-based index)
  * \param pDataModel [input]: the data model to use. If NULL, the stored data model is used
  * \return the item ID
  */
wxVirtualItemID wxVirtualTreePath::GetItemAtLevel(long lLevel, wxVirtualIDataModel *pDataModel) const
{
    //check & init
    wxVirtualIDataModel *pModel = pDataModel;
    if (!pModel) pModel = m_pDataModel;
    if (!pModel)  return(wxVirtualItemID());

    long lDepth = m_uiDepth;
    if ((lLevel < 0) || (lLevel >= lDepth)) return(wxVirtualItemID());

    wxVirtualItemID id = pModel->GetRootItem();
    const size_t *pPath = m_Path.m_vPath;
    if (m_uiDepth >= s_uiStaticSize) pPath = m_Path.m_pPath;

    long i;
    for(i=0;i<=lLevel;i++)
    {
        id = pModel->GetChild(id, pPath[i]);
        if (!id.IsOK()) return(id);
    }

    return(id);
}

/** Get the index of the item at a specific level
  * \param lLevel [input]: the level to query
  * \return the index of the item at this level
  */
size_t wxVirtualTreePath::GetIndexAtLevel(long lLevel) const
{
    long lDepth = m_uiDepth;
    if ((lLevel < 0) || (lLevel >= lDepth)) return(size_t(-1));

    const size_t *pPath = m_Path.m_vPath;
    if (m_uiDepth >= s_uiStaticSize) pPath = m_Path.m_pPath;
    return(pPath[lLevel]);
}

/** Get the level of the item
  * \return the level of the item
  */
long wxVirtualTreePath::GetLevel(void) const
{
    return(m_uiDepth);
}

/** Set the item ID stored
  * \param rID [input]: the ID to store
  * \param pDataModel [input]: the data model to use. If NULL, the data model stored in rID is used
  */
void wxVirtualTreePath::Set(const wxVirtualItemID &rID, wxVirtualIDataModel *pDataModel)
{
    Release();
    Build(rID, pDataModel);
}

/** Set from a rank
  * \param uiRank         [input]: the rank of the item. This is the depth first rank of the item
  *                             0 = root item
  *                             1 = 1st child of root item
  *                             2 = 1st child of item 1, or 2nd child of root item if 1 has no children
  *                             ...
  * \param pDataModel     [input]: the data model to use. If NULL, the data model stored in rStartPoint is used
  * \param pStateModel    [input]: the state model to use. Children of collapsed items are skipped
  *                                if NULL, then all items are considered expanded
  * \param rStartPoint    [input]: the item identifying the start point
  * \param uiStartingRank [input]: rank of the starting point
  */
void wxVirtualTreePath::Set(size_t uiRank,
                            wxVirtualIDataModel *pDataModel,
                            wxVirtualIStateModel *pStateModel,
                            const wxVirtualItemID &rStartPoint,
                            size_t uiStartingRank)
{
    if (!pDataModel) pDataModel = rStartPoint.GetTopModel();
    if (!pDataModel)
    {
        m_pDataModel = WX_VDV_NULL_PTR;
        m_uiDepth    = 0;
        return;
    }
    wxVirtualItemID id = pDataModel->FindItem(uiRank, pStateModel, rStartPoint, uiStartingRank);
    Set(id, pDataModel);
}

/** Set from a rank
  * \param uiRank [input]     : the rank of the item. This is the depth first rank of the item
  *                             0 = root item
  *                             1 = 1st child of root item
  *                             2 = 1st child of item 1, or 2nd child of root item if 1 has no children
  *                             ...
  * \param pDataModel  [input]: the data model to use. The root item is used as start point
  * \param pStateModel [input]: the state model to use. Children of collapsed items are skipped
  *                             if NULL, then all items are considered expanded
  */
void wxVirtualTreePath::Set(size_t uiRank, wxVirtualIDataModel *pDataModel,
                            wxVirtualIStateModel *pStateModel)
{
    if (!pDataModel) return;
    wxVirtualItemID idRoot = pDataModel->GetRootItem();
    Set(uiRank, pDataModel, pStateModel, idRoot, 0);
}

//------------------- DATA MODEL ------------------------------------//
/** Set data model
  * \param pDataModel [input]: the new data model
  */
void wxVirtualTreePath::SetDataModel(wxVirtualIDataModel *pDataModel)
{
    m_pDataModel = pDataModel;
}

/** Get data model
  * \return the current data model
  */
wxVirtualIDataModel* wxVirtualTreePath::GetDataModel(void)
{
    return(m_pDataModel);
}
