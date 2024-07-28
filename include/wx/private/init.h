///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/init.h
// Purpose:     Private initialization-related data.
// Author:      Vadim Zeitlin
// Created:     2023-09-02
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_INIT_H_
#define _WX_PRIVATE_INIT_H_

// ----------------------------------------------------------------------------
// Initialization data contains parameters we get from the OS entry function.
// ----------------------------------------------------------------------------

struct WXDLLIMPEXP_BASE wxInitData
{
private:
    wxInitData() = default;

public:
    // Get the single global object.
    static wxInitData& Get();

    // Initialize from ANSI command line arguments: argv contents should be
    // static, i.e. remain valid until the end of the program.
    void Initialize(int argc, char** argv);

    // Initialize from wide command line arguments if we hadn't been
    // initialized in some other way: this allows to call this function
    // unconditionally, even when these wide arguments were themselves
    // synthesized from ANSI ones by our own code.
    //
    // Note that here we currently make a copy of the arguments internally, so
    // they don't need to be static.
    void InitIfNecessary(int argc, wchar_t** argv);

    // This function is used instead of the dtor because the global object can
    // be initialized multiple times.
    void Free();


    // We always have argc and (Unicode) argv, they're filled by Initialize()
    // and argv as well as its elements are owned by us, see Free().
    int argc = 0;
    wchar_t** argv = nullptr;

#ifdef __WINDOWS__
    // Initialize from the implicitly available Unicode command line.
    void MSWInitialize();

    // This pointer is non-null only if MSWInitialize() was called. In this
    // case, argv is also set to it and, because this pointer needs to be freed
    // using MSW-specific function, argv must not be freed at all.
    //
    // It's also possible to use Initialize(), even under Windows, in which
    // case this pointer remains null and argv must be freed as usual.
    wchar_t** argvMSW = nullptr;
#endif // __WINDOWS__

    // wxMSW traditionally doesn't use narrow command line arguments, but the
    // other ports do need them, even under Windows, so we keep them here.
#ifndef __WXMSW__
    // Initializes argvA using argc and argv. This means that argc and argv
    // MUST be initialized before calling this function.
    void InitArgvA();

    // This pointer may or not need to be freed, as indicated by ownsArgvA flag.
    char** argvA = nullptr;
    bool ownsArgvA = false;
#endif // !__WXMSW__

    wxDECLARE_NO_COPY_CLASS(wxInitData);
};

// Type of the hook function, see wxAddEntryHook(). If this function returns
// a value different from -1, the process exits using it as error code.
using wxEntryHook = int (*)();

// Set a special hook function which will be called before performing any
// normal initialization. Note that this hook can't use any wxWidgets
// functionality because nothing has been initialized yet, but can use
// wxInitData to examine the command line arguments and determine if it should
// be applied.
//
// This is currently used only by wxWebViewChromium to allow running Chromium
// helper applications without initializing GTK under Linux but could, in
// principle, be used for any other similar purpose.
WXDLLIMPEXP_BASE void wxAddEntryHook(wxEntryHook hook);

#endif // _WX_PRIVATE_INIT_H_
