/**********************************************************************/
/** FILE    : VirtualFilteringDataModel.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : proxy data model for filtering items                   **/
/**********************************************************************/


#ifndef WX_VIRTUAL_FILTERING_DATA_MODEL_H_
#define WX_VIRTUAL_FILTERING_DATA_MODEL_H_

#include <wx/VirtualDataView/Models/VirtualIArrayProxyDataModel.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewFilter;

/** \class wxVirtualFilteringDataModel : proxy data model which provides filtering.
  */
class WXDLLIMPEXP_VDV wxVirtualFilteringDataModel : public wxVirtualIArrayProxyDataModel
{
    public:
        //constructors & destructor
        wxVirtualFilteringDataModel(void);                                                  ///< \brief default constructor
        virtual ~wxVirtualFilteringDataModel(void);                                         ///< \brief destructor

        /// \struct TFilter : the filtering information for a field
        struct TFilter
        {
            size_t                      m_uiField;                                          ///< \brief the filtered field
            wxVirtualDataViewFilter*    m_pFilter;                                          ///< \brief the filter for this field
        };

        typedef std::vector<TFilter>  TFilters;                                             ///< \brief array of filters

        //filters
        void                        ClearFilters(void);                                     ///< \brief clear all filter
        void                        PushFilter(size_t uiField,
                                               wxVirtualDataViewFilter *pFilter);           ///< \brief push filter
        void                        PopFilter(size_t uiField);                              ///< \brief pop filter
        void                        PopFilter(void);                                        ///< \brief pop the last filter
        bool                        IsFiltering(size_t uiField) const;                      ///< \brief check if a field is filtering
        bool                        IsFiltering(void) const;                                ///< \brief check if at least one field is filtering
        wxVirtualDataViewFilter*    GetFilter(size_t uiField) const;                        ///< \brief get the filter for a field
        TFilters&                   Filters(void);                                          ///< \brief get all the filters
        const TFilters&             Filters(void) const;                                    ///< \brief get all the filters - const version

    protected:
        //data
        TFilters                    m_vFilters;                                             ///< \brief list of filters
        wxVirtualItemIDs            m_vTempArray;                                           ///< \brief temp array

        //interface
        virtual void    DoGetChildren(wxVirtualItemIDs &vChildren,
                                      const wxVirtualItemID &rIDParent);                    ///< \brief get all children

        //apply filter
        bool IsAccepted(const wxVirtualItemID &rID);                                        ///< \brief check if an item is accepted
        void ApplyFilters(wxVirtualItemIDs &vFiltered,
                          const wxVirtualItemIDs &vUnfiltered);                             ///< \brief apply filters to a list of IDs


};

#endif

