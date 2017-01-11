/**********************************************************************/
/** FILE    : VirtualDataViewRenderer.h                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view renderer                                   **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_RENDERER_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

//forward declarations
class WXDLLIMPEXP_BASE wxVariant;
class WXDLLIMPEXP_CORE wxDC;
class WXDLLIMPEXP_CORE wxSize;
class WXDLLIMPEXP_CORE wxRect;
class WXDLLIMPEXP_CORE wxColour;
class WXDLLIMPEXP_CORE wxWindow;

class WXDLLIMPEXP_VDV wxVirtualDataViewItemAttr;
class WXDLLIMPEXP_VDV wxVirtualDataViewItemState;

/** \class wxVirtualDataViewRenderer : a renderer for the wxVirtualDataViewCtrl control
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewRenderer
{
    public:
        //enums
        /// \brief horizontal alignment
        enum EHAlign
        {
            WX_HORIZ_ALIGN_LEFT     = 0,                                        ///< \brief aligned to the left
            WX_HORIZ_ALIGN_CENTRE   = 1,                                        ///< \brief centred horizontally
            WX_HORIZ_ALIGN_RIGHT    = 2                                         ///< \brief aligned to the right
        };

        /// \brief vertical alignment
        enum EVAlign
        {
            WX_VERT_ALIGN_TOP       = 0,                                        ///< \brief aligned to the top
            WX_VERT_ALIGN_CENTRE    = 1,                                        ///< \brief centred vertically
            WX_VERT_ALIGN_BOTTOM    = 2                                         ///< \brief aligned to the bottom
        };

        /// \brief ellipsization mode
        enum EEllipsizeMode
        {
            WX_ELLIPSIZE_NONE       = 0,                                        ///< \brief do not ellipsize the text at all
            WX_ELLIPSIZE_START      = 1,                                        ///< \brief replace the start of the text with "..."
            WX_ELLIPSIZE_MIDDLE     = 2,                                        ///< \brief replace the middle of the text with "..."
            WX_ELLIPSIZE_END        = 3                                         ///< \brief replace the end of the text with "..."
        };

        /// \brief ECheckBoxValue : the check box values
        enum ECheckBoxValue
        {
            WX_CHECKBOX_UNCHECKED       = 0,                                    ///< \brief the checkbox is unchecked
            WX_CHECKBOX_CHECKED         = 1,                                    ///< \brief the checkbox is checked
            WX_CHECKBOX_UNDETERMINED    = 2                                     ///< \brief the checkbox state is undetermined
        };

        //constructors & destructor
        wxVirtualDataViewRenderer(void);                                        ///< \brief constructor
        virtual ~wxVirtualDataViewRenderer(void);                               ///< \brief destructor

        //interface
        virtual wxVirtualDataViewRenderer* Clone(void) const             = 0;   ///< \brief clone the renderer
        virtual void Render(wxWindow *pWindow, const wxRect &rRect,
                            wxDC &rDC,
                            wxVirtualDataViewItemAttr *pAttr,
                            const wxVirtualDataViewItemState &rState,
                            const wxVariant& vValue)                     = 0;   ///< \brief render the item
        virtual wxSize GetSize(wxWindow *pWindow,
                               wxVirtualDataViewItemAttr *pAttr,
                               const wxVirtualDataViewItemState &rState,
                               const wxVariant& vValue) const            = 0;   ///< \brief return the size of the item

        //helper methods : rendering
        static void RenderBackground(wxWindow *pWindow,
                                     const wxRect &rRect, wxDC &rDC,
                                     wxVirtualDataViewItemAttr *pAttr,
                                     bool bWindowBackground = false);           ///< \brief draw a background
        static void RenderText(wxWindow *pWindow,
                               const wxRect &rRect, wxDC &rDC,
                               const wxString &rsText,
                               int iXOffset, int iYOffset,
                               wxVirtualDataViewItemAttr *pAttr,
                               const wxVirtualDataViewItemState &rState,
                               EHAlign eHAlign, EVAlign eVAlign,
                               EEllipsizeMode eEllipsization);                  ///< \brief draw text without background
        static void RenderSelectionRect(wxWindow *pWindow,
                                        const wxRect &rRect, wxDC &rDC,
                                        const wxVirtualDataViewItemState &rState,
                                        wxColour *pColour);                     ///< \brief draw a selection rectangle
        static void RenderFocusRect(wxWindow *pWindow,
                                    const wxRect &rRect, wxDC &rDC,
                                    const wxVirtualDataViewItemState &rState);  ///< \brief draw a focus rectangle
        static void RenderCheckBox(wxWindow *pWindow,
                                   const wxRect &rRect, wxDC &rDC,
                                   ECheckBoxValue eValue,
                                   bool bMouseOver = false);                    ///< \brief draw a checkbox
        static void RenderButtonFrame(wxWindow *pWindow,
                                      const wxRect &rRect, wxDC &rDC,
                                      bool bPressed, bool bMouseOver,
                                      bool bDefault);                           ///< \brief draw a blank push button
        static void RenderProgressBar(wxWindow *pWindow,
                                      const wxRect &rRect, wxDC &rDC,
                                      size_t uiValue, size_t uiMax);            ///< \brief draw a progress bar
        static void RenderTreeButton(wxWindow *pWindow,
                                     const wxRect &rRect, wxDC &rDC,
                                     bool bExpanded);                           ///< \brief draw a tree button
        static void RenderTreeLine(wxWindow *pWindow,
                                   const wxRect &rRect, wxDC &rDC,
                                   wxCoord x0, wxCoord y0,
                                   wxCoord x1, wxCoord y1);                     ///< \brief draw a tree line
        static void GetTextColour(wxWindow *pWindow, wxDC &rDC,
                                  wxVirtualDataViewItemAttr *pAttr,
                                  const wxVirtualDataViewItemState &rState,
                                  wxColour &rColour);                           ///< \brief compute text colour

        //helper methods : measuring
        static wxSize GetCheckboxSize(wxWindow *pWindow);                       ///< \brief get a check box size
        static wxSize GetTextExtent(wxWindow *pWindow, const wxString &rsText,
                           wxVirtualDataViewItemAttr *pAttr = WX_VDV_NULL_PTR); ///< \brief measure text size

    protected:
        //methods
        void CopyBaseRenderer(const wxVirtualDataViewRenderer &rhs);            ///< \brief copy the base renderer
};

#endif

