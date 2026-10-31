/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        icons.cpp
// Purpose:     Icons used for the pages of the widgets sample
// Author:      wxWidgets team
// Created:     2026-10-31
// Copyright:   (c) 2026 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
#endif

#include "icons.h"
#include "widgets.h"

#ifdef wxHAS_SVG

// ----------------------------------------------------------------------------
// SVG data
// ----------------------------------------------------------------------------

// All icons use 16x16 view box. The light versions are meant to be shown on
// light background and use dark outlines, while the dark versions are used in
// the dark mode and use light outlines on dark background.
//
// Note that only the subset of SVG supported by NanoSVG is used here, i.e. no
// text, styles, clipping paths or masks.

static const char svg_activity_indicator_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <circle cx="8" cy="2.5" r="1.6" fill="#404040"/>
  <circle cx="11.889" cy="4.111" r="1.6" fill="#404040" fill-opacity="0.89"/>
  <circle cx="13.5" cy="8" r="1.6" fill="#404040" fill-opacity="0.78"/>
  <circle cx="11.889" cy="11.889" r="1.6" fill="#404040" fill-opacity="0.67"/>
  <circle cx="8" cy="13.5" r="1.6" fill="#404040" fill-opacity="0.56"/>
  <circle cx="4.111" cy="11.889" r="1.6" fill="#404040" fill-opacity="0.45"/>
  <circle cx="2.5" cy="8" r="1.6" fill="#404040" fill-opacity="0.34"/>
  <circle cx="4.111" cy="4.111" r="1.6" fill="#404040" fill-opacity="0.23"/>
</svg>)svg";

static const char svg_activity_indicator_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <circle cx="8" cy="2.5" r="1.6" fill="#d4d4d4"/>
  <circle cx="11.889" cy="4.111" r="1.6" fill="#d4d4d4" fill-opacity="0.89"/>
  <circle cx="13.5" cy="8" r="1.6" fill="#d4d4d4" fill-opacity="0.78"/>
  <circle cx="11.889" cy="11.889" r="1.6" fill="#d4d4d4" fill-opacity="0.67"/>
  <circle cx="8" cy="13.5" r="1.6" fill="#d4d4d4" fill-opacity="0.56"/>
  <circle cx="4.111" cy="11.889" r="1.6" fill="#d4d4d4" fill-opacity="0.45"/>
  <circle cx="2.5" cy="8" r="1.6" fill="#d4d4d4" fill-opacity="0.34"/>
  <circle cx="4.111" cy="4.111" r="1.6" fill="#d4d4d4" fill-opacity="0.23"/>
</svg>)svg";

static const char svg_bmp_combo_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#ffffff" stroke="#404040"/>
  <rect x="10" y="1" width="5" height="4" fill="#e6e6e6"/>
  <path d="M10.5 1V5" fill="none" stroke="#404040"/>
  <path d="M10.5 2h4l-2 2z" fill="#404040"/>
  <rect x="2" y="2" width="2" height="2" fill="#d13438"/>
  <rect x="5" y="2" width="4" height="1" fill="#6e6e6e"/>
  <rect x="0.5" y="5.5" width="15" height="10" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="6.5" width="14" height="3" fill="#cce4f7"/>
  <rect x="2" y="7" width="2" height="2" fill="#d13438"/>
  <rect x="5" y="7" width="6" height="1" fill="#6e6e6e"/>
  <rect x="2" y="10" width="2" height="2" fill="#0f7b0f"/>
  <rect x="5" y="10" width="5" height="1" fill="#6e6e6e"/>
  <rect x="2" y="13" width="2" height="2" fill="#0063b1"/>
  <rect x="5" y="13" width="4" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_bmp_combo_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="10" y="1" width="5" height="4" fill="#454545"/>
  <path d="M10.5 1V5" fill="none" stroke="#d4d4d4"/>
  <path d="M10.5 2h4l-2 2z" fill="#d4d4d4"/>
  <rect x="2" y="2" width="2" height="2" fill="#ff6b6b"/>
  <rect x="5" y="2" width="4" height="1" fill="#a8a8a8"/>
  <rect x="0.5" y="5.5" width="15" height="10" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="6.5" width="14" height="3" fill="#1f4e70"/>
  <rect x="2" y="7" width="2" height="2" fill="#ff6b6b"/>
  <rect x="5" y="7" width="6" height="1" fill="#a8a8a8"/>
  <rect x="2" y="10" width="2" height="2" fill="#6ccb5f"/>
  <rect x="5" y="10" width="5" height="1" fill="#a8a8a8"/>
  <rect x="2" y="13" width="2" height="2" fill="#60a5fa"/>
  <rect x="5" y="13" width="4" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_button_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="3.5" width="15" height="9" rx="2" fill="#e6e6e6" stroke="#404040"/>
  <rect x="4" y="7.5" width="8" height="1" fill="#404040"/>
</svg>)svg";

static const char svg_button_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="3.5" width="15" height="9" rx="2" fill="#454545" stroke="#d4d4d4"/>
  <rect x="4" y="7.5" width="8" height="1" fill="#d4d4d4"/>
</svg>)svg";

static const char svg_check_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="1.5" y="1.5" width="13" height="13" rx="2" fill="#0067c0" stroke="#0067c0"/>
  <path d="M4.5 8.5l2.5 2.5l4.5 -5" fill="none" stroke="#ffffff" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round"/>
</svg>)svg";

static const char svg_check_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="1.5" y="1.5" width="13" height="13" rx="2" fill="#4cc2ff" stroke="#4cc2ff"/>
  <path d="M4.5 8.5l2.5 2.5l4.5 -5" fill="none" stroke="#000000" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round"/>
</svg>)svg";

