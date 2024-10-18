/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/longlong.cpp
// Purpose:     implementation of wxLongLong
// Author:      Jeffrey C. Ollie <jeff@ollie.clive.ia.us>, Vadim Zeitlin
// Created:     10.02.99
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

#include "wx/wxprec.h"

#include "wx/longlong.h"

#if wxUSE_STREAMS
    #include "wx/txtstrm.h"
#endif

#include "wx/ioswrap.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void *wxLongLong::asArray() const
{
    static unsigned char temp[8];

    temp[0] = wx_truncate_cast(unsigned char, ((m_ll >> 56) & 0xFF));
    temp[1] = wx_truncate_cast(unsigned char, ((m_ll >> 48) & 0xFF));
    temp[2] = wx_truncate_cast(unsigned char, ((m_ll >> 40) & 0xFF));
    temp[3] = wx_truncate_cast(unsigned char, ((m_ll >> 32) & 0xFF));
    temp[4] = wx_truncate_cast(unsigned char, ((m_ll >> 24) & 0xFF));
    temp[5] = wx_truncate_cast(unsigned char, ((m_ll >> 16) & 0xFF));
    temp[6] = wx_truncate_cast(unsigned char, ((m_ll >> 8)  & 0xFF));
    temp[7] = wx_truncate_cast(unsigned char, ((m_ll >> 0)  & 0xFF));

    return temp;
}

void *wxULongLong::asArray() const
{
    static unsigned char temp[8];

    temp[0] = wx_truncate_cast(unsigned char, ((m_ll >> 56) & 0xFF));
    temp[1] = wx_truncate_cast(unsigned char, ((m_ll >> 48) & 0xFF));
    temp[2] = wx_truncate_cast(unsigned char, ((m_ll >> 40) & 0xFF));
    temp[3] = wx_truncate_cast(unsigned char, ((m_ll >> 32) & 0xFF));
    temp[4] = wx_truncate_cast(unsigned char, ((m_ll >> 24) & 0xFF));
    temp[5] = wx_truncate_cast(unsigned char, ((m_ll >> 16) & 0xFF));
    temp[6] = wx_truncate_cast(unsigned char, ((m_ll >> 8)  & 0xFF));
    temp[7] = wx_truncate_cast(unsigned char, ((m_ll >> 0)  & 0xFF));

    return temp;
}

#if wxUSE_STD_IOSTREAM

// input/output
WXDLLIMPEXP_BASE
std::ostream& operator<< (std::ostream& o, const wxLongLong& ll)
{
    return o << ll.ToString();
}

WXDLLIMPEXP_BASE
std::ostream& operator<< (std::ostream& o, const wxULongLong& ll)
{
    return o << ll.ToString();
}

#endif // wxUSE_STD_IOSTREAM

WXDLLIMPEXP_BASE wxString& operator<< (wxString& s, const wxLongLong& ll)
{
    return s << ll.ToString();
}

WXDLLIMPEXP_BASE wxString& operator<< (wxString& s, const wxULongLong& ll)
{
    return s << ll.ToString();
}

#if wxUSE_STREAMS

WXDLLIMPEXP_BASE wxTextOutputStream& operator<< (wxTextOutputStream& o, const wxULongLong& ll)
{
    return o << ll.ToString();
}

WXDLLIMPEXP_BASE wxTextOutputStream& operator<< (wxTextOutputStream& o, const wxLongLong& ll)
{
    return o << ll.ToString();
}

#define READ_STRING_CHAR(s, idx, len) ((idx!=len) ? (wxChar)s[idx++] : wxT('\0'))

WXDLLIMPEXP_BASE class wxTextInputStream &operator>>(class wxTextInputStream &o, wxULongLong &ll)
{
    wxString s = o.ReadWord();

    ll = wxULongLong(0l, 0l);
    size_t length = s.length();
    size_t idx = 0;

    wxChar ch = READ_STRING_CHAR(s, idx, length);

    // Skip WS
    while (ch==wxT(' ') || ch==wxT('\t'))
        ch = READ_STRING_CHAR(s, idx, length);

    // Read number
    wxULongLong multiplier(0l, 10l);
    while (ch>=wxT('0') && ch<=wxT('9')) {
        long lValue = (unsigned) (ch - wxT('0'));
        ll = ll * multiplier + wxULongLong(0l, lValue);
        ch = READ_STRING_CHAR(s, idx, length);
    }

    return o;
}

WXDLLIMPEXP_BASE class wxTextInputStream &operator>>(class wxTextInputStream &o, wxLongLong &ll)
{
    wxString s = o.ReadWord();

    ll = wxLongLong(0l, 0l);
    size_t length = s.length();
    size_t idx = 0;

    wxChar ch = READ_STRING_CHAR(s, idx, length);

    // Skip WS
    while (ch==wxT(' ') || ch==wxT('\t'))
        ch = READ_STRING_CHAR(s, idx, length);

    // Ask for sign
    int iSign = 1;
    if (ch==wxT('-') || ch==wxT('+')) {
        iSign = ((ch==wxT('-')) ? -1 : 1);
        ch = READ_STRING_CHAR(s, idx, length);
    }

    // Read number
    wxLongLong multiplier(0l, 10l);
    while (ch>=wxT('0') && ch<=wxT('9')) {
        long lValue = (unsigned) (ch - wxT('0'));
        ll = ll * multiplier + wxLongLong(0l, lValue);
        ch = READ_STRING_CHAR(s, idx, length);
    }

    ll = ll * wxLongLong((wxLongLong_t) iSign);

    return o;
}

WXDLLIMPEXP_BASE class wxTextOutputStream &operator<<(class wxTextOutputStream &o, wxULongLong_t value)
{
    return o << wxULongLong(value).ToString();
}

WXDLLIMPEXP_BASE class wxTextOutputStream &operator<<(class wxTextOutputStream &o, wxLongLong_t value)
{
    return o << wxLongLong(value).ToString();
}

WXDLLIMPEXP_BASE class wxTextInputStream &operator>>(class wxTextInputStream &o, wxULongLong_t &value)
{
    wxULongLong ll;
    o >> ll;
    value = ll.GetValue();
    return o;
}

WXDLLIMPEXP_BASE class wxTextInputStream &operator>>(class wxTextInputStream &o, wxLongLong_t &value)
{
    wxLongLong ll;
    o >> ll;
    value = ll.GetValue();
    return o;
}

#endif // wxUSE_STREAMS
