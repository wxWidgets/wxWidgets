/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/bmpcboxcmn.cpp
// Purpose:     wxBitmapComboBox
// Author:      Jaakko Salli
// Created:     2008-04-09
// Copyright:   (c) 2008 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#include "wx/bmpcbox.h"

#if wxUSE_BITMAPCOMBOBOX

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dc.h"
    #include "wx/ctrlsub.h"
#endif

#include "wx/settings.h"

// For wxODCB_XXX flags
#include "wx/odcombo.h"


const char wxBitmapComboBoxNameStr[] = "bitmapComboBox";

#if defined(wxBITMAPCOMBOBOX_OWNERDRAWN_BASED)

#define IMAGE_SPACING_RIGHT         4  // Space left of image

#define IMAGE_SPACING_LEFT          4  // Space right of image, left of text

#define EXTRA_FONT_HEIGHT           0  // Add to increase min. height of list items

#define wxBCB_DEFAULT_ITEM_HEIGHT  13


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void wxBitmapComboBoxBase::Init()
{
    m_fontHeight = 0;
    m_imgAreaWidth = 0;
    m_indent = 0;
    m_usedImgSize = wxDefaultSize;
}

void wxBitmapComboBoxBase::UpdateInternals()
{
    m_fontHeight = GetControl()->GetCharHeight()
        + GetControl()->FromDIP(EXTRA_FONT_HEIGHT);

    while ( m_bitmapbundles.size() < GetItemContainer()->GetCount() )
        m_bitmapbundles.push_back( wxBitmapBundle() );

    if ( m_usedImgSize.x != -1 && !m_bitmapbundles.empty() )
    {
        m_usedImgSize = wxBitmapBundle::GetConsensusSizeFor
                        (
                            GetControl(),
                            m_bitmapbundles
                        );
    }
}

// ----------------------------------------------------------------------------
// Item manipulation
// ----------------------------------------------------------------------------

void wxBitmapComboBoxBase::DoSetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( n < m_bitmapbundles.size(), "invalid item index" );
    m_bitmapbundles.at(n) = bitmap;
}

wxBitmap wxBitmapComboBoxBase::GetItemBitmap(unsigned int n) const
{
    wxCHECK_MSG( n < m_bitmapbundles.size(), wxNullBitmap, "invalid item index" );
    return m_bitmapbundles.at(n).GetBitmapFor(
        const_cast<wxBitmapComboBoxBase*>(this)->GetControl()
    );
}

// ----------------------------------------------------------------------------
// wxItemContainer methods
// ----------------------------------------------------------------------------

void wxBitmapComboBoxBase::BCBDoClear()
{
    m_bitmapbundles.clear();

    m_usedImgSize = wxDefaultSize;

    DetermineIndent();
}

void wxBitmapComboBoxBase::BCBDoDeleteOneItem(unsigned int n)
{
    if ( n < m_bitmapbundles.size() )
    {
        m_bitmapbundles.erase(m_bitmapbundles.begin() + n);
    }
}

// ----------------------------------------------------------------------------
// Preparation and Calculations
// ----------------------------------------------------------------------------

bool wxBitmapComboBoxBase::OnAddBitmap(const wxBitmapBundle& bitmap)
{
    if ( bitmap.IsOk() )
    {
        wxSize bmpDefaultSize = bitmap.GetPreferredLogicalSizeFor(GetControl());

        if ( m_usedImgSize.x < 0 )
        {
            // If size not yet determined, get it from this image.
            m_usedImgSize = bmpDefaultSize;

            // Adjust control size to vertically fit the bitmap
            wxWindow* ctrl = GetControl();
            ctrl->InvalidateBestSize();
            wxSize newSz = ctrl->GetBestSize();
            wxSize sz = ctrl->GetSize();
            if ( newSz.y > sz.y )
                ctrl->SetSize(sz.x, newSz.y);
            else
                DetermineIndent();
        }

        wxCHECK_MSG( bmpDefaultSize == m_usedImgSize,
                     false,
                     "you can only add images of same size" );

        return true;
    }

    return false;
}

int wxBitmapComboBoxBase::DetermineIndent()
{
    // Recalculate amount of empty space needed in front of
    // text in control itself.
    int indent = m_imgAreaWidth = 0;

    if ( m_usedImgSize.x > 0 )
    {
        indent = m_usedImgSize.x
            + GetControl()->FromDIP(IMAGE_SPACING_LEFT)
            + GetControl()->FromDIP(IMAGE_SPACING_RIGHT);
        m_imgAreaWidth = indent;

        indent -= 3;
    }

    return indent;
}

// ----------------------------------------------------------------------------
// Item drawing and measuring
// ----------------------------------------------------------------------------

void wxBitmapComboBoxBase::DrawBackground(wxDC& dc,
                                          const wxRect& rect,
                                          int WXUNUSED(item),
                                          int flags) const
{
    if ( flags & wxODCB_PAINTING_SELECTED )
    {
        const int vSizeDec = 0;  // Vertical size reduction of selection rectangle edges

        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

        wxColour selCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        dc.SetPen(selCol);
        dc.SetBrush(selCol);
        dc.DrawRectangle(rect.x,
                         rect.y+vSizeDec,
                         rect.width,
                         rect.height-(vSizeDec*2));
    }
    else
    {
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

        wxColour selCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        dc.SetPen(selCol);
        dc.SetBrush(selCol);
        dc.DrawRectangle(rect);
    }
}

void wxBitmapComboBoxBase::DrawItem(wxDC& dc,
                                    const wxRect& rect,
                                    int item,
                                    const wxString& text,
                                    int WXUNUSED(flags)) const
{
    const wxBitmapBundle& bb = m_bitmapbundles.at(item);
    if ( bb.IsOk() )
    {
        const wxWindow* win = const_cast<wxBitmapComboBoxBase*>(this)->GetControl();
        wxBitmap bmp = bb.GetBitmapFor(win);

        wxCoord w = bmp.GetLogicalWidth();
        wxCoord h = bmp.GetLogicalHeight();

        const int imgSpacingLeft = win->FromDIP(IMAGE_SPACING_LEFT);

        // Draw the image centered
        dc.DrawBitmap(bmp,
                      rect.x + (m_usedImgSize.x-w)/2 + imgSpacingLeft,
                      rect.y + (rect.height-h)/2,
                      true);
    }

    if ( !text.empty() )
        dc.DrawText(text,
                    rect.x + m_imgAreaWidth + 1,
                    rect.y + (rect.height-dc.GetCharHeight())/2);
}

wxCoord wxBitmapComboBoxBase::MeasureItem(size_t WXUNUSED(item)) const
{
    if ( m_usedImgSize.y >= 0 )
    {
        int imgHeightArea = m_usedImgSize.y + 2;
        return imgHeightArea > m_fontHeight ? imgHeightArea : m_fontHeight;
    }

    const wxWindow* win = const_cast<wxBitmapComboBoxBase*>(this)->GetControl();
    return win->FromDIP(wxBCB_DEFAULT_ITEM_HEIGHT);
}

#endif // wxBITMAPCOMBOBOX_OWNERDRAWN_BASED

#endif // wxUSE_BITMAPCOMBOBOX
