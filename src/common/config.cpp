///////////////////////////////////////////////////////////////////////////////
// Name:        config.cpp
// Purpose:     implementation of wxConfig class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     07.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Karsten Ballüder   Ballueder@usa.net  
//                       Vadim Zeitlin      <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================
#ifdef __GNUG__
#pragma implementation "app.h"
#endif

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#include  <wx/string.h>
#include  <wx/intl.h>
#include  <wx/file.h>
#include  <wx/log.h>
#include  <wx/textfile.h>
#include  <wx/config.h>

#include <stdlib.h>
#include <ctype.h>

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxConfig
// ----------------------------------------------------------------------------
wxConfig::~wxConfig()
{
}

// ----------------------------------------------------------------------------
// static & global functions
// ----------------------------------------------------------------------------

// understands both Unix and Windows (but only under Windows) environment
// variables expansion: i.e. $var, $(var) and ${var} are always understood
// and in addition under Windows %var% is also.
wxString ExpandEnvVars(const wxString& str)
{
  wxString strResult;

  // don't change the values the enum elements: they must be equal
  // to the matching [closing] delimiter.
  enum Bracket
  { 
    Bracket_None, 
    Bracket_Normal  = ')', 
    Bracket_Curly   = '}',
#ifdef  __WINDOWS__
    Bracket_Windows = '%'     // yeah, Windows people are a bit strange ;-)
#endif
  };

  uint m;
  for ( uint n = 0; n < str.Len(); n++ ) {
    switch ( str[n] ) {
#ifdef  __WINDOWS__
      case '%':
#endif  //WINDOWS
      case '$':
        {
          Bracket bracket;
          #ifdef  __WINDOWS__
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
            #ifdef  __WINDOWS__
              if ( bracket != Bracket_Windows )
            #endif
                if ( bracket != Bracket_None )
                  strResult << str[n - 1];
            strResult << str[n] << strVarName;
          }

          // check the closing bracket
          if ( bracket != Bracket_None ) {
            if ( m == str.Len() || str[m] != (char)bracket ) {
              wxLogWarning("missing '%c' at position %d in '%s'.",
                           (char)bracket, m + 1, str.c_str());
            }
            else {
              // skip closing bracket
              if ( pszValue == NULL )
                strResult << (char)bracket;
              m++;
            }
          }

          n = m - 1;  // skip variable name
        }
        break;

      case '\\':
        n++;
        // fall through

      default:
        strResult += str[n];
    }
  }

  return strResult;
}
