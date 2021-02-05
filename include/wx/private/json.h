///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/json.h
// Purpose:     Helper functions to handle JSON data
// Author:      Tobias Taschner
// Created:     2020-01-17
// Copyright:   (c) 2020 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_JSON_H_
#define _WX_PRIVATE_JSON_H_

namespace wxJSON
{

// Decode a string literal including escape sequences
// Returns false if the input string is not a valid JSON string
bool DecodeString(const wxString& in, wxString* out)
{
    const wxWCharBuffer buf = in.wc_str();
    const wchar_t* ch = buf.data();
    // String has to chart with a quote
    if (*(ch++) != '"')
        return false;
    out->clear();
    out->reserve(buf.length());
    const wchar_t* end = buf.data() + buf.length() - 1;
    for (; ch < end; ++ch)
    {
        if (*ch == '\\')
        {
            switch (*(++ch))
            {
            case 'b':
                out->append('\b');
                break;
            case 'n':
                out->append('\n');
                break;
            case 'r':
                out->append('\r');
                break;
            case 't':
                out->append('\t');
                break;
            case 'f':
                out->append('\f');
                break;
            case '/':
                out->append('/');
                break;
            case '"':
                out->append('"');
                break;
            case '\\':
                out->append('\\');
                break;
            case 'u':
#if SIZEOF_WCHAR_T == 2
                            // In this case, we handle surrogates without doing anything special was wchar_t strings use UTF-17 encoding.
                if (wxIsxdigit(ch[1]) && wxIsxdigit(ch[2]) &&
                    wxIsxdigit(ch[3]) && wxIsxdigit(ch[4]))
                {
                    wchar_t uchar = wxHexToDec(wxString(&ch[3], 2)) |
                        wxHexToDec(wxString(&ch[1], 2)) >> 8;
                    out->append(uchar);
                    ch += 4;
                }
#else
    #error Implement correct surrogate handling.
#endif
                break;
            default:
                return false;
                break;
            }
        }
        else
            out->append(*ch);
    }

    // String has to end with a quote
    return (*ch) == '"';
}

} // namespace JSON

#endif // _WX_PRIVATE_JSON_H_
