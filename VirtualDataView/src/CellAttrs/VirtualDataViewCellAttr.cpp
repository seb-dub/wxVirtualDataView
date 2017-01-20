/**********************************************************************/
/** FILE    : VirtualDataViewCellAttr.cpp                            **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view cell attribute                             **/
/**********************************************************************/


#include <wx/VirtualDataView/CellAttrs/VirtualDataViewCellAttr.h>
#include <wx/VirtualDataView/CellAttrs/VirtualDataViewItemAttr.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewTextRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewBitmapRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewBitmapTextRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewCheckBoxRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewNumberRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewDateRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewTimeRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewDateTimeRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewColourRenderer.h>
#include <wx/VirtualDataView/Renderers/VirtualDataViewProgressRenderer.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewTextEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewCheckBoxEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewNumberEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewDateEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewTimeEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewDateTimeEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewColourEditor.h>
#include <wx/VirtualDataView/Editors/VirtualDataViewProgressEditor.h>
#include <wx/VirtualDataView/FilterEditors/VirtualDataViewIFilterEditor.h>
#include <wx/VirtualDataView/FilterEditors/VirtualDataViewStdFilterEditor.h>

//---------------------- CONSTRUCTORS & DESTRUCTOR ------------------//
/** Default constructor
  * \param sType         [input]: the type name (identify a wxVariant type)
  *                               default = "string"
  * \param pRenderer     [input]: the renderer (default = NULL)
  * \param pEditor       [input]: the editor (default = NULL)
  * \param pAttr         [input]: the default graphic attributes (default = NULL)
  * \param pFilterEditor [input]: the filter editor
  */
wxVirtualDataViewCellAttr::wxVirtualDataViewCellAttr(wxString sType,
                                                     wxVirtualDataViewRenderer* pRenderer,
                                                     wxVirtualDataViewEditor* pEditor,
                                                     wxVirtualDataViewItemAttr *pAttr,
                                                     wxVirtualDataViewIFilterEditor *pFilterEditor)
    : m_sType(sType),
      m_pRenderer(pRenderer),
      m_pEditor(pEditor),
      m_pFilterEditor(pFilterEditor),
      m_pAttr(pAttr),
      m_iFlags(WX_CELL_READ_ONLY),
      m_bOwnRenderer(true),
      m_bOwnEditor(true),
      m_bOwnFilterEditor(true),
      m_bOwnAttributes(true)
{
}

/** Copy constructor
  */
wxVirtualDataViewCellAttr::wxVirtualDataViewCellAttr(const wxVirtualDataViewCellAttr &rhs)
    : m_sType(rhs.m_sType),
      m_pRenderer(WX_VDV_NULL_PTR),
      m_pEditor(WX_VDV_NULL_PTR),
      m_pFilterEditor(WX_VDV_NULL_PTR),
      m_pAttr(WX_VDV_NULL_PTR),
      m_iFlags(WX_CELL_READ_ONLY),
      m_bOwnRenderer(true),
      m_bOwnEditor(true),
      m_bOwnFilterEditor(true),
      m_bOwnAttributes(true)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewCellAttr& wxVirtualDataViewCellAttr::operator=(const wxVirtualDataViewCellAttr &rhs)
{
    if (&rhs == this) return(*this);
    Release();
    Copy(rhs);
    return(*this);
}

/** Destructor
  */
wxVirtualDataViewCellAttr::~wxVirtualDataViewCellAttr(void)
{
    Release();
}

//---------------------- INTERNAL -----------------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewCellAttr::Copy(const wxVirtualDataViewCellAttr &rhs)
{
    m_sType = rhs.m_sType;
    m_iFlags = rhs.m_iFlags;

    CopyRenderer(rhs);
    CopyEditor(rhs);
    CopyFilterEditor(rhs);
    CopyGraphicAttributes(rhs);
}

/** Release the object
  */
void wxVirtualDataViewCellAttr::Release(void)
{
    ReleaseRenderer();
    ReleaseEditor();
    ReleaseFilterEditor();
    ReleaseGraphicAttributes();
}

