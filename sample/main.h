
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class wxStreamToTextRedirector;
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
class WXDLLIMPEXP_VDV wxVirtualDataViewCtrl;
class WXDLLIMPEXP_VDV wxVirtualDataViewFilterCtrl;

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
    public:
        virtual bool OnInit(void);
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h);
    ~MyFrame(void);

private:

    wxStreamToTextRedirector*   m_pRedirectCOUT;                ///< \brief redirect std::cout to the log panel
    wxStreamToTextRedirector*   m_pRedirectCERR;                ///< \brief redirect std::cerr to the log panel

    //methods
    void BuildContent(void);
    void        AddNotebookPage(wxWindow *pWindow, wxString sName);
    wxWindow*   CreateTableView(wxWindow *pParent);
    wxWindow*   CreateTreeView(wxWindow *pParent, bool bIsEmpty);
    wxWindow*   CreateScrolledWindow(wxWindow *pParent);
    wxWindow*   CreateListView(wxWindow *pParent);
    wxWindow*   CreateFilterUI(wxWindow *pParent);
    wxWindow*   CreateComboFilterUI(wxWindow *pParent);
    wxWindow*   CreateLogWindow(void);
    void        AddColumns(wxVirtualDataViewCtrl *pWindow);
    void        FillFilterItems(wxVirtualDataViewFilterCtrl *pFilterCtrl);
    void        OnPopupButton(wxCommandEvent &rEvent);

    wxDECLARE_EVENT_TABLE();
};