///////////////////////////////////////////////////////////////////////////////
// Name:        txtstrm.cpp
// Purpose:     Text stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "txtstrm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "wx/txtstrm.h"
#include <ctype.h>


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// Unix: "\n"
// Dos:  "\r\n"
// Mac:  "\r"

// ----------------------------------------------------------------------------
// wxTextInputStream
// ----------------------------------------------------------------------------

wxTextInputStream::wxTextInputStream(wxInputStream &s, const wxString &sep)
  : m_input(s), m_separators(sep)
{
}

wxTextInputStream::~wxTextInputStream()
{
}

wxChar wxTextInputStream::NextNonSeparators()
{
    wxChar c = (wxChar) 0;
    for (;;)
    {
	if (!m_input) return (wxChar) 0;
        c = m_input.GetC();
	
	if (c != wxT('\n') && 
	    c != wxT('\r') &&
	    !m_separators.Contains(c)) 
	  return c;
    }

}

inline bool wxTextInputStream::EatEOL(const wxChar &c)
{
  if (c == wxT('\n')) return TRUE; // eat on UNIX
  
  if (c == wxT('\r')) // eat on both Mac and DOS
    {
      if (!m_input) return TRUE;
      wxChar c2 = m_input.GetC();
      
      if (c2 != wxT('\n'))  m_input.Ungetch( c2 ); // Don't eat on Mac
      return TRUE;
    }

  return FALSE;
}

void wxTextInputStream::SkipIfEndOfLine( wxChar c )
{
    if (EatEOL(c)) return;
    else m_input.Ungetch( c );  // no line terminator
}

wxUint32 wxTextInputStream::Read32()
{
    /* I only implemented a simple integer parser */
    int sign;
    wxInt32 i;

    if (!m_input) return 0;
    int c = NextNonSeparators(); 
    if (c==(wxChar)0) return 0;

    i = 0;
    if (! (c == wxT('-') || c == wxT('+') || isdigit(c)) )
    {
        m_input.Ungetch(c);
        return 0;
    }

    if (c == wxT('-'))
    {
        sign = -1;
        c = m_input.GetC();
    } else
    if (c == wxT('+'))
    {
        sign = 1;
        c = m_input.GetC();
    } else
    {
        sign = 1;
    }

    while (isdigit(c))
    {
        i = i*10 + (c - (int)wxT('0'));
        c = m_input.GetC();
    }

    SkipIfEndOfLine( c );

    i *= sign;

    return (wxUint32)i;
}

wxUint16 wxTextInputStream::Read16()
{
    return (wxUint16)Read32();
}

wxUint8 wxTextInputStream::Read8()
{
    return (wxUint8)Read32();
}

double wxTextInputStream::ReadDouble()
{
    /* I only implemented a simple float parser */
    double f;
    int sign;

    if (!m_input) return 0;
    int c = NextNonSeparators();
    if (c==(wxChar)0) return 0;

    f = 0.0;
    if (! (c == wxT('.') || c == wxT(',') || c == wxT('-') || c == wxT('+') || isdigit(c)) )
    {
        m_input.Ungetch(c);
        return 0.0;
    }

    if (c == wxT('-'))
    {
        sign = -1;
        c = m_input.GetC();
    } else
    if (c == wxT('+'))
    {
        sign = 1;
        c = m_input.GetC();
    }
    else
    {
        sign = 1;
    }

    while (isdigit(c))
    {
        f = f*10 + (c - wxT('0'));
        c = m_input.GetC();
    }

    if (c == wxT('.') || c == wxT(','))
    {
        double f_multiplicator = (double) 0.1;

        c = m_input.GetC();

        while (isdigit(c))
	{
            f += (c-wxT('0'))*f_multiplicator;
            f_multiplicator /= 10;
            c = m_input.GetC();
        }

        if (c == wxT('e'))
	{
            double f_multiplicator = 0.0;
            int i, e;

            c = m_input.GetC();

            switch (c)
	    {
                case wxT('-'): f_multiplicator = 0.1;  break;
		case wxT('+'): f_multiplicator = 10.0; break;
	    }

            e = Read8();  // why only max 256 ?

            for (i=0;i<e;i++)
                f *= f_multiplicator;
        }
	else
	    SkipIfEndOfLine( c );
    }
    else
    {
        m_input.Ungetch(c);
    }

    f *= sign;

    return f;
}

