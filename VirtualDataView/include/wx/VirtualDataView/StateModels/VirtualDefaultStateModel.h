/**********************************************************************/
/** FILE    : VirtualDefaultStateModel.h                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : default state model                                    **/
/**********************************************************************/


#ifndef WX_VIRTUAL_DEFAULT_STATE_MODEL_H_
#define WX_VIRTUAL_DEFAULT_STATE_MODEL_H_

#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/hashmap.h>

WX_DECLARE_HASH_MAP(wxVirtualItemID, int, wxVirtualItemIDHash, wxVirtualItemIDEqual, THashMapState);

/** \class wxVirtualDefaultStateModel : default implementation of wxVirtualIStateModel
  */
class WXDLLIMPEXP_VDV wxVirtualDefaultStateModel : public wxVirtualIStateModel
{
    public:
        //constructors & destructor
        wxVirtualDefaultStateModel(void);                                                   ///< \brief default constructor
        virtual ~wxVirtualDefaultStateModel(void);                                          ///< \brief destructor

        //parameters
        bool AllowRecursion(void) const;                                                    ///< \brief check if algorithms are allowed to run recursively
        void AllowRecursion(bool bAllow = true);                                            ///< \brief allow/forbid algorithms to run recursively

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
        friend class JobBase;

        //enum
        /// \enum EStateType : for optimizing select all / deselect all
        enum EStateType
        {
            WX_STATE_DEFAULT         = 0,                                                   ///< \brief state of item is used
            WX_STATE_ALL             = 1,                                                   ///< \brief state is ON for all items
            WX_STATE_NONE            = 2                                                    ///< \brief state is OFF for all items
        };

        //data
        THashMapState               m_MapOfStates;                                          ///< \brief map of states
        EStateType                  m_eSelectionType;                                       ///< \brief kind of selection
        wxVirtualItemID             m_FocusedItem;                                          ///< \brief focused item
        bool                        m_bAllowRecursion;                                      ///< \brief allow recursion

        //methods
        int  GetState(const wxVirtualItemID &rID) const;                                    ///< \brief get the state
        void SetState(const wxVirtualItemID &rID, int iState);                              ///< \brief set the state
};

#endif

