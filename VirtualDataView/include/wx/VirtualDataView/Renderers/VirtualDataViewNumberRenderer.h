/**********************************************************************/
/** FILE    : VirtualDataViewNumberRenderer.h                        **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view text renderer                              **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_NUMBER_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_NUMBER_RENDERER_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>

/** \class wxVirtualDataViewNumberRenderer : a number renderer for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewNumberRenderer: public wxVirtualDataViewRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewNumberRenderer(bool bFloat = false,
                                        EEllipsizeMode eMode = WX_ELLIPSIZE_NONE,
                                        EHAlign eHAlign = WX_HORIZ_ALIGN_LEFT,
                                        EVAlign eVAlign = WX_VERT_ALIGN_CENTRE);    ///< \brief default constructor
        virtual ~wxVirtualDataViewNumberRenderer(void);                             ///< \brief destructor

        //interface implementation
        virtual wxVirtualDataViewRenderer* Clone(void) const;                       ///< \brief clone the renderer
        virtual void Render(wxWindow *pWindow, const wxRect &rRect,
                            wxDC &rDC,
                            wxVirtualDataViewItemAttr *pAttr,
                            const wxVirtualDataViewItemState &rState,
                            const wxVariant& vValue);                               ///< \brief render the item
        virtual wxSize GetSize(wxWindow *pWindow,
                               wxVirtualDataViewItemAttr *pAttr,
                               const wxVirtualDataViewItemState &rState,
                               const wxVariant& vValue) const;                      ///< \brief return the size of the item

        //alignment
        void SetAlignment(EHAlign eHAlign, EVAlign eVAlign);                        ///< \brief set alignment
        void GetAlignment(EHAlign &eHAlign, EVAlign &eVAlign) const;                ///< \brief get alignment

        //ellipsization
        void SetEllipsizeMode(EEllipsizeMode eMode);                                ///< \brief set ellipsization mode
        EEllipsizeMode GetEllipsizeMode(void) const;                                ///< \brief get ellipsization mode

        //floating point / integer
        bool    IsFloatingPoint(void) const;                                        ///< \brief return true if the format is floating point
        bool    IsInteger(void) const;                                              ///< \brief return true if the format is integer
        void    SetFloatingPoint(bool bFloat = true);                               ///< \brief set floating point / integer
        void    SetInteger(bool bInteger = true);                                   ///< \brief set integer / floating point

        //floating point options
        enum EFormat
        {
            WX_FP_FORMAT_DECIMAL                        = 0x00,                     ///< \brief same as printf("%f") : decimal point representation
            WX_FP_FORMAT_SCIENTIFIC                     = 0x01,                     ///< \brief same as printf("%e") : scientific notation
            WX_FP_FORMAT_COMPACT                        = 0x02,                     ///< \brief same as printf("%g") : %e or %f, whichever is shorter.
            WX_FP_FORMAT_UPPER_CASE                     = 0x04,                     ///< \brief upper case (%F, %G or %E)
            WX_FP_FORMAT_SHOW_ALWAYS_DECIMAL_SEPARATOR  = 0x08                      ///< \brief decimal separator always shown
        };
        int GetFloatingPointFormat(void) const;                                     ///< \brief get the floating point format
        void SetFloatingPointFormat(int iFormat);                                   ///< \brief set the floating point format

        //precision
        int GetPrecision(void) const;                                               ///< \brief get the minimal amount of digits to write
        void SetPrecision(int iPrecision);                                          ///< \brief set the minimal amount of digits to write

        //width
        int GetWidth(void) const;                                                   ///< \brief get the amount of characters to print
        void SetWidth(int iWidth);                                                  ///< \brief set the amount of characters to print
        bool IsZeroUsedAsPaddingChar(void) const;                                   ///< \brief return true if '0' is used as padding character
        bool IsSpaceUsedAsPaddingChar(void) const;                                  ///< \brief return true if ' ' is used as padding character
        void SetUseZeroAsPaddingChar(bool bUseZero = true);                         ///< \brief toggle '0'/' ' as padding character
        void SetUseSpaceAsPaddingChar(bool bUseSpace = true);                       ///< \brief toggle ' '/'0' as padding character

        //plus sign
        bool IsPlusSignAlwaysShown(void) const;                                     ///< \brief return true if '+' sign is prependded on positive numbers
        void SetPlusSignAlwaysShown(bool bShow = true);                             ///< \brief toggle '+' sign ON/OFF on positive numbers
        void SetPlusSignHidden(bool bHidden = true);                                ///< \brief toggle '+' sign OFF/ON on positive numbers

        //signed / unsigned
        bool IsSigned(void) const;                                                  ///< \brief true if the number is signed
        bool IsUnsigned(void) const;                                                ///< \brief true if the number is unsigned
        void SetSigned(bool bSigned = true);                                        ///< \brief toggle signed/unsigned
        void SetUnsigned(bool bUnsigned = true);                                    ///< \brief toggle unsigned/signed

    protected:
        //data
        wxUint32                m_bFloat                : 1;                        ///< \brief floating point number
        wxUint32                m_iFloatingPointFormat  : 4;                        ///< \brief floating point format
        wxUint32                m_bPrependPlusSign      : 1;                        ///< \brief true if '+' sign is prepended on positive numbers
        wxUint32                m_bUnsigned             : 1;                        ///< \brief signed / unsigned
        wxUint32                m_bUseZeroAsPadChar     : 1;                        ///< \brief use '0' as padding character
        wxUint32                m_eEllipsizeMode        : 2;                        ///< \brief ellipsization mode
        wxUint32                m_eHorizontalAlignment  : 2;                        ///< \brief horizontal alignment
        wxUint32                m_eVerticalAlignment    : 2;                        ///< \brief vertical alignment
        wxUint32                m_iPrecision            : 9;                        ///< \brief the minimal amount of digits to write
        wxUint32                m_iWidth                : 9;                        ///< \brief the minimal amount of characters to write

        //method
        wxString ConvertToString(const wxVariant &rvValue) const;                   ///< \brief convert the variant to a string


};

#endif

