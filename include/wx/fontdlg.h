#ifndef _WX_FONTDLG_H_BASE_
#define _WX_FONTDLG_H_BASE_

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
#elif defined(__WXMAC__)
#include "wx/generic/fontdlgg.h"
#   define wxFontDialog wxGenericFontDialog
#   define classwxFontDialog classwxGenericFontDialog
#elif defined(__WXSTUBS__)
#include "wx/generic/fontdlgg.h"
#   define wxFontDialog wxGenericFontDialog
#   define classwxFontDialog classwxGenericFontDialog
#endif

#endif
    // _WX_FONTDLG_H_BASE_
