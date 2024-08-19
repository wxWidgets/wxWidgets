/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/property.cpp
// Purpose:     wxPGProperty and related support classes
// Author:      Jaakko Salli
// Created:     2008-08-23
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/log.h"
#endif

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/property.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/private.h"

#if wxPG_USE_RENDERER_NATIVE
#include "wx/renderer.h"
#endif

#include <array>

#define PWC_CHILD_SUMMARY_LIMIT         16 // Maximum number of children summarized in a parent property's
                                           // value field.

#define PWC_CHILD_SUMMARY_CHAR_LIMIT    64 // Character limit of summary field when not editing

// -----------------------------------------------------------------------

static void wxPGDrawFocusRect(wxWindow *win, wxDC& dc,
                              int x, int y, int w, int h)
{
    wxRect rect(x, y+((h-dc.GetCharHeight())/2), w, h);
    // In 3.0 comaptibilty mode we always need to use custom renderer because
    // native renderer for this port requires a reference to the valid window
    // to be drawn which is not passed to DrawCaptionSelectionRect function.
#if wxPG_USE_RENDERER_NATIVE && !WXWIN_COMPATIBILITY_3_0
    wxASSERT_MSG( win, wxS("Invalid window to be drawn") );
    wxRendererNative::Get().DrawFocusRect(win, dc, rect);
#else
    wxUnusedVar(win);

    dc.SetPen(wxPen(*wxBLACK,1,wxPENSTYLE_DOT));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect);
#endif // wxPG_USE_RENDERER_NATIVE/!wxPG_USE_RENDERER_NATIVE
}

// -----------------------------------------------------------------------
// wxPGCellRenderer
// -----------------------------------------------------------------------

wxSize wxPGCellRenderer::GetImageSize( const wxPGProperty* WXUNUSED(property),
                                       int WXUNUSED(column),
                                       int WXUNUSED(item) ) const
{
     return wxSize(0, 0);
}

void wxPGCellRenderer::DrawText( wxDC& dc, const wxRect& rect,
                                 int xOffset, const wxString& text ) const
{
    dc.DrawText( text,
                 rect.x+xOffset+wxPG_XBEFORETEXT,
                 rect.y+((rect.height-dc.GetCharHeight())/2) );
}

void wxPGCellRenderer::DrawEditorValue( wxDC& dc, const wxRect& rect,
                                        int xOffset, const wxString& text,
                                        wxPGProperty* property,
                                        const wxPGEditor* editor ) const
{
    int yOffset = ((rect.height-dc.GetCharHeight())/2);

    if ( editor )
    {
        wxRect rect2(rect);
        rect2.Offset(xOffset, yOffset);
        rect2.height -= yOffset * 2;
        editor->DrawValue( dc, rect2, property, text );
    }
    else
    {
        dc.DrawText( text,
                     rect.x+xOffset+wxPG_XBEFORETEXT,
                     rect.y+yOffset );
    }
}

#if WXWIN_COMPATIBILITY_3_0

// This is ugly, but we need to know whether our version of this function was
// called or not, so we set this global variable to true before calling it and
// then reset it to false if the base class version was used.
//
// To make things even worse, we also use this variable to check if we're
// called from our own code (in which case we just need to reset it) or from
// the application (in which case we actually need to draw something).
namespace
{
bool wxPGCellRendererDrawCaptionSelectionRectFlag = false;
}

void wxPGCellRenderer::DrawCaptionSelectionRect( wxDC& dc,
                                       int x, int y,
                                       int w, int h ) const
{
    if ( wxPGCellRendererDrawCaptionSelectionRectFlag )
        wxPGCellRendererDrawCaptionSelectionRectFlag = false;
    else
        wxPGDrawFocusRect(nullptr, dc, x, y, w, h);
}
#endif // WXWIN_COMPATIBILITY_3_0

void wxPGCellRenderer::DrawCaptionSelectionRect(wxWindow* win, wxDC& dc,
                                                int x, int y, int w, int h) const
{
    wxPGDrawFocusRect(win, dc, x, y, w, h);
}

int wxPGCellRenderer::PreDrawCell( wxDC& dc, const wxRect& rect, const wxPropertyGrid* propGrid, const wxPGCell& cell, int flags ) const
{
    int imageWidth = 0;

    // If possible, use cell colours
    if ( !(flags & DontUseCellBgCol) )
    {
        const wxColour& bgCol = cell.GetBgCol();
        dc.SetPen(bgCol);
        dc.SetBrush(bgCol);
    }

    if ( !(flags & DontUseCellFgCol) )
    {
        dc.SetTextForeground(cell.GetFgCol());
    }

    // Draw Background, but only if not rendering in control
    // (as control already has rendered correct background).
    if ( !(flags & (Control|ChoicePopup)) )
        dc.DrawRectangle(rect);

    // Use cell font, if provided
    const wxFont& font = cell.GetFont();
    if ( font.IsOk() )
        dc.SetFont(font);

    wxBitmap bmp = cell.GetBitmap().GetBitmapFor(propGrid);
    if ( bmp.IsOk() )
    {
        int hMax = rect.height - wxPG_CUSTOM_IMAGE_SPACINGY;
        wxBitmap scaledBmp;
        int yOfs;
        if ( bmp.GetHeight() <= hMax )
        {
            scaledBmp = bmp;
            yOfs = (hMax - bmp.GetHeight()) / 2;
        }
        else
        {
            double scale = (double)hMax / bmp.GetHeight();
            scaledBmp = wxPropertyGrid::RescaleBitmap(bmp, scale, scale);
            yOfs = 0;
        }

        dc.DrawBitmap( scaledBmp,
                       rect.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                       rect.y + wxPG_CUSTOM_IMAGE_SPACINGY + yOfs,
                       true );
        imageWidth = scaledBmp.GetWidth();
    }

    return imageWidth;
}

void wxPGCellRenderer::PostDrawCell( wxDC& dc,
                                     const wxPropertyGrid* propGrid,
                                     const wxPGCell& cell,
                                     int WXUNUSED(flags) ) const
{
    // Revert font
    const wxFont& font = cell.GetFont();
    if ( font.IsOk() )
        dc.SetFont(propGrid->GetFont());
}

// -----------------------------------------------------------------------
// wxPGDefaultRenderer
// -----------------------------------------------------------------------

bool wxPGDefaultRenderer::Render( wxDC& dc, const wxRect& rect,
                                  const wxPropertyGrid* propertyGrid, wxPGProperty* property,
                                  int column, int item, int flags ) const
{
    const wxPGEditor* editor = nullptr;

    wxString text;
    bool isUnspecified = property->IsValueUnspecified();
    int selItem = item;

    if ( column == 1 && item == -1 )
    {
        int cmnVal = property->GetCommonValue();
        if ( cmnVal >= 0 )
        {
            // Common Value
            if ( !isUnspecified )
            {
                text = propertyGrid->GetCommonValueLabel(cmnVal);
                DrawText( dc, rect, 0, text );
                if ( !text.empty() )
                    return true;
            }
            return false;
        }
        // We need to know the current selection to override default
        // cell settings (colours, etc.) with custom cell settings
        // which can be defined separately for any single choice item.
        selItem = property->GetChoiceSelection();
    }

    int imageWidth = 0;
    int preDrawFlags = flags;
    bool res = false;
    wxPGCell cell;

    property->GetDisplayInfo(column, selItem, flags, &text, &cell);

    // Property image takes precedence over cell image
    if ( column == 1 && !isUnspecified && property->GetValueImage() )
        cell.SetBitmap(wxBitmapBundle());

    imageWidth = PreDrawCell( dc, rect, propertyGrid, cell, preDrawFlags );

    if ( column == 1 )
    {
        editor = property->GetColumnEditor(column);

        if ( !isUnspecified )
        {
            // Regular property value

            wxSize imageSize = propertyGrid->GetImageSize(property, item);

            wxPGPaintData paintdata;
            paintdata.m_parent = propertyGrid;
            paintdata.m_choiceItem = item;

            if ( imageSize.x > 0 )
            {
                wxRect imageRect(rect.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                                 rect.y+wxPG_CUSTOM_IMAGE_SPACINGY,
                                 wxPG_CUSTOM_IMAGE_WIDTH,
                                 rect.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                dc.SetPen( wxPen(propertyGrid->GetCellTextColour(), 1, wxPENSTYLE_SOLID) );

                paintdata.m_drawnWidth = imageSize.x;
                paintdata.m_drawnHeight = imageSize.y;

                property->OnCustomPaint( dc, imageRect, paintdata );

                imageWidth = paintdata.m_drawnWidth;
            }

#if WXWIN_COMPATIBILITY_3_2
            // Special implementation with check if user-overriden obsolete function is still in use
            text = property->GetValueAsStringWithCheck();
#else
            text = property->GetValueAsString();
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2

            // Add units string?
            if ( propertyGrid->GetColumnCount() <= 2 )
            {
                wxString unitsString = property->GetAttribute(wxPG_ATTR_UNITS, wxString());
                if ( !unitsString.empty() )
                    text = wxString::Format(wxS("%s %s"), text, unitsString );
            }
        }

        if ( text.empty() )
        {
            text = property->GetHintText();
            if ( !text.empty() )
            {
                res = true;

                const wxColour& hCol =
                    propertyGrid->GetCellDisabledTextColour();
                dc.SetTextForeground(hCol);

                // Must make the editor nullptr to override its own rendering
                // code.
                editor = nullptr;
            }
        }
        else
        {
            res = true;
        }
    }

    int imageOffset = property->GetImageOffset(imageWidth);

    DrawEditorValue( dc, rect, imageOffset, text, property, editor );

    // active caption gets nice dotted rectangle
    if ( property->IsCategory() && column == 0 )
    {
        if ( flags & Selected )
        {
            if ( imageOffset > 0 )
            {
                imageOffset -= DEFAULT_IMAGE_OFFSET_INCREMENT;
                imageOffset += wxCC_CUSTOM_IMAGE_MARGIN2 + 4;
            }

            const wxRect rectCaption( rect.x+wxPG_XBEFORETEXT-wxPG_CAPRECTXMARGIN+imageOffset,
                                      rect.y-wxPG_CAPRECTYMARGIN+1,
                                      ((wxPropertyCategory*)property)->GetTextExtent(propertyGrid,
                                                                                     propertyGrid->GetCaptionFont())
                                      +(wxPG_CAPRECTXMARGIN*2),
                                      propertyGrid->GetFontHeight()+(wxPG_CAPRECTYMARGIN*2) );

#if WXWIN_COMPATIBILITY_3_0
            wxPGCellRendererDrawCaptionSelectionRectFlag = true;
            DrawCaptionSelectionRect( dc,
                                      rectCaption.x, rectCaption.y,
                                      rectCaption.width, rectCaption.height );
            if ( wxPGCellRendererDrawCaptionSelectionRectFlag )
            {
                // This means that the user-defined overridden version of the
                // function was called -- because the base class one was not.
                // So just reset the flag and don't do anything else.
                wxPGCellRendererDrawCaptionSelectionRectFlag = false;
            }
            else
                // This means that our own version was called, so we now need
                // to call the other overload, to use the user-defined version
                // if any or to just draw the rectangle in the base class
                // version otherwise.
#endif // WXWIN_COMPATIBILITY_3_0
            DrawCaptionSelectionRect( const_cast<wxPropertyGrid*>(propertyGrid), dc,
                                      rectCaption.x, rectCaption.y,
                                      rectCaption.width, rectCaption.height );
        }
    }

    PostDrawCell(dc, propertyGrid, cell, preDrawFlags);

    return res;
}

wxSize wxPGDefaultRenderer::GetImageSize( const wxPGProperty* property,
                                          int column,
                                          int item ) const
{
    if ( property && column == 1 )
    {
        if ( item == -1 )
        {
            wxBitmap* bmp = property->GetValueImage();

            if ( bmp && bmp->IsOk() )
                return bmp->GetSize();
        }
    }
    return wxSize(0,0);
}

// -----------------------------------------------------------------------
// wxPGCellData
// -----------------------------------------------------------------------

wxPGCellData::wxPGCellData()
    : wxObjectRefData()
    , m_hasValidText(false)
{
}

// -----------------------------------------------------------------------
// wxPGCell
// -----------------------------------------------------------------------

wxPGCell::wxPGCell( const wxString& text,
                    const wxBitmapBundle& bitmap,
                    const wxColour& fgCol,
                    const wxColour& bgCol )
    : wxObject()
{
    wxPGCellData* data = new wxPGCellData();
    m_refData = data;
    data->m_text = text;
    data->m_bitmapBundle = bitmap;
    data->m_fgCol = fgCol;
    data->m_bgCol = bgCol;
    data->m_hasValidText = true;
}

wxObjectRefData *wxPGCell::CloneRefData( const wxObjectRefData *data ) const
{
    wxPGCellData* c = new wxPGCellData();
    const wxPGCellData* o = (const wxPGCellData*) data;
    c->m_text = o->m_text;
    c->m_bitmapBundle = o->m_bitmapBundle;
    c->m_fgCol = o->m_fgCol;
    c->m_bgCol = o->m_bgCol;
    c->m_hasValidText = o->m_hasValidText;
    return c;
}

void wxPGCell::SetText( const wxString& text )
{
    AllocExclusive();

    GetData()->SetText(text);
}

void wxPGCell::SetBitmap( const wxBitmapBundle& bitmap )
{
    AllocExclusive();

    GetData()->SetBitmap(bitmap);
}

void wxPGCell::SetFgCol( const wxColour& col )
{
    AllocExclusive();

    GetData()->SetFgCol(col);
}

void wxPGCell::SetFont( const wxFont& font )
{
    AllocExclusive();

    GetData()->SetFont(font);
}

void wxPGCell::SetBgCol( const wxColour& col )
{
    AllocExclusive();

    GetData()->SetBgCol(col);
}

void wxPGCell::MergeFrom( const wxPGCell& srcCell )
{
    AllocExclusive();

    wxPGCellData* data = GetData();

    if ( srcCell.HasText() )
        data->SetText(srcCell.GetText());

    if ( srcCell.GetFgCol().IsOk() )
        data->SetFgCol(srcCell.GetFgCol());

    if ( srcCell.GetBgCol().IsOk() )
        data->SetBgCol(srcCell.GetBgCol());

    if ( srcCell.GetBitmap().IsOk() )
        data->SetBitmap(srcCell.GetBitmap());
}

void wxPGCell::SetEmptyData()
{
    AllocExclusive();
}

// -----------------------------------------------------------------------
// wxPGChoiceEntry
// -----------------------------------------------------------------------

wxPGChoiceEntry::wxPGChoiceEntry()
    : wxPGCell()
    , m_value(wxPG_INVALID_VALUE)
{
}

wxPGChoiceEntry::wxPGChoiceEntry(const wxPGChoiceEntry& other)
    : wxPGCell(other)
    , m_value(other.m_value)
{
}

wxPGChoiceEntry::wxPGChoiceEntry(const wxString& label, int value)
    : wxPGCell()
    , m_value(value)
{
    SetText(label);
}

wxPGChoiceEntry& wxPGChoiceEntry::operator=(const wxPGChoiceEntry& other)
{
    if ( this != &other )
    {
        Ref(other);
    }
    m_value = other.m_value;
    return *this;
}

// -----------------------------------------------------------------------
// wxPGChoicesData
// -----------------------------------------------------------------------

wxPGChoicesData::~wxPGChoicesData()
{
    Clear();
}

void wxPGChoicesData::Clear()
{
    m_items.clear();
}

void wxPGChoicesData::CopyDataFrom(wxPGChoicesData* data)
{
    wxASSERT(m_items.empty());

    m_items = data->m_items;
}

wxPGChoiceEntry& wxPGChoicesData::Insert(int index,
    const wxPGChoiceEntry& item)
{
    std::vector<wxPGChoiceEntry>::iterator it;
    if ( index == -1 )
    {
        it = m_items.end();
        index = (int)m_items.size();
    }
    else
    {
        it = m_items.begin() + index;
    }

    m_items.insert(it, item);

    wxPGChoiceEntry& ownEntry = m_items[index];

    // Need to fix value?
    if ( ownEntry.GetValue() == wxPG_INVALID_VALUE )
        ownEntry.SetValue(index);

    return ownEntry;
}

// -----------------------------------------------------------------------
// wxPGProperty
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPGProperty, wxObject);

