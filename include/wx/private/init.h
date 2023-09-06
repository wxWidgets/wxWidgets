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
    wxInitData() = default;

    // Get the single global object.
    static wxInitData& Get();

    // Initialize from ANSI command line arguments.
    void Initialize(int argc, char** argv);

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
#else // !__WINDOWS__
    // Under other platforms we typically need the original, non-Unicode
    // command line version, so we keep it too. Unlike argv that we allocate,
    // this pointer doesn't need to be freed.
    char** argvA = nullptr;
#endif // __WINDOWS__

    wxDECLARE_NO_COPY_CLASS(wxInitData);
};

#endif // _WX_PRIVATE_INIT_H_
