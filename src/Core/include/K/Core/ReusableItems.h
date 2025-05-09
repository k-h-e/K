/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_REUSABLEITEMS_H_
#define K_CORE_REUSABLEITEMS_H_

#include <vector>
#include <cassert>

namespace K {
namespace Core {

//! Pool-like item container, keeping track of which items are currently in use and which are idle (= waiting to be
//! re-used), and providing efficient means of moving items between those two states.
/*!
 *  Multiple groups can be set up in order to organize the items currently in use. Each item that is currently in use
 *  belongs to exactly one of these groups.
 *
 *  The template parameter type <code>T</code> must be copyable and default-constructible. 
 *
 *  There's no support for multithreaded use. Specifically, the items might get stored in a compact way without spacer
 *  fields.
 */
template<class T>
class ReusableItems {
  public:
    class Iterator;
    class IteratorProvider;
    
    ReusableItems(int numGroups);
    ReusableItems()                                      = delete;
    ReusableItems(const ReusableItems &other)            = default;
    ReusableItems &operator=(const ReusableItems &other) = default;
    ReusableItems(ReusableItems &&other)                 = default;
    ReusableItems &operator=(ReusableItems &&other)      = default;
    ~ReusableItems()                                     = default;

    //! Resets the container for the specified number of groups.
    void Reset(int numGroups);
    //! Provides another item for the client to use, assigning it to the specified group. The item is either taken from
    //! the set of idle items, or newly created if no idle item is currently present.
    /*!
     *  The item remains owned by the container. With respect to iterations over the in-use items, new items get
     *  appended at the end of their group.
     *
     *  When the client has finished using the item, it must put it back into the pool of idle items for future re-use
     *  using \ref Put().
     *
     *  Iterators for groups other than the one specified will not get invalidated.
     */
    inline T &Get(int groupId);
    //! Same as \ref Get(), but also provides the id of the item returned. Item ids are never negative.
    T &Get(int groupId, int &outItemId);
    //! Puts back the specified item into the idle pool for future re-use.
    /*!
     *  The item \ref Put() back last will be the first item that \ref Get() returns to use.
     */
    void Put(int itemId);
    //! Moves the specified in-use item to the specified in-use group.
    /*!
     *  The item is added to the back of the target group. It is irrelevant in which in-use group the item has been when
     *  the method was called.
     */
    void Move(int itemId, int targetGroup);
    //! Tells whether or not the specified item group is empty.
    bool Empty(int groupId);
    //! Allows to iterate over the items in the specified group.
    IteratorProvider Iterate(int groupId);
    //! Grants access to the specified item.
    inline T &Item(int itemId);
    //! Tells the total number of items in the container, including the idle ones.
    inline int Count();
    //! Tells the number of idle items in the container (those not currently in use, but waiting to
    //! be re-used).
    inline int IdleCount();
    
  private:
    struct ItemInfo {
        int prev;
        int next;
        T   item;

        ItemInfo()                                 = default;
        ItemInfo(const ItemInfo &other)            = default;
        ItemInfo &operator=(const ItemInfo &other) = default;
        ItemInfo(ItemInfo &&other)                 = default;
        ItemInfo &operator=(ItemInfo &&other)      = default;
        ~ItemInfo()                                = default;
    };

    void AddIdleItem();
    
    std::vector<ItemInfo> items_;
    std::vector<int>      groupAnchors_;
    int                   idleAnchor_;
    int                   idleCount_;
};

//! Allows for iterating over the items that are currently in use and assigned to a specific group.
template<class T>
class ReusableItems<T>::Iterator {
  public:
    Iterator()                                 = delete;
    Iterator(const Iterator &other)            = default;
    Iterator &operator=(const Iterator &other) = default;
    Iterator(Iterator &&other)                 = default;
    Iterator &operator=(Iterator &&other)      = default;
    ~Iterator()                                = default;

    Iterator &operator++();
    bool operator!=(const Iterator &other) const;
    //! Returns the item the iterator is currently pointing at.
    /*!
     *  Undefined in case the iterator is \ref AtEnd().
     */
    T &operator*();
    //! The item id associated with the item the iterator is currently pointing at.
    /*!
     *  The current item can be \ref Put() back or \ref Move() d to another group using the returned id without
     *  invalidating the iterator.
     *
     *  Undefined in case the iterator is \ref AtEnd().
     */
    int ItemId();
    //! Tells whether or not the iterator points behind the end of the sequence.
    bool AtEnd();
    
  private:
    friend class ReusableItems;
    
    Iterator(std::vector<ItemInfo> &items, int groupAnchor, bool isAtEnd);

    std::vector<ItemInfo> *items_;
    int                   currentItem_;
    int                   nextItem_;
    int                   groupAnchor_;
};

//! Supports standard iteration over a given item group.
template<class T>
class ReusableItems<T>::IteratorProvider {
  public:
    //! Returns an iterator pointing to the beginning of the item group sequence.
    Iterator begin() { return begin_; }
    //! Returns an iterator pointing behind the end of the item group sequence.
    Iterator end()   { return end_;   }
    
  private:
    friend class ReusableItems;
    
    IteratorProvider(const Iterator &begin, const Iterator &end) : begin_(begin), end_(end) {}
    
