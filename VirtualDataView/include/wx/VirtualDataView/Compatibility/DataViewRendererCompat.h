/**********************************************************************/
/** FILE    : DataViewRendererCompat.h                               **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a wxDataViewRenderer using internally wxVirualDataView **/
/**********************************************************************/


#ifndef _DATA_VIEW_RENDERER_COMPAT_H_
#define _DATA_VIEW_RENDERER_COMPAT_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

class WXDLLIMPEXP_VDV wxVirtualDataViewEditor;
class WXDLLIMPEXP_VDV wxVirtualDataViewRenderer;

#if WX_USE_COMPATIBILITY_LAYER_WITH_DVC != 0
#include <wx/dataview.h>

//------------------------- DECLARATION -----------------------------//
/** \class wxDataViewRendererCompat : a compatibility class for wxDataViewRenderer
  * This is a wxDataViewRenderer, and can be used by wxDataViewCtrl directly.
  * This can be used when some Renderers/Editors have been defined for wxVirtualDataViewCtrl. This allows to
  * use them with wxDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxDataViewRendererCompat : public wxDataViewCustomRenderer
{
    public:
        //constructors & destructor
        wxDataViewRendererCompat(const wxString &rVariantType = GetDefaultType(),
                                 wxDataViewCellMode eMode = wxDATAVIEW_CELL_INERT,
                                 int iAlign = wxDVR_DEFAULT_ALIGNMENT);                     ///< \brief constructor
        virtual ~wxDataViewRendererCompat(void);                                            ///< \brief destructor

        //renderer
        void SetRenderer(wxVirtualDataViewRenderer *pRenderer);                             ///< \brief set the renderer
        wxVirtualDataViewRenderer* GetRenderer(void);                                       ///< \brief get the renderer
        bool HasRendererOwnership(void) const;                                              ///< \brief check if it has the renderer ownership
        void TakeRendererOwnership(void);                                                   ///< \brief take renderer ownership
        void ReleaseRendererOwnership(void);                                                ///< \brief release renderer ownership

        //editor
        void SetEditor(wxVirtualDataViewEditor *pEditor);                                   ///< \brief set the editor
        wxVirtualDataViewEditor* GetEditor(void);                                           ///< \brief get the editor
        bool HasEditorOwnership(void) const;                                                ///< \brief check if it has the editor ownership
        void TakeEditorOwnership(void);                                                     ///< \brief take editor ownership
        void ReleaseEditorOwnership(void);                                                  ///< \brief release editor ownership

        //wxDataViewCustomRenderer interface
        virtual bool ActivateCell(const wxRect &rCell,
                                  wxDataViewModel* pDataModel,
                                  const wxDataViewItem &rID,
                                  unsigned int uiCol,
                                  const wxMouseEvent *pMouseEvent);                         ///< \brief react to cell activation
        virtual wxWindow*  CreateEditorCtrl(wxWindow *pParent,
                                            wxRect rLabelRect, const wxVariant &rValue);    ///< \brief create the actual editor control once editing is about to start
        virtual wxSize  GetSize(void) const;                                                ///< \brief return size required to show content
        virtual bool  GetValueFromEditorCtrl(wxWindow *pEditor, wxVariant &rValue);         ///< \brief get the value from the editor control (pointed to by editor)
        virtual bool  HasEditorCtrl(void) const;                                            ///< \brief true if this renderer supports in-place editing
        virtual bool  Activate(wxRect rCell, wxDataViewModel *pDataModel,
                              const wxDataViewItem &rID, unsigned int uiCol);               ///< \brief react to an activation of a cell
        virtual bool  LeftClick(wxPoint ptCursor, wxRect rCell,
                                wxDataViewModel *pDataModel,
                                const wxDataViewItem &rID, unsigned int uiCol);             ///< \brief react to a left click
        virtual bool  Render(wxRect rCell, wxDC *pDC, int iState);                          ///< \brief render the cell
        virtual bool  StartDrag(const wxPoint &ptCursor, const wxRect &rCell,
                                wxDataViewModel *pDataModel,
                                const wxDataViewItem &rID, unsigned int uiCol);             ///< \brief start a drag operation
    protected:
        //data
        wxVirtualDataViewRenderer*  m_pRenderer;                                            ///< \brief the data model
        bool                        m_bOwnRenderer;                                         ///< \brief true if the renderer is owned
        wxVirtualDataViewEditor*    m_pEditor;                                              ///< \brief graphic attributes
        bool                        m_bOwnEditor;                                           ///< \brief true if the editor is owned

        //methods
        void ReleaseRenderer(void);                                                         ///< \brief release the renderer
        void ReleaseEditor(void);                                                           ///< \brief release the editor
};

#endif // WX_USE_COMPATIBILITY_LAYER_WITH_DVC
#endif

