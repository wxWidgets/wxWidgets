/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/barbutton.mm
// Purpose:     wxBarButton class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/barbutton.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/brush.h"
#include "wx/panel.h"
#include "wx/bmpbuttn.h"
#include "wx/settings.h"
#include "wx/dcscreen.h"
#include "wx/dcclient.h"
#include "wx/toplevel.h"
#include "wx/image.h"
#endif

#include "wx/osx/private.h"
#include "wx/osx/iphone/private.h"

#include "wx/mstream.h"
#include "wx/arrimpl.cpp"

// FIXME should be moved to a "generic" implementation
WX_DEFINE_EXPORTED_OBJARRAY(wxBarButtonArray);


#include "wx/stockitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxBarButton, wxBarButtonBase)

BEGIN_EVENT_TABLE(wxBarButton, wxBarButtonBase)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

/// Creation function taking a text label.
bool wxBarButton::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& WXUNUSED(validator),
                         const wxString& WXUNUSED(name))
{
    // Create native (Cocoa) object
    m_nativeBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@""
                                                             style:UIBarButtonItemStylePlain
                                                            target:nil
                                                            action:nil];
    
    SetStyle(style);
    SetLabel(label);
    SetParent(parent);
    m_rect = wxRect(pos, size);
    
    return true;
}

/// Creation function taking a bitmap label.
bool wxBarButton::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxBitmap& bitmap,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& WXUNUSED(validator),
                         const wxString& WXUNUSED(name))
{
    // Create native (Cocoa) object
    m_nativeBarButtonItem = [[UIBarButtonItem alloc] initWithImage:nil
                                                             style:UIBarButtonItemStylePlain
                                                            target:nil
                                                            action:nil];

    
    SetStyle(style);
    SetNormalBitmap(bitmap);
    SetParent(parent);
    m_rect = wxRect(pos, size);
    
    return true;
}

wxBarButton::~wxBarButton()
{
    if (m_nativeBarButtonItem) {
        [m_nativeBarButtonItem release];
        m_nativeBarButtonItem = nil;
    }
}

void wxBarButton::Init()
{
    m_id = -1;
    m_enabled = true;
    m_selected = false;
    m_highlighted = false;
    m_imageId = -1;
    m_style = 0;
    m_parent = NULL;
    m_marginX = 0;
    m_marginY = 0;
    
    m_borderColour = NULL;
    m_backgroundColour = NULL;
    m_foregroundColour = NULL;
    m_selectionColour = NULL;
    
    m_nativeBarButtonItem = NULL;
}

void wxBarButton::Copy(const wxBarButton& item)
{
    m_normalBitmap = item.m_normalBitmap;
    m_disabledBitmap = item.m_disabledBitmap;
    m_selectedBitmap = item.m_selectedBitmap;
    m_highlightedBitmap = item.m_highlightedBitmap;
    m_rect = item.m_rect;
    m_label = item.m_label;
    m_badge = item.m_badge;
    m_id = item.m_id;
    m_imageId = item.m_imageId;
    m_enabled = item.m_enabled;
    m_selected = item.m_selected;
    m_highlighted = item.m_highlighted;
    m_style = item.m_style;
    
    m_borderColour = item.m_borderColour;
    m_backgroundColour = item.m_backgroundColour;
    m_foregroundColour = item.m_foregroundColour;
    m_selectionColour = item.m_selectionColour;
    m_borderColour = item.m_borderColour;
    
    m_font = item.m_font;
    m_parent = item.m_parent;
    m_marginX = item.m_marginX;
    m_marginY = item.m_marginY;
    
    m_nativeBarButtonItem = [item.m_nativeBarButtonItem copy];
}

/// Sets the button background colour.
bool wxBarButton::SetBackgroundColour(const wxColour &colour)
{
    UIColor *cocoaColor = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    [m_nativeBarButtonItem setBackgroundColor:cocoaColor];
    
    m_backgroundColour = colour;
    return true;
}

