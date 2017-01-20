
#include "TestTreeDataModel.h"
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/artprov.h>

//3 levels hierarchy
//  level 1 : NB_ITEMS_LEVEL_1 items. Index starting at 1. Stored in main ID. Row = 0, Column = 0
//  level 2 : each level 1 item has NB_ITEMS_LEVEL_2 items. Index starting at 1. Stored in row ID, column ID = 0
//  level 3 : each level 2 item has NB_ITEMS_LEVEL_3 items. Index starting at 1. Stored in column ID

#define NB_ITEMS_LEVEL_1        10
#define NB_ITEMS_LEVEL_2        10000
#define NB_ITEMS_LEVEL_3        10

//-------------------- NESTED CLASS TID -----------------------------//
TestTreeDataModel::TID::TID(wxVirtualItemID id, size_t uiField)
                : m_id(id), m_uiField(uiField)
{}

bool TestTreeDataModel::TID::operator<(const TestTreeDataModel::TID &rhs) const
{
    if (m_id < rhs.m_id) return(true);
    if (rhs.m_id < m_id) return(false);

    return(m_uiField < rhs.m_uiField);
}

//------------------- CONSTRUCTORS & DESTRUCTOR ---------------------//
/** Default constructor
  */
TestTreeDataModel::TestTreeDataModel(bool bEmpty)
    : wxVirtualIDataModel(),
      m_bEmpty(bEmpty),
      m_bSpeedOptimization(true)
{
    m_attrEven.SetColour(wxColour(255, 0, 0));
    m_attrOdd.SetColour(wxColour(0, 0, 255));

    m_bmp1 = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU);
    m_bmp2 = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU);
    m_bmp3 = wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU);
    m_dt   = wxDateTime::Now();
}

/** Destructor
  */
TestTreeDataModel::~TestTreeDataModel(void)
{
}

//------------ INTERFACE IMPLEMENTATION -----------------------------//
/** Get the parent item ID
  * \param rID [input]: the child item
  * \return the parent item
  */
wxVirtualItemID TestTreeDataModel::GetParent(const wxVirtualItemID &rID)
{
    //root/invalid return invalid item
    if (m_bEmpty) return(CreateInvalidItemID());
    if (rID.IsRootOrInvalid()) return(CreateInvalidItemID());

    wxVirtualItemID id(rID);
    id.SetModel(this);

    //row != 0 & column != 0 : level 3 item, return level 2 item
    //row != 0 & column == 0 : level 2 item, return level 1 item
    if (rID.GetRow() != 0)
    {
        if (rID.GetColumn() != 0)
        {
            //this is a level 3 item. Return a level 2 item
            //level 2 item : column ID = 0
            id.SetColumn(0);
            id.SetChildIndex(rID.GetRow() - 1);
        }
        else
        {
            //this is a level 2 item. Return a level 1 item
            id.SetColumn(0);
            id.SetRow(0);
            id.SetChildIndex(rID.GetUInt32() - 1);
        }

        return(id);
    }
    else
    {
        //row = 0 : level 1 item -> return root item
        id.SetUInt32(0);
        id.SetRow(0);
        id.SetColumn(0);
        id.ResetChildIndex();
    }

    return(id);
}

/** Get the amount of children
  * \param rID [input]: the parent item to query
  * \return the amount of children of rID
  */
size_t TestTreeDataModel::GetChildCount(const wxVirtualItemID &rID)
{
    if (m_bEmpty) return(0);
    if (IsRootItem(rID)) return(NB_ITEMS_LEVEL_1);
    if (rID.GetRow() == 0) return(NB_ITEMS_LEVEL_2);
    if (rID.GetColumn() == 0) return(NB_ITEMS_LEVEL_3);
    return(0);
}

/** Get child item ID
  * \param rIDParent    [input]: the parent item ID
  * \param uiChildIndex [input]: the index of the child to get (0 based)
  * \return the ID of the child item
  */
