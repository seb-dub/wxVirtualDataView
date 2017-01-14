/**********************************************************************/
/** FILE    : VirtualDataViewFiltersList.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a collection of filters                                **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_FILTERS_LIST_H_
#define _VIRTUAL_DATA_VIEW_FILTERS_LIST_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/VirtualDataView/VirtualItemID.h>
#include <vector>

//forward declarations
class WXDLLIMPEXP_BASE  wxVariant;
class WXDLLIMPEXP_VDV   wxVirtualDataViewFilter;
class WXDLLIMPEXP_VDV   wxVirtualIDataModel;


/** \class wxVirtualDataViewFiltersList : a filter for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewFiltersList
{
    public:
        //types
        /// \struct TFilter : the filtering information for a field
        struct TFilter
        {
            size_t                      m_uiField;                                  ///< \brief the filtered field
            wxVirtualDataViewFilter*    m_pFilter;                                  ///< \brief the filter for this field
        };
        typedef std::vector<TFilter>  TFilters;                                     ///< \brief array of filters

        //constructors & destructor
        wxVirtualDataViewFiltersList(void);                                         ///< \brief constructor
        ~wxVirtualDataViewFiltersList(void);                                        ///< \brief destructor

        //list management
        bool                        ClearFilters(void);                             ///< \brief clear all filter
        bool                        PushFilter(size_t uiField,
                                               wxVirtualDataViewFilter *pFilter);   ///< \brief push filter
        bool                        PopFilter(size_t uiField);                      ///< \brief pop filter
        bool                        PopFilter(void);                                ///< \brief pop the last filter
        bool                        IsFiltering(size_t uiField) const;              ///< \brief check if a field is filtering
        bool                        IsFiltering(void) const;                        ///< \brief check if at least one field is filtering
        wxVirtualDataViewFilter*    GetFilter(size_t uiField) const;                ///< \brief get the filter for a field
        TFilters&                   Filters(void);                                  ///< \brief get all the filters
        const TFilters&             Filters(void) const;                            ///< \brief get all the filters - const version

        //test
        bool    AcceptValue(const wxVariant& rvValue) const;                        ///< \brief accept or reject a value
        bool    AcceptValue(const wxVariant& rvValue, size_t uiField) const;        ///< \brief accept or reject a value
        bool    IsAccepted(const wxVirtualItemID &rID,
                           wxVirtualIDataModel *pDataModel);                        ///< \brief accept or reject an item from a wxVirtualIDataModel
        bool    IsAcceptedNonRecursive(const wxVirtualItemID &rID,
                                       wxVirtualIDataModel *pDataModel);            ///< \brief accept or reject an item from a wxVirtualIDataModel
        void    FilterItems(wxVirtualItemIDs &vFilteredIDs,
                            const wxVirtualItemIDs &vUnfilteredIDs,
                            wxVirtualIDataModel *pDataModel);                       ///< \brief filter a list of items from a wxVirtualIDataModel

    protected:
        //data
        TFilters        m_vFilters;                                                 ///< \brief list of filters
};

#endif

