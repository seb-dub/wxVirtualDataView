/**********************************************************************/
/** FILE    : VirtualDataTableBase.h                                 **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a base class for viewing a very large amount of items  **/
/**********************************************************************/

#ifndef _WX_VIRTUAL_TABLE_BASE_H_
#define _WX_VIRTUAL_TABLE_BASE_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/systhemectrl.h>
#include <wx/control.h>

class WXDLLIMPEXP_VDV wxVirtualDataTableCtrl;
class WXDLLIMPEXP_VDV wxVirtualITableTreeDataModel;
class WXDLLIMPEXP_VDV wxVirtualDataViewCellAttrProvider;
class WXDLLIMPEXP_VDV wxVirtualDataViewCellMapper;
class WXDLLIMPEXP_VDV wxVirtualDataViewItemAttr;

extern const char wxsVirtualDataTableBaseName[];

/** \class wxVirtualDataTableBase : main class for table view of the items. Used by wxVirtualDataTableCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataTableBase : public wxSystemThemedControl<wxControl>
{
    public:

        //constructors & destructor
        wxVirtualDataTableBase(void);                                       ///< \brief default constructor. Create() must be called manually
        wxVirtualDataTableBase(wxWindow *pParent, wxWindowID id,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long lStyle    = 0,
                              wxString sName = wxsVirtualDataTableBaseName);///< \brief constructor
        virtual ~wxVirtualDataTableBase(void);                              ///< \brief destructor

        //initialization
        bool Create(wxWindow *pParent, wxWindowID id, const wxPoint& pos,
                    const wxSize& size, long lStyle, wxString sName);       ///< \brief really creates the control and sets the initial number of items in it

        //owner
        wxVirtualDataTableCtrl*  GetOwner(void) const;                       ///< \brief get the owner window
        void SetOwner(wxVirtualDataTableCtrl *pOwner);                       ///< \brief set the owner window

        //windows management
        #ifdef __WXUNIVERSAL__
        bool IsCurrent(void) const;                                         ///< \brief check if the current window is the current one
        #endif

        //visual attributes
        wxVisualAttributes GetDefaultAttributes(void) const;                ///< \brief get the default window attributes
        static wxVisualAttributes
         GetClassDefaultAttributes(wxWindowVariant variant =
                                   wxWINDOW_VARIANT_NORMAL);                ///< \brief get the default window attributes

        //data model
        wxVirtualITableTreeDataModel*   GetDataModel(void) const;                   ///< \brief get the current data model used
        void                            SetDataModel(wxVirtualITableTreeDataModel *pModel);  ///< \brief set the current data model

        //callbacks for data change
        void OnModelRowCountChanged(void);                                  ///< \brief the model has changed its amount of row
        void OnModelColCountChanged(void);                                  ///< \brief the model has changed its amount of columns

        //rows mapping
        wxVirtualDataViewCellMapper* GetRowMapper(void) const;              ///< \brief get the row mapper
        void SetRowMapper(wxVirtualDataViewCellMapper *pMapper);            ///< \brief set the row mapper
        size_t GetModelRow(size_t uiViewRow) const;                         ///< \brief convert a view row coord to a model row coord
        size_t GetViewRow(size_t uiModelRow) const;                         ///< \brief convert a model row coord to a view row coord
        size_t GetAmountOfViewRows(void) const;                             ///< \brief get the amount of view rows

        //column mapping
        wxVirtualDataViewCellMapper* GetColMapper(void) const;              ///< \brief get the column mapper
        void SetColMapper(wxVirtualDataViewCellMapper *pMapper);            ///< \brief set the column mapper
        size_t GetModelCol(size_t uiViewCol) const;                         ///< \brief convert a view column coord to a model column coord
        size_t GetViewCol(size_t uiModelCol) const;                         ///< \brief convert a model column coord to a view column coord
        size_t GetAmountOfViewCols(void) const;                             ///< \brief get the amount of view columns

        //row and columns size
        wxCoord GetRowHeight(size_t uiViewRow) const;                       ///< \brief get the height of a row
        wxCoord GetColumnWidth(size_t uiViewCol) const;                     ///< \brief get the column width

    protected:
        //data
        wxVirtualDataTableCtrl*             m_pOwner;                       ///< \brief the owner window
        wxVirtualITableTreeDataModel*       m_pDataModel;                   ///< \brief the data model
        wxVirtualDataViewCellMapper*        m_pRowMapper;                   ///< \brief convert row coordinates (view/model)
        wxVirtualDataViewCellMapper*        m_pColMapper;                   ///< \brief convert column coordinates (view/model)
        wxCoord                             m_iMarginX;                     ///< \brief X margin (amount of pixels on left and right of the cell)
        wxCoord                             m_iMarginY;                     ///< \brief Y margin (amount of pixels on the top and bottom of the cell)
        wxCoord                             m_iGridPenWidth;                ///< \brief line width used to draw

        //methods
        void Init(void);                                                    ///< \brief common part of all constructors
        void Release(void);                                                 ///< \brief release internal resources
        void GetBaseItemAttr(wxVirtualDataViewItemAttr &rBaseAttr) const;   ///< \brief get base item attributes

        //interface: event handlers
        virtual void OnPaint(wxPaintEvent& event);                          ///< \brief paint handler
        virtual void OnSetOrKillFocus(wxFocusEvent& event);                 ///< \brief change focus handler
        virtual void OnKeyDown(wxKeyEvent& event);                          ///< \brief key down event handler
        virtual void OnLeftDown(wxMouseEvent& event);                       ///< \brief left button down event handler
        virtual void OnLeftDClick(wxMouseEvent& event);                     ///< \brief double click event handler

        //interface with default behaviour
        virtual wxVisualAttributes DoGetDefaultAttributes(void) const;      ///< \brief get the default window attributes

    private:
        //methods
        //rows mapping
        size_t DoGetModelRow(size_t uiViewRow) const;                       ///< \brief convert a view row coord to a model row coord
        size_t DoGetViewRow(size_t uiModelRow) const;                       ///< \brief convert a model row coord to a view row coord

        //columns mapping
        size_t DoGetModelCol(size_t uiViewCol) const;                       ///< \brief convert a view column coord to a model column coord
        size_t DoGetViewCol(size_t uiModelCol) const;                       ///< \brief convert a model column coord to a view column coord

        //cell sizes
        wxCoord DoGetRowHeight(size_t uiModelRow) const;                    ///< \brief get the height of a row
        wxCoord DoGetColumnWidth(size_t uiModelCol) const;                  ///< \brief get the column width

        wxDECLARE_NO_COPY_CLASS(wxVirtualDataTableBase);
        wxDECLARE_ABSTRACT_CLASS(wxVirtualDataTableBase);
};

#endif // _WX_VLBOX_H_

