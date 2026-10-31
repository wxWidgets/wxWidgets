/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        icons.h
// Purpose:     Icons used for the pages of the widgets sample
// Author:      wxWidgets team
// Created:     2026-10-31
// Copyright:   (c) 2026 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLE_WIDGETS_ICONS_H_
#define _WX_SAMPLE_WIDGETS_ICONS_H_

#include "wx/bmpbndl.h"

// All the icons used for the pages of the sample.
enum WidgetsIcon
{
    WidgetsIcon_ActivityIndicator,
    WidgetsIcon_BmpComboBox,
    WidgetsIcon_Button,
    WidgetsIcon_CheckBox,
    WidgetsIcon_Choice,
    WidgetsIcon_Choicebook,
    WidgetsIcon_ColourPicker,
    WidgetsIcon_ComboBox,
    WidgetsIcon_DatePicker,
    WidgetsIcon_DirCtrl,
    WidgetsIcon_DirPicker,
    WidgetsIcon_FilePicker,
    WidgetsIcon_FontPicker,
    WidgetsIcon_Gauge,
    WidgetsIcon_Header,
    WidgetsIcon_Hyperlink,
    WidgetsIcon_Listbook,
    WidgetsIcon_ListBox,
    WidgetsIcon_Native,
    WidgetsIcon_Notebook,
    WidgetsIcon_ODComboBox,
    WidgetsIcon_RadioBox,
    WidgetsIcon_Slider,
    WidgetsIcon_SpinButton,
    WidgetsIcon_StaticBitmap,
    WidgetsIcon_StaticBox,
    WidgetsIcon_Text,
    WidgetsIcon_TimePicker,
    WidgetsIcon_ToggleButton,
    WidgetsIcon_Max
};

// Return the bundle for the given icon of the default size of 16x16.
//
// The icons are defined in SVG format in both light and dark versions and the
// returned bundle uses the one appropriate for the current system appearance
// when the bitmap is retrieved from it, so the images need to be retrieved
// from it again, e.g. by calling wxWithImages::SetImages(), when the
// appearance changes.
//
// If SVG support is not available, the bundle uses the XPM version of the icon.
wxBitmapBundle CreateWidgetsIconBundle(WidgetsIcon icon);

#endif // _WX_SAMPLE_WIDGETS_ICONS_H_
