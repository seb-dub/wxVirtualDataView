/**********************************************************************/
/** FILE    : VirtualDataViewCellAttr.h                              **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view cell attribute                             **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_CELL_ATTR_H_
#define _VIRTUAL_DATA_VIEW_CELL_ATTR_H_

#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/string.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewRenderer;
class WXDLLIMPEXP_VDV wxVirtualDataViewEditor;
class WXDLLIMPEXP_VDV wxVirtualDataViewIFilterEditor;
class WXDLLIMPEXP_VDV wxVirtualDataViewItemAttr;

    class WXDLLIMPEXP_ADV wxDataViewRenderer;

//------------------ DECLARATION ------------------------------------//
/** \class wxVirtualDataViewCellAttr : defines data type, renderer
  *        and editors for data view controls
  *  The class owns its objects (renderer, editor, attribute)
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewCellAttr
{
    public:

        //enums
        /// \enum ECellType: read-only / read-write.
        enum ECellType
        {
            WX_CELL_READ_ONLY       = 0,                                    ///< \brief cell is read-only
            WX_CELL_EDITABLE        = 1,                                    ///< \brief cell can be edited using a custom editor
            WX_CELL_ACTIVATABLE     = 2,                                    ///< \brief cell can be modified using a simple click, space or enter
        };

        //constructors & destructor
        wxVirtualDataViewCellAttr(wxString sType = "string",
                 wxVirtualDataViewRenderer* pRenderer = WX_VDV_NULL_PTR,
                 wxVirtualDataViewEditor* pEditor = WX_VDV_NULL_PTR,
                 wxVirtualDataViewItemAttr *pAttr = WX_VDV_NULL_PTR,
          wxVirtualDataViewIFilterEditor *pFilter = WX_VDV_NULL_PTR);       ///< \brief default constructor
        wxVirtualDataViewCellAttr(const wxVirtualDataViewCellAttr &rhs);    ///< \brief copy constructor
        wxVirtualDataViewCellAttr&
            operator=(const wxVirtualDataViewCellAttr &rhs);                ///< \brief assignation operator
        ~wxVirtualDataViewCellAttr(void);                                   ///< \brief destructor

        //type
        wxString GetType(void) const;                                       ///< \brief get the type
        void SetType(wxString sType);                                       ///< \brief set the type

        //flags
        void SetCellFlags(int iFlags);                                      ///< \brief set cell flags
        int GetCellFlags(void) const;                                       ///< \brief get cell flags
        bool IsReadOnly(void) const;                                        ///< \brief return true if the cell is read-only
        bool IsActivatable(void) const;                                     ///< \brief return true if the cell is activatable
        bool IsEditable(void) const;                                        ///< \brief return true if the cell is editable
        void SetReadOnly(bool bReadOnly = true);                            ///< \brief set read-only / editable
        void SetEditable(bool bEditable = true);                            ///< \brief set editable / read-only
        void SetActivatable(bool bActivatable = true);                      ///< \brief set / unset activatable

        //set default types
        bool SetFromType(void);                                             ///< \brief set the attributes from the type
        void SetString(void);                                               ///< \brief set the attribute as string
        void SetBitmap(void);                                               ///< \brief set the attribute as a bitmap
        void SetBitmapText(void);                                           ///< \brief set the attribute as a bitmap + text
        void SetProgress(int iMin = 0, int iMax = 100,
                         bool bDrawValue = true);                           ///< \brief set the attribute as a progress bar
        void SetBool(void);                                                 ///< \brief set the attribute as boolean
        void SetTriState(void);                                             ///< \brief set the attribute as a tri state checkbox
        void SetInteger(void);                                              ///< \brief set the attribute as integer
        void SetUnsignedInteger(void);                                      ///< \brief set the attribute as unsigned integer
        void SetDouble(void);                                               ///< \brief set the attribute as double
        void SetDateTime(void);                                             ///< \brief set the attribute as date-time
        void SetDate(void);                                                 ///< \brief set the attribute as date
        void SetTime(void);                                                 ///< \brief set the attribute as time
        void SetColour(void);                                               ///< \brief set the attribute as colour

        //renderer
        wxVirtualDataViewRenderer* GetRenderer(void) const;                 ///< \brief get the renderer
        void SetRenderer(wxVirtualDataViewRenderer *pRenderer);             ///< \brief set the renderer
        bool IsOwnRenderer(void) const;                                     ///< \brief check if the renderer is owned by the object
        void TakeRendererOwnership(void);                                   ///< \brief take ownership of the renderer
        void ReleaseRendererOwnership(void);                                ///< \brief release ownership of the renderer

        //editor
        wxVirtualDataViewEditor* GetEditor(void) const;                     ///< \brief get the editor
        void SetEditor(wxVirtualDataViewEditor *pRenderer);                 ///< \brief set the editor
        bool IsOwnEditor(void) const;                                       ///< \brief check if the editor is owned by the object
        void TakeEditorOwnership(void);                                     ///< \brief take ownership of the editor
        void ReleaseEditorOwnership(void);                                  ///< \brief release ownership of the editor

        //filter
        wxVirtualDataViewIFilterEditor* GetFilterEditor(void) const;        ///< \brief get the filter editor
        void SetFilterEditor(wxVirtualDataViewIFilterEditor *pEditor);      ///< \brief set the filter editor
        bool IsOwnFilterEditor(void) const;                                 ///< \brief check if the filter editor is owned by the object
        void TakeFilterEditorOwnership(void);                               ///< \brief take ownership of the filter editor
        void ReleaseFilterEditorOwnership(void);                            ///< \brief release ownership of the filter editor

        //default item graphic attributes
        wxVirtualDataViewItemAttr*  GetGraphicAttributes(void) const;       ///< \brief get the graphic attributes
        void SetGraphicAttributes(wxVirtualDataViewItemAttr *pAttr);        ///< \brief set the graphic attributes
        bool IsOwnGraphicAttributes(void) const;                            ///< \brief check if the graphic attributes is owned by the object
        void TakeGraphicAttributesOwnership(void);                          ///< \brief take ownership of the graphic attributes
        void ReleaseGraphicAttributesOwnership(void);                       ///< \brief release ownership of the graphic attributes

        //compatibility with wxDataViewCtrl
//        wxVirtualDataViewCellAttr(wxDataViewRenderer *pRenderer);
    protected:
        //data
        wxString                            m_sType;                        ///< \brief type of data
        wxVirtualDataViewRenderer*          m_pRenderer;                    ///< \brief the renderer
        wxVirtualDataViewEditor*            m_pEditor;                      ///< \brief the editor
        wxVirtualDataViewIFilterEditor*     m_pFilterEditor;                ///< \brief the filter editor
        wxVirtualDataViewItemAttr*          m_pAttr;                        ///< \brief the graphic attributes
        int                                 m_iFlags;                       ///< \brief cell flags
        int                                 m_bOwnRenderer : 1;             ///< \brief own renderer
        int                                 m_bOwnEditor   : 1;             ///< \brief own editor
        int                                 m_bOwnFilterEditor : 1;         ///< \brief own filter editor
        int                                 m_bOwnAttributes : 1;           ///< \brief own graphic attributes

        //methods
        void Copy(const wxVirtualDataViewCellAttr &rhs);                    ///< \brief copy an object
        void Release(void);                                                 ///< \brief release the object
        void ReleaseRenderer(void);                                         ///< \brief release the renderer
        void ReleaseEditor(void);                                           ///< \brief release the editor
        void ReleaseFilterEditor(void);                                     ///< \brief release the filter editor
        void ReleaseGraphicAttributes(void);                                ///< \brief release the graphic attributes
        void CopyRenderer(const wxVirtualDataViewCellAttr &rhs);            ///< \brief copy the renderer
        void CopyEditor(const wxVirtualDataViewCellAttr &rhs);              ///< \brief copy the editor
        void CopyFilterEditor(const wxVirtualDataViewCellAttr &rhs);        ///< \brief copy the filter editor
        void CopyGraphicAttributes(const wxVirtualDataViewCellAttr &rhs);   ///< \brief copy the graphic attributes
};

//--------------------- INLINE IMPLEMENTATION -----------------------//
/** Get the type
  * \return the wxVariant type
  */