wxString wxTextInputStream::ReadString()
{
  return ReadLine();
}

wxString wxTextInputStream::ReadLine()
{
    wxChar c;
    wxString line;

    for (;;)
    {
        if (!m_input) break;
        c = m_input.GetC();
	
	if (EatEOL(c)) break;
	
        line += c;
    }

    return line;
}

wxString wxTextInputStream::ReadWord()
{
    if (!m_input) return "";
  
    wxString word;
    wxChar c=NextNonSeparators();
    if (c==(wxChar)0) return "";

    for (;;)
    {
	if (m_separators.Contains(c)) break;
	
	if (EatEOL(c)) break;
	
        word += c;

	if (!m_input) break;
        c = m_input.GetC();
    }

    return word;
}

wxTextInputStream& wxTextInputStream::operator>>(wxString& word)
{
  word = ReadWord();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxChar& c)
{
    if (!m_input)
    {
        c = (wxChar) 0;
        return *this;
    }

    c = m_input.GetC();

    if (EatEOL(c)) c=wxT('\n');
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxInt16& i)
{
    i = (wxInt16)Read16();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxInt32& i)
{
    i = (wxInt32)Read32();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxUint16& i)
{
    i = Read16();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxUint32& i)
{
    i = Read32();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(double& i)
{
    i = ReadDouble();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(float& f)
{
    f = (float)ReadDouble();
    return *this;
}

wxTextOutputStream::wxTextOutputStream(wxOutputStream& s)
  : m_output(s)
{
}

wxTextOutputStream::~wxTextOutputStream()
{
}

void wxTextOutputStream::Write32(wxUint32 i)
{
    wxString str;
    str.Printf(wxT("%u"), i);

    WriteString(str);
}

void wxTextOutputStream::Write16(wxUint16 i)
{
    wxString str;
    str.Printf(wxT("%u"), i);

    WriteString(str);
}

void wxTextOutputStream::Write8(wxUint8 i)
{
    wxString str;
    str.Printf(wxT("%u"), i);

    WriteString(str);
}

void wxTextOutputStream::WriteDouble(double d)
{
    wxString str;

    str.Printf(wxT("%f"), d);
    WriteString(str);
}

void wxTextOutputStream::WriteString(const wxString& string)
{
    for (size_t i = 0; i < string.Len(); i++)
    {
        wxChar c = string[i];
        if (c == wxT('\n'))
        {
#if   defined(__WINDOWS__)
            c = wxT('\r');
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
            c = wxT('\n');
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
#elif defined(__UNIX__)
            c = wxT('\n');
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
#elif defined(__WXMAC__)
            c = wxT('\r');
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
#elif   defined(__OS2__)
            c = wxT('\r');
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
            c = wxT('\n');
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
#else
            #error  "wxTextOutputStream: unsupported platform."
#endif
        }
        else
        {
            m_output.Write( (const void*)(&c), sizeof(wxChar) );
        }
   }
}

wxTextOutputStream& wxTextOutputStream::operator<<(const wxChar *string)
{
    WriteString( wxString(string) );
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(const wxString& string)
{
    WriteString( string );
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxChar c)
{
    WriteString( wxString(c) );
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt16 c)
{
    wxString str;
    str.Printf(wxT("%d"), (signed int)c);
    WriteString(str);
    
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt32 c)
{
    wxString str;
    str.Printf(wxT("%ld"), (signed long)c);
    WriteString(str);
    
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint16 c)
{
    wxString str;
    str.Printf(wxT("%u"), (unsigned int)c);
    WriteString(str);
    
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint32 c)
{
    wxString str;
    str.Printf(wxT("%lu"), (unsigned long)c);
    WriteString(str);

    return *this;
}

wxTextOutputStream &wxTextOutputStream::operator<<(double f)
{
    WriteDouble(f);
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(float f)
{
    WriteDouble((double)f);
    return *this;
}

wxTextOutputStream &endl( wxTextOutputStream &stream )
{
    return stream << wxT('\n');
}

#endif
  // wxUSE_STREAMS
