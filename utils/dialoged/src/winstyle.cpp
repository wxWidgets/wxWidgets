/////////////////////////////////////////////////////////////////////////////
// Name:        winstyle.cpp
// Purpose:     Window styles
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "winstyle.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "winstyle.h"

/*
* Styles
*/

/* wxListBox */
static wxWindowStylePair g_WindowStylesListBox[] = {
    { "wxLB_SINGLE", wxLB_SINGLE },
    { "wxLB_MULTIPLE", wxLB_MULTIPLE },
    { "wxLB_EXTENDED", wxLB_EXTENDED },
    { "wxLB_NEEDED_SB", wxLB_NEEDED_SB },
    { "wxLB_ALWAYS_SB", wxLB_ALWAYS_SB },
    { "wxLB_SORT", wxLB_SORT },
    { "wxLB_OWNERDRAW", wxLB_OWNERDRAW },
    { "wxLB_HSCROLL", wxLB_HSCROLL }
};

int g_WindowStylesListBoxCount = sizeof(g_WindowStylesListBox)/sizeof(wxWindowStylePair) ;

/* wxComboxBox */
static wxWindowStylePair g_WindowStylesComboBox[] = {
    { "wxCB_SIMPLE", wxCB_SIMPLE },
    { "wxCB_DROPDOWN", wxCB_DROPDOWN },
    { "wxCB_READONLY", wxCB_READONLY },
    { "wxCB_SORT", wxCB_SORT }
};

int g_WindowStylesComboBoxCount = sizeof(g_WindowStylesComboBox)/sizeof(wxWindowStylePair) ;

#if 0
/* wxChoice */
static wxWindowStylePair g_WindowStylesChoice[] = {
};

int g_WindowStylesChoiceCount = sizeof(g_WindowStylesChoice)/sizeof(wxWindowStylePair) ;
#endif

/* wxGauge */
static wxWindowStylePair g_WindowStylesGauge[] = {
    { "wxGA_PROGRESSBAR", wxGA_PROGRESSBAR },
    { "wxGA_HORIZONTAL", wxGA_HORIZONTAL },
    { "wxGA_VERTICAL", wxGA_VERTICAL }
};

int g_WindowStylesGaugeCount = sizeof(g_WindowStylesGauge)/sizeof(wxWindowStylePair) ;

/* wxTextCtrl */
static wxWindowStylePair g_WindowStylesTextCtrl[] = {
    //  { "wxPASSWORD", wxPASSWORD},
    //  { "wxPROCESS_ENTER", wxPROCESS_ENTER},
    { "wxTE_PASSWORD", wxTE_PASSWORD},
    { "wxTE_READONLY", wxTE_READONLY},
    { "wxTE_PROCESS_ENTER", wxTE_PROCESS_ENTER},
    { "wxTE_MULTILINE", wxTE_MULTILINE}
};

int g_WindowStylesTextCtrlCount = sizeof(g_WindowStylesTextCtrl)/sizeof(wxWindowStylePair) ;

/* wxRadioButton */
static wxWindowStylePair g_WindowStylesRadioButton[] = {
    { "wxRB_GROUP", wxRB_GROUP }
};

int g_WindowStylesRadioButtonCount = sizeof(g_WindowStylesRadioButton)/sizeof(wxWindowStylePair) ;

/* wxRadioBox */
static wxWindowStylePair g_WindowStylesRadioBox[] = {
    { "wxRA_SPECIFY_COLS", wxRA_SPECIFY_COLS },
    { "wxRA_SPECIFY_ROWS", wxRA_SPECIFY_ROWS }
    /*
    ,
    { "wxRA_HORIZONTAL", wxRA_HORIZONTAL },
    { "wxRA_VERTICAL", wxRA_VERTICAL }
    */
};

int g_WindowStylesRadioBoxCount = sizeof(g_WindowStylesRadioBox)/sizeof(wxWindowStylePair) ;

