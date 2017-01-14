/**********************************************************************/
/** FILE    : VirtualDataViewColumn.cpp                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : interface to a data view column                        **/
/**********************************************************************/


#include <wx/VirtualDataView/Columns/VirtualDataViewColumn.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/FilterEditors/VirtualDataViewIFilterEditor.h>

//-------------------- STATIC MEMBERS -------------------------------//
const size_t wxVirtualDataViewColumn::wxVDVC_DEFAULT_WIDTH = 80;

//---------------------- CONSTRUCTORS & DESTRUCTOR ------------------//
/** Default constructor
  */
wxVirtualDataViewColumn::wxVirtualDataViewColumn(void)
    : m_uiModelColumn(0)
{
    Init(wxVDVC_DEFAULT_WIDTH, wxALIGN_CENTER, wxVDATAVIEW_COL_RESIZABLE);
}

/** Constructor : the column display a text
  * \param sTitle        [input]: the title of the column
  * \param uiModelColumn [input]: the index of the corresponding column in the data model
  * \param uiWidth       [input]: the initial width of the column
  * \param uiAlign       [input]: the alignment of the column
  * \param iFlags        [input]: list of style flags. See wxVirtualDataViewColumnFlags
  */
wxVirtualDataViewColumn::wxVirtualDataViewColumn(const wxString& sTitle,
                                                 size_t uiModelColumn,
                                                 size_t uiWidth,
                                                 wxAlignment uiAlign,
                                                 int iFlags)
    : m_uiModelColumn(uiModelColumn),
      m_sTitle(sTitle)
{
    Init(uiWidth, uiAlign, iFlags);
}

/** Constructor : the column display a bitmap
  * \param bitmap        [input]: the column bitmap to display
  * \param uiModelColumn [input]: the index of the corresponding column in the data model
  * \param uiWidth       [input]: the initial width of the column
  * \param uiAlign       [input]: the alignment of the column
  * \param iFlags        [input]: list of style flags. See wxVirtualDataViewColumnFlags
  */
wxVirtualDataViewColumn::wxVirtualDataViewColumn(const wxBitmap& bitmap,
                                                 size_t uiModelColumn,
                                                 size_t uiWidth,
                                                 wxAlignment uiAlign,
                                                 int iFlags)
    : m_uiModelColumn(uiModelColumn),
      m_Bitmap(bitmap)
{
    Init(uiWidth, uiAlign, iFlags);
}

/** Copy constructor
  */
wxVirtualDataViewColumn::wxVirtualDataViewColumn(const wxVirtualDataViewColumn &rhs)
    : m_uiModelColumn(0)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewColumn& wxVirtualDataViewColumn::operator=(const wxVirtualDataViewColumn &rhs)
{
    if (&rhs == this) return(*this);
    Release();
    Copy(rhs);
    return(*this);
}

/** Destructor
  */
wxVirtualDataViewColumn::~wxVirtualDataViewColumn(void)
{
    Release();
}

//----------------------- INTERNAL METHODS --------------------------//
/** Initialization
  * \param lWidth        [input]: the initial width of the column
  * \param uiAlign       [input]: the alignment of the column
  * \param iFlags        [input]: list of style flags. See wxVirtualDataViewColumnFlags
  */
void wxVirtualDataViewColumn::Init(long lWidth, wxAlignment iAlign, int iFlags)
{
    m_lWidth            = lWidth;
    m_lMinWidth         = 0;
    m_Alignment         = iAlign;
    m_iFlags            = iFlags;
    m_bSort             = false;
    m_bSortAscending    = true;
    m_bIsShown          = true;
    m_pAttributes       = WX_VDV_NULL_PTR;
    m_eType             = wxVDATAVIEW_COL_STRING;
}