static const char svg_choice_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#ffffff" stroke="#404040"/>
  <rect x="10" y="1" width="5" height="4" fill="#e6e6e6"/>
  <path d="M10.5 1V5" fill="none" stroke="#404040"/>
  <path d="M10.5 2h4l-2 2z" fill="#404040"/>
  <rect x="2" y="2" width="6" height="1" fill="#6e6e6e"/>
  <rect x="0.5" y="5.5" width="10" height="10" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="9" width="9" height="3" fill="#0067c0"/>
  <rect x="2" y="7" width="6" height="1" fill="#6e6e6e"/>
  <rect x="2" y="10" width="5" height="1" fill="#ffffff"/>
  <rect x="2" y="13" width="7" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_choice_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="10" y="1" width="5" height="4" fill="#454545"/>
  <path d="M10.5 1V5" fill="none" stroke="#d4d4d4"/>
  <path d="M10.5 2h4l-2 2z" fill="#d4d4d4"/>
  <rect x="2" y="2" width="6" height="1" fill="#a8a8a8"/>
  <rect x="0.5" y="5.5" width="10" height="10" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="9" width="9" height="3" fill="#4cc2ff"/>
  <rect x="2" y="7" width="6" height="1" fill="#a8a8a8"/>
  <rect x="2" y="10" width="5" height="1" fill="#000000"/>
  <rect x="2" y="13" width="7" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_choicebook_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#ffffff" stroke="#404040"/>
  <rect x="10" y="1" width="5" height="4" fill="#e6e6e6"/>
  <path d="M10.5 1V5" fill="none" stroke="#404040"/>
  <path d="M10.5 2h4l-2 2z" fill="#404040"/>
  <rect x="2" y="2" width="6" height="1" fill="#6e6e6e"/>
  <rect x="3.5" y="7.5" width="12" height="7" fill="#e6e6e6" stroke="#404040"/>
  <rect x="0.5" y="9.5" width="12" height="6" fill="#ffffff" stroke="#404040"/>
  <rect x="2" y="11" width="6" height="1" fill="#6e6e6e"/>
  <rect x="2" y="13" width="8" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_choicebook_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="10" y="1" width="5" height="4" fill="#454545"/>
  <path d="M10.5 1V5" fill="none" stroke="#d4d4d4"/>
  <path d="M10.5 2h4l-2 2z" fill="#d4d4d4"/>
  <rect x="2" y="2" width="6" height="1" fill="#a8a8a8"/>
  <rect x="3.5" y="7.5" width="12" height="7" fill="#454545" stroke="#d4d4d4"/>
  <rect x="0.5" y="9.5" width="12" height="6" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="2" y="11" width="6" height="1" fill="#a8a8a8"/>
  <rect x="2" y="13" width="8" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_colour_picker_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="4.5" width="3" height="3" fill="#4d4d4d" stroke="#404040"/>
  <rect x="4.5" y="4.5" width="3" height="3" fill="#8c8c8c" stroke="#404040"/>
  <rect x="8.5" y="4.5" width="3" height="3" fill="#ffffff" stroke="#404040"/>
  <rect x="0.5" y="8.5" width="3" height="3" fill="#f2b600" stroke="#404040"/>
  <rect x="4.5" y="8.5" width="3" height="3" fill="#b4009e" stroke="#404040"/>
  <rect x="8.5" y="8.5" width="3" height="3" fill="#00a2c7" stroke="#404040"/>
  <rect x="0.5" y="12.5" width="3" height="3" fill="#d13438" stroke="#404040"/>
  <rect x="4.5" y="12.5" width="3" height="3" fill="#0063b1" stroke="#404040"/>
  <rect x="8.5" y="12.5" width="3" height="3" fill="#0f7b0f" stroke="#404040"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#2e9b3c"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#404040"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#ffffff" stroke="#404040" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#404040"/>
  </g>
</svg>)svg";

static const char svg_colour_picker_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="4.5" width="3" height="3" fill="#b0b0b0" stroke="#d4d4d4"/>
  <rect x="4.5" y="4.5" width="3" height="3" fill="#9a9a9a" stroke="#d4d4d4"/>
  <rect x="8.5" y="4.5" width="3" height="3" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="0.5" y="8.5" width="3" height="3" fill="#ffd75e" stroke="#d4d4d4"/>
  <rect x="4.5" y="8.5" width="3" height="3" fill="#e879f9" stroke="#d4d4d4"/>
  <rect x="8.5" y="8.5" width="3" height="3" fill="#5ee0f5" stroke="#d4d4d4"/>
  <rect x="0.5" y="12.5" width="3" height="3" fill="#ff6b6b" stroke="#d4d4d4"/>
  <rect x="4.5" y="12.5" width="3" height="3" fill="#60a5fa" stroke="#d4d4d4"/>
  <rect x="8.5" y="12.5" width="3" height="3" fill="#6ccb5f" stroke="#d4d4d4"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#5cc96a"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#d4d4d4"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#2b2b2b" stroke="#d4d4d4" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#d4d4d4"/>
  </g>
</svg>)svg";

static const char svg_combo_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#ffffff" stroke="#404040"/>
  <rect x="10" y="1" width="5" height="4" fill="#e6e6e6"/>
  <path d="M10.5 1V5" fill="none" stroke="#404040"/>
  <path d="M10.5 2h4l-2 2z" fill="#404040"/>
  <rect x="2" y="2" width="6" height="1" fill="#6e6e6e"/>
  <rect x="0.5" y="5.5" width="15" height="10" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="6" width="9.5" height="3" fill="#cce4f7"/>
  <rect x="2" y="7" width="6" height="1" fill="#6e6e6e"/>
  <rect x="2" y="10" width="7" height="1" fill="#6e6e6e"/>
  <rect x="2" y="13" width="5" height="1" fill="#6e6e6e"/>
  <rect x="11" y="6" width="4" height="9" fill="#e6e6e6"/>
  <path d="M10.5 6V15" fill="none" stroke="#404040"/>
  <rect x="12" y="7" width="2" height="4" fill="#8c8c8c"/>