#if WXWIN_COMPATIBILITY_3_2
wxString* wxPGProperty::sm_wxPG_LABEL = nullptr;
#endif // WXWIN_COMPATIBILITY_3_2
const wxString wxPGProperty::sm_labelItem("@!");

void wxPGProperty::Init()
{
    m_commonValue = -1;
    m_arrIndex = 0xFFFF;
    m_parent = nullptr;

    m_parentState = nullptr;

    m_clientData = nullptr;
    m_clientObject = nullptr;

    m_customEditor = nullptr;
#if wxUSE_VALIDATORS
    m_validator = nullptr;
#endif

    m_maxLen = 0; // infinite maximum length

    m_flags = wxPGPropertyFlags::Property;

    m_depth = 1;

    SetExpanded(true);
}


void wxPGProperty::Init( const wxString& label, const wxString& name )
{
    if ( label != wxPG_LABEL )
        m_label = label;

    if ( name != wxPG_LABEL )
        DoSetName( name );
    else
        DoSetName( m_label );

    Init();
}

void wxPGProperty::InitAfterAdded( wxPropertyGridPageState* pageState,
                                   wxPropertyGrid* propgrid )
{
    //
    // Called after property has been added to grid or page
    // (so propgrid can be null, too).

    wxPGProperty* parent = m_parent;
    bool parentIsRoot = wxDynamicCast(parent, wxPGRootProperty) != nullptr;

    //
    // Convert invalid cells to default ones in this grid
    for ( wxPGCell& cell : m_cells )
    {
        if ( cell.IsInvalid() )
        {
            cell = IsCategory() ? propgrid->GetCategoryDefaultCell()
                                : propgrid->GetPropertyDefaultCell();
        }
    }

    m_parentState = pageState;

    if ( !parentIsRoot && !parent->IsCategory() )
    {
        m_cells = parent->m_cells;
    }

    // If in hideable adding mode, or if assigned parent is hideable, then
    // make this one hideable.
    if (
         ( !parentIsRoot && parent->HasFlag(wxPGPropertyFlags::Hidden) ) ||
         ( propgrid && (propgrid->HasInternalFlag(wxPropertyGrid::wxPG_FL_ADDING_HIDEABLES)) )
       )
        SetFlag(wxPGPropertyFlags::Hidden);

    // Set custom image flag.
    int custImgHeight = OnMeasureImage().y;
    if ( custImgHeight == wxDefaultCoord )
    {
        SetFlag(wxPGPropertyFlags::CustomImage);
    }

    if ( propgrid && (propgrid->HasFlag(wxPG_LIMITED_EDITING)) )
        SetFlag(wxPGPropertyFlags::NoEditor);

    // Make sure parent has some parental flags
    if ( !parent->HasFlag(wxPGPropertyFlags::ParentalFlags) )
        parent->SetParentalType(wxPGPropertyFlags::MiscParent);

    if ( !IsCategory() )
    {
        // This is not a category.

        // Depth.
        //
        unsigned char depth = 1;
        if ( !parentIsRoot )
        {
            depth = parent->GetDepth();
            if ( !parent->IsCategory() )
                depth++;
        }
        m_depth = depth;
        unsigned char greyDepth = depth;

        if ( !parentIsRoot )
        {
            wxPropertyCategory* pc;

            if ( parent->IsCategory() )
                pc = (wxPropertyCategory* ) parent;
            else
                // This conditional compile is necessary to
                // bypass some compiler bug.
                pc = pageState->GetPropertyCategory(parent);

            if ( pc )
                greyDepth = pc->GetDepth();
            else
                greyDepth = parent->m_depthBgCol;
        }

        m_depthBgCol = greyDepth;
    }
    else
    {
        // This is a category.

        // depth
        unsigned char depth = 1;
        if ( !parentIsRoot )
        {
            depth = parent->GetDepth() + 1;
        }
        m_depth = depth;
        m_depthBgCol = depth;
    }

    //
    // Has initial children
    if ( HasAnyChild() )
    {
        // Check parental flags
        wxASSERT_MSG( ((m_flags & wxPGPropertyFlags::ParentalFlags) ==
                            wxPGPropertyFlags::Aggregate) ||
                      ((m_flags & wxPGPropertyFlags::ParentalFlags) ==
                            wxPGPropertyFlags::MiscParent),
                      wxS("wxPGProperty parental flags set incorrectly at ")
                      wxS("this time") );

        if ( HasFlag(wxPGPropertyFlags::Aggregate) )
        {
            // Properties with private children are not expanded by default.
            SetExpanded(false);
        }
        else if ( propgrid && propgrid->HasFlag(wxPG_HIDE_MARGIN) )
        {
            // ...unless it cannot be expanded by user and therefore must
            // remain visible at all times
            SetExpanded(true);
        }

        //
        // Prepare children recursively
        for ( wxPGProperty* child : m_children )
        {
            child->InitAfterAdded(pageState, pageState->GetGrid());
        }

        if ( propgrid && propgrid->HasExtraStyle(wxPG_EX_AUTO_UNSPECIFIED_VALUES) )
            SetFlagRecursively(wxPGPropertyFlags::AutoUnspecified, true);
    }
}

void wxPGProperty::OnDetached(wxPropertyGridPageState* WXUNUSED(state),
                              wxPropertyGrid* propgrid)
{
    if ( propgrid )
    {
        const wxPGCell& propDefCell = propgrid->GetPropertyDefaultCell();
        const wxPGCell& catDefCell = propgrid->GetCategoryDefaultCell();

        // Make default cells invalid
        for ( wxPGCell& cell : m_cells )
        {
            if ( cell.IsSameAs(propDefCell) ||
                 cell.IsSameAs(catDefCell) )
            {
                cell.UnRef();
            }
        }
    }
}

wxPGProperty::wxPGProperty()
    : wxObject()
{
    Init();
}


wxPGProperty::wxPGProperty( const wxString& label, const wxString& name )
    : wxObject()
{
    Init( label, name );
}


wxPGProperty::~wxPGProperty()
{
    delete m_clientObject;

    Empty();  // this deletes items

#if wxUSE_VALIDATORS
    delete m_validator;
#endif

    // This makes it easier for us to detect dangling pointers
    m_parent = nullptr;
}


bool wxPGProperty::IsSomeParent( wxPGProperty* candidate ) const
{
    wxPGProperty* parent = m_parent;
    do
    {
        if ( parent == candidate )
            return true;
        parent = parent->m_parent;
    } while ( parent );
    return false;
}

void wxPGProperty::SetLabel(const wxString& label)
{
    m_label = label;
    // Update cell text if possible
    if ( HasCell(0) )
    {
        wxPGCell& cell = GetCell(0);
        if ( cell.HasText() )
        {
            cell.SetText(label);
        }
    }
}

void wxPGProperty::SetName( const wxString& newName )
{
    wxPropertyGrid* pg = GetGrid();

    if ( pg )
        pg->SetPropertyName(this, newName);
    else
        DoSetName(newName);
}

wxString wxPGProperty::GetName() const
{
    wxPGProperty* parent = GetParent();

    if ( m_name.empty() || !parent || parent->IsCategory() || parent->IsRoot() )
        return m_name;

    return m_parent->GetName() + wxS(".") + m_name;
}

wxPropertyGrid* wxPGProperty::GetGrid() const
{
    return m_parentState ? m_parentState->GetGrid() : nullptr;
}

