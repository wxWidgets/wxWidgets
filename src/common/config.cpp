///////////////////////////////////////////////////////////////////////////////
// Name:        config.cpp
// Purpose:     implementation of wxConfigBase class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Karsten Ballüder   Ballueder@usa.net
//                       Vadim Zeitlin      <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
  #pragma implementation "confbase.h"
#endif

#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include  <wx/confbase.h>

#ifdef wxUSE_CONFIG

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#include  <wx/app.h>
#include  <wx/file.h>
#include  <wx/log.h>
#include  <wx/textfile.h>
#include  <wx/utils.h>

// we must include (one of) these files for wxConfigBase::Create
#if defined(__WXMSW__) && defined(wxCONFIG_WIN32_NATIVE)
  #ifdef __WIN32__
    #include  <wx/msw/regconf.h>
  #else  //WIN16
    #include  <wx/msw/iniconf.h>
  #endif
#else // either we're under Unix or wish to use files even under Windows
  #include  <wx/fileconf.h>
#endif

#include  <stdlib.h>
#include  <math.h>
#include  <ctype.h>       // for isalnum()

// ----------------------------------------------------------------------------
// global and class static variables
// ----------------------------------------------------------------------------

wxConfigBase *wxConfigBase::ms_pConfig     = NULL;
bool          wxConfigBase::ms_bAutoCreate = TRUE;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxConfigBase
// ----------------------------------------------------------------------------

// Not all args will always be used by derived classes, but
// including them all in each class ensures compatibility.
wxConfigBase::wxConfigBase(const wxString& appName, const wxString& vendorName,
    const wxString& WXUNUSED(localFilename), const wxString& WXUNUSED(globalFilename), long style):
        m_appName(appName), m_vendorName(vendorName), m_style(style)
{
    m_bExpandEnvVars = TRUE; m_bRecordDefaults = FALSE;
}

wxConfigBase *wxConfigBase::Set(wxConfigBase *pConfig)
{
  wxConfigBase *pOld = ms_pConfig;
  ms_pConfig = pConfig;
  return pOld;
}

wxConfigBase *wxConfigBase::Create()
{
  if ( ms_bAutoCreate && ms_pConfig == NULL ) {
    ms_pConfig =
    #if defined(__WXMSW__) && defined(wxCONFIG_WIN32_NATIVE)
      #ifdef __WIN32__
        new wxRegConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName());
      #else  //WIN16
        new wxIniConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName());
      #endif
    #else // either we're under Unix or wish to use files even under Windows
      new wxFileConfig(wxTheApp->GetAppName());
    #endif
  }

  return ms_pConfig;
}

wxString wxConfigBase::Read(const wxString& key, const wxString& defVal) const
{
  wxString s;
  Read(key, &s, defVal);
  return s;
}

bool wxConfigBase::Read(const wxString& key, wxString *str, const wxString& defVal) const
{
    if (!Read(key, str))
    {
        *str = ExpandEnvVars(defVal);
        return FALSE;
    }
    else
        return TRUE;
}

bool wxConfigBase::Read(const wxString& key, long *pl, long defVal) const
{
    if (!Read(key, pl))
    {
        *pl = defVal;
        return FALSE;
    }
    else
        return TRUE;
}

bool wxConfigBase::Read(const wxString& key, double* val) const
{
    wxString str;
    if (Read(key, & str))
    {
        *val = atof(str);
        return TRUE;
    }
    else
        return FALSE;
}

bool wxConfigBase::Read(const wxString& key, double* val, double defVal) const
{
    if (!Read(key, val))
    {
        *val = defVal;
        return FALSE;
    }
    else
        return TRUE;
}

bool wxConfigBase::Read(const wxString& key, bool* val) const
{
    long l;
    if (Read(key, & l))
    {
        *val = (l != 0);
        return TRUE;
    }
    else
        return FALSE;
}

bool wxConfigBase::Read(const wxString& key, bool* val, bool defVal) const
{
    if (!Read(key, val))
    {
        *val = defVal;
        return FALSE;
    }
    else
        return TRUE;
}

// Convenience functions

bool wxConfigBase::Read(const wxString& key, int *pi) const
{
    long l;
    bool ret = Read(key, &l);
    if (ret)
        *pi = (int) l;
    return ret;
}

bool wxConfigBase::Read(const wxString& key, int *pi, int defVal) const
{
    long l;
    bool ret = Read(key, &l, (long) defVal);
    *pi = (int) l;
    return ret;
}

bool wxConfigBase::Write(const wxString& key, double val)
{
    wxString str;
    str.Printf("%f", val);
    return Write(key, str);
}

bool wxConfigBase::Write(const wxString& key, bool value)
{
    long l = (value ? 1 : 0);
    return Write(key, l);
}

wxString wxConfigBase::ExpandEnvVars(const wxString& str) const
{
    wxString tmp; // Required for BC++
    if (IsExpandingEnvVars())
        tmp = wxExpandEnvVars(str);
    else
        tmp = str;
    return tmp;
}

// ----------------------------------------------------------------------------
// wxConfigPathChanger
// ----------------------------------------------------------------------------

