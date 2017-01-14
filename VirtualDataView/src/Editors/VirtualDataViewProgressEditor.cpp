/**********************************************************************/
/** FILE    : VirtualDataViewProgressEditor.cpp                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view progress editor                            **/
/**********************************************************************/

#include <wx/VirtualDataView/Editors/VirtualDataViewProgressEditor.h>

#define WX_USE_NATIVE_SLIDER        0

#include <wx/slider.h>
#if WX_USE_NATIVE_SLIDER != 0

    #define SLIDER      wxSlider

#else

    #include <wx/dcclient.h>
    #include <wx/renderer.h>


    #define SLIDER      wxProgressSlider

    class wxProgressSlider : public wxControl
    {
        public:
            wxProgressSlider(wxWindow *pParent, wxWindowID id, int iValue, int iMin, int iMax,
                             const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
                             int iFlags = 0)
                : wxControl(pParent, id, pos, size, 0),
                  m_iMin(iMin), m_iMax(iMax), m_bDrawLabel(false)
            {
                if (iFlags & wxSL_VALUE_LABEL) m_bDrawLabel = true;

                Bind(wxEVT_PAINT             , &wxProgressSlider::OnPaint       , this);
                Bind(wxEVT_LEFT_DOWN         , &wxProgressSlider::OnMouseDown   , this);
                Bind(wxEVT_LEFT_UP           , &wxProgressSlider::OnMouseUp     , this);
                Bind(wxEVT_MOTION            , &wxProgressSlider::OnMouseMove   , this);
                Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxProgressSlider::OnCaptureLost , this);
            }

            ~wxProgressSlider(void)
            {
            }

            int  GetValue(void) const {return(m_iValue);}
            void SetValue(int iValue) {m_iValue = iValue;}

            void SetRange(int iMin, int iMax) {m_iMin = iMin; m_iMax = iMax;}
            void ComputeValue(int x);

        protected:
            //data
            int     m_iMin;
            int     m_iMax;
            int     m_iValue;
            bool    m_bDrawLabel;

            //methods
            void OnMouseDown(wxMouseEvent &rEvent);
            void OnMouseUp(wxMouseEvent &rEvent);
            void OnMouseMove(wxMouseEvent &rEvent);
            void OnPaint(wxPaintEvent &rEvent);
            void Draw(wxDC &rDC);
            void OnCaptureLost(wxMouseCaptureLostEvent &rEvent);
    };

    //------------------ PROGESS WIDGETS --------------------------------//
    void wxProgressSlider::OnMouseDown(wxMouseEvent &rEvent)
    {
        CaptureMouse();
        ComputeValue(rEvent.GetX());
    }

    void wxProgressSlider::OnMouseUp(wxMouseEvent &rEvent)
    {
        ComputeValue(rEvent.GetX());
        ReleaseMouse();
    }

    void wxProgressSlider::OnMouseMove(wxMouseEvent &rEvent)
    {
        if (!HasCapture())
        {
            rEvent.Skip();
            return;
        }
        ComputeValue(rEvent.GetX());
    }

    void wxProgressSlider::OnPaint(wxPaintEvent &rEvent)
    {
        wxPaintDC oDC(this);
        Draw(oDC);
    }

    void wxProgressSlider::Draw(wxDC &rDC)
    {
        wxRect oRect = GetClientRect();

        int iValue = m_iValue;
        iValue -= m_iMin;
        if (iValue < 0) iValue = 0;
        if (iValue > m_iMax - m_iMin) iValue = m_iMax - m_iMin;

        wxRendererNative &rRenderer = wxRendererNative::Get();
        rRenderer.DrawGauge(this, rDC, oRect, m_iValue, m_iMax - m_iMin);

        if (m_bDrawLabel)
        {
            wxString sText = wxString::Format("%d", m_iValue);
            rRenderer.DrawItemText(this, rDC, sText, oRect, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL, 0, wxELLIPSIZE_NONE);
        }

        //draw cursor
        wxPen oPen(*wxBLACK, 1, wxPENSTYLE_SOLID);
        int iWidth = oRect.GetWidth();
        int x      = m_iValue - m_iMin;
        x *= iWidth;
        x /= (m_iMax - m_iMin);
        rDC.DrawLine(x, 0, x, oRect.GetHeight());
    }


    void wxProgressSlider::OnCaptureLost(wxMouseCaptureLostEvent &rEvent)
    {

    }

    void wxProgressSlider::ComputeValue(int x)
    {
        wxSize s = GetClientSize();
        int iWidth = s.GetWidth();
        int iValue = x;

        if (iValue < 0)
        {
            m_iValue = m_iMin;
        }
        else if (iValue > iWidth - 1)
        {
            m_iValue = m_iMax;
        }
        else
        {
            int iRange = m_iMax - m_iMin;
            if (iRange <= 0) iRange = 1;

            iValue = (iValue * iRange) / iWidth + m_iMin;
            m_iValue = iValue;
        }

        wxClientDC oDC(this);
        Draw(oDC);
    }

