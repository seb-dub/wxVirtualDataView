
#include "TestListDataModel.h"
#include <wx/VirtualDataView/StateModels/VirtualIStateModel.h>
#include <wx/VirtualDataView/Types/BitmapText.h>
#include <wx/artprov.h>

#define INITIAL_ITEMS_COUNT     100000

//-------------------- NESTED CLASS TID -----------------------------//
TestListDataModel::TID::TID(size_t id, size_t uiField)
                : m_uiID(id), m_uiField(uiField)
{}

bool TestListDataModel::TID::operator<(const TestListDataModel::TID &rhs) const
{
    if (m_uiID < rhs.m_uiID) return(true);
    if (rhs.m_uiID < m_uiID) return(false);

    return(m_uiField < rhs.m_uiField);
}


//------------------- CONSTRUCTORS & DESTRUCTOR ---------------------//
/** Default constructor
  */
TestListDataModel::TestListDataModel(void)
    : wxVirtualIListDataModel(),
      m_uiItemCount(INITIAL_ITEMS_COUNT)
{
    m_attrEven.SetColour(wxColour(255, 0, 0));
    m_attrOdd.SetColour(wxColour(0, 0, 255));

    m_bmp1 = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU);
    m_bmp2 = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU);
}

/** Destructor
  */
TestListDataModel::~TestListDataModel(void)
{
}

//-------------------- INTERNAL METHODS -----------------------------//
/** Get the item data
  * \param uiItemID [input]: index of the item to query
  * \param uiField [input]: the field of the data to query
  * \return the data of the item. Return invalid variant if no data is associated
  */
WX_VDV_INLINE wxVariant TestListDataModel::DoGetItemData(size_t uiItemID,  size_t uiField)
{
    if (uiItemID >= m_uiItemCount) return(wxVariant());

    TMapOfValues::iterator it = m_MapOfValues.find(TID(uiItemID, uiField));
    if (it != m_MapOfValues.end())
    {
        wxVariant vValue = it->second;
        return(vValue);
    }

    switch(uiField)
    {
        case 0  :   {
                        //bitmap + text
                        wxString sValue = wxString::Format("ID=%d field=%d", uiItemID, uiField);
                        wxBitmap bmp = m_bmp1;
                        if (uiItemID % 2 == 0) bmp = m_bmp2;

                        wxBitmapText bt(sValue, bmp);

                        wxVariant vValue;
                        vValue << bt;
                        return(vValue);
                    };
                    break;

        case 1  :   if ((uiItemID % 2) == 0) return(wxVariant((long) 0));
                    return(wxVariant((long) 1));
                    break;

        //date / time
        case 4  :   //date
        case 5  :   //time
        case 6  :   //date time
                    return(wxVariant(wxDateTime::Now()));
                    break;

        //numbers
        case 7  :   return(wxVariant((long) uiItemID));
        case 8  :   return(wxVariant((long) uiItemID));
        case 9  :   return(wxVariant((double) uiItemID));

        //colour
        case 10 :   {
                        wxColour c;
                        c.SetRGB(uiItemID);
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

    wxString sValue = wxString::Format("ID=%d field=%d", uiItemID, uiField);
    return(wxVariant(sValue));
}

//------------ INTERFACE IMPLEMENTATION -----------------------------//
/** Get amount of items
  * \return the amount of items
  */
size_t TestListDataModel::GetItemCount(void)
{
    return(m_uiItemCount);
}

/** Get the item data
  * \param uiItemID [input]: index of the item to query
  * \param uiField [input]: the field of the data to query
  * \param eType [input] : the kind of data to get
  * \return the data of the item. Return invalid variant if no data is associated
  */
wxVariant TestListDataModel::GetListItemData(size_t uiItemID,  size_t uiField, EDataType eType)
{
    //tooltip
    if (eType == WX_ITEM_TOOLTIP_DATA)
    {
        wxString sValue = wxString::Format("Tooltip ID=%d field=%d", uiItemID, uiField);
        return(sValue);
    }

    //help
    if (eType == WX_ITEM_HELP_DATA)
    {
        wxString sValue = wxString::Format("Help ID=%d field=%d", uiItemID, uiField);
        return(sValue);
    }

    if (eType != WX_ITEM_MAIN_DATA) return(wxVariant());
    return(DoGetItemData(uiItemID, uiField));
}

/** Get the item graphic attributes
  * \param uiItemID [input] : the index of the item to query
  * \param uiField [input]: the field index of the item to query
  * \param rState [input] : the state of the item
  * \return a pointer to the graphic attributes for this item. The data is owned by the model.
  *         returning a NULL Pointer is valid: the default attributes will be used
  */
wxVirtualDataViewItemAttr* TestListDataModel::GetListItemAttribute(size_t uiItemID, size_t uiField,
                                                               const wxVirtualDataViewItemState &rState)
{
    if (uiItemID % 2 == 0) return(&m_attrEven);
    return(&m_attrOdd);
}

/** Set the item data
  * \param uiItemID [input]: the index of the item to modify
  * \param uiField [input]: the field of the data to modify
  * \param vValue  [input]: the new value
  * \param eType   [input]: the kind of value to modify
  * \return true if the data was modified, false otherwise
  */
bool TestListDataModel::SetListItemData(size_t uiItemID, size_t uiField,
                                    const wxVariant &vValue,
                                    EDataType eType)
{
    if (eType != WX_ITEM_MAIN_DATA) return(true);

    TID tid(uiItemID, uiField);
    m_MapOfValues.erase(tid);
    m_MapOfValues.insert(std::make_pair(tid, vValue));
    return(true);
}

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
void TestListDataModel::GetAllValues(wxVector<wxVariant> &rvVariants, size_t uiField,
                                     wxVirtualIStateModel *pStateModel)
{
    rvVariants.clear();

    size_t i, uiCount;
    uiCount = m_uiItemCount;
    rvVariants.reserve(uiCount);
    for(i=0;i<uiCount;i++)
    {
        wxVariant v = DoGetItemData(i, uiField);
        rvVariants.push_back(v);
    }
}

//--------------------- ITEM COUNT ----------------------------------//
/** Set item new count
  * \param uiNewCount [input]: the new item count
  */
void TestListDataModel::SetItemCount(size_t uiNewCount)
{
    m_uiItemCount = uiNewCount;
}

