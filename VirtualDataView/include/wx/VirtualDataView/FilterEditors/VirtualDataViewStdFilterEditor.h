/**********************************************************************/
/** FILE    : VirtualDataViewStdFilterEditor.h                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : standard impl of wxVirtualDataViewIFilterEditor        **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_STD_FILTER_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_STD_FILTER_EDITOR_H_

#include <wx/VirtualDataView/FilterEditors/VirtualDataViewIFilterEditor.h>

class WXDLLIMPEXP_VDV   wxVirtualDataViewFilterComboCtrl;
class WXDLLIMPEXP_VDV   wxComboboxState;

/** \class wxVirtualDataViewStdFilterEditor : standard implementation of wxVirtualDataViewIFilterEditor.
  * It uses a custom combobox, with a search entry and multiple selection possible
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewStdFilterEditor : public wxVirtualDataViewIFilterEditor
{
    public:
        //constructors & destructor
        wxVirtualDataViewStdFilterEditor(void);                                     ///< \brief constructor
        virtual ~wxVirtualDataViewStdFilterEditor(void);                            ///< \brief destructor

        //interface for UI
        virtual wxVirtualDataViewIFilterEditor* Clone(void) const;                  ///< \brief clone the filter editor
        virtual wxSize    GetSize(wxWindow *pWin);                                  ///< \brief get the size of the filter editor
        virtual void      Draw(wxWindow *pWin, wxDC &rDC, wxRect &rRect);           ///< \brief draw the filter editor
        virtual bool      StartFilter(wxWindow *pParent, wxRect &rRect);            ///< \brief activate the filter editor
        virtual bool      CanFinishFilterEdition(void);                             ///< \brief return true if filter edition can be finished
        virtual bool      FinishFilterEdition(bool bCancelled = false);             ///< \brief end filter edition
        virtual wxWindow* GetEditorWindow(void);                                    ///< \brief get the editor window
        virtual void      ClearFilter(void);                                        ///< \brief clear all items in the filter editor
        virtual void      InitFilter(wxVirtualIDataModel *pSrcModel,
                                     size_t uiField,
                                     wxVirtualIStateModel *pStateModel);            ///< \brief initialize the filter

    protected:
        //data
        wxVirtualDataViewFilterComboCtrl*   m_pComboBox;                            ///< \brief the combobox
        wxComboboxState*                    m_pComboBoxState;                       ///< \brief combobox state

        //methods
        void Copy(const wxVirtualDataViewStdFilterEditor &rhs);                     ///< \brief copy object
        void CreateComboBox(wxWindow* pParent, wxRect &rRect);                      ///< \brief create the combo box
        void SetComboBoxSize(wxRect &rRect);                                        ///< \brief set the combo box size

        //override from base
        void CaptureToBitmap(wxWindow* pEditor);                                    ///< \brief draw the editor to a bitmap

        //event handlers
        void OnFilterChanged(wxCommandEvent &rEvent);                               ///< \brief combo-box contents changed
        void OnFilterInit(wxCommandEvent &rEvent);                                  ///< \brief drop-down event for the combo-box
};

#endif

