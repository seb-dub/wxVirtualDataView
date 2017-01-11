/**********************************************************************/
/** FILE    : VirtualItemIDSearch.hpp                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : utilities used for helping iterating on items          **/
/**********************************************************************/


#ifndef WX_VIRTUAL_ITEM_ID_SEARCH_HPP_
#define WX_VIRTUAL_ITEM_ID_SEARCH_HPP_

//---------------------- UTILITIES METHODS --------------------------//
template<typename TJob, bool bSkipCurrent>
wxVirtualItemID wxGetNextVirtualItemID(TJob &rJob);             ///< \brief find the next item ID fulfilling a criteria

template<typename TJob, bool bSkipCurrent>
wxVirtualItemID wxGetPrevVirtualItemID(TJob &rJob);             ///< \brief find the previous item ID fulfilling a criteria

//-------------------- BASE JOB DECLARATION -------------------------//
/** \struct wxVirtualItemIDSearcher defines the base class for functors to use in
  * wxGetNextVirtualItemID and wxGetPrevVirtualItemID
  */
struct wxVirtualItemIDSearcher
{
    //constructor
    wxVirtualItemIDSearcher(wxVirtualIStateModel *pStateModel,
                            wxVirtualIDataModel *pDataModel,
                            wxVirtualItemID idStart);           ///< \brief constructor

    //base methods
    bool ItemIsOK(void);                                        ///< \brief returns true if the current item is valid
    wxVirtualItemID GetItem(void);                              ///< \brief return the current item
    void NextItem(void);                                        ///< \brief move to the next item
    void PrevItem(void);                                        ///< \brief move to the previous item

    //data
    wxVirtualIStateModel*       m_pStateModel;                  ///< \brief state model
    wxVirtualIDataModel*        m_pDataModel;                   ///< \brief data model
    wxVirtualItemID             m_idCurrent;                    ///< \brief current item
};

//----------------- COMMONLY USED FUNCTORS --------------------------//
//--------------- SELECTED ITEMS ------------------------------------//
/// \struct wxVirtualItemSearchSelected: finds the next/previous selected item
struct wxVirtualItemSearchSelected : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchSelected(wxVirtualIStateModel *pStateModel,
                                wxVirtualIDataModel *pDataModel,
                                wxVirtualItemID idStart);       ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria
};

//--------------- UNSELECTED ITEMS ----------------------------------//

/// \struct wxVirtualItemSearchUnselected: finds the next/previous unselected item
struct wxVirtualItemSearchUnselected : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchUnselected(wxVirtualIStateModel *pStateModel,
                                wxVirtualIDataModel *pDataModel,
                                wxVirtualItemID idStart);       ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria
};

//------------------- CHECKED / UNCHECKED ITEM ----------------------//
/// \struct wxVirtualItemSearchChecked: finds the next/previous checked/unchecked/mixed item
struct wxVirtualItemSearchChecked : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchChecked(wxVirtualIStateModel *pStateModel,
                               wxVirtualIDataModel *pDataModel,
                               wxVirtualItemID idStart,
                               int iState);                     ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria

    //data
    int     m_iSearchedState;                                   ///< \brief searched state
};

//----------------- ENABLED ITEMS -----------------------------------//
/// \struct wxVirtualItemSearchEnabled: finds the next/previous enabled item
struct wxVirtualItemSearchEnabled : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchEnabled(wxVirtualIStateModel *pStateModel,
                                wxVirtualIDataModel *pDataModel,
                                wxVirtualItemID idStart);       ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria
};

//----------------- DISABLED ITEMS ----------------------------------//
/// \struct wxVirtualItemSearchDisabled: finds the next/previous disabled item
struct wxVirtualItemSearchDisabled : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchDisabled(wxVirtualIStateModel *pStateModel,
                                wxVirtualIDataModel *pDataModel,
                                wxVirtualItemID idStart);       ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria
};

