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
// If the input is not quoted string it will be returned as the input
wxString DecodeString(const wxString& str)
{
    if (!str.starts_with('"') || !str.ends_with('"'))
        return str;

    wxString result;
    const wxWCharBuffer buf = str.wc_str();
    const wchar_t* end = buf.data() + buf.length() - 1;
    for (const wchar_t* ch = buf.data() + 1; ch < end; ++ch)
    {
        if (*ch == '\\')
        {
            switch (*(++ch))
            {
            case 'b':
                result.append('\b');
                break;
            case 'n':
                result.append('\n');
                break;
            case 'r':
                result.append('\r');
                break;
            case 't':
                result.append('\t');
                break;
            case 'v':
                result.append('\v');
                break;
            case '\'':
                result.append('\'');
                break;
            case '"':
                result.append('"');
                break;
            case '\\':
                result.append('\\');
                break;
            case 'u':
                if (wxIsxdigit(ch[1]) && wxIsxdigit(ch[2]) &&
                    wxIsxdigit(ch[3]) && wxIsxdigit(ch[4]))
                {
                    wxUChar uchar = wxHexToDec(wxString(&ch[3], 2)) |
                        wxHexToDec(wxString(&ch[1], 2)) >> 8;
                    result.append(uchar);
                    ch += 4;
                }
                break;
            case 'x':
                if (wxIsxdigit(ch[1]) && wxIsxdigit(ch[2]))
                {
                    wxChar hchar = wxHexToDec(wxString(&ch[1], 2));
                    result.append(hchar);
                    ch += 2;
                }
                break;

            default:
                result.append(*ch);
                break;
            }
        }
        else
            result.append(*ch);
    }

    return result;
}

} // namespace JSON

#endif // _WX_PRIVATE_JSON_H_
