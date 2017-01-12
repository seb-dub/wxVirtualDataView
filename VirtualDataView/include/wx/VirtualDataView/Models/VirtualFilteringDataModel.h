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

        //interface reimplementation from wxVirtualIDataModel
        //filtering - get all values as string
        virtual void GetAllValues(wxArrayString &rvStrings, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of strings

        //filtering - get all values as bool
        virtual void GetAllValues(wxArrayBool &rvBools, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of bool

        //filtering - get all values as signed integers
        virtual void GetAllValues(wxArrayShort &rvShorts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of short
        virtual void GetAllValues(wxArrayInt &rvInts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of int
        virtual void GetAllValues(wxArrayLong &rvLongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of long
        virtual void GetAllValues(wxArrayLongLong &rvLongLongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of longlong

        //filtering - get all values as unsigned integers
        virtual void GetAllValues(wxArrayUShort &rvUShorts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned short
        virtual void GetAllValues(wxArrayUInt &rvUInts, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned int
        virtual void GetAllValues(wxArrayULong &rvULongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned long
        virtual void GetAllValues(wxArrayULongLong &rvULongLongs, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of unsigned longlong

        //filtering - get all values as floats/doubles
        virtual void GetAllValues(wxArrayFloat &rvFloats, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of floats
        virtual void GetAllValues(wxArrayDouble &rvDoubles, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of doubles

        //filtering - get all values as variants
        virtual void GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                  wxVirtualIStateModel *pStateModel);                       ///< \brief get all the values inside an array of variants

    protected:
        //data
        TFilters                    m_vFilters;                                             ///< \brief list of filters
        wxVirtualItemIDs            m_vTempArray;                                           ///< \brief temp array
        bool                        m_bApplyFiltersForGetAllValues;                         ///< \brief if true, current filters are applied when calling GetAllValues

        //interface
        virtual void    DoGetChildren(wxVirtualItemIDs &vChildren,
                                      const wxVirtualItemID &rIDParent);                    ///< \brief get all children

        //apply filter
        bool IsAccepted(const wxVirtualItemID &rID);                                        ///< \brief check if an item is accepted
        bool IsAcceptedNonRecursive(const wxVirtualItemID &rID);                            ///< \brief check if an item is accepted, ignoring filter recursivity
        void ApplyFilters(wxVirtualItemIDs &vFiltered,
                          const wxVirtualItemIDs &vUnfiltered);                             ///< \brief apply filters to a list of IDs

    private:
        //methods
        template<typename TSet, typename TArray, typename T>
        void DoGetAllValues(TArray &rArray, size_t uiField,
                            wxVirtualIStateModel *pStateModel);                             ///< \brief helper method for getting all the values

};

#endif

