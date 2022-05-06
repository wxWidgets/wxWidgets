///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/accelcmn.cpp
// Purpose:     implementation of platform-independent wxAcceleratorEntry parts
// Author:      Vadim Zeitlin
// Created:     2007-05-05
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_ACCEL

#ifndef WX_PRECOMP
    #include "wx/accel.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/crt.h"
#endif //WX_PRECOMP

wxAcceleratorTable wxNullAcceleratorTable;

// ============================================================================
// wxAcceleratorEntry implementation
// ============================================================================

wxGCC_WARNING_SUPPRESS(missing-field-initializers)

static const struct wxKeyName
{
    wxKeyCode code;
    const char *name;
    const char *display_name;
} wxKeyNames[] =
{
    { WXK_DELETE,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Delete") },
    { WXK_DELETE,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Del") },
    { WXK_BACK,             wxTRANSLATE_IN_CONTEXT("keyboard key", "Back"),          wxTRANSLATE_IN_CONTEXT("keyboard key", "Backspace") },
    { WXK_INSERT,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Insert") },
    { WXK_INSERT,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Ins") },
    { WXK_RETURN,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Enter") },
    { WXK_RETURN,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Return") },
    { WXK_PAGEUP,           wxTRANSLATE_IN_CONTEXT("keyboard key", "PageUp"),        wxTRANSLATE_IN_CONTEXT("keyboard key", "Page Up") },
    { WXK_PAGEDOWN,         wxTRANSLATE_IN_CONTEXT("keyboard key", "PageDown"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Page Down") },
    { WXK_PAGEUP,           wxTRANSLATE_IN_CONTEXT("keyboard key", "PgUp") },
    { WXK_PAGEDOWN,         wxTRANSLATE_IN_CONTEXT("keyboard key", "PgDn") },
    { WXK_LEFT,             wxTRANSLATE_IN_CONTEXT("keyboard key", "Left"),          wxTRANSLATE_IN_CONTEXT("keyboard key", "Left") },
    { WXK_RIGHT,            wxTRANSLATE_IN_CONTEXT("keyboard key", "Right"),         wxTRANSLATE_IN_CONTEXT("keyboard key", "Right") },
    { WXK_UP,               wxTRANSLATE_IN_CONTEXT("keyboard key", "Up"),            wxTRANSLATE_IN_CONTEXT("keyboard key", "Up") },
    { WXK_DOWN,             wxTRANSLATE_IN_CONTEXT("keyboard key", "Down"),          wxTRANSLATE_IN_CONTEXT("keyboard key", "Down") },
    { WXK_HOME,             wxTRANSLATE_IN_CONTEXT("keyboard key", "Home") },
    { WXK_END,              wxTRANSLATE_IN_CONTEXT("keyboard key", "End") },
    { WXK_SPACE,            wxTRANSLATE_IN_CONTEXT("keyboard key", "Space") },
    { WXK_TAB,              wxTRANSLATE_IN_CONTEXT("keyboard key", "Tab") },
    { WXK_ESCAPE,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Esc") },
    { WXK_ESCAPE,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Escape") },
    { WXK_CANCEL,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Cancel") },
    { WXK_CLEAR,            wxTRANSLATE_IN_CONTEXT("keyboard key", "Clear") },
    { WXK_MENU,             wxTRANSLATE_IN_CONTEXT("keyboard key", "Menu") },
    { WXK_PAUSE,            wxTRANSLATE_IN_CONTEXT("keyboard key", "Pause") },
    { WXK_CAPITAL,          wxTRANSLATE_IN_CONTEXT("keyboard key", "Capital") },
    { WXK_SELECT,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Select") },
    { WXK_PRINT,            wxTRANSLATE_IN_CONTEXT("keyboard key", "Print") },
    { WXK_EXECUTE,          wxTRANSLATE_IN_CONTEXT("keyboard key", "Execute") },
    { WXK_SNAPSHOT,         wxTRANSLATE_IN_CONTEXT("keyboard key", "Snapshot") },
    { WXK_HELP,             wxTRANSLATE_IN_CONTEXT("keyboard key", "Help") },
    { WXK_ADD,              wxTRANSLATE_IN_CONTEXT("keyboard key", "Add") },
    { WXK_SEPARATOR,        wxTRANSLATE_IN_CONTEXT("keyboard key", "Separator") },
    { WXK_SUBTRACT,         wxTRANSLATE_IN_CONTEXT("keyboard key", "Subtract") },
    { WXK_DECIMAL,          wxTRANSLATE_IN_CONTEXT("keyboard key", "Decimal") },
    { WXK_MULTIPLY,         wxTRANSLATE_IN_CONTEXT("keyboard key", "Multiply") },
    { WXK_DIVIDE,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Divide") },
    { WXK_NUMLOCK,          wxTRANSLATE_IN_CONTEXT("keyboard key", "Num_lock"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Lock") },
    { WXK_SCROLL,           wxTRANSLATE_IN_CONTEXT("keyboard key", "Scroll_lock"),   wxTRANSLATE_IN_CONTEXT("keyboard key", "Scroll Lock") },
    { WXK_NUMPAD_SPACE,     wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Space"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Space") },
    { WXK_NUMPAD_TAB,       wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Tab"),        wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Tab") },
    { WXK_NUMPAD_ENTER,     wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Enter"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Enter") },
    { WXK_NUMPAD_HOME,      wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Home"),       wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Home") },
    { WXK_NUMPAD_LEFT,      wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Left"),       wxTRANSLATE_IN_CONTEXT("keyboard key", "Num left") },
    { WXK_NUMPAD_UP,        wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Up"),         wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Up") },
    { WXK_NUMPAD_RIGHT,     wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Right"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Right") },
    { WXK_NUMPAD_DOWN,      wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Down"),       wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Down") },
    { WXK_NUMPAD_PAGEUP,    wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_PageUp"),     wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Page Up") },
    { WXK_NUMPAD_PAGEDOWN,  wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_PageDown"),   wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Page Down") },
    { WXK_NUMPAD_PAGEUP,    wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Prior") },
    { WXK_NUMPAD_PAGEDOWN,  wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Next") },
    { WXK_NUMPAD_END,       wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_End"),        wxTRANSLATE_IN_CONTEXT("keyboard key", "Num End") },
    { WXK_NUMPAD_BEGIN,     wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Begin"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Begin") },
    { WXK_NUMPAD_INSERT,    wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Insert"),     wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Insert") },
    { WXK_NUMPAD_DELETE,    wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Delete"),     wxTRANSLATE_IN_CONTEXT("keyboard key", "Num Delete") },
    { WXK_NUMPAD_EQUAL,     wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Equal"),      wxTRANSLATE_IN_CONTEXT("keyboard key", "Num =") },
    { WXK_NUMPAD_MULTIPLY,  wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Multiply"),   wxTRANSLATE_IN_CONTEXT("keyboard key", "Num *") },
    { WXK_NUMPAD_ADD,       wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Add"),        wxTRANSLATE_IN_CONTEXT("keyboard key", "Num +") },
    { WXK_NUMPAD_SEPARATOR, wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Separator"),  wxTRANSLATE_IN_CONTEXT("keyboard key", "Num ,") },
    { WXK_NUMPAD_SUBTRACT,  wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Subtract"),   wxTRANSLATE_IN_CONTEXT("keyboard key", "Num -") },
    { WXK_NUMPAD_DECIMAL,   wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Decimal"),    wxTRANSLATE_IN_CONTEXT("keyboard key", "Num .") },
    { WXK_NUMPAD_DIVIDE,    wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_Divide"),     wxTRANSLATE_IN_CONTEXT("keyboard key", "Num /") },
    { WXK_WINDOWS_LEFT,     wxTRANSLATE_IN_CONTEXT("keyboard key", "Windows_Left") },
    { WXK_WINDOWS_RIGHT,    wxTRANSLATE_IN_CONTEXT("keyboard key", "Windows_Right") },
    { WXK_WINDOWS_MENU,     wxTRANSLATE_IN_CONTEXT("keyboard key", "Windows_Menu") },
    { WXK_COMMAND,          wxTRANSLATE_IN_CONTEXT("keyboard key", "Command") },
};

wxGCC_WARNING_RESTORE(missing-field-initializers)

// return true if the 2 strings refer to the same accel
//
// as accels can be either translated or not, check for both possibilities and
// also compare case-insensitively as the key names case doesn't count
static inline bool CompareAccelString(const wxString& str, const char *accel)
{
    return str.CmpNoCase(accel) == 0
#if wxUSE_INTL
            || str.CmpNoCase(wxGetTranslation(accel, wxString(), "keyboard key")) == 0
#endif
            ;
}

// return prefixCode+number if the string is of the form "<prefix><number>" and
// 0 if it isn't
//
// first and last parameter specify the valid domain for "number" part
static int IsNumberedAccelKey(const wxString& str,
                              const char *prefix,
                              wxKeyCode prefixCode,
                              unsigned first,
                              unsigned last)
{
    const size_t lenPrefix = wxStrlen(prefix);
    if ( !CompareAccelString(str.Left(lenPrefix), prefix) )
        return 0;

    unsigned long num;
    if ( !str.Mid(lenPrefix).ToULong(&num) )
        return 0;

    if ( num < first || num > last )
    {
        // this must be a mistake, chances that this is a valid name of another
        // key are vanishingly small
        wxLogDebug(wxT("Invalid key string \"%s\""), str);
        return 0;
    }

    return prefixCode + num - first;
}

/* static */
bool
wxAcceleratorEntry::ParseAccel(const wxString& text, int *flagsOut, int *keyOut)
{
    // the parser won't like trailing spaces
    wxString label = text;
    label.Trim(true);

    // For compatibility with the old wx versions which accepted (and actually
    // even required) a TAB character in the string passed to this function we
    // ignore anything up to the first TAB. Notice however that the correct
    // input consists of just the accelerator itself and nothing else, this is
    // done for compatibility and compatibility only.
    int posTab = label.Find(wxT('\t'));
    if ( posTab == wxNOT_FOUND )
        posTab = 0;
    else
        posTab++;

    // parse the accelerator string
    int accelFlags = wxACCEL_NORMAL;
    wxString current;
    for ( size_t n = (size_t)posTab; n < label.length(); n++ )
    {
        bool skip = false;
        if ( !skip && ( (label[n] == '+') || (label[n] == '-') ) )
        {
            if ( CompareAccelString(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "ctrl")) )
                accelFlags |= wxACCEL_CTRL;
            else if ( CompareAccelString(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "alt")) )
                accelFlags |= wxACCEL_ALT;
            else if ( CompareAccelString(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "shift")) )
                accelFlags |= wxACCEL_SHIFT;
            else if ( CompareAccelString(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "rawctrl")) )
                accelFlags |= wxACCEL_RAW_CTRL;
            else if ( CompareAccelString(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "num ")) )
            {
                // This isn't really a modifier, but is part of the name of keys
                // that have a =/- in them (e.g. num + and num -)
                // So we want to skip the processing if we see it
                skip = true;
                current += label[n];

                continue;
            }
            else // not a recognized modifier name
            {
                // we may have "Ctrl-+", for example, but we still want to
                // catch typos like "Crtl-A" so only give the warning if we
                // have something before the current '+' or '-', else take
                // it as a literal symbol
                if ( current.empty() )
                {
                    current += label[n];

                    // skip clearing it below
                    continue;
                }
                else
                {
                    wxLogDebug(wxT("Unknown accel modifier: '%s'"), current);
                }
            }

            current.clear();
        }
        else // not special character
        {
            // Preserve case of the key (see comment below)
            current += label[n];
        }
    }

    int keyCode;
    const size_t len = current.length();
    switch ( len )
    {
        case 0:
            wxLogDebug(wxT("No accel key found, accel string ignored."));
            return false;

        case 1:
            // it's just a letter
            keyCode = current[0U];

            // ...or maybe not. A translation may be single character too (e.g.
            // Chinese), but if it's a Latin character, that's unlikely
            if ( keyCode < 128 )
            {
                // if the key is used with any modifiers, make it an uppercase one
                // because Ctrl-A and Ctrl-a are the same; but keep it as is if it's
                // used alone as 'a' and 'A' are different
                if ( accelFlags != wxACCEL_NORMAL )
                    keyCode = wxToupper(keyCode);
                break;
            }
            wxFALLTHROUGH;

        default:
            keyCode = IsNumberedAccelKey(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "F"),
                                         WXK_F1, 1, 24);
            if ( !keyCode )
            {
                for ( size_t n = 0; n < WXSIZEOF(wxKeyNames); n++ )
                {
                    const wxKeyName& kn = wxKeyNames[n];
                    if ( CompareAccelString(current, kn.name)
                         || ( kn.display_name && CompareAccelString(current, kn.display_name) ) )
                    {
                        keyCode = kn.code;
                        break;
                    }
                }
            }

            if ( !keyCode )
                keyCode = IsNumberedAccelKey(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_F"),
                                             WXK_NUMPAD_F1, 1, 4);
            if ( !keyCode )
                keyCode = IsNumberedAccelKey(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_"),
                                             WXK_NUMPAD0, 0, 9);
            if ( !keyCode )
                keyCode = IsNumberedAccelKey(current, wxTRANSLATE_IN_CONTEXT("keyboard key", "SPECIAL"),
                                             WXK_SPECIAL1, 1, 20);

            if ( !keyCode )
            {
                wxLogDebug(wxT("Unrecognized accel key '%s', accel string ignored."), current);
                return false;
            }
    }


    wxASSERT_MSG( keyCode, wxT("logic error: should have key code here") );

    if ( flagsOut )
        *flagsOut = accelFlags;
    if ( keyOut )
        *keyOut = keyCode;

    return true;
}

/* static */
wxAcceleratorEntry *wxAcceleratorEntry::Create(const wxString& str)
{
    const wxString accelStr = str.AfterFirst('\t');
    if ( accelStr.empty() )
    {
        // It's ok to pass strings not containing any accelerators at all to
        // this function, wxMenuItem code does it and we should just return
        // NULL in this case.
        return NULL;
    }

    int flags,
        keyCode;
    if ( !ParseAccel(accelStr, &flags, &keyCode) )
        return NULL;

    return new wxAcceleratorEntry(flags, keyCode);
}

bool wxAcceleratorEntry::FromString(const wxString& str)
{
    return ParseAccel(str, &m_flags, &m_keyCode);
}

namespace
{

wxString PossiblyLocalize(const wxString& str, bool localize)
{
    return localize ? wxGetTranslation(str, wxString(), "keyboard key") : str;
}

}

wxString wxAcceleratorEntry::AsPossiblyLocalizedString(bool localized) const
{
    wxString text;

    int flags = GetFlags();
    if ( flags & wxACCEL_ALT )
        text += PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "Alt+"), localized);
    if ( flags & wxACCEL_CTRL )
        text += PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "Ctrl+"), localized);
    if ( flags & wxACCEL_SHIFT )
        text += PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "Shift+"), localized);
#if defined(__WXMAC__)
    if ( flags & wxACCEL_RAW_CTRL )
        text += PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "RawCtrl+"), localized);
#endif

    const int code = GetKeyCode();

    if ( code >= WXK_F1 && code <= WXK_F24 )
        text << PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "F"), localized)
             << code - WXK_F1 + 1;
    else if ( code >= WXK_NUMPAD_F1 && code <= WXK_NUMPAD_F4 )
        text << PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_F"), localized)
             << code - WXK_NUMPAD_F1 + 1;
    else if ( code >= WXK_NUMPAD0 && code <= WXK_NUMPAD9 )
        text << PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "KP_"), localized)
             << code - WXK_NUMPAD0;
    else if ( code >= WXK_SPECIAL1 && code <= WXK_SPECIAL20 )
        text << PossiblyLocalize(wxTRANSLATE_IN_CONTEXT("keyboard key", "SPECIAL"), localized)
             << code - WXK_SPECIAL1 + 1;
    else // check the named keys
    {
        size_t n;
        for ( n = 0; n < WXSIZEOF(wxKeyNames); n++ )
        {
            const wxKeyName& kn = wxKeyNames[n];
            if ( code == kn.code )
            {
                text << PossiblyLocalize(kn.display_name ? kn.display_name : kn.name, localized);
                break;
            }
        }

        if ( n == WXSIZEOF(wxKeyNames) )
        {
            // must be a simple key
            if (
#if !wxUSE_UNICODE
                 // we can't call wxIsalnum() for non-ASCII characters in ASCII
                 // build as they're only defined for the ASCII range (or EOF)
                 wxIsascii(code) &&
#endif // ANSI
                    wxIsprint(code) )
            {
                text << (wxChar)code;
            }
            else
            {
                wxFAIL_MSG( wxT("unknown keyboard accelerator code") );
            }
        }
    }

    return text;
}

wxAcceleratorEntry *wxGetAccelFromString(const wxString& label)
{
    return wxAcceleratorEntry::Create(label);
}

#endif // wxUSE_ACCEL