</svg>)svg";

static const char svg_combo_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="10" y="1" width="5" height="4" fill="#454545"/>
  <path d="M10.5 1V5" fill="none" stroke="#d4d4d4"/>
  <path d="M10.5 2h4l-2 2z" fill="#d4d4d4"/>
  <rect x="2" y="2" width="6" height="1" fill="#a8a8a8"/>
  <rect x="0.5" y="5.5" width="15" height="10" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="6" width="9.5" height="3" fill="#1f4e70"/>
  <rect x="2" y="7" width="6" height="1" fill="#a8a8a8"/>
  <rect x="2" y="10" width="7" height="1" fill="#a8a8a8"/>
  <rect x="2" y="13" width="5" height="1" fill="#a8a8a8"/>
  <rect x="11" y="6" width="4" height="9" fill="#454545"/>
  <path d="M10.5 6V15" fill="none" stroke="#d4d4d4"/>
  <rect x="12" y="7" width="2" height="4" fill="#9a9a9a"/>
</svg>)svg";

static const char svg_date_picker_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="3.5" width="11" height="12" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="4" width="10" height="3" fill="#0067c0"/>
  <rect x="2" y="9" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="4.5" y="9" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="7" y="9" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="9.5" y="9" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="2" y="11" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="4.5" y="11" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="7" y="11" width="1.5" height="1" fill="#d13438"/>
  <rect x="9.5" y="11" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="2" y="13" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="4.5" y="13" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="7" y="13" width="1.5" height="1" fill="#6e6e6e"/>
  <rect x="9.5" y="13" width="1.5" height="1" fill="#6e6e6e"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#2e9b3c"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#404040"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#ffffff" stroke="#404040" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#404040"/>
  </g>
</svg>)svg";

static const char svg_date_picker_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="3.5" width="11" height="12" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="4" width="10" height="3" fill="#4cc2ff"/>
  <rect x="2" y="9" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="4.5" y="9" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="7" y="9" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="9.5" y="9" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="2" y="11" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="4.5" y="11" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="7" y="11" width="1.5" height="1" fill="#ff6b6b"/>
  <rect x="9.5" y="11" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="2" y="13" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="4.5" y="13" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="7" y="13" width="1.5" height="1" fill="#a8a8a8"/>
  <rect x="9.5" y="13" width="1.5" height="1" fill="#a8a8a8"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#5cc96a"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#d4d4d4"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#2b2b2b" stroke="#d4d4d4" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#d4d4d4"/>
  </g>
</svg>)svg";

static const char svg_dir_ctrl_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M0.5 5.5V0.5H3.2l1 1H7.5V5.5z" fill="#f4c84a" stroke="#a87c10" stroke-linejoin="round"/>
  <path d="M0.5 2.5H7.5" fill="none" stroke="#a87c10"/>
  <path d="M3.5 6V13.5H6M3.5 8.5H6" fill="none" stroke="#8c8c8c"/>
  <path d="M6.5 10.5V6.5H10l1 1H15.5V10.5z" fill="#f4c84a" stroke="#a87c10" stroke-linejoin="round"/>
  <path d="M6.5 15.5V11.5H10l1 1H15.5V15.5z" fill="#f4c84a" stroke="#a87c10" stroke-linejoin="round"/>
</svg>)svg";

static const char svg_dir_ctrl_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M0.5 5.5V0.5H3.2l1 1H7.5V5.5z" fill="#d9a63a" stroke="#f2d58c" stroke-linejoin="round"/>
  <path d="M0.5 2.5H7.5" fill="none" stroke="#f2d58c"/>
  <path d="M3.5 6V13.5H6M3.5 8.5H6" fill="none" stroke="#9a9a9a"/>
  <path d="M6.5 10.5V6.5H10l1 1H15.5V10.5z" fill="#d9a63a" stroke="#f2d58c" stroke-linejoin="round"/>
  <path d="M6.5 15.5V11.5H10l1 1H15.5V15.5z" fill="#d9a63a" stroke="#f2d58c" stroke-linejoin="round"/>
</svg>)svg";

static const char svg_dir_picker_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M0.5 14.5V5.5H4.8l1 1H11.5V14.5z" fill="#f4c84a" stroke="#a87c10" stroke-linejoin="round"/>
  <path d="M0.5 7.5H11.5" fill="none" stroke="#a87c10"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#2e9b3c"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#404040"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#ffffff" stroke="#404040" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#404040"/>
  </g>
</svg>)svg";

static const char svg_dir_picker_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M0.5 14.5V5.5H4.8l1 1H11.5V14.5z" fill="#d9a63a" stroke="#f2d58c" stroke-linejoin="round"/>
  <path d="M0.5 7.5H11.5" fill="none" stroke="#f2d58c"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#5cc96a"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#d4d4d4"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#2b2b2b" stroke="#d4d4d4" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#d4d4d4"/>
  </g>
</svg>)svg";

static const char svg_file_picker_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M1.5 3.5H7.5L10.5 6.5V15.5H1.5z" fill="#ffffff" stroke="#404040" stroke-linejoin="round"/>
  <path d="M7.5 3.5V6.5H10.5" fill="none" stroke="#404040" stroke-linejoin="round"/>
  <rect x="3" y="8" width="4" height="1" fill="#6e6e6e"/>
  <rect x="3" y="10" width="6" height="1" fill="#6e6e6e"/>
  <rect x="3" y="12" width="6" height="1" fill="#6e6e6e"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#2e9b3c"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#404040"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#ffffff" stroke="#404040" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#404040"/>
  </g>
