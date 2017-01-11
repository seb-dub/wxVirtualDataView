/**********************************************************************/
/** FILE    : ThemeHandleWin32.cpp                                   **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a Win32 implementation of wxVirtualHeaderIRenderer      **/
/**********************************************************************/

#include <wx/VirtualDataView/Header/ThemeHandleWin32.h>

#if wxUSE_MS_WIN_THEME != 0
//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Constructor
  * \param pWin [input]: the window using the them
  * \param classes [input]: name of the theme to open
  */
wxThemeHandleWin32::wxThemeHandleWin32(const wxWindow *pWin, const wchar_t *classes)
    : m_hTheme(WX_VDV_NULL_PTR)
{
    wxUxThemeEngine *pThemeEngine = wxUxThemeEngine::Get();
    if (pThemeEngine)
    {
        m_hTheme = (HTHEME) pThemeEngine->OpenThemeData(GetHwndOf(pWin), classes);
    }
}

/** Destructor
  */
wxThemeHandleWin32::~wxThemeHandleWin32(void)
{
    if (m_hTheme)
    {
        wxUxThemeEngine::Get()->CloseThemeData(m_hTheme);
    }
}

//------------------------ REOPEN -----------------------------------//
/** Reopen the theme
  * \param pWin [input]: the window using the them
  * \param classes [input]: name of the theme to open
  * \return true on success, false on failure
  */
bool wxThemeHandleWin32::Reopen(const wxWindow *pWin, const wchar_t *classes)
{
    wxUxThemeEngine *pThemeEngine = wxUxThemeEngine::Get();
    if (!pThemeEngine) return(false);

    //close
    pThemeEngine->CloseThemeData(m_hTheme);

    //reopen
    m_hTheme = (HTHEME) pThemeEngine->OpenThemeData(GetHwndOf(pWin), classes);
    return(m_hTheme);
}

#endif // wxUSE_MS_WIN_THEME
