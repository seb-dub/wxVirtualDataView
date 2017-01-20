/**********************************************************************/
/** FILE    : VirtualDataViewItemAttr.h                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view item attribute                             **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_ITEM_ATTR_H_
#define _VIRTUAL_DATA_VIEW_ITEM_ATTR_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/colour.h>
#include <wx/font.h>

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
    class WXDLLIMPEXP_ADV wxDataViewItemAttr;
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

//-------------------------------- DECLARATION -------------------------------------//
/** \class wxVirtualDataViewItemAttr : defines a graphic attributes. Used by renderers
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewItemAttr
{
    public:

        //constructors & destructor
        wxVirtualDataViewItemAttr(void);                                ///< \brief default constructor
        wxVirtualDataViewItemAttr(const wxVirtualDataViewItemAttr &rhs);///< \brief copy constructor
        wxVirtualDataViewItemAttr&
            operator=(const wxVirtualDataViewItemAttr &rhs);            ///< \brief assignation operator
        ~wxVirtualDataViewItemAttr(void);                               ///< \brief destructor

        //default
        static wxVirtualDataViewItemAttr GetDefault(void);              ///< \brief get the default object

        //status
        void Clear(void);                                               ///< \brief clear the attribute
        bool IsComplete(void) const;                                    ///< \brief check if the attribute is complete

        //background colour
        bool HasBackgroundColour(void) const;                           ///< \brief check if a background colour is available
        const wxColour& GetBackgroundColour(void) const;                ///< \brief get the background colour
        void SetBackgroundColour(const wxColour &rColour);              ///< \brief set the background colour

        //fore colour
        bool HasColour(void) const;                                     ///< \brief check if a foreground colour is available
        const wxColour& GetColour() const;                              ///< \brief get the foreground colour
        void SetColour(const wxColour &rColour);                        ///< \brief set the foreground colour

        //font
        wxFont  GetFont(void) const;                                    ///< \brief get the user defined font
        void SetFont(const wxFont &rFont);                              ///< \brief set the user defined font
        void SetBold(bool bSet);                                        ///< \brief set/unset the bold font attribute
        void SetItalic(bool bSet);                                      ///< \brief set/unset the italic font attribute
        bool GetBold(void) const;                                       ///< \brief check if the font is bold
        bool GetItalic(void) const;                                     ///< \brief check if the font is italic
        bool HasFont(void) const;                                       ///< \brief return true if at least one font attribute is set
        wxFont GetEffectiveFont(const wxFont& rFont) const;             ///< \brief return the font to apply

        //default
        bool IsDefault(void) const;                                     ///< \brief check if the default behaviour is applied

        //combine
        void Combine(wxVirtualDataViewItemAttr *pAttr);                 ///< \brief combine this attribute with another one
        void Combine(const wxVirtualDataViewItemAttr &rhs);             ///< \brief combine this attribute with another one
        wxVirtualDataViewItemAttr&
         operator+=(const wxVirtualDataViewItemAttr &rhs);              ///< \brief combine this attribute with another one
        wxVirtualDataViewItemAttr&
         operator+=(const wxVirtualDataViewItemAttr *pRHS);             ///< \brief combine this attribute with another one
        wxVirtualDataViewItemAttr
          operator+(const wxVirtualDataViewItemAttr &rhs) const;        ///< \brief combine this attribute with another one

        //compare
        bool IsSame(const wxVirtualDataViewItemAttr &rhs) const;        ///< \brief check if the 2 attributes are identical
        bool operator==(const wxVirtualDataViewItemAttr &rhs) const;    ///< \brief operator ==
        bool operator!=(const wxVirtualDataViewItemAttr &rhs) const;    ///< \brief operator !=

        //compatibility with wxDataViewCtrl
#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
        wxVirtualDataViewItemAttr(const wxDataViewItemAttr &rAttr);     ///< \brief constructor from a wxDataViewAttr
        void FromDataViewAttr(const wxVirtualDataViewItemAttr &rAttr);  ///< \brief set from a wxVirtualDataViewItemAttr
        wxVirtualDataViewItemAttr ToDataViewAttr(void) const;           ///< \brief convert to a wxVirtualDataViewItemAttr
#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC

    protected:
        //data
        wxColour    m_cForeColour;                                      ///< \brief foreground colour
        wxColour    m_cBackColour;                                      ///< \brief background colour
        wxFont      m_Font;                                             ///< \brief the font
        bool        m_bIsBold;                                          ///< \brief true for bold text
        bool        m_bIsItalic;                                        ///< \brief true for italic text

        //methods
        void Release(void);                                             ///< \brief release the object
        void Copy(const wxVirtualDataViewItemAttr &rhs);                ///< \brief copy an object
};

//--------------------- INLINE IMPLEMENTATION -----------------------//
//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Default constructor
  */
WX_VDV_INLINE wxVirtualDataViewItemAttr::wxVirtualDataViewItemAttr(void)
    : m_bIsBold(false),
      m_bIsItalic(false)
{

}