wxVirtualItemID TestTreeDataModel::GetChild(const wxVirtualItemID &rIDParent,
                                            size_t uiChildIndex)
{
    if (m_bEmpty) return(CreateInvalidItemID());
    if (IsRootItem(rIDParent))
    {
        //parent is root : return a level 1 item
        if (uiChildIndex >= NB_ITEMS_LEVEL_1) return(CreateInvalidItemID());
        wxVirtualItemID id;
        id.SetUInt32(uiChildIndex + 1);
        id.SetRow(0);
        id.SetColumn(0);
        id.SetModel(this);
        id.SetChildIndex(uiChildIndex);
        return(id);
    }

    if (rIDParent.GetRow() == 0)
    {
        //parent is level 1 item : return a level 2 item
        if (uiChildIndex >= NB_ITEMS_LEVEL_2) return(CreateInvalidItemID());

        wxVirtualItemID id;
        id.SetUInt32(rIDParent.GetUInt32());
        id.SetRow(uiChildIndex + 1);
        id.SetColumn(0);
        id.SetModel(this);
        id.SetChildIndex(uiChildIndex);
        return(id);
    }

    if (rIDParent.GetColumn() == 0)
    {
       //parent is level 2 item : return a level 3 item
        if (uiChildIndex >= NB_ITEMS_LEVEL_3) return(CreateInvalidItemID());

        wxVirtualItemID id;
        id.SetUInt32(rIDParent.GetUInt32());
        id.SetRow(rIDParent.GetRow());
        id.SetColumn(uiChildIndex + 1);
        id.SetModel(this);
        id.SetChildIndex(uiChildIndex);
        return(id);
    }

    //invalid
    return(CreateInvalidItemID());
}

/** Get the amount of fields in the item
  * \return 3
  */
size_t TestTreeDataModel::GetFieldCount(const wxVirtualItemID &rID)
{
    //if (GetDepth(rID) == 1) return(size_t(-1));
    return(12);
}

/** Get the item data
  * \param rID   [input] : the ID of the item to query
  * \param uiField [input]: the field of the data to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  */
wxVariant TestTreeDataModel::GetItemData(const wxVirtualItemID &rID,
                                         size_t uiField, EDataType eType)
{
    //tooltip
    if (eType == WX_ITEM_TOOLTIP_DATA)
    {
        wxString sValue = wxString::Format("Tooltip ID=%d row=%d col=%d field=%d", rID.GetInt32(), rID.GetRow(), rID.GetColumn(), uiField);
        return(sValue);
    }

    //help
    if (eType == WX_ITEM_HELP_DATA)
    {
        wxString sValue = wxString::Format("Help ID=%d row=%d col=%d field=%d", rID.GetInt32(), rID.GetRow(), rID.GetColumn(), uiField);
        return(sValue);
    }

    if (eType != WX_ITEM_MAIN_DATA) return(wxVariant());
    if (m_bEmpty) return(wxVariant());
    if (!rID.IsOK()) return(wxVariant());


    TMapOfValues::iterator it = m_MapOfValues.find(TID(rID, uiField));
    if (it != m_MapOfValues.end())
    {
        wxVariant vValue = it->second;
//        long lValue = vValue.GetLong();
        return(vValue);
    }

    switch(uiField)
    {
        //bitmap + text
        case 0  :   {
                        //bitmap + text
                        wxString sValue = wxString::Format("ID=%d row=%d col=%d field=%d", rID.GetInt32(), rID.GetRow(), rID.GetColumn(), uiField);
                        wxBitmap bmp = wxNullBitmap;
                        switch(GetDepth(rID))
                        {
                            case 0 : bmp = m_bmp1; break;
                            case 1 : bmp = m_bmp2; break;
                            case 2 :
                            default: bmp = m_bmp3; break;
                        }
                        wxBitmapText bt(sValue, bmp);

                        wxVariant vValue;
                        vValue << bt;
                        return(vValue);
                    };
                    break;

        //boolean
        case 1  :   if ((rID.GetRow() % 2) == 0) return(wxVariant((long) 0));
                    return(wxVariant((long) 1));
                    break;

        //date / time
        case 4  :   //date
        case 5  :   //time
        case 6  :   //date time
                    return(wxVariant(m_dt));
                    break;

        //numbers
        case 7  :   return(wxVariant(rID.GetInt32()));
        case 8  :   return(wxVariant(rID.GetInt32()));
        case 9  :   return(wxVariant((double) rID.GetInt32()));

        //colour
        case 10 :   {
                        wxColour c;
                        c.SetRGB(rID.GetUInt32());
                        wxVariant v;
                        v << c;
                        return(v);
                    };
                    break;

        //progress bar
        case 11 :   return(wxVariant((long)50));

        //default (string)
        case 2  :
        case 3  :
        default :   break;
    }

    wxString sValue = wxString::Format("ID=%d row=%d col=%d field=%d", rID.GetInt32(), rID.GetRow(), rID.GetColumn(), uiField);
//    wxString sValue = wxString::Format("col=%d field=%d", rID.GetColumn(), uiField);
    return(wxVariant(sValue));
}

/** Set the item data
  * \param rID     [input]: the ID of the item to modify
  * \param uiField [input]: the field of the data to modify
  * \param vValue  [input]: the new value
  * \param eType   [input]: the kind of value to modify
  * \return true if the data was modified, false otherwise
  */
