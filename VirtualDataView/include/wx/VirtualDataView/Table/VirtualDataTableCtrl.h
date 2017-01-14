/**********************************************************************/
/** FILE    : VirtualDataTableCtrl.h                                 **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a class for viewing a very large amount of items       **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_TABLE_CTRL_H_
#define _WX_VIRTUAL_TABLE_CTRL_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/vscroll.h>
#include <wx/systhemectrl.h>

class WXDLLIMPEXP_VDV wxVirtualITableTreeDataModel;
class WXDLLIMPEXP_VDV wxVirtualDataTableBase;
class WXDLLIMPEXP_VDV wxVirtualDataViewCellMapper;
class WXDLLIMPEXP_VDV wxVirtualDataTableHeader;
class WXDLLIMPEXP_VDV wxVirtualDataTableCornerHeader;

extern const char wxVirtualDataTableCtrlNameStr[];

//styles
#define wxVDT_SINGLE                  0x0000     // for convenience
#define wxVDT_MULTIPLE                0x0001     // can select multiple items

#define wxVDT_NO_COL_HEADER           0x0002     // column titles not visible
#define wxVDT_NO_ROW_HEADER           0x0004     // row titles not visible
#define wxVDT_HORIZ_RULES             0x0008     // light horizontal rules between rows
#define wxVDT_VERT_RULES              0x0010     // light vertical rules between columns

#define wxVDT_ROW_LINES               0x0020     // alternating colour in rows
#define wxVDT_VARIABLE_LINE_HEIGHT    0x0040     // variable line height

/// \class wxVirtualDataTableCtrl : the main virtual data table control
class WXDLLIMPEXP_VDV wxVirtualDataTableCtrl : public wxSystemThemedControl<wxHVScrolledWindow>
{
    public:

        //constructors & destructor
        wxVirtualDataTableCtrl(void);                                           ///< \brief default constructor. Create() must be called manually
        wxVirtualDataTableCtrl(wxWindow *pParent, wxWindowID id,
                              const wxPoint& pos, const wxSize& size,
                              long lStyle,
                              const wxValidator& validator = wxDefaultValidator,
                              wxString sName = wxVirtualDataTableCtrlNameStr);  ///< \brief constructor
        virtual ~wxVirtualDataTableCtrl(void);                                  ///< \brief destructor

        //main window
        wxVirtualDataTableBase* GetMainWindow(void) const;                      ///< \brief get the main window

        //headers
        wxVirtualDataTableHeader*        GetColumnHeaderWindow(void) const;     ///< \brief get the column header window
        wxVirtualDataTableHeader*        GetRowHeaderWindow(void) const;        ///< \brief get the row header window
        wxVirtualDataTableCornerHeader*  GetCornerHeaderWindow(void) const;     ///< \brief get the corner header window

        //initialization
        bool Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                    const wxSize& size, long lStyle,
                    const wxValidator& validator, wxString sName);              ///< \brief really creates the control and sets the initial number of items in it

        //windows management
        #ifdef __WXUNIVERSAL__
        bool IsCurrent(void) const;                                             ///< \brief check if the current window is the current one
        #endif

        //visual attributes
        wxVisualAttributes GetDefaultAttributes(void) const;                    ///< \brief get the default window attributes
        static wxVisualAttributes
         GetClassDefaultAttributes(wxWindowVariant variant =
                                   wxWINDOW_VARIANT_NORMAL);                    ///< \brief get the default window attributes

        //data model
        wxVirtualITableTreeDataModel*   GetDataModel(void);                     ///< \brief get the current data model used
        void    SetDataModel(wxVirtualITableTreeDataModel *pModel);             ///< \brief set the current data model

        //callbacks for data change
        void OnModelRowCountChanged(void);                                      ///< \brief the model has changed its amount of row
        void OnModelColCountChanged(void);                                      ///< \brief the model has changed its amount of columns

        //rows mapping
        wxVirtualDataViewCellMapper* GetRowMapper(void) const;                  ///< \brief get the row mapper
        void SetRowMapper(wxVirtualDataViewCellMapper *pMapper);                ///< \brief set the row mapper
        size_t GetModelRow(size_t uiViewRow) const;                             ///< \brief convert a view row coord to a model row coord
        size_t GetViewRow(size_t uiModelRow) const;                             ///< \brief convert a model row coord to a view row coord
        size_t GetAmountOfViewRows(void) const;                                 ///< \brief get the amount of view rows

        //column mapping
        wxVirtualDataViewCellMapper* GetColMapper(void) const;                  ///< \brief get the column mapper
        void SetColMapper(wxVirtualDataViewCellMapper *pMapper);                ///< \brief set the column mapper
        size_t GetModelCol(size_t uiViewCol) const;                             ///< \brief convert a view column coord to a model column coord
        size_t GetViewCol(size_t uiModelCol) const;                             ///< \brief convert a model column coord to a view column coord
        size_t GetAmountOfViewCols(void) const;                                 ///< \brief get the amount of view columns

        //selection

        //current item

        //margins

        //selection colour

        //row and columns size
        wxCoord GetRowHeight(size_t uiViewRow) const;                           ///< \brief get the height of a row
        wxCoord GetColumnWidth(size_t uiViewCol) const;                         ///< \brief get the column width

    protected:
        //data
        wxVirtualDataTableBase*         m_pClientArea;                          ///< \brief the window showing the data
        wxVirtualDataTableHeader*       m_pColumnHeaderWindow;                  ///< \brief column header window
        wxVirtualDataTableHeader*       m_pRowHeaderWindow;                     ///< \brief row header window
        wxVirtualDataTableCornerHeader* m_pCornerHeaderWindow;                  ///< \brief corner header window
        int                             m_dx;

        //methods
        void InitDataTable(void);                                               ///< \brief initialize the control

        //wxHVScrolledWindow interface implementation
        virtual wxCoord OnGetRowHeight(size_t uiViewRow) const;                 ///< \brief get the height of a row
        virtual wxCoord OnGetColumnWidth(size_t uiViewCol) const;               ///< \brief get the column width

        //scrolling event handlers
        virtual void OnSize(wxSizeEvent &event);                                ///< \brief sizing event handlers
        virtual void OnScrollEvent(wxScrollWinEvent &event);                    ///< \brief scrolling event handler

    private:
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataTableCtrl);

};

#endif // _WX_VLBOX_H_

