/////////////////////////////////////////////////////////////////////////////
// Name:        strconv.cpp
// Purpose:     Unicode conversion classes
// Author:      Ove Kaaven, Robert Roebling, Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Ove Kaaven, Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "strconv.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef __SALFORDC__
  #include <clib.h>
#endif

#include "wx/debug.h"
#include "wx/strconv.h"

//----------------------------------------------------------------------------
// wxConvCurrent
//----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConv *) wxConvCurrent = &wxConvLibc;

#if !wxUSE_WCHAR_T
//----------------------------------------------------------------------------
// stand-ins in absence of wchar_t
//----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConv) wxConvLibc, wxConvFile;

#else

//----------------------------------------------------------------------------
// wxMBConv
//----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConv) wxConvLibc;

size_t wxMBConv::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    return wxMB2WC(buf, psz, n);
}

size_t wxMBConv::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    return wxWC2MB(buf, psz, n);
}

const wxWCharBuffer wxMBConv::cMB2WC(const char *psz) const
{
    if (psz) 
    {
        size_t nLen = MB2WC((wchar_t *) NULL, psz, 0);
        wxWCharBuffer buf(nLen);
        MB2WC((wchar_t *)(const wchar_t *) buf, psz, nLen);
        return buf;
    } 
    else 
        return wxWCharBuffer((wchar_t *) NULL);
}

const wxCharBuffer wxMBConv::cWC2MB(const wchar_t *psz) const
{
    if (psz) 
    {
        size_t nLen = WC2MB((char *) NULL, psz, 0);
        wxCharBuffer buf(nLen);
        WC2MB((char *)(const char *) buf, psz, nLen);
        return buf;
    } 
    else 
        return wxCharBuffer((char *) NULL);
}

//----------------------------------------------------------------------------
// standard file conversion
//----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConvFile) wxConvFile;

// just use the libc conversion for now
size_t wxMBConvFile::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    return wxMB2WC(buf, psz, n);
}

size_t wxMBConvFile::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    return wxWC2MB(buf, psz, n);
}

#ifdef __WXGTK12__

//----------------------------------------------------------------------------
// standard gdk conversion
//----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConvGdk) wxConvGdk;

#include <gdk/gdk.h>

size_t wxMBConvGdk::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
  if (buf) {
    return gdk_mbstowcs((GdkWChar *)buf, psz, n);
  } else {
    GdkWChar *nbuf = new GdkWChar[n=strlen(psz)];
    size_t len = gdk_mbstowcs(nbuf, psz, n);
    delete [] nbuf;
    return len;
  }
}

size_t wxMBConvGdk::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
  char *mbstr = gdk_wcstombs((GdkWChar *)psz);
  size_t len = mbstr ? strlen(mbstr) : 0;
  if (buf) {
    if (len > n) len = n;
    memcpy(buf, psz, len);
    if (len < n) buf[len] = 0;
  }
  return len;
}
#endif // GTK > 1.0

// ----------------------------------------------------------------------------
// UTF-7
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConvUTF7) wxConvUTF7;

