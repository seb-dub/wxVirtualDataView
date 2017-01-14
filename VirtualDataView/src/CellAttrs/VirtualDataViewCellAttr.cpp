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
      m_iFlags(WX_CELL_READ_ONLY)
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
      m_iFlags(WX_CELL_READ_ONLY)
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

    if (rhs.m_pRenderer) m_pRenderer = rhs.m_pRenderer->Clone();
    else                 m_pRenderer = WX_VDV_NULL_PTR;

    if (rhs.m_pEditor)   m_pEditor   = rhs.m_pEditor->Clone();
    else                 m_pEditor   = WX_VDV_NULL_PTR;

    if (rhs.m_pFilterEditor)    m_pFilterEditor   = rhs.m_pFilterEditor->Clone();
    else                        m_pFilterEditor   = WX_VDV_NULL_PTR;

    if (rhs.m_pAttr)     m_pAttr     = new wxVirtualDataViewItemAttr(*rhs.m_pAttr);
    else                 m_pAttr     = WX_VDV_NULL_PTR;
}

/** Release the object
  */
void wxVirtualDataViewCellAttr::Release(void)
{
    if (m_pRenderer) delete(m_pRenderer);
    m_pRenderer = WX_VDV_NULL_PTR;

    if (m_pEditor) delete(m_pEditor);
    m_pEditor = WX_VDV_NULL_PTR;

    if (m_pFilterEditor) delete(m_pFilterEditor);
    m_pFilterEditor = WX_VDV_NULL_PTR;

    if (m_pAttr) delete(m_pAttr);
    m_pAttr = WX_VDV_NULL_PTR;
}

//------------------------------- SETTERS ---------------------------//
/** Set the type
  * \param sType [input] : the new type
  */
void wxVirtualDataViewCellAttr::SetType(wxString sType)
{
    m_sType = sType;
}

/** Set cell flags
  * \param iFlags [input]: the new cell flags. Combination of ECellType
  */
void wxVirtualDataViewCellAttr::SetCellFlags(int iFlags)
{
    m_iFlags = iFlags;
}

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
    if (m_pRenderer) delete(m_pRenderer);
    m_pRenderer = pRenderer;
}

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
    if (m_pEditor) delete(m_pEditor);
    m_pEditor = pEditor;
}

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
    if (m_pFilterEditor) delete(m_pFilterEditor);
    m_pFilterEditor = pFilterEditor;
}

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
    if (m_pAttr) delete(m_pAttr);
    m_pAttr = pAttr;
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

//--------------------- DEFAULT TYPES -------------------------------//
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
