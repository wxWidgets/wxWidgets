/*-*- c++ -*-********************************************************
 * kbList.h : a double linked list                                  *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *
 *******************************************************************/

#ifndef   KBLIST_H
#   define   KBLIST_H

#ifdef __GNUG__
#   pragma interface "kbList.h"
#endif

#ifndef   NULL
#   define   NULL   0
#endif

/**@name Double linked list implementation. */
//@{

/** kbListNode is a class used by kbList. It represents a single
    element in the list. It is not intended for general use outside
    kbList functions.
*/
struct kbListNode
{
   /// pointer to next node or NULL
   struct kbListNode *next;
   /// pointer to previous node or NULL
   struct kbListNode *prev;
   /// pointer to the actual data 
   void *element;
   /** Constructor - it automatically links the node into the list, if
       the iprev, inext parameters are given.
       @param ielement pointer to the data for this node (i.e. the data itself)
       @param iprev if not NULL, use this as previous element in list
       @param inext if not NULL, use this as next element in list
   */
   kbListNode( void *ielement,
               kbListNode *iprev = (kbListNode *) NULL,
               kbListNode *inext = (kbListNode *) NULL);
   /// Destructor.
   ~kbListNode();
};

/** The main list class, handling void pointers as data.
  */

class kbList
{
public:
   /// An iterator class for kbList, just like for the STL classes.
   class iterator
   {
   protected:
      /// the node to which this iterator points
      kbListNode *node;
      friend class kbList;
         public:
      /** Constructor.
          @param n if not NULL, the node to which to point
      */
      iterator(kbListNode *n = (kbListNode *) NULL);
      /** Dereference operator.
          @return the data pointer of the node belonging to this
          iterator
      */
      void * operator*();

      /** Increment operator - prefix, goes to next node in list.
          @return itself
      */
      iterator & operator++();

      /** Decrement operator - prefix, goes to previous node in list.
          @return itself
      */
      iterator & operator--();

      /** Increment operator - prefix, goes to next node in list.
          @return itself
      */
      iterator & operator++(int); //postfix

      /** Decrement operator - prefix, goes to previous node in list.
          @return itself
      */
      iterator & operator--(int); //postfix

      /** Comparison operator.
          @return true if not equal.
      */
      bool operator !=(iterator const &) const; 

      /* Comparison operator.
         @return true if equal
      */
      bool operator ==(iterator const &) const; 

      /** Returns a pointer to the node associated with this iterator.
          This function is not for general use and should be
          protected. However, if protected, it cannot be called from
          derived classes' iterators. (Is this a bug in gcc/egcs?)
          @return the node pointer
      */
      inline kbListNode * Node(void) const
         { return node; }
   };

   /** Constructor.
       @param ownsEntriesFlag if true, the list owns the entries and
       will issue a delete on each of them when deleting them. If
       false, the entries themselves will not get deleted. Do not use
       this with array types!
   */
   kbList(bool ownsEntriesFlag = true);

   /** Destructor.
       If entries are owned, they will all get deleted from here.
   */
   ~kbList();

   /** Tell list whether it owns objects. If owned, they can be
       deleted by list. See the constructor for more details.
       @param ownsflag if true, list will own entries
   */
   void ownsObjects(bool ownsflag = true)
      { ownsEntries = ownsflag; }

   /** Query whether list owns entries.
       @return true if list owns entries
   */
   bool ownsObjects(void)
      { return ownsEntries; }
   
   /** Add an entry at the end of the list.
       @param element pointer to data
   */
   void push_back(void *element);

   /** Add an entry at the head of the list.
       @param element pointer to data
   */
   void push_front(void *element);

   /** Get element from end of the list and delete it.
       NOTE: In this case the element's data will not get deleted by
       the list. It is the responsibility of the caller to free it.
       @return the element data
   */
   void *pop_back(void);

   /** Get element from head of the list and delete it.
       NOTE: In this case the element's data will not get deleted by
       the list. It is the responsibility of the caller to free it.
       @return the element data
   */
   void *pop_front(void);

