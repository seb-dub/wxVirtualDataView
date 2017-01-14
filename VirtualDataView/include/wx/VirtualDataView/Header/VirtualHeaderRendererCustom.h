/**********************************************************************/
/** FILE    : VirtualHeaderRendererCustom.h                          **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a custom implementation of wxVirtualHeaderIRenderer    **/
/**********************************************************************/

#ifndef _VIRTUAL_HEADER_RENDERER_CUSTOM_H_
#define _VIRTUAL_HEADER_RENDERER_CUSTOM_H_

#include <wx/VirtualDataView/Header/VirtualHeaderIRenderer.h>

//------------------------- DECLARATION -----------------------------//
/** \class wxVirtualHeaderRendererCustom : a custom implementation of wxVirtualHeaderIRenderer
  */
class WXDLLIMPEXP_VDV wxVirtualHeaderRendererCustom : public wxVirtualHeaderIRenderer
{
    public:
        //enum
        /// \enum ESettings : identify a specific state
        enum ESettings
        {
            WX_VHRC_NORMAL_STATE_SETTINGS        = 0,                           ///< \brief normal state settings
            WX_VHRC_CURRENT_STATE_SETTINGS       = 1,                           ///< \brief item is current
            WX_VHRC_HOTTRACKED_STATE_SETTINGS    = 2,                           ///< \brief item is hot-tracked
            WX_VHRC_PUSHED_STATE_SETTINGS        = 3                            ///< \brief item is pushed
        };

        //structs
        struct TSettings
        {
            wxColour    m_cFirstColour;                                         ///< \brief 1st gradient background colour
            wxColour    m_cSecondColour;                                        ///< \brief 2nd gradient background colour
            wxDirection m_iGradientDirection;                                   ///< \brief gradient direction
            wxFont      m_Font;                                                 ///< \brief text font
            wxColour    m_cTextColour;                                          ///< \brief text colour
            wxColour    m_EdgeLightColour;                                      ///< \brief edge colour in light part of button
            wxColour    m_EdgeShadowColour;                                     ///< \brief edge colour in shadow part of button
            wxColour    m_SortArrowColour;                                      ///< \brief the sort arrow colour
        };

        //constructors & destructor
        wxVirtualHeaderRendererCustom(wxWindow *pWin);                          ///< \brief constructor
        virtual ~wxVirtualHeaderRendererCustom(void);                           ///< \brief destructor

        //interface
        virtual bool IsOK(void) const;                                          ///< \brief check if the engine is usable
        virtual wxSize GetHeaderButtonSize(const THeaderData &rData);           ///< \brief get the header button height
        virtual void DrawHeader(wxDC &rDC, const wxRect &r,
                                const THeaderData &rData);                      ///< \brief draw the header
        virtual void DrawHeaderContent(wxDC &rDC, const wxRect &r,
                                       const THeaderData &rData);               ///< \brief draw the header content, without background
        virtual void DrawHeaderBackground(wxDC &rDC, const wxRect &r);          ///< \brief draw the header background only
        virtual void PrepareDC(wxDC &rDC);                                      ///< \brief prepare the DC for rendering

        //painting settings
        void SetGraphicSettings(ESettings eSetting, const TSettings &rSettings);///< \brief set the graphic settings
        void GetGraphicSettings(ESettings eSetting,
                                TSettings &rSettings) const;                    ///< \brief get the graphic settings
        void ResetGraphicSettings(ESettings eSetting);                          ///< \brief reset the graphic settings

        //margins
        void GetMargins(int &iMarginX, int &iMarginY) const;                    ///< \brief get the margins size
        void SetMargins(int iMarginX, int iMarginY);                            ///< \brief set the margins size

    protected:
        //struct
        struct TSettingsObjects
        {
            wxPen   m_PenEdgesShadow;                                           ///< \brief pen for shadow edges
            wxPen   m_PenEdgesLight;                                            ///< \brief pen for lit edges
        };
        static const int s_iNbSettings = 4;                                     ///< \brief amount of settings

        //data
        TSettings           m_Settings[s_iNbSettings];                          ///< \brief graphic attributes for the specific states
        TSettingsObjects    m_DCObjects[s_iNbSettings];                         ///< \brief DC objects
        int                 m_iMarginX;                                         ///< \brief left and right margin surrounding text & bitmaps
        int                 m_iMarginY;                                         ///< \brief top and bottom margin surrounding text & bitmaps

        //methods
        void Init(void);                                                        ///< \brief initialization
        void CreateDCObjects(ESettings eSetting);                               ///< \brief create the DC objects

    private:
        //methods - inlined
        ESettings DoGetCurrentSetting(const THeaderData &rData) const;          ///< \brief get current settings
        void DoDrawHeaderBackground(wxDC &rDC, const wxRect &r,
                                    ESettings eSetting,
                                    const wxColour &cColour) const;             ///< \brief draw the header background
        void DoDrawHeaderContent(wxDC &rDC, const wxRect &r,
                                 const THeaderData &rData) const;               ///< \brief draw the header content
        void DoDrawArrow(wxDC &rDC, wxPoint *pts, const THeaderData &rData,
                         ESettings eSettings) const;                            ///< \brief draw a sorting arrow
};

#endif // _VIRTUAL_HEADER_RENDERER_CUSTOM_H_
