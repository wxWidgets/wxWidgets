/*-*- c++ -*-********************************************************
 * wxexthlp.cpp - an external help controller for wxWindows         *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifdef __GNUG__
#   pragma implementation "wxexthlp.h"
#endif

#include   "wx/setup.h"
#include   "wx/helpbase.h"
#include   "wx/generic/helpext.h"
#include   "wx/string.h"
#include   "wx/utils.h"
#include   <stdio.h>
#include   <ctype.h>
#include   <sys/stat.h>
#include   <unistd.h>


#define WXEXTHELP_INCLUDE_KBLIST
/**
 ** This class uses kbList, a simple linked list. Until someone
 ** rewrites it to use wxList instead, I include the relevant bits and
 ** pieces of kbList here. It's a tiny class anyway, so it won't make
 ** a big difference. The comments probably take up more space than
 ** its code.
 **/

#ifdef WXEXTHELP_INCLUDE_KBLIST

/********************* kbList.h, verbose copy: ****************************/
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
               kbListNode *iprev = NULL,
               kbListNode *inext = NULL);
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
      iterator(kbListNode *n = NULL);
      /** Dereference operator.
          @return the data pointer of the node belonging to this
          iterator
      */
      void * operator*();

      /** This operator allows us to write if(i). It is <em>not</em> a 
          dereference operator and the result is always useless apart
          from its logical value!
      */
      operator void*() const { return node == NULL ? (void*)0 : (void*)(-1); }
      
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

   /** Remove an element from the list _without_ deleting the object.
       @param  i iterator pointing to the element to be deleted
       @return the value of the element just removed
   */
   void *remove(iterator& i) { void *p = *i; doErase(i); return p; }

   /** Erase an element, move iterator to following element.
       @param i iterator pointing to the element to be deleted
   */
   void erase(iterator & i) { deleteContent(i); doErase(i); }

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
   inline bool empty(void) const
      { return first == NULL ; }

protected:
   /// if true, list owns entries
   bool        ownsEntries;
   /// pointer to first element in list
   kbListNode *first;
   /// pointer to last element in list
   kbListNode *last;
protected:
   /** Erase an element, move iterator to following element.
       @param i iterator pointing to the element to be deleted
   */
   void doErase(iterator & i);

   /** Deletes the actual content if ownsflag is set.
       param iterator i
   */
   inline void deleteContent(iterator i)
      { if(ownsEntries) delete *i; }


private:
   /// forbid copy construction
   kbList(kbList const &foo);
   /// forbid assignments
   kbList& operator=(const kbList& foo);
};

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
      inline iterator(kbListNode *n = NULL) \
         : kbList::iterator(n) {} \
      inline type * operator*() \
         /* the cast is needed for MS VC++ 5.0 */ \
         { return (type *)((kbList::iterator *)this)->operator*() ; } \
   }; \
   inline name(bool ownsEntriesFlag = TRUE) \
      : kbList(ownsEntriesFlag) {} \
   \
   inline type *pop_back(void) \
      { return (type *) kbList::pop_back(); } \
   \
   inline type *pop_front(void) \
      { return (type *) kbList::pop_front(); } \
   \
   type *remove(iterator& i) \
      { return (type *)kbList::remove(i); } \
   inline void erase(iterator & i) \
      { deleteContent(i); kbList::erase(i); } \
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
      while ( first != NULL ) \
      { \
         next = first->next; \
         if(ownsEntries) \
            delete (type *)first->element; \
         delete first; \
         first = next; \
      } \
   } \
protected: \
   inline void deleteContent(iterator i) \
      { if(ownsEntries) delete *i; } \
}


/************************* copy of kbList.cpp: ****************************/
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
kbList::iterator::operator++(int /* foo */)
{
   return operator++();
}

kbList::iterator &
kbList::iterator::operator--(int /* bar */)
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
      i = first;
      return;
   }
   i = kbList::iterator(new kbListNode(element, i.Node()->prev, i.Node()));
}

