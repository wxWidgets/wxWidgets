#ifndef __FONTDLGH_BASE__
#define __FONTDLGH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/fontdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/fontdlgg.h"
#   define wxFontDialog wxGenericFontDialog
#   define classwxFontDialog classwxGenericFontDialog
#elif defined(__WXGTK__)
#include "wx/generic/fontdlgg.h"
#   define wxFontDialog wxGenericFontDialog
#   define classwxFontDialog classwxGenericFontDialog
#elif defined(__WXQT__)
#include "wx/generic/fontdlgg.h"
#   define wxFontDialog wxGenericFontDialog
#   define classwxFontDialog classwxGenericFontDialog
#endif

#endif
    // __FONTDLGH_BASE__
