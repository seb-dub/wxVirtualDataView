/**********************************************************************/
/** FILE    : VirtualDataViewCellMapper.h                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : model coordinates/view coordinates convertor           **/
/**********************************************************************/

#ifndef _VIRTUAL_DATA_VIEW_CELL_MAPPER_H_
#define _VIRTUAL_DATA_VIEW_CELL_MAPPER_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

class WXDLLIMPEXP_VDV wxVirtualITableTreeDataModel;

/** \class wxVirtualDataViewCellMapper: it converts a model coordinate to a view coordinate.
  * The coordinate can be a row or a column index.
  * This allow to support easily :
  *     - row/column reordering
  *     - row/column filtering (by returning s_NotFound in GetModelCoord)
  *     - row/column duplication (by returning the same value in GetModelCoord for 2 different view coord)
  *     - frozen row/column (by returning the frozen row/column model index in GetModelCoord(0))
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewCellMapper
{
    public:
        //constants
        static size_t s_NotFound;                                       ///< \brief special value for invisible row/column

        //constructors & destructor
        wxVirtualDataViewCellMapper(void);                              ///< \brief constructor
        virtual ~wxVirtualDataViewCellMapper(void);                     ///< \brief destructor

        //interface
        virtual size_t GetModelCoord(size_t uiViewCoord)         = 0;   ///< \brief convert a view coord to a model coord
        virtual size_t GetViewCoord(size_t uiModelCoord)         = 0;   ///< \brief convert a model coord to a view coord
        virtual size_t GetAmountOfViewCoords(void) const         = 0;   ///< \brief get the amount of visible coordinates
        virtual void OnModelChanged(wxVirtualITableTreeDataModel *pModel) = 0;   ///< \brief the model has been updated
};

#endif