</svg>)svg";

static const char svg_file_picker_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M1.5 3.5H7.5L10.5 6.5V15.5H1.5z" fill="#2b2b2b" stroke="#d4d4d4" stroke-linejoin="round"/>
  <path d="M7.5 3.5V6.5H10.5" fill="none" stroke="#d4d4d4" stroke-linejoin="round"/>
  <rect x="3" y="8" width="4" height="1" fill="#a8a8a8"/>
  <rect x="3" y="10" width="6" height="1" fill="#a8a8a8"/>
  <rect x="3" y="12" width="6" height="1" fill="#a8a8a8"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#5cc96a"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#d4d4d4"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#2b2b2b" stroke="#d4d4d4" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#d4d4d4"/>
  </g>
</svg>)svg";

static const char svg_font_picker_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M0.5 15.5L4.5 4.5H6.5L10.5 15.5H8.5L7.6 13H3.4L2.5 15.5zM4 11.2H7L5.5 6.9z" fill="#404040" fill-rule="evenodd"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#2e9b3c"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#404040"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#ffffff" stroke="#404040" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#404040"/>
  </g>
</svg>)svg";

static const char svg_font_picker_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M0.5 15.5L4.5 4.5H6.5L10.5 15.5H8.5L7.6 13H3.4L2.5 15.5zM4 11.2H7L5.5 6.9z" fill="#d4d4d4" fill-rule="evenodd"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#5cc96a"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#d4d4d4"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#2b2b2b" stroke="#d4d4d4" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#d4d4d4"/>
  </g>
</svg>)svg";

static const char svg_gauge_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="4.5" width="15" height="7" fill="#ffffff" stroke="#404040"/>
  <rect x="2" y="6" width="8" height="4" fill="#06b025"/>
</svg>)svg";

static const char svg_gauge_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="4.5" width="15" height="7" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="2" y="6" width="8" height="4" fill="#3ccb4c"/>
</svg>)svg";

static const char svg_header_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="3.5" width="15" height="12" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="1" width="14" height="4" fill="#e6e6e6"/>
  <rect x="0.5" y="0.5" width="15" height="5" fill="none" stroke="#404040"/>
  <path d="M5.5 1V5M10.5 1V5" fill="none" stroke="#404040"/>
  <rect x="2" y="3" width="2" height="1" fill="#6e6e6e"/>
  <rect x="7" y="3" width="2" height="1" fill="#6e6e6e"/>
  <path d="M11.75 2h3l-1.5 2z" fill="#404040"/>
  <rect x="2" y="7" width="3" height="1" fill="#6e6e6e"/>
  <rect x="7" y="7" width="3" height="1" fill="#6e6e6e"/>
  <rect x="12" y="7" width="2" height="1" fill="#6e6e6e"/>
  <rect x="2" y="10" width="2" height="1" fill="#6e6e6e"/>
  <rect x="7" y="10" width="3" height="1" fill="#6e6e6e"/>
  <rect x="12" y="10" width="2" height="1" fill="#6e6e6e"/>
  <rect x="2" y="13" width="3" height="1" fill="#6e6e6e"/>
  <rect x="7" y="13" width="2" height="1" fill="#6e6e6e"/>
  <rect x="12" y="13" width="2" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_header_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="3.5" width="15" height="12" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="1" width="14" height="4" fill="#454545"/>
  <rect x="0.5" y="0.5" width="15" height="5" fill="none" stroke="#d4d4d4"/>
  <path d="M5.5 1V5M10.5 1V5" fill="none" stroke="#d4d4d4"/>
  <rect x="2" y="3" width="2" height="1" fill="#a8a8a8"/>
  <rect x="7" y="3" width="2" height="1" fill="#a8a8a8"/>
  <path d="M11.75 2h3l-1.5 2z" fill="#d4d4d4"/>
  <rect x="2" y="7" width="3" height="1" fill="#a8a8a8"/>
  <rect x="7" y="7" width="3" height="1" fill="#a8a8a8"/>
  <rect x="12" y="7" width="2" height="1" fill="#a8a8a8"/>
  <rect x="2" y="10" width="2" height="1" fill="#a8a8a8"/>
  <rect x="7" y="10" width="3" height="1" fill="#a8a8a8"/>
  <rect x="12" y="10" width="2" height="1" fill="#a8a8a8"/>
  <rect x="2" y="13" width="3" height="1" fill="#a8a8a8"/>
  <rect x="7" y="13" width="2" height="1" fill="#a8a8a8"/>
  <rect x="12" y="13" width="2" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_hyperlink_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="1" y="1" width="3" height="3" rx="1" fill="#0067c0"/>
  <rect x="5" y="1" width="3" height="3" rx="1" fill="#0067c0"/>
  <rect x="9" y="1" width="6" height="3" rx="1" fill="#0067c0"/>
  <rect x="1" y="5" width="14" height="1" fill="#0067c0"/>
  <path d="M6.5 15.5V8Q6.5 6.5 7.5 6.5Q8.5 6.5 8.5 8V10.5H12.5Q14.5 10.5 14.5 12.5V13L13.5 15.5z" fill="#ffffff" stroke="#404040" stroke-linejoin="round"/>
  <path d="M10.5 10.5V12.5M12.5 10.5V12.5" fill="none" stroke="#404040"/>
</svg>)svg";

