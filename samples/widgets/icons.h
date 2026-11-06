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
enum class WidgetsIcon
{
    ActivityIndicator,
    BmpComboBox,
    Button,
    CheckBox,
    Choice,
    Choicebook,
    ColourPicker,
    ComboBox,
    DatePicker,
    DirCtrl,
    DirPicker,
    FilePicker,
    FontPicker,
    Gauge,
    Header,
    Hyperlink,
    Listbook,
    ListBox,
    Native,
    Notebook,
    ODComboBox,
    RadioBox,
    Slider,
    SpinButton,
    StaticBitmap,
    StaticBox,
    Text,
    TimePicker,
    ToggleButton,
    Max
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

// Return the name of the icon, e.g. "Button" for WidgetsIcon::Button.
const char* GetWidgetsIconName(WidgetsIcon icon);

#endif // _WX_SAMPLE_WIDGETS_ICONS_H_