//------------------- EXPANDED ITEMS --------------------------------//
/// \struct wxVirtualItemSearchExpanded: finds the next/previous expanded item
struct wxVirtualItemSearchExpanded : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchExpanded(wxVirtualIStateModel *pStateModel,
                                wxVirtualIDataModel *pDataModel,
                                wxVirtualItemID idStart);       ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria
};

//------------------- COLLAPSED ITEMS -------------------------------//
/// \struct wxVirtualItemSearchCollapsed: finds the next/previous collapsed item
struct wxVirtualItemSearchCollapsed : public wxVirtualItemIDSearcher
{
    wxVirtualItemSearchCollapsed(wxVirtualIStateModel *pStateModel,
                                 wxVirtualIDataModel *pDataModel,
                                 wxVirtualItemID idStart);      ///< \brief constructor
    bool CheckItem(void);                                       ///< \brief returns true if the current item fulfils the criteria
};

//----------------------- INLINE IMPLEMENTATION ---------------------//
//------------------- UTITLITIES METHODS ----------------------------//
/** Helper : find the next item fulfilling a criteria
  * \param bSkipCurrent [template] : if true, the method TJob::NextItem() is called BEFORE entering the main search loop
  *                                  This will in effect skip the current item, which is probably desired when calling the
  *                                  method repeatidly
  *                                  For implementing FindNext/FindPrev : set this param to TRUE
  *                                  For implementing FindFirst : set this param to FALSE
  * \param rJob [input]: the functor. Must implement the following methods:
  *                         bool CheckItem(void); //check if the current item is fulfilling the criteria
  *                         wxVirtualItemID GetItem(void); //get the current item
  *                         void NextItem(void); //move to the next item
  *                         bool ItemIsOK(void); //returns true if the current item is valid
  * \return the next item fulfilling the criteria, or an invalid item if none are found
  */
template<typename TJob, bool bSkipCurrent>
wxVirtualItemID wxGetNextVirtualItemID(TJob &rJob)
{
    if (bSkipCurrent) rJob.NextItem();
    while(rJob.ItemIsOK())
    {
        if (rJob.CheckItem()) return(rJob.GetItem());
        rJob.NextItem();
    }
    return(wxVirtualItemID());
}

/** Helper : find the previous item fulfilling a criteria
  * \param bSkipCurrent [template] : if true, the method TJob::PrevItem() is called BEFORE entering the main search loop
  *                                  This will in effect skip the current item, which is probably desired when calling the
  *                                  method repeatidly
  *                                  For implementing FindNext/FindPrev : set this param to TRUE
  *                                  For implementing FindFirst : set this param to FALSE
  * \param rJob [input]: the functor. Must implement the following methods:
  *                         bool CheckItem(void); //check if the current item is fulfilling the criteria
  *                         wxVirtualItemID GetItem(void); //get the current item
  *                         void PrevItem(void); //move to the previous item
  *                         bool ItemIsOK(void); //returns true if the current item is valid
  * \return the previous item fulfilling the criteria, or an invalid item if none are found
  */
template<typename TJob, bool bSkipCurrent>
wxVirtualItemID wxGetPrevVirtualItemID(TJob &rJob)
{
    if (bSkipCurrent) rJob.PrevItem();
    while(rJob.ItemIsOK())
    {
        if (rJob.CheckItem()) return(rJob.GetItem());
        rJob.PrevItem();
    }
    return(wxVirtualItemID());
}

//--------------- wxVirtualItemIDSearcher ---------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemIDSearcher::wxVirtualItemIDSearcher(wxVirtualIStateModel *pStateModel,
                                                               wxVirtualIDataModel *pDataModel,
                                                               wxVirtualItemID idStart)
    : m_pStateModel(pStateModel),
      m_pDataModel(pDataModel),
      m_idCurrent(idStart)
{
}

/** returns true if the current item is valid **/
WX_VDV_INLINE bool wxVirtualItemIDSearcher::ItemIsOK(void)
{
    return(!m_idCurrent.IsInvalid());
}