/* wxSlider */
static wxWindowStylePair g_WindowStylesSlider[] = {
    { "wxSL_HORIZONTAL", wxSL_HORIZONTAL },
    { "wxSL_VERTICAL", wxSL_VERTICAL },
    { "wxSL_AUTOTICKS", wxSL_AUTOTICKS },
    { "wxSL_LABELS", wxSL_LABELS },
    { "wxSL_LEFT", wxSL_LEFT },
    { "wxSL_TOP", wxSL_TOP },
    { "wxSL_RIGHT", wxSL_RIGHT },
    { "wxSL_BOTTOM", wxSL_BOTTOM },
    { "wxSL_BOTH", wxSL_BOTH },
    { "wxSL_SELRANGE", wxSL_SELRANGE }
};

int g_WindowStylesSliderCount = sizeof(g_WindowStylesSlider)/sizeof(wxWindowStylePair) ;

/* wxScrollBar */
static wxWindowStylePair g_WindowStylesScrollBar[] = {
    { "wxSB_HORIZONTAL", wxSB_HORIZONTAL },
    { "wxSB_VERTICAL", wxSB_VERTICAL }
};

int g_WindowStylesScrollBarCount = sizeof(g_WindowStylesScrollBar)/sizeof(wxWindowStylePair) ;

/* wxButton */
static wxWindowStylePair g_WindowStylesButton[] = {
    { "wxBU_AUTODRAW", wxBU_AUTODRAW },
    { "wxBU_NOAUTODRAW", wxBU_NOAUTODRAW }
};

int g_WindowStylesButtonCount = sizeof(g_WindowStylesButton)/sizeof(wxWindowStylePair) ;

/* wxTreeCtrl */
static wxWindowStylePair g_WindowStylesTreeCtrl[] = {
    { "wxTR_HAS_BUTTONS", wxTR_HAS_BUTTONS },
    { "wxTR_EDIT_LABELS", wxTR_EDIT_LABELS },
    { "wxTR_LINES_AT_ROOT", wxTR_LINES_AT_ROOT }
};

int g_WindowStylesTreeCtrlCount = sizeof(g_WindowStylesTreeCtrl)/sizeof(wxWindowStylePair) ;

/* wxListCtrl */
static wxWindowStylePair g_WindowStylesListCtrl[] = {
    { "wxLC_ICON", wxLC_ICON },
    { "wxLC_SMALL_ICON", wxLC_SMALL_ICON },
    { "wxLC_LIST", wxLC_LIST },
    { "wxLC_REPORT", wxLC_REPORT },
    { "wxLC_ALIGN_TOP", wxLC_ALIGN_TOP },
    { "wxLC_ALIGN_LEFT", wxLC_ALIGN_LEFT },
    { "wxLC_AUTOARRANGE", wxLC_AUTOARRANGE },
    { "wxLC_USER_TEXT", wxLC_USER_TEXT },
    { "wxLC_EDIT_LABELS", wxLC_EDIT_LABELS },
    { "wxLC_NO_HEADER", wxLC_NO_HEADER },
    { "wxLC_NO_SORT_HEADER", wxLC_NO_SORT_HEADER },
    { "wxLC_SINGLE_SEL", wxLC_SINGLE_SEL },
    { "wxLC_SORT_ASCENDING", wxLC_SORT_ASCENDING },
    { "wxLC_SORT_DESCENDING", wxLC_SORT_DESCENDING }
};

int g_WindowStylesListCtrlCount = sizeof(g_WindowStylesListCtrl)/sizeof(wxWindowStylePair) ;

/* wxSpinButton */
static wxWindowStylePair g_WindowStylesSpinButton[] = {
    { "wxSP_VERTICAL", wxSP_VERTICAL},
    { "wxSP_HORIZONTAL", wxSP_HORIZONTAL},
    { "wxSP_ARROW_KEYS", wxSP_ARROW_KEYS},
    { "wxSP_WRAP", wxSP_WRAP}
};

int g_WindowStylesSpinButtonCount = sizeof(g_WindowStylesSpinButton)/sizeof(wxWindowStylePair) ;

/* wxSplitterWindow */
static wxWindowStylePair g_WindowStylesSplitterWindow[] = {
    { "wxSP_NOBORDER", wxSP_NOBORDER},
    { "wxSP_3D", wxSP_3D},
    { "wxSP_BORDER", wxSP_BORDER}
};

int g_WindowStylesSplitterWindowCount = sizeof(g_WindowStylesSplitterWindow)/sizeof(wxWindowStylePair) ;

