
#include "main.h"
#include "TestDataModel.h"
#include "TestTreeDataModel.h"
#include "TestListDataModel.h"
#include "TestScrolledWindow.h"

#include <wx/VirtualDataView/Table/VirtualDataTableCtrl.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewCtrl.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewListCtrl.h>
#include <wx/VirtualDataView/DataView/VirtualDataViewColHeader.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterCtrl.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPanel.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterComboCtrl.h>
#include <wx/VirtualDataView/FiltersUI/VirtualDataViewFilterPopup.h>

#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttrProvider.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewChoiceEditor.h>



#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/filedlg.h>

//filters : user interface
//      -> text filter (simple match + begin/end/contains/not)
//      -> numeric filter (simple match + arithmetic match)
//      -> composed filters for 1 column
//      -> filters which applies on all columns (database like)
//VPM tree view maybe

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit(void)
{
    if (!wxApp::OnInit()) return(false);

    MyFrame *frame = new MyFrame(WX_VDV_NULL_PTR, "wxVirtualDataViewCtrl test", 40, 40, 1000, 540);

    frame->Maximize();
    frame->Show(true);
    return(true);
}


// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)

wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h)
    : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h)),
      m_pRedirectCOUT(NULL), m_pRedirectCERR(NULL)
{
    BuildContent();
}

MyFrame::~MyFrame(void)
{
    if (m_pRedirectCOUT)
    {
        delete(m_pRedirectCOUT);
        m_pRedirectCOUT = 0;
    }
    if (m_pRedirectCERR)
    {
        delete(m_pRedirectCERR);
        m_pRedirectCERR = 0;
    }
}

void MyFrame::BuildContent(void)
{
    //create notebook and pages
    wxNotebook *pNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
    AddNotebookPage(CreateTreeView(pNotebook, false), "Tree");
    AddNotebookPage(CreateTreeView(pNotebook, true), "Empty Tree");
    AddNotebookPage(CreateListView(pNotebook), "List view");
    AddNotebookPage(CreateFilterUI(pNotebook), "Filter UI");
    AddNotebookPage(CreateComboFilterUI(pNotebook), "Filter combo UI");
    AddNotebookPage(CreateTableView(pNotebook), "Table");
    AddNotebookPage(CreateScrolledWindow(pNotebook), "ScrollTest");

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
    AddNotebookPage(CreateDataViewTree(pNotebook), "DataView Compat Layer: tree");
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

    //log window
    wxWindow *pLog = CreateLogWindow();

    //add to sizer
    wxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(pNotebook, 1, wxGROW, 0);
    pMainSizer->Add(pLog, 0, wxGROW);
    SetSizerAndFit(pMainSizer);

    //status bar
    wxStatusBar *pStatusBar = new wxStatusBar(this, wxID_ANY, 0);
    int vStatusBarWidths[1] = { -1 };
    int vStatusBarStyles[1] = { wxSB_NORMAL };
    pStatusBar->SetFieldsCount(1, vStatusBarWidths);
    pStatusBar->SetStatusStyles(1, vStatusBarStyles);
    SetStatusBar(pStatusBar);
}

wxWindow* MyFrame::CreateLogWindow(void)
{
    wxTextCtrl *pLog = new wxTextCtrl(this, wxID_ANY, wxString(), wxDefaultPosition,
                                      wxDefaultSize, wxTE_MULTILINE );
    pLog->SetMinSize(wxSize(200, 100));
    wxLog::SetActiveTarget(new wxLogTextCtrl(pLog));
    wxLogMessage("This is the log window");

    m_pRedirectCOUT = new wxStreamToTextRedirector(pLog);
	m_pRedirectCERR = new wxStreamToTextRedirector(pLog, &std::cerr);

    return(pLog);
}

void MyFrame::AddNotebookPage(wxWindow *pWindow, wxString sName)
{
    wxNotebook *pNotebook = (wxNotebook *) pWindow->GetParent();
    pWindow->SetMinSize(wxSize(300, 200));
    pNotebook->AddPage(pWindow, sName);
}

wxWindow* MyFrame::CreateTableView(wxWindow *pParent)
{
    int iStyle = 0; //wxVDV_NO_ROW_HEADER;
    wxVirtualDataTableCtrl *pTable = new wxVirtualDataTableCtrl(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, iStyle);
    TestDataModel *pModel = new TestDataModel;
    pTable->SetDataModel(pModel);

    //set row 5 to bold red on green background
    wxVirtualDataViewItemAttr *pAttr = new wxVirtualDataViewItemAttr();
    pAttr->SetBackgroundColour(wxColour(0, 255, 0));
    pAttr->SetColour(wxColour(255, 0, 0));
    pAttr->SetBold(true);
    wxVirtualDataViewCellAttr *pCellAttr = new wxVirtualDataViewCellAttr;
    pCellAttr->SetGraphicAttributes(pAttr);
    pModel->GetAttributesProvider()->SetRowAttribute(5, pCellAttr);

    return(pTable);
}

