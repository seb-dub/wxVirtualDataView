/**********************************************************************/
/** FILE    : VirtualListModelRenderer.h                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : list implementation of wxVirtualIModelRenderer         **/
/**********************************************************************/


#ifndef WX_VIRTUAL_LIST_MODEL_RENDERERER_H_
#define WX_VIRTUAL_LIST_MODEL_RENDERERER_H_

#include <wx/VirtualDataView/ModelRenderer/VirtualTreeModelRenderer.h>

/** \class wxVirtualListModelRenderer : an implementation of wxVirtualIModelRenderer
  * Optimized for list rendering
  */
class WXDLLIMPEXP_VDV wxVirtualListModelRenderer : public wxVirtualTreeModelRenderer
{
    public:
        //constructors & destructor
        wxVirtualListModelRenderer(wxVirtualDataViewBase* pClient,
                                   wxVirtualDataViewCtrl* pScroller);   ///< \brief constructor
        virtual ~wxVirtualListModelRenderer(void);                      ///< \brief destructor

        //interface - event handling
        virtual bool PaintModel(wxDC &rDC);                             ///< \brief paint the model
    protected:
        //sizing & scrolling
        virtual wxVirtualTreePath ConvertRowToTreePath(int iLine) const;///< \brief convert a line number to a tree path

};

#endif

