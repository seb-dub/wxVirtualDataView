
#include <wx/panel.h>
#include <wx/button.h>

class TestScrolledWindow : public wxPanel
{
    public:
        TestScrolledWindow(wxWindow *pParent);
        ~TestScrolledWindow(void);

    private:
        wxWindow*   m_pMainWindow;
        wxButton*   m_pButtonHScrollbar;
        wxButton*   m_pButtonVScrollbar;
        wxButton*   m_pButtonReportSize;

        void OnToggleHScrollbar(wxCommandEvent &rEvent);
        void OnToggleVScrollbar(wxCommandEvent &rEvent);
        void OnReportSize(wxCommandEvent &rEvent);

        void DoToggleScrollbar(int iScrollbar, const char *szMsg);
};
