////////////////////
//
// craeted by Alex
//
////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include <memory.h>

#ifndef WX_PRECOMP
    #include "wx/defs.h"
#endif

char *
copystring (const char *s)
{
  if (s == NULL) s = "";
  size_t len = strlen (s) + 1;

  char *news = new char[len];
  memcpy (news, s, len);    // Should be the fastest

  return news;
}

const char *wxGetTranslation(const char *str)
{
    return str;
}
