/**********************************************************************/
/** FILE    : VirtualDataViewProgressRenderer.h                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view progress bar renderer                      **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_PROGRESS_RENDERER_H_
#define _VIRTUAL_DATA_VIEW_PROGRESS_RENDERER_H_

#include <wx/VirtualDataView/Renderers/VirtualDataViewRenderer.h>

/** \class wxVirtualDataViewProgressRenderer : a progress bar renderer for wxVirtualDataViewCtrl
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewProgressRenderer: public wxVirtualDataViewRenderer
{
    public:
        //constructors & destructor
        wxVirtualDataViewProgressRenderer(void);                                    ///< \brief default constructor
        virtual ~wxVirtualDataViewProgressRenderer(void);                           ///< \brief destructor

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

        //range
        void SetRange(int iMin, int iMax);                                          ///< \brief set the valid range of the progress bar
        void GetRange(int &iMin, int &iMax) const;                                  ///< \brief get the valid range of the progress bar

        //draw value
        bool IsValueDrawn(void) const;                                              ///< \brief check if the value is drawn as text
        void SetValueIsDrawn(bool bIsDrawn);                                        ///< \brief set if the value is drawn as text

    protected:
        //data
        int     m_iMin;                                                             ///< \brief minimal value
        int     m_iMax;                                                             ///< \brief maximal value
        bool    m_bDrawValue;                                                       ///< \brief draw the text value of the item
};

#endif

