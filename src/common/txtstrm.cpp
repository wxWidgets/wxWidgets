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

wxTextInputStream::wxTextInputStream(wxInputStream& s)
  : m_input(&s)
{
}

wxTextInputStream::~wxTextInputStream()
{
}

wxChar wxTextInputStream::NextNonWhiteSpace()
{
    wxChar c = (wxChar) 0;
    for (;;)
    {
        c = m_input->GetC();
	if (!m_input) return (wxChar) 0;
	
	if (c != _T('\n') &&
	    c != _T('\r') &&
	    c != _T('\t') &&
	    c != _T(' '))
	{
	    return c;
	}
    }

    // this shouldn't happen
    return (wxChar) 0;
}

void wxTextInputStream::SkipIfEndOfLine( wxChar c )
{
    if (c == _T('\n'))
    {
        // eat on UNIX
	return;
    }

    if (c == _T('\r'))
    {
        // eat on both Mac and DOS
	
        wxChar c2 = m_input->GetC();
        if (!m_input) return;
	
        if (c2 == _T('\n'))
        {
	    // eat on DOS
	    return;
	}
        else
	{
	    // Don't eat on Mac
            m_input->Ungetch( c2 );
	}
    }
    else
    {
        // no line terminator
        m_input->Ungetch( c );
    }
}

wxUint32 wxTextInputStream::Read32()
{
    /* I only implemented a simple integer parser */
    int sign;
    wxInt32 i;

    int c = NextNonWhiteSpace();
    if (!m_input) return 0;

    i = 0;
    if (! (c == _T('-') || c == _T('+') || isdigit(c)) )
    {
        m_input->Ungetch(c);
        return 0;
    }

    if (c == _T('-'))
    {
        sign = -1;
        c = m_input->GetC();
    } else
    if (c == _T('+'))
    {
        sign = 1;
        c = m_input->GetC();
    } else
    {
        sign = 1;
    }

    while (isdigit(c))
    {
        i = i*10 + (c - (int)_T('0'));
        c = m_input->GetC();
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

    int c = NextNonWhiteSpace();
    if (!m_input) return 0.0;

    f = 0.0;
    if (! (c == _T('.') || c == _T('-') || c == _T('+') || isdigit(c)) )
    {
        m_input->Ungetch(c);
        return 0.0;
    }

    if (c == _T('-'))
    {
        sign = -1;
        c = m_input->GetC();
    } else
    if (c == _T('+'))
    {
        sign = 1;
        c = m_input->GetC();
    }
    else
    {
        sign = 1;
    }

    while (isdigit(c))
    {
        f = f*10 + (c - _T('0'));
        c = m_input->GetC();
    }

    if (c == _T('.'))
    {
        double f_multiplicator = (double) 0.1;

        c = m_input->GetC();

        while (isdigit(c))
	{
            f += (c-_T('0'))*f_multiplicator;
            f_multiplicator /= 10;
            c = m_input->GetC();
        }

        if (c == _T('e'))
	{
            double f_multiplicator = 0.0;
            int i, e;

            c = m_input->GetC();

            switch (c)
	    {
                case _T('-'): f_multiplicator = 0.1;  break;
		case _T('+'): f_multiplicator = 10.0; break;
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
        m_input->Ungetch(c);
    }

    f *= sign;

    return f;
}

wxString wxTextInputStream::ReadString()
{
    wxChar c;
    wxString line;

    for (;;)
    {
        c = m_input->GetC();
        if (!m_input) break;
	
        if (c == _T('\n'))
        {
	    // eat on UNIX
	    break;
	}
	
        if (c == _T('\r'))
        {
            // eat on both Mac and DOS
	
            wxChar c2 = m_input->GetC();
	    if (!m_input) break;
	
	    if (c2 == _T('\n'))
            {
	        // eat on DOS
		break;
	    }
            else
	    {
	        // Don't eat on Mac
                m_input->Ungetch( c2 );
		break;
	    }
        }
	
        line += c;
    }

    return line;
}

wxTextInputStream& wxTextInputStream::operator>>(wxString& line)
{
  line = ReadString();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxChar& c)
{
    wxChar c1 = m_input->GetC();
    if (!m_input)
    {
        c = (wxChar) 0;
        return *this;
    }

    if (c1 == _T('\r'))
    {
        c = _T('\n');
        wxChar c2 = m_input->GetC();
	if (!m_input) return *this;
	
	if (c2 != _T('\n'))
	{
	    // we are on a Mac
            m_input->Ungetch( c2 );
	}
    }
    else
    {
        c = c1;
    }

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
  : m_output(&s)
{
}

wxTextOutputStream::~wxTextOutputStream()
{
}

void wxTextOutputStream::Write32(wxUint32 i)
{
    wxString str;
    str.Printf(_T("%u"), i);

    WriteString(str);
}

void wxTextOutputStream::Write16(wxUint16 i)
{
    wxString str;
    str.Printf(_T("%u"), i);

    WriteString(str);
}

void wxTextOutputStream::Write8(wxUint8 i)
{
    wxString str;
    str.Printf(_T("%u"), i);

    WriteString(str);
}

void wxTextOutputStream::WriteDouble(double d)
{
    wxString str;

    str.Printf(_T("%f"), d);
    WriteString(str);
}

void wxTextOutputStream::WriteString(const wxString& string)
{
    for (size_t i = 0; i < string.Len(); i++)
    {
        wxChar c = string[i];
        if (c == _T('\n'))
        {
#if   defined(__WINDOWS__)
            c = _T('\r');
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
            c = _T('\n');
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
#elif defined(__UNIX__)
            c = _T('\n');
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
#elif defined(__WXMAC__)
            c = _T('\r');
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
#elif   defined(__OS2__)
            c = _T('\r');
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
            c = _T('\n');
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
#else
            #error  "wxTextOutputStream: unsupported platform."
#endif
        }
        else
        {
            m_output->Write( (const void*)(&c), sizeof(wxChar) );
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
    Write16( (wxUint16)c );
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt32 c)
{
    Write32( (wxUint32)c );
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint16 c)
{
    Write16(c);
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint32 c)
{
    Write32(c);
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
    return stream << _T('\n');
}

#endif
  // wxUSE_STREAMS