void MyFrame::AddColumns(wxVirtualDataViewCtrl *pWindow)
{
    if (!pWindow) return;

    wxVirtualDataViewColumn oCol1("Bitmap + Text", 0);
    oCol1.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_BITMAPTEXT);
    oCol1.SetFromType();
    oCol1.SetWidth(200);
    oCol1.SetMinWidth(50);
    oCol1.SetEditable();
    pWindow->Columns().AppendColumn(oCol1);

    wxVirtualDataViewColumn oCol2("Boolean", 1);
    oCol2.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_BOOL);
    oCol2.SetFromType();
    oCol2.SetWidth(50);
    oCol2.SetEditable();
    oCol2.SetActivatable();
    pWindow->Columns().AppendColumn(oCol2);

    wxVirtualDataViewColumn oCol3("Choice", 2);
    oCol3.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_STRING);
    oCol3.SetFromType();
    oCol3.SetWidth(200);
    oCol3.SetEditable();
    wxVirtualDataViewChoiceEditor *pChoiceEditor = new wxVirtualDataViewChoiceEditor;
    wxArrayString vChoices;
    vChoices.push_back("One");
    vChoices.push_back("Two");
    vChoices.push_back("Three");
    pChoiceEditor->SetReadOnly();
    pChoiceEditor->SetChoices(vChoices);
    oCol3.GetAttribute()->SetEditor(pChoiceEditor);
    pWindow->Columns().AppendColumn(oCol3);

    wxVirtualDataViewColumn oCol4("ComboBox", 3);
    oCol4.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_STRING);
    oCol4.SetFromType();
    oCol4.SetWidth(200);
    oCol4.SetEditable();
    pChoiceEditor = new wxVirtualDataViewChoiceEditor;
    pChoiceEditor->SetReadOnly(false);
    pChoiceEditor->SetChoices(vChoices);
    oCol4.GetAttribute()->SetEditor(pChoiceEditor);
    pWindow->Columns().AppendColumn(oCol4);

    wxVirtualDataViewColumn oCol5("Date", 4);
    oCol5.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_DATE);
    oCol5.SetFromType();
    oCol5.SetWidth(200);
    oCol5.SetEditable(true);
    pWindow->Columns().AppendColumn(oCol5);

    wxVirtualDataViewColumn oCol6("Time", 5);
    oCol6.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_TIME);
    oCol6.SetFromType();
    oCol6.SetWidth(200);
    oCol6.SetEditable(true);
    pWindow->Columns().AppendColumn(oCol6);

    wxVirtualDataViewColumn oCol7("Date + time", 6);
    oCol7.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_DATE_TIME);
    oCol7.SetFromType();
    oCol7.SetWidth(200);
    oCol7.SetEditable(true);
    pWindow->Columns().AppendColumn(oCol7);

    wxVirtualDataViewColumn oCol8("Signed integer", 7);
    oCol8.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_INTEGER);
    oCol8.SetFromType();
    oCol8.SetWidth(200);
    oCol8.SetEditable(true);
    pWindow->Columns().AppendColumn(oCol8);

    wxVirtualDataViewColumn oCol9("Unsigned integer", 8);
    oCol9.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_UNSIGNED_INTEGER);
    oCol9.SetFromType();
    oCol9.SetWidth(200);
    oCol9.SetEditable(true);
    pWindow->Columns().AppendColumn(oCol9);

    wxVirtualDataViewColumn oCol10("Double", 9);
    oCol10.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_DOUBLE);
    oCol10.SetFromType();
    oCol10.SetWidth(200);
    oCol10.SetEditable(true);
    pWindow->Columns().AppendColumn(oCol10);

    wxVirtualDataViewColumn oCol11("Colour", 10);
    oCol11.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_COLOUR);
    oCol11.SetFromType();
    oCol11.SetWidth(200);
    oCol11.SetEditable(true);
    oCol11.SetActivatable(true);
    pWindow->Columns().AppendColumn(oCol11);

    wxVirtualDataViewColumn oCol12("Progress", 11);
    oCol12.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_PROGRESS);
    oCol12.SetFromType();
    oCol12.SetWidth(200);
    oCol12.SetEditable(true);
    oCol12.SetActivatable(true);
    pWindow->Columns().AppendColumn(oCol12);
}

wxWindow* MyFrame::CreateTreeView(wxWindow *pParent, bool bIsEmpty)
{
    int iStyle = 0; //wxVDV_NO_ROW_HEADER;
    wxVirtualDataViewCtrl *pTree = new wxVirtualDataViewCtrl(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, iStyle);
    //pTree->HideRowHeader();

    //add columns
    AddColumns(pTree);

    //show filters - after adding columns
    //because otherwise they have a height of 0: there are no editors defined !
    pTree->ShowFilters();

    //set model
    TestTreeDataModel *pModel = new TestTreeDataModel(bIsEmpty);
    pTree->SetDataModel(pModel);

    return(pTree);
}


