/**********************************************************************/
/** FILE    : wxVirtualItemID.h                                      **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : a data view item ID                                    **/
/**********************************************************************/


#ifndef _VIRTUAL_ITEM_ID_H_
#define _VIRTUAL_ITEM_ID_H_

#include <wx/defs.h>
#include <wx/VirtualDataView/VirtualDataViewDefs.h>
#include <wx/vector.h>

//------------------------- DECLARATION -----------------------------//
class WXDLLIMPEXP_VDV wxVirtualIDataModel;
/** \class wxVirtualItemID \brief encapsulates an item ID
  *
  * <B> OVERVIEW : </B>
  * Encapsulates an item ID
  * An item ID identifies uniquely an item inside a wxVirtualIDataModel.
  * The implementation of wxVirtualIDataModel is responsible for allocating these items, and
  * for interpreting them correctly.
  *
  * The item ID stores:
  *     - an array of 3 size_t. The wxVirtualIDataModel is free to interpret them however it likes,
  *       with the caveat that a all-0 item ID has a special meaning (see later)
  *     - a child index (size_t): this is to speed up several key algorithms, without requiring
  *       complex implementation in wxVirtualIDataModel.
  *       The special value size_t(-1) means that the child index is not known - in this case,
  *       the algorithms relying on it will do a linear search to find it back.
  *     - the pointer to the wxVirtualIDataModel to which it refers
  *
  * Note that the item ID does not use dynamic memory allocation. So it can it efficiently passed around as
  * return value / arguments in functions.
  *
  * <B> COMPARISONS & HASHING: </B>
  * There is also built-in comparison / equality operators (==, !=, <, >, <=, >=, IsSame(), IsLess())
  * and hash code calculation (see GetHashCode()). This makes the item easy to use inside various containers,
  * and it respond well to sorting algorithms.
  * For wxHashMap and wxHashSet, the functions wxVirtualItemIDHash and wxVirtualItemIDEqual are defined as well
  * For hashing & comparison, only the 3 size_t are taken into account. The child index and the model pointer
  * are not used. This may be counter-intuitive, but it is needed in case proxy models are used (for sorting/filtering)
  * This avoids some code complexity
  *
  * Example why it is needed:
  *        the object wxVirtualItemID stores the pointer to the linked data model.
  *        This is for convenience purpose only, and it should be treated with care.
  *        Comparison operators (==, !=, <, >, <=, >=) and GetHashCode do not take them into account
  *        The reason is because of the layering proxy data models on top of main data models (for sorting and filtering for example)
  *        Example :
  *             \code
  *             wxVirtualIDataModel *pDataModel = ... // get main data model here
  *             wxVirtualItemID idRoot = pDataModel->GetRootItem();
  *             wxVirtualItemID idChild1 = pDataModel->GetChild(idRoot, 0);
  *             ...
  *             pDataModel = ApplySortingFilters(); //some user method which apply a wxVirtualSortingDataModel on top of pDataModel
  *             idRoot = pDataModel->GetRootItem();
  *             wxVirtualItemID idChild2 = pDataModel->GetChild(idRoot, 0);
  *             if (idChild1 == idChild2) //BAM! here idChild1.GetModel() != idChild2.GetModel() but they may represent the same item...
  *             \endcode
  *
  * <B> SPECIAL ITEMS : </B>
  * Special values for item ID: (?? means : the value is not tested, and any value is acceptable)
  *     ------------------------------------------------------------------------------------
  *     | Description   | size_t #1 | size_t #2 | size_t #3 | Model  |  Method to call     |
  *     |               |  (ID)     |  (row)    | (column)  | pointer|  for checking       |
  *     ------------------------------------------------------------------------------------
  *     | Invalid       |   0       |  0        |  0        |  NULL  |  IsInvalid()        |
  *     | Root          |   0       |  0        |  0        | != NULL|  IsRootItem()       |
  *     | RootOrInvalid |   0       |  0        |  0        | ??     |  IsRootOrInvalid()  |
  *     | OK            |   != 0    |  ??       |  ??       | != NULL|  IsOK()             |
  *     ------------------------------------------------------------------------------------
  *         - The Root item is a valid item, with all values set to 0. It mains usage is to
  *           identify to top-most item in the hierarchy. It has no parent.
  *           All its size_t members are set to 0.
  *           Its model pointer is VALID.
  *         - The normal items ("OK") are the most common items. There are nearly no conditions on them
  *           (the first size_t must be != 0, and the model pointer must be valid)
  *         - An Invalid item is created with the default constructor. It should be returned by the wxVirtualIDataModel
  *           when an item does not exist (wxVirtualIDataModel::GetParent(), wxVirtualIDataModel::GetChild(),
  *           wxVirtualIDataModel::FindItem, wxVirtualIDataModel::NextItem/PrevItem, search methods, ...)
  *         - RootOrInvalid is a shortcut for (Invalid OR Root). This is needed in several places in the code
  *           and has thus been optimized.
  *
  * <B> STORING, RETRIEVING : </B>
  * Ultimately, the wxVirtualIDataModel is responsible for interpreting the meaning of the wxVirtualItemID.
  * Therefore, storing & retrieving the item IDs depends solely on the underlying data model, namely if it uses
  * persistent IDs or not (persistent ID: the same ID refers ALWAYS to the same data).
  * The code itself does not store the wxVirtualItemID. It dynamically queries the wxVirtualIDataModel each time it is needed.
  * There are few exceptions to this rule:
  *     - the default implementation of wxVirtualIStateModel: this implementation assumes PERSISTENT wxVirtualItemID
  *     - the wxVirtualIModelRenderer implementation wxVirtualTreeModelRenderer. It stores a wxVirtualItemID indirectly
  *       using a wxVirtualTreePath item. This should be OK even for not persistent item IDs, but if the model is changing
  *       very often, then an override of it might be needed (ConvertRowToTreePath() needs to be overridden)
  *
  * In case persistent ID cannot / should not be used, then it is possible to use the utility class
  * wxVirtualTreePath : it stores an item using its tree path (list of child index from root node)
  *
  * <B> CROSS-MODEL MANAGEMENT : </B>
  * It is not recommended to compare items belonging to distinct wxVirtualIDataModel. Most of the time,
  * it has no sense. If it is however needed / desired for your specific application, it is certainly possible.
  * Keep in mind that the wxVirtualItemID comparison and hashing methods do NOT take into account the data model pointer.
  * So in short, the code will probably do the right thing when you compare/store/manage items belonging to several different models:
  * it will assume the ID part (the 3 size_t) identifies uniquely the item WHATEVER the data model created it.
  * If it is not desired, then you have several possibilities:
  *     1 - redesign your data models. Use only one data model, and use fully the 3 size_t of the items ID.
  *         You can store void pointers there, and make your data model manages the memory behind the scenes.
  *     2 - use custom comparison/hashing methods wherever necessary
  *     3 - implement your own wxVirtualIModelRenderer. This is a massive work. For this reason, many methods inside the built-in
  *         wxVirtualTreeModelRenderer are virtual, allowing easy patching
  *
  * <B> CHILD INDEX : </B>
  * The child index is normally reserved for data models which cannot find it back easily.
  * This is especially true for sorting / filtering proxy models.
  * The rules are:
  *     - only the top-most data model in the hierarchy should use it
  *     - it is not part of the item ID itself. Therefore, no comparisons/hashing are done on it
  *     - it is initialized to size_t(-1)
  *     - it is the responsibility of the data model to set/get it (see GetChildIndex() / SetChildIndex())
  *

  */
