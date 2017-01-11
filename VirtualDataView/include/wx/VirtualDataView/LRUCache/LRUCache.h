/**********************************************************************/
/** FILE    : LRUCache.h                                             **/
/** PROJECT : wxVirtualDataViewCtrl                                  **/
/** LICENSE : wxWindows Library Licence                              **/
/** PURPOSE : provides a simple cache system                         **/
/**********************************************************************/



#ifndef _lru_cache_using_std_
#define _lru_cache_using_std_

#include <list>
#include <map>

/** \class LRUCache : provides a Least-Recently-Used (LRU) replacement cache
  * The cache has a fixed size.
  */
template <typename K, typename V>
class LRUCache
{
    public:
        //typedefs
        typedef K TKey;                                             ///< \brief key stored in the cache
        typedef V TValue;                                           ///< \brief value stored in the cache

        //constructor
        LRUCache(size_t c);                                         ///< \brief constructor
        ~LRUCache(void);                                            ///< \brief destructor

        //get cached value for key
        TValue& operator()(const TKey &rKey);                       ///< \brief get cached value for a key

        //query & insert
        bool        HasKey(const TKey &rKey) const;                 ///< \brief check if a key is present in the cache
        TValue&     GetValue(const TKey &rKey);                     ///< \brief get the value stored
        void Insert(const TKey &rKey,const TValue &rValue);         ///< \brief record a new key/value pair in the cache


        //capacity
        size_t GetCacheSize(void) const;                            ///< \brief get cache size
        void   SetCacheSize(size_t uiSize);                         ///< \brief set new cache size

        //clear
        void   ClearCache(void);                                    ///< \brief clear the cache

    private:
        //typedefs

        typedef std::list<TKey> TKeyTracker;                        ///< \brief Key access history, most recent at back
        typedef typename TKeyTracker::iterator TKeyIterator;        ///< \brief key tracker iterator
        typedef std::pair<TValue, TKeyIterator> TValueKeyIterator;  ///< \brief (value, key tracker iterator) pair
        typedef std::map<TKey,TValueKeyIterator> TKeyToValue;       ///< \brief Key to value and key history iterator

        //data
        size_t          m_uiCapacity;                               ///< \brief Maximum number of key-value pairs to be retained
        TKeyTracker     m_KeyTracker;                               ///< \brief key access history - most recent at back (use a std::list)
        TKeyToValue     m_KeyToValue;                               ///< \brief key to value map

        //methods
        void Evict(void);                                           ///< \brief purge the least recently used element in the cache


};

//------------------- IMPLEMENTATION --------------------------------//
//-------------------- CONSTRUCTOR & DESTRUCTOR ---------------------//
/** Constructor specifies the cached function and the
  * maximum number of records to be stored
  * \param uiMaxSize [input]: the maximal size of the cache
  */
template <typename K, typename V>
inline LRUCache<K, V>::LRUCache(size_t uiMaxSize)
    : m_uiCapacity(uiMaxSize)
{
    if (m_uiCapacity == 0) m_uiCapacity++;
}

/** Destructor
  */
template <typename K, typename V>
inline LRUCache<K, V>::~LRUCache(void)
{
}

//---------------- INTERNAL METHODS ---------------------------------//
/** Purge the least-recently-used element in the cache
  */
template <typename K, typename V>
inline void LRUCache<K, V>::Evict(void)
{
    //Assert method is never called when cache is empty
    if (m_KeyTracker.empty()) return;

    //Identify least recently used key
    typename TKeyToValue::iterator it;
    it = m_KeyToValue.find(m_KeyTracker.front());
    if (it == m_KeyToValue.end()) return;

    //Erase both elements to completely purge record
    m_KeyToValue.erase(it);
    m_KeyTracker.pop_front();
}

//-------------------- QUERY & INSERT -------------------------------//
/** Check if a key is present in the cache
  * \param rKey [input]: the key to retrieve
  * \return true if an entry for this key exists, false otherwise
  */
template <typename K, typename V>
inline bool LRUCache<K, V>::HasKey(const TKey &rKey) const
{
    if (m_KeyToValue.find(rKey) != m_KeyToValue.end()) return(true);
    return(false);
}

/** Get the value stored
  * \param rKey [input]: the key to retrieve
  * \return a reference to the value stored
  */
template <typename K, typename V>
inline typename LRUCache<K, V>::TValue& LRUCache<K, V>::GetValue(const TKey &rKey)
{
    typename TKeyToValue::iterator it = m_KeyToValue.find(rKey);
    if (it == m_KeyToValue.end())
    {
        TValue vValue;
        Insert(rKey, vValue);
        it = m_KeyToValue.find(rKey);
    }

    TValueKeyIterator &rValueIteratorPair = it->second;
    return(rValueIteratorPair.first);
}

/** Record a new key/value pair in the cache
  * \param rKey   [input]: the key to store
  * \param rValue [input]: the value to store
  */
template <typename K, typename V>
inline void LRUCache<K, V>::Insert(const TKey &rKey,const TValue &rValue)
{
    //Method is only called on cache misses
    if (m_KeyToValue.find(rKey) != m_KeyToValue.end())
    {
        return;
    }

    //Make space if necessary
    if (m_KeyToValue.size() == m_uiCapacity) Evict();

    //Record k as most-recently-used key
    typename TKeyTracker::iterator it;
    it = m_KeyTracker.insert(m_KeyTracker.end(), rKey);

    //Create the key-value entry, linked to the usage record.
    m_KeyToValue.insert(std::make_pair(rKey,
                                       std::make_pair(rValue, it)
                                       )
                       );
}

//----------------- CAPACITY ----------------------------------------//
/** Get cache size
  * \return the cache size
  */
template <typename K, typename V>
inline size_t LRUCache<K, V>::GetCacheSize(void) const
{
    return(m_uiCapacity);
}

/** Set new cache size
  * \param uiSize [input]: the new cache size
  */
template <typename K, typename V>
inline void LRUCache<K, V>::SetCacheSize(size_t uiSize)
{
    if (uiSize < m_uiCapacity)
    {
        //make space
        size_t uiDiff = m_uiCapacity - uiSize;
        size_t i;
        for(i=0;i<uiDiff;i++)
        {
            Evict();
        }
    }
    m_uiCapacity = uiSize;
}

//--------------------- CLEAR THE CACHE -----------------------------//
/** Clear the cache
  */
template <typename K, typename V>
inline void LRUCache<K, V>::ClearCache(void)
{
    m_KeyTracker.clear();
    m_KeyToValue.clear();
}

//----------------------- GET CACHED VALUE --------------------------//
/** Get the cached value for a given key
  * \param rKey [input] : the key
  * \return the cached value
  */
template <typename K, typename V>
inline typename LRUCache<K, V>::TValue& LRUCache<K, V>::operator()(const TKey &rKey)
{
    //Attempt to find existing record
    const typename TKeyToValue::iterator it = m_KeyToValue.find(rKey);

    if (it == m_KeyToValue.end())
    {
        //not in cache. Create new record
        const TValue vValue;
        Insert(rKey, vValue);

        //Return the freshly computed value
        return(vValue);
    }

    //We do have it:
    //Update access record by moving accessed key to back of list
    TValueKeyIterator &rValueKeyIterator = it->second;
    TKeyIterator      &rKeyIterator      = rValueKeyIterator.second;
    m_KeyTracker.splice(m_KeyTracker.end(),
                        m_KeyTracker,
                        rKeyIterator);

    //Return the retrieved value
    return(rValueKeyIterator.first);
}

#endif
