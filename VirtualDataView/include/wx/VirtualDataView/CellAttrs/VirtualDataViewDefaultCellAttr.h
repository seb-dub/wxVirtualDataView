/**********************************************************************/
/** FILE    : VirtualDataViewDefaultCellAttr.h                       **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a default data view cell attribute                     **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_DEFAULT_CELL_ATTR_H_
#define _VIRTUAL_DATA_VIEW_DEFAULT_CELL_ATTR_H_

#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>

//------------------ DECLARATION ------------------------------------//
/** \class wxVirtualDataViewDefaultCellAttr : a default attribute
  * It is of string type, use a text renderer, has no editors, and use
  * system default attributes
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewDefaultCellAttr : public wxVirtualDataViewCellAttr
{
    public:

        //constructors & destructor
        wxVirtualDataViewDefaultCellAttr(void);     ///< \brief default constructor
        ~wxVirtualDataViewDefaultCellAttr(void);    ///< \brief destructor
};

#endif