class WXDLLIMPEXP_VDV wxVirtualItemID
{
    public:

        //constructors & destructor
        wxVirtualItemID(void);                                              ///< \brief default constructor
        wxVirtualItemID(void *pID,
                        size_t uiRow = 0, size_t uiCol = 0,
                        wxVirtualIDataModel *pModel = WX_VDV_NULL_PTR);     ///< \brief constructor from a void pointer
        explicit wxVirtualItemID(wxInt32 iData32,
                                 size_t uiRow = 0, size_t uiCol = 0,
                            wxVirtualIDataModel *pModel = WX_VDV_NULL_PTR); ///< \brief constructor from a 32 bits signed integer
        explicit wxVirtualItemID(wxUint32 uiData32,
                                 size_t uiRow = 0, size_t uiCol = 0,
                            wxVirtualIDataModel *pModel = WX_VDV_NULL_PTR); ///< \brief constructor from a 32 bits unsigned integer
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
        explicit wxVirtualItemID(wxInt64 iData64,
                                 size_t uiRow = 0, size_t uiCol = 0,
                            wxVirtualIDataModel *pModel = WX_VDV_NULL_PTR); ///< \brief constructor from a 64 bits signed integer
        explicit wxVirtualItemID(wxUint64 uiData64,
                                 size_t uiRow = 0, size_t uiCol = 0,
                            wxVirtualIDataModel *pModel = WX_VDV_NULL_PTR); ///< \brief constructor from a 64 bits unsigned integer
#endif // WX_VDD_ALLOW_64BITS_ITEMID
        ~wxVirtualItemID(void);                                             ///< \brief destructor