wxConfigPathChanger::wxConfigPathChanger(const wxConfigBase *pContainer,
                                 const wxString& strEntry)
{
  m_pContainer = (wxConfigBase *)pContainer;
//  wxString strPath = strEntry.Before(wxCONFIG_PATH_SEPARATOR);
  wxString strPath = strEntry.BeforeLast(wxCONFIG_PATH_SEPARATOR);

  // special case of "/keyname" when there is nothing before "/"
  if ( strPath.IsEmpty() && ((!strEntry.IsEmpty()) && strEntry[0] == wxCONFIG_PATH_SEPARATOR ))
    strPath = wxCONFIG_PATH_SEPARATOR;

  if ( !strPath.IsEmpty() ) {
    // do change the path
    m_bChanged = TRUE;
//    m_strName = strEntry.Right(wxCONFIG_PATH_SEPARATOR);
    m_strName = strEntry.AfterLast(wxCONFIG_PATH_SEPARATOR);
    m_strOldPath = m_pContainer->GetPath();
    m_strOldPath += wxCONFIG_PATH_SEPARATOR;
    m_pContainer->SetPath(strPath);
  }
  else {
    // it's a name only, without path - nothing to do
    m_bChanged = FALSE;
    m_strName = strEntry;
  }
}

wxConfigPathChanger::~wxConfigPathChanger()
{
  // only restore path if it was changed
  if ( m_bChanged ) {
    m_pContainer->SetPath(m_strOldPath);
  }
}

// ----------------------------------------------------------------------------
// static & global functions
// ----------------------------------------------------------------------------

// understands both Unix and Windows (but only under Windows) environment
// variables expansion: i.e. $var, $(var) and ${var} are always understood
// and in addition under Windows %var% is also.
wxString wxExpandEnvVars(const wxString& str)
{
  wxString strResult;
  strResult.Alloc(str.Len());

  // don't change the values the enum elements: they must be equal
  // to the matching [closing] delimiter.
  enum Bracket
  {
    Bracket_None,
    Bracket_Normal  = ')',
    Bracket_Curly   = '}'
#ifdef  __WXMSW__
    ,Bracket_Windows = '%'     // yeah, Windows people are a bit strange ;-)
#endif
  };

  size_t m;
  for ( size_t n = 0; n < str.Len(); n++ ) {
    switch ( str[n] ) {
#ifdef  __WXMSW__
      case '%':
#endif  //WINDOWS
      case '$':
        {
          Bracket bracket;
          #ifdef  __WXMSW__
            if ( str[n] == '%' )
              bracket = Bracket_Windows;
            else
          #endif  //WINDOWS
          if ( n == str.Len() - 1 ) {
            bracket = Bracket_None;
          }
          else {
            switch ( str[n + 1] ) {
              case '(':
                bracket = Bracket_Normal;
                n++;                   // skip the bracket
                break;

              case '{':
                bracket = Bracket_Curly;
                n++;                   // skip the bracket
                break;

              default:
                bracket = Bracket_None;
            }
          }

          m = n + 1;

          while ( m < str.Len() && (isalnum(str[m]) || str[m] == '_') )
            m++;

          wxString strVarName(str.c_str() + n + 1, m - n - 1);

          const char *pszValue = getenv(strVarName);
          if ( pszValue != NULL ) {
            strResult += pszValue;
          }
          else {
            // variable doesn't exist => don't change anything
            #ifdef  __WXMSW__
              if ( bracket != Bracket_Windows )
            #endif
                if ( bracket != Bracket_None )
                  strResult << str[n - 1];
            strResult << str[n] << strVarName;
          }

          // check the closing bracket
          if ( bracket != Bracket_None ) {
            if ( m == str.Len() || str[m] != (char)bracket ) {
              wxLogWarning(_("Environment variables expansion failed: "
                             "missing '%c' at position %d in '%s'."),
                           (char)bracket, m + 1, str.c_str());
            }
            else {
              // skip closing bracket unless the variables wasn't expanded
              if ( pszValue == NULL )
                strResult << (char)bracket;
              m++;
            }
          }

          n = m - 1;  // skip variable name
        }
        break;

      case '\\':
        // backslash can be used to suppress special meaning of % and $
        if ( n != str.Len() && (str[n + 1] == '%' || str[n + 1] == '$') ) {
          strResult += str[++n];

          break;
        }
        //else: fall through

      default:
        strResult += str[n];
    }
  }

  return strResult;
}

// this function is used to properly interpret '..' in path
void wxSplitPath(wxArrayString& aParts, const char *sz)
{
  aParts.Empty();

  wxString strCurrent;
  const char *pc = sz;
  for ( ;; ) {
    if ( *pc == '\0' || *pc == wxCONFIG_PATH_SEPARATOR ) {
      if ( strCurrent == "." ) {
        // ignore
      }
      else if ( strCurrent == ".." ) {
        // go up one level
        if ( aParts.IsEmpty() )
          wxLogWarning(_("'%s' has extra '..', ignored."), sz);
        else
          aParts.Remove(aParts.Count() - 1);

        strCurrent.Empty();
      }
      else if ( !strCurrent.IsEmpty() ) {
        aParts.Add(strCurrent);
        strCurrent.Empty();
      }
      //else:
        // could log an error here, but we prefer to ignore extra '/'

      if ( *pc == '\0' )
        return;
    }
    else
      strCurrent += *pc;

    pc++;
  }
}

#endif

  // wxUSE_CONFIG

