/*******************************************************************************
 * Simplified Wrapper and Interface Generator  (SWIG)
 * 
 * Author : David Beazley
 *
 * Department of Computer Science        
 * University of Chicago
 * 1100 E 58th Street
 * Chicago, IL  60637
 * beazley@cs.uchicago.edu
 *
 * Please read the file LICENSE for the copyright and terms by which SWIG
 * can be used and distributed.
 *******************************************************************************/
/***********************************************************************
 * $Header$
 *
 * newdoc.cxx
 *
 * SWIG Documentation system. (2nd attempt)
 *
 * SWIG organizes documentation as a tree structure where each node is a
 * documentation entry (DocEntry) of some kind.   To generate documentation,
 * we simply traverse the tree and call output methods.
 *
 * A sample documentation tree looks something like the following :
 *
 *  TITLE ---->  SECTION 1 ---->  func1
 *                         ---->  func2
 *                         ---->  func3
 *                         ---->  class ---> func1
 *                                      ---> func2
 *                                      ---> var1
 *                         ---->  func4
 * 
 *        ---->  SECTION 2 ---->  func1
 *                         ---->  var1
 *
 * and so on.
 *
 * This structure makes it possible to organize C++ classes and more
 * complicated structures.   Hopefully this will provide enough structure
 * for later versions of SWIG.
 *
 *************************************************************************/

#include "internal.h"
#include <ctype.h>

extern char *get_time();
static char *last_name = 0;

DocEntry *DocEntry::dead_entries = 0;

// Utility function for converting a string to upper case

static void str_toupper(char *str) {
  char *c;
  c = str;
  while (*c) {
    *c = toupper(*c);
    c++;
  }
}

// --------------------------------------------------------------------
// DocEntry::~DocEntry 
//
// Destroy a documentation entry.   Destroys this entry and all of
// its children.
// --------------------------------------------------------------------

DocEntry::~DocEntry() {
  DocEntry *de, *de1;

  if (name) delete name;
  
  // Now kill all of the children (well, figuratively speaking)

  de = child;
  while (de) {
    de1 = de->next;
    delete de;
    de = de1;
  }
}

// --------------------------------------------------------------------
// void DocEntry::sort_children()
//
// Sort children by name (not height).  This function gathers all
// of the children up into an array of pointers.   Then we do an
// insertion sort on it and place the children back in order.
// --------------------------------------------------------------------

void DocEntry::sort_children() {

    int count = 0;
    int i,j;
    DocEntry  *d;
    DocEntry **list;
    DocEntry  *v;

    if (!child) return;           // Nothing to sort

    d = child;
    while (d) {
      count++;
      d = d->next;
    }

    // allocate a temporary array for sorting everything

    list = new DocEntry *[count+2];
    
    // Now put pointers into list
    
    d = child;
    i = 0;
    while (d) {
      list[i] = d;
      d = d->next;
      i++;
    }

    // Do an insertion sort by name

    for (i = 1; i < count; i++) {
      v = list[i];
      j = i;
      while((j > 0) && (strcmp(list[j-1]->name,v->name) > 0)) {
	list[j] = list[j-1];
	j--;
      }
      list[j] = v;
    }

    // Now, we're going to reorganize the children in order

    list[count] = 0;
    child = list[0];            // Our child is the first one in the list
    d = child;
    for (i = 0; i < count; i++) {
      d->next = list[i+1];
      d = d->next;
    }
    delete list;
}

// --------------------------------------------------------------------
// void DocEntry::output()
//
// Output this entry
// --------------------------------------------------------------------

void DocEntry::output(Documentation *) {
  fprintf(stderr,"SWIG (internal) : No output method defined for DocEntry.\n");
}

// --------------------------------------------------------------------
// DocEntry::add(DocEntry *de)
//
// Adds a new DocEntry as a sibling.   Basically we just walk down the
// linked list and append ourselves to the end.   The documentation
// Entry we're adding may, in fact, have siblings too, but this function
// Should still work.
// --------------------------------------------------------------------

void DocEntry::add(DocEntry *de) {
  DocEntry *d,*d1;
  d = next;
  d1 = this;
  while (d) {
    d1 = d;
    d = d->next;
  }
  d1->next = de;           
  de->previous = d1;              // Set up the previous list member
}


// --------------------------------------------------------------------
// DocEntry::addchild(DocEntry *de)
//
// Adds a new DocEntry as a child.   If we're in Ignore mode, the
// documentation entry is still created, but we simply abandon it.
// --------------------------------------------------------------------

void DocEntry::addchild(DocEntry *de) {
  if (!IgnoreDoc) {
    if (child) child->add(de);
    else child = de;
  } else {
    if (dead_entries) dead_entries->add(de);
    else dead_entries = de;
  }
}

// -------------------------------------------------------------------
// DocEntry::remove()
//
// Removes a documentation entry from the tree and places it on
// the dead_entries list
// -------------------------------------------------------------------