int wxPGProperty::Index( const wxPGProperty* p ) const
{
    auto it = std::find(m_children.begin(), m_children.end(), p);
    return it != m_children.end() ? (int)(it - m_children.begin()) : wxNOT_FOUND;
}

bool wxPGProperty::ValidateValue( wxVariant& WXUNUSED(value), wxPGValidationInfo& WXUNUSED(validationInfo) ) const
{
    return true;
}

void wxPGProperty::OnSetValue()
{
}

void wxPGProperty::RefreshChildren ()
{
}

void wxPGProperty::OnValidationFailure( wxVariant& WXUNUSED(pendingValue) )
{
}

#if WXWIN_COMPATIBILITY_3_0
void wxPGProperty::GetDisplayInfo( unsigned int column,
                                   int choiceIndex,
                                   int flags,
                                   wxString* pString,
                                   const wxPGCell** pCell )
{
    wxASSERT_MSG(!pCell || !(*pCell),
          wxS("Cell pointer is a dummy argument and shouldn't be used"));
    wxUnusedVar(pCell);
    GetDisplayInfo(column, choiceIndex, flags, pString, (wxPGCell*)nullptr);
}
#endif // WXWIN_COMPATIBILITY_3_0

void wxPGProperty::GetDisplayInfo( unsigned int column,
                                   int choiceIndex,
                                   int flags,
                                   wxString* pString,
                                   wxPGCell* pCell )
{
    wxCHECK_RET( GetGrid(),
                 wxS("Cannot obtain display info for detached property") );

    // Get default cell
    wxPGCell cell = GetCell(column);

    if ( !(flags & wxPGCellRenderer::ChoicePopup) )
    {
        // Not painting list of choice popups, so get text from property
        if ( column != 1 || !IsValueUnspecified() || IsCategory() )
        {
            // Override default cell settings with
            // custom settings defined for choice item.
            if (column == 1 && !IsValueUnspecified() && choiceIndex != wxNOT_FOUND)
            {
                const wxPGChoiceEntry& entry = m_choices[choiceIndex];
                cell.MergeFrom(entry);
            }
        }
        else
        {
            // Use special unspecified value cell
            cell.MergeFrom(GetGrid()->GetUnspecifiedValueAppearance());
        }

        if ( cell.HasText() )
        {
            *pString = cell.GetText();
        }
        else
        {
            if ( column == 0 )
                *pString = GetLabel();
            else if ( column == 1 )
                *pString = GetDisplayedString();
            else if ( column == 2 )
                *pString = GetAttribute(wxPG_ATTR_UNITS, wxString());
        }
    }
    else
    {
        wxASSERT( column == 1 );

        if ( choiceIndex != wxNOT_FOUND )
        {
            // Override default cell settings with
            // custom settings defined for choice item.
            const wxPGChoiceEntry& entry = m_choices[choiceIndex];
            cell.MergeFrom(entry);

            *pString = m_choices.GetLabel(choiceIndex);
        }
    }

    wxASSERT_MSG( cell.GetData(),
                  wxString::Format(wxS("Invalid cell for property %s"),
                                   GetName()) );

    // We need to return customized cell object.
    if (pCell)
    {
        *pCell = cell;
    }
}

/*
wxString wxPGProperty::GetColumnText( unsigned int col, int choiceIndex ) const
{

    if ( col != 1 || choiceIndex == wxNOT_FOUND )
    {
        const wxPGCell& cell = GetCell(col);
        if ( cell->HasText() )
        {
            return cell->GetText();
        }
        else
        {
            if ( col == 0 )
                return GetLabel();
            else if ( col == 1 )
                return GetDisplayedString();
            else if ( col == 2 )
                return GetAttribute(wxPG_ATTR_UNITS, wxString());
        }
    }
    else
    {
        // Use choice
        return m_choices.GetLabel(choiceIndex);
    }

    return wxString();
}
*/

void wxPGProperty::DoGenerateComposedValue( wxString& text,
                                            wxPGPropValFormatFlags flags,
                                            const wxVariantList* valueOverrides,
                                            wxPGHashMapS2S* childResults ) const
{
    size_t iMax = m_children.size();

    text.clear();
    if ( iMax == 0 )
        return;

    if ( iMax > PWC_CHILD_SUMMARY_LIMIT &&
         !(flags & wxPGPropValFormatFlags::FullValue) )
        iMax = PWC_CHILD_SUMMARY_LIMIT;

    size_t iMaxMinusOne = iMax-1;

    if ( !IsTextEditable() )
        flags |= wxPGPropValFormatFlags::UneditableCompositeFragment;

    wxPGProperty* curChild = m_children[0];

    bool overridesLeft = false;
    wxVariant overrideValue;
    wxVariantList::const_iterator node;

    if ( valueOverrides )
    {
        node = valueOverrides->begin();
        if ( node != valueOverrides->end() )
        {
            overrideValue = *node;
            overridesLeft = true;
        }
    }

    size_t i;
    for ( i = 0; i < iMax; i++ )
    {
        wxVariant childValue;

        wxString childLabel = curChild->GetLabel();

        // Check for value override
        if ( overridesLeft && overrideValue.GetName() == childLabel )
        {
            if ( !overrideValue.IsNull() )
                childValue = overrideValue;
            else
                childValue = curChild->GetValue();
            ++node;
            if ( node != valueOverrides->end() )
                overrideValue = *node;
            else
                overridesLeft = false;
        }
        else
        {
            childValue = curChild->GetValue();
        }

        wxString s;
        if ( !childValue.IsNull() )
        {
            if ( overridesLeft &&
                 curChild->HasFlag(wxPGPropertyFlags::ComposedValue) &&
                 childValue.IsType(wxPG_VARIANT_TYPE_LIST) )
            {
                wxVariantList& childList = childValue.GetList();
                DoGenerateComposedValue(s, flags|wxPGPropValFormatFlags::CompositeFragment,
                                        &childList, childResults);
            }
            else
            {
#if WXWIN_COMPATIBILITY_3_2
                // Special implementation with check if user-overriden obsolete function is still in use
                s = curChild->ValueToStringWithCheck(childValue,
                                            flags|wxPGPropValFormatFlags::CompositeFragment);
#else
                s = curChild->ValueToString(childValue,
                                            flags|wxPGPropValFormatFlags::CompositeFragment);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
            }
        }

        if ( childResults && curChild->HasAnyChild() )
            (*childResults)[curChild->GetName()] = s;

        bool skip = false;
        if ( !!(flags & wxPGPropValFormatFlags::UneditableCompositeFragment) && s.empty() )
            skip = true;

        if ( !curChild->HasAnyChild() || skip )
            text += s;
        else
            text += wxS("[") + s + wxS("]");

        if ( i < iMaxMinusOne )
        {
            if ( text.length() > PWC_CHILD_SUMMARY_CHAR_LIMIT &&
                 !(flags & wxPGPropValFormatFlags::EditableValue) &&
                 !(flags & wxPGPropValFormatFlags::FullValue) )
                break;

            if ( !skip )
            {
                if ( !curChild->HasAnyChild() )
                    text += wxS("; ");
                else
                    text += wxS(" ");
            }

            curChild = m_children[i+1];
        }
    }

    if ( i < m_children.size() )
    {
        if ( !text.EndsWith(wxS("; ")) )
            text += wxS("; ...");
        else
            text += wxS("...");
    }
}

#if WXWIN_COMPATIBILITY_3_2
// By call to obsolete function we want to check if user-overriden function is still in use
wxString wxPGProperty::ValueToStringWithCheck(wxVariant& variant, wxPGPropValFormatFlags flags) const
{
    m_oldValueToStringCalled = false;
    wxString res = ValueToString(variant, static_cast<int>(flags));
    if ( m_oldValueToStringCalled )
    {
        // Our own function was called - this implies that call was forwarded to the new overriding
        // function and there is no need to call it explicitly.
    }
    else
    {   // User-overriden obsolete function was called
        wxFAIL_MSG(wxString::Format("in %s use ValueToString with 'flags' argument as wxPGPropValFormatFlags", GetClassInfo()->GetClassName()));
    }
    return res;
}
#endif // WXWIN_COMPATIBILITY_3_2

wxString wxPGProperty::ValueToString( wxVariant& WXUNUSED(value),
                                      wxPGPropValFormatFlags flags ) const
{
    wxCHECK_MSG( HasAnyChild(),
                 wxString(),
                 wxS("If user property does not have any children, it must ")
                 wxS("override GetValueAsString") );

    // FIXME: Currently code below only works if value is actually m_value
    wxASSERT_MSG( !!(flags & wxPGPropValFormatFlags::ValueIsCurrent),
                  wxS("Sorry, currently default wxPGProperty::ValueToString() ")
                  wxS("implementation only works if value is m_value.") );

    wxString text;
    DoGenerateComposedValue(text, flags);
    return text;
}

#if WXWIN_COMPATIBILITY_3_2
// By call to obsolete function we want to check if user-overriden function is still in use
wxString wxPGProperty::GetValueAsStringWithCheck(wxPGPropValFormatFlags flags) const
{
    m_oldGetValueAsString = false;
    wxString res = GetValueAsString(static_cast<int>(flags));
    if ( m_oldGetValueAsString )
    {
        // Our own function was called - this implies that call was forwarded to the new overriding
        // function and there is no need to call it explicitly.
    }
    else
    {   // User-overriden obsolete function was called
        wxFAIL_MSG(wxString::Format("in %s use GetValueAsString with 'flags' argument as wxPGPropValFormatFlags", GetClassInfo()->GetClassName()));
    }
    return res;
}
#endif // WXWIN_COMPATIBILITY_3_2

wxString wxPGProperty::GetValueAsString(wxPGPropValFormatFlags flags) const
{
    wxPropertyGrid* pg = GetGrid();
    wxCHECK_MSG( pg, wxString(),
                 wxS("Cannot get valid value for detached property") );

    if ( IsValueUnspecified() )
        return pg->GetUnspecifiedValueText(flags);

    if ( m_commonValue == -1 )
    {
        wxVariant value(GetValue());
#if WXWIN_COMPATIBILITY_3_2
        // Special implementation with check if user-overriden obsolete function is still in use
        return ValueToStringWithCheck(value, flags|wxPGPropValFormatFlags::ValueIsCurrent);
#else
        return ValueToString(value, flags|wxPGPropValFormatFlags::ValueIsCurrent);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    }

    //
    // Return common value's string representation
    const wxPGCommonValue* cv = pg->GetCommonValue(m_commonValue);

    if ( !!(flags & wxPGPropValFormatFlags::FullValue) )
    {
        return cv->GetLabel();
    }
    else if ( !!(flags & wxPGPropValFormatFlags::EditableValue) )
    {
        return cv->GetEditableText();
    }
    else
    {
        return cv->GetLabel();
    }
}

#if WXWIN_COMPATIBILITY_3_2
// By call to obsolete function we want to check if user-overriden function is still in use
bool wxPGProperty::IntToValueWithCheck(wxVariant& variant, int number, wxPGPropValFormatFlags flags) const
{
    m_oldIntToValueCalled = false;
    bool res = IntToValue(variant, number, static_cast<int>(flags));
    if ( m_oldIntToValueCalled )
    {
        // Our own function was called - this implies that call was forwarded to the new overriding
        // function and there is no need to call it explicitly.
    }
    else
    {   // User-overriden obsolete function was called
        wxFAIL_MSG(wxString::Format("in %s use IntoToValue with 'flags' argument as wxPGPropValFormatFlags", GetClassInfo()->GetClassName()));
    }
    return res;
}
#endif // WXWIN_COMPATIBILITY_3_2

bool wxPGProperty::IntToValue( wxVariant& variant, int number, wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    variant = (long)number;
    return true;
}

