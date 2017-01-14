/**********************************************************************/
/** FILE    : VirtualDataViewFilterCtrl.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a list view for showing values to filter               **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_VIEW_FILTER_CTRL_H_
#define _WX_VIRTUAL_VIEW_FILTER_CTRL_H_

#include <wx/VirtualDataView/DataView/VirtualDataViewListCtrl.h>

class WXDLLIMPEXP_VDV wxVirtualDataFilterModel;
extern const char wxVirtualDataViewFilterCtrlNameStr[];

/** \class wxVirtualDataViewFilterCtrl : a virtual list view for showing the list of values contained in
  * a column of a wxVirtualDataViewCtrl.
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFilterCtrl : public wxVirtualDataViewListCtrl
{
    public:

        //constructors & destructor
        wxVirtualDataViewFilterCtrl(void);                                      ///< \brief default constructor. Create() must be called manually
        wxVirtualDataViewFilterCtrl(wxWindow *pParent,
                             wxWindowID id = wxID_ANY,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize,
                             long lStyle = 0,
                             const wxValidator& validator = wxDefaultValidator,
                           wxString sName = wxVirtualDataViewFilterCtrlNameStr);  ///< \brief constructor
        virtual ~wxVirtualDataViewFilterCtrl(void);                             ///< \brief destructor

        //initialization
        virtual bool Create(wxWindow *pParent, wxWindowID id,
                            const wxPoint& pos,  const wxSize& size,
                            long lStyle,
                            const wxValidator& validator, wxString sName,
                            wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR,
                            wxVirtualIStateModel *pStateModel = WX_VDV_NULL_PTR,
                    wxVirtualIModelRenderer *pModelRenderer = WX_VDV_NULL_PTR); ///< \brief really creates the control and sets the initial number of items in it


        //data model
        virtual void            SetDataModel(wxVirtualIDataModel *pModel);      ///< \brief set the current data model. Current proxies will be kept
        wxVirtualDataFilterModel* GetFilterDataModel(void);                     ///< \brief get the filter data model

        //fill filter
        virtual void            InitFilter(wxVirtualIDataModel *pSrcModel,
                                           size_t uiField,
                                           wxVirtualIStateModel *pStateModel);  ///< \brief fill the filter with data from a source model

        //array of values
        wxVector<wxVariant>&        Values(void);                               ///< \brief direct access to the array of values
        const wxVector<wxVariant>&  Values(void) const;                         ///< \brief direct access to the array of values - const version

        //display string of the filter
        virtual wxString GetFilterDisplayString(void) const;                    ///< \brief return the filter string to display

        //special items
        virtual void CheckSpecialItems(ECheckState eState);                     ///< \brief check/uncheck special items
        virtual void SetHasBlanks(bool bHasBlanks);                             ///< \brief set if <blanks> item is present
        virtual void SetHasNonBlanks(bool bHasNonBlanks);                       ///< \brief set if <non-blanks> item is present
        virtual bool HasBlanks(void) const;                                     ///< \brief check if the <blanks> item is present
        virtual bool HasNonBlanks(void) const;                                  ///< \brief check if the <non-blanks> item is present

        //save / restore current state
        virtual void SaveState(void);                                           ///< \brief save the current state
        virtual void RestoreState(void);                                        ///< \brief restore the saved state

    protected:
        //data
        wxVirtualItemIDs    m_vSavedCheckedItems;                               ///< \brief list of checked items

    private:
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataViewFilterCtrl);
};

#endif // _WX_VLBOX_H_