static const char svg_hyperlink_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="1" y="1" width="3" height="3" rx="1" fill="#4cc2ff"/>
  <rect x="5" y="1" width="3" height="3" rx="1" fill="#4cc2ff"/>
  <rect x="9" y="1" width="6" height="3" rx="1" fill="#4cc2ff"/>
  <rect x="1" y="5" width="14" height="1" fill="#4cc2ff"/>
  <path d="M6.5 15.5V8Q6.5 6.5 7.5 6.5Q8.5 6.5 8.5 8V10.5H12.5Q14.5 10.5 14.5 12.5V13L13.5 15.5z" fill="#2b2b2b" stroke="#d4d4d4" stroke-linejoin="round"/>
  <path d="M10.5 10.5V12.5M12.5 10.5V12.5" fill="none" stroke="#d4d4d4"/>
</svg>)svg";

static const char svg_listbook_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="5.5" y="0.5" width="10" height="15" fill="#ffffff" stroke="#404040"/>
  <rect x="0.5" y="0.5" width="5" height="15" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="5.5" width="4" height="4" fill="#cce4f7"/>
  <rect x="2" y="2" width="2" height="2" fill="#8c8c8c"/>
  <rect x="2" y="6.5" width="2" height="2" fill="#0067c0"/>
  <rect x="2" y="11" width="2" height="2" fill="#8c8c8c"/>
  <rect x="7" y="3" width="6" height="1" fill="#6e6e6e"/>
  <rect x="7" y="5" width="7" height="1" fill="#6e6e6e"/>
  <rect x="7" y="7" width="5" height="1" fill="#6e6e6e"/>
  <rect x="7" y="9" width="7" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_listbook_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="5.5" y="0.5" width="10" height="15" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="0.5" y="0.5" width="5" height="15" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="5.5" width="4" height="4" fill="#1f4e70"/>
  <rect x="2" y="2" width="2" height="2" fill="#9a9a9a"/>
  <rect x="2" y="6.5" width="2" height="2" fill="#4cc2ff"/>
  <rect x="2" y="11" width="2" height="2" fill="#9a9a9a"/>
  <rect x="7" y="3" width="6" height="1" fill="#a8a8a8"/>
  <rect x="7" y="5" width="7" height="1" fill="#a8a8a8"/>
  <rect x="7" y="7" width="5" height="1" fill="#a8a8a8"/>
  <rect x="7" y="9" width="7" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_list_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="15" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="5" width="9.5" height="3" fill="#cce4f7"/>
  <rect x="2" y="2" width="6" height="1" fill="#6e6e6e"/>
  <rect x="2" y="6" width="7" height="1" fill="#6e6e6e"/>
  <rect x="2" y="10" width="5" height="1" fill="#6e6e6e"/>
  <rect x="2" y="13" width="6" height="1" fill="#6e6e6e"/>
  <rect x="11" y="1" width="4" height="14" fill="#e6e6e6"/>
  <path d="M10.5 1V15" fill="none" stroke="#404040"/>
  <path d="M11 3.5h4l-2 -2z" fill="#404040"/>
  <path d="M11 12.5h4l-2 2z" fill="#404040"/>
  <rect x="12" y="5" width="2" height="4" fill="#8c8c8c"/>
</svg>)svg";

static const char svg_list_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="15" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="5" width="9.5" height="3" fill="#1f4e70"/>
  <rect x="2" y="2" width="6" height="1" fill="#a8a8a8"/>
  <rect x="2" y="6" width="7" height="1" fill="#a8a8a8"/>
  <rect x="2" y="10" width="5" height="1" fill="#a8a8a8"/>
  <rect x="2" y="13" width="6" height="1" fill="#a8a8a8"/>
  <rect x="11" y="1" width="4" height="14" fill="#454545"/>
  <path d="M10.5 1V15" fill="none" stroke="#d4d4d4"/>
  <path d="M11 3.5h4l-2 -2z" fill="#d4d4d4"/>
  <path d="M11 12.5h4l-2 2z" fill="#d4d4d4"/>
  <rect x="12" y="5" width="2" height="4" fill="#9a9a9a"/>
</svg>)svg";

static const char svg_native_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M8 1L14.5 4.5L8 8L1.5 4.5z" fill="#9fd0f5" stroke="#404040" stroke-linejoin="round"/>
  <path d="M1.5 4.5L8 8V15L1.5 11.5z" fill="#e04a4a" stroke="#404040" stroke-linejoin="round"/>
  <path d="M14.5 4.5L8 8V15L14.5 11.5z" fill="#6cbf3a" stroke="#404040" stroke-linejoin="round"/>
</svg>)svg";

static const char svg_native_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M8 1L14.5 4.5L8 8L1.5 4.5z" fill="#5aa9e6" stroke="#d4d4d4" stroke-linejoin="round"/>
  <path d="M1.5 4.5L8 8V15L1.5 11.5z" fill="#e65c5c" stroke="#d4d4d4" stroke-linejoin="round"/>
  <path d="M14.5 4.5L8 8V15L14.5 11.5z" fill="#7fcf4a" stroke="#d4d4d4" stroke-linejoin="round"/>
</svg>)svg";