        //core methods - pure client data
        const void*  GetClientData(void) const;                             ///< \brief get the client data
        void   SetClientData(const void *pData);                            ///< \brief set the client data
        static size_t GetClientDataSize(void);                              ///< \brief get the client data size

        //methods - ID
        bool IsOK(void) const;                                              ///< \brief check if the item is OK (model OK + ID != 0)
        bool IsRootItem(void) const;                                        ///< \brief check if the item is the root. (model OK + ID == 0)
        bool IsRoot(void) const;                                            ///< \brief check if the item is the root. (model OK + ID == 0)
        bool IsInvalid(void) const;                                         ///< \brief check if the item is Invalid (model NOK + ID == 0)
        bool IsRootOrInvalid(void) const;                                   ///< \brief check if the item is Root or Invalid (ID == 0, model not checked)
        void* GetID(void) const;                                            ///< \brief get the item ID

         //convenience ID - 32 bits integers
        void SetInt32(wxInt32 iValue);                                      ///< \brief set the item from a signed 32 bits value
        void SetUInt32(wxUint32 iValue);                                    ///< \brief set the item from an usigned 32 bits value
        wxInt32 GetInt32(void) const;                                       ///< \brief get the item from a signed 32 bits value
        wxUint32 GetUInt32(void) const;                                     ///< \brief get the item from an unsigned 32 bits value

        //convenience ID - 64 bits integers
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
        void SetInt64(wxInt64 iValue);                                      ///< \brief set the item from a signed 64 bits value
        void SetUInt64(wxUint64 iValue);                                    ///< \brief set the item from an unsigned 64 bits value
        wxInt64 GetInt64(void) const;                                       ///< \brief get the item from a signed 64 bits value
        wxUint64 GetUInt64(void) const;                                     ///< \brief get the item from an unsigned 64 bits value
#endif // WX_VDD_ALLOW_64BITS_ITEMID

        //methods : row
        size_t GetRow(void)    const;                                       ///< \brief get the row of the item
        void SetRow(size_t uiRow);                                          ///< \brief set the row of the item

        //methods : column
        size_t GetColumn(void) const;                                       ///< \brief get the column of the item
        void SetColumn(size_t uiRow);                                       ///< \brief set the column of the item

        //method : model
        wxVirtualIDataModel* GetTopModel(void) const;                       ///< \brief get the top model
        void SetModel(wxVirtualIDataModel* pModel);                         ///< \brief set the model

        //convenience: parent, child, sibling
        wxVirtualItemID GetParent(void) const;                              ///< \brief get the parent item
        wxVirtualItemID GetChild(size_t uiIndex) const;                     ///< \brief get the child item
        size_t          GetChildCount(void) const;                          ///< \brief get the amount of children of the item
        size_t          FindChildIndex(void) const;                         ///< \brief get the index of the item (use the wxVirtualIDataModel)
        size_t          GetDepth(void) const;                               ///< \brief get the depth of the item

        //child index
        size_t          GetChildIndex(void) const;                          ///< \brief get the child index
        void            SetChildIndex(size_t uiChildIndex);                 ///< \brief set the child index
        void            ResetChildIndex(void);                              ///< \brief set the child index to "don't know"
        bool            HasChildIndex(void) const;                          ///< \brief check if the child index is valid

        //comparison
        bool IsSame(const wxVirtualItemID &rhs) const;                      ///< \brief check if 2 items are identical
        bool IsLess(const wxVirtualItemID &rhs) const;                      ///< \brief check if *this < rhs

        bool operator==(const wxVirtualItemID &rhs) const;                  ///< \brief operator ==
        bool operator!=(const wxVirtualItemID &rhs) const;                  ///< \brief operator !=
        bool operator<(const wxVirtualItemID &rhs) const;                   ///< \brief operator <
        bool operator<=(const wxVirtualItemID &rhs) const;                  ///< \brief operator <=
        bool operator>(const wxVirtualItemID &rhs) const;                   ///< \brief operator >
        bool operator>=(const wxVirtualItemID &rhs) const;                  ///< \brief operator >=