/** Copy the column
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewColumn::Copy(const wxVirtualDataViewColumn &rhs)
{
    m_uiModelColumn     = rhs.m_uiModelColumn;
    m_Bitmap            = rhs.m_Bitmap;
    m_sTitle            = rhs.m_sTitle;
    m_lWidth            = rhs.m_lWidth;
    m_lMinWidth         = rhs.m_lMinWidth;
    m_Alignment         = rhs.m_Alignment;
    m_iFlags            = rhs.m_iFlags;
    m_bSort             = rhs.m_bSort;
    m_bSortAscending    = rhs.m_bSortAscending;
    m_bIsShown          = rhs.m_bIsShown;
    m_pAttributes       = WX_VDV_NULL_PTR;
    if (rhs.m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr(*rhs.m_pAttributes);
    m_eType             = rhs.m_eType;
}

/** Release the resources
  */
void wxVirtualDataViewColumn::Release(void)
{
    if (m_pAttributes) delete(m_pAttributes);
    m_pAttributes = WX_VDV_NULL_PTR;
}

//----------------------- MODEL COLUMN ------------------------------//
/** Set the model column
  * \param uiColumn [input] : the model column
  */
void wxVirtualDataViewColumn::SetModelColumn(size_t uiColumn)
{
    m_uiModelColumn = uiColumn;
}

/** Get the model column
  * \return the model column
  */
size_t wxVirtualDataViewColumn::GetModelColumn(void) const
{
    return(m_uiModelColumn);
}

//----------------------- BITMAP ------------------------------------//
/** Set the column bitmap
  * \param bitmap [input] : the new column bitmap
  */
void wxVirtualDataViewColumn::SetBitmap(const wxBitmap& bitmap)
{
    m_Bitmap = bitmap;
}

/** Get the column bitmap
  * \return the column bitmap
  */
wxBitmap wxVirtualDataViewColumn::GetBitmap(void) const
{
    return(m_Bitmap);
}

//------------------------ TITLE ------------------------------------//
/** Set the title
  * \param rsTitle [input] : the new title
  */
void wxVirtualDataViewColumn::SetTitle(const wxString &rsTitle)
{
    m_sTitle = rsTitle;
}

/** Get the tile
  * \return the column title
  */
wxString wxVirtualDataViewColumn::GetTitle(void) const
{
    return(m_sTitle);
}

//--------------- WIDTH & MIN WIDTH ---------------------------------//
/** Set the column width
  * \param iWidth [input] : the new column width
  */
void wxVirtualDataViewColumn::SetWidth(int iWidth)
{
    m_lWidth = iWidth;
}

/** Get the column VISIBLE width (0 if hidden)
  * \return the column width. It returns 0 if the column is hidden
  *         Use GetDesignWidth() to get the width even if it is hidden
  */
int wxVirtualDataViewColumn::GetWidth(void) const
{
    if (!m_bIsShown) return(0);
    return(GetDesignWidth());
}

/** Get the column DESIGN width (non-0 if hidden)
  * \return the column width.
  */
int wxVirtualDataViewColumn::GetDesignWidth(void) const
{
    switch(m_lWidth)
    {
        case wxVDATAVIEW_COL_DEFAULT_WIDTH  :   return(wxVDVC_DEFAULT_WIDTH);
//        case wxVDATAVIEW_COL_AUTOSIZE       :   return(m_pOwner->GetBestColumnWidth(m_pOwner->GetColumnIndex(this)));
        default                             :   break;
    }
    return(m_lWidth);
}

/** Set the minimal column width
  * \param iMinWidth [input] : the new minimal column width
  */
void wxVirtualDataViewColumn::SetMinWidth(int iMinWidth)
{
    m_lMinWidth = iMinWidth;
}

/** Get the minimal column width
  * \return the minimal column width
  */
int wxVirtualDataViewColumn::GetMinWidth(void) const
{
    return(m_lMinWidth);
}

//------------------------ ALIGNMENT --------------------------------//
/** Set the alignment
  * \param uiAlign [input]: the new alignment
  */
void wxVirtualDataViewColumn::SetAlignment(wxAlignment uiAlign)
{
    m_Alignment = uiAlign;
}

