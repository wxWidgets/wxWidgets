/*
 * Name:        wx/chkconf.h
 * Purpose:     check the config settings for consistency
 * Author:      Vadim Zeitlin
 * Modified by:
 * Created:     09.08.00
 * RCS-ID:      $Id$
 * Copyright:   (c) 2000 Vadim Zeitlin <vadim@wxwindows.org>
 * Licence:     wxWindows license
 */

/*
   this global setting determintes what should we do if the setting FOO
   requires BAR and BAR is not set: we can either silently define BAR
   (default, recommended) or give an error and abort (mainly useful for
   developpers only)
 */
#define wxABORT_ON_CONFIG_ERROR

#if wxUSE_BUTTON || \
    wxUSE_CARET || \
    wxUSE_COMBOBOX || \
    wxUSE_BMPBUTTON || \
    wxUSE_CHECKBOX || \
    wxUSE_CHECKLISTBOX || \
    wxUSE_CHOICE || \
    wxUSE_GAUGE || \
    wxUSE_GRID || \
    wxUSE_NEW_GRID || \
    wxUSE_IMAGLIST || \
    wxUSE_LISTBOX || \
    wxUSE_LISTCTRL || \
    wxUSE_NOTEBOOK || \
    wxUSE_RADIOBOX || \
    wxUSE_RADIOBTN || \
    wxUSE_SASH || \
    wxUSE_SCROLLBAR || \
    wxUSE_SLIDER || \
    wxUSE_SPINBTN || \
    wxUSE_SPINCTRL || \
    wxUSE_SPLITTER || \
    wxUSE_STATBMP || \
    wxUSE_STATBOX || \
    wxUSE_STATLINE || \
    wxUSE_STATUSBAR || \
    wxUSE_TAB_DIALOG || \
    wxUSE_TOOLBAR || \
    wxUSE_TOOLTIPS || \
    wxUSE_TREECTRL
#    if !wxUSE_CONTROLS
#        ifdef wxABORT_ON_CONFIG_ERROR
#            error "wxUSE_CONTROLS unset but some controls used"
#        else
#            undef wxUSE_CONTROLS
#            define wxUSE_CONTROLS 1
#        endif
#    endif
#endif /* controls */

#if wxUSE_RADIOBTN
#   if defined(__WXUNIVERSAL__) && !wxUSE_CHECKBOX
#        ifdef wxABORT_ON_CONFIG_ERROR
#            error "wxUSE_RADIOBTN requires wxUSE_CHECKBOX in wxUniversal"
#        else
#            undef wxUSE_CHECKBOX
#            define wxUSE_CHECKBOX 1
#        endif
#   endif
#endif /* wxUSE_RADIOBTN */

#if wxUSE_STOPWATCH
#    if !wxUSE_LONGLONG
#        ifdef wxABORT_ON_CONFIG_ERROR
#            error "wxUSE_STOPWATCH requires wxUSE_LONGLONG"
#        else
#            undef wxUSE_LONGLONG
#            define wxUSE_LONGLONG 1
#        endif
#    endif
#endif /* wxUSE_STOPWATCH */

#if !defined(wxUSE_DYNAMIC_CLASSES) || !wxUSE_DYNAMIC_CLASSES
#   if wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_DYNAMIC_CLASSES must be defined as 1"
#   else
#       undef wxUSE_DYNAMIC_CLASSES
#       define wxUSE_DYNAMIC_CLASSES
#   endif
#endif /* wxUSE_DYNAMIC_CLASSES */

#if wxUSE_MIMETYPE && !wxUSE_TEXTFILE
#   if wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_MIMETYPE requires wxUSE_TEXTFILE"
#   else
#       undef wxUSE_TEXTFILE
#       define wxUSE_TEXTFILE 1
#   endif
#endif /* wxUSE_MIMETYPE */

#if wxUSE_TEXTFILE && !wxUSE_FILE
#   if wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_TEXTFILE requires wxUSE_FILE"
#   else
#       undef wxUSE_FILE
#       define wxUSE_FILE 1
#   endif
#endif /* wxUSE_TEXTFILE */
