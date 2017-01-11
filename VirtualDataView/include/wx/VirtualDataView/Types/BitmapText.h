/**********************************************************************/
/** FILE    : BitmapText.h                                           **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : an association of bitmap + text                        **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_BITMAP_TEXT_H_
#define _VIRTUAL_DATA_VIEW_BITMAP_TEXT_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/icon.h>

/** \class wxBitmapText: an association of bitmap + text
  */
class WXDLLIMPEXP_VDV wxBitmapText : public wxObject
{
    public:
        //constructors & destructor
        wxBitmapText(const wxString &sText = wxEmptyString,
                     const wxIcon &rIcon = wxNullIcon);                 ///< \brief default constructor
        wxBitmapText(const wxString &sText,
                     const wxBitmap &rBitmap);                          ///< \brief constructor
        wxBitmapText(const wxBitmapText &rhs);                          ///< \brief copy constructor
        wxBitmapText& operator=(const wxBitmapText &rhs);               ///< \brief assignation operator
        ~wxBitmapText(void);                                            ///< \brief destructor

        //text
        void SetText(const wxString &sText);                            ///< \brief set the text
        wxString GetText(void) const;                                   ///< \brief get the text

        //bitmap
        void SetBitmap(const wxBitmap &rBitmap);                        ///< \brief set the bitmap
        void SetIcon(const wxIcon &rIcon);                              ///< \brief set the icon
        const wxIcon& GetIcon(void) const;                              ///< \brief get the icon
        wxIcon& GetIcon(void);                                          ///< \brief get the icon

        //operators
        bool IsSameAs(const wxBitmapText &rhs) const;                   ///< \brief comparison test
        bool operator==(const wxBitmapText &rhs) const;                 ///< \brief operator ==
        bool operator!=(const wxBitmapText& rhs) const;                 ///< \brief operator !=


    private:
        wxString        m_sText;                                        ///< \brief the text data
        wxIcon          m_Icon;                                         ///< \brief the bitmap data

        wxDECLARE_DYNAMIC_CLASS(wxBitmapText);
};
DECLARE_VARIANT_OBJECT_EXPORTED(wxBitmapText, WXDLLIMPEXP_VDV)

#endif