static const char svg_notebook_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="6.5" y="2.5" width="5" height="4" fill="#e6e6e6" stroke="#404040"/>
  <path d="M0.5 1.5H6.5V5.5H15.5V15.5H0.5z" fill="#ffffff" stroke="#404040"/>
  <rect x="2" y="3" width="3" height="1" fill="#6e6e6e"/>
  <rect x="8" y="4" width="2" height="1" fill="#6e6e6e"/>
  <rect x="2" y="8" width="10" height="1" fill="#6e6e6e"/>
  <rect x="2" y="10" width="8" height="1" fill="#6e6e6e"/>
  <rect x="2" y="12" width="11" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_notebook_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="6.5" y="2.5" width="5" height="4" fill="#454545" stroke="#d4d4d4"/>
  <path d="M0.5 1.5H6.5V5.5H15.5V15.5H0.5z" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="2" y="3" width="3" height="1" fill="#a8a8a8"/>
  <rect x="8" y="4" width="2" height="1" fill="#a8a8a8"/>
  <rect x="2" y="8" width="10" height="1" fill="#a8a8a8"/>
  <rect x="2" y="10" width="8" height="1" fill="#a8a8a8"/>
  <rect x="2" y="12" width="11" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_od_combo_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#ffffff" stroke="#404040"/>
  <rect x="10" y="1" width="5" height="4" fill="#e6e6e6"/>
  <path d="M10.5 1V5" fill="none" stroke="#404040"/>
  <path d="M10.5 2h4l-2 2z" fill="#404040"/>
  <rect x="2" y="2" width="7" height="2" fill="#6e6e6e"/>
  <rect x="0.5" y="5.5" width="15" height="10" fill="#ffffff" stroke="#404040"/>
  <rect x="1" y="6" width="14" height="3" fill="#cce4f7"/>
  <rect x="2" y="7" width="11" height="1" fill="#404040"/>
  <path d="M2 10.5H13" fill="none" stroke="#404040" stroke-dasharray="3 1"/>
  <path d="M2 13.5H13" fill="none" stroke="#404040" stroke-dasharray="1 1"/>
</svg>)svg";

static const char svg_od_combo_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="10" y="1" width="5" height="4" fill="#454545"/>
  <path d="M10.5 1V5" fill="none" stroke="#d4d4d4"/>
  <path d="M10.5 2h4l-2 2z" fill="#d4d4d4"/>
  <rect x="2" y="2" width="7" height="2" fill="#a8a8a8"/>
  <rect x="0.5" y="5.5" width="15" height="10" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="1" y="6" width="14" height="3" fill="#1f4e70"/>
  <rect x="2" y="7" width="11" height="1" fill="#d4d4d4"/>
  <path d="M2 10.5H13" fill="none" stroke="#d4d4d4" stroke-dasharray="3 1"/>
  <path d="M2 13.5H13" fill="none" stroke="#d4d4d4" stroke-dasharray="1 1"/>
</svg>)svg";

static const char svg_radio_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <circle cx="4" cy="4" r="3" fill="#ffffff" stroke="#0067c0"/>
  <circle cx="4" cy="4" r="1.5" fill="#0067c0"/>
  <circle cx="4" cy="12" r="3" fill="#ffffff" stroke="#404040"/>
  <rect x="9" y="4" width="6" height="1" fill="#6e6e6e"/>
  <rect x="9" y="12" width="5" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_radio_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <circle cx="4" cy="4" r="3" fill="#2b2b2b" stroke="#4cc2ff"/>
  <circle cx="4" cy="4" r="1.5" fill="#4cc2ff"/>
  <circle cx="4" cy="12" r="3" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="9" y="4" width="6" height="1" fill="#a8a8a8"/>
  <rect x="9" y="12" width="5" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_slider_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="1" y="7" width="14" height="2" rx="1" fill="#8c8c8c"/>
  <rect x="1" y="7" width="6" height="2" rx="1" fill="#0067c0"/>
  <circle cx="7.5" cy="8" r="3.5" fill="#ffffff" stroke="#404040"/>
  <circle cx="7.5" cy="8" r="1.75" fill="#0067c0"/>
</svg>)svg";

static const char svg_slider_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="1" y="7" width="14" height="2" rx="1" fill="#9a9a9a"/>
  <rect x="1" y="7" width="6" height="2" rx="1" fill="#4cc2ff"/>
  <circle cx="7.5" cy="8" r="3.5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <circle cx="7.5" cy="8" r="1.75" fill="#4cc2ff"/>
</svg>)svg";

static const char svg_spin_button_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="2.5" width="15" height="11" fill="#ffffff" stroke="#404040"/>
  <rect x="10" y="3" width="5" height="10" fill="#e6e6e6"/>
  <path d="M10.5 3V13M11 8H15" fill="none" stroke="#404040"/>
  <path d="M10.5 6.25h4l-2 -2z" fill="#404040"/>
  <path d="M10.5 9.75h4l-2 2z" fill="#404040"/>
  <rect x="2" y="7" width="6" height="2" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_spin_button_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="2.5" width="15" height="11" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="10" y="3" width="5" height="10" fill="#454545"/>
  <path d="M10.5 3V13M11 8H15" fill="none" stroke="#d4d4d4"/>
  <path d="M10.5 6.25h4l-2 -2z" fill="#d4d4d4"/>
  <path d="M10.5 9.75h4l-2 2z" fill="#d4d4d4"/>
  <rect x="2" y="7" width="6" height="2" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_static_bitmap_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="2.5" width="15" height="11" fill="#cde6fa" stroke="#404040"/>
  <path d="M1 13L5.5 7.5L9 11L11 9L15 13z" fill="#3c9a3c"/>
  <circle cx="11.5" cy="5.5" r="1.5" fill="#f2b600"/>
</svg>)svg";

static const char svg_static_bitmap_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="2.5" width="15" height="11" fill="#24445e" stroke="#d4d4d4"/>
  <path d="M1 13L5.5 7.5L9 11L11 9L15 13z" fill="#4fae4f"/>
  <circle cx="11.5" cy="5.5" r="1.5" fill="#ffd75e"/>
</svg>)svg";

static const char svg_static_box_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M4 3.5H0.5V15.5H15.5V3.5H12" fill="none" stroke="#404040"/>
  <rect x="5" y="3" width="6" height="1" fill="#404040"/>
  <rect x="3.5" y="6.5" width="3" height="3" fill="#ffffff" stroke="#404040"/>
  <rect x="8" y="7.5" width="5" height="1" fill="#6e6e6e"/>
  <rect x="3.5" y="11.5" width="3" height="3" fill="#ffffff" stroke="#404040"/>
  <rect x="8" y="12.5" width="4" height="1" fill="#6e6e6e"/>