        //hash
        unsigned long GetHashCode(void) const;                              ///< \brief return a hash code

    protected:
        //union
        /// \union stores data as void* or integers
        union UItemID
        {
            void*       m_pData;                                            ///< \brief void pointer
            wxInt32     m_iData32;                                          ///< \brief 32 bits signed integer
            wxUint32    m_uiData32;                                         ///< \brief 32 bits unsigned integer
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
            wxUint8     m_CharData[8];                                      ///< \brief 8 bytes
            wxInt64     m_iData64;                                          ///< \brief 64 bits signed integer
            wxUint64    m_uiData64;                                         ///< \brief 64 bits unsigned integer
#else
            wxUint8     m_CharData[4];                                      ///< \brief 4 bytes
#endif // WX_VDD_ALLOW_64BITS_ITEMID
        };

        /// \struct TStandardID: convenience for storing 1 pointer/id + 1 row and 1 column
        struct TStandardID
        {
            UItemID     m_ID;                                               ///< \brief the item ID
            size_t      m_uiRow;                                            ///< \brief row of the item
            size_t      m_uiCol;                                            ///< \brief column of the item
        };

        /// \union UData : either TStandardID or client data
        union UData
        {
            TStandardID m_StandardID;                                       ///< \brief standard item ID
            wxUint8     m_Data[sizeof(TStandardID)];                        ///< \brief custom item ID
        };

        //data
        UData                       m_Data;                                 ///< \brief the item ID
        wxVirtualIDataModel*        m_pModel;                               ///< \brief the item model
        size_t                      m_uiChildIndex;                         ///< \brief child index

        //methods
        void Init(void);                                                    ///< \brief initialization to invalid
};

typedef wxVector<wxVirtualItemID> wxVirtualItemIDs;

//----------------- HASHING FUNCTORS --------------------------------//
class wxVirtualItemIDHash
{
    public:
        wxVirtualItemIDHash(void) {}
        unsigned long operator()(const wxVirtualItemID &key) const
        {
            return(key.GetHashCode());
        }
        wxVirtualItemIDHash& operator=(const wxVirtualItemIDHash &rhs)
        {
            return(*this);
        }
};

class wxVirtualItemIDEqual
{
    public:
        wxVirtualItemIDEqual(void) {}
        unsigned long operator()(const wxVirtualItemID &a,
                                 const wxVirtualItemID &b) const
        {
            return(a == b);
        }
        wxVirtualItemIDEqual& operator=(const wxVirtualItemIDEqual &rhs)
        {
            return(*this);
        }
};

//--------------------- INLINE IMPLEMENTATION -----------------------//
//---------------- CONSTRUCTORS & DESTRUCTOR ------------------------//
/** Default constructor
  */
WX_VDV_INLINE wxVirtualItemID::wxVirtualItemID(void)
{
    Init();
}

/** Constructor from a void pointer
  * \param pID    [input]: the item ID
  * \param uiRow  [input]: the item row
  * \param uiCol  [input]: the item column
  * \param pModel [input]: the model to which the item ID belongs
  */
WX_VDV_INLINE wxVirtualItemID::wxVirtualItemID(void *pID,
                                               size_t uiRow, size_t uiCol,
                                               wxVirtualIDataModel *pModel)
{
    //force initialization, otherwise some bits may not be initialized correctly
    Init();

    m_Data.m_StandardID.m_ID.m_pData = pID;
    m_Data.m_StandardID.m_uiRow = uiRow;
    m_Data.m_StandardID.m_uiCol = uiCol;
    m_pModel = pModel;
}

/** Constructor from a signed 32 bits integer
  * \param iData32 [input]: the item ID
  * \param uiRow   [input]: the item row
  * \param uiCol   [input]: the item column
  * \param pModel  [input]: the model to which the item ID belongs
  */
WX_VDV_INLINE wxVirtualItemID::wxVirtualItemID(wxInt32 iData32,
                                               size_t uiRow, size_t uiCol,
                                               wxVirtualIDataModel *pModel)
{
    //force initialization, otherwise some bits may not be initialized correctly
    Init();

    m_Data.m_StandardID.m_ID.m_iData32 = iData32;
    m_Data.m_StandardID.m_uiRow = uiRow;
    m_Data.m_StandardID.m_uiCol = uiCol;
    m_pModel = pModel;
}