void DocEntry::remove() {

  if (previous) {
    if (next) 
      previous->next = next;           // Take out of the linked list
    else 
      previous->next = 0;
  } else {                             // Make sure our parent isn't pointing to us
    if (parent) 
      parent->child = next;  
  }

  previous = 0;
  next = 0;
  
  if (!dead_entries) dead_entries = this;
  else dead_entries->add(this);

}

// -------------------------------------------------------------------
// void DocEntry::style(char *name, char *value)
//
// Set style parameters of a documentation entry
// -------------------------------------------------------------------

void DocEntry::style(char *pname, char *) {
  if (strcmp(pname,"sort") == 0) {
    sorted = 1;
  } else if (strcmp(pname,"nosort") == 0) {
    sorted = 0;
  } else if (strcmp(pname,"info") == 0) {
    print_info = 1;
  } else if (strcmp(pname,"noinfo") == 0) {
    print_info = 0;
  } else if (strcmp(pname,"pre") == 0) {
    format = 0;
  } else if (strcmp(pname,"format") == 0) {
    format = 1;
  }
}

// -------------------------------------------------------------------
// void DocEntry::parse_args(int argc, char **argv)
//
// Take command line options and process them.  This really only
// applies to the top-level documentation entry.
// -------------------------------------------------------------------

static char *doc_usage = "\
Documentation Processing : \n\
     -Sformat         - Reformat comment text\n\
     -Sinfo           - Print C formatting information (the default)\n\
     -Snoinfo         - Omit C formatting information.\n\
     -Snosort         - Print everything in order (the default)\n\
     -Spre            - Assume comments are pre-formatted (the default)\n\
     -Ssort           - Sort documentation alphabetically\n\n";