//-------------------------- STATUS ---------------------------------//
/** Check if the attribute is complete
  * \return true if the attribute is complete (all defined), false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::IsComplete(void) const
{
    if (!m_cForeColour.IsOk()) return(false);
    if (!m_cBackColour.IsOk()) return(false);
    if (!m_Font.IsOk())        return(false);
    return(true);
}

//----------------- BACKGROUND COLOUR -------------------------------//
/** Check if a background colour is available
  * \return true if a background colour is available, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::HasBackgroundColour(void) const
{
    return(m_cBackColour.IsOk());
}

/** Get the background colour
  * \return a const reference to the background colour
  */
WX_VDV_INLINE const wxColour& wxVirtualDataViewItemAttr::GetBackgroundColour(void) const
{
    return(m_cBackColour);
}

/** Set the background colour
  * \param rColour [input] : the new background colour
  */
WX_VDV_INLINE void wxVirtualDataViewItemAttr::SetBackgroundColour(const wxColour &rColour)
{
    m_cBackColour = rColour;
}

//----------------- FOREGROUND COLOUR -------------------------------//
/** Check if a foreground colour is available
  * \return true if a foreground colour is available, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::HasColour(void) const
{
    return(m_cForeColour.IsOk());
}

/** Get the foreground colour
  * \return a const reference to the foreground colour
  */
WX_VDV_INLINE const wxColour& wxVirtualDataViewItemAttr::GetColour(void) const
{
    return(m_cForeColour);
}

/** Set the foreground colour
  * \param rColour [input] : the new foreground colour
  */
WX_VDV_INLINE void wxVirtualDataViewItemAttr::SetColour(const wxColour &rColour)
{
    m_cForeColour = rColour;
}

//---------------------- FONT ---------------------------------------//
/** Get the user defined font
  * \return the font
  */
WX_VDV_INLINE wxFont wxVirtualDataViewItemAttr::GetFont(void) const
{
    return(m_Font);
}

/** Set/unset the bold font attribute
  * \param bSet [input]: if true, the attribute is set.
  *                      if false, the attribute is unset
  */
WX_VDV_INLINE void wxVirtualDataViewItemAttr::SetBold(bool bSet)
{
    m_bIsBold = bSet;
}

/** Set/unset the italic font attribute
  * \param bSet [input]: if true, the attribute is set.
  *                      if false, the attribute is unset
  */
WX_VDV_INLINE void wxVirtualDataViewItemAttr::SetItalic(bool bSet)
{
    m_bIsItalic = bSet;
}

/** Check if the font is bold
  * \return true if the font is bold, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::GetBold(void) const
{
    return(m_bIsBold);
}

/** Check if the font is italic
  * \return true if the font is italic, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::GetItalic(void) const
{
    return(m_bIsItalic);
}

/** Return true if at least one font attribute is set
  * \return true if at least one font attribute is set
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::HasFont(void) const
{
    return((m_bIsBold) || (m_bIsItalic) || (m_Font.IsOk()));
}

//-------------------- DEFAULT --------------------------------------//
/** Check if the default behaviour is applied
  * \return true if the default behaviour is applied, false otherwise
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::IsDefault(void) const
{
    if (HasColour()) return(false);
    if (HasFont()) return(false);
    if (HasBackgroundColour()) return(false);
    return(true);
}

//-------------------- COMBINE --------------------------------------//
/** Combine this attribute with another one
  * \param rhs [input]: the 2nd attribute
  * The 1st attribute (this one) takes precedence.
  * For example if both this and rhs have a font defined, the font defined
  * in THIS OBJECT is used
  * \return a reference to this object
  */
WX_VDV_INLINE wxVirtualDataViewItemAttr& wxVirtualDataViewItemAttr::
  operator+=(const wxVirtualDataViewItemAttr &rhs)
{
    Combine(rhs);
    return(*this);
}

/** Combine this attribute with another one
  * \param rhs [input]: the 2nd attribute
  * The 1st attribute (this one) takes precedence.
  * For example if both this and rhs have a font defined, the font defined
  * in THIS OBJECT is used
  * \return a reference to this object
  */
WX_VDV_INLINE wxVirtualDataViewItemAttr& wxVirtualDataViewItemAttr::
  operator+=(const wxVirtualDataViewItemAttr *pRHS)
{
    if (pRHS) Combine(*pRHS);
    return(*this);
}

/** Combine this attribute with another one
  * \param rhs [input]: the 2nd attribute
  * The 1st attribute (this one) takes precedence.
  * For example if both this and rhs have a font defined, the font defined
  * in THIS OBJECT is used
  * \return a new object resulting of the combination of this + rhs
  */
WX_VDV_INLINE wxVirtualDataViewItemAttr wxVirtualDataViewItemAttr::
    operator+(const wxVirtualDataViewItemAttr &rhs) const
{
    wxVirtualDataViewItemAttr oResult(*this);
    oResult.Combine(rhs);
    return(oResult);
}

//--------------------------- COMPARE -------------------------------//
/** Operator ==
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::
    operator==(const wxVirtualDataViewItemAttr &rhs) const
{
    return(IsSame(rhs));
}

/** Operator !=
  */
WX_VDV_INLINE bool wxVirtualDataViewItemAttr::
    operator!=(const wxVirtualDataViewItemAttr &rhs) const
{
    return(!IsSame(rhs));
}

#endif

