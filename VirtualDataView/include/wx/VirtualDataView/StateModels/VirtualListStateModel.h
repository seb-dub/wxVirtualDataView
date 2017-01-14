/**********************************************************************/
/** FILE    : VirtualListStateModel.h                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : state model optimized for list views                   **/
/**********************************************************************/


#ifndef WX_VIRTUAL_LIST_STATE_MODEL_H_
#define WX_VIRTUAL_LIST_STATE_MODEL_H_

#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/Types/SizeTHashMap.hpp>

WX_DECLARE_HASH_MAP(size_t, int, wxSizeTHash, wxSizeTEqual, THashMapListState);

/** \class wxVirtualListStateModel : implementation of wxVirtualIStateModel, optimized for list views.
  * The model is optimized for a flat list, namely:
  *     - there is no hierarchy (the items have no children)
  *     - the items can be accessed by a unique index from 0 to GetItemCount() - 1
  *
  * This follows that items cannot be expanded or collapsed. So they always stay expanded.
  * Expand/collapse has no effect
  *
  * The state is stored using a hash-map, with key = row index in the list. More memory could be saved if
  * the state was stored in a flat array for example. This would however require explicit synchronisation between
  * data model and state model (for resizing array each time the amount of items change)
  */
class WXDLLIMPEXP_VDV wxVirtualListStateModel : public wxVirtualIStateModel
{
    public:
        //constructors & destructor
        wxVirtualListStateModel(void);                                                      ///< \brief default constructor
        virtual ~wxVirtualListStateModel(void);                                             ///< \brief destructor

        //item ID methods
        wxVirtualItemID GetItemID(size_t uiRowIndex) const;                                 ///< \brief compute an item ID from a row index
        size_t          GetRowIndex(const wxVirtualItemID &rID) const;                      ///< \brief compute the row index from the item ID

        //interface
        virtual bool IsEnabled(const wxVirtualItemID &rID) ;                                ///< \brief check if the item is enabled / disabled
        virtual bool IsSelected(const wxVirtualItemID &rID);                                ///< \brief check if the item is selected / deselected
        virtual bool HasFocus(const wxVirtualItemID &rID);                                  ///< \brief check if the item has the focus
        virtual bool IsExpanded(const wxVirtualItemID &rID);                                ///< \brief check if the item is expanded / collapsed
        virtual ECheckState GetCheck(const wxVirtualItemID &rID);                           ///< \brief get the check state of the item
        virtual wxVirtualItemID GetFocusedItem(void);                                       ///< \brief get the focused item
        virtual size_t GetSelectedItems(wxVirtualItemIDs &vIDs);                            ///< \brief get the list of selected items

        //interface with default implementation
        virtual bool SetState(const wxVirtualDataViewItemState &rState,
                              const wxVirtualItemID &rID, bool bRecurse);                   ///< \brief set all states at once
        virtual void GetState(wxVirtualDataViewItemState &rState,
                              const wxVirtualItemID &rID);                                  ///< \brief get all states at once

        //callbacks
        virtual void OnDataModelChanged(wxVirtualIDataModel* pDataModel);                   ///< \brief data model has changed
        virtual bool DeleteItem(const wxVirtualItemID &rID);                                ///< \brief delete an item
        virtual bool Enable(const wxVirtualItemID &rID, bool bEnable, bool bRecursive);     ///< \brief enable / disable items
        virtual bool Select(const wxVirtualItemID &rID, bool bSelect, bool bRecursive);     ///< \brief select / deselect items
        virtual bool SelectRange(const wxVirtualItemID &rID1, const wxVirtualItemID &rID2,
                                 bool bSelect = true);                                      ///< \brief select / deselect a range of items
        virtual bool SelectList(const wxVirtualItemIDs &vIDs, bool bSelect = true);         ///< \brief select / deselect a list of items
        virtual bool SelectAll(void);                                                       ///< \brief select all items
        virtual bool DeselectAll(void);                                                     ///< \brief deselect all items
        virtual bool Expand(const wxVirtualItemID &rID, bool bExpand, bool bRecursive);     ///< \brief expand / collapse items
        virtual bool SetCheck(const wxVirtualItemID &rID, ECheckState eCheck,
                              bool bRecursive);                                             ///< \brief set the check state of the item
        virtual bool SetFocus(const wxVirtualItemID &rID);                                  ///< \brief set focus on one item

    private:
        //data
        wxVirtualIDataModel*        m_pDataModel;                                           ///< \brief the data model
        THashMapListState           m_MapOfStates;                                          ///< \brief map of states
        wxVirtualItemID             m_FocusedItem;                                          ///< \brief focused item

        //methods
        wxVirtualItemID DoGetItemID(size_t uiRowIndex) const;                               ///< \brief compute an item ID from a row index
        size_t          DoGetRowIndex(const wxVirtualItemID &rID) const;                    ///< \brief compute the row index from the item ID
        int  GetState(const wxVirtualItemID &rID) const;                                    ///< \brief get the state
        int  GetState(size_t uiRow) const;                                                  ///< \brief get the state
        void SetState(const wxVirtualItemID &rID, int iState);                              ///< \brief set the state
        void SetState(size_t uiRow, int iState);                                            ///< \brief set the state
};

#endif

