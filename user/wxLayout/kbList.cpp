/*-*- c++ -*-********************************************************
 * kbList.cc : a double linked list                                 *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$          *
 *                                                                  *
 * $Log$
 * Revision 1.1  1998/06/29 12:44:36  KB
 * Added my wxWindows based layout engine to the repository.
 * It arranges text and graphics for display on a wxDC.
 * This code is licensed under the LGPL.
 *
 * Revision 1.1.1.1  1998/06/13 21:51:12  karsten
 * initial code
 *
 * Revision 1.4  1998/05/24 14:48:00  KB
 * lots of progress on Python, but cannot call functions yet
 * kbList fixes again?
 *
 * Revision 1.3  1998/05/18 17:48:34  KB
 * more list<>->kbList changes, fixes for wxXt, improved makefiles
 *
 * Revision 1.2  1998/05/14 16:39:31  VZ
 *
 * fixed SIGSEGV in ~kbList if the list is empty
 *
 * Revision 1.1  1998/05/13 19:02:11  KB
 * added kbList, adapted MimeTypes for it, more python, new icons
 *
 *******************************************************************/

#ifdef __GNUG__
#   pragma implementation "kbList.h"
#endif

#include   "kbList.h"


kbListNode::kbListNode( void *ielement,
                        kbListNode *iprev,
                        kbListNode *inext)
{
   next = inext;
   prev = iprev;
   if(prev)
      prev->next = this;
   if(next)
      next->prev = this;
   element = ielement;
}

kbListNode::~kbListNode()
{
   if(prev)
      prev->next = next;
   if(next)
      next->prev = prev;
}


kbList::iterator::iterator(kbListNode *n)
{
   node = n;
}

void *
kbList::iterator::operator*() 
{
   return node->element;
}

kbList::iterator &
kbList::iterator::operator++()
{
   node  = node ? node->next : NULL;
   return *this;
}

kbList::iterator &
kbList::iterator::operator--()
{
   node = node ? node->prev : NULL; 
   return *this;
}
kbList::iterator &
kbList::iterator::operator++(int foo)
{
   return operator++();
}

kbList::iterator &
kbList::iterator::operator--(int bar)
{
   return operator--();
}


bool
kbList::iterator::operator !=(kbList::iterator const & i) const
{
   return node != i.node;
}

bool
kbList::iterator::operator ==(kbList::iterator const & i) const
{
   return node == i.node;
}

kbList::kbList(bool ownsEntriesFlag)
{
   first = NULL;
   last = NULL;
   ownsEntries = ownsEntriesFlag;
}

void
kbList::push_back(void *element)
{
   if(! first) // special case of empty list
   {
      first = new kbListNode(element);
      last = first;
      return;
   }
   else
      last = new kbListNode(element, last);
}

void
kbList::push_front(void *element)
{
   if(! first) // special case of empty list
   {
      push_back(element);
      return;
   }
   else
      first = new kbListNode(element, NULL, first);
}

void *
kbList::pop_back(void)
{
   iterator i;
   void *data;
   bool ownsFlagBak = ownsEntries;
   i = tail();
   data = *i;
   ownsEntries = false;
   erase(i);
   ownsEntries = ownsFlagBak;
   return data;
}

void *
kbList::pop_front(void)
{
   iterator i;
   void *data;
   bool ownsFlagBak = ownsEntries;
   
   i = begin();
   data = *i;
   ownsEntries = false;
   erase(i);
   ownsEntries = ownsFlagBak;
   return data;
   
}

void
kbList::insert(kbList::iterator & i, void *element)
{   
   if(! i.Node())
      return;
   else if(i.Node() == first)
   {
      push_front(element);
      return;
   }
   else if(i.Node() == last)
   {
      push_back(element);
      return;
   }
   i = kbList::iterator(new kbListNode(element, i.Node()->prev, i.Node()));
}

void
kbList::erase(kbList::iterator & i)
{
   kbListNode
      *node = i.Node(),
      *prev, *next;
   
   if(! node) // illegal iterator
      return;

   prev = node->prev;
   next = node->next;
   
   // correct first/last:
   if(node == first)
      first = node->next;
   if(node == last)  // don't put else here!
      last = node->prev;

   // build new links:
   if(prev)
      prev->next = next;
   if(next)
      next->prev = prev;

   // delete this node and contents:
   if(ownsEntries)
      delete *i;
   delete i.Node();

   // change the iterator to next element:
   i = kbList::iterator(next);
}

kbList::~kbList()
{
   kbListNode *next;

   while ( first != NULL )
   {
      next = first->next;
      if(ownsEntries)
         delete first->element;
      delete first;
      first = next;
   }
}

kbList::iterator
kbList::begin(void) const
{
   return kbList::iterator(first);
}

kbList::iterator
kbList::tail(void) const
{
   return kbList::iterator(last);
}

kbList::iterator
kbList::end(void) const
{
   return kbList::iterator(NULL); // the one after the last
}

unsigned
kbList::size(void) const // inefficient
{
   unsigned count = 0;
   kbList::iterator i;
   for(i = begin(); i != end(); i++, count++)
      ;
   return count;
}







#ifdef   KBLIST_TEST

#include   <iostream.h>

KBLIST_DEFINE(kbListInt,int);
   
int main(void)
{
   int
      n, *ptr;
   kbListInt
      l;
   kbListInt::iterator
      i;
   
   for(n = 0; n < 10; n++)
   {
      ptr = new int;
      *ptr = n*n;
      l.push_back(ptr);
   }

   i = l.begin(); // first element
   i++; // 2nd
   i++; // 3rd
   i++; // 4th, insert here:
   ptr = new int;
   *ptr = 4444;
   l.insert(i,ptr);

   // this cannot work, because l.end() returns NULL:
   i = l.end(); // behind last
   i--;  // still behind last
   l.erase(i);  // doesn't do anything

   // this works:
   i = l.tail(); // last element
   i--;
   --i;
   l.erase(i); // erase 3rd last element (49)
   
   for(i = l.begin(); i != l.end(); i++)
      cout << *i << '\t' << *((int *)*i) << endl;

   
   return 0;
}
#endif