#if 0
static char utf7_setD[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz"
                        "0123456789'(),-./:?";
static char utf7_setO[]="!\"#$%&*;<=>@[]^_`{|}";
static char utf7_setB[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz"
                        "0123456789+/";
#endif

// TODO: write actual implementations of UTF-7 here
size_t wxMBConvUTF7::MB2WC(wchar_t * WXUNUSED(buf),
                           const char * WXUNUSED(psz),
                           size_t WXUNUSED(n)) const
{
  return 0;
}

size_t wxMBConvUTF7::WC2MB(char * WXUNUSED(buf),
                           const wchar_t * WXUNUSED(psz),
                           size_t WXUNUSED(n)) const
{
  return 0;
}

//----------------------------------------------------------------------------
// UTF-8
//----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConvUTF8) wxConvUTF8;

static unsigned long utf8_max[]={0x7f,0x7ff,0xffff,0x1fffff,0x3ffffff,0x7fffffff,0xffffffff};

size_t wxMBConvUTF8::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
  size_t len = 0;

  while (*psz && ((!buf) || (len<n))) {
    unsigned char cc=*psz++, fc=cc;
    unsigned cnt;
    for (cnt=0; fc&0x80; cnt++) fc<<=1;
    if (!cnt) {
      // plain ASCII char
      if (buf) *buf++=cc;
      len++;
    } else {
      cnt--;
      if (!cnt) {
        // invalid UTF-8 sequence
        return (size_t)-1;
      } else {
        unsigned ocnt=cnt-1;
        unsigned long res=cc&(0x3f>>cnt);
        while (cnt--) {
          cc = *psz++;
          if ((cc&0xC0)!=0x80) {
            // invalid UTF-8 sequence
            return (size_t)-1;
          }
          res=(res<<6)|(cc&0x3f);
        }
        if (res<=utf8_max[ocnt]) {
          // illegal UTF-8 encoding
          return (size_t)-1;
        }
        if (buf) *buf++=res;
        len++;
      }
    }
  }
  if (buf && (len<n)) *buf = 0;
  return len;
}

size_t wxMBConvUTF8::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
  size_t len = 0;

  while (*psz && ((!buf) || (len<n))) {
    unsigned long cc=(*psz++)&0x7fffffff;
    unsigned cnt;
    for (cnt=0; cc>utf8_max[cnt]; cnt++);
    if (!cnt) {
      // plain ASCII char
      if (buf) *buf++=cc;
      len++;
    } else {
      len+=cnt+1;
      if (buf) {
        *buf++=(-128>>cnt)|((cc>>(cnt*6))&(0x3f>>cnt));
        while (cnt--)
          *buf++=0x80|((cc>>(cnt*6))&0x3f);
      }
    }
  }
  if (buf && (len<n)) *buf = 0;
  return len;
}

// ----------------------------------------------------------------------------
// specified character set
// ----------------------------------------------------------------------------

#ifndef WX_PRECOMP
  #include "wx/dynarray.h"
  #include "wx/filefn.h"
  #include "wx/textfile.h"
  #include "wx/tokenzr.h"
  #include "wx/utils.h"
#endif

class wxCharacterSet
{
public:
  wxArrayString names;
  wchar_t *data;
};

WX_DECLARE_OBJARRAY(wxCharacterSet, wxCSArray);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxCSArray);

static wxCSArray wxCharsets;

static void wxLoadCharacterSets(void)
{
  static bool already_loaded = FALSE;

  if (already_loaded) return;

  already_loaded = TRUE;
#if defined(__UNIX__) && wxUSE_TEXTFILE
  // search through files in /usr/share/i18n/charmaps
  wxString fname;
  for (fname = ::wxFindFirstFile(wxT("/usr/share/i18n/charmaps/*"));
       !fname.IsEmpty();
       fname = ::wxFindNextFile()) {
    wxTextFile cmap(fname);
    if (cmap.Open()) {
      wxCharacterSet *cset = new wxCharacterSet;
      wxString comchar,escchar;
      bool in_charset = FALSE;

      // wxFprintf(stderr,wxT("Loaded: %s\n"),fname.c_str());

      wxString line;
      for (line = cmap.GetFirstLine();
           !cmap.Eof();
           line = cmap.GetNextLine()) {
        // wxFprintf(stderr,wxT("line contents: %s\n"),line.c_str());
        wxStringTokenizer token(line);
        wxString cmd = token.GetNextToken();
        if (cmd == comchar) {
          if (token.GetNextToken() == wxT("alias"))
            cset->names.Add(token.GetNextToken());
        }
        else if (cmd == wxT("<code_set_name>"))
          cset->names.Add(token.GetNextToken());
        else if (cmd == wxT("<comment_char>"))
          comchar = token.GetNextToken();
        else if (cmd == wxT("<escape_char>"))
          escchar = token.GetNextToken();
        else if (cmd == wxT("<mb_cur_min>")) {
          delete cset;
          cset = (wxCharacterSet *) NULL;
          break; // we don't support multibyte charsets ourselves (yet)
        }
        else if (cmd == wxT("CHARMAP")) {
          cset->data = (wchar_t *)calloc(256, sizeof(wchar_t));
          in_charset = TRUE;
        }
        else if (cmd == wxT("END")) {
          if (token.GetNextToken() == wxT("CHARMAP"))
            in_charset = FALSE;
        }
        else if (in_charset) {
          // format: <NUL> /x00 <U0000> NULL (NUL)
          //         <A>   /x41 <U0041> LATIN CAPITAL LETTER A
          wxString hex = token.GetNextToken();
          // skip whitespace (why doesn't wxStringTokenizer do this?)
          while (wxIsEmpty(hex) && token.HasMoreTokens()) hex = token.GetNextToken();
          wxString uni = token.GetNextToken();
          // skip whitespace again
          while (wxIsEmpty(uni) && token.HasMoreTokens()) uni = token.GetNextToken();
          if ((hex.Len() > 2) && (wxString(hex.GetChar(0)) == escchar) && (hex.GetChar(1) == wxT('x')) &&
              (uni.Left(2) == wxT("<U"))) {
            hex.MakeUpper(); uni.MakeUpper();
            int pos = ::wxHexToDec(hex.Mid(2,2));
            if (pos>=0) {
              unsigned long uni1 = ::wxHexToDec(uni.Mid(2,2));
              unsigned long uni2 = ::wxHexToDec(uni.Mid(4,2));
              cset->data[pos] = (uni1 << 16) | uni2;
              // wxFprintf(stderr,wxT("char %02x mapped to %04x (%c)\n"),pos,cset->data[pos],cset->data[pos]);
            }
          }
        }
      }
      if (cset) {
        cset->names.Shrink();
        wxCharsets.Add(cset);
      }
    }
  }
#endif
  wxCharsets.Shrink();
}

