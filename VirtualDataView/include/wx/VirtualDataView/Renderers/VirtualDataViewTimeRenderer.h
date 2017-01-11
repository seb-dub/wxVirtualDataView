/**********************************************************************/
/** FILE    : VirtualDataViewTimeRenderer.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view time renderer                              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_TIME_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_TIME_RENDERER_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewTextRenderer.h>

/** \class wxVirtualDataViewTimeRenderer : a time renderer for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewTimeRenderer: public wxVirtualDataViewTextRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewTimeRenderer(bool bISOFormat = true,
                                      EEllipsizeMode eMode = WX_ELLIPSIZE_NONE,
                                      EHAlign eHAlign = WX_HORIZ_ALIGN_LEFT,
                                      EVAlign eVAlign = WX_VERT_ALIGN_CENTRE);      ///< \brief default constructor
        virtual ~wxVirtualDataViewTimeRenderer(void);                               ///< \brief destructor

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

        //options
        bool UseISOFormat(void) const;                                              ///< \brief true if ISO format is used (YYYY-MM-DD)
        void SetUseISOFormat(bool bISO = true);                                     ///< \brief use ISO format (YYYY-MM-DD) or local format
        void SetUseLocalFormat(void);                                               ///< \brief use local format

    protected:
        //data
        bool    m_bISOFormat;                                                       ///< \brief true if ISO format is used

        //methods
        wxString ConvertToString(const wxVariant &rvValue) const;                   ///< \brief convert the variant to string
};

#endif

