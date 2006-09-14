///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/theme.h
// Purpose:     wxTheme class manages all configurable aspects of the
//              application including the look (wxRenderer), feel
//              (wxInputHandler) and the colours (wxColourScheme)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_THEME_H_
#define _WX_UNIV_THEME_H_

// ----------------------------------------------------------------------------
// wxTheme
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxArtProvider;
class WXDLLEXPORT wxColourScheme;
class WXDLLEXPORT wxInputConsumer;
class WXDLLEXPORT wxInputHandler;
class WXDLLEXPORT wxRenderer;
struct WXDLLEXPORT wxThemeInfo;

class WXDLLEXPORT wxTheme
{
public:
    // static methods
    // --------------

    // create the default theme
    static bool CreateDefault();

    // create the theme by name (will return NULL if not found)
    static wxTheme *Create(const wxString& name);

    // change the current scheme
    static wxTheme *Set(wxTheme *theme);

    // get the current theme (never NULL)
    static wxTheme *Get() { return ms_theme; }

    // the theme methods
    // -----------------

    // get the renderer implementing all the control-drawing operations in
    // this theme
    virtual wxRenderer *GetRenderer() = 0;

    // get the art provider to be used together with this theme
    virtual wxArtProvider *GetArtProvider() = 0;

    // get the input handler of the given type, forward to the standard one
    virtual wxInputHandler *GetInputHandler(const wxString& handlerType,
                                            wxInputConsumer *consumer) = 0;

    // get the colour scheme for the control with this name
    virtual wxColourScheme *GetColourScheme() = 0;

    // implementation only from now on
    // -------------------------------

    virtual ~wxTheme();

private:
    // the list of descriptions of all known themes
    static wxThemeInfo *ms_allThemes;

    // the current theme
    static wxTheme *ms_theme;
    friend struct WXDLLEXPORT wxThemeInfo;
};

// ----------------------------------------------------------------------------
// dynamic theme creation helpers
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxThemeInfo
{
    typedef wxTheme *(*Constructor)();

    // theme name and (user readable) description
    wxString name, desc;

    // the function to create a theme object
    Constructor ctor;

    // next node in the linked list or NULL
    wxThemeInfo *next;

    // constructor for the struct itself
    wxThemeInfo(Constructor ctor, const wxChar *name, const wxChar *desc);
};

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// to use a standard theme insert this macro into one of the application files:
// without it, an over optimizing linker may discard the object module
// containing the theme implementation entirely
#define WX_USE_THEME(themename)                                             \
    extern WXDLLEXPORT_DATA(bool) wxThemeUse##themename;                    \
    static struct wxThemeUserFor##themename                                 \
    {                                                                       \
        wxThemeUserFor##themename() { wxThemeUse##themename = true; }       \
    } wxThemeDoUse##themename

// to declare a new theme, this macro must be used in the class declaration
#define WX_DECLARE_THEME(themename)                                         \
    private:                                                                \
        static wxThemeInfo ms_info##themename;                              \
    public:                                                                 \
        const wxThemeInfo *GetThemeInfo() const                             \
            { return &ms_info##themename; }

// and this one must be inserted in the source file
#define WX_IMPLEMENT_THEME(classname, themename, themedesc)                 \
    WXDLLEXPORT_DATA(bool) wxThemeUse##themename = true;                    \
    wxTheme *wxCtorFor##themename() { return new classname; }               \
    wxThemeInfo classname::ms_info##themename(wxCtorFor##themename,         \
                                              wxT( #themename ), themedesc)

#endif // _WX_UNIV_THEME_H_

