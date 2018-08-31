///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/txtstrm.cpp
// Purpose:     Text stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/98
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "wx/txtstrm.h"

#ifndef WX_PRECOMP
    #include "wx/crt.h"
#endif

#include <ctype.h>

// ----------------------------------------------------------------------------
// wxTextInputStream
// ----------------------------------------------------------------------------

#if wxUSE_UNICODE
wxTextInputStream::wxTextInputStream(wxInputStream &s,
                                     const wxString &sep,
                                     const wxMBConv& conv)
  : m_input(s), m_separators(sep), m_conv(conv.Clone())
{
    m_validBegin =
    m_validEnd = 0;

#if SIZEOF_WCHAR_T == 2
    m_lastWChar = 0;
#endif // SIZEOF_WCHAR_T == 2
}
#else
wxTextInputStream::wxTextInputStream(wxInputStream &s, const wxString &sep)
  : m_input(s), m_separators(sep)
{
    m_validBegin =
    m_validEnd = 0;

    m_lastBytes[0] = 0;
}
#endif

wxTextInputStream::~wxTextInputStream()
{
#if wxUSE_UNICODE
    delete m_conv;
#endif // wxUSE_UNICODE
}

void wxTextInputStream::UngetLast()
{
    if ( m_validEnd )
    {
        m_input.Ungetch(m_lastBytes, m_validEnd);

        m_validBegin =
        m_validEnd = 0;
    }
}

wxChar wxTextInputStream::GetChar()
{
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
    // Return the already raed character remaining from the last call to this
    // function, if any.
    if ( m_lastWChar )
    {
        const wxChar wc = m_lastWChar;
        m_lastWChar = 0;
        return wc;
    }
#endif // SIZEOF_WCHAR_T

    // If we have any non-decoded bytes left from the last call, shift them to
    // be at the beginning of the buffer.
    if ( m_validBegin < m_validEnd )
    {
        m_validEnd -= m_validBegin;
        memmove(m_lastBytes, m_lastBytes + m_validBegin, m_validEnd);
    }
    else // All bytes were already decoded and consumed.
    {
        m_validEnd = 0;
    }

    // We may need to decode up to 4 characters if we have input starting with
    // 3 BOM-like bytes, but not actually containing a BOM, as decoding it will
    // only succeed when 4 bytes are read -- and will yield 4 wide characters.
    wxChar wbuf[4];
    for(size_t inlen = 0; inlen < sizeof(m_lastBytes); inlen++)
    {
        if ( inlen >= m_validEnd )
        {
            // actually read the next character
            m_lastBytes[inlen] = m_input.GetC();

            if(m_input.LastRead() <= 0)
                return 0;

            m_validEnd++;
        }
        //else: Retry decoding what we already have in the buffer.

        switch ( m_conv->ToWChar(wbuf, WXSIZEOF(wbuf), m_lastBytes, inlen + 1) )
        {
            case 0:
                // this is a bug in converter object as it should either fail
                // or decode non-empty string to something non-empty
                wxFAIL_MSG("ToWChar() can't return 0 for non-empty input");
                break;

            case wxCONV_FAILED:
                // the buffer probably doesn't contain enough bytes to decode
                // as a complete character, try with more bytes
                break;

            default:
                // If we couldn't decode a single character during the last
                // loop iteration, but decoded more than one of them with just
                // one extra byte, the only explanation is that we were using a
                // wxConvAuto conversion recognizing the initial BOM and that
                // it couldn't detect the presence or absence of BOM so far,
                // but now finally has enough data to see that there is none.
                // As we must have fallen back to Latin-1 in this case, return
                // just the first byte and keep the other ones for the next
                // time.
                m_validBegin = 1;
                return wbuf[0];

#if SIZEOF_WCHAR_T == 2
            case 2:
                // When wchar_t uses UTF-16, we could have decoded a single
                // Unicode code point as 2 wchar_t characters and there is
                // nothing else to do here but to return the first one now and
                // remember the second one for the next call, as there is no
                // way to fit both of them into a single wxChar in this case.
                m_lastWChar = wbuf[1];
#endif // SIZEOF_WCHAR_T == 2
                wxFALLTHROUGH;

            case 1:
                m_validBegin = inlen + 1;

                // we finally decoded a character
                return wbuf[0];
        }
    }

    // There should be no encoding which requires more than 10 bytes to decode
    // at least one character (the most actually seems to be 7: 3 for the
    // initial BOM, which is ignored, and 4 for the longest possible encoding
    // of a Unicode character in UTF-8), so something must be wrong with our
    // conversion but we have no way to signal it from here and just return 0
    // as if we reached the end of the stream.
    m_validBegin = 0;
    m_validEnd = sizeof(m_lastBytes);

    return 0;
#else
    m_lastBytes[0] = m_input.GetC();

    if(m_input.LastRead() <= 0)
    {
        m_validEnd = 0;
        return 0;
    }

    m_validEnd = 1;

    return m_lastBytes[0];
#endif

}