/** Get the alignment
  * \return the column alignment
  */
wxAlignment wxVirtualDataViewColumn::GetAlignment(void) const
{
    return(m_Alignment);
}

//-------------------------- VISIBLE --------------------------------//
/** Return true if the column is shown
  * \return true if the column is shown
  */
bool wxVirtualDataViewColumn::IsShown(void) const
{
    return(m_bIsShown);
}

/** Return true if the column is hidden
  * \return true if the column is hidden
  */
bool wxVirtualDataViewColumn::IsHidden(void) const
{
    return(!m_bIsShown);
}

/** Show/hide the column
  * \param bShow [input]: true for showing the column, false for hiding it
  */
void wxVirtualDataViewColumn::Show(bool bShow)
{
    m_bIsShown = bShow;
}

/** Hide/show the column
  * \param bHide [input]: true for hiding the column, false for showing it
  */
void wxVirtualDataViewColumn::Hide(bool bHide)
{
    m_bIsShown = !bHide;
}

/** Toggle the visibility ON/OFF
  */
void wxVirtualDataViewColumn::ToggleHideShow(void)
{
    m_bIsShown = !m_bIsShown;
}

//------------------ STYLE FLAGS ------------------------------------//
/** Set the column style flags
  * \param iFlags [input]: the new style flags
  */
void wxVirtualDataViewColumn::SetFlags(int iFlags)
{
    m_iFlags = iFlags;
}

/** Return the current column style flags
  * \return the current column style flags
  */
int wxVirtualDataViewColumn::GetFlags(void) const
{
    return(m_iFlags);
}

/** Check if the column is sortable
  * \return true if the column is sortable, false otherwise
  */
bool wxVirtualDataViewColumn::IsSortable(void) const
{
    return(m_iFlags & wxVDATAVIEW_COL_SORTABLE);
}

/** Check if the column can be reordered
  * \return true if the column can be reordered, false otherwise
  */
bool wxVirtualDataViewColumn::CanReorder(void) const
{
    return(m_iFlags & wxVDATAVIEW_COL_REORDERABLE);
}

/** Check if the column can be resized
  * \return true if the column can resized, false otherwise
  */
bool wxVirtualDataViewColumn::CanResize(void) const
{
    return(m_iFlags & wxVDATAVIEW_COL_RESIZABLE);
}

//----------------------- SORTING -----------------------------------//
/** Set if the column is used for sorting or not
  * \param bIsSortKey [input]: true if the column is used for sorting, false otherwise
  */
void wxVirtualDataViewColumn::SetSortKey(bool bIsSortKey)
{
    m_bSort = bIsSortKey;
}

/** Return true if the column is the one currently used for sorting
  * \return true if the column is the one currently used for sorting
  */
bool wxVirtualDataViewColumn::IsSortKey(void) const
{
    return(m_bSort);
}

/** For sortable columns indicate whether we should sort in ascending or
  * descending order (this should only be taken into account if IsSortKey())
  * \return true if the sorting is ascending
  */
bool wxVirtualDataViewColumn::IsSortOrderAscending(void) const
{
    return(m_bSortAscending);
}

/** Set the sorting order for this column
  * \param bAscending [input] : true if the sorting order is ascending
  */
void wxVirtualDataViewColumn::SetSortOrder(bool bAscending)
{
    m_bSortAscending = bAscending;
}

//---------------------- ATTRIBUTES ---------------------------------//
/** Set the column attributes
  * \param pAttr [input]: the new attributes. Ownership is taken.
  *                       It must have been allocated with "new"
  */
void wxVirtualDataViewColumn::SetAttribute(wxVirtualDataViewCellAttr *pAttr)
{
    if (m_pAttributes == pAttr) return;
    if (m_pAttributes) delete(m_pAttributes);
    m_pAttributes = pAttr;
}

/** Get the column attributes
  * \return the column attributes. Do not delete the object:
  * it is still owned by the class
  */