void DocEntry::parse_args(int argc, char **argv) {
  int  i;
  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if (strcmp(argv[i],"-Ssort") == 0) {
	    this->style("sort",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Snosort") == 0) {
	    this->style("nosort",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Sinfo") == 0) {
	    this->style("info",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Snoinfo") == 0) {
	    this->style("noinfo",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Spre") == 0) {
	    this->style("pre",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Sformat") == 0) {
	    this->style("format",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(doc_usage,stderr);
	  }
      }
  }
}


// -------------------------------------------------------------------
// DocTitle::DocTitle(char *title, DocEntry *_parent);
//
// Create a new title documentation entry.   The name of the entry
// is the title.   
//
// The body text is optional, but may be filled in with a description
// as well.
// -------------------------------------------------------------------

DocTitle::DocTitle(char *title, DocEntry *_parent) {
  name = copy_string(title);
  str_toupper(name);
  parent = _parent;
  child = 0;
  next = 0;
  previous = 0;
  usage << title << "\n";
  counter = 1;
  is_separator = 1;
  line_number = ::start_line;
  end_line = ::line_number;
  file = copy_string(input_file);
  if (_parent) {
    sorted = _parent->sorted;
    format = _parent->format;
    print_info = _parent->print_info;
  } else {
    sorted = SWIGDEFAULT_SORT;
    format = SWIGDEFAULT_FORMAT;
    print_info = SWIGDEFAULT_INFO;
  }
  comment_handler->set_entry(this);
  if (last_name) delete last_name;
  last_name = 0;
}
// --------------------------------------------------------------------
// DocTitle::output(Documentation *d)
//
// Output a title to the Documentation module
// --------------------------------------------------------------------

void DocTitle::output(Documentation *d) {
  DocEntry *de;

  d->title(this);
  if (sorted) {
    sort_children();
  }

  // Now output my children 

  de = child;
  while (de) {
    de->output(d);
    de = de->next;
  }
}

// -------------------------------------------------------------------
// DocSection::DocSection(char *section, DocEntry *_parent);
//
// Create a new documentation section.  The name and description is
// set to the name of the section.   The text field is optional
// but could contain a more complete description.
//
// The sorted field indicates whether members of this section are
// sorted or not.
// -------------------------------------------------------------------

DocSection::DocSection(char *section, DocEntry *_parent) {
  name = copy_string(section);
  str_toupper(name);
  parent = _parent;
  child = 0;
  next = 0;
  previous = 0;
  usage << section;
  counter = 1;
  is_separator = 1;
  if (_parent) _parent->addchild(this);
  line_number = ::start_line;
  end_line = ::line_number;
  file = copy_string(input_file);
  if (_parent) {
    sorted = _parent->sorted;
    format = _parent->format;
    print_info = _parent->print_info;
  } else {
    sorted = SWIGDEFAULT_SORT;
    format = SWIGDEFAULT_FORMAT;
    print_info = SWIGDEFAULT_INFO;
  }
  comment_handler->set_entry(this);
  if (last_name) delete last_name;
  last_name = 0;
}

// --------------------------------------------------------------------
// DocSection::output(Documentation *d)
//
// Output a section to the documentation module
// --------------------------------------------------------------------

void DocSection::output(Documentation *d) {
  DocEntry *de;

  // Make a new section

  d->newsection(this,this->parent->counter++);   // Make a new section

  // Sort the children if necessary

  if (sorted) {
    sort_children();
  }

  // Now output my children 

  de = child;
  while (de) {
    de->output(d);
    de = de->next;
  }

  // End this section

  d->endsection();

}


// -------------------------------------------------------------------
// DocDecl::DocDecl(char *fname, DocEntry *_parent);
//
// Create documentation for a function declaration.
// -------------------------------------------------------------------

DocDecl::DocDecl(char *fname, DocEntry *_parent) {
  name = copy_string(fname);
  str_toupper(name);
  parent = _parent;
  child = 0;
  next = 0;
  previous = 0;
  is_separator = 0;
  if (_parent) _parent->addchild(this);
  line_number = ::start_line;
  end_line = ::line_number;
  file = copy_string(input_file);
  if (_parent) {
    sorted = _parent->sorted;
    format = _parent->format;
    print_info = _parent->print_info;
  } else {
    sorted = SWIGDEFAULT_SORT;
    format = SWIGDEFAULT_FORMAT;
    print_info = SWIGDEFAULT_INFO;
  }
  comment_handler->set_entry(this);
  if (last_name) delete last_name;
  last_name = copy_string(name);
}


// --------------------------------------------------------------------
// DocDecl::DocDecl(DocEntry *de, DocEntry *_parent) 
//
// Make a new declaration entry, but copy attributes from someone else
// --------------------------------------------------------------------

DocDecl::DocDecl(DocEntry *de, DocEntry *_parent) {
  name = copy_string(de->name);
  usage = de->usage.get();
  cinfo = de->cinfo.get();
  text = de->text.get();
  line_number = de->line_number;
  end_line = de->end_line;
  file = copy_string(de->file);
  print_info = de->print_info;
  format = de->format;
  if (_parent) {
    _parent->addchild(this);
  }
}

// --------------------------------------------------------------------
// DocDecl::output(Documentation *d)
//
// Output a function to the documentation module
// --------------------------------------------------------------------

void DocDecl::output(Documentation *d) {
  d->print_decl(this);
}

// -------------------------------------------------------------------
// DocClass::DocClass(char *classname, DocEntry *_parent);
//
// Create a new class section.   Classes are created as funny sorts of
// sections.
//
// The sorted field indicates whether members of this section are
// sorted or not.
// -------------------------------------------------------------------

DocClass::DocClass(char *classname, DocEntry *_parent) {
  name = copy_string(classname);
  str_toupper(name);
  parent = _parent;
  child = 0;
  next = 0;
  previous = 0;
  usage << classname<< "\n";
  counter = 1;
  is_separator = 1;
  if (_parent) _parent->addchild(this);
  line_number = ::start_line;
  end_line = ::line_number;
  file = copy_string(input_file);
  if (_parent) {
    sorted = _parent->sorted;
    format = _parent->format;
    print_info = _parent->print_info;
  } else {
    sorted = SWIGDEFAULT_SORT;
    format = SWIGDEFAULT_FORMAT;
    print_info = SWIGDEFAULT_INFO;
  }
  comment_handler->set_entry(this);
  if (last_name) delete last_name;
  last_name = copy_string(name);

}

// --------------------------------------------------------------------
// DocClass::output(Documentation *d)
//
// Output a section to the documentation module
// --------------------------------------------------------------------

void DocClass::output(Documentation *d) {
  DocEntry *de;

  // Make a new section

  d->newsection(this,this->parent->counter++);   // Make a subsection for this

  // Sort the children if necessary

  if (sorted) {
    sort_children();
  }

  // Now output my children 

  de = child;
  while (de) {
    de->output(d);
    de = de->next;
  }

  // End this section

  d->endsection();

  // We now check to see if the next thing is a separator.  If not, we'll
  // emit a separator

  if (next) {
    if (!next->is_separator)
      d->separator();
  }
}

// -------------------------------------------------------------------
// DocText::DocText(char *_text, DocEntry *_parent);
//
// Create documentation for a function declaration.
// -------------------------------------------------------------------

DocText::DocText(char *_text, DocEntry *_parent) {
  if (!last_name) 
    name = copy_string("");         // There is no name for text
  else
    name = copy_string(last_name);
  parent = _parent;
  child = 0;
  next = 0;
  previous = 0;
  text << _text;
  is_separator = 0;
  if (_parent) _parent->addchild(this);
  if (_parent) {
    sorted = _parent->sorted;
    format = _parent->format;
    print_info = _parent->print_info;
  } else {
    sorted = SWIGDEFAULT_SORT;
    format = SWIGDEFAULT_FORMAT;
    print_info = SWIGDEFAULT_INFO;
  }
}

// --------------------------------------------------------------------
// DocText::output(Documentation *d)
//
// Output a function to the documentation module
// --------------------------------------------------------------------

void DocText::output(Documentation *d) {
  d->print_text(this);
}