#if WXWIN_COMPATIBILITY_3_2
// By call to obsolete function we want to check if user-overriden function is still in use#if WXWIN_COMPATIBILITY_3_2
bool wxPGProperty::StringToValueWithCheck(wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags) const
{
    m_oldStringToValueCalled = false;
    bool res = StringToValue(variant, text, static_cast<int>(flags));
    if ( m_oldStringToValueCalled )
    {
        // Our own function was called - this implies that call was forwarded to the new overriding
        // function and there is no need to call it explicitly.
    }
    else
    {   // User-overriden obsolete function was called
        wxFAIL_MSG(wxString::Format("in %s use StringToValue with 'flags' argument as wxPGPropValFormatFlags", GetClassInfo()->GetClassName()));
    }
    return res;
}
#endif // WXWIN_COMPATIBILITY_3_2

// Convert semicolon delimited tokens into child values.
bool wxPGProperty::StringToValue( wxVariant& v, const wxString& text, wxPGPropValFormatFlags flags ) const
{
    if ( !HasAnyChild() )
        return false;

    unsigned int curChild = 0;

    unsigned int iMax = m_children.size();

    if ( iMax > PWC_CHILD_SUMMARY_LIMIT &&
         !(flags & wxPGPropValFormatFlags::FullValue) )
        iMax = PWC_CHILD_SUMMARY_LIMIT;

    bool changed = false;

    wxString token;
    size_t pos = 0;

    // It is best only to add non-empty group items
    bool addOnlyIfNotEmpty = false;
    const wxChar delimiter = wxS(';');

    size_t tokenStart = 0xFFFFFF;

    wxVariantList temp_list;
    wxVariant list(temp_list);

    wxPGPropValFormatFlags propagatedFlags = flags & (wxPGPropValFormatFlags::ReportError|wxPGPropValFormatFlags::ProgrammaticValue);

    wxLogTrace("propgrid",
               wxS(">> %s.StringToValue('%s')"), GetLabel(), text);

    wxString::const_iterator it = text.begin();
    wxUniChar a = ( it != text.end() ) ? *it : wxUniChar(0);

    for ( ;; )
    {
        // How many units we iterate string forward at the end of loop?
        // We need to keep track of this or risk going to negative
        // with it-- operation.
        unsigned int strPosIncrement = 1;

        if ( tokenStart != 0xFFFFFF )
        {
            // Token is running
            if ( a == delimiter || a == 0 )
            {
                token = text.substr(tokenStart,pos-tokenStart);
                token.Trim(true);
                size_t len = token.length();

                if ( !addOnlyIfNotEmpty || len > 0 )
                {
                    const wxPGProperty* child = Item(curChild);
                    wxVariant variant(child->GetValue());
                    wxString childName = child->GetBaseName();

                    wxLogTrace("propgrid",
                               wxS("token = '%s', child = %s"),
                               token, childName);

                    // Add only if editable or setting programmatically
                    if ( !!(flags & wxPGPropValFormatFlags::ProgrammaticValue) ||
                         (!child->HasFlag(wxPGPropertyFlags::Disabled) &&
                          !child->HasFlag(wxPGPropertyFlags::ReadOnly)) )
                    {
                        if ( len > 0 )
                        {
#if WXWIN_COMPATIBILITY_3_2
                            // Special implementation with check if user-overriden obsolete function is still in use
                            if ( child->StringToValueWithCheck(variant, token,
                                                       propagatedFlags | wxPGPropValFormatFlags::CompositeFragment) )
#else
                            if ( child->StringToValue(variant, token,
                                                      propagatedFlags | wxPGPropValFormatFlags::CompositeFragment) )
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
                            {
                                // We really need to set the variant's name
                                // *after* child->StringToValue() has been
                                // called, since variant's value may be set by
                                // assigning another variant into it, which
                                // then usually causes name to be copied (i.e.
                                // usually cleared) as well.
                                variant.SetName(childName);
                                list.Append(variant);

                                changed = true;
                            }
                        }
                        else
                        {
                            // Empty, becomes unspecified
                            variant.MakeNull();
                            variant.SetName(childName);
                            list.Append(variant);
                            changed = true;
                        }
                    }

                    curChild++;
                    if ( curChild >= iMax )
                        break;
                }

                tokenStart = 0xFFFFFF;
            }
        }
        else
        {
            // Token is not running
            if ( a != wxS(' ') )
            {

                addOnlyIfNotEmpty = false;

                // Is this a group of tokens?
                if ( a == wxS('[') )
                {
                    int depth = 1;

                    if ( it != text.end() ) ++it;
                    pos++;
                    size_t startPos = pos;

                    // Group item - find end
                    while ( it != text.end() && depth > 0 )
                    {
                        a = *it;
                        ++it;
                        pos++;

                        if ( a == wxS(']') )
                            depth--;
                        else if ( a == wxS('[') )
                            depth++;
                    }

                    token = text.substr(startPos,pos-startPos-1);

                    if ( token.empty() )
                        break;

                    const wxPGProperty* child = Item(curChild);

                    wxVariant oldChildValue = child->GetValue();
                    wxVariant variant(oldChildValue);

                    if ( !!(flags & wxPGPropValFormatFlags::ProgrammaticValue) ||
                         (!child->HasFlag(wxPGPropertyFlags::Disabled) &&
                          !child->HasFlag(wxPGPropertyFlags::ReadOnly)) )
                    {
                        wxString childName = child->GetBaseName();

#if WXWIN_COMPATIBILITY_3_2
                        // Special implementation with check if user-overriden obsolete function is still in use
                        bool stvRes = child->StringToValueWithCheck(variant, token,
                                                            propagatedFlags);
#else
                        bool stvRes = child->StringToValue(variant, token,
                                                           propagatedFlags);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
                        if ( stvRes || (variant != oldChildValue) )
                        {
                            variant.SetName(childName);
                            list.Append(variant);

                            changed = true;
                        }
                        else
                        {
                            // No changes...
                        }
                    }

                    curChild++;
                    if ( curChild >= iMax )
                        break;

                    addOnlyIfNotEmpty = true;

                    tokenStart = 0xFFFFFF;
                }
                else
                {
                    tokenStart = pos;

                    if ( a == delimiter )
                        strPosIncrement -= 1;
                }
            }
        }

        if ( a == 0 )
            break;

        it += strPosIncrement;

        a = ( it != text.end() ) ? *it : wxUniChar(0);

        pos += strPosIncrement;
    }

    if ( changed )
        v = list;

    return changed;
}

bool wxPGProperty::SetValueFromString( const wxString& text, wxPGPropValFormatFlags flags )
{
    wxVariant variant(m_value);
    bool res = StringToValue(variant, text, flags);
    if ( res )
        SetValue(variant);
    return res;
}

bool wxPGProperty::SetValueFromInt( long number, wxPGPropValFormatFlags flags )
{
    wxVariant variant(m_value);
    bool res = IntToValue(variant, number, flags);
    if ( res )
        SetValue(variant);
    return res;
}

wxSize wxPGProperty::OnMeasureImage( int WXUNUSED(item) ) const
{
    if ( m_valueBitmapBundle.IsOk() )
    {
        wxPropertyGrid* pg = GetGrid();
        wxBitmap bmp;
        double scale = 1.0;
        if ( pg )
        {
            bmp = m_valueBitmapBundle.GetBitmapFor(pg);
            int hMax = pg->GetImageSize().GetHeight();
            if ( bmp.GetHeight() > hMax )
            {
                scale = (double)hMax / bmp.GetHeight();
            }
        }
        else
        {
            bmp = m_valueBitmapBundle.GetBitmap(m_valueBitmapBundle.GetDefaultSize());
        }

        return wxSize(wxRound(scale*bmp.GetWidth()), wxDefaultCoord);
    }

    return wxSize(0,0);
}

int wxPGProperty::GetImageOffset( int imageWidth ) const
{
    int imageOffset = 0;

    if ( imageWidth )
    {
        // Do not increment offset too much for wide images
        if ( imageWidth <= (wxPG_CUSTOM_IMAGE_WIDTH+5) )
            imageOffset = imageWidth + DEFAULT_IMAGE_OFFSET_INCREMENT;
        else
            imageOffset = imageWidth + 1;
    }

    return imageOffset;
}

wxPGCellRenderer* wxPGProperty::GetCellRenderer( int WXUNUSED(column) ) const
{
    return wxPGGlobalVars->m_defaultRenderer;
}

void wxPGProperty::OnCustomPaint( wxDC& dc,
                                  const wxRect& rect,
                                  wxPGPaintData& paintData)
{
    wxCHECK_RET( m_valueBitmapBundle.IsOk(), wxS("invalid bitmap bundle") );

    wxBitmap bmp = m_valueBitmapBundle.GetBitmapFor(paintData.m_parent);
    wxBitmap scaledBmp;
    int yOfs;
    if ( bmp.GetHeight() <= rect.height )
    {
        scaledBmp = bmp;
        yOfs = (rect.height - bmp.GetHeight()) / 2;
    }
    else
    {
        double scale = (double)rect.height / bmp.GetHeight();
        scaledBmp = wxPropertyGrid::RescaleBitmap(bmp, scale, scale);
        yOfs = 0;
    }

    dc.DrawBitmap(scaledBmp,rect.x, rect.y + yOfs);
}

const wxPGEditor* wxPGProperty::DoGetEditorClass() const
{
    return wxPGEditor_TextCtrl;
}

// Default extra property event handling - that is, none at all.
bool wxPGProperty::OnEvent( wxPropertyGrid*, wxWindow*, wxEvent& )
{
    return false;
}


void wxPGProperty::SetValue( wxVariant value, wxVariant* pList, wxPGSetValueFlags flags )
{
    // If auto unspecified values are not wanted (via window or property style),
    // then get default value instead of null wxVariant.
    if ( value.IsNull() && !!(flags & wxPGSetValueFlags::ByUser) &&
         !UsesAutoUnspecified() )
    {
        value = GetDefaultValue();
    }

    if ( !value.IsNull() )
    {
        wxVariant tempListVariant;

        SetCommonValue(-1);
        // List variants are reserved a special purpose
        // as intermediate containers for child values
        // of properties with children.
        if ( value.IsType(wxPG_VARIANT_TYPE_LIST) )
        {
            //
            // However, situation is different for composed string properties
            if ( HasFlag(wxPGPropertyFlags::ComposedValue) )
            {
                tempListVariant = value;
                pList = &tempListVariant;
            }

            wxVariant newValue;
            AdaptListToValue(value, &newValue);
            value = newValue;
            //wxLogDebug(wxS(">> %s.SetValue() adapted list value to type '%s'"),GetName(),value.GetType());
        }

        if ( HasFlag(wxPGPropertyFlags::Aggregate) )
            flags |= wxPGSetValueFlags::Aggregated;

        if ( pList && !pList->IsNull() )
        {
            wxASSERT( pList->IsType(wxPG_VARIANT_TYPE_LIST) );
            wxASSERT( HasAnyChild() );
            wxASSERT( !IsCategory() );

            wxVariantList& list = pList->GetList();
            unsigned int i = 0;

            //wxLogDebug(wxS(">> %s.SetValue() pList parsing"),GetName());

            // Children in list can be in any order, but we will give hint to
            // GetPropertyByNameWH(). This optimizes for full list parsing.
            for ( auto node : list )
            {
                wxVariant& childValue = *node;
                wxPGProperty* child = GetPropertyByNameWH(childValue.GetName(), i);
                if ( child )
                {
                    //wxLogDebug(wxS("%i: child = %s, childValue.GetType()=%s"),i,child->GetBaseName(),childValue.GetType());
                    if ( childValue.IsType(wxPG_VARIANT_TYPE_LIST) )
                    {
                        if ( child->HasFlag(wxPGPropertyFlags::Aggregate) && !(flags & wxPGSetValueFlags::Aggregated) )
                        {
                            wxVariant listRefCopy = childValue;
                            child->SetValue(childValue, &listRefCopy, flags| wxPGSetValueFlags::FromParent);
                        }
                        else
                        {
                            wxVariant oldVal = child->GetValue();
                            child->SetValue(oldVal, &childValue, flags| wxPGSetValueFlags::FromParent);
                        }
                    }
                    else if ( child->GetValue() != childValue )
                    {
                        // For aggregate properties, we will trust RefreshChildren()
                        // to update child values.
                        if ( !HasFlag(wxPGPropertyFlags::Aggregate) )
                            child->SetValue(childValue, nullptr, flags| wxPGSetValueFlags::FromParent);
                        if ( !!(flags & wxPGSetValueFlags::ByUser) )
                            child->SetFlag(wxPGPropertyFlags::Modified);
                    }
                }
                i++;
            }

            // Always call OnSetValue() for a parent property (do not call it
            // here if the value is non-null because it will then be called
            // below)
            if ( value.IsNull() )
                OnSetValue();
        }

        if ( !value.IsNull() )
        {
            m_value = value;
            OnSetValue();
        }

        if ( !!(flags & wxPGSetValueFlags::ByUser) )
            SetFlag(wxPGPropertyFlags::Modified);

        if ( HasFlag(wxPGPropertyFlags::Aggregate) )
            RefreshChildren();
    }
    else
    {
        if ( m_commonValue != -1 )
        {
            wxPropertyGrid* pg = GetGrid();
            if ( !pg || m_commonValue != pg->GetUnspecifiedCommonValue() )
                SetCommonValue(-1);
        }

        m_value = value;

        // Set children to unspecified, but only if aggregate or
        // value is <composed>
        if ( AreChildrenComponents() )
        {
            for ( wxPGProperty* child : m_children )
                child->SetValue(value, nullptr, flags| wxPGSetValueFlags::FromParent);
        }
    }

    if ( !(flags & wxPGSetValueFlags::FromParent) )
        UpdateParentValues();

    //
    // Update editor control.
    if ( !!(flags & wxPGSetValueFlags::RefreshEditor) )
    {
        wxPropertyGrid* pg = GetGridIfDisplayed();
        if ( pg )
        {
            wxPGProperty* selected = pg->GetSelectedProperty();

            // Only refresh the control if this was selected, or
            // this was some parent of selected, or vice versa)
            if ( selected && (selected == this ||
                              selected->IsSomeParent(this) ||
                              this->IsSomeParent(selected)) )
                RefreshEditor();

            pg->DrawItemAndValueRelated(this);
        }
    }
}