wxChar wxTextInputStream::NextNonSeparators()
{
    for (;;)
    {
        wxChar c = GetChar();
        if (!c)
            return c;

        if (c != wxT('\n') &&
            c != wxT('\r') &&
            m_separators.Find(c) < 0)
          return c;
    }

}

bool wxTextInputStream::EatEOL(const wxChar &c)
{
    if (c == wxT('\n')) return true; // eat on UNIX

    if (c == wxT('\r')) // eat on both Mac and DOS
    {
        wxChar c2 = GetChar();
        if (!c2) return true; // end of stream reached, had enough :-)

        if (c2 != wxT('\n')) UngetLast(); // Don't eat on Mac
        return true;
    }

    return false;
}

wxUint64 wxTextInputStream::Read64(int base)
{
    wxASSERT_MSG( !base || (base > 1 && base <= 36), wxT("invalid base") );
    if(!m_input) return 0;

    wxString word = ReadWord();
    if(word.empty())
        return 0;

    wxUint64 res;
    if(!word.ToULongLong(&res, base))
        return 0;
    return res;
}

wxUint32 wxTextInputStream::Read32(int base)
{
    wxASSERT_MSG( !base || (base > 1 && base <= 36), wxT("invalid base") );
    if(!m_input) return 0;

    wxString word = ReadWord();
    if(word.empty())
        return 0;
    return wxStrtoul(word.c_str(), 0, base);
}

wxUint16 wxTextInputStream::Read16(int base)
{
    return (wxUint16)Read32(base);
}

wxUint8 wxTextInputStream::Read8(int base)
{
    return (wxUint8)Read32(base);
}

wxInt64 wxTextInputStream::Read64S(int base)
{
    wxASSERT_MSG( !base || (base > 1 && base <= 36), wxT("invalid base") );
    if(!m_input) return 0;

    wxString word = ReadWord();
    if(word.empty())
        return 0;

    wxInt64 res;
    if(!word.ToLongLong(&res, base))
        return 0;
    return res;
}

wxInt32 wxTextInputStream::Read32S(int base)
{
    wxASSERT_MSG( !base || (base > 1 && base <= 36), wxT("invalid base") );
    if(!m_input) return 0;

    wxString word = ReadWord();
    if(word.empty())
        return 0;
    return wxStrtol(word.c_str(), 0, base);
}

wxInt16 wxTextInputStream::Read16S(int base)
{
    return (wxInt16)Read32S(base);
}

wxInt8 wxTextInputStream::Read8S(int base)
{
    return (wxInt8)Read32S(base);
}

double wxTextInputStream::ReadDouble()
{
    if(!m_input) return 0;
    wxString word = ReadWord();
    if(word.empty())
        return 0;
    return wxStrtod(word.c_str(), 0);
}

wxString wxTextInputStream::ReadLine()
{
    wxString line;

    while ( !m_input.Eof() )
    {
        wxChar c = GetChar();
        if (!c)
            break;

        if (EatEOL(c))
            break;

        line += c;
    }

    return line;
}

wxString wxTextInputStream::ReadWord()
{
    wxString word;

    if ( !m_input )
        return word;

    wxChar c = NextNonSeparators();
    if ( !c )
        return word;

    word += c;

    while ( !m_input.Eof() )
    {
        c = GetChar();
        if (!c)
            break;

        if (m_separators.Find(c) >= 0)
            break;

        if (EatEOL(c))
            break;

        word += c;
    }

    return word;
}

