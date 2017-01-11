/**********************************************************************/
/** FILE    : VirtualDataViewColourRenderer.h                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view colour renderer                            **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_COLOUR_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_COLOUR_RENDERER_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>

/** \class wxVirtualDataViewColourRenderer : a colour renderer for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewColourRenderer: public wxVirtualDataViewRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewColourRenderer(void);                                      ///< \brief default constructor
        virtual ~wxVirtualDataViewColourRenderer(void);                             ///< \brief destructor

        //interface implementation
        virtual wxVirtualDataViewRenderer* Clone(void) const;                       ///< \brief clone the renderer
        virtual void Render(wxWindow *pWindow, const wxRect &rRect,
                            wxDC &rDC,
                            wxVirtualDataViewItemAttr *pAttr,
                            const wxVirtualDataViewItemState &rState,
                            const wxVariant& vValue);                               ///< \brief render the item
        virtual wxSize GetSize(wxWindow *pWindow,
                               wxVirtualDataViewItemAttr *pAttr,
                               const wxVirtualDataViewItemState &rState,
                               const wxVariant& vValue) const;                      ///< \brief return the size of the item
};

#endif

