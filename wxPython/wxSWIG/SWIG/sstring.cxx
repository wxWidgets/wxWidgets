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

#include "internal.h"
#include <ctype.h>

//-----------------------------------------------------------------------
// char *copy_string(char *str)
//
// Makes a copy of string str.  Returns a pointer to it.
//-----------------------------------------------------------------------

char *copy_string(char *str) {
  char *res = 0;
  if (str) {
    res = new char[strlen(str)+1];
    strcpy(res,str);
  }
  return res;
}

//-----------------------------------------------------------------------
// void format_string(char *str)
//
// Replace all of the escape sequences in the string str.   It is
// assumed that the new string is smaller than the original!
//-----------------------------------------------------------------------

void format_string(char *str) {
  char *newstr, *c,*c1;
  int  state;
  if (!str) return;
  newstr = copy_string(str);
  c = newstr;
  c1 = str;
  state = 0;
  while (*c) {
    switch(state) {
    case 0:
      if (*c == '\\') 
	state = 1;
      else {
	*(c1++) = *c;
	state = 0;
      }
      break;
   case 1:
      // We're in a simple escape sequence figure out what to do
      switch(*c) {
      case 'n':
	*(c1++) = '\n';
	break;
      case 'f':
	*(c1++) = '\f';
	break;
      case 'r':
	*(c1++) = '\r';
	break;
      case 't':
	*(c1++) = '\t';
	break;
      case '\\':
	*(c1++) = '\\';
      break;
      case '\"':
	*(c1++) = '\"';
      break;
      case '\'':
	*(c1++) = '\'';
      break;
      default:
	*(c1++) = '\\';
        *(c1++) = *c;
      }
      state = 0;
      break;
    default:
      *(c1++) = *c;
      state = 0;
    }
    c++;
  }
  *c1 = 0;
  delete newstr;
}

// ---------------------------------------------------------------------------
// $Header$
// sstring.cxx
//
// SWIG String class.
// This class is used to construct long strings when writing 
// wrapper functions.  It also "mimicks" the C++ streams I/O
// library for creating strings.  For example :
//
//     str << "hi there" << 3 << "\n";
// 
// Will append the given strings to str.
//
// The idea here is to provide a mechanism for writing wrapper
// functions as strings before writing them out to a file.
//
// ---------------------------------------------------------------------------
#define INIT_MAXSIZE  64

// ---------------------------------------------------------------
// Pools. This is a list of available strings for memory allocation
// and deletion.
// ---------------------------------------------------------------

struct StrMem {
  char  *str;
  int    len;
};

#define POOLSIZE 100

static StrMem pool[POOLSIZE];
static int pool_index = 0;

// Returns an item from the pool that can accomodate len
static char *get_pool(int len, int &newlen) {
  int i,j;
  char *nc;
  if (pool_index < 1) {
    newlen = len;
    return new char[len];
  }
  i = pool_index-1;
  j = 0;
  while(i >= 0) {
    if ((pool[i].len >= len) && (pool[i].len <= 4*len)) {
      nc = pool[i].str;
      newlen = pool[i].len;
      pool[i].str = pool[pool_index-1].str;
      pool[i].len = pool[pool_index-1].len;
      pool_index--;
      return nc;
    }
    j++;
    i--;
  }
  newlen = len;
  return new char[len];
}

// Puts an item onto the pool

static void put_pool(char *str, int len) {
  if (len < INIT_MAXSIZE) {
    delete [] str;
    return;
  }
  if (pool_index == POOLSIZE) {
    delete [] pool[pool_index-1].str;
    pool_index--;
  }
  pool[pool_index].str = str;
  pool[pool_index].len = len;
  if (pool_index != POOLSIZE) 
    pool_index++;
}

// ---------------------------------------------------------------
// String::String()
//
// Create a new string with nothing in it
// ---------------------------------------------------------------

String::String() {
  maxsize = INIT_MAXSIZE;
  str = get_pool(maxsize,maxsize);   // May return a pool that is larger
  str[0] = 0;
  len = 0;
}

// ---------------------------------------------------------------
// String::String(const char *s)
//
// Create a new string copied from a normal C-style string
// ---------------------------------------------------------------

String::String(const char *s) {
  int  max = INIT_MAXSIZE;
  int  l = 0;
  if (s) {
    l = (int) strlen(s);
    if ((l+1) > max) max = l+1;
  }
  str = get_pool(max,maxsize);
  if (s) {
    strcpy(str,s);
    len = l;
  } else {
    str[0] = 0;
    len = 0;
  }
}

