/**********************************************************************/
/** FILE    : VirtualDataViewItemState.h                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view item state                                 **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_ITEM_STATE_H_
#define _VIRTUAL_DATA_VIEW_ITEM_STATE_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>

//-------------------------------- DECLARATION -------------------------------------//

/** \class wxVirtualDataViewItemState : defines an item state
  * It is only a storage class. It does not interact directly with the controls or models.
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewItemState
{
    public:

        //constructors & destructor
        wxVirtualDataViewItemState(bool bSelected  = false,
                                   bool bFocused   = false,
                                   bool bEnabled   = true,
                                   bool bExpanded  = false,
                                   int iCheckState = 0);        ///< \brief default constructor
        ~wxVirtualDataViewItemState(void);                      ///< \brief destructor

        //selection state
        bool IsSelected(void) const;                            ///< \brief check if the item is selected
        void Select(bool bSelect = true);                       ///< \brief select/unselect the item
        void UnSelect(void);                                    ///< \brief unselect the item
        void ToggleSelect(void);                                ///< \brief toggle selection state

        //focus state
        bool IsFocused(void) const;                             ///< \brief check if the focus is set on the item
        void SetFocused(bool bHasFocus = true);                 ///< \brief set/unset the focus on the item
        void UnsetFocus(void);                                  ///< \brief unset the focus on the item
        void ToggleFocus(void);                                 ///< \brief toggle the focus on the item

        //enabled state
        bool IsEnabled(void) const;                             ///< \brief return true if the item is enabled
        bool IsDisabled(void) const;                            ///< \brief return true if the item is disabled
        void Enable(bool bEnable = true);                       ///< \brief enable/disable the item
        void Disable(void);                                     ///< \brief disable the item
        void ToggleEnabled(void);                               ///< \brief toggle the enabled state

        //expanded/collapsed
        bool IsCollapsed(void) const;                           ///< \brief return true if the item is collapsed
        bool IsExpanded(void) const;                            ///< \brief return true if the item is expanded
        void Collapse(void);                                    ///< \brief collapse
        void Expand(void);                                      ///< \brief expand
        void ToggleExpandCollapse(void);                        ///< \brief toggle expanded/collapsed state

        //check state
        int GetCheckState(void) const;                          ///< \brief get the check state
        void SetCheckState(int iState);                         ///< \brief set the check state

        //default state
        void SetDefault(void);                                  ///< \brief reset the state to default state
        bool IsDefault(void) const;                             ///< \brief check if the state is the default state

    protected:
        //data
        bool    m_bIsSelected;                                  ///< \brief true if the item is selected, false otherwise
        bool    m_bIsFocused;                                   ///< \brief true if the item is focused, false otherwise
        bool    m_bIsEnabled;                                   ///< \brief true if the item is enabled, false otherwise
        bool    m_bIsExpanded;                                  ///< \brief true if the item is expanded
        int     m_iCheckState;                                  ///< \brief the check state
};

//--------------------- INLINE IMPLEMENTATION -----------------------//
//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Default constructor
  * \param bSelected   [input]: true if the item is selected, false otherwise
  * \param bFocused    [input]: true if the item is focused, false otherwise
  * \param bEnabled    [input]: true it the item is enabled, false otherwise
  * \param bExpanded   [input]: true if the item is expanded
  * \param iCheckState [input]: the checkbox state (see wxVirtualIStateModel::ECheckState)
  */
WX_VDV_INLINE wxVirtualDataViewItemState::wxVirtualDataViewItemState(bool bSelected,
                                                                     bool bFocused,
                                                                     bool bEnabled,
                                                                     bool bExpanded,
                                                                     int iCheckState)
    : m_bIsSelected(bSelected),
      m_bIsFocused(bFocused),
      m_bIsEnabled(bEnabled),
      m_bIsExpanded(bExpanded),
      m_iCheckState(iCheckState)
{

}

/** Destructor
  */