void
kbList::doErase(kbList::iterator & i)
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
   // now done separately
   //if(ownsEntries)
   //delete *i;
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

#endif
/************************* end of kbList code *****************************/

struct wxExtHelpMapEntry
{
   int      id;
   wxString url;
   wxString doc;
   wxExtHelpMapEntry(int iid, wxString const &iurl, wxString const &idoc)
      { id = iid; url = iurl; doc = idoc; }
};
KBLIST_DEFINE(wxExtHelpMapList, wxExtHelpMapEntry);


struct wxBusyCursor
{
   wxBusyCursor() { wxBeginBusyCursor(); }
   ~wxBusyCursor() { wxEndBusyCursor(); }
};

IMPLEMENT_CLASS(wxExtHelpController, wxHelpControllerBase)
   
/**
   This class implements help via an external browser.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.
*/

wxExtHelpController::wxExtHelpController(void)
{
   m_MapList = NULL;
   m_BrowserName = WXEXTHELP_DEFAULTBROWSER;
   m_BrowserIsNetscape = WXEXTHELP_DEFAULTBROWSER_IS_NETSCAPE;

   char *browser = getenv(WXEXTHELP_ENVVAR_BROWSER);
   if(browser)
   {
      m_BrowserName = browser;
      browser = getenv(WXEXTHELP_ENVVAR_BROWSERISNETSCAPE);
      m_BrowserIsNetscape = browser && (atoi(browser) != 0);
   }
}

wxExtHelpController::~wxExtHelpController(void)
{
   if(m_MapList) delete m_MapList;
}

void
wxExtHelpController::SetBrowser(wxString const & browsername, bool isNetscape)
{
   m_BrowserName = browsername;
   m_BrowserIsNetscape = isNetscape;
}

/** This must be called to tell the controller where to find the
    documentation.
    @param file - NOT a filename, but a directory name.
    @return true on success
*/
bool
wxExtHelpController::Initialize(const wxString& file)
{
   return LoadFile(file);
}

  
bool
wxExtHelpController::LoadFile(const wxString& ifile = "")
{
   wxString mapFile, file, url, doc;
   int id,i,len;
   char buffer[WXEXTHELP_BUFLEN];
   
   wxBusyCursor b; // display a busy cursor

   if(! ifile.IsEmpty())
   {
      file = ifile;
      if(! wxIsAbsolutePath(file))
      {
         file = wxGetWorkingDirectory();
         file << WXEXTHELP_SEPARATOR << ifile;
      }
      else
         file = ifile;
   
      if(! wxDirExists(file))
         return false;
      
      mapFile << file << WXEXTHELP_SEPARATOR << WXEXTHELP_MAPFILE;
   }
   else // try to reload old file
      mapFile = m_MapFile;

   if(! wxFileExists(mapFile))
      return false;

   if(m_MapList) delete m_MapList;
   m_MapList = new wxExtHelpMapList;
   m_NumOfEntries = 0;
   
   FILE *input = fopen(mapFile.c_str(),"rt");
   if(! input)
      return false;
   do
   {
      if(fgets(buffer,WXEXTHELP_BUFLEN,input) && *buffer != WXEXTHELP_COMMENTCHAR)
      {
         len = strlen(buffer);
         if(buffer[len-1] == '\n')
            buffer[len-1] = '\0'; // cut of trailing newline
         if(sscanf(buffer,"%d", &id) != 1)
            break; // error
         for(i=0; isdigit(buffer[i])||isspace(buffer[i]); i++)
            ; // find begin of URL
         url = "";
         while(buffer[i] && ! isspace(buffer[i]) && buffer[i] !=
               WXEXTHELP_COMMENTCHAR)
            url << buffer[i++];
         while(buffer[i] && buffer[i] != WXEXTHELP_COMMENTCHAR)
            i++;
         doc = "";
         if(buffer[i])
            doc = (buffer + i + 1); // skip the comment character
         m_MapList->push_back(new wxExtHelpMapEntry(id,url,doc));
         m_NumOfEntries++;
      }
      else
         perror("");
   }while(! feof(input));
   fclose(input);
   
   m_MapFile = file; // now it's valid
   return true;
}

