/**********************************************************************/
/** FILE    : VirtualDataViewTextRenderer.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view text renderer                              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_TEXT_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_TEXT_RENDERER_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>

/** \class wxVirtualDataViewTextRenderer : a basic text renderer for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewTextRenderer: public wxVirtualDataViewRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewTextRenderer(EEllipsizeMode eMode = WX_ELLIPSIZE_NONE,
                                      EHAlign eHAlign = WX_HORIZ_ALIGN_LEFT,
                                      EVAlign eVAlign = WX_VERT_ALIGN_CENTRE);      ///< \brief default constructor
        virtual ~wxVirtualDataViewTextRenderer(void);                               ///< \brief destructor

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
        EEllipsizeMode          m_eEllipsizeMode;                                   ///< \brief ellipsization mode
        EHAlign                 m_eHorizontalAlignment;                             ///< \brief horizontal alignment
        EVAlign                 m_eVerticalAlignment;                               ///< \brief vertical alignment

};

#endif