//-------------------- RENDERER -------------------------------------//
/** Set the renderer
  * \param pRenderer [input]: the new renderer
  *                           Ownership is taken
  *                           It must have been allocated with "new"
  *                           Previous renderer is deleted.
  *                           If old and new objects are same, nothing is done
  */
void wxVirtualDataViewCellAttr::SetRenderer(wxVirtualDataViewRenderer *pRenderer)
{
    if (pRenderer == m_pRenderer) return;
    ReleaseRenderer();
    m_pRenderer = pRenderer;
}

/** Release the renderer
  */
void wxVirtualDataViewCellAttr::ReleaseRenderer(void)
{
    if ((m_pRenderer) && (m_bOwnRenderer)) delete(m_pRenderer);
    m_pRenderer = WX_VDV_NULL_PTR;
}

/** Copy the renderer
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewCellAttr::CopyRenderer(const wxVirtualDataViewCellAttr &rhs)
{
    if (!rhs.m_bOwnRenderer)
    {
        m_pRenderer = rhs.m_pRenderer;
        m_bOwnRenderer = false;
        return;
    }

    if (rhs.m_pRenderer) m_pRenderer = rhs.m_pRenderer->Clone();
    else                 m_pRenderer = WX_VDV_NULL_PTR;
    m_bOwnRenderer = true;
}

/** Check if the renderer is owned by the object
  * \return true if the renderer is owned by this object, false otherwise
  * If it is owned, the destructor will call "delete" on the renderer
  */
bool wxVirtualDataViewCellAttr::IsOwnRenderer(void) const
{
    return(m_bOwnRenderer);
}

/** Take ownership of the renderer
  * The destructor will call "delete" on the renderer
  */
void wxVirtualDataViewCellAttr::TakeRendererOwnership(void)
{
    m_bOwnRenderer = true;
}

/** Release ownership of the renderer
  * The destructor will do nothing with the renderer
  */
void wxVirtualDataViewCellAttr::ReleaseRendererOwnership(void)
{
    m_bOwnRenderer = false;
}

//-------------------- EDITOR ---------------------------------------//
/** Set the editor
  * \param pEditor [input]: the new editor
  *                         Ownership is taken
  *                         It must have been allocated with "new"
  *                         Previous editor is deleted.
  *                         If old and new objects are same, nothing is done
  */
void wxVirtualDataViewCellAttr::SetEditor(wxVirtualDataViewEditor *pEditor)
{
    if (pEditor == m_pEditor) return;
    ReleaseEditor();
    m_pEditor = pEditor;
}

/** Release the editor
  */
void wxVirtualDataViewCellAttr::ReleaseEditor(void)
{
    if ((m_pEditor) && (m_bOwnEditor)) delete(m_pEditor);
    m_pEditor = WX_VDV_NULL_PTR;
}

/** Copy the editor
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewCellAttr::CopyEditor(const wxVirtualDataViewCellAttr &rhs)
{
    if (!rhs.m_bOwnEditor)
    {
        m_pEditor = rhs.m_pEditor;
        m_bOwnEditor = false;
        return;
    }

    if (rhs.m_pEditor) m_pEditor = rhs.m_pEditor->Clone();
    else               m_pEditor = WX_VDV_NULL_PTR;
    m_bOwnEditor = true;
}

/** Check if the editor is owned by the object
  * \return true if the editor is owned by this object, false otherwise
  * If it is owned, the destructor will call "delete" on the editor
  */
bool wxVirtualDataViewCellAttr::IsOwnEditor(void) const
{
    return(m_bOwnEditor);
}

/** Take ownership of the editor
  * The destructor will call "delete" on the editor
  */
void wxVirtualDataViewCellAttr::TakeEditorOwnership(void)
{
    m_bOwnEditor = true;
}

/** Release ownership of the editor
  * The destructor will do nothing with the editor
  */
void wxVirtualDataViewCellAttr::ReleaseEditorOwnership(void)
{
    m_bOwnEditor = false;
}