/** Constructor from an unsigned 32 bits integer
  * \param uiData32 [input]: the item ID
  * \param uiRow   [input]: the item row
  * \param uiCol   [input]: the item column
  * \param pModel  [input]: the model to which the item ID belongs
  */
WX_VDV_INLINE wxVirtualItemID::wxVirtualItemID(wxUint32 uiData32,
                                               size_t uiRow, size_t uiCol,
                                               wxVirtualIDataModel *pModel)
{
    //force initialization, otherwise some bits may not be initialized correctly
    Init();

    m_Data.m_StandardID.m_ID.m_uiData32 = uiData32;
    m_Data.m_StandardID.m_uiRow = uiRow;
    m_Data.m_StandardID.m_uiCol = uiCol;
    m_pModel = pModel;
}

#if WX_VDD_ALLOW_64BITS_ITEMID != 0

/** Constructor from a signed 64 bits integer
  * \param iData64 [input]: the item ID
  * \param uiRow   [input]: the item row
  * \param uiCol   [input]: the item column
  * \param pModel  [input]: the model to which the item ID belongs
  */
WX_VDV_INLINE wxVirtualItemID::wxVirtualItemID(wxInt64 iData64,
                                               size_t uiRow, size_t uiCol,
                                               wxVirtualIDataModel *pModel)
{
    //force initialization, otherwise some bits may not be initialized correctly
    Init();

    m_Data.m_StandardID.m_ID.m_iData64 = iData64;
    m_Data.m_StandardID.m_uiRow = uiRow;
    m_Data.m_StandardID.m_uiCol = uiCol;
    m_pModel = pModel;
}

/** Constructor from an unsigned 64 bits integer
  * \param uiData64 [input]: the item ID
  * \param uiRow   [input]: the item row
  * \param uiCol   [input]: the item column
  * \param pModel  [input]: the model to which the item ID belongs
  */
WX_VDV_INLINE wxVirtualItemID::wxVirtualItemID(wxUint64 uiData64,
                                               size_t uiRow, size_t uiCol,
                                               wxVirtualIDataModel *pModel)
{
    //force initialization, otherwise some bits may not be initialized correctly
    Init();

    m_Data.m_StandardID.m_ID.m_uiData64 = uiData64;
    m_Data.m_StandardID.m_uiRow = uiRow;
    m_Data.m_StandardID.m_uiCol = uiCol;
    m_pModel = pModel;
}

#endif // WX_VDD_ALLOW_64BITS_ITEMID

/** Destructor
  */
WX_VDV_INLINE wxVirtualItemID::~wxVirtualItemID(void)
{
}

//------------------------ INTERNAL ---------------------------------//
/** Initialization to invalid
  */
WX_VDV_INLINE void wxVirtualItemID::Init(void)
{
    #if WX_VDD_ALLOW_64BITS_ITEMID != 0
    m_Data.m_StandardID.m_ID.m_uiData64 = 0;
#else
    m_Data.m_StandardID.m_ID.m_uiData32 = 0;
#endif // WX_VDD_ALLOW_64BITS_ITEMID
    m_Data.m_StandardID.m_uiRow = 0;
    m_Data.m_StandardID.m_uiCol = 0;
    m_pModel = WX_VDV_NULL_PTR;
    m_uiChildIndex = size_t(-1);
}

//---------- CORE METHODS - PURE CLIENT DATA ------------------------//
/** Get the client data
  * \return a pointer to the internal buffer
  *         The buffer is still owned by the wxVirtualItemID object: do not delete it
  *         The size of the buffer is returned by GetClientDataSize static method
  *         It is determined at compile time
  */
WX_VDV_INLINE const void* wxVirtualItemID::GetClientData(void) const
{
    return(m_Data.m_Data);
}

/** Set the client data
  * \param pData [input]: the data to copy to the internal buffer
  *                       It must be at least GetClientDataSize() bytes
  *                       Only the bytes which fit in the internal buffer are copied
  *                       if NULL, the data is initialized to 0
  */
WX_VDV_INLINE void wxVirtualItemID::SetClientData(const void *pData)
{
    if (pData) memcpy(m_Data.m_Data, pData, sizeof(TStandardID));
    else       memset(m_Data.m_Data, 0, sizeof(TStandardID));
}

/** Get the client data size
  * \return the size of the internal buffer, in bytes.
  *         This is a compile time constant (implemented using sizeof())
  */
WX_VDV_INLINE size_t wxVirtualItemID::GetClientDataSize(void)
{
    return(sizeof(TStandardID));
}

