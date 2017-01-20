/**********************************************************************/
/** FILE    : VirtualDataViewRendererCompat.h                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxVirtualDataViewRenderer wrapping a wxDVCRenderer   **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_RENDERER_COMPAT_H_
#define _VIRTUAL_DATA_VIEW_RENDERER_COMPAT_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
    class WXDLLIMPEXP_ADV wxDataViewCustomRenderer;

/** \class wxVirtualDataViewRendererCompat : a renderer which uses internally a wxDataViewRenderer
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewRendererCompat: public wxVirtualDataViewRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewRendererCompat(
                        wxDataViewCustomRenderer* pRenderer = WX_VDV_NULL_PTR);     ///< \brief default constructor
        virtual ~wxVirtualDataViewRendererCompat(void);                             ///< \brief destructor

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

        //alignment
        void SetAlignment(EHAlign eHAlign, EVAlign eVAlign);                        ///< \brief set alignment
        void GetAlignment(EHAlign &eHAlign, EVAlign &eVAlign) const;                ///< \brief get alignment

        //ellipsization
        void SetEllipsizeMode(EEllipsizeMode eMode);                                ///< \brief set ellipsization mode
        EEllipsizeMode GetEllipsizeMode(void) const;                                ///< \brief get ellipsization mode

    protected:
        //data
        wxDataViewCustomRenderer*   m_pRenderer;                                    ///< \brief the renderer

};

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
#endif

