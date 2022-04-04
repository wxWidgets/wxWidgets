///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/backend.h
// Author:      Paul Cornett
// Copyright:   (c) 2022 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __WXGTK3__
namespace wxGTKImpl
{
    WXDLLIMPEXP_CORE bool IsWayland(void* instance);
    WXDLLIMPEXP_CORE bool IsX11(void* instance);
}
#endif