//------------------------ METHODS ----------------------------------//
/** Check if the item is OK (model OK + ID != 0)
  * \return true if the item ID is OK, false otherwise
  */
WX_VDV_INLINE bool wxVirtualItemID::IsOK(void) const
{
    if (!m_pModel) return(false);

#if WX_VDD_ALLOW_64BITS_ITEMID != 0
    if (m_Data.m_StandardID.m_ID.m_uiData64) return(true);
#else
    if (m_Data.m_StandardID.m_ID.m_pData) return(true);
#endif // WX_VDD_ALLOW_64BITS_ITEMID
    return(false);
}

/** Check if the item is the root. (model OK + ID == 0)
  * \return true if the item is the root item, false otherwise
  */
WX_VDV_INLINE bool wxVirtualItemID::IsRootItem(void) const
{
    if (!m_pModel) return(false);

    #if WX_VDD_ALLOW_64BITS_ITEMID != 0
    if (m_Data.m_StandardID.m_ID.m_uiData64) return(false);
#else
    if (m_Data.m_StandardID.m_ID.m_pData) return(false);
#endif // WX_VDD_ALLOW_64BITS_ITEMID
    return(true);
}

/** Check if the item is invalid (model NOK + ID == 0)
  * \return true if the item ID is OK, false otherwise
  */
WX_VDV_INLINE bool wxVirtualItemID::IsInvalid(void) const
{
    if (m_pModel) return(false);

#if WX_VDD_ALLOW_64BITS_ITEMID != 0
    if (m_Data.m_StandardID.m_ID.m_uiData64) return(false);
#else
    if (m_Data.m_StandardID.m_ID.m_pData) return(false);
#endif // WX_VDD_ALLOW_64BITS_ITEMID
    return(true);
}

/** Check if the item is Root or Invalid (ID == 0, model not checked)
  * \return true if the item is the Root item
  *         true if the item is an Invalid item
  *         false in all other cases
  */
WX_VDV_INLINE bool wxVirtualItemID::IsRootOrInvalid(void) const
{
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
    if (m_Data.m_StandardID.m_ID.m_uiData64) return(false);
#else
    if (m_Data.m_StandardID.m_ID.m_pData) return(false);
#endif // WX_VDD_ALLOW_64BITS_ITEMID
    return(true);
}

/** Get the item ID
  * \return the item ID
  */
WX_VDV_INLINE void* wxVirtualItemID::GetID(void) const
{
    return(m_Data.m_StandardID.m_ID.m_pData);
}

//---- CONVENIENCE - 32 BITS INTEGERS -------------------------------//
/** Set the item from a signed 32 bits value
  * \param iValue [input]: the new item value
  */
WX_VDV_INLINE void wxVirtualItemID::SetInt32(wxInt32 iValue)
{
    m_Data.m_StandardID.m_ID.m_iData32 = iValue;
}

/** Set the item from an usigned 32 bits value
  * \param iValue [input]: the new item value
  */
WX_VDV_INLINE void wxVirtualItemID::SetUInt32(wxUint32 uiValue)
{
    m_Data.m_StandardID.m_ID.m_uiData32 = uiValue;
}

/** Get the item from a signed 32 bits value
  * \return the ID as a signed 32 bits value
  */
WX_VDV_INLINE wxInt32 wxVirtualItemID::GetInt32(void) const
{
    return(m_Data.m_StandardID.m_ID.m_iData32);
}

/** Get the item from an unsigned 32 bits value
  * \return the ID as an unsigned 32 bits value
  */
WX_VDV_INLINE wxUint32 wxVirtualItemID::GetUInt32(void) const
{
    return(m_Data.m_StandardID.m_ID.m_uiData32);
}

//---- CONVENIENCE - 64 BITS INTEGERS -------------------------------//
#if WX_VDD_ALLOW_64BITS_ITEMID != 0
/** Set the item from a signed 64 bits value
  * \param iValue [input]: the new item value
  */
WX_VDV_INLINE void wxVirtualItemID::SetInt64(wxInt64 iValue)
{
    m_Data.m_StandardID.m_ID.m_iData64 = iValue;
}

/** Set the item from an unsigned 64 bits value
  * \param iValue [input]: the new item value
  */
WX_VDV_INLINE void wxVirtualItemID::SetUInt64(wxUint64 uiValue)
{
    m_Data.m_StandardID.m_ID.m_uiData64 = uiValue;
}

