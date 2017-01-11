/**********************************************************************/
/** FILE    : ThemeHandleWin32.h                                     **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a Win32 implementation of wxVirtualHeaderIRenderer      **/
/**********************************************************************/

#ifndef _THEME_HANDLE_WIN32_H_
#define _THEME_HANDLE_WIN32_H_

#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>

#ifdef __WXMSW__
    #if wxUSE_UXTHEME
        #define wxUSE_MS_WIN_THEME      1
        #include "wx/msw/uxtheme.h"
    #endif // wxUSE_UXTHEME
#endif // __WXMSW__

#if wxUSE_MS_WIN_THEME != 0

/** \class ThemeHandleWin32 a class for wrapping the theme handle on Win32
  *
  * a class for wrapping the theme handle on Win32. The reason that wxUXThemHandle is not
  * used is because we need to reset the theme
  */
class wxThemeHandleWin32
{
    public:
        //constructor & destructors
        wxThemeHandleWin32(const wxWindow *pWin, const wchar_t *classes);   ///< \brief constructor
        ~wxThemeHandleWin32(void);                                          ///< \brief destructor

        //access to the them
        operator HTHEME() const {return m_hTheme;}                          ///< \brief get access to the them

        //reopen
        bool Reopen(const wxWindow *pWin, const wchar_t *classes);          ///< \brief reopen the theme

    private:
        //data
        HTHEME      m_hTheme;                                               ///< \brief the theme data
};

#endif // wxUSE_MS_WIN_THEME
#endif // _THEME_HANDLE_WIN32_H_