void wxPGProperty::SetValueInEvent( const wxVariant& value ) const
{
    wxCHECK_RET( GetGrid(),
                 wxS("Cannot store pending value for detached property"));
    GetGrid()->ValueChangeInEvent(value);
}

void wxPGProperty::SetFlagRecursively( wxPGPropertyFlags flag, bool set )
{
    ChangeFlag(flag, set);

    for ( wxPGProperty* child : m_children )
        child->SetFlagRecursively(flag, set);
}

void wxPGProperty::RefreshEditor()
{
    if ( !m_parent )
        return;

    wxPropertyGrid* pg = GetGrid();
    if ( pg && pg->GetSelectedProperty() == this )
        pg->RefreshEditor();
}

wxVariant wxPGProperty::GetDefaultValue() const
{
    wxVariant defVal = GetAttribute(wxPG_ATTR_DEFAULT_VALUE);
    if ( !defVal.IsNull() )
        return defVal;

    wxVariant value = GetValue();

    if ( !value.IsNull() )
    {
        wxString valueType(value.GetType());

        if (valueType == wxPG_VARIANT_TYPE_LONG)
            return wxVariant(0L);
        if ( valueType == wxPG_VARIANT_TYPE_STRING )
            return wxVariant(wxString());
        if ( valueType == wxPG_VARIANT_TYPE_BOOL )
            return wxVariant(false);
        if ( valueType == wxPG_VARIANT_TYPE_DOUBLE )
            return wxVariant(0.0);
        if ( valueType == wxPG_VARIANT_TYPE_ARRSTRING )
            return wxVariant(wxArrayString());
#if wxUSE_LONGLONG
        if ( valueType == wxPG_VARIANT_TYPE_LONGLONG )
            return wxVariant(wxLongLong(0));
        if ( valueType == wxPG_VARIANT_TYPE_ULONGLONG )
            return wxVariant(wxULongLong(0));
#endif
        if ( valueType == wxS("wxColour") )
            return WXVARIANT(*wxBLACK);
#if wxUSE_DATETIME
        if ( valueType == wxPG_VARIANT_TYPE_DATETIME )
            return wxVariant(wxDateTime::Now());
#endif
        if ( valueType == wxS("wxFont") )
            return WXVARIANT(*wxNORMAL_FONT);
        if ( valueType == wxPoint_VariantType )
            return WXVARIANT(wxPoint(0, 0));
        if ( valueType == wxSize_VariantType )
            return WXVARIANT(wxSize(0, 0));
    }

    return wxVariant();
}

void wxPGProperty::Enable( bool enable )
{
    wxPropertyGrid* pg = GetGrid();

    // Preferably call the version in the owning wxPropertyGrid,
    // since it handles the editor de-activation.
    if ( pg )
        pg->EnableProperty(this, enable);
    else
        DoEnable(enable);
}

void wxPGProperty::DoEnable( bool enable )
{
    ChangeFlag(wxPGPropertyFlags::Disabled, !enable);

    // Apply same to sub-properties as well
    for ( wxPGProperty* child : m_children )
        child->DoEnable( enable );
}

void wxPGProperty::EnsureCells( unsigned int column )
{
    if ( column >= m_cells.size() )
    {
        // Fill empty slots with default cells
        wxPropertyGrid* pg = GetGrid();
        wxPGCell defaultCell;

        if ( pg )
        {
            defaultCell = IsCategory() ? pg->GetCategoryDefaultCell()
                                       : pg->GetPropertyDefaultCell();
        }

        // Alloc new default cells.
        m_cells.resize(column+1, defaultCell);
    }
}

void wxPGProperty::SetCell( int column,
                            const wxPGCell& cell )
{
    EnsureCells(column);

    m_cells[column] = cell;
}

void wxPGProperty::AdaptiveSetCell( unsigned int firstCol,
                                    unsigned int lastCol,
                                    const wxPGCell& cell,
                                    const wxPGCell& srcData,
                                    wxPGCellData* unmodCellData,
                                    wxPGPropertyFlags ignoreWithFlags,
                                    bool recursively )
{
    //
    // Sets cell in memory optimizing fashion. That is, if
    // current cell data matches unmodCellData, we will
    // simply get reference to data from cell. Otherwise,
    // cell information from srcData is merged into current.
    //

    if ( !(m_flags & ignoreWithFlags) && !IsRoot() )
    {
        EnsureCells(lastCol);

        for ( unsigned int col=firstCol; col<=lastCol; col++ )
        {
            if ( m_cells[col].GetData() == unmodCellData )
            {
                // Data matches... use cell directly
                m_cells[col] = cell;
            }
            else
            {
                // Data did not match... merge valid information
                m_cells[col].MergeFrom(srcData);
            }
        }
    }

    if ( recursively )
    {
        for ( wxPGProperty* child : m_children )
            child->AdaptiveSetCell( firstCol,
                                    lastCol,
                                    cell,
                                    srcData,
                                    unmodCellData,
                                    ignoreWithFlags,
                                    recursively );
    }
}

void wxPGProperty::ClearCells(wxPGPropertyFlags ignoreWithFlags, bool recursively)
{
    if ( !(m_flags & ignoreWithFlags) && !IsRoot() )
    {
        m_cells.clear();
    }

    if ( recursively )
    {
        for ( wxPGProperty* child : m_children )
        {
            child->ClearCells(ignoreWithFlags, recursively);
        }
    }
}

const wxPGCell& wxPGProperty::GetCell( unsigned int column ) const
{
    if ( m_cells.size() > column )
        return m_cells[column];

    wxPropertyGrid* pg = GetGrid();
    wxASSERT_MSG( pg,
                  wxS("Cannot get cell for detached property") );
    if ( !pg )
    {
        static const wxPGCell invalidCell;
        return invalidCell;
    }

    return IsCategory() ? pg->GetCategoryDefaultCell()
                        : pg->GetPropertyDefaultCell();
}

wxPGCell& wxPGProperty::GetOrCreateCell( unsigned int column )
{
    EnsureCells(column);
    return m_cells[column];
}

void wxPGProperty::SetBackgroundColour(const wxColour& colour, wxPGPropertyValuesFlags flags)
{
    wxPGProperty* firstProp = this;
    bool recursively = !!(flags & wxPGPropertyValuesFlags::Recurse);

    //
    // If category is tried to set recursively, skip it and only
    // affect the children.
    if ( recursively )
    {
        while ( firstProp->IsCategory() )
        {
            if ( !firstProp->HasAnyChild() )
                return;
            firstProp = firstProp->Item(0);
        }
    }

    wxPGCell& firstCell = firstProp->GetCell(0);
    wxPGCellData* firstCellData = firstCell.GetData();

    wxPGCell newCell(firstCell);
    newCell.SetBgCol(colour);
    wxPGCell srcCell;
    srcCell.SetBgCol(colour);

    AdaptiveSetCell( 0,
                     GetParentState()->GetColumnCount()-1,
                     newCell,
                     srcCell,
                     firstCellData,
                     recursively ? wxPGPropertyFlags::Category : wxPGPropertyFlags::Null,
                     recursively );
}

void wxPGProperty::SetTextColour(const wxColour& colour, wxPGPropertyValuesFlags flags)
{
    wxPGProperty* firstProp = this;
    bool recursively = !!(flags & wxPGPropertyValuesFlags::Recurse);

    //
    // If category is tried to set recursively, skip it and only
    // affect the children.
    if ( recursively )
    {
        while ( firstProp->IsCategory() )
        {
            if ( !firstProp->HasAnyChild() )
                return;
            firstProp = firstProp->Item(0);
        }
    }

    wxPGCell& firstCell = firstProp->GetCell(0);
    wxPGCellData* firstCellData = firstCell.GetData();

    wxPGCell newCell(firstCell);
    newCell.SetFgCol(colour);
    wxPGCell srcCell;
    srcCell.SetFgCol(colour);

    AdaptiveSetCell( 0,
                     GetParentState()->GetColumnCount()-1,
                     newCell,
                     srcCell,
                     firstCellData,
                     recursively ? wxPGPropertyFlags::Category : wxPGPropertyFlags::Null,
                     recursively );
}

void wxPGProperty::SetDefaultColours(wxPGPropertyValuesFlags flags)
{
    wxPGProperty* firstProp = this;
    bool recursively = !!(flags & wxPGPropertyValuesFlags::Recurse);

    // If category is tried to set recursively, skip it and only
    // affect the children.
    if ( recursively )
    {
        while ( firstProp->IsCategory() )
        {
            if ( !firstProp->HasAnyChild() )
                return;
            firstProp = firstProp->Item(0);
        }
    }

    ClearCells(recursively ? wxPGPropertyFlags::Category : wxPGPropertyFlags::Null,
               recursively);
}

wxPGEditorDialogAdapter* wxPGProperty::GetEditorDialog() const
{
    return nullptr;
}

bool wxPGProperty::DoSetAttribute( const wxString& WXUNUSED(name), wxVariant& WXUNUSED(value) )
{
    return false;
}

void wxPGProperty::SetAttribute( const wxString& name, wxVariant value )
{
    if ( !DoSetAttribute(name, value) || !wxPGGlobalVars->HasExtraStyle(wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES) )
    {
        m_attributes.Set(name, value);
    }
    // Because changing some attributes (like wxPG_FLOAT_PRECISION, wxPG_UINT_BASE,
    // wxPG_UINT_PREFIX, wxPG_ATTR_UNITS) affect displayed form of the property value
    // so we have to redraw property to be sure that displayed value is in sync
    // with current attributes.
    wxPropertyGrid* pg = GetGridIfDisplayed();
    if ( pg )
    {
        pg->RefreshProperty(this);
    }
}