#endif // WX_USE_NATIVE_SLIDER

//------------------ CONSTRUCTORS & DESTRUCTOR ----------------------//
/** Default constructor
  */
wxVirtualDataViewProgressEditor::wxVirtualDataViewProgressEditor(void)
    : wxVirtualDataViewEditor(),
      m_pEditor(WX_VDV_NULL_PTR),
      m_iMin(0), m_iMax(100),
      m_bDrawValue(true)
{

}

/** Copy constructor
  * \param rhs [input]: the object to copy
  */
wxVirtualDataViewProgressEditor::wxVirtualDataViewProgressEditor(const wxVirtualDataViewProgressEditor &rhs)
    : wxVirtualDataViewEditor(),
      m_pEditor(WX_VDV_NULL_PTR)
{
    Copy(rhs);
}

/** Assignation operator
  */
wxVirtualDataViewProgressEditor& wxVirtualDataViewProgressEditor::operator=(const wxVirtualDataViewProgressEditor &rhs)
{
    if (this == &rhs) return(*this);
    Copy(rhs);
    return(*this);
}

/** Destructor - virtual
  */
wxVirtualDataViewProgressEditor::~wxVirtualDataViewProgressEditor(void)
{

}

//--------------------- INTERNAL METHODS ----------------------------//
/** Copy an object
  * \param rhs [input]: the object to copy
  */
void wxVirtualDataViewProgressEditor::Copy(const wxVirtualDataViewProgressEditor &rhs)
{
    m_iMin          = rhs.m_iMin;
    m_iMax          = rhs.m_iMax;
    m_bDrawValue    = rhs.m_bDrawValue;
}

//------------------- INTERFACE IMPLEMENTATION ----------------------//
/** Clone the editor
  * \return a new object, allocated with "new"
  */
wxVirtualDataViewEditor* wxVirtualDataViewProgressEditor::Clone(void) const
{
    return(new wxVirtualDataViewProgressEditor(*this));
}

/** Start editing
  * \param rID            [input]: the ID of the item to edit
  * \param uiField        [input]: the item field to edit
  * \param rvInitialValue [input]: the initial value of the item (for editor initialization)
  * \param pParent        [input]: the parent window of the editor
  * \param rRect          [input]: the placement of the editor in the parent window
  * \return true on success, false on failure
  */
bool wxVirtualDataViewProgressEditor::DoStartEditing(const wxVirtualItemID &rID,
                                                     size_t uiField,
                                                     const wxVariant &rvInitialValue,
                                                     wxWindow *pParent,
                                                     wxRect rRect)
{
    int iValue = rvInitialValue.GetInteger();

    if (!m_pEditor)
    {
        int iFlags = wxHORIZONTAL;
        if (m_bDrawValue) iFlags |= wxSL_VALUE_LABEL;
        m_pEditor = new SLIDER(pParent, wxNewId(), iValue, m_iMin, m_iMax,
                               rRect.GetLeftTop(), rRect.GetSize(), iFlags);
        if (!m_pEditor) return(false);

        m_pEditor->Show();
    }

    SLIDER *pSlider = (SLIDER*) m_pEditor;
    pSlider->SetRange(m_iMin, m_iMax);
    pSlider->SetSize(rRect.GetSize());
    pSlider->SetValue(iValue);

    return(true);
}

