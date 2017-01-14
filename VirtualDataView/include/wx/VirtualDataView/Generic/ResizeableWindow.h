/**********************************************************************/
/** FILE    : ResizeableWindow.h                                     **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxPanel which can be resized                         **/
/**********************************************************************/

#ifndef _WX_RESIZEABLE_WINDOW_H_
#define _WX_RESIZEABLE_WINDOW_H_

#include <wx/panel.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewFilterCtrl;
extern const char wxResizeableWindowNameStr[];

/** \class wxResizeablePanel : a wxPanel which can be resized.
  * A resizing border is shown on the panel. This resizing border can be native on Windows,
  * or can be custom drawn (default on all platforms)
  *     Use SetNativeResizeBorder() to use native border (MS Windows only) or
  *     SetResizeBorders() to initialize the border sizes
  *     Override PaintBorders() to have custom resize borders painting
  * Sizing events can be optionally forwarded to the parent window
  */
class WXDLLIMPEXP_VDV wxResizeableWindow : public wxPanel
{
    public:

        //constructors & destructor
        wxResizeableWindow(void);                                                   ///< \brief default constructor. Create() must be called manually
        wxResizeableWindow(wxWindow *pParent,
                          wxWindowID id      = wxID_ANY,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long lStyle        = wxTAB_TRAVERSAL,
                          wxString sName     = wxResizeableWindowNameStr);          ///< \brief constructor
        virtual ~wxResizeableWindow(void);                                          ///< \brief destructor

        //initialization
        virtual bool CreateWin(wxWindow *pParent,
                               wxWindowID id      = wxID_ANY,
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize,
                               long lStyle        = wxTAB_TRAVERSAL,
                               wxString sName     = wxResizeableWindowNameStr);     ///< \brief really creates the window

        //borders
        void SetNativeResizeBorder(bool bNativeBorder = true);                      ///< \brief set native resizing border
        bool IsNativeResizeBorder(void) const;                                      ///< \brief check if native resizing border is used
        void SetResizeBorders(int iUp, int iDown, int iLeft, int iRight);           ///< \brief set resizing borders (non-native)
        void GetResizeBorders(int &iUp, int &iDown, int &iLeft, int &iRight) const; ///< \brief get resizing borders (non-native)

        //minimal size
        bool RespectMinimalSize(void) const;                                        ///< \brief check if minimal size is respected
        void RespectMinimalSize(bool bRespect);                                     ///< \brief set if the minimal size is respected

        //forward to parent
        bool ForwardToParent(void) const;                                           ///< \brief check if the sizing events are forwarded to the parent
        void ForwardToParent(bool bForward);                                        ///< \brief set if the sizing events are forwarded to the parent

    protected:
        //enum
        /// \enum EBorder : indicates which border(s) is (are) currently resized. Combinations allowed
        enum EBorder
        {
            WX_E_BORDER_NONE        = 0x00,                                         ///< \brief no borders
            WX_E_BORDER_LEFT        = 0x01,                                         ///< \brief left border
            WX_E_BORDER_RIGHT       = 0x02,                                         ///< \brief right border
            WX_E_BORDER_TOP         = 0x04,                                         ///< \brief top border
            WX_E_BORDER_BOTTOM      = 0x08                                          ///< \brief bottom border
        };

        //data
        bool    m_bNativeBorder;                                                    ///< \brief true for native borders
        int     m_BorderSizes[4];                                                   ///< \brief border sizes, in pixels. <= 0 means no resizing border
        int     m_iResizing;                                                        ///< \brief combination of EBorder flags indicating which borders are resizing
        bool    m_bRespectMinimalSize;                                              ///< \brief true if minimal size is respected
        bool    m_bForwardToParent;                                                 ///< \brief true for forwarding the size to the parent window

        //wxWindow override
        virtual wxSize DoGetBestSize(void) const;                                   ///< \brief get best size

        //event handlers
        virtual void OnResize(wxSizeEvent &rEvent);                                 ///< \brief event handler on resize
        virtual void OnMouseEvent(wxMouseEvent &rEvent);                            ///< \brief event handler for mouse events
        virtual void OnMouseLeave(wxMouseEvent &rEvent);                            ///< \brief event handler for mouse leave event
        virtual void OnMouseCaptureLost(wxMouseCaptureLostEvent &rEvent);           ///< \brief event handler for mouse capture lost
        virtual void OnMouseCaptureChanged(wxMouseCaptureChangedEvent &rEvent);     ///< \brief event handler for mouse capture changed
        virtual void OnEraseBackground(wxEraseEvent &rEvent);                       ///< \brief event handler for erase background events
        virtual void OnPaintEvent(wxPaintEvent &rEvent);                            ///< \brief event handler for paint events

        //methods
        virtual void PaintBorders(wxDC &rDC);                                       ///< \brief paint the borders
        virtual void ResizeChildren(const wxSize &rSize);                           ///< \brief resize the children

        bool ShouldExpandChildWindow(void) const;                                   ///< \brief check if the child window should be expanded

    private:
        //methods
        void DoGetBorders(int &iUp, int &iDown, int &iLeft, int &iRight) const;     ///< \brief get borders size - negatives values are set to 0
        void DoDrawPointsH(wxDC &rDC, int x, int y, int h);                         ///< \brief draw 3 aligned points horizontally
        void DoDrawPointsV(wxDC &rDC, int x, int y, int w);                         ///< \brief draw 3 aligned points vertically
        void DoDraw2PointsH(wxDC &rDC, int x, int y, int h);                        ///< \brief draw 2 points in a corner
        void DoDraw1Point(wxDC &rDC, int x, int y, int w, int h);                   ///< \brief draw 1 point in a corner

        wxDECLARE_ABSTRACT_CLASS(wxResizeableWindow);
};

#endif