//--------------------- FILTER EDITOR -------------------------------//
/** Set the filter editor
  * \param pFilterEditor [input]: the new filter
  *                               Ownership is taken
  *                               It must have been allocated with "new"
  *                               Previous editor is deleted.
  *                               If old and new objects are same, nothing is done
  */
void wxVirtualDataViewCellAttr::SetFilterEditor(wxVirtualDataViewIFilterEditor *pFilterEditor)
{
    if (pFilterEditor == m_pFilterEditor) return;
    ReleaseFilterEditor();
    m_pFilterEditor = pFilterEditor;
}

/** Release the filter editor
  */
void wxVirtualDataViewCellAttr::ReleaseFilterEditor(void)
{
    if ((m_pFilterEditor) && (m_bOwnFilterEditor)) delete(m_pFilterEditor);
    m_pFilterEditor = WX_VDV_NULL_PTR;
}

/** Copy the filter editor
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewCellAttr::CopyFilterEditor(const wxVirtualDataViewCellAttr &rhs)
{
    if (!rhs.m_bOwnFilterEditor)
    {
        m_pFilterEditor = rhs.m_pFilterEditor;
        m_bOwnFilterEditor = false;
        return;
    }

    if (rhs.m_pFilterEditor) m_pFilterEditor = rhs.m_pFilterEditor->Clone();
    else                     m_pFilterEditor = WX_VDV_NULL_PTR;
    m_bOwnFilterEditor = true;
}

/** Check if the filter editor is owned by the object
  * \return true if the filter editor is owned by this object, false otherwise
  * If it is owned, the destructor will call "delete" on the filter editor
  */
bool wxVirtualDataViewCellAttr::IsOwnFilterEditor(void) const
{
    return(m_bOwnFilterEditor);
}

/** Take ownership of the filter editor
  * The destructor will call "delete" on the filter editor
  */
void wxVirtualDataViewCellAttr::TakeFilterEditorOwnership(void)
{
    m_bOwnFilterEditor = true;
}

/** Release ownership of the filter editor
  * The destructor will do nothing with the filter editor
  */
void wxVirtualDataViewCellAttr::ReleaseFilterEditorOwnership(void)
{
    m_bOwnFilterEditor = false;
}

//-------------------- GRAPHIC ATTRIBUTES ---------------------------//
/** Set the graphic attributes
  * \param pAttr [input]: the new graphic attributes
  *                       Ownership is taken
  *                       It must have been allocated with "new"
  *                       Previous graphic attributes is deleted.
  *                       If old and new objects are same, nothing is done
  */
void wxVirtualDataViewCellAttr::SetGraphicAttributes(wxVirtualDataViewItemAttr *pAttr)
{
    if (pAttr == m_pAttr) return;
    ReleaseGraphicAttributes();
    m_pAttr = pAttr;
}

/** Release the graphic attributes
  */
void wxVirtualDataViewCellAttr::ReleaseGraphicAttributes(void)
{
    if ((m_pAttr) && (m_bOwnAttributes)) delete(m_pAttr);
    m_pAttr = WX_VDV_NULL_PTR;
}

/** Copy the graphic attributes
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewCellAttr::CopyGraphicAttributes(const wxVirtualDataViewCellAttr &rhs)
{
    if (!rhs.m_bOwnAttributes)
    {
        m_pAttr = rhs.m_pAttr;
        m_bOwnAttributes = false;
        return;
    }

    if (rhs.m_pAttr) m_pAttr = new wxVirtualDataViewItemAttr(*rhs.m_pAttr);
    else             m_pAttr = WX_VDV_NULL_PTR;
    m_bOwnAttributes = true;
}

/** Check if the graphic attributes is owned by the object
  * \return true if the graphic attributes is owned by this object, false otherwise
  * If it is owned, the destructor will call "delete" on the graphic attributes
  */
bool wxVirtualDataViewCellAttr::IsOwnGraphicAttributes(void) const
{
    return(m_bOwnAttributes);
}

/** Take ownership of the graphic attributes
  * The destructor will call "delete" on the graphic attributes
  */
void wxVirtualDataViewCellAttr::TakeGraphicAttributesOwnership(void)
{
    m_bOwnAttributes = true;
}

