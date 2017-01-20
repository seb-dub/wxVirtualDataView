/**********************************************************************/
/** FILE    : DataViewModelCompat.h                                  **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : an implementation of wxDataViewModel using a wxVDV mod **/
/**********************************************************************/


#ifndef _DATA_VIEW_MODEL_COMPAT_H_
#define _DATA_VIEW_MODEL_COMPAT_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

class WXDLLIMPEXP_VDV wxVirtualIDataModel;

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
#include <wx/dataview.h>

//------------------------- DECLARATION -----------------------------//
/** \class wxDataViewModelCompat : a compatibility class for wxDataViewCtrl.
  * This is a wxDataViewModel implementation, using internally a wxVirtualIDataModel
  */
class WXDLLIMPEXP_VDV wxDataViewModelCompat : public wxDataViewModel
{
    public:
        //constructors & destructor
        wxDataViewModelCompat(wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR);   ///< \brief constructor
        virtual ~wxDataViewModelCompat(void);                                       ///< \brief destructor

        //get / set internal model
        wxVirtualIDataModel* GetModel(void);                                        ///< \brief get the internal data model
        void SetModel(wxVirtualIDataModel *pDataModel);                             ///< \brief set the internal data model

        //wxDataViewModel interface implementation
        virtual unsigned int GetColumnCount(void) const;                            ///< \brief get amount of columns
        virtual wxString GetColumnType(unsigned int uiCol) const;                   ///< \brief get the type of the column
        virtual void GetValue(wxVariant &rvVariant,
                              const wxDataViewItem &rID, unsigned int uiCol) const; ///< \brief get the value of an item
        virtual bool SetValue(const wxVariant &rvVariant, const wxDataViewItem &rID,
                              unsigned int uiCol);                                  ///< \brief set the item value
        virtual wxDataViewItem GetParent(const wxDataViewItem &rID) const;          ///< \brief get the parent of the item
        virtual bool IsContainer(const wxDataViewItem &rID) const;                  ///< \brief check if the item is a container
        virtual unsigned int GetChildren(const wxDataViewItem &rID,
                                         wxDataViewItemArray &vChildren) const;     ///< \brief get the list of children

    protected:
        //data
        wxVirtualIDataModel*        m_pDataModel;                                   ///< \brief the data model
};

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
#endif