wxTextInputStream& wxTextInputStream::operator>>(wxString& word)
{
    word = ReadWord();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(char& c)
{
    c = m_input.GetC();
    if(m_input.LastRead() <= 0) c = 0;

    if (EatEOL(c))
        c = '\n';

    return *this;
}

#if wxUSE_UNICODE && wxWCHAR_T_IS_REAL_TYPE

wxTextInputStream& wxTextInputStream::operator>>(wchar_t& wc)
{
    wc = GetChar();

    return *this;
}

#endif // wxUSE_UNICODE

wxTextInputStream& wxTextInputStream::operator>>(wxInt16& i)
{
    i = Read16S();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxInt32& i)
{
    i = Read32S();
    return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxInt64& i)
{
    i = Read64S();
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

wxTextInputStream& wxTextInputStream::operator>>(wxUint64& i)
{
    i = Read64();
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



#if wxUSE_UNICODE
wxTextOutputStream::wxTextOutputStream(wxOutputStream& s,
                                       wxEOL mode,
                                       const wxMBConv& conv)
  : m_output(s), m_conv(conv.Clone())
#else
wxTextOutputStream::wxTextOutputStream(wxOutputStream& s, wxEOL mode)
  : m_output(s)
#endif
{
    m_mode = mode;
    if (m_mode == wxEOL_NATIVE)
    {
#if defined(__WINDOWS__)
        m_mode = wxEOL_DOS;
#else
        m_mode = wxEOL_UNIX;
#endif
    }

#if wxUSE_UNICODE && SIZEOF_WCHAR_T == 2
    m_lastWChar = 0;
#endif // SIZEOF_WCHAR_T == 2
}

wxTextOutputStream::~wxTextOutputStream()
{
#if wxUSE_UNICODE
    delete m_conv;
#endif // wxUSE_UNICODE
}

void wxTextOutputStream::SetMode(wxEOL mode)
{
    m_mode = mode;
    if (m_mode == wxEOL_NATIVE)
    {
#if defined(__WINDOWS__)
        m_mode = wxEOL_DOS;
#else
        m_mode = wxEOL_UNIX;
#endif
    }
}

void wxTextOutputStream::Write64(wxUint64 i)
{
    WriteString(wxString::Format("%" wxLongLongFmtSpec "u", i));
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
    str.Printf(wxT("%u"), (unsigned)i);

    WriteString(str);
}

void wxTextOutputStream::Write8(wxUint8 i)
{
    wxString str;
    str.Printf(wxT("%u"), (unsigned)i);

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
    size_t len = string.length();

    wxString out;
    out.reserve(len);

    for ( size_t i = 0; i < len; i++ )
    {
        const wxChar c = string[i];
        if ( c == wxT('\n') )
        {
            switch ( m_mode )
            {
                case wxEOL_DOS:
                    out << wxT("\r\n");
                    continue;

                case wxEOL_MAC:
                    out << wxT('\r');
                    continue;

                default:
                    wxFAIL_MSG( wxT("unknown EOL mode in wxTextOutputStream") );
                    wxFALLTHROUGH;

                case wxEOL_UNIX:
                    // don't treat '\n' specially
                    ;
            }
        }

        out << c;
    }

#if wxUSE_UNICODE
    // FIXME-UTF8: use wxCharBufferWithLength if/when we have it
    wxCharBuffer buffer = m_conv->cWC2MB(out.wc_str(), out.length(), &len);
    m_output.Write(buffer, len);
#else
    m_output.Write(out.c_str(), out.length() );
#endif
}

wxTextOutputStream& wxTextOutputStream::PutChar(wxChar c)
{
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
    wxCharBuffer buffer;
    size_t len;
    if ( m_lastWChar )
    {
        wxChar buf[2];
        buf[0] = m_lastWChar;
        buf[1] = c;
        buffer = m_conv->cWC2MB(buf, WXSIZEOF(buf), &len);
        m_lastWChar = 0;
    }
    else
    {
        buffer = m_conv->cWC2MB(&c, 1, &len);
    }

    if ( !len )
    {
        // Conversion failed, possibly because we have the first half of a
        // surrogate character, so just store it and write it out when the
        // second half is written to the stream too later.
        //
        // Notice that if we already had had a valid m_lastWChar, it is simply
        // discarded here which is very bad, but there is no way to signal an
        // error from here and this is not worse than the old code behaviour.
        m_lastWChar = c;
    }
    else
    {
        for ( size_t n = 0; n < len; n++ )
        {
            const char c2 = buffer[n];
            if ( c2 == '\n' )
            {
                switch ( m_mode )
                {
                    case wxEOL_DOS:
                        m_output.Write("\r\n", 2);
                        continue;

                    case wxEOL_MAC:
                        m_output.Write("\r", 1);
                        continue;

                    default:
                        wxFAIL_MSG( wxT("unknown EOL mode in wxTextOutputStream") );
                        wxFALLTHROUGH;

                    case wxEOL_UNIX:
                        // don't treat '\n' specially
                        ;
                }
            }

            m_output.Write(&c2, 1);
        }
    }
#else // SIZEOF_WCHAR_T == 4
    WriteString( wxString(&c, *m_conv, 1) );
#endif // SIZEOF_WCHAR_T == 2 or 4
#else
    WriteString( wxString(&c, wxConvLocal, 1) );
#endif
    return *this;
}

void wxTextOutputStream::Flush()
{
#if wxUSE_UNICODE
    const size_t len = m_conv->FromWChar(NULL, 0, L"", 1);
    if ( len > m_conv->GetMBNulLen() )
    {
        wxCharBuffer buf(len);
        m_conv->FromWChar(buf.data(), len, L"", 1);
        m_output.Write(buf, len - m_conv->GetMBNulLen());
    }
#endif // wxUSE_UNICODE
}

wxTextOutputStream& wxTextOutputStream::operator<<(const wxString& string)
{
    WriteString( string );
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(char c)
{
    WriteString( wxString::FromAscii(c) );

    return *this;
}

#if wxUSE_UNICODE && wxWCHAR_T_IS_REAL_TYPE

wxTextOutputStream& wxTextOutputStream::operator<<(wchar_t wc)
{
    PutChar(wc);

    return *this;
}

#endif // wxUSE_UNICODE

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt16 c)
{
    Write(c);

    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt32 c)
{
    Write(c);

    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt64 c)
{
    Write(c);

    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint16 c)
{
    Write(c);

    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint32 c)
{
    Write(c);

    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint64 c)
{
    Write(c);

    return *this;
}

wxTextOutputStream &wxTextOutputStream::operator<<(double f)
{
    Write(f);
    return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(float f)
{
    Write(f);
    return *this;
}

wxTextOutputStream &endl( wxTextOutputStream &stream )
{
    return stream.PutChar(wxT('\n'));
}

#endif
  // wxUSE_STREAMS