wxVirtualDataViewCellAttr* wxVirtualDataViewColumn::GetAttribute(void) const
{
    return(m_pAttributes);
}

//----------------- GET EDITOR & RENDERER ---------------------------//
/** Get the column editor
  * \return the column editor. May be NULL
  */
wxVirtualDataViewEditor* wxVirtualDataViewColumn::GetEditor(void)
{
    if (m_pAttributes) return(m_pAttributes->GetEditor());
    return(WX_VDV_NULL_PTR);
}

/** Get the column renderer
  * \return the column renderer. May be NULL
  */
wxVirtualDataViewRenderer* wxVirtualDataViewColumn::GetRenderer(void)
{
    if (m_pAttributes) return(m_pAttributes->GetRenderer());
    return(WX_VDV_NULL_PTR);
}

/** Get the column filter editor
  * \return the column filter editor. May be NULL
  */
wxVirtualDataViewIFilterEditor* wxVirtualDataViewColumn::GetFilterEditor(void)
{
    if (m_pAttributes) return(m_pAttributes->GetFilterEditor());
    return(WX_VDV_NULL_PTR);
}

/** Get the column filter
  * \return the column filter. May be NULL
  */
wxVirtualDataViewFilter* wxVirtualDataViewColumn::GetFilter(void)
{
    if (!m_pAttributes) return(WX_VDV_NULL_PTR);
    wxVirtualDataViewIFilterEditor *pFilterEditor = m_pAttributes->GetFilterEditor();
    if (pFilterEditor) return(pFilterEditor->GetFilter());
    return(WX_VDV_NULL_PTR);
}

//------------------- CELL FLAGS ------------------------------------//
/** Set cell flags
  * \param iFlags [input]: the new cell flags. Combination of ECellType
  */
void wxVirtualDataViewColumn::SetCellFlags(int iFlags)
{
    if (m_pAttributes) m_pAttributes->SetCellFlags(iFlags);
}

/** Get cell flags
  * \return the cell flags (combination of ECellType
  */
int wxVirtualDataViewColumn::GetCellFlags(void) const
{
    if (m_pAttributes) return(m_pAttributes->GetCellFlags());
    return(0);
}

/** Return true if the cell is read-only
  * \return true if the cell is read-only
  */
bool wxVirtualDataViewColumn::IsReadOnly(void) const
{
    if (m_pAttributes) return(m_pAttributes->IsReadOnly());
    return(true);
}

/** Return true if the cell is activatable
  * \return true if the cell is activatable
  */
bool wxVirtualDataViewColumn::IsActivatable(void) const
{
    if (m_pAttributes) return(m_pAttributes->IsActivatable());
    return(false);
}

/** Return true if the cell is editable
  * \return true if the cell is editable
  */
bool wxVirtualDataViewColumn::IsEditable(void) const
{
    if (m_pAttributes) return(m_pAttributes->IsEditable());
    return(false);
}

/** Set read-only / editable
  * \param bReadOnly [input]: if true, the cell is read-only
  *                           if false, the cell is read-write (editable)
  */
void wxVirtualDataViewColumn::SetReadOnly(bool bReadOnly)
{
    if (m_pAttributes) m_pAttributes->SetReadOnly(bReadOnly);
}

/** Set editable / read-only
  * \param bEditable [input]: if true, the cell is editable
  *                           if false, the cell is read-only
  */
void wxVirtualDataViewColumn::SetEditable(bool bEditable)
{
    if (m_pAttributes) m_pAttributes->SetEditable(bEditable);
}

/** Set / unset activatable
  * \param bActivatable [input]: if true, the cell can be edited with a simple click / return / space
  *                              if false, the cell cannot be edited with a simple click / return / space
  */
void wxVirtualDataViewColumn::SetActivatable(bool bActivatable)
{
    if (m_pAttributes) m_pAttributes->SetActivatable(bActivatable);
}

//-------------------------- TYPE -----------------------------------//
/** Get the type
  * \return the column type
  */