</svg>)svg";

static const char svg_static_box_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <path d="M4 3.5H0.5V15.5H15.5V3.5H12" fill="none" stroke="#d4d4d4"/>
  <rect x="5" y="3" width="6" height="1" fill="#d4d4d4"/>
  <rect x="3.5" y="6.5" width="3" height="3" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="8" y="7.5" width="5" height="1" fill="#a8a8a8"/>
  <rect x="3.5" y="11.5" width="3" height="3" fill="#2b2b2b" stroke="#d4d4d4"/>
  <rect x="8" y="12.5" width="4" height="1" fill="#a8a8a8"/>
</svg>)svg";

static const char svg_text_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="2.5" width="15" height="11" fill="#ffffff" stroke="#404040"/>
  <circle cx="4.25" cy="9" r="1.75" fill="none" stroke="#404040" stroke-width="1.25"/>
  <path d="M6 6.5V11" fill="none" stroke="#404040" stroke-width="1.25"/>
  <path d="M8 4.5V11" fill="none" stroke="#404040" stroke-width="1.25"/>
  <circle cx="9.75" cy="9" r="1.75" fill="none" stroke="#404040" stroke-width="1.25"/>
  <path d="M13.5 4V12" fill="none" stroke="#0067c0"/>
</svg>)svg";

static const char svg_text_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="2.5" width="15" height="11" fill="#2b2b2b" stroke="#d4d4d4"/>
  <circle cx="4.25" cy="9" r="1.75" fill="none" stroke="#d4d4d4" stroke-width="1.25"/>
  <path d="M6 6.5V11" fill="none" stroke="#d4d4d4" stroke-width="1.25"/>
  <path d="M8 4.5V11" fill="none" stroke="#d4d4d4" stroke-width="1.25"/>
  <circle cx="9.75" cy="9" r="1.75" fill="none" stroke="#d4d4d4" stroke-width="1.25"/>
  <path d="M13.5 4V12" fill="none" stroke="#4cc2ff"/>
</svg>)svg";

static const char svg_time_picker_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <circle cx="6" cy="10" r="5.5" fill="#ffffff" stroke="#404040"/>
  <path d="M6 6.5V10H8.5" fill="none" stroke="#404040" stroke-linecap="round" stroke-linejoin="round"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#2e9b3c"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#404040"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#ffffff" stroke="#404040" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#404040"/>
  </g>
</svg>)svg";

static const char svg_time_picker_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <circle cx="6" cy="10" r="5.5" fill="#2b2b2b" stroke="#d4d4d4"/>
  <path d="M6 6.5V10H8.5" fill="none" stroke="#d4d4d4" stroke-linecap="round" stroke-linejoin="round"/>
  <g transform="translate(12 4) rotate(45) translate(0 -5)">
    <rect x="-1.75" y="0" width="3.5" height="4" rx="1.75" fill="#5cc96a"/>
    <rect x="-2.25" y="3.5" width="4.5" height="1.25" rx="0.5" fill="#d4d4d4"/>
    <rect x="-1" y="4.75" width="2" height="4" fill="#2b2b2b" stroke="#d4d4d4" stroke-width="0.75"/>
    <path d="M-0.6 8.75L0 10.5L0.6 8.75z" fill="#d4d4d4"/>
  </g>
</svg>)svg";

static const char svg_toggle_button_light[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="6" rx="1.5" fill="#e6e6e6" stroke="#404040"/>
  <rect x="4" y="3" width="8" height="1" fill="#404040"/>
  <rect x="0.5" y="9.5" width="15" height="6" rx="1.5" fill="#0067c0" stroke="#0067c0"/>
  <rect x="4" y="12" width="8" height="1" fill="#ffffff"/>
</svg>)svg";

static const char svg_toggle_button_dark[] = R"svg(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
  <rect x="0.5" y="0.5" width="15" height="6" rx="1.5" fill="#454545" stroke="#d4d4d4"/>
  <rect x="4" y="3" width="8" height="1" fill="#d4d4d4"/>
  <rect x="0.5" y="9.5" width="15" height="6" rx="1.5" fill="#4cc2ff" stroke="#4cc2ff"/>
  <rect x="4" y="12" width="8" height="1" fill="#000000"/>
</svg>)svg";

// ----------------------------------------------------------------------------
// bundle implementation using light or dark icon depending on the appearance
// ----------------------------------------------------------------------------

namespace
{

class LightDarkBundleImpl : public wxBitmapBundleImpl
{
public:
    LightDarkBundleImpl(const wxBitmapBundle& light, const wxBitmapBundle& dark)
        : m_light(light),
          m_dark(dark)
    {
    }

    wxSize GetDefaultSize() const override
    {
        return GetCurrent().GetDefaultSize();
    }

    wxSize GetPreferredBitmapSizeAtScale(double scale) const override
    {
        return GetCurrent().GetPreferredBitmapSizeAtScale(scale);
    }

    wxBitmap GetBitmap(const wxSize& size) override
    {
        return GetCurrent().GetBitmap(size);
    }

private:
    const wxBitmapBundle& GetCurrent() const
    {
        return wxSystemSettings::GetAppearance().IsDark() ? m_dark : m_light;
    }