WX_VDV_INLINE wxVirtualDataViewItemState::~wxVirtualDataViewItemState(void)
{

}

//-------------------- SELECTION STATE ------------------------------//
/** Check if the item is selected
  * \return true if the item is selected, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsSelected(void) const
{
    return(m_bIsSelected);
}

/** Select/unselect the item
  * \param bSelect [input]: true for selecting the item, false otherwise
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::Select(bool bSelect)
{
    m_bIsSelected = bSelect;
}

/** Unselect the item
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::UnSelect(void)
{
    m_bIsSelected = false;
}

/** Toggle selection state
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::ToggleSelect(void)
{
    m_bIsSelected = !m_bIsSelected;
}

//----------------------- FOCUS STATE -------------------------------//
/** Check if the focus is set on the item
  * \return true if the item has the focus, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsFocused(void) const
{
    return(m_bIsFocused);
}

/** Set/unset the focus on the item
  * \param bHasFocus [input]: true for setting the focus on the item, false otherwise
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::SetFocused(bool bHasFocus)
{
    m_bIsFocused = bHasFocus;
}

/** Unset the focus on the item
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::UnsetFocus(void)
{
    m_bIsFocused = false;
}

/** Toggle the focus on the item
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::ToggleFocus(void)
{
    m_bIsFocused = !m_bIsFocused;
}

//------------------- ENABLED STATE ---------------------------------//
/** Return true if the item is enabled
  * \return true if the item is enabled
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsEnabled(void) const
{
    return(m_bIsEnabled);
}

/** Return true if the item is disabled
  * \return true if the item is disabled
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsDisabled(void) const
{
    return(!m_bIsEnabled);
}

/** Enable/disable the item
  * \param bEnable [input]: true of enabling the item, false for disabling it
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::Enable(bool bEnable)
{
    m_bIsEnabled = bEnable;
}

/** Disable the item
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::Disable(void)
{
    m_bIsEnabled = false;
}

/** Toggle the enabled state
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::ToggleEnabled(void)
{
    m_bIsEnabled = !m_bIsEnabled;
}

//----------------- EXPANDED / COLLAPSED ----------------------------//
/** Return true if the item is collapsed
  * \return true if the item is collapsed
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsCollapsed(void) const
{
    return(!m_bIsExpanded);
}

/** Return true if the item is expanded
  * \return true if the item is expanded
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsExpanded(void) const
{
    return(m_bIsExpanded);
}

/** Collapse the item
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::Collapse(void)
{
    m_bIsExpanded = false;
}

/** Expand the item
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::Expand(void)
{
    m_bIsExpanded = true;
}

/** Toggle expanded/collapsed state
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::ToggleExpandCollapse(void)
{
    m_bIsExpanded = !m_bIsExpanded;
}

//---------------------- CHECK STATE --------------------------------//
/** Get the check state
  * \return the check state (see wxVirtualIStateModel::ECheckState)
  */
WX_VDV_INLINE int wxVirtualDataViewItemState::GetCheckState(void) const
{
    return(m_iCheckState);
}

/** Set the check state
  * \param iState [input]: the new check state (see wxVirtualIStateModel::ECheckState)
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::SetCheckState(int iState)
{
    m_iCheckState = iState;
}

//----------------------- DEFAULT STATE -----------------------------//
/** Reset the state to default state
  */
WX_VDV_INLINE void wxVirtualDataViewItemState::SetDefault(void)
{
    m_iCheckState = 0;
    m_bIsExpanded = false;
    m_bIsFocused  = false;
    m_bIsEnabled  = true;
    m_bIsSelected = false;
}

/** Check if the state is the default state
  * \return true if the state is the default state, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemState::IsDefault(void) const
{
    if (m_bIsExpanded) return(false);
    if (m_bIsFocused) return(false);
    if (m_bIsSelected) return(false);
    if (!m_bIsEnabled) return(false);
    if (m_iCheckState != 0) return(false);
    return(true);
}

#endif