static wxCharacterSet *wxFindCharacterSet(const wxChar *charset)
{
  if (!charset) return (wxCharacterSet *)NULL;
  wxLoadCharacterSets();
  for (size_t n=0; n<wxCharsets.GetCount(); n++)
    if (wxCharsets[n].names.Index(charset) != wxNOT_FOUND)
      return &(wxCharsets[n]);
  return (wxCharacterSet *)NULL;
}

WXDLLEXPORT_DATA(wxCSConv) wxConvLocal((const wxChar *)NULL);

wxCSConv::wxCSConv(const wxChar *charset)
{
  m_name = (wxChar *) NULL;
  m_cset = (wxCharacterSet *) NULL;
  m_deferred = TRUE;
  SetName(charset);
}

wxCSConv::~wxCSConv()
{
  if (m_name) free(m_name);
}

void wxCSConv::SetName(const wxChar *charset)
{
  if (charset) {
#ifdef __UNIX__
    // first, convert the character set name to standard form
    wxString codeset;
    if (wxString(charset,3).CmpNoCase(wxT("ISO")) == 0) {
      // make sure it's represented in the standard form: ISO_8859-1
      codeset = wxT("ISO_");
      charset += 3;
      if ((*charset == wxT('-')) || (*charset == wxT('_'))) charset++;
      if (wxStrlen(charset)>4) {
        if (wxString(charset,4) == wxT("8859")) {
          codeset << wxT("8859-");
          if (*charset == wxT('-')) charset++;
        }
      }
    }
    codeset << charset;
    codeset.MakeUpper();
    m_name = wxStrdup(codeset.c_str());
    m_deferred = TRUE;
#endif
  }
}

void wxCSConv::LoadNow()
{
//  wxPrintf(wxT("Conversion request\n"));
  if (m_deferred) {
    if (!m_name) {
#ifdef __UNIX__
      wxChar *lang = wxGetenv(wxT("LANG"));
      wxChar *dot = lang ? wxStrchr(lang, wxT('.')) : (wxChar *)NULL;
      if (dot) SetName(dot+1);
#endif
    }
    m_cset = wxFindCharacterSet(m_name);
    m_deferred = FALSE;
  }
}

size_t wxCSConv::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
  ((wxCSConv *)this)->LoadNow(); // discard constness
  if (buf) {
    if (m_cset) {
      for (size_t c=0; c<n; c++)
        buf[c] = m_cset->data[(unsigned char)(psz[c])];
    } else {
      // latin-1 (direct)
      for (size_t c=0; c<n; c++)
        buf[c] = (unsigned char)(psz[c]);
    }
    return n;
  }
  return strlen(psz);
}

size_t wxCSConv::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
  ((wxCSConv *)this)->LoadNow(); // discard constness
  if (buf) {
    if (m_cset) {
      for (size_t c=0; c<n; c++) {
        size_t n;
        for (n=0; (n<256) && (m_cset->data[n] != psz[c]); n++);
        buf[c] = (n>0xff) ? '?' : n;
      }
    } else {
      // latin-1 (direct)
      for (size_t c=0; c<n; c++)
        buf[c] = (psz[c]>0xff) ? '?' : psz[c];
    }
    return n;
  }
  return wcslen(psz);
}

#endif
  //wxUSE_WCHAR_T