    const wxBitmapBundle m_light;
    const wxBitmapBundle m_dark;
};

const struct
{
    const char* light;
    const char* dark;
} gs_icons[] =
{
    { svg_activity_indicator_light, svg_activity_indicator_dark },
    { svg_bmp_combo_box_light, svg_bmp_combo_box_dark },
    { svg_button_light, svg_button_dark },
    { svg_check_box_light, svg_check_box_dark },
    { svg_choice_light, svg_choice_dark },
    { svg_choicebook_light, svg_choicebook_dark },
    { svg_colour_picker_light, svg_colour_picker_dark },
    { svg_combo_box_light, svg_combo_box_dark },
    { svg_date_picker_light, svg_date_picker_dark },
    { svg_dir_ctrl_light, svg_dir_ctrl_dark },
    { svg_dir_picker_light, svg_dir_picker_dark },
    { svg_file_picker_light, svg_file_picker_dark },
    { svg_font_picker_light, svg_font_picker_dark },
    { svg_gauge_light, svg_gauge_dark },
    { svg_header_light, svg_header_dark },
    { svg_hyperlink_light, svg_hyperlink_dark },
    { svg_listbook_light, svg_listbook_dark },
    { svg_list_box_light, svg_list_box_dark },
    { svg_native_light, svg_native_dark },
    { svg_notebook_light, svg_notebook_dark },
    { svg_od_combo_box_light, svg_od_combo_box_dark },
    { svg_radio_box_light, svg_radio_box_dark },
    { svg_slider_light, svg_slider_dark },
    { svg_spin_button_light, svg_spin_button_dark },
    { svg_static_bitmap_light, svg_static_bitmap_dark },
    { svg_static_box_light, svg_static_box_dark },
    { svg_text_light, svg_text_dark },
    { svg_time_picker_light, svg_time_picker_dark },
    { svg_toggle_button_light, svg_toggle_button_dark },
};

} // anonymous namespace

wxBitmapBundle CreateWidgetsIconBundle(WidgetsIcon icon)
{
    static_assert(WXSIZEOF(gs_icons) == WidgetsIcon_Max, "icons table size mismatch");

    wxCHECK_MSG( icon >= 0 && icon < WidgetsIcon_Max, wxBitmapBundle(),
                 "invalid icon" );

    const wxSize size(16, 16);

    return wxBitmapBundle::FromImpl(new LightDarkBundleImpl
                                    (
                                        wxBitmapBundle::FromSVG(gs_icons[icon].light, size),
                                        wxBitmapBundle::FromSVG(gs_icons[icon].dark, size)
                                    ));
}

#else // !wxHAS_SVG

// ----------------------------------------------------------------------------
// fall back to the XPM icons when SVG is not supported
// ----------------------------------------------------------------------------

#include "icons/activityindicator.xpm"
#include "icons/bmpcombobox.xpm"
#include "icons/button.xpm"
#include "icons/checkbox.xpm"
#include "icons/choice.xpm"
#include "icons/choicebk.xpm"
#include "icons/clrpicker.xpm"
#include "icons/combobox.xpm"
#include "icons/datepick.xpm"
#include "icons/dirctrl.xpm"
#include "icons/dirpicker.xpm"
#include "icons/filepicker.xpm"
#include "icons/fontpicker.xpm"
#include "icons/gauge.xpm"
#include "icons/header.xpm"
#include "icons/hyperlnk.xpm"
#include "icons/listbook.xpm"
#include "icons/listbox.xpm"
#include "icons/native.xpm"
#include "icons/notebook.xpm"
#include "icons/odcombobox.xpm"
#include "icons/radiobox.xpm"
#include "icons/slider.xpm"
#include "icons/spinbtn.xpm"
#include "icons/statbmp.xpm"
#include "icons/statbox.xpm"
#include "icons/text.xpm"
#include "icons/timepick.xpm"
#include "icons/toggle.xpm"

namespace
{

const char* const* const gs_icons[] =
{
    activityindicator_xpm,
    bmpcombobox_xpm,
    button_xpm,
    checkbox_xpm,
    choice_xpm,
    choicebk_xpm,
    clrpicker_xpm,
    combobox_xpm,
    datepick_xpm,
    dirctrl_xpm,
    dirpicker_xpm,
    filepicker_xpm,
    fontpicker_xpm,
    gauge_xpm,
    header_xpm,
    hyperlnk_xpm,
    listbook_xpm,
    listbox_xpm,
    native_xpm,
    notebook_xpm,
    odcombobox_xpm,
    radio_xpm,
    slider_xpm,
    spinbtn_xpm,
    statbmp_xpm,
    statbox_xpm,
    text_xpm,
    timepick_xpm,
    toggle_xpm,
};

} // anonymous namespace

wxBitmapBundle CreateWidgetsIconBundle(WidgetsIcon icon)
{
    static_assert(WXSIZEOF(gs_icons) == WidgetsIcon_Max, "icons table size mismatch");

    wxCHECK_MSG( icon >= 0 && icon < WidgetsIcon_Max, wxBitmapBundle(),
                 "invalid icon" );

    return WidgetsPage::CreateBitmapBundle(gs_icons[icon]);
}

#endif // wxHAS_SVG/!wxHAS_SVG

// ----------------------------------------------------------------------------
// icon names
// ----------------------------------------------------------------------------

const char* GetWidgetsIconName(WidgetsIcon icon)
{
    static const char* const s_names[] =
    {
        "ActivityIndicator",
        "BmpComboBox",
        "Button",
        "CheckBox",
        "Choice",
        "Choicebook",
        "ColourPicker",
        "ComboBox",
        "DatePicker",
        "DirCtrl",
        "DirPicker",
        "FilePicker",
        "FontPicker",
        "Gauge",
        "Header",
        "Hyperlink",
        "Listbook",
        "ListBox",
        "Native",
        "Notebook",
        "ODComboBox",
        "RadioBox",
        "Slider",
        "SpinButton",
        "StaticBitmap",
        "StaticBox",
        "Text",
        "TimePicker",
        "ToggleButton",
    };

    static_assert(WXSIZEOF(s_names) == WidgetsIcon_Max, "icon names size mismatch");

    wxCHECK_MSG( icon >= 0 && icon < WidgetsIcon_Max, "", "invalid icon" );

    return s_names[icon];
}