/** Get the item from a signed 64 bits value
  * \return the ID as a signed 64 bits value
  */
WX_VDV_INLINE wxInt64 wxVirtualItemID::GetInt64(void) const
{
    return(m_Data.m_StandardID.m_ID.m_iData64);
}

/** Get the item from an unsigned 64 bits value
  * \return the ID as an unsigned 64 bits value
  */
WX_VDV_INLINE wxUint64 wxVirtualItemID::GetUInt64(void) const
{
    return(m_Data.m_StandardID.m_ID.m_uiData64);
}
#endif // WX_VDD_ALLOW_64BITS_ITEMID

//----------------------- ROW ---------------------------------------//

/** Get the row of the item
  * \return the row of the item
  */
WX_VDV_INLINE size_t wxVirtualItemID::GetRow(void) const
{
    return(m_Data.m_StandardID.m_uiRow);
}

/** Set the row of the item
  * \param uiRow [input] the row of the item
  */
WX_VDV_INLINE void wxVirtualItemID::SetRow(size_t uiRow)
{
    m_Data.m_StandardID.m_uiRow = uiRow;
}

//---------------------------- COLUMN -------------------------------//

/** Get the column of the item
  * \return the column of the item
  */
WX_VDV_INLINE size_t wxVirtualItemID::GetColumn(void) const
{
    return(m_Data.m_StandardID.m_uiCol);
}

/** Set the column of the item
  * \param uiCol [input] the column of the item
  */
WX_VDV_INLINE void wxVirtualItemID::SetColumn(size_t uiCol)
{
    m_Data.m_StandardID.m_uiCol = uiCol;
}

//-------------------------- MODEL ----------------------------------//
/** Set the model of the item
  * \param pModel [input]: the model to which the item belongs
  *                        The model object is not owned
  */
WX_VDV_INLINE void wxVirtualItemID::SetModel(wxVirtualIDataModel* pModel)
{
    m_pModel = pModel;
}

//----------------------- CHILD INDEX -------------------------------//
/** Get the child index
  * \return the child index
  */
WX_VDV_INLINE size_t wxVirtualItemID::GetChildIndex(void) const
{
    return(m_uiChildIndex);
}

/** Set the child index
  * \param uiChildIndex [input]: the new child index
  */
WX_VDV_INLINE void wxVirtualItemID::SetChildIndex(size_t uiChildIndex)
{
    m_uiChildIndex = uiChildIndex;
}

/** Set the child index to "don't know" (== size_t(-1))
  */
WX_VDV_INLINE void wxVirtualItemID::ResetChildIndex(void)
{
    m_uiChildIndex = size_t(-1);
}

/** Check if the child index is valid
  * \return true if the child index is valid, false otherwise
  */
WX_VDV_INLINE bool wxVirtualItemID::HasChildIndex(void) const
{
    return(m_uiChildIndex != size_t(-1));
}

//----------------------- COMPARISON --------------------------------//
/** Check if 2 items are identical
  * \param rhs [input] : the second object
  * \return true if *this == rhs, false otherwise
  */
WX_VDV_INLINE bool wxVirtualItemID::IsSame(const wxVirtualItemID &rhs) const
{
    //reminder : do not take into account m_pModel and m_uiChildIndex

    const TStandardID &rLeftID  = m_Data.m_StandardID;
    const TStandardID &rRightID = rhs.m_Data.m_StandardID;
    if (rLeftID.m_ID.m_pData  != rRightID.m_ID.m_pData) return(false);
    if (rLeftID.m_uiRow  != rRightID.m_uiRow) return(false);
    if (rLeftID.m_uiCol  != rRightID.m_uiCol) return(false);
    return(true);
}

/** Check if *this < rhs
  * \param rhs [input] : the second object
  * \return true if *this == rhs, false otherwise
  */
WX_VDV_INLINE bool wxVirtualItemID::IsLess(const wxVirtualItemID &rhs) const
{
    //reminder : do not take into account m_pModel and m_uiChildIndex

    const TStandardID &rLeftID  = m_Data.m_StandardID;
    const TStandardID &rRightID = rhs.m_Data.m_StandardID;

    if (rLeftID.m_ID.m_pData  < rRightID.m_ID.m_pData) return(true);
    if (rLeftID.m_ID.m_pData  > rRightID.m_ID.m_pData) return(false);

    if (rLeftID.m_uiRow  < rRightID.m_uiRow) return(true);
    if (rLeftID.m_uiRow  > rRightID.m_uiRow) return(false);

    if (rLeftID.m_uiCol  < rRightID.m_uiCol) return(true);
    if (rLeftID.m_uiCol  > rRightID.m_uiCol) return(false);

    return(false);
}