/** Release ownership of the graphic attributes
  * The destructor will do nothing with the graphic attributes
  */
void wxVirtualDataViewCellAttr::ReleaseGraphicAttributesOwnership(void)
{
    m_bOwnAttributes = false;
}

//------------------------------- FLAGS -----------------------------//
/** Set cell flags
  * \param iFlags [input]: the new cell flags. Combination of ECellType
  */
void wxVirtualDataViewCellAttr::SetCellFlags(int iFlags)
{
    m_iFlags = iFlags;
}

/** Set read-only / editable
  * \param bReadOnly [input]: if true, the cell is read-only
  *                           if false, the cell is read-write (editable)
  */
void wxVirtualDataViewCellAttr::SetReadOnly(bool bReadOnly)
{
    if (bReadOnly) m_iFlags &= ~(WX_CELL_EDITABLE);
    else           m_iFlags |=   WX_CELL_EDITABLE;
}

/** Set editable / read-only
  * \param bEditable [input]: if true, the cell is editable
  *                           if false, the cell is read-only
  */
void wxVirtualDataViewCellAttr::SetEditable(bool bEditable)
{
    if (bEditable) m_iFlags |=   WX_CELL_EDITABLE;
    else           m_iFlags &= ~(WX_CELL_EDITABLE);
}

/** Set / unset activatable
  * \param bActivatable [input]: if true, the cell can be edited with a simple click / return / space
  *                              if false, the cell cannot be edited with a simple click / return / space
  */
void wxVirtualDataViewCellAttr::SetActivatable(bool bActivatable)
{
    if (bActivatable) m_iFlags |=   WX_CELL_ACTIVATABLE;
    else              m_iFlags &= ~(WX_CELL_ACTIVATABLE);
}

//--------------------- TYPES ---------------------------------------//
/** Set the type
  * \param sType [input] : the new type
  */
void wxVirtualDataViewCellAttr::SetType(wxString sType)
{
    m_sType = sType;
}

/** Set the attributes from the type
  * \return true if the type could be recognized
  *         false otherwise
  */
bool wxVirtualDataViewCellAttr::SetFromType(void)
{
    if (m_sType == "string")        {SetString();           return(true);}
    if (m_sType == "bitmap")        {SetBitmap();           return(true);}
    if (m_sType == "bitmaptext")    {SetBitmapText();       return(true);}
    if (m_sType == "progress")      {SetProgress();         return(true);}
    if (m_sType == "bool")          {SetBool();             return(true);}
    if (m_sType == "tristate")      {SetTriState();         return(true);}
    if (m_sType == "integer")       {SetInteger();          return(true);}
    if (m_sType == "uinteger")      {SetUnsignedInteger();  return(true);}
    if (m_sType == "double")        {SetDouble();           return(true);}
    if (m_sType == "datetime")      {SetDateTime();         return(true);}
    if (m_sType == "date")          {SetDate();             return(true);}
    if (m_sType == "time")          {SetTime();             return(true);}
    if (m_sType == "colour")        {SetColour();           return(true);}
    return(false);
}

/** Set the attribute as string
  */