WX_VDV_INLINE wxString wxVirtualDataViewCellAttr::GetType(void) const
{
    return(m_sType);
}

/** Get the renderer
  * \return the renderer. Do not delete it :
  *          it is owned by the wxVirtualDataViewCellAttr object
  */
WX_VDV_INLINE wxVirtualDataViewRenderer* wxVirtualDataViewCellAttr::GetRenderer(void) const
{
    return(m_pRenderer);
}

/** Get the editor
  * \return the editor. Do not delete it :
  *          it is owned by the wxVirtualDataViewCellAttr object
  */
WX_VDV_INLINE wxVirtualDataViewEditor* wxVirtualDataViewCellAttr::GetEditor(void) const
{
    return(m_pEditor);
}

/** Get the filter editor
  * \return the filter editor. Do not delete it :
  *          it is owned by the wxVirtualDataViewCellAttr object
  */
WX_VDV_INLINE wxVirtualDataViewIFilterEditor* wxVirtualDataViewCellAttr::GetFilterEditor(void) const
{
    return(m_pFilterEditor);
}


/** Get the graphic attributes
  * \return the graphic attributes. Do not delete it :
  *          it is owned by the wxVirtualDataViewCellAttr object
  */
WX_VDV_INLINE wxVirtualDataViewItemAttr*  wxVirtualDataViewCellAttr::GetGraphicAttributes(void) const
{
    return(m_pAttr);
}

/** Get cell flags
  * \return the cell flags (combination of ECellType
  */
WX_VDV_INLINE int wxVirtualDataViewCellAttr::GetCellFlags(void) const
{
    return(m_iFlags);
}

/** Return true if the cell is read-only
  * \return true if the cell is read-only
  */
WX_VDV_INLINE bool wxVirtualDataViewCellAttr::IsReadOnly(void) const
{
    return((m_iFlags & WX_CELL_EDITABLE) == 0);
}

/** Return true if the cell is activatable
  * \return true if the cell is activatable
  */
WX_VDV_INLINE bool wxVirtualDataViewCellAttr::IsActivatable(void) const
{
    return(m_iFlags & WX_CELL_ACTIVATABLE);
}

/** Return true if the cell is editable
  * \return true if the cell is editable
  */
WX_VDV_INLINE bool wxVirtualDataViewCellAttr::IsEditable(void) const
{
    return(m_iFlags & WX_CELL_EDITABLE);
}

#endif

