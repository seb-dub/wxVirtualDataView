/**********************************************************************/
/** FILE    : VirtualDataViewListCtrl.h                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : specialization for list views                          **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_LIST_CTRL_H_
#define _WX_VIRTUAL_VIEW_LIST_CTRL_H_

#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>

extern const char wxVirtualDataViewListCtrlNameStr[];

/** \class wxVirtualDataViewListCtrl : a specialization for list views.
  * The data model can be optimized for lists (see wxVirtualIListDataModel for a nearly complete implementation)
  * The state model can as well be optimized: a default implementation wxVirtualListStateModel is used by default, and makes
  * some generic assumptions valid for list views:
  *     - no hierarchy : only the root item has N children
  *     - each item can be accessed by its row index / child index (from 0 to N-1)
  *
  * If the assumptions are not valid, then use wxVirtualDataViewCtrl directly
  * The data model still has to be provided by the user. The class wxVirtualIListDataModel can be used
  * with this class, but it is not mandatory. In the latter case, ensure that the state model is compatible
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewListCtrl : public wxVirtualDataViewCtrl
{
    public:

        //constructors & destructor
        wxVirtualDataViewListCtrl(void);                                        ///< \brief default constructor. Create() must be called manually
        wxVirtualDataViewListCtrl(wxWindow *pParent, wxWindowID id,
                             const wxPoint& pos, const wxSize& size,
                             long lStyle,
                             const wxValidator& validator = wxDefaultValidator,
                           wxString sName = wxVirtualDataViewListCtrlNameStr);  ///< \brief constructor
        virtual ~wxVirtualDataViewListCtrl(void);                               ///< \brief destructor

        //initialization
        virtual bool Create(wxWindow *pParent, wxWindowID id,
                            const wxPoint& pos,  const wxSize& size,
                            long lStyle,
                            const wxValidator& validator, wxString sName,
                            wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR,
                            wxVirtualIStateModel *pStateModel = WX_VDV_NULL_PTR,
                    wxVirtualIModelRenderer *pModelRenderer = WX_VDV_NULL_PTR); ///< \brief really creates the control and sets the initial number of items in it

        //data models
        virtual void            SetDataModel(wxVirtualIDataModel *pModel);      ///< \brief set the current data model. Current proxies will be kept

        //item iteration : children
        size_t          GetAmountOfItems(void) const;                           ///< \brief get the amount of children item

        //selection
//        size_t GetSelectedItems(wxVirtualItemIDs &vIDs) const;                  ///< \brief get all the selected items
//        wxVirtualItemID GetFirstSelectedItem(void) const;                       ///< \brief get the 1st selected item
//        wxVirtualItemID GetNextSelectedItem(const wxVirtualItemID &rID) const;  ///< \brief get the next selected item
//        wxVirtualItemID GetPrevSelectedItem(const wxVirtualItemID &rID) const;  ///< \brief get the previous selected item
//        wxVirtualItemID GetFirstUnselectedItem(void) const;                     ///< \brief get the 1st un-selected item
//        wxVirtualItemID GetNextUnselectedItem(const wxVirtualItemID &rID) const;///< \brief get the next un-selected item
//        wxVirtualItemID GetPrevUnselectedItem(const wxVirtualItemID &rID) const;///< \brief get the previous un-selected item
//        void SelectItem(const wxVirtualItemID &rID, bool bSelect = true,
//                        bool bRecursive = false, bool bSendEvent = true);       ///< \brief select one item and optionally its children
//        void DeselectItem(const wxVirtualItemID &rID, bool bRecursive = false,
//                          bool bSendEvent = true);                              ///< \brief deselect an item and optionally its children
//        void ToggleSelectedState(const wxVirtualItemID &rID,
//                                 bool bRecursive = false,
//                                 bool bSendEvent = true);                       ///< \brief toggle selection state of 1 or more items
//        void SelectAll(bool bSelect = true, bool bSendEvent = true);            ///< \brief select all items
//        void DeselectAll(bool bSendEvent = true);                               ///< \brief de-select all items
//        void SelectItems(const wxVirtualItemIDs &vIDs, bool bSendEvent = true); ///< \brief select one or more items
//        bool IsSelected(const wxVirtualItemID &rID) const;                      ///< \brief check if an item is selected
//        bool IsDeselected(const wxVirtualItemID &rID) const;                    ///< \brief check if an item is deselected

        //check state
//        ECheckState GetCheckState(const wxVirtualItemID &rID) const;            ///< \brief get check state of an item
//        bool        IsChecked(const wxVirtualItemID &rID) const;                ///< \brief test if an item is checked
//        bool        IsUnchecked(const wxVirtualItemID &rID) const;              ///< \brief test if an item is unchecked
//        bool        IsCheckStateUndetermined(const wxVirtualItemID &rID) const; ///< \brief test if an item is in unknown check state
//        void        CheckItem(const wxVirtualItemID &rID,
//                              ECheckState eState = WX_CHECKSTATE_CHECKED,
//                              bool bRecursive = false, bool bSendEvent = true); ///< \brief check / uncheck items
//        void        UncheckItem(const wxVirtualItemID &rID,
//                                bool bRecursive = false,
//                                bool bSendEvent = true);                        ///< \brief uncheck items
//        void        SetCheckUndetermined(const wxVirtualItemID &rID,
//                                         bool bRecursive = false,
//                                         bool bSendEvent = true);               ///< \brief set items in undetermined check state
//        size_t      GetCheckedItems(wxVirtualItemIDs &vIDs,
//                        ECheckState eState = WX_CHECKSTATE_CHECKED) const;      ///< \brief get all the checked items
//        size_t      GetUncheckedItems(wxVirtualItemIDs &vIDs) const;            ///< \brief get all the unchecked items
//        size_t      GetUndeterminedCheckedItems(wxVirtualItemIDs &vIDs) const;  ///< \brief get all the items which are in an undetermined check state
//        wxVirtualItemID GetFirstCheckedItem(ECheckState
//                                        eState = WX_CHECKSTATE_CHECKED) const;  ///< \brief get the 1st checked item
//        wxVirtualItemID GetNextCheckedItem(const wxVirtualItemID &rID,
//                             ECheckState eState = WX_CHECKSTATE_CHECKED) const; ///< \brief get the next checked item
//        wxVirtualItemID GetPrevCheckedItem(const wxVirtualItemID &rID,
//                             ECheckState eState = WX_CHECKSTATE_CHECKED) const; ///< \brief get the previous checked item
//        wxVirtualItemID GetFirstUncheckedItem(void) const;                      ///< \brief get the 1st unchecked item
//        wxVirtualItemID GetNextUncheckedItem(const wxVirtualItemID &rID) const; ///< \brief get the next unchecked item
//        wxVirtualItemID GetPrevUncheckedItem(const wxVirtualItemID &rID) const; ///< \brief get the previous unchecked item
//        wxVirtualItemID GetFirstUndeterminedItem(void) const;                   ///< \brief get the 1st undetermined item
//        wxVirtualItemID GetNextUndeterminedItem(
//                                            const wxVirtualItemID &rID) const;  ///< \brief get the next undetermined item
//        wxVirtualItemID GetPrevUndeterminedItem(
//                                            const wxVirtualItemID &rID) const;  ///< \brief get the previous undetermined item
//        void CheckAll(ECheckState eState = WX_CHECKSTATE_CHECKED,
//                      bool bSendEvent = true);                                  ///< \brief check all items
//        void UncheckAll(bool bSendEvent = true);                                ///< \brief un-check all items
//        void SetAllUndeterminedCheckState(bool bSendEvent = true);              ///< \brief set all items in undetermined check state
//        void ToggleCheckState(const wxVirtualItemID &rID,
//                              bool bRecursive = false, bool bSendEvent = true); ///< \brief toggle check state of 1 or more item (undetermined becomes checked)

        //focused state
//        wxVirtualItemID GetFocusedItem(void) const;                             ///< \brief get the focused item
//        void SetFocusedItem(const wxVirtualItemID &rID, bool bSendEvent = true);///< \brief set the focused item
//        int GetFocusedColumn(void) const;                                       ///< \brief get focused column ID
//        void SetFocusedColumn(int iCol, bool bSendEvent = true);                ///< \brief set the focused column ID

        //enabled / disabled state
//        bool IsItemEnabled(const wxVirtualItemID &rID) const;                   ///< \brief check if an item is enabled
//        bool IsItemDisabled(const wxVirtualItemID &rID) const;                  ///< \brief check if an item is disabled
//        void EnableItem(const wxVirtualItemID &rID, bool bEnable = true,
//                        bool bRecursive = false);                               ///< \brief enable an item
//        void DisableItem(const wxVirtualItemID &rID, bool bRecursive = false);  ///< \brief disable an item
//        wxVirtualItemID GetFirstEnabledItem(bool bEnabled = true) const;        ///< \brief get first enabled item
//        wxVirtualItemID GetNextEnabledItem(const wxVirtualItemID &rID,
//                                           bool bEnabled = true) const;         ///< \brief get next enabled item
//        wxVirtualItemID GetPrevEnabledItem(const wxVirtualItemID &rID,
//                                           bool bEnabled = true) const;         ///< \brief get previous enabled item
//        size_t          GetAllEnabledItems(wxVirtualItemIDs &vIDs,
//                                           bool bEnabled = true) const;         ///< \brief get all enabled items
//        wxVirtualItemID GetFirstDisabledItem(void) const;                       ///< \brief get first disabled item
//        wxVirtualItemID GetNextDisabledItem(const wxVirtualItemID &rID) const;  ///< \brief get next disabled item
//        wxVirtualItemID GetPrevDisabledItem(const wxVirtualItemID &rID) const;  ///< \brief get previous disabled item
//        size_t          GetAllDisabledItems(wxVirtualItemIDs &vIDs) const;      ///< \brief get all disabled items

        //activate an item
//        void ActivateItem(const wxVirtualItemID &rID, int iCol,
//                          bool bSendEvent = true);                              ///< \brief activate an item

        //hit test & bounding rect
//        bool HitTest(THitTest &rResult, wxCoord x, wxCoord y,
//                     bool bClientAreaOnly = true);                              ///< \brief performs a hit test
//        bool GetColItemRect(wxRect &rRect,
//                            const wxVirtualItemID &rID,
//                            size_t uiColIndex,
//                            bool bClientCoordinates = true);                    ///< \brief get the rectangle of an item
//        bool GetItemRect(wxRect &rRect,
//                         const wxVirtualItemID &rID,
//                         bool bClientCoordinates = true);                       ///< \brief get the rectangle of an item

        //visibility
//        bool IsItemVisible(const wxVirtualItemID &rID, size_t uiCol = 0,
//                           bool bCheckCol = true);                              ///< \brief check if the item is visible
//        bool MakeItemVisible(const wxVirtualItemID &rID, size_t uiCol,
//                             EVisibilityFlags eFlag = WX_VISIBLE_DONT_CARE,
//                             bool bSelect = true);                              ///< \brief make the item visible (without expanding)
//        bool EnsureItemVisible(const wxVirtualItemID &rID, size_t uiCol,
//                               EVisibilityFlags eFlag = WX_VISIBLE_DONT_CARE,
//                               bool bSelect = true);                            ///< \brief ensure the item is visible (with expanding when needed)
//        wxVirtualItemID  GetFirstVisibleItem(void) const;                       ///< \brief get the 1st visible item
//        wxVirtualItemID  GetNextVisibleItem(const wxVirtualItemID &rID) const;  ///< \brief get the next visible item
//        wxVirtualItemID  GetPrevVisibleItem(const wxVirtualItemID &rID) const;  ///< \brief get the previous visible item
//        size_t           GetAllVisibleItems(wxVirtualItemIDs &vIDs) const;      ///< \brief get all visible items

        //get / set value
//        wxVariant GetItemData(const wxVirtualItemID &rID, int iCol,
//                              EDataType eType = WX_ITEM_MAIN_DATA) const;       ///< \brief get item data
//        bool      SetItemData(const wxVirtualItemID &rID, int iCol,
//                              wxVariant vValue,
//                              bool bSendEvent = true,
//                              EDataType eType = WX_ITEM_MAIN_DATA);             ///< \brief set item data
//
//        //item edition
//        bool            StartItemEdition(const wxVirtualItemID &rID, int iCol); ///< \brief start the edition of an item
//        wxVirtualItemID GetEditedItem(void) const;                              ///< \brief get the currently edited item
//
//        //graphic attributes
//        wxVirtualDataViewItemAttr GetItemAttribute(const wxVirtualItemID &rID,
//                                                   int iCol,
//                                            bool bMergeWithColAttrib = true);   ///< \brief get the item graphic attributes
//
//        //searching
//        wxVirtualItemID FindFirst(const wxVariant &rvValue,
//                                  bool bFullMatch, bool bCaseSensitive,
//                                  bool bRegEx,
//                                  bool bSelect = true,
//                                  bool bMakeVisible = true, int iCol = -1);     ///< \brief find the 1st item matching a criteria
//        wxVirtualItemID FindNext(const wxVirtualItemID &rID,
//                                 const wxVariant &rvValue,
//                                 bool bFullMatch, bool bCaseSensitive,
//                                 bool bRegEx,
//                                 bool bSelect = true,
//                                 bool bMakeVisible = true, int iCol = -1);      ///< \brief find the next item matching a criteria
//        wxVirtualItemID FindPrev(const wxVirtualItemID &rID,
//                                 const wxVariant &rvValue,
//                                 bool bFullMatch, bool bCaseSensitive,
//                                 bool bRegEx,
//                                 bool bSelect = true,
//                                 bool bMakeVisible = true, int iCol = -1);      ///< \brief find the previous item matching a criteria
//

    protected:
        //data


    private:
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataViewListCtrl);
};

#endif // _WX_VLBOX_H_

