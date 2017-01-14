/**********************************************************************/
/** FILE    : VirtualIStateModel.h                                   **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a state model                             **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_STATE_MODEL_H_
#define WX_VIRTUAL_I_STATE_MODEL_H_

#include <wx/VirtualDataView/VirtualItemID.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewItemState;

/** \class wxVirtualIStateModel : a class for providing item states.
  * Each item has the following states:
  *     -enabled / disabled (disabled = edition of data is not possible by the user)
  *     -selected or not
  *     -has focus or not
  *     -is expanded or not
  */
class WXDLLIMPEXP_VDV wxVirtualIStateModel
{
    public:
        //enum
        /// \enum ECheckState : check state of the item
        enum ECheckState
        {
            WX_CHECKSTATE_UNCHECKED         = 0,                                            ///< \brief the item is un-checked
            WX_CHECKSTATE_CHECKED           = 1,                                            ///< \brief the item is checked
            WX_CHECKSTATE_UNDETERMINED      = 2                                             ///< \brief the item state is mixed (or unknown)
        };

        //constructors & destructor
        wxVirtualIStateModel(void);                                                         ///< \brief default constructor
        virtual ~wxVirtualIStateModel(void);                                                ///< \brief destructor

        //interface
        virtual bool IsEnabled(const wxVirtualItemID &rID)       = 0;                       ///< \brief check if the item is enabled / disabled
        virtual bool IsSelected(const wxVirtualItemID &rID)      = 0;                       ///< \brief check if the item is selected / deselected
        virtual bool HasFocus(const wxVirtualItemID &rID)        = 0;                       ///< \brief check if the item has the focus
        virtual bool IsExpanded(const wxVirtualItemID &rID)      = 0;                       ///< \brief check if the item is expanded / collapsed
        virtual ECheckState GetCheck(const wxVirtualItemID &rID) = 0;                       ///< \brief get the check state of the item
        virtual wxVirtualItemID GetFocusedItem(void)             = 0;                       ///< \brief get the focused item
        virtual size_t GetSelectedItems(wxVirtualItemIDs &vIDs)  = 0;                       ///< \brief get the list of selected items

        //get selected/unselected items
        virtual wxVirtualItemID GetFirstSelectedItem(void);                                 ///< \brief get the first selected item
        virtual wxVirtualItemID GetFirstSelectedItem(wxVirtualIDataModel *pDataModel,
                                                     bool bSelected = true);                ///< \brief get the first selected / unselected item
        virtual wxVirtualItemID GetPrevSelectedItem(const wxVirtualItemID &rID,
                                                    wxVirtualIDataModel *pDataModel,
                                                    bool bSelected = true);                 ///< \brief get previous selected / unselected item
        virtual wxVirtualItemID GetNextSelectedItem(const wxVirtualItemID &rID,
                                                    wxVirtualIDataModel *pDataModel,
                                                    bool bSelected = true);                 ///< \brief get next selected / unselected item

        //get checked / unchecked items
        virtual wxVirtualItemID GetFirstCheckedItem(wxVirtualIDataModel *pDataModel,
                                                    ECheckState eState);                    ///< \brief get the first checked/unchecked/mixed item
        virtual wxVirtualItemID GetPrevCheckedItem(const wxVirtualItemID &rID,
                                                   wxVirtualIDataModel *pDataModel,
                                                   ECheckState eState);                     ///< \brief get previous checked/unchecked/mixed item
        virtual wxVirtualItemID GetNextCheckedItem(const wxVirtualItemID &rID,
                                                   wxVirtualIDataModel *pDataModel,
                                                   ECheckState eState);                     /// \brief get next checked/unchecked/mixed item

        //get enabled/disabled items
        virtual wxVirtualItemID GetFirstEnabledItem(wxVirtualIDataModel *pDataModel,
                                                     bool bEnabled = true);                 ///< \brief get the first enabled/disabled item
        virtual wxVirtualItemID GetPrevEnabledItem(const wxVirtualItemID &rID,
                                                    wxVirtualIDataModel *pDataModel,
                                                    bool bEnabled = true);                  ///< \brief get previous enabled/disabled item
        virtual wxVirtualItemID GetNextEnabledItem(const wxVirtualItemID &rID,
                                                    wxVirtualIDataModel *pDataModel,
                                                    bool bEnabled = true);                  ///< \brief get next enabled/disabled item

        //get expanded/collapsed items
        virtual wxVirtualItemID GetFirstExpandedItem(wxVirtualIDataModel *pDataModel,
                                                     bool bExpanded = true);                ///< \brief get the first expanded/collapsed item
        virtual wxVirtualItemID GetPrevExpandedItem(const wxVirtualItemID &rID,
                                                    wxVirtualIDataModel *pDataModel,
                                                    bool bExpanded = true);                 ///< \brief get previous expanded/collapsed item
        virtual wxVirtualItemID GetNextExpandedItem(const wxVirtualItemID &rID,
                                                    wxVirtualIDataModel *pDataModel,
                                                    bool bExpanded = true);                 ///< \brief get next expanded/collapsed item

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

};

#endif