/** Answer to a single click/space/return
  * \param rID            [input]: the ID of the item to activate
  * \param uiField        [input]: the item field to activate
  * \param rRect          [input]: rectangle of the cell
  * \param ptPos          [input]: the relative position of the mouse event.
  *                                Negative coord indicates keyboard event.
  * \param rInOutValue    [input/output]: on input, the current value of the item
  *                                       on output, the new value of the item
  *                                       The item is modified only if true is returned
  * \return true if the item was modified, false otherwise
  */
bool wxVirtualDataViewProgressEditor::Activate(const wxVirtualItemID &rID, size_t uiField,
                                               wxRect rRect, wxPoint ptPos, wxVariant &rInOutValue)
{
//    int iValue = rInOutValue.GetInteger();
    if (ptPos.x < 0) return(false);
    int iValue = ptPos.x;

    int iWidth = rRect.GetWidth();

    if (iValue < 0)
    {
        iValue = m_iMin;
    }
    else if (iValue > iWidth - 1)
    {
        iValue = m_iMax;
    }
    else
    {
        int iRange = m_iMax - m_iMin;
        if (iRange <= 0) iRange = 1;

        iValue = (iValue * iRange) / iWidth + m_iMin;
        iValue = iValue;
    }
    rInOutValue = (long) iValue;
    return(true);
}

/** Test if edition can be finished
  * \return true if edition can be finished,
  *         false otherwise (vetoed, because of wrong value)
  */
bool wxVirtualDataViewProgressEditor::CanFinishEdition(void)
{
    return(true);
}

/** Finish the edition (destroy editor)
  * \return true on success, false on failure
  */
bool wxVirtualDataViewProgressEditor::FinishEdition(void)
{
    if (!m_pEditor) return(true);
    DestroyEditor(m_pEditor);
    m_pEditor = WX_VDV_NULL_PTR;
    return(true);
}

/** Get the value
  * \param rValue [output]: the value of the item
  * \return true if a value was put in rValue
  *         false otherwise
  */
bool wxVirtualDataViewProgressEditor::GetValue(wxVariant &rvValue)
{
    if (!m_pEditor) return(false);

    SLIDER *pSlider = (SLIDER*) m_pEditor;
    rvValue = (long) pSlider->GetValue();
    return(true);
}

/** Get the editor window
  * \return the editor window
  */
wxWindow* wxVirtualDataViewProgressEditor::GetEditorWindow(void)
{
    return(m_pEditor);
}

//------------------ RANGE ------------------------------------------//
/** Set the valid range of the progress bar
  * \param iMin [input]: the new minimal value of the progress bar
  * \param iMax [input]: the new maximal value of the progress bar
  */
void wxVirtualDataViewProgressEditor::SetRange(int iMin, int iMax)
{
    m_iMin = iMin;
    m_iMax = iMax;

    //check & correct
    if (m_iMin > m_iMax)
    {
        int iTemp = m_iMin;
        m_iMin = m_iMax;
        m_iMax = iTemp;
    }
    if (m_iMin == m_iMax) m_iMax++;
}

/** Get the valid range of the progress bar
  * \param iMin [output]: the minimal value of the progress bar
  * \param iMax [output]: the maximal value of the progress bar
  */
void wxVirtualDataViewProgressEditor::GetRange(int &iMin, int &iMax) const
{
    iMin = m_iMin;
    iMax = m_iMax;
}

//----------------- DRAW VALUE --------------------------------------//
/** Check if the value is drawn as text
  * \return true if the text value is drawn on top of the progress bar
  */
bool wxVirtualDataViewProgressEditor::IsValueDrawn(void) const
{
    return(m_bDrawValue);
}

/** Set if the value is drawn as text
  * \param bIsDrawn [input] : true if the text value is drawn on top of the progress bar
  */
void wxVirtualDataViewProgressEditor::SetValueIsDrawn(bool bIsDrawn)
{
    m_bDrawValue = bIsDrawn;
}