    Iterator begin_;
    Iterator end_;
};

template<class T>
ReusableItems<T>::ReusableItems(int numGroups) {
    Reset(numGroups);
}

template<class T>
void ReusableItems<T>::Reset(int numGroups) {
    if (numGroups < 1) {
        numGroups = 1;
    }

    items_.clear();
    groupAnchors_.clear();
    idleCount_ = 0;
    // idleAnchor_ gets set below.

    ItemInfo info;
    for (int i = 0; i < numGroups; ++i) {
        int anchor { static_cast<int>(items_.size()) };
        groupAnchors_.push_back(anchor);
        info.prev = anchor;
        info.next = anchor;
        items_.push_back(info);
    }

    idleAnchor_ = static_cast<int>(items_.size());
    info.prev = idleAnchor_;
    info.next = idleAnchor_;
    items_.push_back(info);
}

template<class T>
T &ReusableItems<T>::Get(int groupId, int &outItemId) {
    assert((groupId >= 0) && (groupId < static_cast<int>(groupAnchors_.size())));
    
    if (!idleCount_) {
        AddIdleItem();
    }
    
    ItemInfo &idleAnchorInfo { items_[idleAnchor_] };
    int      item            { idleAnchorInfo.next };
    ItemInfo &itemInfo       { items_[item] };
    int      nextIdleItem    { itemInfo.next };
    idleAnchorInfo.next       = nextIdleItem;
    items_[nextIdleItem].prev = idleAnchor_;
    
    int       inUseAnchor     { groupAnchors_[groupId] };
    ItemInfo &inUseAnchorInfo { items_[inUseAnchor] };
    int       lastInUseItem   { inUseAnchorInfo.prev };
    items_[lastInUseItem].next = item;
    itemInfo.prev              = lastInUseItem;
    itemInfo.next              = inUseAnchor;
    inUseAnchorInfo.prev       = item;
    
    --idleCount_;
    outItemId = item;
    return itemInfo.item;
}

template<class T>
T &ReusableItems<T>::Get(int groupId) {
    int id;
    return Get(groupId, &id);
}

template<class T>
void ReusableItems<T>::Put(int itemId) {
    // Unlink...
    ItemInfo &itemInfo { items_[itemId] };
    items_[itemInfo.prev].next = itemInfo.next;
    items_[itemInfo.next].prev = itemInfo.prev;
    
    // Link in as first idle item...
    ItemInfo &idleAnchorInfo { items_[idleAnchor_] };
    int firstIdleItem        { idleAnchorInfo.next };
    idleAnchorInfo.next        = itemId;
    itemInfo.prev              = idleAnchor_;
    itemInfo.next              = firstIdleItem;
    items_[firstIdleItem].prev = itemId;
    
    ++idleCount_;
}

template<class T>
void ReusableItems<T>::Move(int itemId, int targetGroup) {
    // Unlink...
    ItemInfo &itemInfo { items_[itemId] };
    items_[itemInfo.prev].next = itemInfo.next;
    items_[itemInfo.next].prev = itemInfo.prev;
    
    // Link in as last item in target group...
    ItemInfo &targetGroupAnchorInfo { items_[groupAnchors_[targetGroup]] };
    int lastTargetGroupItem         { targetGroupAnchorInfo.prev };
    items_[lastTargetGroupItem].next = itemId;
    itemInfo.prev                    = lastTargetGroupItem;
    itemInfo.next                    = groupAnchors_[targetGroup];
    targetGroupAnchorInfo.prev       = itemId;
}

template<class T>
bool ReusableItems<T>::Empty(int groupId) {
    ItemInfo &groupAnchorInfo { items_[groupAnchors_[groupId]] };
    return (groupAnchorInfo.next == groupAnchors_[groupId]);
}

template<class T>
typename ReusableItems<T>::IteratorProvider ReusableItems<T>::Iterate(int groupId) {
    return IteratorProvider(Iterator(items_, groupAnchors_[groupId], false),
                            Iterator(items_, groupAnchors_[groupId], true));
}

template<class T>
T &ReusableItems<T>::Item(int itemId) {
    return items_[itemId].item;
}

template<class T>
int ReusableItems<T>::Count() {
    return (int)items_.size() - (int)groupAnchors_.size() - 1;
}

template<class T>
int ReusableItems<T>::IdleCount() {
    return idleCount_;
}

template<class T>
void ReusableItems<T>::AddIdleItem() {
    ItemInfo &idleAnchorInfo { items_[idleAnchor_] };
    ItemInfo newItemInfo;
    int      newItem         { static_cast<int>(items_.size()) };
    int      firstIdleItem   { idleAnchorInfo.next };
    
    idleAnchorInfo.next        = newItem;
    newItemInfo.prev           = idleAnchor_;
    newItemInfo.next           = firstIdleItem;
    items_[firstIdleItem].prev = newItem;
    
    items_.push_back(newItemInfo);
    ++idleCount_;
}

// ---

template<class T>
ReusableItems<T>::Iterator::Iterator(std::vector<ItemInfo> &items, int groupAnchor, bool isAtEnd)
        : items_{&items},
          groupAnchor_{groupAnchor} {
    currentItem_ = isAtEnd ? groupAnchor_ : (*items_)[groupAnchor_].next;
    nextItem_    = (*items_)[currentItem_].next;
}

template<class T>
typename ReusableItems<T>::Iterator &ReusableItems<T>::Iterator::operator++() {
    if (currentItem_ != groupAnchor_) {
        currentItem_ = nextItem_;
        nextItem_    = (*items_)[currentItem_].next;
    }
    return *this;
}

template<class T>
bool ReusableItems<T>::Iterator::operator!=(const Iterator &other) const {
    return (currentItem_ != other.currentItem_);
}

template<class T>
T &ReusableItems<T>::Iterator::operator*() {
    return (*items_)[currentItem_].item;
}

template<class T>
int ReusableItems<T>::Iterator::ItemId() {
    return currentItem_;
}

template<class T>
bool ReusableItems<T>::Iterator::AtEnd() {
    return (currentItem_ == groupAnchor_);
}

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_REUSABLEITEMS_H_