//----------------- COMPARISON OPERATORS ----------------------------//
/** Operator == **/
WX_VDV_INLINE bool wxVirtualItemID::operator==(const wxVirtualItemID &rhs) const
{
    return(IsSame(rhs));
}

/** Operator != **/
WX_VDV_INLINE bool wxVirtualItemID::operator!=(const wxVirtualItemID &rhs) const
{
    //reminder : do not take into account m_pModel and m_uiChildIndex
    //if (m_pModel != rhs.m_pModel) return(true);

    const TStandardID &rLeftID  = m_Data.m_StandardID;
    const TStandardID &rRightID = rhs.m_Data.m_StandardID;
    if (rLeftID.m_ID.m_pData  != rRightID.m_ID.m_pData ) return(true);
    if (rLeftID.m_uiRow  != rRightID.m_uiRow) return(true);
    if (rLeftID.m_uiCol  != rRightID.m_uiCol) return(true);
    return(false);
}

/** Operator < **/
WX_VDV_INLINE bool wxVirtualItemID::operator<(const wxVirtualItemID &rhs) const
{
    return(IsLess(rhs));
}

/** Operator <= **/
WX_VDV_INLINE bool wxVirtualItemID::operator<=(const wxVirtualItemID &rhs) const
{
    if (IsLess(rhs)) return(true);
    if (IsSame(rhs)) return(true);
    return(false);
}

/** Operator > **/
WX_VDV_INLINE bool wxVirtualItemID::operator>(const wxVirtualItemID &rhs) const
{
    return(rhs.IsLess(*this));
}

/** Operator >= **/
WX_VDV_INLINE bool wxVirtualItemID::operator>=(const wxVirtualItemID &rhs) const
{
    if (rhs.IsLess(*this)) return(true);
    if (IsSame(rhs)) return(true);
    return(false);
}

//-------------------- HASH CODE ------------------------------------//
//For 32 bit machines:
const unsigned long s_uiFNVPrime = 16777619u;
const unsigned long s_uiFNVOffsetBasis = 2166136261u;

//For 64 bit machines:
//const unsigned long s_uiFNVPrime = 1099511628211u;
//const unsigned long s_uiFNVOffsetBasis = 14695981039346656037u;

WX_VDV_INLINE unsigned long wxComputeHashSizeT(size_t uiValue, unsigned long uiCurrentHash)
{
    unsigned long uiHash = uiCurrentHash;
    const size_t uiSize = sizeof(size_t);
    size_t uiByte;
    unsigned char *vBytes = (unsigned char *) &uiValue;
    for(uiByte = 0; uiByte < uiSize; uiByte++)
    {
        uiHash *= s_uiFNVPrime;
        uiHash ^= vBytes[uiByte];
    }
    return(uiHash);
}

/** Return a hash code
  * \return a hash code
  */
WX_VDV_INLINE unsigned long wxVirtualItemID::GetHashCode(void) const
{
    //reminder : do not take into account m_pModel and m_uiChildIndex

    //NOK: too many collisions
//    unsigned long ulResult = 0;
//
//    const TStandardID &rID = m_Data.m_StandardID;
//    ulResult = reinterpret_cast<size_t>(rID.m_ID.m_pData);
//    ulResult ^= (rID.m_uiRow << 16);
//    ulResult ^= (rID.m_uiCol << 16);

    //OK (murmurhash3 avalanche trigger)
//    unsigned long ulResult = 0;
//
//    const TStandardID &rID = m_Data.m_StandardID;
//    ulResult = reinterpret_cast<size_t>(rID.m_ID.m_pData);
//    ulResult = ulResult * 101 + rID.m_uiRow;
//    ulResult = ulResult * 101 + rID.m_uiCol;

    unsigned long ulResult = s_uiFNVOffsetBasis;
    const TStandardID &rID = m_Data.m_StandardID;
    ulResult = wxComputeHashSizeT(reinterpret_cast<size_t>(rID.m_ID.m_pData), ulResult);
    ulResult = wxComputeHashSizeT(rID.m_uiRow, ulResult);
    ulResult = wxComputeHashSizeT(rID.m_uiCol, ulResult);

    return(ulResult);
}

#endif