wxVirtualDataViewColumn::EType wxVirtualDataViewColumn::GetType(void) const
{
    return(m_eType);
}

/** Set the column type
  * \param eType [input]: the new type. It is just an indication.
  *                       Call SetFromType to automatically allocate renderer & editor for built-in types
  */
void wxVirtualDataViewColumn::SetType(EType eType)
{
    m_eType = eType;
}

//---------------------- DEFAULT TYPES ------------------------------//
/** Set the attributes from the type
  * \return true if the type could be recognized, false otherwise
  */
bool wxVirtualDataViewColumn::SetFromType(void)
{
    switch(m_eType)
    {
        case wxVDATAVIEW_COL_STRING             :   SetString();            return(true);
        case wxVDATAVIEW_COL_BITMAP             :   SetBitmap();            return(true);
        case wxVDATAVIEW_COL_BITMAPTEXT         :   SetBitmapText();        return(true);
        case wxVDATAVIEW_COL_PROGRESS           :   SetProgress();          return(true);
        case wxVDATAVIEW_COL_BOOL               :   SetBool();              return(true);
        case wxVDATAVIEW_COL_TRISTATE           :   SetTriState();          return(true);
        case wxVDATAVIEW_COL_INTEGER            :   SetInteger();           return(true);
        case wxVDATAVIEW_COL_UNSIGNED_INTEGER   :   SetUnsignedInteger();   return(true);
        case wxVDATAVIEW_COL_DOUBLE             :   SetDouble();            return(true);
        case wxVDATAVIEW_COL_DATE_TIME          :   SetDateTime();          return(true);
        case wxVDATAVIEW_COL_DATE               :   SetDate();              return(true);
        case wxVDATAVIEW_COL_TIME               :   SetTime();              return(true);
        case wxVDATAVIEW_COL_COLOUR             :   SetColour();            return(true);
        case wxVDATAVIEW_COL_UNKNOWN            :
        default                                 :   break;
    }

    return(false);
}

/** Set the attribute as string
  */
void wxVirtualDataViewColumn::SetString(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetString();
}

/** Set the attribute as a bitmap
  */
void wxVirtualDataViewColumn::SetBitmap(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetBitmap();
}

/** Set the attribute as a bitmap + text
  */
void wxVirtualDataViewColumn::SetBitmapText(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetBitmapText();
}

/** Set the attribute as a progress bar
  * \param iMin         [input]: the minimal value
  * \param iMax         [input]: the maximal value
  * \param bDrawValue   [input]: true if the text value is drawn on top of the progress bar
  */
void wxVirtualDataViewColumn::SetProgress(int iMin, int iMax, bool bDrawValue)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetProgress(iMin, iMax, bDrawValue);
}

/** Set the attribute as boolean (check-box)
  */
void wxVirtualDataViewColumn::SetBool(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetBool();
}

/** Set the attribute as a tri state checkbox
  */
void wxVirtualDataViewColumn::SetTriState(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetTriState();
}

/** Set the attribute as integer
  */
void wxVirtualDataViewColumn::SetInteger(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetInteger();
}

/** Set the attribute as unsigned integer
  */
void wxVirtualDataViewColumn::SetUnsignedInteger(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetUnsignedInteger();
}

/** Set the attribute as double
  */
void wxVirtualDataViewColumn::SetDouble(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetDouble();
}

/** Set the attribute as date-time
  */
void wxVirtualDataViewColumn::SetDateTime(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetDateTime();
}

/** Set the attribute as date
  */
void wxVirtualDataViewColumn::SetDate(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetDate();
}

/** Set the attribute as time
  */
void wxVirtualDataViewColumn::SetTime(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetTime();
}

/** Set the attribute as colour
  */
void wxVirtualDataViewColumn::SetColour(void)
{
    if (!m_pAttributes) m_pAttributes = new wxVirtualDataViewCellAttr;
    if (m_pAttributes) m_pAttributes->SetColour();
}