   /** Insert an element into the list.
       @param i an iterator pointing to the element, before which the new one should be inserted
       @param element the element data
   */
   void insert(iterator & i, void *element);

   /** Erase an element, move iterator to following element.
       @param i iterator pointing to the element to be deleted
   */
   void erase(iterator & i);
   
   /* Get head of list.
      @return iterator pointing to head of list
   */
   iterator begin(void) const;

   /* Get end of list.
   @return iterator pointing after the end of the list. This is an
   invalid iterator which cannot be dereferenced or decremented. It is
   only of use in comparisons. NOTE: this is different from STL!
   @see tail
   */
   iterator end(void) const;

   /* Get last element in list.
      @return iterator pointing to the last element in the list.
      @see end
   */
   iterator tail(void) const;

   /* Get the number of elements in the list.
      @return number of elements in the list
   */
   unsigned size(void) const;

   /* Query whether list is empty.
      @return true if list is empty
   */
   bool empty(void) const
      { return first == NULL ; }

protected:
   /// if true, list owns entries
   bool        ownsEntries;
   /// pointer to first element in list
   kbListNode *first;
   /// pointer to last element in list
   kbListNode *last;

private:
   /// forbid copy construction
   kbList(kbList const &foo);
   /// forbid assignments
   kbList& operator=(const kbList& foo);
};

/// just for backward compatibility, will be removed soon
typedef kbList::iterator kbListIterator;
/// cast an iterator to a pointer, compatibility only to be removed
#define   kbListICast(type, iterator)   ((type *)*iterator)
/// cast an iterator to a const pointer, compatibility only to be removed
#define   kbListIcCast(type, iterator)   ((type const *)*iterator)

/** Macro to define a kbList with a given name, having elements of
    pointer to the given type. I.e. KBLIST_DEFINE(Int,int) would
    create a kbListInt type holding int pointers.
*/
#define   KBLIST_DEFINE(name,type)   \
class name : public kbList \
{ \
public: \
   class iterator : public kbList::iterator \
   { \
   protected: \
      inline iterator(kbList::iterator const & i) \
         { node = i.Node(); } \
      friend class name; \
   public: \
      inline iterator(kbListNode *n = (kbListNode *) NULL) \
         : kbList::iterator(n) {} \
      inline type * operator*() \
         /* the cast is needed for MS VC++ 5.0 */ \
         { return (type *)((kbList::iterator *)this)->operator*() ; } \
   }; \
   inline name(bool ownsEntriesFlag = FALSE) \
      : kbList(ownsEntriesFlag) {} \
   \
   inline void push_back(type *element) \
      { kbList::push_back((void *)element); } \
   \
   inline void push_front(type *element) \
      { kbList::push_front((void *)element); } \
   \
   inline type *pop_back(void) \
      { return (type *) kbList::pop_back(); } \
   \
   inline type *pop_front(void) \
      { return (type *) kbList::pop_front(); } \
   \
   inline void insert(iterator & i, type *element) \
      { kbList::insert(i, (void *) element); } \
   \
   inline void erase(iterator & i) \
      { kbList::erase(i); } \
   \
   inline iterator begin(void) const \
      { return kbList::begin(); } \
   \
   inline iterator end(void) const \
      { return kbList::end(); } \
   \
   inline iterator tail(void) const \
      { return kbList::tail(); } \
   ~name() \
   { \
      kbListNode *next; \
      while ( first != (kbListNode *) NULL ) \
      { \
         next = first->next; \
         if(ownsEntries) \
            delete typecast(first->element); \
         delete first; \
         first = next; \
      } \
   } \
   private: \
   inline type * typecast(void *ptr) \
      { return (type *) ptr; } \
   }

#ifdef   MCONFIG_H
/// define the most commonly used list type once:
KBLIST_DEFINE(kbStringList, String);
#endif

#endif // KBLIST_H