void wxVirtualDataViewCellAttr::SetString(void)
{
    m_sType = "string";
    SetRenderer(new wxVirtualDataViewTextRenderer());
    SetEditor(new wxVirtualDataViewTextEditor());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as a bitmap
  */
void wxVirtualDataViewCellAttr::SetBitmap(void)
{
    m_sType = "bitmap";
    SetRenderer(new wxVirtualDataViewBitmapRenderer());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as a bitmap + text
  */
void wxVirtualDataViewCellAttr::SetBitmapText(void)
{
    m_sType = "bitmaptext";
    SetRenderer(new wxVirtualDataViewBitmapTextRenderer());
    SetEditor(new wxVirtualDataViewTextEditor());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as a progress bar
  * \param iMin         [input]: the minimal value
  * \param iMax         [input]: the maximal value
  * \param bDrawValue   [input]: true if the text value is drawn on top of the progress bar
  */
void wxVirtualDataViewCellAttr::SetProgress(int iMin, int iMax, bool bDrawValue)
{
    m_sType = "progress";
    wxVirtualDataViewProgressRenderer *pRenderer = new wxVirtualDataViewProgressRenderer();
    pRenderer->SetRange(iMin, iMax);
    pRenderer->SetValueIsDrawn(bDrawValue);
    SetRenderer(pRenderer);

    wxVirtualDataViewProgressEditor *pEditor = new wxVirtualDataViewProgressEditor();
    pEditor->SetValueIsDrawn(bDrawValue);
    pEditor->SetRange(iMin, iMax);
    SetEditor(pEditor);

    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as boolean (check-box)
  */
void wxVirtualDataViewCellAttr::SetBool(void)
{
    m_sType = "bool";
    SetRenderer(new wxVirtualDataViewCheckBoxRenderer(false));
    SetEditor(new wxVirtualDataViewCheckBoxEditor(false));
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as a tri state checkbox
  */
void wxVirtualDataViewCellAttr::SetTriState(void)
{
    m_sType = "tristate";
    SetRenderer(new wxVirtualDataViewCheckBoxRenderer(true));
    SetEditor(new wxVirtualDataViewCheckBoxEditor(true));
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as integer
  */
void wxVirtualDataViewCellAttr::SetInteger(void)
{
    m_sType = "integer";

    wxVirtualDataViewNumberRenderer *pRen = new wxVirtualDataViewNumberRenderer(false);
    pRen->SetSigned();
    SetRenderer(pRen);

    wxVirtualDataViewNumberEditor *pEditor = new wxVirtualDataViewNumberEditor();
    pEditor->SetSpinControl(true);
    pEditor->SetInteger(true);
    pEditor->SetRangeAuto();
    SetEditor(pEditor);

    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as unsigned integer
  */
void wxVirtualDataViewCellAttr::SetUnsignedInteger(void)
{
    m_sType = "uinteger";

    wxVirtualDataViewNumberRenderer *pRen = new wxVirtualDataViewNumberRenderer(false);
    pRen->SetUnsigned();
    SetRenderer(pRen);

    wxVirtualDataViewNumberEditor *pEditor = new wxVirtualDataViewNumberEditor();
    pEditor->SetSpinControl(true);
    pEditor->SetInteger(true);
    pEditor->SetRangeAuto();
    long lMin, lMax;
    pEditor->GetRange(lMin, lMax);
    lMin = 0;
    pEditor->SetRange(lMin, lMax);
    SetEditor(pEditor);


    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as double
  */
void wxVirtualDataViewCellAttr::SetDouble(void)
{
    m_sType = "double";

    wxVirtualDataViewNumberRenderer *pRen = new wxVirtualDataViewNumberRenderer(true);
    SetRenderer(pRen);

    wxVirtualDataViewNumberEditor *pEditor = new wxVirtualDataViewNumberEditor();
    pEditor->SetSpinControl(true);
    pEditor->SetFloatingPoint(true);
    pEditor->SetRangeAuto();
    SetEditor(pEditor);

    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as date-time
  */
void wxVirtualDataViewCellAttr::SetDateTime(void)
{
    m_sType = "datetime";
    SetRenderer(new wxVirtualDataViewDateTimeRenderer());
    SetEditor(new wxVirtualDataViewDateTimeEditor());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as date
  */
void wxVirtualDataViewCellAttr::SetDate(void)
{
    m_sType = "date";
    SetRenderer(new wxVirtualDataViewDateRenderer());
    SetEditor(new wxVirtualDataViewDateEditor());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as time
  */
void wxVirtualDataViewCellAttr::SetTime(void)
{
    m_sType = "time";
    SetRenderer(new wxVirtualDataViewTimeRenderer());
    SetEditor(new wxVirtualDataViewTimeEditor());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}

/** Set the attribute as colour
  */
void wxVirtualDataViewCellAttr::SetColour(void)
{
    m_sType = "colour";
    SetRenderer(new wxVirtualDataViewColourRenderer());
    SetEditor(new wxVirtualDataViewColourEditor());
    SetFilterEditor(new wxVirtualDataViewStdFilterEditor());
}