/* wxTabCtrl */
static wxWindowStylePair g_WindowStylesTabCtrl[] = {
    { "wxTC_MULTILINE", wxTC_MULTILINE},
    { "wxTC_RIGHTJUSTIFY", wxTC_RIGHTJUSTIFY},
    { "wxTC_FIXEDWIDTH", wxTC_FIXEDWIDTH},
    { "wxTC_OWNERDRAW", wxTC_OWNERDRAW}
};

int g_WindowStylesTabCtrlCount = sizeof(g_WindowStylesTabCtrl)/sizeof(wxWindowStylePair) ;

/* wxStatusBar95 */
static wxWindowStylePair g_WindowStylesStatusBar[] = {
    { "wxST_SIZEGRIP", wxST_SIZEGRIP}
};

int g_WindowStylesStatusBarCount = sizeof(g_WindowStylesStatusBar)/sizeof(wxWindowStylePair) ;

/* wxControl */
static wxWindowStylePair g_WindowStylesControl[] = {
    { "wxFIXED_LENGTH", wxFIXED_LENGTH},
    { "wxALIGN_LEFT", wxALIGN_LEFT},
    { "wxALIGN_CENTRE", wxALIGN_CENTRE},
    { "wxALIGN_RIGHT", wxALIGN_RIGHT},
    { "wxCOLOURED", wxCOLOURED}
};

int g_WindowStylesControlCount = sizeof(g_WindowStylesControl)/sizeof(wxWindowStylePair) ;

/* wxToolBar */
static wxWindowStylePair g_WindowStylesToolBar[] = {
    { "wxTB_3DBUTTONS", wxTB_3DBUTTONS},
    { "wxTB_HORIZONTAL", wxTB_HORIZONTAL},
    { "wxTB_VERTICAL", wxTB_VERTICAL},
    { "wxTB_FLAT", wxTB_FLAT}
};

int g_WindowStylesToolBarCount = sizeof(g_WindowStylesToolBar)/sizeof(wxWindowStylePair) ;

/* Frame/dialog */
static wxWindowStylePair g_WindowStylesDialog[] = {
    { "wxSTAY_ON_TOP", wxSTAY_ON_TOP},
    { "wxCAPTION", wxCAPTION},
    { "wxICONIZE", wxICONIZE},
    { "wxMINIMIZE", wxICONIZE},
    { "wxMAXIMIZE", wxMAXIMIZE},
    { "wxTHICK_FRAME", wxTHICK_FRAME},
    { "wxRESIZE_BORDER", wxRESIZE_BORDER},
    { "wxSYSTEM_MENU", wxSYSTEM_MENU},
    { "wxMINIMIZE_BOX", wxMINIMIZE_BOX},
    { "wxMAXIMIZE_BOX", wxMAXIMIZE_BOX},
    { "wxRESIZE_BOX", wxRESIZE_BOX}
};

int g_WindowStylesDialogCount = sizeof(g_WindowStylesDialog)/sizeof(wxWindowStylePair) ;

/* Generic */
static wxWindowStylePair g_WindowStylesWindow[] = {
    { "wxBORDER", wxBORDER},
    { "wxDOUBLE_BORDER", wxDOUBLE_BORDER},
    { "wxSUNKEN_BORDER", wxSUNKEN_BORDER},
    { "wxRAISED_BORDER", wxRAISED_BORDER},
    { "wxSIMPLE_BORDER", wxSIMPLE_BORDER},
    { "wxSTATIC_BORDER", wxSTATIC_BORDER},
    { "wxTRANSPARENT_WINDOW", wxTRANSPARENT_WINDOW},
    { "wxNO_BORDER", wxNO_BORDER},
    { "wxCLIP_CHILDREN", wxCLIP_CHILDREN}
    
    /* Would be duplicated with e.g. wxLB_HSCROLL
    { "wxVSCROLL", wxVSCROLL },
    { "wxHSCROLL", wxHSCROLL }
    */
};

int g_WindowStylesWindowCount = sizeof(g_WindowStylesWindow)/sizeof(wxWindowStylePair) ;

/*
* A table holding all class style objects
*/


wxWindowStyleTable::wxWindowStyleTable():
m_classes(wxKEY_STRING)
{
}

