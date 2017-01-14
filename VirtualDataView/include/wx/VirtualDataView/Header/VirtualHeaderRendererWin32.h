/**********************************************************************/
/** FILE    : VirtualHeaderRendererWin32.h                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a Win32 implementation of wxVirtualHeaderIRenderer     **/
/**********************************************************************/

#ifndef _VIRTUAL_HEADER_RENDERER_WIN32_H_
#define _VIRTUAL_HEADER_RENDERER_WIN32_H_

#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>
#include <wx/VirtualDataView/Header/ThemeHandleWin32.h>

#if wxUSE_MS_WIN_THEME != 0
//------------------------- DECLARATION -----------------------------//
/** \class wxVirtualHeaderRendererWin32 : a Win32 implementation of wxVirtualHeaderIRenderer
  */
class WXDLLIMPEXP_VDV wxVirtualHeaderRendererWin32 : public wxVirtualHeaderIRenderer
{
    public:
        //constructors & destructor
        wxVirtualHeaderRendererWin32(wxWindow *pWin);                           ///< \brief constructor
        virtual ~wxVirtualHeaderRendererWin32(void);                            ///< \brief destructor

        //interface
        virtual bool IsOK(void) const;                                          ///< \brief check if the engine is usable
        virtual wxSize GetHeaderButtonSize(const THeaderData &rData);           ///< \brief get the header button height
        virtual void DrawHeader(wxDC &rDC, const wxRect &r,
                                const THeaderData &rData);                      ///< \brief draw the header
        virtual void DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                       const THeaderData &rData);               ///< \brief draw the header content, without background
        virtual void DrawHeaderBackground(wxDC &rDC, const wxRect &r);          ///< \brief draw the header background only

        virtual void ResetTheme(void);                                          ///< \brief reset theme data

        //settings
        void GetBackColours(wxColour &rC1, wxColour &rC2) const;                ///< \brief get background colours (assumed gradient)


    private:
        //data
        wxUxThemeEngine*    m_pThemeEngine;                                     ///< \brief the theme engine
        wxThemeHandleWin32  m_HeaderTheme;                                      ///< \brief the specific theme for the header window

        wxDECLARE_NO_COPY_CLASS(wxVirtualHeaderRendererWin32);
};

#endif // wxUSE_MS_WIN_THEME
#endif // _VIRTUAL_HEADER_RENDERER_GENERIC_H_
