///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/backend.h
// Author:      Paul Cornett
// Copyright:   (c) 2022 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __WXGTK3__
namespace wxGTKImpl
{
    bool IsWayland(void* instance);
    bool IsX11(void* instance);
}
#endif
