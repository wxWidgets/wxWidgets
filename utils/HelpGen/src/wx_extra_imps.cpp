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

#ifdef __WXMSW__
// from filefn.cpp
void WXDLLEXPORT wxSplitPath(const char *pszFileName,
                             wxString *pstrPath,
                             wxString *pstrName,
                             wxString *pstrExt)
{
  wxCHECK_RET( pszFileName, _("NULL file name in wxSplitPath") );

  const char *pDot = strrchr(pszFileName, FILE_SEP_EXT);
  const char *pSepUnix = strrchr(pszFileName, FILE_SEP_PATH_UNIX);
  const char *pSepDos = strrchr(pszFileName, FILE_SEP_PATH_DOS);

  // take the last of the two
  size_t nPosUnix = pSepUnix ? pSepUnix - pszFileName : 0;
  size_t nPosDos = pSepDos ? pSepDos - pszFileName : 0;
  if ( nPosDos > nPosUnix )
    nPosUnix = nPosDos;
//  size_t nLen = Strlen(pszFileName);

  if ( pstrPath )
    *pstrPath = wxString(pszFileName, nPosUnix);
  if ( pDot ) {
    size_t nPosDot = pDot - pszFileName;
    if ( pstrName )
      *pstrName = wxString(pszFileName + nPosUnix + 1, nPosDot - nPosUnix);
    if ( pstrExt )
      *pstrExt = wxString(pszFileName + nPosDot + 1);
  }
  else {
    if ( pstrName )
      *pstrName = wxString(pszFileName + nPosUnix + 1);
    if ( pstrExt )
      pstrExt->Empty();
  }
}

wxLocale *wxGetLocale()
{
    return NULL;
}

const char *wxLocale::GetString(const char *szOrigString, 
                                const char *) const
{
    return szOrigString;
}

#else // !MSW
const char *wxGetTranslation(const char *str)
{
    return str;
}
#endif // MSW