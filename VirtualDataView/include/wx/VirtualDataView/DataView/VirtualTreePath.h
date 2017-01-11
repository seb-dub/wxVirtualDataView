/**********************************************************************/
/** FILE    : VirtualTreePath.h                                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : stores a tree path in a persistent manner              **/
/**********************************************************************/


#ifndef WX_VIRTUAL_I_TREE_PATH_H_
#define WX_VIRTUAL_I_TREE_PATH_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>

class WXDLLIMPEXP_VDV wxVirtualItemID;
class WXDLLIMPEXP_VDV wxVirtualIDataModel;
class WXDLLIMPEXP_VDV wxVirtualIStateModel;

/** \class wxVirtualTreePath : a class for storing an itemID in a persistent manner
  */
class WXDLLIMPEXP_VDV wxVirtualTreePath
{
    public:
        //constructors & destructor
        wxVirtualTreePath(void);                                                        ///< \brief default constructor
        wxVirtualTreePath(const wxVirtualItemID &rID);                                  ///< \brief constructor from an ItemID and a data model
        wxVirtualTreePath(const wxVirtualTreePath &rhs);                                ///< \brief copy constructor
        wxVirtualTreePath& operator=(const wxVirtualTreePath &rhs);                     ///< \brief assignation operator
        ~wxVirtualTreePath(void);                                                       ///< \brief destructor

        //set / get
        wxVirtualItemID Get(wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR) const;   ///< \brief retrieve the item ID stored
        wxVirtualItemID GetItemAtLevel(long lLevel,
                              wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR) const; ///< \brief retrieve the item ID at a specific level
        size_t GetIndexAtLevel(long lLevel) const;                                      ///< \brief get the index of the item at a specific level
        long  GetLevel(void) const;                                                     ///< \brief get the level of the item
        void Set(const wxVirtualItemID &rID,
                 wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR);                    ///< \brief set the item ID stored
        void Set(size_t uiRank,
                 wxVirtualIDataModel *pDataModel,
                 wxVirtualIStateModel *pStateModel,
                 const wxVirtualItemID &rStartPoint,
                 size_t uiStartingRank);                                                ///< \brief set from a rank
        void Set(size_t uiRank, wxVirtualIDataModel *pDataModel,
                 wxVirtualIStateModel *pStateModel);                                    ///< \brief set from a rank

        //data model
        void SetDataModel(wxVirtualIDataModel *pDataModel);                             ///< \brief set data model
        wxVirtualIDataModel* GetDataModel(void);                                        ///< \brief get data model

    protected:
        //typedefs
        static const size_t s_uiStaticSize = 10;                                        ///< \brief max depth of path stored statically
        /// \union UData : stores path data either statically or dynamically
        union UData
        {
            size_t  m_vPath[s_uiStaticSize];                                            ///< \brief path data - statically allocated
            size_t* m_pPath;                                                            ///< \brief path data - dynamically allocated
        };

        //data
        UData                   m_Path;                                                 ///< \brief path data
        size_t                  m_uiDepth;                                              ///< \brief depth of the path
        wxVirtualIDataModel*    m_pDataModel;                                           ///< \brief data model of the item

        //method
        void Init(void);                                                                ///< \brief initialize the object
        void Copy(const wxVirtualTreePath &rhs);                                        ///< \brief copy an object
        void Release(void);                                                             ///< \brief release an object
        void Build(const wxVirtualItemID &rID,
                   wxVirtualIDataModel *pDataModel = WX_VDV_NULL_PTR);                  ///< \brief set the item ID stored
};

#endif