/** return the current item **/
WX_VDV_INLINE wxVirtualItemID wxVirtualItemIDSearcher::GetItem(void)
{
    return(m_idCurrent);
}

/** move to the next item **/
WX_VDV_INLINE void wxVirtualItemIDSearcher::NextItem(void)
{
    m_idCurrent = m_pDataModel->NextItem(m_idCurrent, WX_VDV_NULL_PTR, 1);
}

/** move to the previous item **/
WX_VDV_INLINE void wxVirtualItemIDSearcher::PrevItem(void)
{
    m_idCurrent = m_pDataModel->PrevItem(m_idCurrent, WX_VDV_NULL_PTR, 1);
}

//------------------------ SELECTED ITEMS ---------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemSearchSelected::wxVirtualItemSearchSelected(wxVirtualIStateModel *pStateModel,
                                                                       wxVirtualIDataModel *pDataModel,
                                                                       wxVirtualItemID idStart)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchSelected::CheckItem(void)
{
    return(m_pStateModel->IsSelected(m_idCurrent));
}

//------------------- UNSELECTED ITEMS ------------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemSearchUnselected::wxVirtualItemSearchUnselected(wxVirtualIStateModel *pStateModel,
                                                                       wxVirtualIDataModel *pDataModel,
                                                                       wxVirtualItemID idStart)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchUnselected::CheckItem(void)
{
    return(!m_pStateModel->IsSelected(m_idCurrent));
}

//------------------- CHECKED / UNCHECKED ITEMS ---------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  * \param iState       [input]: searched state
  **/
WX_VDV_INLINE wxVirtualItemSearchChecked::wxVirtualItemSearchChecked(wxVirtualIStateModel *pStateModel,
                                                                     wxVirtualIDataModel *pDataModel,
                                                                     wxVirtualItemID idStart, int iState)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart),
      m_iSearchedState(iState)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchChecked::CheckItem(void)
{
    if (m_pStateModel->GetCheck(m_idCurrent) == m_iSearchedState) return(true);
    return(false);
}

//------------------------ ENABLED ITEMS ----------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemSearchEnabled::wxVirtualItemSearchEnabled(wxVirtualIStateModel *pStateModel,
                                                                     wxVirtualIDataModel *pDataModel,
                                                                     wxVirtualItemID idStart)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchEnabled::CheckItem(void)
{
    return(m_pStateModel->IsEnabled(m_idCurrent));
}

//------------------- DISABLED ITEMS --------------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemSearchDisabled::wxVirtualItemSearchDisabled(wxVirtualIStateModel *pStateModel,
                                                                       wxVirtualIDataModel *pDataModel,
                                                                       wxVirtualItemID idStart)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchDisabled::CheckItem(void)
{
    return(!m_pStateModel->IsEnabled(m_idCurrent));
}

//------------------------ EXPANDED ITEMS ---------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemSearchExpanded::wxVirtualItemSearchExpanded(wxVirtualIStateModel *pStateModel,
                                                                       wxVirtualIDataModel *pDataModel,
                                                                       wxVirtualItemID idStart)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchExpanded::CheckItem(void)
{
    return(m_pStateModel->IsExpanded(m_idCurrent));
}

//------------------------ COLLAPSED ITEMS --------------------------//
/** Constructor
  * \param pStateModel  [input]: the state model to use
  * \param pDataModel   [input]: the data model to use
  * \param idStart      [input]: the starting item ID
  **/
WX_VDV_INLINE wxVirtualItemSearchCollapsed::wxVirtualItemSearchCollapsed(wxVirtualIStateModel *pStateModel,
                                                                         wxVirtualIDataModel *pDataModel,
                                                                         wxVirtualItemID idStart)
    : wxVirtualItemIDSearcher(pStateModel, pDataModel, idStart)
{}

/** Returns true if the current item fulfils the criteria
  */
WX_VDV_INLINE bool wxVirtualItemSearchCollapsed::CheckItem(void)
{
    return(!m_pStateModel->IsExpanded(m_idCurrent));
}


#endif