bool
wxExtHelpController::CallBrowser(wxString const &relativeURL)
{
   wxBusyCursor b; // display a busy cursor
   wxString command;

   if(m_BrowserIsNetscape) // try re-loading first
   {
      wxString lockfile;
      wxGetHomeDir(&lockfile);
      lockfile << WXEXTHELP_SEPARATOR << ".netscape/lock";
      struct stat statbuf;
      if(lstat(lockfile.c_str(), &statbuf) == 0)
      // cannot use wxFileExists, because it's a link pointing to a
      // non-existing location      if(wxFileExists(lockfile))
      {
         long success;
         command << m_BrowserName << " -remote openURL("
                 << "file://" << m_MapFile
                 << WXEXTHELP_SEPARATOR << relativeURL << ")"; 
         success = wxExecute(command);
         if(success != 0 ) // returns PID on success
            return true;
      }
   }
   command = m_BrowserName;
   command << " file://"
           << m_MapFile << WXEXTHELP_SEPARATOR << relativeURL; 
   return wxExecute(command) != 0; 
}

bool
wxExtHelpController::DisplayContents(void)
{
   if(! m_NumOfEntries)
      return false;
   wxBusyCursor b; // display a busy cursor
   return KeywordSearch("");
}
      
bool
wxExtHelpController::DisplaySection(int sectionNo)
{
   if(! m_NumOfEntries)
      return false;

   wxBusyCursor b; // display a busy cursor
   wxExtHelpMapList::iterator i = m_MapList->begin();
   while(i != m_MapList->end())
   {
      if((**i).id == sectionNo)
         return CallBrowser((**i).url);
      i++;
   }
   return false;
}

bool
wxExtHelpController::DisplayBlock(long blockNo)
{
   return DisplaySection((int)blockNo);
}

bool
wxExtHelpController::KeywordSearch(const wxString& k)
{
   if(! m_NumOfEntries)
      return false;

   wxBusyCursor b; // display a busy cursor
   wxString     *choices = new wxString[m_NumOfEntries];
   wxString     *urls = new wxString[m_NumOfEntries];
   wxString compA, compB;
   
   int          idx = 0, j;
   bool         rc;
   bool         showAll = k.IsEmpty();
   wxExtHelpMapList::iterator i = m_MapList->begin();
   
   compA = k; compA.LowerCase(); // we compare case insensitive
   while(i != m_MapList->end())
   {
      compB = (**i).doc; compB.LowerCase();
      if((showAll || compB.Contains(k)) && ! compB.IsEmpty()) 
      {
         urls[idx] = (**i).url;
         // doesn't work:
         // choices[idx] = (**i).doc.Contains((**i).doc.Before(WXEXTHELP_COMMENTCHAR));
         //if(choices[idx].IsEmpty()) // didn't contain the ';'
         //   choices[idx] = (**i).doc;
         choices[idx] = "";
         for(j=0;(**i).doc.c_str()[j]
                && (**i).doc.c_str()[j] != WXEXTHELP_COMMENTCHAR; j++)
            choices[idx] << (**i).doc.c_str()[j];
         idx++;
      }
      i++;
   }

   if(idx == 1)
      rc = CallBrowser(urls[0]);
   else if(idx == 0)
   {
      wxMessageBox(_("No entries found."));
      rc = false;
   }
   else
   {
      idx = wxGetSingleChoiceIndex(showAll ? _("Help Index") : _("Relevant entries:"),
                                   showAll ? _("Help Index") : _("Entries found"),
                                   idx,choices);
      if(idx != -1)
         rc = CallBrowser(urls[idx]);
      else
         rc = false;
   }
   delete[] urls;
   delete[] choices;
   
   return rc;
}


bool
wxExtHelpController::Quit(void)
{
   return true;
}

void
wxExtHelpController::OnQuit(void)
{
}

