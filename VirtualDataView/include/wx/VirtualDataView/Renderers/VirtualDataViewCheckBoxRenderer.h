/**********************************************************************/
/** FILE    : VirtualDataViewCheckBoxRenderer.h                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view bool renderer using a checkbox             **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_CHECK_BOX_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_CHECK_BOX_RENDERER_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>

/** \class wxVirtualDataViewCheckBoxRenderer : a checkbox renderer for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewCheckBoxRenderer: public wxVirtualDataViewRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewCheckBoxRenderer(bool bTriState = false);                  ///< \brief default constructor
        virtual ~wxVirtualDataViewCheckBoxRenderer(void);                           ///< \brief destructor

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

        //tri-state
        bool IsTriState(void) const;                                                ///< \brief check if the renderer is tri-state
        void SetTriState(bool bTriState);                                           ///< \brief set the renderer to tri-state

    protected:
        //data
        bool            m_bTriState;                                                ///< \brief true if the renderer is tristate

};

#endif

