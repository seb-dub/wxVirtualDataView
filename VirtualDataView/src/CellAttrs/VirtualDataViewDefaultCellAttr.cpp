/**********************************************************************/
/** FILE    : VirtualDataViewDefaultCellAttr.cpp                     **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a default data view cell attribute                     **/
/**********************************************************************/

#include <wx/VirtualDataView/CellAttrs/VirtualDataViewDefaultCellAttr.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewTextRenderer.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>

//---------------------- CONSTRUCTORS & DESTRUCTOR ------------------//
/** Default constructor
  */
wxVirtualDataViewDefaultCellAttr::wxVirtualDataViewDefaultCellAttr(void)
    : wxVirtualDataViewCellAttr("string",
                                new wxVirtualDataViewTextRenderer,
                                WX_VDV_NULL_PTR,
                                new wxVirtualDataViewItemAttr(wxVirtualDataViewItemAttr::GetDefault()))
{
}

/** Destructor
  */
wxVirtualDataViewDefaultCellAttr::~wxVirtualDataViewDefaultCellAttr(void)
{
}