wxWindowStyleTable::~wxWindowStyleTable()
{
    ClearTable();
}

void wxWindowStyleTable::Init()
{
    AddStyles("wxWindow", g_WindowStylesWindowCount, g_WindowStylesWindow);
    AddStyles("wxDialog", g_WindowStylesDialogCount, g_WindowStylesDialog);
    AddStyles("wxButton", g_WindowStylesButtonCount, g_WindowStylesButton);
    AddStyles("wxTextCtrl", g_WindowStylesTextCtrlCount, g_WindowStylesTextCtrl);
    AddStyles("wxSpinButton", g_WindowStylesSpinButtonCount, g_WindowStylesSpinButton);
    AddStyles("wxListBox", g_WindowStylesListBoxCount, g_WindowStylesListBox);
    AddStyles("wxRadioButton", g_WindowStylesRadioButtonCount, g_WindowStylesRadioButton);
    AddStyles("wxRadioBox", g_WindowStylesRadioBoxCount, g_WindowStylesRadioBox);
    AddStyles("wxControl", g_WindowStylesControlCount, g_WindowStylesControl);
    AddStyles("wxListCtrl", g_WindowStylesListCtrlCount, g_WindowStylesListCtrl);
    AddStyles("wxTreeCtrl", g_WindowStylesTreeCtrlCount, g_WindowStylesTreeCtrl);
    AddStyles("wxSlider", g_WindowStylesSliderCount, g_WindowStylesSlider);
    AddStyles("wxGauge", g_WindowStylesGaugeCount, g_WindowStylesGauge);
    AddStyles("wxComboBox", g_WindowStylesComboBoxCount, g_WindowStylesComboBox);
    //    AddStyles("wxChoice", g_WindowStylesChoice, g_WindowStylesChoice);
    AddStyles("wxScrollBar", g_WindowStylesScrollBarCount, g_WindowStylesScrollBar);
}

// Operations
void wxWindowStyleTable::ClearTable()
{
    wxNode* node = m_classes.First();
    while (node)
    {
        wxWindowStyleClass* styleClass = (wxWindowStyleClass*) node->Data();
        delete styleClass;
        node = node->Next();
    }
    m_classes.Clear();
}

void wxWindowStyleTable::AddStyles(const wxString& className, int n, wxWindowStylePair *styles)
{
    wxWindowStyleClass* styleClass = new wxWindowStyleClass(n, styles);
    m_classes.Append(className, styleClass);
}

wxWindowStyleClass* wxWindowStyleTable::FindClass(const wxString& className)
{
    wxNode* node = m_classes.Find(className);
    if (node)
    {
        wxWindowStyleClass* styleClass = (wxWindowStyleClass*) node->Data();
        return styleClass;
    }
    else
        return NULL;
}

bool wxWindowStyleTable::GenerateStyleStrings(const wxString& className, long windowStyle, char *buf)
{
    wxWindowStyleClass* styleClass = FindClass(className);
    if (!styleClass)
        return FALSE;
    
    styleClass->GenerateStyleStrings(windowStyle, buf);
    return TRUE;
}

/*
* Holds all the styles for a particular class
*/

wxWindowStyleClass::wxWindowStyleClass(int n, wxWindowStylePair *styles)
{
    m_styleCount = n;
    m_styles = styles;
    /*
    m_styles = new wxWindowStylePair[n];
    int i;
    for (i = 0; i < n; i++)
    m_styles[i] = styles[i];
    */
}

wxWindowStyleClass::~wxWindowStyleClass()
{
}

void wxWindowStyleClass::GenerateStyleStrings(long windowStyle, char *buf)
{
    int i;
    for (i = 0; i < m_styleCount; i++)
        GenerateStyle(buf, windowStyle, m_styles[i].m_styleId, m_styles[i].m_styleName);
}

bool wxWindowStyleClass::GenerateStyle(char *buf, long windowStyle, long flag, const wxString& strStyle)
{
    // Ignore zero flags
    if (flag == 0)
        return TRUE;
    
    if ((windowStyle & flag) == flag)
    {
        if (strlen(buf) > 0)
            strcat(buf, " | ");
        strcat(buf, (const char*) strStyle);
        return TRUE;
    }
    else
        return FALSE;
}

