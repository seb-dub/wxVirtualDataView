/**********************************************************************/
/** FILE    : VirtualDataViewColumn.h                                **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view column                                     **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_COLUMN_H_
#define _VIRTUAL_DATA_VIEW_COLUMN_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/string.h>
#include <wx/bitmap.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewCellAttr;
class WXDLLIMPEXP_VDV wxVirtualDataViewEditor;
class WXDLLIMPEXP_VDV wxVirtualDataViewRenderer;
class WXDLLIMPEXP_VDV wxVirtualDataViewFilter;
class WXDLLIMPEXP_VDV wxVirtualDataViewIFilterEditor;

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
class WXDLLIMPEXP_ADV wxDataViewColumn;
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

/** \class wxVirtualDataViewColumn : a column for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewColumn
{
    public:
        //enums
        /// \enum wxVirtualDataViewColumnFlags : style flags for column. Can be combined
        enum wxVirtualDataViewColumnFlags
        {
            wxVDATAVIEW_COL_RESIZABLE     = 0x00000001,                                 ///< \brief the column is resizeable
            wxVDATAVIEW_COL_SORTABLE      = 0x00000002,                                 ///< \brief the column is sortable
            wxVDATAVIEW_COL_REORDERABLE   = 0x00000004,                                 ///< \brief the column is reorderable
            wxVDATAVIEW_COL_HIDDEN        = 0x00000008,                                 ///< \brief the column is hidden
            wxVDATAVIEW_COL_DEFAULT       = wxVDATAVIEW_COL_RESIZABLE |
                                            wxVDATAVIEW_COL_SORTABLE  |
                                            wxVDATAVIEW_COL_REORDERABLE                 ///< \brief the column is by default resizeable, sortable, reorderable
        };

        /// \enum wxVirtualDataViewColumnWidth : special values for column width
        enum wxVirtualDataViewColumnWidth
        {
            wxVDATAVIEW_COL_DEFAULT_WIDTH   = -1,                                       ///< \brief the column has a default width
            wxVDATAVIEW_COL_AUTOSIZE        = -2                                        ///< \brief the column resize itself automatically
        };

        /// \enum EType : the type of data
        enum EType
        {
            wxVDATAVIEW_COL_STRING              = 0,                                    ///< \brief a string
            wxVDATAVIEW_COL_BITMAP              = 1,                                    ///< \brief a bitmap/icon
            wxVDATAVIEW_COL_BITMAPTEXT          = 2,                                    ///< \brief a bitmap/icon + text
            wxVDATAVIEW_COL_PROGRESS            = 3,                                    ///< \brief a progress bar
            wxVDATAVIEW_COL_BOOL                = 4,                                    ///< \brief a boolean (checkbox)
            wxVDATAVIEW_COL_TRISTATE            = 5,                                    ///< \brief a tri-state checkbox
            wxVDATAVIEW_COL_INTEGER             = 6,                                    ///< \brief an integer
            wxVDATAVIEW_COL_UNSIGNED_INTEGER    = 7,                                    ///< \brief an unsigned integer
            wxVDATAVIEW_COL_DOUBLE              = 8,                                    ///< \brief a double
            wxVDATAVIEW_COL_DATE_TIME           = 9,                                    ///< \brief a date + time
            wxVDATAVIEW_COL_DATE                = 10,                                   ///< \brief a date
            wxVDATAVIEW_COL_TIME                = 11,                                   ///< \brief a time
            wxVDATAVIEW_COL_COLOUR              = 12,                                   ///< \brief a colour
            wxVDATAVIEW_COL_UNKNOWN             = -1                                    ///< \brief custom
        };

        //constants
        static const size_t wxVDVC_DEFAULT_WIDTH;                                       ///< \brief default width

        //constructors & destructor
        wxVirtualDataViewColumn(void);                                                  ///< \brief default constructor
        wxVirtualDataViewColumn(const wxString& sTitle,
                                size_t uiModelColumn,
                                size_t uiWidth      = wxVDVC_DEFAULT_WIDTH,
                                wxAlignment uiAlign = wxALIGN_CENTER,
                                int iFlags          = wxVDATAVIEW_COL_DEFAULT);         ///< \brief constructor
        wxVirtualDataViewColumn(const wxBitmap& bitmap,
                                size_t uiModelColumn,
                                size_t uiWidth      = wxVDVC_DEFAULT_WIDTH,
                                wxAlignment uiAlign = wxALIGN_CENTER,
                                int iFlags          = wxVDATAVIEW_COL_DEFAULT);         ///< \brief constructor
        wxVirtualDataViewColumn(const wxVirtualDataViewColumn &rhs);                    ///< \brief copy constructor
        wxVirtualDataViewColumn& operator=(const wxVirtualDataViewColumn &rhs);         ///< \brief assignation operator
        ~wxVirtualDataViewColumn(void);                                                 ///< \brief destructor

        //model column
        void   SetModelColumn(size_t uiColumn);                                         ///< \brief set the model column
        size_t GetModelColumn(void) const;                                              ///< \brief get the model column

        //bitmap
        void SetBitmap(const wxBitmap& bitmap);                                         ///< \brief set the column bitmap
        wxBitmap GetBitmap(void) const;                                                 ///< \brief get the column bitmap

        //title
        void SetTitle(const wxString &rsTitle);                                         ///< \brief set the title
        wxString GetTitle(void) const;                                                  ///< \brief get the title

        //width & min width
        void SetWidth(int iWidth);                                                      ///< \brief set the column width
        int GetWidth(void) const;                                                       ///< \brief get the column VISIBLE width (0 if hidden)
        int GetDesignWidth(void) const;                                                 ///< \brief get the column DESIGN width (non-0 if hidden)

        void SetMinWidth(int iMinWidth);                                                ///< \brief set the minimal column width
        int GetMinWidth(void) const;                                                    ///< \brief get the minimal column width

        //alignment
        void SetAlignment(wxAlignment uiAlign);                                         ///< \brief set the alignment
        wxAlignment GetAlignment(void) const;                                           ///< \brief get the alignment

        //flags
        void SetFlags(int iFlags);                                                      ///< \brief set the style flags
        int GetFlags(void) const;                                                       ///< \brief get the style flags
        bool IsSortable(void) const;                                                    ///< \brief check if the column is sortable
        bool CanReorder(void) const;                                                    ///< \brief check if the column can be reordered
        bool CanResize(void) const;                                                     ///< \brief check if the column can be resized

        //cell flags
        void SetCellFlags(int iFlags);                                                  ///< \brief set cell flags
        int GetCellFlags(void) const;                                                   ///< \brief get cell flags
        bool IsReadOnly(void) const;                                                    ///< \brief return true if the cell is read-only
        bool IsActivatable(void) const;                                                 ///< \brief return true if the cell is activatable
        bool IsEditable(void) const;                                                    ///< \brief return true if the cell is editable
        void SetReadOnly(bool bReadOnly = true);                                        ///< \brief set read-only / editable
        void SetEditable(bool bEditable = true);                                        ///< \brief set editable / read-only
        void SetActivatable(bool bActivatable = true);                                  ///< \brief set / unset activatable

        //sorting
        void SetSortKey(bool bIsSortKey);                                               ///< \brief set if the column is used for sorting or not
        bool IsSortKey(void) const;                                                     ///< \brief return true if the column is the one currently used for sorting
        bool IsSortOrderAscending(void) const;                                          ///< \brief return true if the sorting is ascending
        void SetSortOrder(bool bAscending);                                             ///< \brief set the sorting order for this column

        //visible
        bool IsShown(void) const;                                                       ///< \brief return true if the column is shown
        bool IsHidden(void) const;                                                      ///< \brief return true if the column is hidden
        void Show(bool bShow = true);                                                   ///< \brief show/hide the column
        void Hide(bool bHide = true);                                                   ///< \brief hide/show the column
        void ToggleHideShow(void);                                                      ///< \brief toggle the visibility ON/OFF

        //attributes
        void SetAttribute(wxVirtualDataViewCellAttr *pAttr);                            ///< \brief set the column attributes
        wxVirtualDataViewCellAttr* GetAttribute(void) const;                            ///< \brief get the column attributes

        //shortcut : get renderer & editor & filter
        wxVirtualDataViewEditor*        GetEditor(void);                                ///< \brief get the column editor
        wxVirtualDataViewRenderer*      GetRenderer(void);                              ///< \brief get the column renderer
        wxVirtualDataViewFilter*        GetFilter(void);                                ///< \brief get the column filter
        wxVirtualDataViewIFilterEditor* GetFilterEditor(void);                          ///< \brief get the column filter editor

        //type
        EType GetType(void) const;                                                      ///< \brief get the type
        void  SetType(EType eType);                                                     ///< \brief set the type

        //default type
        bool SetFromType(void);                                                         ///< \brief set the attributes from the type
        void SetString(void);                                                           ///< \brief set the attribute as string
        void SetBitmap(void);                                                           ///< \brief set the attribute as a bitmap
        void SetBitmapText(void);                                                       ///< \brief set the attribute as a bitmap + text
        void SetProgress(int iMin = 0, int iMax = 100, bool bDrawValue = true);         ///< \brief set the attribute as a progress bar
        void SetBool(void);                                                             ///< \brief set the attribute as boolean
        void SetTriState(void);                                                         ///< \brief set the attribute as a tri state checkbox
        void SetInteger(void);                                                          ///< \brief set the attribute as integer
        void SetUnsignedInteger(void);                                                  ///< \brief set the attribute as unsigned integer
        void SetDouble(void);                                                           ///< \brief set the attribute as double
        void SetDateTime(void);                                                         ///< \brief set the attribute as date-time
        void SetDate(void);                                                             ///< \brief set the attribute as date
        void SetTime(void);                                                             ///< \brief set the attribute as time
        void SetColour(void);                                                           ///< \brief set the attribute as colour

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
        //convert to wxDataViewColumn
        wxDataViewColumn ConvertToDataViewCol(void) const;                              ///< \brief convert to a wxDataViewColumn
        void SetFromDataViewCol(const wxDataViewColumn &rCol);                          ///< \brief set from a wxDataViewColumn
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

    protected:
        //data
        size_t                      m_uiModelColumn;                                    ///< \brief model column
        wxBitmap                    m_Bitmap;                                           ///< \brief bitmap of the column
        wxString                    m_sTitle;                                           ///< \brief column title
        long                        m_lWidth;                                           ///< \brief width of the column
        long                        m_lMinWidth;                                        ///< \brief minimal width of the column
        wxAlignment                 m_Alignment;                                        ///< \brief alignment of title and values
        int                         m_iFlags;                                           ///< \brief style flags
        bool                        m_bSort;                                            ///< \brief true if the column is sorted
        bool                        m_bSortAscending;                                   ///< \brief true if the column is sorted in ascending order
        bool                        m_bIsShown;                                         ///< \brief true if the column is shown
        wxVirtualDataViewCellAttr*  m_pAttributes;                                      ///< \brief column attributes
        EType                       m_eType;                                            ///< \brief the type

        //methods
        void Copy(const wxVirtualDataViewColumn &rhs);                                  ///< \brief copy the column
        void Release(void);                                                             ///< \brief release the resources
        void Init(long lWidth, wxAlignment iAlign, int iFlags);                         ///< \brief initialization
};

#endif

