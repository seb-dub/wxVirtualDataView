/**********************************************************************/
/** FILE    : ThemedSearchCtrl.h                                     **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : the wxSearchCtrl, with correct theming enabled         **/
/**********************************************************************/

#ifndef _WX_THEMED_SEARCH_CTRL_H_
#define _WX_THEMED_SEARCH_CTRL_H_

#include <wx/systhemectrl.h>
#include <wx/srchctrl.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

/** \class wxThemedSearchCtrl : the wxSearchCtrl, with correct theming enabled
  *
  * The wxSearchCtrl, with correct theming enabled.
  * The wxSearchCtrl does not look correct with high contrast themes on Windows. There are 2 main problems
  * to correct:
  *     1 - the colours for the text control are not the correct.
  *         the solution is to derive from wxSystemThemedControl<wxSearchCtrl> and
  *         to enable the themes in the constructors
  *
  *     2 - the bitmaps are not drawn with correct backgrounds
  *         the solution is to override the undocumented protected methods:
  *             virtual wxBitmap RenderSearchBitmap( int x, int y, bool renderDrop );
  *             virtual wxBitmap RenderCancelBitmap( int x, int y );
  *         these methods are only available in the generic version (all ports except Mac OS X),
  *         therefore a compilation guard is used (#if wxUSE_NATIVE_SEARCH_CONTROL == 0)
  */
class WXDLLIMPEXP_VDV wxThemedSearchCtrl : public  wxSystemThemedControl<wxSearchCtrl>
{
    public:

        //constructors & destructor
        wxThemedSearchCtrl(void);                                                   ///< \brief default constructor. Create() must be called manually
        wxThemedSearchCtrl(wxWindow*          pParent,
                           wxWindowID         id,
                           const wxString    &value     = wxEmptyString,
                           const wxPoint     &pos       = wxDefaultPosition,
                           const wxSize      &size      = wxDefaultSize,
                           long               style     = 0,
                           const wxValidator &validator = wxDefaultValidator,
                           const wxString    &name      = wxSearchCtrlNameStr);     ///< \brief constructor
        virtual ~wxThemedSearchCtrl(void);                                          ///< \brief destructor

        //initialization
        virtual bool CreateWin(wxWindow*          pParent,
                              wxWindowID         id,
                           const wxString    &value     = wxEmptyString,
                           const wxPoint     &pos       = wxDefaultPosition,
                           const wxSize      &size      = wxDefaultSize,
                           long               style     = 0,
                           const wxValidator &validator = wxDefaultValidator,
                           const wxString    &name      = wxSearchCtrlNameStr);     ///< \brief really creates the window

    protected:

#if wxUSE_NATIVE_SEARCH_CONTROL == 0
        virtual wxBitmap RenderSearchBitmap( int x, int y, bool renderDrop);        ///< \brief render the search button
        virtual wxBitmap RenderCancelBitmap( int x, int y );                        ///< \brief render the cancel button
#endif // wxUSE_NATIVE_SEARCH_CONTROL
};

#endif