void wxPGProperty::SetAttributes( const wxPGAttributeStorage& attributes )
{
    wxPGAttributeStorage::const_iterator it = attributes.StartIteration();
    wxVariant variant;

    while ( attributes.GetNext(it, variant) )
        SetAttribute( variant.GetName(), variant );
}

wxVariant wxPGProperty::DoGetAttribute( const wxString& WXUNUSED(name) ) const
{
    return wxVariant();
}


wxVariant wxPGProperty::GetAttribute( const wxString& name ) const
{
    wxVariant value = DoGetAttribute(name);
    return !value.IsNull() ? value : m_attributes.FindValue(name);
}

wxString wxPGProperty::GetAttribute( const wxString& name, const wxString& defVal ) const
{
    wxVariant variant = m_attributes.FindValue(name);

    return variant.IsNull() ? defVal : variant.GetString();
}

long wxPGProperty::GetAttributeAsLong( const wxString& name, long defVal ) const
{
    wxVariant variant = m_attributes.FindValue(name);

    return variant.IsNull() ? defVal : variant.GetLong();
}

double wxPGProperty::GetAttributeAsDouble( const wxString& name, double defVal ) const
{
    wxVariant variant = m_attributes.FindValue(name);

    return variant.IsNull() ? defVal : variant.GetDouble();
}

wxVariant wxPGProperty::GetAttributesAsList() const
{
    wxVariantList tempList;
    wxVariant v( tempList, wxString::Format(wxS("@%s@attr"),m_name) );

    wxPGAttributeStorage::const_iterator it = m_attributes.StartIteration();
    wxVariant variant;

    while ( m_attributes.GetNext(it, variant) )
        v.Append(variant);

    return v;
}

// Utility flags are excluded.
// Store the literals in the internal representation for better performance.
static const std::array<std::pair<wxPGPropertyFlags, const wxStringCharType*>, 4> gs_propFlagToString
{ {
  { wxPGPropertyFlags::Disabled,  wxS("DISABLED")  },
  { wxPGPropertyFlags::Hidden,    wxS("HIDDEN")    },
  { wxPGPropertyFlags::NoEditor,  wxS("NOEDITOR")  },
  { wxPGPropertyFlags::Collapsed, wxS("COLLAPSED") }
} };

wxString wxPGProperty::GetFlagsAsString(wxPGPropertyFlags flagsMask) const
{
    wxString s;
    const wxPGPropertyFlags relevantFlags = m_flags & flagsMask & wxPGPropertyFlags::StringStoredFlags;

    for ( auto& item : gs_propFlagToString )
    {
        if ( !!(relevantFlags & item.first) )
        {
            if ( !s.empty() )
            {
                s.append(wxS("|"));
            }
            s.append(item.second);
        }
    }

    return s;
}

void wxPGProperty::SetFlagsFromString( const wxString& str )
{
    wxPGPropertyFlags flags = wxPGPropertyFlags::Null;

    WX_PG_TOKENIZER1_BEGIN(str, wxS('|'))
        for ( auto& item : gs_propFlagToString )
        {
            if ( token == item.second )
            {
                flags |= item.first;
                break;
            }
        }
    WX_PG_TOKENIZER1_END()

    m_flags = (m_flags & ~wxPGPropertyFlags::StringStoredFlags) | flags;
}

wxValidator* wxPGProperty::DoGetValidator() const
{
    return nullptr;
}

int wxPGProperty::InsertChoice( const wxString& label, int index, int value )
{
    wxPropertyGrid* pg = GetGrid();
    const int sel = GetChoiceSelection();

    int newSel = (sel == wxNOT_FOUND) ? 0 : sel;

    const int numChoices = m_choices.GetCount();
    if ( index == wxNOT_FOUND )
        index = numChoices;

    if ( numChoices > 0 && index <= sel )
        newSel++;

    m_choices.Insert(label, index, value);
    // Set new selection if it was modified
    // or if the first element was added.
    if ( sel != newSel || numChoices == 0 )
        SetChoiceSelection(newSel);

    if ( pg && this == pg->GetSelection() )
        GetEditorClass()->InsertItem(pg->GetEditorControl(),label,index);

    return index;
}


void wxPGProperty::DeleteChoice( int index )
{
    wxPropertyGrid* pg = GetGrid();

    int sel = GetChoiceSelection();
    int newSel = sel;

    // Adjust current value
    if ( sel == index )
    {
        SetValueToUnspecified();
        newSel = 0;
    }
    else if ( index < sel )
    {
        newSel--;
    }

    m_choices.RemoveAt(index);

    if ( sel != newSel )
        SetChoiceSelection(newSel);

    if ( pg && this == pg->GetSelection() )
        GetEditorClass()->DeleteItem(pg->GetEditorControl(), index);
}

int wxPGProperty::GetChoiceSelection() const
{
    wxVariant value = GetValue();
    wxString valueType = value.GetType();
    int index = wxNOT_FOUND;

    if ( IsValueUnspecified() || m_choices.GetCount() == 0 )
        return wxNOT_FOUND;

    if ( valueType == wxPG_VARIANT_TYPE_LONG )
    {
        index = m_choices.Index(value.GetLong());
    }
    else if ( valueType == wxPG_VARIANT_TYPE_STRING )
    {
        index = m_choices.Index(value.GetString());
    }
    else if ( valueType == wxPG_VARIANT_TYPE_BOOL )
    {
        index = value.GetBool()? 1 : 0;
    }

    return index;
}

void wxPGProperty::SetChoiceSelection( int newValue )
{
    wxCHECK_RET( m_choices.IsOk(), wxS("invalid choiceinfo") );

    // Allow setting the value of -1 to reset the selection, for consistency
    // with wxChoice::SetSelection().
    if ( newValue == wxNOT_FOUND )
    {
        SetValueToUnspecified();
        return;
    }

    wxCHECK_RET( newValue >= 0 && newValue < (int)m_choices.GetCount(),
                 wxS("New index is out of range") );

    // Changes value of a property with choices, but only
    // works if the value type is long or string.
    wxString valueType = GetValue().GetType();

    if ( valueType == wxPG_VARIANT_TYPE_STRING )
    {
        SetValue( m_choices.GetLabel(newValue) );
    }
    else  // if ( valueType == wxPG_VARIANT_TYPE_LONG )
    {
        SetValue( m_choices.GetValue(newValue) );
    }
}

bool wxPGProperty::SetChoices( const wxPGChoices& choices )
{
    // Property must be de-selected first (otherwise choices in
    // the control would be de-synced with true choices)
    wxPropertyGrid* pg = GetGrid();
    bool isSelected = pg && pg->GetSelection() == this;
    if ( isSelected )
    {
        pg->ClearSelection();
    }

    m_choices.Assign(choices);
    if ( isSelected )
    {
        wxWindow* ctrl = pg->GetEditorControl();
        if ( ctrl )
            GetEditorClass()->SetItems(ctrl, m_choices.GetLabels());
    }

    // This may be needed to trigger some initialization
    // (but don't do it if property is somewhat uninitialized)
    wxVariant defVal = GetDefaultValue();
    if ( !defVal.IsNull() )
        SetValue(defVal);

    if ( isSelected )
    {
        // Recreate editor
        pg->DoSelectProperty(this, wxPGSelectPropertyFlags::Force);
    }

    return true;
}


const wxPGEditor* wxPGProperty::GetEditorClass() const
{
    const wxPGEditor* editor = m_customEditor ? m_customEditor : DoGetEditorClass();

    // Maybe override editor if common value specified
    if ( GetDisplayedCommonValueCount() )
    {
        // TextCtrlAndButton -> ComboBoxAndButton
        if ( wxDynamicCast(editor, wxPGTextCtrlAndButtonEditor) )
            editor = wxPGEditor_ChoiceAndButton;

        // TextCtrl -> ComboBox
        else if ( wxDynamicCast(editor, wxPGTextCtrlEditor) )
            editor = wxPGEditor_ComboBox;
    }

    return editor;
}

bool wxPGProperty::Hide( bool hide, wxPGPropertyValuesFlags flags )
{
    wxPropertyGrid* pg = GetGrid();
    if ( pg )
        return pg->HideProperty(this, hide, flags);

    return DoHide( hide, flags );
}

bool wxPGProperty::DoHide( bool hide, wxPGPropertyValuesFlags flags )
{
    ChangeFlag(wxPGPropertyFlags::Hidden, hide);

    if ( !!(flags & wxPGPropertyValuesFlags::Recurse) )
    {
        for ( wxPGProperty* child : m_children )
            child->DoHide(hide, flags | wxPGPropertyValuesFlags::RecurseStarts);
    }

    return true;
}

bool wxPGProperty::HasVisibleChildren() const
{
    for ( wxPGProperty* child : m_children )
    {
        if ( !child->HasFlag(wxPGPropertyFlags::Hidden) )
            return true;
    }

    return false;
}

bool wxPGProperty::RecreateEditor()
{
    wxPropertyGrid* pg = GetGrid();
    wxCHECK_MSG(pg, false, "Cannot recreate editor for detached property");

    wxPGProperty* selected = pg->GetSelection();
    if ( this == selected )
    {
        pg->DoSelectProperty(this, wxPGSelectPropertyFlags::Force);
        return true;
    }
    return false;
}


void wxPGProperty::SetValueImage( const wxBitmapBundle& bmp )
{
    wxCHECK_RET( GetGrid(),
                 wxS("Cannot set image for detached property") );

    if ( bmp.IsOk() )
    {
        m_valueBitmapBundle = bmp;
        m_flags |= wxPGPropertyFlags::CustomImage;
    }
    else
    {
        m_valueBitmapBundle = wxBitmapBundle();
        m_flags &= ~(wxPGPropertyFlags::CustomImage);
    }
}


wxPGProperty* wxPGProperty::GetMainParent() const
{
    wxPGProperty* curChild = const_cast<wxPGProperty*>(this);
    wxPGProperty* curParent = m_parent;

    while ( !curParent->IsRoot() && !curParent->IsCategory() )
    {
        curChild = curParent;
        curParent = curParent->m_parent;
    }

    return curChild;
}


const wxPGProperty* wxPGProperty::GetLastVisibleSubItem() const
{
    //
    // Returns last visible sub-item, recursively.
    if ( !IsExpanded() || !HasAnyChild() )
        return this;

    return Last()->GetLastVisibleSubItem();
}


bool wxPGProperty::IsVisible() const
{
    if ( HasFlag(wxPGPropertyFlags::Hidden) )
        return false;

    for (const wxPGProperty* parent = GetParent(); parent != nullptr; parent = parent->GetParent() )
    {
        if ( !parent->IsExpanded() || parent->HasFlag(wxPGPropertyFlags::Hidden) )
            return false;
    }

    return true;
}

wxPropertyGrid* wxPGProperty::GetGridIfDisplayed() const
{
    wxPropertyGridPageState* state = GetParentState();
    if ( !state )
        return nullptr;
    wxPropertyGrid* propGrid = state->GetGrid();
    return state == propGrid->GetState() ? propGrid : nullptr;
}

int wxPGProperty::GetY2( int lh ) const
{
    const wxPGProperty* parent;
    const wxPGProperty* child = this;

    int y = 0;

    for ( parent = GetParent(); parent != nullptr; parent = child->GetParent() )
    {
        if ( !parent->IsExpanded() )
            return parent->GetY2(lh);
        y += parent->GetChildrenHeight(lh, child->GetIndexInParent());
        y += lh;
        child = parent;
    }

    y -= lh;  // need to reduce one level

    return y;
}


int wxPGProperty::GetY() const
{
    wxPropertyGrid *pg = GetGrid();
    wxCHECK_MSG( pg, 0,
                 wxS("Cannot obtain coordinates of detached property") );
    return GetY2(pg->GetRowHeight());
}

