/**********************************************************************/
/** FILE    : VirtualHeaderRendererGeneric.h                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a generic implementation of wxVirtualHeaderIRenderer   **/
/**********************************************************************/

#ifndef _VIRTUAL_HEADER_RENDERER_GENERIC_H_
#define _VIRTUAL_HEADER_RENDERER_GENERIC_H_

#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>

//------------------------- DECLARATION -----------------------------//
/** \class wxVirtualHeaderRendererGeneric : a generic implementation of wxVirtualHeaderIRenderer
  */
class WXDLLIMPEXP_VDV wxVirtualHeaderRendererGeneric : public wxVirtualHeaderIRenderer
{
    public:
        //constructors & destructor
        wxVirtualHeaderRendererGeneric(wxWindow *pWin);                         ///< \brief constructor
        virtual ~wxVirtualHeaderRendererGeneric(void);                          ///< \brief destructor

        //interface
        virtual bool IsOK(void) const;                                          ///< \brief check if the engine is usable
        virtual wxSize GetHeaderButtonSize(const THeaderData &rData);           ///< \brief get the header button size
        virtual void DrawHeader(wxDC &rDC, const wxRect &r,
                                const THeaderData &rData);                      ///< \brief draw the header
        virtual void DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                       const THeaderData &rData);               ///< \brief draw the header content, without background
        virtual void DrawHeaderBackground(wxDC &rDC, const wxRect &r);          ///< \brief draw the header background only

        //static method, for falling back in case
        //other renderers fail
        static wxSize GetHeaderButtonSizeGeneric(wxWindow *pWin,
                                                 const THeaderData &rData);     ///< \brief get the header button size
        static void DrawBackgroundGeneric(wxWindow *pWin, wxDC &rDC,
                                          const wxRect &r);                     ///< \brief fallback for background
        static void DrawContentGeneric(wxWindow *pWin, wxDC &rDC,
                                       const wxRect &r,
                                       const THeaderData &rData);               ///< \brief fallback for content
        static void DrawHeaderGeneric(wxWindow *pWin, wxDC &rDC,
                                      const wxRect &r,
                                      const THeaderData &rData);                ///< \brief fallback for background + content
};

#endif // _VIRTUAL_HEADER_RENDERER_GENERIC_H_
