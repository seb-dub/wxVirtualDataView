/**********************************************************************/
/** FILE    : VirtualDataViewIFilterEditor.h                         **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : combines a filter and the UI for modifying it          **/
/**********************************************************************/


#ifndef _VIRTUAL_DATA_VIEW_I_FILTER_EDITOR_H_
#define _VIRTUAL_DATA_VIEW_I_FILTER_EDITOR_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/gdicmn.h>
#include <wx/event.h>
#include <wx/bitmap.h>

//forward declarations
class WXDLLIMPEXP_CORE  wxDC;
class WXDLLIMPEXP_CORE  wxRect;
class WXDLLIMPEXP_CORE  wxWindow;
class WXDLLIMPEXP_CORE  wxSize;
class WXDLLIMPEXP_VDV   wxVirtualIDataModel;
class WXDLLIMPEXP_VDV   wxVirtualIStateModel;
class WXDLLIMPEXP_VDV   wxVirtualDataViewFilter;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_VDV, wxEVT_VDV_FILTER_EDITION_BEGIN, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_VDV, wxEVT_VDV_FILTER_EDITION_CHANGED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_VDV, wxEVT_VDV_FILTER_EDITION_END, wxCommandEvent);

/** \class wxVirtualDataViewIFilterEditor : combines a filter and the UI for modifying it for wxVirtualDataViewCtrl
  *
  * Combines a filter and the UI for modifying it for wxVirtualDataViewCtrl.
  *
  * The interface methods are decomposed in 2 parts:
  *    1 - a User Interface part: this is where the user modify the filter. A combo-box or a search text entry are simple examples
  *    2 - a Filter part: the methods allow to retrieve the filter itself. The filter is owned by the
  *        wxVirtualDataViewIFilterEditor object
  *
  *     Interface for UI:
  *     \code
  *      public:
  *         virtual wxVirtualDataViewIFilterEditor* Clone(void) const           = 0;    // clone the filter
  *         virtual wxSize    GetSize(wxWindow *pWin)                           = 0;    //get the size of the filter
  *         virtual void      Draw(wxWindow *pWin, wxDC &rDC, wxRect &rRect)    = 0;    //draw the filter
  *         virtual bool      StartFilter(wxWindow *pParent, wxRect &rRect)     = 0;    //activate the filter
  *         virtual bool      CanFinishFilterEdition(void)                      = 0;    //return true if filter edition can be finished
  *         virtual bool      FinishFilterEdition(bool bCancelled = false)      = 0;    //end filter edition
  *         virtual wxWindow* GetEditorWindow(void)                             = 0;    //get the editor window
  *         virtual void      ClearFilter(void)                                 = 0;    //clear all items in the filter
  *         virtual void      InitFilter(wxVirtualIDataModel *pSrcModel,
                                         size_t uiField,
                                         wxVirtualIStateModel *pStateModel)     = 0;    // initialize the filter
  *     \endcode
  *
  *     Interface for Filter:
  *     \code
  *     public:
  *         virtual wxVirtualDataViewFilter* GetFilter(void);                           // get the filter
  *     protected:
  *         wxVirtualDataViewFilter*    m_pFilter;                                      // the current filter
  *         virtual void SetFilter(wxVirtualDataViewFilter *pFilter);                   // set the filter
  *         virtual void ReleaseFilter(void);                                           // release the filter
  *     \endcode
  *
  *
  *   Do not make the confusion between:
  *     - a filter object : wxVirtualDataViewFilter (called "filter" for short)
  *       It performs the actual filtering
  *     - a filter editor : wxVirtualDataViewIFilterEditor (called "editor" for short)
  *       It allows the user to modify the filter via controls such as wxCombobox
  *
  *   The wxVirtualDataViewIFilterEditor object owns by default a filter object, allocated with "new"
  *   This is the most common strategy, and default implementation is provided for it.
  *   For wxVirtualDataViewCtrl, it works well because:
  *     - the wxVirtualDataViewCtrl owns the list of columns
  *     - the list of columns owns each column
  *     - each columns owns its wxVirtualDataViewIFilterEditor
  *   So as long as the wxVirtualDataViewCtrl is not destroyed, the filters are valid.
  *   If the filter object should not be owned by the editor, then the following methods
  * should be overridden:
  *     virtual wxVirtualDataViewFilter* GetFilter(void);                           //get the filter
  *     virtual void SetFilter(wxVirtualDataViewFilter *pFilter);                   //set the filter
  *     virtual void ReleaseFilter(void);                                           //release the filter IMPORTANT !
  *
  * Especially ReleaseFilter() should be overridden, because it is called by the destructor.
  */
class WXDLLIMPEXP_VDV wxVirtualDataViewIFilterEditor : public wxEvtHandler
{
    public:
        //constructors & destructor
        wxVirtualDataViewIFilterEditor(void);                                       ///< \brief constructor
        virtual ~wxVirtualDataViewIFilterEditor(void);                              ///< \brief destructor

        //interface for UI
        virtual wxVirtualDataViewIFilterEditor* Clone(void) const           = 0;    ///< \brief clone the filter editor
        virtual wxSize    GetSize(wxWindow *pWin)                           = 0;    ///< \brief get the size of the filter editor
        virtual void      Draw(wxWindow *pWin, wxDC &rDC, wxRect &rRect);           ///< \brief draw the filter editor
        virtual bool      StartFilter(wxWindow *pParent, wxRect &rRect)     = 0;    ///< \brief activate the filter editor
        virtual bool      CanFinishFilterEdition(void)                      = 0;    ///< \brief return true if filter edition can be finished
        virtual bool      FinishFilterEdition(bool bCancelled = false)      = 0;    ///< \brief end filter edition
        virtual wxWindow* GetEditorWindow(void)                             = 0;    ///< \brief get the editor window
        virtual void      ClearFilter(void)                                 = 0;    ///< \brief clear all items in the filter editor
        virtual void      InitFilter(wxVirtualIDataModel *pSrcModel,
                                     size_t uiField,
                                     wxVirtualIStateModel *pStateModel)     = 0;    ///< \brief initialize the filter
        virtual bool      IsFiltering(void)                                 = 0;    ///< \brief check if the filter is currently filtering something

        //interface for filter
        virtual wxVirtualDataViewFilter* GetFilter(void);                           ///< \brief get the filter

    protected:
        //data
        wxSize                      m_DefaultSize;                                  ///< \brief default size
        wxVirtualDataViewFilter*    m_pFilter;                                      ///< \brief the current filter
        wxBitmap                    m_EditorBitmap;                                 ///< \brief bitmap of the editor

        //methods
        void CopyBase(const wxVirtualDataViewIFilterEditor &rhs);                   ///< \brief copy base object
        void DestroyEditor(wxWindow* pEditor);                                      ///< \brief helper method for destroying an editor
        virtual void CaptureToBitmap(wxWindow* pEditor);                            ///< \brief capture the editor to a bitmap

        virtual void NotifyFilterBegin(void);                                       ///< \brief notify the parent that the filter edition has begun
        virtual void NotifyFilterChanged(void);                                     ///< \brief notify the parent that the filter value has changed
        virtual void NotifyFilterEnd(bool bCancelled);                              ///< \brief notify the parent that the filter edition was finished
        void DoNotify(wxEventType eType, int iValue = 0);                           ///< \brief notify the parent

        //filter methods
        virtual void SetFilter(wxVirtualDataViewFilter *pFilter);                   ///< \brief set the filter
        virtual void ReleaseFilter(void);                                           ///< \brief release the filter
};

#endif