bool TestTreeDataModel::SetItemData(const wxVirtualItemID &rID, size_t uiField,
                                      const wxVariant &vValue,
                                      EDataType eType)
{
    if (eType != WX_ITEM_MAIN_DATA) return(true);
    if (m_bEmpty) return(false);

    TID tid(rID, uiField);
    m_MapOfValues.erase(tid);
    m_MapOfValues.insert(std::make_pair(tid, vValue));
    return(true);
}

/** Get the item graphic attributes
  * \param rID [input] : the ID of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param rState [input] : the state of the item
  * \return a pointer to the graphic attributes for this item. The data is owned by the model.
  *         returning a NULL Pointer is valid: the default attributes will be used
  */
wxVirtualDataViewItemAttr* TestTreeDataModel::GetItemAttribute(const wxVirtualItemID &rID,
                                                                 size_t uiField,
                                                                 const wxVirtualDataViewItemState &rState)
{
    if (m_bEmpty) return(NULL);
    if (rID.GetInt32() % 2 == 0) return(&m_attrEven);
    return(&m_attrOdd);
}

//------- FILTERING - GET ALL VALUES AS STRINGS ---------------------//
/** Get all the values of a field
  * \param rvStrings [output]: an array of string. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void TestTreeDataModel::GetAllValues(wxArrayString &rvStrings, size_t uiField,
                                     wxVirtualIStateModel *pStateModel)
{
    if (pStateModel)
    {
        wxVirtualIDataModel::GetAllValues(rvStrings, uiField, pStateModel);
        return;
    }

    size_t uiSize = NB_ITEMS_LEVEL_1 * NB_ITEMS_LEVEL_2 * NB_ITEMS_LEVEL_3;
    rvStrings.clear();
    rvStrings.reserve(uiSize);
    rvStrings.resize(uiSize);

    char vStatic[100];

    size_t i, j, k;
    size_t uiMax = 10000000;

    uiSize = 0;
    for(i=0;i<NB_ITEMS_LEVEL_1;i++)
    {
        for(j=0;j<NB_ITEMS_LEVEL_2;j++)
        {
            for(k=0;k<NB_ITEMS_LEVEL_3;k++)
            {
                snprintf(vStatic, sizeof(vStatic) / sizeof(char), "ID=%llu row=%llu col=%llu field=%llu", i, j, k, uiField);
                rvStrings[uiSize] = vStatic;
                //wxString s(vStatic);
                //rvStrings.push_back(s);

//                rvStrings.push_back(wxString::Format("ID=%d row=%d col=%d field=%d", i, j, k, uiField));

                uiSize++;
                if (uiSize >= uiMax) return;
            }
        }
    }
}


//------- FILTERING - GET ALL VALUES AS VARIANT ---------------------//
/** Get all the values inside an array of variants
  * \param rvVariants [output]: an array of variants. Previous content is lost
  *                           It contains the list of all values
  *                           Each value should be represented only once
  * \param uiField     [input]: the field to scan
  * \param pStateModel [input]: the state model. If NULL, all items are scanned
  *                             if Non-NULL, the children of collapsed items are ignored
  *
  * The default implementation use:
  *     NextItem (if pStateModel != NULL)
  *     GetChildCount / GetChild (if pStateModel == NULL)
  *     GetItemData
  *  The values are stored as strings in a wxArrayString in the output variant
  *
  * O(n) time, O(n) space
  * Reimplementation strongly recommended if filtering is used
  */
void TestTreeDataModel::GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                     wxVirtualIStateModel *pStateModel)
{
    if (pStateModel)
    {
        wxVirtualIDataModel::GetAllValues(rvVariants, uiField, pStateModel);
        return;
    }

    size_t uiSize = NB_ITEMS_LEVEL_1 * NB_ITEMS_LEVEL_2 * NB_ITEMS_LEVEL_3;
    rvVariants.clear();
    rvVariants.reserve(uiSize);

    size_t i, j, k;
    size_t uiMax = 1000000;
    uiSize = 0;
    for(i=0;i<NB_ITEMS_LEVEL_1;i++)
    {
        for(j=0;j<NB_ITEMS_LEVEL_2;j++)
        {
            for(k=0;k<NB_ITEMS_LEVEL_3;k++)
            {
                rvVariants.push_back(wxString::Format("ID=%d row=%d col=%d field=%d", i, j, k, uiField));
                uiSize++;
                if (uiSize >= uiMax) return;
            }
        }
    }

}

