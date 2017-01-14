/**********************************************************************/
/** FILE    : VirtualHeaderRendererNative.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a native implementation of wxVirtualHeaderIRenderer    **/
/**********************************************************************/

#ifndef _VIRTUAL_HEADER_RENDERER_NATIVE_H_
#define _VIRTUAL_HEADER_RENDERER_NATIVE_H_

#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>

//------------------------- DECLARATION -----------------------------//
#ifdef __WXMSW__
    #if wxUSE_UXTHEME
        #define wxUSE_MS_WIN_THEME      1
    #endif // wxUSE_UXTHEME
#endif // __WXMSW__

/** \class wxVirtualHeaderRendererNative : a native implementation of wxVirtualHeaderIRenderer
  */
#if wxUSE_MS_WIN_THEME != 0

#include <wx/VirtualDataView/Header/VirtualHeaderRendererWin32.h>
typedef WXDLLIMPEXP_VDV wxVirtualHeaderRendererWin32    wxVirtualHeaderRendererNative;

#else

#include <wx/VirtualDataView/Header/VirtualHeaderRendererGeneric.h>
typedef WXDLLIMPEXP_VDV wxVirtualHeaderRendererGeneric  wxVirtualHeaderRendererNative;

#endif // wxUSE_MS_WIN_THEME


#endif // _VIRTUAL_HEADER_RENDERER_NATIVE_H_
