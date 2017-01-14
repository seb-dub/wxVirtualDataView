
#include "TestScrolledWindow.h"
#include <wx/sizer.h>
#include <wx/log.h>

TestScrolledWindow::TestScrolledWindow(wxWindow *pParent)
    : wxPanel(pParent)
{
    m_pMainWindow       = new wxWindow(this, wxNewId());
    m_pButtonHScrollbar = new wxButton(this, wxNewId(), "Toggle horizontal scrollbar");
    m_pButtonVScrollbar = new wxButton(this, wxNewId(), "Toggle vertical scrollbar");
    m_pButtonReportSize = new wxButton(this, wxNewId(), "Print size");

    wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *pButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    pButtonSizer->Add(1, 0, 1);
    pButtonSizer->Add(m_pButtonHScrollbar, 1, wxALL|wxEXPAND, 5);
    pButtonSizer->Add(1, 0, 1);
    pButtonSizer->Add(m_pButtonVScrollbar, 1, wxALL|wxEXPAND, 5);
    pButtonSizer->Add(1, 0, 1);
    pButtonSizer->Add(m_pButtonReportSize, 1, wxALL|wxEXPAND, 5);
    pButtonSizer->Add(1, 0, 1);

    pMainSizer->Add(m_pMainWindow, 1, wxALL|wxEXPAND, 5);
    pMainSizer->Add(pButtonSizer, 0, wxALL|wxEXPAND, 0);

    SetSizer(pMainSizer);

    m_pButtonHScrollbar->Bind(wxEVT_BUTTON, &TestScrolledWindow::OnToggleHScrollbar, this);
    m_pButtonVScrollbar->Bind(wxEVT_BUTTON, &TestScrolledWindow::OnToggleVScrollbar, this);
    m_pButtonReportSize->Bind(wxEVT_BUTTON, &TestScrolledWindow::OnReportSize, this);
}

TestScrolledWindow::~TestScrolledWindow(void)
{
}

void TestScrolledWindow::OnToggleHScrollbar(wxCommandEvent &rEvent)
{
    DoToggleScrollbar(wxHORIZONTAL, "horizontal");
}

void TestScrolledWindow::OnToggleVScrollbar(wxCommandEvent &rEvent)
{
    DoToggleScrollbar(wxVERTICAL, "vertical");
}

void TestScrolledWindow::OnReportSize(wxCommandEvent &rEvent)
{
    int w, h;
    m_pMainWindow->GetClientSize(&w, &h);
    wxLogMessage("width = %d height = %d", w, h);
}

void TestScrolledWindow::DoToggleScrollbar(int iScrollbar, const char *szMsg)
{
    int iPos = m_pMainWindow->GetScrollPos(iScrollbar);
    int iRange = m_pMainWindow->GetScrollRange(iScrollbar);

    bool bIsOn = !((iPos == 0) && (iRange <= 1));
    if (bIsOn) m_pMainWindow->SetScrollbar(iScrollbar, 0, 0, 0, true);
    else       m_pMainWindow->SetScrollbar(iScrollbar, 0, 10, 100, true);

    wxLogMessage("Toggle %s scrollbar : is %s", szMsg, (!bIsOn ? "ON" : "OFF"));
}