// This is used by Insert etc.
void wxPGProperty::DoAddChild( wxPGProperty* prop, int index,
                               bool correct_mode )
{
    if ( index < 0 || (size_t)index >= m_children.size() )
    {
        if ( correct_mode ) prop->m_arrIndex = m_children.size();
        m_children.push_back( prop );
    }
    else
    {
        m_children.insert( m_children.begin()+index, prop);
        if ( correct_mode ) FixIndicesOfChildren( index );
    }

    prop->m_parent = this;
}

void wxPGProperty::DoPreAddChild( int index, wxPGProperty* prop )
{
    wxASSERT_MSG( !prop->GetBaseName().empty(),
                  wxS("Property's children must have unique, non-empty ")
                  wxS("names within their scope") );

    prop->m_arrIndex = index;
    m_children.insert( m_children.begin()+index,
                       prop );

    int custImgHeight = prop->OnMeasureImage().y;
    if ( custImgHeight == wxDefaultCoord /*|| custImgHeight > 1*/ )
        prop->m_flags |= wxPGPropertyFlags::CustomImage;

    prop->m_parent = this;
}

void wxPGProperty::AddPrivateChild( wxPGProperty* prop )
{
    if ( !(m_flags & wxPGPropertyFlags::ParentalFlags) )
        SetParentalType(wxPGPropertyFlags::Aggregate);

    wxASSERT_MSG( (m_flags & wxPGPropertyFlags::ParentalFlags) == wxPGPropertyFlags::Aggregate,
                  wxS("Do not mix up AddPrivateChild() calls with other ")
                  wxS("property adders.") );

    DoPreAddChild( m_children.size(), prop );
}

wxPGProperty* wxPGProperty::InsertChild( int index,
                                         wxPGProperty* childProperty )
{
    if ( index < 0 )
        index = m_children.size();

    if ( m_parentState )
    {
        m_parentState->DoInsert(this, index, childProperty);
    }
    else
    {
        if ( !(m_flags & wxPGPropertyFlags::ParentalFlags) )
            SetParentalType(wxPGPropertyFlags::MiscParent);

        wxASSERT_MSG( (m_flags & wxPGPropertyFlags::ParentalFlags) == wxPGPropertyFlags::MiscParent,
                      wxS("Do not mix up AddPrivateChild() calls with other ")
                      wxS("property adders.") );

        DoPreAddChild( index, childProperty );
    }

    return childProperty;
}

void wxPGProperty::RemoveChild( wxPGProperty* p )
{
    auto it = std::find(m_children.begin(), m_children.end(), p);
    if ( it != m_children.end() )
    {
        m_children.erase(it);
    }
}

void wxPGProperty::RemoveChild(unsigned int index)
{
    m_children.erase(m_children.begin()+index);
}

#if WXWIN_COMPATIBILITY_3_2
void wxPGProperty::SortChildren(int (*fCmp)(wxPGProperty**, wxPGProperty**))
{
    wxArray_SortFunction<wxPGProperty*> sf(fCmp);
    std::sort(m_children.begin(), m_children.end(), sf);
}
#endif // WXWIN_COMPATIBILITY_3_2

void wxPGProperty::SortChildren(bool (*fCmp)(wxPGProperty*, wxPGProperty*))
{
    std::sort(m_children.begin(), m_children.end(), fCmp);
}

void wxPGProperty::AdaptListToValue( wxVariant& list, wxVariant* value ) const
{
    wxASSERT( HasAnyChild() );
    wxASSERT( !IsCategory() );

    *value = GetValue();

    if ( list.GetCount() == 0 )
        return;

    wxASSERT( GetChildCount() >= (unsigned int)list.GetCount() );

    bool allChildrenSpecified;

    // Don't fully update aggregate properties unless all children have
    // specified value
    if ( HasFlag(wxPGPropertyFlags::Aggregate) )
        allChildrenSpecified = AreAllChildrenSpecified(&list);
    else
        allChildrenSpecified = true;

    size_t n = 0;
    wxVariant childValue = list[n];

    //wxLogDebug(wxS(">> %s.AdaptListToValue()"),GetBaseName());

    for ( unsigned int i = 0; i < GetChildCount(); i++ )
    {
        const wxPGProperty* child = Item(i);
        if ( childValue.GetName() == child->GetBaseName() )
        {
            //wxLogDebug(wxS("  %s(n=%i), %s"),childValue.GetName(),n,childValue.GetType());

            if ( childValue.IsType(wxPG_VARIANT_TYPE_LIST) )
            {
                wxVariant cv2(child->GetValue());
                child->AdaptListToValue(childValue, &cv2);
                childValue = cv2;
            }

            if ( allChildrenSpecified )
            {
                *value = ChildChanged(*value, i, childValue);
            }

            n++;
            if ( n == list.GetCount() )
                break;
            childValue = list[n];
        }
    }
}


void wxPGProperty::FixIndicesOfChildren( unsigned int starthere )
{
    for ( unsigned int i = starthere; i < GetChildCount(); i++)
        Item(i)->m_arrIndex = i;
}


// Returns (direct) child property with given name (or nullptr if not found)
wxPGProperty* wxPGProperty::GetPropertyByName( const wxString& name ) const
{
    for ( wxPGProperty* p : m_children )
    {
        if ( p->m_name == name )
            return p;
    }

    // Does it have point, then?
    int pos = name.Find(wxS('.'));
    if ( pos <= 0 )
        return nullptr;

    wxPGProperty* p = GetPropertyByName(name. substr(0,pos));

    if ( !p || !p->HasAnyChild() )
        return nullptr;

    return p->GetPropertyByName(name.substr(pos+1,name.length()-pos-1));
}

wxPGProperty* wxPGProperty::GetPropertyByNameWH( const wxString& name, unsigned int hintIndex ) const
{
    unsigned int i = hintIndex >= GetChildCount() ? 0 : hintIndex;
    unsigned int lastIndex = i - 1;

    if ( lastIndex >= GetChildCount() )
        lastIndex = GetChildCount() - 1;

    for (;;)
    {
        wxPGProperty* p = Item(i);
        if ( p->m_name == name )
            return p;

        if ( i == lastIndex )
            break;

        i++;
        if ( i == GetChildCount() )
            i = 0;
    }

    return nullptr;
}

int wxPGProperty::GetChildrenHeight( int lh, int iMax ) const
{
    // Returns height of children, recursively, and
    // by taking expanded/collapsed status into account.
    //
    // iMax is used when finding property y-positions.
    //

    unsigned int _iMax = iMax == -1 ? GetChildCount() : iMax;
    wxASSERT( _iMax <= GetChildCount() );

    if ( !IsExpanded() && GetParent() )
        return 0;

    int h = 0;
    for ( unsigned int i = 0; i < _iMax; i++ )
    {
        wxPGProperty* pwc = Item(i);

        if ( !pwc->HasFlag(wxPGPropertyFlags::Hidden) )
        {
            if ( !pwc->IsExpanded() ||
                 !pwc->HasAnyChild() )
                h += lh;
            else
                h += pwc->GetChildrenHeight(lh) + lh;
        }
    }

    return h;
}

wxPGProperty* wxPGProperty::GetItemAtY( unsigned int y,
                                        unsigned int lh,
                                        unsigned int* nextItemY ) const
{
    wxCHECK( nextItemY, nullptr );

    // Linear search at the moment
    //
    // nextItemY = y of next visible property, final value will be written back.
    wxPGProperty* result = nullptr;
    wxPGProperty* current = nullptr;
    unsigned int iy = *nextItemY;

    for ( wxPGProperty* pwc : m_children )
    {
        if ( !pwc->HasFlag(wxPGPropertyFlags::Hidden) )
        {
            // Found?
            if ( y < iy )
            {
                result = current;
                break;
            }

            iy += lh;

            if ( pwc->IsExpanded() &&
                 pwc->HasAnyChild() )
            {
                result = pwc->GetItemAtY( y, lh, &iy );
                if ( result )
                    break;
            }

            current = pwc;
        }
    }

    // Found?
    if ( !result && y < iy )
        result = current;

    *nextItemY = iy;

    /*
    if ( current )
    {
        wxLogDebug(wxS("%s::GetItemAtY(%i) -> %s"),this->GetLabel(),y,current->GetLabel());
    }
    else
    {
        wxLogDebug(wxS("%s::GetItemAtY(%i) -> NULL"),this->GetLabel(),y);
    }
    */

    return result;
}

void wxPGProperty::Empty()
{
    if ( !HasFlag(wxPGPropertyFlags::ChildrenAreCopies) )
    {
        for ( wxPGProperty* child : m_children )
        {
            delete child;
        }
    }

    m_children.clear();
}

wxPGProperty* wxPGProperty::GetItemAtY( unsigned int y ) const
{
    wxPropertyGrid *pg = GetGrid();
    wxCHECK_MSG( pg, nullptr,
                 wxS("Cannot obtain property item for detached property") );

    unsigned int nextItem = 0;
    return GetItemAtY(y, pg->GetRowHeight(), &nextItem);
}

void wxPGProperty::DeleteChildren()
{
    wxPropertyGridPageState* state = m_parentState;

    if ( !HasAnyChild() )
        return;

    // Because deletion is sometimes deferred, we have to use
    // this sort of code for enumerating the child properties.
    unsigned int i = GetChildCount();
    while ( i > 0 )
    {
        i--;
        state->DoDelete(Item(i), true);
    }
}

bool wxPGProperty::IsChildSelected( bool recursive ) const
{
    for ( wxPGProperty* child : m_children )
    {
        // Test child
        if ( m_parentState->DoIsPropertySelected( child ) )
            return true;

        // Test sub-children
        if ( recursive && child->IsChildSelected( recursive ) )
            return true;
    }

    return false;
}

wxVariant wxPGProperty::ChildChanged( wxVariant& WXUNUSED(thisValue),
                                      int WXUNUSED(childIndex),
                                      wxVariant& WXUNUSED(childValue) ) const
{
    return wxVariant();
}

bool wxPGProperty::AreAllChildrenSpecified( const wxVariant* pendingList ) const
{
    const wxVariantList* pList = nullptr;
    wxVariantList::const_iterator node;

    if ( pendingList )
    {
        pList = &pendingList->GetList();
        node = pList->begin();
    }

    for ( wxPGProperty* child : m_children )
    {
        const wxVariant* listValue = nullptr;
        wxVariant value;

        if ( pendingList )
        {
            const wxString& childName = child->GetBaseName();

            for ( ; node != pList->end(); ++node )
            {
                const wxVariant& item = **node;
                if ( item.GetName() == childName )
                {
                    listValue = &item;
                    value = item;
                    break;
                }
            }
        }

        if ( !listValue )
            value = child->GetValue();

        if ( value.IsNull() )
            return false;

        // Check recursively
        if ( child->HasAnyChild() )
        {
            const wxVariant* childList = nullptr;

            if ( listValue && listValue->IsType(wxPG_VARIANT_TYPE_LIST) )
                childList = listValue;

            if ( !child->AreAllChildrenSpecified(childList) )
                return false;
        }
    }

    return true;
}

wxPGProperty* wxPGProperty::UpdateParentValues()
{
    wxPGProperty* parent = m_parent;
    if ( parent && parent->HasFlag(wxPGPropertyFlags::ComposedValue) &&
         !parent->IsCategory() && !parent->IsRoot() )
    {
        wxString s;
        parent->DoGenerateComposedValue(s);
        parent->m_value = s;
        return parent->UpdateParentValues();
    }
    return this;
}

bool wxPGProperty::IsTextEditable() const
{
    if ( HasFlag(wxPGPropertyFlags::ReadOnly) )
        return false;

    if ( HasFlag(wxPGPropertyFlags::NoEditor) &&
         (HasAnyChild() ||
          wxString(GetEditorClass()->GetClassInfo()->GetClassName()).EndsWith(wxS("Button")))
       )
        return false;

    return true;
}