wxWindow* MyFrame::CreateListView(wxWindow *pParent)
{
    int iStyle = 0; //wxVDV_NO_ROW_HEADER;
    //wxVirtualDataViewCtrl *pList = new wxVirtualDataViewCtrl(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, iStyle);
    wxVirtualDataViewListCtrl *pList = new wxVirtualDataViewListCtrl(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, iStyle);
    //pList->HideRowHeader();

    //add columns
    AddColumns(pList);

    //show filters - after adding columns
    //because otherwise they have a height of 0: there are no editors defined !
    pList->ShowFilters();

    //set model
    TestListDataModel *pModel = new TestListDataModel();
    pList->SetDataModel(pModel);

    return(pList);
}

void MyFrame::FillFilterItems(wxVirtualDataViewFilterCtrl *pFilterCtrl)
{
    if (!pFilterCtrl) return;

    wxVector<wxVariant> &rvValues = pFilterCtrl->Values();
    size_t i, uiSize;
    uiSize = 20;
    rvValues.clear();
    rvValues.reserve(uiSize);
    for(i=0;i<uiSize;i++)
    {
        rvValues.push_back(wxVariant(wxString::Format("%d", i)));
    }

    pFilterCtrl->OnDataModelChanged();
    pFilterCtrl->AutoSizeAllColumns();
}

wxWindow* MyFrame::CreateFilterUI(wxWindow *pParent)
{
    wxVirtualDataViewFilterPanel *pFilterPanel = new wxVirtualDataViewFilterPanel(pParent);
    wxVirtualDataViewFilterCtrl *pFilterCtrl = pFilterPanel->GetFilterControl();
    FillFilterItems(pFilterCtrl);
    return(pFilterPanel);
}

wxWindow* MyFrame::CreateComboFilterUI(wxWindow *pParent)
{
    wxPanel *pPanel = new wxPanel(pParent);

    //sizer
    wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

    //combo box
    wxVirtualDataViewFilterComboCtrl *pFilterCombo = new wxVirtualDataViewFilterComboCtrl(pPanel);
    wxVirtualDataViewFilterCtrl *pFilterCtrl = pFilterCombo->GetFilterControl();
    FillFilterItems(pFilterCtrl);

    //button triggering a popup
    wxButton *pButton = new wxButton(pPanel, wxNewId(), "Show Popup");
    pButton->Bind(wxEVT_BUTTON, &MyFrame::OnPopupButton, this);

    //add to sizer
    pMainSizer->Add(pFilterCombo, 0, wxALL, 0);
    pMainSizer->Add(pButton, 0, wxALL, 0);
    SetSizerAndFit(pMainSizer);

    return(pPanel);
}

wxWindow* MyFrame::CreateScrolledWindow(wxWindow *pParent)
{
    return(new TestScrolledWindow(pParent));
}

void MyFrame::OnPopupButton(wxCommandEvent &rEvent)
{
    wxVirtualDataViewFilterPopup *pPopupWin = new wxVirtualDataViewFilterPopup(this);
    wxVirtualDataViewFilterPanel *pPanel = pPopupWin->GetFilterPanel();
    if (pPanel) FillFilterItems(pPanel->GetFilterControl());

    pPopupWin->SetPosition(wxPoint(100, 100));
    pPopupWin->Show();
    pPopupWin->SetFocus();
}

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
wxWindow* MyFrame::CreateDataViewTree(wxWindow *pParent)
{
    int iStyle = 0;
    wxVirtualDataViewCtrl *pTree = new wxVirtualDataViewCtrl(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, iStyle);
    //pList->HideRowHeader();

    //add columns
    wxVirtualDataViewColumn oCol1("title", 0);
    oCol1.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_STRING);
    oCol1.SetFromType();
    oCol1.SetWidth(200);
    oCol1.SetMinWidth(50);
    oCol1.SetEditable();
    pTree->Columns().AppendColumn(oCol1);

    wxVirtualDataViewColumn oCol2("artist", 0);
    oCol2.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_STRING);
    oCol2.SetFromType();
    oCol2.SetWidth(200);
    oCol2.SetMinWidth(50);
    oCol2.SetEditable();
    pTree->Columns().AppendColumn(oCol2);

    wxVirtualDataViewColumn oCol3("year", 0);
    oCol3.SetType(wxVirtualDataViewColumn::wxVDATAVIEW_COL_STRING);
    oCol3.SetFromType();
    oCol3.SetWidth(200);
    oCol3.SetMinWidth(50);
    oCol3.SetEditable();
    pTree->Columns().AppendColumn(oCol3);

    //set model
    m_music_model = new MyMusicTreeModel;
    pTree->AssociateModel(m_music_model.get());

    return(pTree);

}
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