// ---------------------------------------------------------------
// String::~String(const char *s)
//
// Destroy a string
// ---------------------------------------------------------------

String::~String() {
  put_pool(str,maxsize);
}

// ---------------------------------------------------------------
// String::add(const char *newstr)
//
// Concatenate newstr onto the current string
// ---------------------------------------------------------------

void String::add(const char *newstr) {
  int   newlen;
  char  *nstr = 0;
  int   newmaxsize;
  int   l;

  l = (int) strlen(newstr);
  newlen = len+l + 1;
  if (newlen >= maxsize-1) {
    newmaxsize = 2*maxsize;
    if (newlen >= newmaxsize -1) newmaxsize = newlen + 1;
    nstr = get_pool(newmaxsize,newmaxsize);
    strcpy(nstr,str);
    put_pool(str,maxsize);
    maxsize = newmaxsize;
    str = nstr;
  }
  strcpy(str+len,newstr);
  len += l;
}

// ---------------------------------------------------------------
// String::add(char)
//
// Adds a single character to the current string
// ---------------------------------------------------------------

void String::add(char nc) {
  int   newlen;
  char  *nstr = 0;
  int   newmaxsize;

  newlen = len+ 1;
  if (newlen >= maxsize-1) {
    newmaxsize = 2*maxsize;
    if (newlen >= newmaxsize -1) newmaxsize = newlen + 1;
    nstr = get_pool(newmaxsize,newmaxsize);
    strcpy(nstr,str);
    put_pool(str,maxsize);
    maxsize = newmaxsize;
    str = nstr;
  }
  str[len++] = nc;
  str[len] = 0;
}

// -----------------------------------------------------------------
// String::insert(const char *newstr)
//
// Inserts a string into the front of a string.  (Primarily used
// for documentation generation)
// -----------------------------------------------------------------

void String::insert(const char *newstr) {
  int  newlen;
  char *nstr = 0;
  int  newmaxsize;
  int  i,l;

  l = strlen(newstr);
  newlen = len + l + 1;
  if (newlen >= maxsize-1) {
    newmaxsize = 2*maxsize;
    if (newlen >= newmaxsize -1) newmaxsize = newlen + 1;
    nstr = get_pool(newmaxsize,newmaxsize);
    strcpy(nstr,str);
    put_pool(str,maxsize);
    maxsize = newmaxsize;
    str = nstr;
  }
  
  /* Shift all of the characters over */

  for (i = len -1; i >= 0; i--) {
    str[i+l] = str[i];
  }
  
  /* Now insert the new string */

  strncpy(str,newstr,l);
  len += l;
  str[len] = 0;                         

}

// -----------------------------------------------------------------
// char *String::get()
//
// Get the current value of the string
// -----------------------------------------------------------------

char *String::get() const {
  return str;
}

// -----------------------------------------------------------------
// String &operator<<(...)
//
// Shorthand for appending to the end of a string
// -----------------------------------------------------------------

String &operator<<(String &t,const char *s) {
  t.add(s);
  return t;
}


String &operator<<(String &t,const char s) {
  t.add(s);
  return t;
}

String &operator<<(String &t,const int a) {
  char  temp[64];
  sprintf(temp,"%d",a);
  t.add(temp);
  return t;
}

String &operator<<(String &t, String &s) {
  t.add(s.get());
  return t;
}

String &String::operator=(const char *s) {
  int newlen;

  if (s) {
    newlen = strlen(s);
    if ((newlen >= maxsize) && (str)) {
      put_pool(str,maxsize);
      str = get_pool(newlen+1,maxsize);
      maxsize = newlen+1;
    }
    strcpy(str,s);
    len = newlen;
  } else {
    str[0] = 0;
    len = 0;
  }
  return *this;
}

// -----------------------------------------------------------------
// String &operator>>(...)
//
// Shorthand for inserting into the beginning of a string
// -----------------------------------------------------------------

String &operator>>(const char *s, String &t) {
  t.insert(s);
  return t;
}

String &operator>>(String &s, String &t) {
  t.insert(s.get());
  return t;
}

// -----------------------------------------------------------------
// void String::untabify()
//
// Expand all tabs into spaces.   This is useful for producing
// documentation and other things.
// -----------------------------------------------------------------

