///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/txtstrm.cpp
// Purpose:     Text stream classes
// Author:      Guilhem Lavaux
// Created:     28/06/98
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STREAMS

#include "wx/txtstrm.h"

#ifndef WX_PRECOMP
    #include "wx/crt.h"
#endif

#include <ctype.h>

// ----------------------------------------------------------------------------
// wxTextInputStream
// ----------------------------------------------------------------------------

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

wxTextInputStream::~wxTextInputStream()
{
    delete m_conv;
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

    // We may need to decode up to 6 characters if we have input starting with
    // 2 null bytes (like in UTF-32BE BOM), and then 3 bytes that look like
    // the start of UTF-8 sequence, as decoding it will only succeed when
    // 6 bytes are read -- and will yield 6 wide characters.
    wxChar wbuf[6];
    for(size_t inlen = 0; inlen < sizeof(m_lastBytes); inlen++)
    {
        if ( inlen >= m_validEnd )
        {
            // actually read the next character
            m_lastBytes[inlen] = m_input.GetC();

            if (m_input.LastRead() == 0)
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
                // but now finally has enough data to see that there is none, or
                // it was trying to decode the data as UTF-8 sequence, but now
                // recognized that it's not valid UTF-8 and switched to fallback.
                // We don't know how long is the first character or if it's decoded
                // as 1 or 2 wchar_t characters, so we need to start with 1 byte again.
                inlen = static_cast<size_t>(-1);
                break;

#if SIZEOF_WCHAR_T == 2
            case 2:
                // When wchar_t uses UTF-16, we could have decoded a single
                // Unicode code point as 2 wchar_t characters and there is
                // nothing else to do here but to return the first one now and
                // remember the second one for the next call, as there is no
                // way to fit both of them into a single wxChar in this case.
                m_lastWChar = wbuf[1];
                wxFALLTHROUGH;
#endif // SIZEOF_WCHAR_T == 2

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

    for ( ;; )
    {
        wxChar c = GetChar();
        if ( m_input.Eof() )
            break;

        if (!c)
        {
            // If we failed to get a character and the stream is not at EOF, it
            // can only mean that decoding the stream contents using our
            // conversion object failed. In this case, we must signal an error
            // at the stream level, as otherwise the code using this function
            // would never know that something went wrong and would continue
            // calling it again and again, resulting in an infinite loop.
            m_input.Reset(wxSTREAM_READ_ERROR);
            break;
        }

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
    if (m_input.LastRead() == 0) c = 0;

    if (EatEOL(c))
        c = '\n';

    return *this;
}

#if wxWCHAR_T_IS_REAL_TYPE

wxTextInputStream& wxTextInputStream::operator>>(wchar_t& wc)
{
    wc = GetChar();

    return *this;
}

#endif // wxWCHAR_T_IS_REAL_TYPE

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



wxTextOutputStream::wxTextOutputStream(wxOutputStream& s,
                                       wxEOL mode,
                                       const wxMBConv& conv)
  : m_output(s), m_conv(conv.Clone())
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

#if SIZEOF_WCHAR_T == 2
    m_lastWChar = 0;
#endif // SIZEOF_WCHAR_T == 2
}

wxTextOutputStream::~wxTextOutputStream()
{
    delete m_conv;
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

    // Note that we have to use the length returned by cWC2MB() here to handle
    // string with the embedded NULs correctly.
    wxCharBuffer buffer = m_conv->cWC2MB(out.wc_str(), out.length(), &len);
    m_output.Write(buffer, len);
}

wxTextOutputStream& wxTextOutputStream::PutChar(wxChar c)
{
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
    WriteString( wxString(&c, 1) );
#endif // SIZEOF_WCHAR_T == 2 or 4
    return *this;
}

void wxTextOutputStream::Flush()
{
    const size_t len = m_conv->FromWChar(nullptr, 0, L"", 1);
    if ( len > m_conv->GetMBNulLen() )
    {
        wxCharBuffer buf(len);
        m_conv->FromWChar(buf.data(), len, L"", 1);
        m_output.Write(buf, len - m_conv->GetMBNulLen());
    }
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

#if wxWCHAR_T_IS_REAL_TYPE

wxTextOutputStream& wxTextOutputStream::operator<<(wchar_t wc)
{
    PutChar(wc);

    return *this;
}

#endif // wxWCHAR_T_IS_REAL_TYPE

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