/// Sets the button foreground (text) colour.
bool wxBarButton::SetForegroundColour(const wxColour &colour)
{
    UIColor *cocoaColor = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    [m_nativeBarButtonItem setForegroundColor:cocoaColor];

    m_foregroundColour = colour;
    return true;
}

/// Sets the selection colour.
void wxBarButton::SetSelectionColour(const wxColour& colour)
{
    UIColor *cocoaColor = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    [m_nativeBarButtonItem setSelectedTextColor:cocoaColor];

    m_selectionColour = colour;
}

/// Sets the button font.
void wxBarButton::SetFont(const wxFont& font)
{
    // FIXME TODO
    m_font = font;
}

/// Sets the button border colour.
void wxBarButton::SetBorderColour(const wxColour &colour)
{
    // FIXME TODO
    m_borderColour = colour;
}

/// Sets the size of the button.
void wxBarButton::SetSize(const wxSize& sz)
{
    // FIXME TODO
    m_rect.SetSize(sz);
}

/// Sets the position of the button.
void wxBarButton::SetPosition(const wxPoint& pt)
{
    // FIXME TODO
    m_rect.SetPosition(pt);
}

/// Sets the normal state bitmap.
void wxBarButton::SetNormalBitmap(const wxBitmap& bitmap)
{
    UIImage *cocoaImage = [bitmap.GetUIImage() retain];

    [m_nativeBarButtonItem setImage:cocoaImage];
    
    m_normalBitmap = bitmap;
}

/// Sets the disabled state bitmap.
void wxBarButton::SetDisabledBitmap(const wxBitmap& bitmap)
{
    // FIXME TODO
    m_disabledBitmap = bitmap;
}

/// Sets the selected state bitmap.
void wxBarButton::SetSelectedBitmap(const wxBitmap& bitmap)
{
    // FIXME TODO
    m_selectedBitmap = bitmap;
}

/// Sets the highlighted state bitmap.
void wxBarButton::SetHighlightedBitmap(const wxBitmap& bitmap)
{
    // FIXME TODO
    m_highlightedBitmap = bitmap;
}

/// Sets the text label.
void wxBarButton::SetLabel(const wxString& label)
{
    [m_nativeBarButtonItem setTitle:[NSString stringWithString:wxCFStringRef(label).AsNSString()]];
    
    m_label = label;
}

// Sets the horizontal margin between button edge and content.
void wxBarButton::SetMarginX(int margin)
{
    UIEdgeInsets imageInsets = [m_nativeBarButtonItem imageInsets];
    imageInsets.top = margin;
    imageInsets.bottom = margin;
    [m_nativeBarButtonItem setImageInsets:imageInsets];
    
    m_marginX = margin;
}

// Sets the vertical margin between button edge content.
void wxBarButton::SetMarginY(int margin)
{
    UIEdgeInsets imageInsets = [m_nativeBarButtonItem imageInsets];
    imageInsets.left = margin;
    imageInsets.right = margin;
    [m_nativeBarButtonItem setImageInsets:imageInsets];

    m_marginY = margin;
}

// Sets the enabled flag.
void wxBarButton::SetEnabled(bool enabled)
{
    [m_nativeBarButtonItem setEnabled:enabled];
    
    m_enabled = enabled;
}

// Sets the selected flag.
void wxBarButton::SetSelected(bool selected)
{
    // FIXME TODO (maybe remove altogether)
    
    m_selected = selected;
}

// Sets the highlighted flag.
void wxBarButton::SetHighlighted(bool highlighted)
{
    // FIXME TODO (maybe remove altogether)

    m_highlighted = highlighted;
}

// Sets the button style.
void wxBarButton::SetStyle(int style)
{
    UIBarButtonItemStyle itemStyle = UIBarButtonItemStylePlain;
    if (style & wxBBU_BORDERED) {
        itemStyle = UIBarButtonItemStyleBordered;
    }
    if (style & wxBBU_DONE) {
        itemStyle = UIBarButtonItemStyleDone;
    }
    
    [m_nativeBarButtonItem setStyle:itemStyle];
    
    m_style = style;
}
