/**********************************************************************/
/** FILE    : VirtualHeaderIRenderer.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : class for handling header control drawing with theme   **/
/**********************************************************************/

#ifndef _VIRTUAL_HEADER_I_RENDERER_H_
#define _VIRTUAL_HEADER_I_RENDERER_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/dc.h>
#include <wx/renderer.h>

//class WXDLLIMPEXP_CORE wxWindow;

//------------------------- DECLARATION -----------------------------//
/** \class wxVirtualHeaderIRenderer : encapsulated all measurements & rendering
  * of header controls
  */
class WXDLLIMPEXP_VDV wxVirtualHeaderIRenderer
{
    public:
        //enums

        //structs
        /// \struct THeaderData : data used for rendering the items
        struct THeaderData
        {
            //same as wxHeaderButtonParams
            wxColour                m_ArrowColour;                              ///< \brief colour of the arrow
            wxColour                m_SelectionColour;                          ///< \brief background colour of the selection
            wxString                m_sLabel;                                   ///< \brief text to be displayed
            wxFont                  m_FontLabel;                                ///< \brief font to use for the text
            wxColour                m_LabelColour;                              ///< \brief colour to use for the text
            wxBitmap                m_Bitmap;                                   ///< \brief bitmap to display on the label
            int                     m_iAlignment;                               ///< \brief content alignment (wxAlignment combination)
            //additional data
            wxHeaderSortIconType    m_eSortIconType;                            ///< \brief sort arrow to display
            bool                    m_bIsCurrent;                               ///< \brief true if the item is the current one (clicked/selected)
            bool                    m_bIsHotTracked;                            ///< \brief true if the item is hot-tracked (mouse hovering on it)
            bool                    m_bIsPushed;                                ///< \brief true if the item is pushed (mouse button down)
            bool                    m_bIsVisible;                               ///< \brief true if the item is visible
            bool                    m_bIsFiltering;                             ///< \brief true if a filter is currently applied on the item
        };

        //constructors & destructor
        wxVirtualHeaderIRenderer(wxWindow *pWin);                               ///< \brief constructor
        virtual ~wxVirtualHeaderIRenderer(void);                                ///< \brief destructor

        //window to render
        wxWindow*   GetWindow(void) const;                                      ///< \brief get the window
        void        SetWindow(wxWindow* pWindow);                               ///< \brief set the window

        //pure interface
        virtual bool IsOK(void) const                                      = 0; ///< \brief check if the engine is usable
        virtual wxSize GetHeaderButtonSize(const THeaderData &rData)       = 0; ///< \brief get the header button height
        virtual void DrawHeader(wxDC &rDC, const wxRect &r,
                                const THeaderData &rData)                  = 0; ///< \brief draw the header
        virtual void DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                       const THeaderData &rData)           = 0; ///< \brief draw the header content, without background
        virtual void DrawHeaderBackground(wxDC &rDC, const wxRect &r)      = 0; ///< \brief draw the header background only

        //interface with default implementation
        virtual void PrepareDC(wxDC &rDC);                                      ///< \brief prepare the DC for rendering
        virtual void ResetTheme(void);                                          ///< \brief reset theme data

    protected:
        //data
        wxWindow*       m_pWindow;                                              ///< \brief window receiving the rendering
};

#endif // _VIRTUAL_HEADER_I_RENDERER_H_