void String::untabify() {
  char *s;
  char *c;
  int   pos;
  int   i;
  int   oldmaxsize;
  // Copy the current string representation

  s = str;
  oldmaxsize = maxsize;

  // Reset the internal state of this string

  len = 0;
  str = get_pool(maxsize,maxsize);
  str[0]= 0;

  // Now walk down the old string and expand tabs.  Tabs are usually place
  // every 8 characters.

  pos = 0;
  c = s;
  while (*c) {
    if (*c == '\n') {
      pos = -1;
    }
    if (*c == '\t') {
      // Expand the character
      for (i = 0; i < (8 - (pos % 8)); i++) {
	this->add(' ');
      }
      pos+=(8-(pos % 8));
    } else {
      this->add(*c);
      pos++;
    }
    c++;
  }

  // Blow away the old string
  put_pool(s,oldmaxsize);
}


// -----------------------------------------------------------------
// void String::replace(const char *token, const char *rep)
//
// Search for tokens in a string and replace them with rep.
// This probably isn't the fastest implementation, but fortunately
// SWIG rarely calls this function.
// -----------------------------------------------------------------

void String::replace(const char *token, const char *rep) {
  char *s, *c, *t;
  int  oldmaxsize = maxsize;
  // Copy the current string representation

  s = str;

  // Now walk down the old string and search for tokens

  c = s;
  t = strstr(c,token);
  if (t) {
    len = 0;
    str = get_pool(maxsize,maxsize);
    while (t) {
      // Found a token in string s
      // Dump characters into our string
      char temp;
      temp = *t;
      *t = 0;
      this->add(c);
      c = t;
      *t = temp;

      // Now dump the replacement string into place

      this->add(rep);
      
      // Jump over the token
      
      c+=strlen(token);
      t = strstr(c,token);
    }
    // Attach rest of the string
    if (*c) 
      this->add(c);
    put_pool(s,oldmaxsize);
  }
}


// -----------------------------------------------------------------
// void String::replaceid(char *token, char *rep)
//
// Searches for valid identifiers matching token and replaces
// them with rep.    Unlike replace() tokens must be a valid C
// identifier (surrounded by whitespace).
// -----------------------------------------------------------------

void String::replaceid(const char *token, const char *rep) {
  char *s, *c, *t;
  int  whitespace, tokenlen;
  int  oldmaxsize = maxsize;
  // Copy the current string representation

  s = str;

  // Reset the internal state of this string

  tokenlen = strlen(token);

  // Now walk down the old string and search for tokens

  c = s;
  t = strstr(c,token);
  if (t) {
    len = 0;
    str = get_pool(maxsize,maxsize);
    while (t) {
      // Found a token in string s
      // Dump characters into our string

      whitespace = 1;
      while (c != t) {
	this->add(*c);
	if (!(isalpha(*c) || (*c == '_') || (*c == '$'))) whitespace = 1;
	else whitespace = 0;
	c++;
      }
    
      if (whitespace) {
	// Check to see if there is whitespace afterwards
	if ((!c[tokenlen]) || (!(isalnum(c[tokenlen]) || (c[tokenlen] == '_') || (c[tokenlen] == '$')))) {
	  this->add(rep);
	} else {
	  this->add(token);
	}
	c+=tokenlen;
      } else {
	this->add(*c);
	c++;
      }
      t = strstr(c,token);
    }

    // Attach rest of the string
    if (*c) 
      this->add(c);

    // Delete the old string
    put_pool(s,oldmaxsize);
  }
}


// -----------------------------------------------------------------
// void String::strip()
//
// Intelligently strips whitespace from a string.    Will not strip
// whitespace if it is between two characters that are part of a
// legal C identifier. For example 'unsigned int'.
// -----------------------------------------------------------------

void String::strip() {
  char *s = str;          // Old pointer value
  char *c, lastchar = 0;    
  int   whitespace = 0;
  int   oldmaxsize = maxsize;

  str = get_pool(maxsize,maxsize);   // Get a new string.
  len = 0;

  c = s;
  while(*c) {
    if (!isspace(*c)) {
      // See if this character doesn't violate our whitespace rules
      if (whitespace) {
	if (isalnum(lastchar) || (lastchar == '_') || (lastchar == '$')) {
	  if (isalnum(*c) || (*c == '_') || (*c == '$')) 
	    this->add(' ');
	}
      }
      this->add(*c);
      lastchar = *c;
      whitespace = 0;
    } else {
      whitespace = 1;
    }
    c++;
  }
  put_pool(s,oldmaxsize);
}
