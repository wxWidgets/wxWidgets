/*
 * Name:        wx/osx/iphone/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Stefan Csomor
 * Modified by:
 * Created:     2008-07-30
 * RCS-ID:      $Id: chkconf.h 54833 2008-07-29 21:03:18Z SC $
 * Copyright:   (c) Stefan Csomor
 * Licence:     wxWindows licence
 */

#ifndef _WX_OSX_IPHONE_CHKCONF_H_
#define _WX_OSX_IPHONE_CHKCONF_H_

/*
 * text rendering system
 */

/* we have different options and we turn on all that make sense
 * under a certain platform
 */

#define wxOSX_USE_CORE_TEXT 0
#define wxOSX_USE_ATSU_TEXT 0

/*
 * turning off capabilities that don't work under iphone yet
 */

#if wxUSE_MDI
#undef wxUSE_MDI
#define wxUSE_MDI 0
#endif

#if wxUSE_MDI_ARCHITECTURE
#undef wxUSE_MDI_ARCHITECTURE
#define wxUSE_MDI_ARCHITECTURE 0
#endif

#if wxUSE_DRAG_AND_DROP
#undef wxUSE_DRAG_AND_DROP
#define wxUSE_DRAG_AND_DROP 0
#endif

#if wxUSE_TASKBARICON
#undef wxUSE_TASKBARICON
#define wxUSE_TASKBARICON 0
#endif

#if wxUSE_TOOLTIPS
#undef wxUSE_TOOLTIPS
#define wxUSE_TOOLTIPS 0
#endif

#if wxUSE_DATAVIEWCTRL
#undef wxUSE_DATAVIEWCTRL
#define wxUSE_DATAVIEWCTRL 0
#endif

#if wxUSE_DRAG_AND_DROP
#undef wxUSE_DRAG_AND_DROP
#define wxUSE_DRAG_AND_DROP 0
#endif

#if wxUSE_TASKBARICON
#undef wxUSE_TASKBARICON
#define wxUSE_TASKBARICON 0
#endif

#define wxUSE_BUTTON 1
#define wxUSE_CARET 1
#define wxUSE_CHOICE 1
#define wxUSE_SCROLLBAR 1
#define wxUSE_STATUSBAR 1
#define wxUSE_NATIVE_STATUSBAR 0
#define wxUSE_TEXTCTRL 1
#define wxUSE_ABOUTDLG 1
#define wxUSE_STATTEXT 1
#define wxUSE_STATLINE 1
#define wxUSE_COLLPANE 1
#define wxUSE_STATBMP 1
#define wxUSE_STATBOX 1
#define wxUSE_CHECKBOX 1
#define wxUSE_RADIOBTN 1
#define wxUSE_RADIOBOX 1
#define wxUSE_TOGGLEBTN 1

#define wxUSE_TOOLBAR 0
#define wxUSE_HTML 0

#define wxUSE_RICHTEXT 0

#define wxUSE_ANIMATIONCTRL 0
#define wxUSE_CALENDARCTRL 0
#define wxUSE_COMBOBOX 0
#define wxUSE_COMBOCTRL 0
#define wxUSE_ODCOMBOBOX 0
#define wxUSE_BITMAPCOMBOBOX 0
#define wxUSE_BMPBUTTON 0
#define wxUSE_CHECKLISTBOX 0
#define wxUSE_GAUGE 0
#define wxUSE_GRID 0
#define wxUSE_LISTBOX 0
#define wxUSE_LISTCTRL 0
#define wxUSE_NOTEBOOK 0
#define wxUSE_SLIDER 0
#define wxUSE_SPINBTN 0
#define wxUSE_SPINCTRL 0
#define wxUSE_TREECTRL 0
#define wxUSE_DATEPICKCTRL 0
#define wxUSE_DATAVIEWCTRL 0
#define wxUSE_EDITABLELISTBOX 0
#define wxUSE_FILEPICKERCTRL 0
#define wxUSE_DIRPICKERCTRL 0
#define wxUSE_FILECTRL 0
#define wxUSE_COLOURPICKERCTRL 0
#define wxUSE_FONTPICKERCTRL 0
#define wxUSE_DEBUGREPORT 0
#define wxUSE_HYPERLINKCTRL 0
#define wxUSE_STC 0
#define wxUSE_AUI 0
#define wxUSE_BUSYINFO 0
#define wxUSE_SEARCHCTRL 0

#define wxUSE_LOGWINDOW 0
#define wxUSE_LOG_DIALOG 0
#define wxUSE_LISTBOOK 0
#define wxUSE_CHOICEBOOK 0
#define wxUSE_TREEBOOK 0
#define wxUSE_TOOLBOOK 0
#define wxUSE_CHOICEDLG 0
#define wxUSE_HELP 0
#define wxUSE_PROGRESSDLG 0
#define  wxUSE_FONTDLG 0
#define wxUSE_FILEDLG 0
#define wxUSE_CHOICEDLG 0
#define wxUSE_NUMBERDLG 0
#define wxUSE_TEXTDLG 0
#define wxUSE_DIRDLG 0
#define wxUSE_STARTUP_TIPS 0
#define wxUSE_WIZARDDLG 0
#define wxUSE_TOOLBAR_NATIVE 0
#define wxUSE_FINDREPLDLG 0
#define wxUSE_TASKBARICON 0


/*
#if wxUSE_POPUPWIN
#undef wxUSE_POPUPWIN
#define wxUSE_POPUPWIN 0
#endif

#if wxUSE_COMBOBOX
#undef wxUSE_COMBOBOX
#define wxUSE_COMBOBOX 0
#endif


#if wxUSE_MENUS
#undef wxUSE_MENUS
#define wxUSE_MENUS 0
#endif

#if wxUSE_CALENDARCTRL
#undef wxUSE_CALENDARCTRL
#define wxUSE_CALENDARCTRL 0
#endif

#if wxUSE_WXHTML_HELP
#undef wxUSE_WXHTML_HELP
#define wxUSE_WXHTML_HELP 0
#endif

#if wxUSE_DOC_VIEW_ARCHITECTURE
#undef wxUSE_DOC_VIEW_ARCHITECTURE
#define wxUSE_DOC_VIEW_ARCHITECTURE 0
#endif

#if wxUSE_PRINTING_ARCHITECTURE
#undef wxUSE_PRINTING_ARCHITECTURE
#define wxUSE_PRINTING_ARCHITECTURE 0
#endif

*/

#if wxUSE_CLIPBOARD
#undef wxUSE_CLIPBOARD
#define wxUSE_CLIPBOARD 0
#endif // wxUSE_CLIPBOARD

#endif
    /* _WX_OSX_IPHONE_CHKCONF_H_ */