// Call after fixed sub-properties added/removed after creation.
// if oldSelInd >= 0 and < new max items, then selection is
// moved to it. Note: oldSelInd -2 indicates that this property
// should be selected.
void wxPGProperty::SubPropsChanged( int oldSelInd )
{
    wxPropertyGridPageState* state = GetParentState();
    wxPropertyGrid* grid = state->GetGrid();

    //
    // Re-repare children (recursively)
    for ( wxPGProperty* child : m_children )
    {
        child->InitAfterAdded(state, grid);
    }

    wxPGProperty* sel = nullptr;
    if ( oldSelInd >= (int)m_children.size() )
        oldSelInd = (int)m_children.size() - 1;

    if ( oldSelInd >= 0 )
        sel = m_children[oldSelInd];
    else if ( oldSelInd == -2 )
        sel = this;

    if ( sel )
        state->DoSelectProperty(sel);

    if ( state == grid->GetState() )
    {
        grid->GetPanel()->Refresh();
    }
}

wxString wxPGProperty::GetHintText() const
{
    wxVariant vHintText = GetAttribute(wxPG_ATTR_HINT);

    return vHintText.IsNull() ? wxString() : vHintText.GetString();
}

int wxPGProperty::GetDisplayedCommonValueCount() const
{
    if ( HasFlag(wxPGPropertyFlags::UsesCommonValue) )
    {
        wxPropertyGrid* pg = GetGrid();
        if ( pg )
            return (int)pg->GetCommonValueCount();
    }
    return 0;
}

void wxPGProperty::SetDefaultValue(wxVariant& value)
{
    SetAttribute(wxPG_ATTR_DEFAULT_VALUE, value);
}

void wxPGProperty::SetEditor(const wxString& editorName)
{
    m_customEditor = wxPropertyGridInterface::GetEditorByName(editorName);
}

bool wxPGProperty::SetMaxLength(int maxLen)
{
    const wxPGEditor* editorClass = GetEditorClass();
    if ( editorClass != wxPGEditor_TextCtrl &&
         editorClass != wxPGEditor_TextCtrlAndButton )
        return false;

    m_maxLen = wxMax(maxLen, 0); // shouldn't be a nagative value
    return true;
}

wxBitmap* wxPGProperty::GetValueImage() const
{
    if ( !m_valueBitmapBundle.IsOk() )
        return nullptr;

    wxPropertyGrid* pg = GetGrid();
    if ( pg )
        m_valueBitmap = m_valueBitmapBundle.GetBitmapFor(pg);
    else
        m_valueBitmap = m_valueBitmapBundle.GetBitmap(m_valueBitmapBundle.GetDefaultSize());

    return &m_valueBitmap;
}

// -----------------------------------------------------------------------
// wxPGRootProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGRootProperty, wxPGProperty, TextCtrl)

wxPGRootProperty::wxPGRootProperty( const wxString& name )
    : wxPGProperty()
{
    m_name = name;
    m_label = m_name;
    SetParentalType(wxPGPropertyFlags::Null);
    m_depth = 0;
}

// -----------------------------------------------------------------------
// wxPropertyCategory
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPropertyCategory, wxPGProperty, TextCtrl)

void wxPropertyCategory::Init()
{
    // don't set colour - prepareadditem method should do this
    SetParentalType(wxPGPropertyFlags::Category);
    m_capFgColIndex = 1;
    m_textExtent = -1;
}

wxPropertyCategory::wxPropertyCategory()
    : wxPGProperty()
{
    Init();
}


wxPropertyCategory::wxPropertyCategory( const wxString &label, const wxString& name )
    : wxPGProperty(label,name)
{
    Init();
}

wxString wxPropertyCategory::ValueToString( wxVariant& WXUNUSED(value),
                                            wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    return m_value.IsType(wxPG_VARIANT_TYPE_STRING) ? m_value.GetString() : wxString();
}

wxString wxPropertyCategory::GetValueAsString(wxPGPropValFormatFlags flags) const
{
    // Unspecified value is always empty string
    return IsValueUnspecified() ? wxString() : wxPGProperty::GetValueAsString(flags);
}

static int DoGetTextExtent(const wxWindow* wnd, const wxString& label, const wxFont& font)
{
    int x = 0, y = 0;
    wnd->GetTextExtent(label, &x, &y, nullptr, nullptr, &font);
    return x;
}

int wxPropertyCategory::GetTextExtent( const wxWindow* wnd, const wxFont& font ) const
{
    return (m_textExtent > 0) ? m_textExtent : DoGetTextExtent(wnd, m_label, font);
}

void wxPropertyCategory::CalculateTextExtent(const wxWindow* wnd, const wxFont& font)
{
    m_textExtent = DoGetTextExtent(wnd, m_label, font);
}

// -----------------------------------------------------------------------
// wxPGChoices
// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Add( const wxString& label, int value )
{
    AllocExclusive();

    wxPGChoiceEntry entry(label, value);
    return m_data->Insert( -1, entry );
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Add( const wxString& label, const wxBitmapBundle& bitmap, int value )
{
    AllocExclusive();

    wxPGChoiceEntry entry(label, value);
    entry.SetBitmap(bitmap);
    return m_data->Insert( -1, entry );
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Insert( const wxPGChoiceEntry& entry, int index )
{
    AllocExclusive();

    return m_data->Insert( index, entry );
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Insert( const wxString& label, int index, int value )
{
    AllocExclusive();

    wxPGChoiceEntry entry(label, value);
    return m_data->Insert( index, entry );
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::AddAsSorted( const wxString& label, int value )
{
    AllocExclusive();

    size_t index = 0;

    while ( index < GetCount() )
    {
        int cmpRes = GetLabel(index).Cmp(label);
        if ( cmpRes > 0 )
            break;
        index++;
    }

    wxPGChoiceEntry entry(label, value);
    return m_data->Insert( index, entry );
}

// -----------------------------------------------------------------------

void wxPGChoices::Add(size_t count, const wxString* labels, const long* values)
{
    AllocExclusive();

    for ( size_t i = 0; i < count; ++i )
    {
        const int value = values ? values[i] : i;
        wxPGChoiceEntry entry(labels[i], value);
        m_data->Insert( i, entry );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxChar* const* labels, const ValArrItem* values )
{
    AllocExclusive();

    for ( unsigned int i = 0; *labels; labels++, i++ )
    {
        const int value = values ? values[i] : i;
        wxPGChoiceEntry entry(*labels, value);
        m_data->Insert( i, entry );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxArrayString& arr, const wxArrayInt& arrint )
{
    AllocExclusive();

    const unsigned int itemcount = arr.size();
    const unsigned int valcount = arrint.size();
    wxASSERT_MSG( valcount >= itemcount || valcount == 0,
                  wxS("Insufficient number of values in the array") );

    for ( unsigned int i = 0; i < itemcount; i++ )
    {
        const int value = (i < valcount) ? arrint[i] : i;
        wxPGChoiceEntry entry(arr[i], value);
        m_data->Insert( i, entry );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::RemoveAt(size_t nIndex, size_t count)
{
    AllocExclusive();

    wxASSERT( m_data->GetRefCount() != -1 );
    m_data->m_items.erase(m_data->m_items.begin()+nIndex,
                          m_data->m_items.begin()+nIndex+count);
}

// -----------------------------------------------------------------------

void wxPGChoices::Clear()
{
    if ( m_data != wxPGChoicesEmptyData )
    {
        AllocExclusive();
        m_data->Clear();
    }
}

// -----------------------------------------------------------------------

int wxPGChoices::Index( const wxString& str ) const
{
    if ( IsOk() )
    {
        for ( unsigned int i = 0; i < m_data->GetCount(); i++ )
        {
            const wxPGChoiceEntry& entry = m_data->Item(i);
            if ( entry.HasText() && entry.GetText() == str )
                return i;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------

int wxPGChoices::Index( int val ) const
{
    if ( IsOk() )
    {
        for ( unsigned int i = 0; i < m_data->GetCount(); i++ )
        {
            if ( GetValue(i) == val )
                return i;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------

wxArrayString wxPGChoices::GetLabels() const
{
    wxArrayString arr;

    if ( IsOk() )
        for ( unsigned int i = 0; i < GetCount(); i++ )
            arr.push_back(GetLabel(i));

    return arr;
}

// -----------------------------------------------------------------------

wxArrayInt wxPGChoices::GetValuesForStrings( const wxArrayString& strings ) const
{
    wxArrayInt arr;

    if ( IsOk() )
    {
        for( const wxString& str : strings )
        {
            int index = Index(str);
            if ( index >= 0 )
                arr.Add(GetValue(index));
            else
                arr.Add(wxPG_INVALID_VALUE);
        }
    }

    return arr;
}

// -----------------------------------------------------------------------

wxArrayInt wxPGChoices::GetIndicesForStrings( const wxArrayString& strings,
                                              wxArrayString* unmatched ) const
{
    wxArrayInt arr;

    if ( IsOk() )
    {
        for( const wxString& str : strings )
        {
            int index = Index(str);
            if ( index >= 0 )
                arr.Add(index);
            else if ( unmatched )
                unmatched->Add(str);
        }
    }

    return arr;
}

// -----------------------------------------------------------------------

void wxPGChoices::AllocExclusive()
{
    EnsureData();

    if ( m_data->GetRefCount() != 1 )
    {
        wxPGChoicesData* data = new wxPGChoicesData();
        data->CopyDataFrom(m_data);
        Free();
        m_data = data;
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::AssignData( wxPGChoicesData* data )
{
    Free();

    if ( data != wxPGChoicesEmptyData )
    {
        m_data = data;
        data->IncRef();
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Init()
{
    m_data = wxPGChoicesEmptyData;
}

// -----------------------------------------------------------------------

void wxPGChoices::Free()
{
    if ( m_data != wxPGChoicesEmptyData )
    {
        m_data->DecRef();
        m_data = wxPGChoicesEmptyData;
    }
}

// -----------------------------------------------------------------------
// wxPGAttributeStorage
// -----------------------------------------------------------------------

static inline void IncDataRef(std::unordered_map<wxString, wxVariantData*>& map)
{
    for( const auto& it: map )
    {
        it.second->IncRef();
    }
}

static inline void DecDataRef(std::unordered_map<wxString, wxVariantData*>& map)
{
    for ( const auto& it : map )
    {
        it.second->DecRef();
    }
}

wxPGAttributeStorage::wxPGAttributeStorage(const wxPGAttributeStorage& other)
{
    m_map = other.m_map;
    IncDataRef(m_map);
}

wxPGAttributeStorage::~wxPGAttributeStorage()
{
    DecDataRef(m_map);
}

wxPGAttributeStorage& wxPGAttributeStorage::operator=(const wxPGAttributeStorage& rhs)
{
    if ( this != &rhs )
    {
        DecDataRef(m_map);
        m_map = rhs.m_map;
        IncDataRef(m_map);
    }
    return *this;
}

void wxPGAttributeStorage::Set(const wxString& name, const wxVariant& value)
{
    wxVariantData* data = value.GetData();

    // Free old, if any
    auto  it = m_map.find(name);
    if ( it != m_map.end() )
    {
        it->second->DecRef();

        if ( !data )
        {
            // If Null variant, just remove from set
            m_map.erase(it);
            return;
        }
    }

    if ( data )
    {
        data->IncRef();

        m_map[name] = data;
    }
}

wxVariant wxPGAttributeStorage::FindValue(const wxString& name) const
{
    auto it = m_map.find(name);
    if ( it != m_map.end() )
    {
        wxVariantData* data = it->second;
        data->IncRef();
        return wxVariant(data, it->first);
    }
    return wxVariant();
}

wxPGAttributeStorage::const_iterator wxPGAttributeStorage::StartIteration() const
{
    return m_map.begin();
}

bool wxPGAttributeStorage::GetNext(const_iterator& it, wxVariant& variant) const
{
    if ( it == m_map.end() )
        return false;

    wxVariantData* data = it->second;
    data->IncRef();
    variant.SetData(data);
    variant.SetName(it->first);
    ++it;
    return true;
}

#endif  // wxUSE_PROPGRID
