/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/property.cpp
// Purpose:     wxPGProperty and related support classes
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-23
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/dcmemory.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
#endif

#include <wx/propgrid/propgrid.h>


#define PWC_CHILD_SUMMARY_LIMIT         16 // Maximum number of children summarized in a parent property's
                                           // value field.

#define PWC_CHILD_SUMMARY_CHAR_LIMIT    64 // Character limit of summary field when not editing


// -----------------------------------------------------------------------

static void wxPGDrawFocusRect( wxDC& dc, const wxRect& rect )
{
#if defined(__WXMSW__) && !defined(__WXWINCE__)
    // FIXME: Use DrawFocusRect code above (currently it draws solid line
    //   for caption focus but works ok for other stuff).
    //   Also, it seems that this code may not work in future wx versions.
    dc.SetLogicalFunction(wxINVERT);

    wxPen pen(*wxBLACK,1,wxDOT);
    pen.SetCap(wxCAP_BUTT);
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
#else
    dc.SetLogicalFunction(wxINVERT);

    dc.SetPen(wxPen(*wxBLACK,1,wxDOT));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
#endif
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
    if ( xOffset )
        xOffset += wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
    dc.DrawText( text,
                 rect.x+xOffset+wxPG_XBEFORETEXT,
                 rect.y+((rect.height-dc.GetCharHeight())/2) );
}

void wxPGCellRenderer::DrawEditorValue( wxDC& dc, const wxRect& rect,
                                        int xOffset, const wxString& text,
                                        wxPGProperty* property,
                                        const wxPGEditor* editor ) const
{
    if ( xOffset )
        xOffset += wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;

    int yOffset = ((rect.height-dc.GetCharHeight())/2);

    if ( editor )
    {
        wxRect rect2(rect); 
        rect2.x += xOffset;
        rect2.y += yOffset;
        rect2.height -= yOffset;
        editor->DrawValue( dc, rect2, property, text );
    }
    else
    {
        dc.DrawText( text,
                     rect.x+xOffset+wxPG_XBEFORETEXT,
                     rect.y+yOffset );
    }
}

void wxPGCellRenderer::DrawCaptionSelectionRect( wxDC& dc, int x, int y, int w, int h ) const
{
    wxRect focusRect(x,y+((h-dc.GetCharHeight())/2),w,h);
    wxPGDrawFocusRect(dc,focusRect);
}

int wxPGCellRenderer::PreDrawCell( wxDC& dc, const wxRect& rect, const wxPGCell& cell, int flags ) const
{
    int imageOffset = 0;

    if ( !(flags & Selected) )
    {
        // Draw using wxPGCell information, if available
        wxColour fgCol = cell.GetFgCol();
        if ( fgCol.Ok() )
            dc.SetTextForeground(fgCol);

        wxColour bgCol = cell.GetBgCol();
        if ( bgCol.Ok() )
        {
            dc.SetPen(bgCol);
            dc.SetBrush(bgCol);
            dc.DrawRectangle(rect);
        }
    }

    const wxBitmap& bmp = cell.GetBitmap();
    if ( bmp.Ok() &&
        // In control, do not draw oversized bitmap
         (!(flags & Control) || bmp.GetHeight() < rect.height )
        )
    {
        dc.DrawBitmap( bmp,
                       rect.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                       rect.y + wxPG_CUSTOM_IMAGE_SPACINGY,
                       true );
        imageOffset = bmp.GetWidth();
    }

    return imageOffset;
}

// -----------------------------------------------------------------------
// wxPGDefaultRenderer
// -----------------------------------------------------------------------

void wxPGDefaultRenderer::Render( wxDC& dc, const wxRect& rect,
                                  const wxPropertyGrid* propertyGrid, wxPGProperty* property,
                                  int column, int item, int flags ) const
{
    bool isUnspecified = property->IsValueUnspecified();

    if ( column == 1 && item == -1 )
    {
        int cmnVal = property->GetCommonValue();
        if ( cmnVal >= 0 )
        {
            // Common Value
            if ( !isUnspecified )
                DrawText( dc, rect, 0, propertyGrid->GetCommonValueLabel(cmnVal) );
            return;
        }
    }

    const wxPGEditor* editor = NULL;
    const wxPGCell* cell = property->GetCell(column);

    wxString text;
    int imageOffset = 0;

    // Use choice cell?
    if ( column == 1 && (flags & Control) )
    {
        int selectedIndex = property->GetChoiceSelection();
        if ( selectedIndex != wxNOT_FOUND )
        {
            const wxPGChoices& choices = property->GetChoices();
            const wxPGCell* ccell = &choices[selectedIndex];
            if ( ccell &&
                 ( ccell->GetBitmap().IsOk() || ccell->GetFgCol().IsOk() || ccell->GetBgCol().IsOk() )
               )
                cell = ccell;
        }
    }

    if ( cell )
    {
        int preDrawFlags = flags;

        if ( propertyGrid->GetInternalFlags() & wxPG_FL_CELL_OVERRIDES_SEL )
            preDrawFlags = preDrawFlags & ~(Selected);

        imageOffset = PreDrawCell( dc, rect, *cell, preDrawFlags );
        text = cell->GetText();
        if ( text == wxS("@!") )
        {
            if ( column == 0 )
                text = property->GetLabel();
            else if ( column == 1 )
                text = property->GetValueString();
            else
                text = wxEmptyString;
        }
    }
    else if ( column == 0 )
    {
        // Caption
        DrawText( dc, rect, 0, property->GetLabel() );
    }
    else if ( column == 1 )
    {
        if ( !isUnspecified )
        {
            editor = property->GetColumnEditor(column);

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

                /*if ( imageSize.x == wxPG_FULL_CUSTOM_PAINT_WIDTH )
                {
                    imageRect.width = m_width - imageRect.x;
                }*/

                dc.SetPen( wxPen(propertyGrid->GetCellTextColour(), 1, wxSOLID) );

                paintdata.m_drawnWidth = imageSize.x;
                paintdata.m_drawnHeight = imageSize.y;

                if ( !isUnspecified )
                {
                    property->OnCustomPaint( dc, imageRect, paintdata );
                }
                else
                {
                    dc.SetBrush(*wxWHITE_BRUSH);
                    dc.DrawRectangle(imageRect);
                }

                imageOffset = paintdata.m_drawnWidth;
            }

            text = property->GetValueString();

            // Add units string?
            if ( propertyGrid->GetColumnCount() <= 2 )
            {
                wxString unitsString = property->GetAttribute(wxPGGlobalVars->m_strUnits, wxEmptyString);
                if ( unitsString.length() )
                    text = wxString::Format(wxS("%s %s"), text.c_str(), unitsString.c_str() );
            }
        }

        if ( text.length() == 0 )
        {
            // Try to show inline help if no text
            wxVariant vInlineHelp = property->GetAttribute(wxPGGlobalVars->m_strInlineHelp);
            if ( !vInlineHelp.IsNull() )
            {
                text = vInlineHelp.GetString();
                dc.SetTextForeground(propertyGrid->GetCellDisabledTextColour());
            }
        }
    }
    else if ( column == 2 )
    {
        // Add units string?
        if ( !text.length() )
            text = property->GetAttribute(wxPGGlobalVars->m_strUnits, wxEmptyString);
    }

    DrawEditorValue( dc, rect, imageOffset, text, property, editor );

    // active caption gets nice dotted rectangle
    if ( property->IsCategory() /*&& column == 0*/ )
    {
        if ( flags & Selected )
        {
            if ( imageOffset > 0 )
                imageOffset += wxCC_CUSTOM_IMAGE_MARGIN2 + 4;

            DrawCaptionSelectionRect( dc,
                                      rect.x+wxPG_XBEFORETEXT-wxPG_CAPRECTXMARGIN+imageOffset,
                                      rect.y-wxPG_CAPRECTYMARGIN+1,
                                      ((wxPropertyCategory*)property)->GetTextExtent(propertyGrid,
                                                                                     propertyGrid->GetCaptionFont())
                                      +(wxPG_CAPRECTXMARGIN*2),
                                      propertyGrid->GetFontHeight()+(wxPG_CAPRECTYMARGIN*2) );
        }
    }
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

            if ( bmp && bmp->Ok() )
                return wxSize(bmp->GetWidth(),bmp->GetHeight());
        }
    }
    return wxSize(0,0);
}

// -----------------------------------------------------------------------
// wxPGCell
// -----------------------------------------------------------------------

wxPGCell::wxPGCell()
{
}

wxPGCell::wxPGCell( const wxString& text,
                    const wxBitmap& bitmap,
                    const wxColour& fgCol,
                    const wxColour& bgCol )
    : m_bitmap(bitmap), m_fgCol(fgCol), m_bgCol(bgCol)
{
    m_text = text;
}

// -----------------------------------------------------------------------
// wxPGProperty
// -----------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPGProperty, wxObject)

wxString* wxPGProperty::sm_wxPG_LABEL = NULL;

void wxPGProperty::Init()
{
    m_commonValue = -1;
    m_arrIndex = 0xFFFF;
    m_parent = NULL;

    m_parentState = (wxPropertyGridPageState*) NULL;

    m_clientData = NULL;
    m_clientObject = NULL;

    m_customEditor = (wxPGEditor*) NULL;
#if wxUSE_VALIDATORS
    m_validator = (wxValidator*) NULL;
#endif
    m_valueBitmap = (wxBitmap*) NULL;

    m_maxLen = 0; // infinite maximum length

    m_flags = wxPG_PROP_PROPERTY;

    m_depth = 1;
    m_bgColIndex = 0;
    m_fgColIndex = 0;

    SetExpanded(true);
}


void wxPGProperty::Init( const wxString& label, const wxString& name )
{
    // We really need to check if &label and &name are NULL pointers
    // (this can if we are called before property grid has been initalized)

    if ( (&label) != NULL && label != wxPG_LABEL )
        m_label = label;

    if ( (&name) != NULL && name != wxPG_LABEL )
        DoSetName( name );
    else
        DoSetName( m_label );

    Init();
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

    delete m_valueBitmap;
#if wxUSE_VALIDATORS
    delete m_validator;
#endif

    unsigned int i;

    for ( i=0; i<m_cells.size(); i++ )
        delete m_cells[i];

    // This makes it easier for us to detect dangling pointers
    m_parent = NULL;
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


wxString wxPGProperty::GetName() const
{
    wxPGProperty* parent = GetParent();

    if ( !m_name.length() || !parent || parent->IsCategory() || parent->IsRoot() )
        return m_name;

    return m_parent->GetName() + wxS(".") + m_name;
}

wxPropertyGrid* wxPGProperty::GetGrid() const
{
    if ( !m_parentState )
        return NULL;
    return m_parentState->GetGrid();
}

int wxPGProperty::Index( const wxPGProperty* p ) const
{
    for ( unsigned int i = 0; i<m_children.size(); i++ )
    {
        if ( p == m_children[i] )
            return i;
    }
    return wxNOT_FOUND;
}

void wxPGProperty::UpdateControl( wxWindow* primary )
{
    if ( primary )
        GetEditorClass()->UpdateControl(this, primary);
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

wxString wxPGProperty::GetColumnText( unsigned int col ) const
{
    wxPGCell* cell = GetCell(col);
    if ( cell )
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
            return GetAttribute(wxPGGlobalVars->m_strUnits, wxEmptyString);
    }

    return wxEmptyString;
}

void wxPGProperty::GenerateComposedValue( wxString& text, int argFlags ) const
{
    int i;
    int iMax = m_children.size();

    text.clear();
    if ( iMax == 0 )
        return;

    if ( iMax > PWC_CHILD_SUMMARY_LIMIT &&
         !(argFlags & wxPG_FULL_VALUE) )
        iMax = PWC_CHILD_SUMMARY_LIMIT;

    int iMaxMinusOne = iMax-1;

    if ( !IsTextEditable() )
        argFlags |= wxPG_UNEDITABLE_COMPOSITE_FRAGMENT;

    wxPGProperty* curChild = m_children[0];

    for ( i = 0; i < iMax; i++ )
    {
        wxString s;
        if ( !curChild->IsValueUnspecified() )
            s = curChild->GetValueString(argFlags|wxPG_COMPOSITE_FRAGMENT);

        bool skip = false;
        if ( (argFlags & wxPG_UNEDITABLE_COMPOSITE_FRAGMENT) && !s.length() )
            skip = true;

        if ( !curChild->GetChildCount() || skip )
            text += s;
        else
            text += wxS("[") + s + wxS("]");

        if ( i < iMaxMinusOne )
        {
            if ( text.length() > PWC_CHILD_SUMMARY_CHAR_LIMIT &&
                 !(argFlags & wxPG_EDITABLE_VALUE) &&
                 !(argFlags & wxPG_FULL_VALUE) )
                break;

            if ( !skip )
            {
                if ( !curChild->GetChildCount() )
                    text += wxS("; ");
                else
                    text += wxS(" ");
            }

            curChild = m_children[i+1];
        }
    }

    // Remove superfluous semicolon and space
    wxString rest;
    if ( text.EndsWith(wxS("; "), &rest) )
        text = rest;

    if ( (unsigned int)i < m_children.size() )
        text += wxS("; ...");
}

wxString wxPGProperty::GetValueAsString( int argFlags ) const
{
    wxCHECK_MSG( GetChildCount() > 0,
                 wxString(),
                 wxT("If user property does not have any children, it must override GetValueAsString") );

    wxString text;
    GenerateComposedValue(text, argFlags);
    return text;
}

wxString wxPGProperty::GetValueString( int argFlags ) const
{
    if ( IsValueUnspecified() )
        return wxEmptyString;

    if ( m_commonValue == -1 )
        return GetValueAsString(argFlags);

    //
    // Return common value's string representation
    wxPropertyGrid* pg = GetGrid();
    const wxPGCommonValue* cv = pg->GetCommonValue(m_commonValue);

    if ( argFlags & wxPG_FULL_VALUE )
    {
        return cv->GetLabel();
    }
    else if ( argFlags & wxPG_EDITABLE_VALUE )
    {
        return cv->GetEditableText();
    }
    else
    {
        return cv->GetLabel();
    }
}

bool wxPGProperty::IntToValue( wxVariant& variant, int number, int WXUNUSED(argFlags) ) const
{
    variant = (long)number;
    return true;
}

// Convert semicolon delimited tokens into child values.
bool wxPGProperty::StringToValue( wxVariant& variant, const wxString& text, int argFlags ) const
{
    if ( !GetChildCount() )
        return false;

    unsigned int curChild = 0;

    unsigned int iMax = m_children.size();

    if ( iMax > PWC_CHILD_SUMMARY_LIMIT &&
         !(argFlags & wxPG_FULL_VALUE) )
        iMax = PWC_CHILD_SUMMARY_LIMIT;

    bool changed = false;

    wxString token;
    size_t pos = 0;

    // Its best only to add non-empty group items
    bool addOnlyIfNotEmpty = false;
    const wxChar delimeter = wxS(';');

    size_t tokenStart = 0xFFFFFF;

    wxVariantList temp_list;
    wxVariant list(temp_list);

    int propagatedFlags = argFlags & wxPG_REPORT_ERROR;

#ifdef __WXDEBUG__
    bool debug_print = false;
#endif

#ifdef __WXDEBUG__
    if ( debug_print )
        wxLogDebug(wxT(">> %s.StringToValue('%s')"),GetLabel().c_str(),text.c_str());
#endif

    wxString::const_iterator it = text.begin();
    wxUniChar a;

    if ( it != text.end() )
        a = *it;
    else
        a = 0;

    for ( ;; )
    {
        if ( tokenStart != 0xFFFFFF )
        {
            // Token is running
            if ( a == delimeter || a == 0 )
            {
                token = text.substr(tokenStart,pos-tokenStart);
                token.Trim(true);
                size_t len = token.length();

                if ( !addOnlyIfNotEmpty || len > 0 )
                {
                    const wxPGProperty* child = Item(curChild);
                #ifdef __WXDEBUG__
                    if ( debug_print )
                        wxLogDebug(wxT("token = '%s', child = %s"),token.c_str(),child->GetLabel().c_str());
                #endif

                    if ( len > 0 )
                    {
                        bool wasUnspecified = child->IsValueUnspecified();

                        wxVariant variant(child->GetValueRef());
                        if ( child->StringToValue(variant, token, propagatedFlags|wxPG_COMPOSITE_FRAGMENT) )
                        {
                            variant.SetName(child->GetBaseName());

                            // Clear unspecified flag only if OnSetValue() didn't
                            // affect it.
                            if ( child->IsValueUnspecified() &&
                                 (wasUnspecified || !UsesAutoUnspecified()) )
                            {
                                variant = child->GetDefaultValue();
                            }

                            list.Append(variant);

                            changed = true;
                        }
                    }
                    else
                    {
                        // Empty, becomes unspecified
                        wxVariant variant2;
                        variant2.SetName(child->GetBaseName());
                        list.Append(variant2);
                        changed = true;
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

                    if ( it != text.end() ) it++;
                    pos++;
                    size_t startPos = pos;

                    // Group item - find end
                    while ( it != text.end() && depth > 0 )
                    {
                        a = *it;
                        it++;
                        pos++;

                        if ( a == wxS(']') )
                            depth--;
                        else if ( a == wxS('[') )
                            depth++;
                    }

                    token = text.substr(startPos,pos-startPos-1);

                    if ( !token.length() )
                        break;

                    const wxPGProperty* child = Item(curChild);

                    wxVariant oldChildValue = child->GetValue();
                    wxVariant variant(oldChildValue);
                    bool stvRes = child->StringToValue( variant, token, propagatedFlags );
                    if ( stvRes || (variant != oldChildValue) )
                    {
                        if ( stvRes )
                            changed = true;
                    }
                    else
                    {
                        // Failed, becomes unspecified
                        variant.MakeNull();
                        changed = true;
                    }

                    variant.SetName(child->GetBaseName());
                    list.Append(variant);

                    curChild++;
                    if ( curChild >= iMax )
                        break;

                    addOnlyIfNotEmpty = true;

                    tokenStart = 0xFFFFFF;
                }
                else
                {
                    tokenStart = pos;

                    if ( a == delimeter )
                    {
                        pos--;
                        it--;
                    }
                }
            }
        }

        if ( a == 0 )
            break;

        it++;
        if ( it != text.end() )
        {
            a = *it;
        }
        else
        {
            a = 0;
        }
        pos++;
    }

    if ( changed )
        variant = list;

    return changed;
}

bool wxPGProperty::SetValueFromString( const wxString& text, int argFlags )
{
    wxVariant variant(m_value);
    bool res = StringToValue(variant, text, argFlags);
    if ( res )
        SetValue(variant);
    return res;
}

bool wxPGProperty::SetValueFromInt( long number, int argFlags )
{
    wxVariant variant(m_value);
    bool res = IntToValue(variant, number, argFlags);
    if ( res )
        SetValue(variant);
    return res;
}

wxSize wxPGProperty::OnMeasureImage( int WXUNUSED(item) ) const
{
    if ( m_valueBitmap )
        return wxSize(m_valueBitmap->GetWidth(),-1);

    return wxSize(0,0);
}

wxPGCellRenderer* wxPGProperty::GetCellRenderer( int WXUNUSED(column) ) const
{
    return wxPGGlobalVars->m_defaultRenderer;
}

void wxPGProperty::OnCustomPaint( wxDC& dc,
                                  const wxRect& rect,
                                  wxPGPaintData& )
{
    wxBitmap* bmp = m_valueBitmap;

    wxCHECK_RET( bmp && bmp->Ok(), wxT("invalid bitmap") );

    wxCHECK_RET( rect.x >= 0, wxT("unexpected measure call") );

    dc.DrawBitmap(*bmp,rect.x,rect.y);
}

const wxPGEditor* wxPGProperty::DoGetEditorClass() const
{
    return wxPG_EDITOR(TextCtrl);
}

// Default extra property event handling - that is, none at all.
bool wxPGProperty::OnEvent( wxPropertyGrid*, wxWindow*, wxEvent& )
{
    return false;
}


void wxPGProperty::SetValue( wxVariant value, wxVariant* pList, int flags )
{
    if ( !value.IsNull() )
    {
        wxVariant tempListVariant;

        SetCommonValue(-1);
        // List variants are reserved a special purpose
        // as intermediate containers for child values
        // of properties with children.
        if ( value.GetType() == wxPG_VARIANT_TYPE_LIST )
        {
            //
            // However, situation is different for composed string properties
            if ( HasFlag(wxPG_PROP_COMPOSED_VALUE) )
            {
                tempListVariant = value;
                pList = &tempListVariant;
            }

            wxVariant newValue;
            AdaptListToValue(value, &newValue);
            value = newValue;
            //wxLogDebug(wxT(">> %s.SetValue() adapted list value to type '%s'"),GetName().c_str(),value.GetType().c_str());
        }

        if ( HasFlag( wxPG_PROP_AGGREGATE) )
            flags |= wxPG_SETVAL_AGGREGATED;

        if ( pList && !pList->IsNull() )
        {
            wxASSERT( pList->GetType() == wxPG_VARIANT_TYPE_LIST );
            wxASSERT( GetChildCount() );
            wxASSERT( !IsCategory() );

            wxVariantList& list = pList->GetList();
            wxVariantList::iterator node;
            unsigned int i = 0;

            //wxLogDebug(wxT(">> %s.SetValue() pList parsing"),GetName().c_str());

            // Children in list can be in any order, but we will give hint to
            // GetPropertyByNameWH(). This optimizes for full list parsing.
            for ( node = list.begin(); node != list.end(); node++ )
            {
                wxVariant& childValue = *((wxVariant*)*node);
                wxPGProperty* child = GetPropertyByNameWH(childValue.GetName(), i);
                if ( child )
                {
                    //wxLogDebug(wxT("%i: child = %s, childValue.GetType()=%s"),i,child->GetBaseName().c_str(),childValue.GetType().c_str());
                    if ( childValue.GetType() == wxPG_VARIANT_TYPE_LIST )
                    {
                        if ( child->HasFlag(wxPG_PROP_AGGREGATE) && !(flags & wxPG_SETVAL_AGGREGATED) )
                        {
                            wxVariant listRefCopy = childValue;
                            child->SetValue(childValue, &listRefCopy, flags|wxPG_SETVAL_FROM_PARENT);
                        }
                        else
                        {
                            wxVariant oldVal = child->GetValue();
                            child->SetValue(oldVal, &childValue, flags|wxPG_SETVAL_FROM_PARENT);
                        }
                    }
                    else if ( child->GetValue() != childValue )
                    {
                        // For aggregate properties, we will trust RefreshChildren()
                        // to update child values.
                        if ( !HasFlag(wxPG_PROP_AGGREGATE) )
                            child->SetValue(childValue, NULL, flags|wxPG_SETVAL_FROM_PARENT);
                        if ( flags & wxPG_SETVAL_BY_USER )
                            child->SetFlag(wxPG_PROP_MODIFIED);
                    }
                }
                i++;
            }
        }

        if ( !value.IsNull() )
        {
            m_value = value;
            OnSetValue();

            if ( !(flags & wxPG_SETVAL_FROM_PARENT) )
                UpdateParentValues();
        }

        if ( flags & wxPG_SETVAL_BY_USER )
            SetFlag(wxPG_PROP_MODIFIED);

        if ( HasFlag(wxPG_PROP_AGGREGATE) )
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
            unsigned int i;
            for ( i=0; i<GetChildCount(); i++ )
                Item(i)->SetValue(value, NULL, flags|wxPG_SETVAL_FROM_PARENT);
        }
    }

    //
    // Update editor control
    //

    // We need to check for these, otherwise GetGrid() may fail.
    if ( flags & wxPG_SETVAL_REFRESH_EDITOR )
        RefreshEditor();
}


void wxPGProperty::SetValueInEvent( wxVariant value ) const
{
    GetGrid()->ValueChangeInEvent(value);
}

void wxPGProperty::SetFlagRecursively( FlagType flag, bool set )
{
    if ( set )
        SetFlag(flag);
    else
        ClearFlag(flag);

    unsigned int i;
    for ( i = 0; i < GetChildCount(); i++ )
        Item(i)->SetFlagRecursively(flag, set);
}

void wxPGProperty::RefreshEditor()
{
    if ( m_parent && GetParentState() )
    {
        wxPropertyGrid* pg = GetParentState()->GetGrid();
        if ( pg->GetSelectedProperty() == this )
        {
            wxWindow* editor = pg->GetEditorControl();
            if ( editor )
                GetEditorClass()->UpdateControl( this, editor );
        }
    }
}


wxVariant wxPGProperty::GetDefaultValue() const
{
    wxVariant defVal = GetAttribute(wxS("DefaultValue"));
    if ( !defVal.IsNull() )
        return defVal;

    wxVariant value = GetValue();

    if ( !value.IsNull() )
    {
        wxString valueType(value.GetType());

        if ( valueType == wxPG_VARIANT_TYPE_LONG )
            return wxPGVariant_Zero;
        if ( valueType == wxPG_VARIANT_TYPE_STRING )
            return wxPGVariant_EmptyString;
        if ( valueType == wxPG_VARIANT_TYPE_BOOL )
            return wxPGVariant_False;
        if ( valueType == wxPG_VARIANT_TYPE_DOUBLE )
            return wxVariant(0.0);
        if ( valueType == wxPG_VARIANT_TYPE_ARRSTRING )
            return wxVariant(wxArrayString());
        if ( valueType == wxS("wxLongLong") )
            return WXVARIANT(wxLongLong(0));
        if ( valueType == wxS("wxULongLong") )
            return WXVARIANT(wxULongLong(0));
        if ( valueType == wxS("wxColour") )
            return WXVARIANT(*wxBLACK);
#if wxUSE_DATETIME
        if ( valueType == wxPG_VARIANT_TYPE_DATETIME )
            return wxVariant(wxDateTime::Now());
#endif
        if ( valueType == wxS("wxFont") )
            return WXVARIANT(*wxNORMAL_FONT);
        if ( valueType == wxS("wxPoint") )
            return WXVARIANT(wxPoint(0, 0));
        if ( valueType == wxS("wxSize") )
            return WXVARIANT(wxSize(0, 0));
    }

    return wxVariant();
}

void wxPGProperty::SetCell( int column, wxPGCell* cellObj )
{
    if ( column >= (int)m_cells.size() )
        m_cells.SetCount(column+1, NULL);

    delete (wxPGCell*) m_cells[column];
    m_cells[column] = cellObj;
}

wxPGEditorDialogAdapter* wxPGProperty::GetEditorDialog() const
{
    return NULL;
}

bool wxPGProperty::DoSetAttribute( const wxString& WXUNUSED(name), wxVariant& WXUNUSED(value) )
{
    return false;
}

void wxPGProperty::SetAttribute( const wxString& name, wxVariant value )
{
    if ( DoSetAttribute( name, value ) )
    {
        // Support working without grid, when possible
        if ( wxPGGlobalVars->HasExtraStyle( wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES ) )
            return;
    }

    m_attributes.Set( name, value );
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
    return m_attributes.FindValue(name);
}

wxString wxPGProperty::GetAttribute( const wxString& name, const wxString& defVal ) const
{
    wxVariant variant = m_attributes.FindValue(name);

    if ( !variant.IsNull() )
        return variant.GetString();

    return defVal;
}

long wxPGProperty::GetAttributeAsLong( const wxString& name, long defVal ) const
{
    wxVariant variant = m_attributes.FindValue(name);

    return wxPGVariantToInt(variant, defVal);
}

double wxPGProperty::GetAttributeAsDouble( const wxString& name, double defVal ) const
{
    double retVal;
    wxVariant variant = m_attributes.FindValue(name);

    if ( wxPGVariantToDouble(variant, &retVal) )
        return retVal;

    return defVal;
}

wxVariant wxPGProperty::GetAttributesAsList() const
{
    wxVariantList tempList;
    wxVariant v( tempList, wxString::Format(wxS("@%s@attr"),m_name.c_str()) );

    wxPGAttributeStorage::const_iterator it = m_attributes.StartIteration();
    wxVariant variant;

    while ( m_attributes.GetNext(it, variant) )
        v.Append(variant);

    return v;
}

// Slots of utility flags are NULL
const unsigned int gs_propFlagToStringSize = 14;

static const wxChar* gs_propFlagToString[gs_propFlagToStringSize] = {
    NULL,
    wxT("DISABLED"),
    wxT("HIDDEN"),
    NULL,
    wxT("NOEDITOR"),
    wxT("COLLAPSED"),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

wxString wxPGProperty::GetFlagsAsString( FlagType flagsMask ) const
{
    wxString s;
    int relevantFlags = m_flags & flagsMask & wxPG_STRING_STORED_FLAGS;
    FlagType a = 1;

    unsigned int i = 0;
    for ( i=0; i<gs_propFlagToStringSize; i++ )
    {
        if ( relevantFlags & a )
        {
            const wxChar* fs = gs_propFlagToString[i];
            wxASSERT(fs);
            if ( s.length() )
                s << wxS("|");
            s << fs;
        }
        a = a << 1;
    }

    return s;
}

void wxPGProperty::SetFlagsFromString( const wxString& str )
{
    FlagType flags = 0;

    WX_PG_TOKENIZER1_BEGIN(str, wxS('|'))
        unsigned int i;
        for ( i=0; i<gs_propFlagToStringSize; i++ )
        {
            const wxChar* fs = gs_propFlagToString[i];
            if ( fs && str == fs )
            {
                flags |= (1<<i);
                break;
            }
        }
    WX_PG_TOKENIZER1_END()

    m_flags = (m_flags & ~wxPG_STRING_STORED_FLAGS) | flags;
}

wxValidator* wxPGProperty::DoGetValidator() const
{
    return (wxValidator*) NULL;
}

int wxPGProperty::InsertChoice( const wxString& label, int index, int value )
{
    wxPropertyGrid* pg = GetGrid();
    int sel = GetChoiceSelection();

    int newSel = sel;

    if ( index == wxNOT_FOUND )
        index = m_choices.GetCount();

    if ( index <= sel )
        newSel++;

    m_choices.Insert(label, index, value);

    if ( sel != newSel )
        SetChoiceSelection(newSel);

    if ( this == pg->GetSelection() )
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

    if ( this == pg->GetSelection() )
        GetEditorClass()->DeleteItem(pg->GetEditorControl(), index);
}

int wxPGProperty::GetChoiceSelection() const
{
    wxVariant value = GetValue();
    wxString valueType = value.GetType();
    int index = wxNOT_FOUND;

    if ( IsValueUnspecified() || !m_choices.GetCount() )
        return wxNOT_FOUND;

    if ( valueType == wxPG_VARIANT_TYPE_LONG )
    {
        index = value.GetLong();
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
    // Changes value of a property with choices, but only
    // works if the value type is long or string.
    wxString valueType = GetValue().GetType();

    wxCHECK_RET( m_choices.IsOk(), wxT("invalid choiceinfo") );

    if ( valueType == wxPG_VARIANT_TYPE_STRING )
    {
        SetValue( m_choices.GetLabel(newValue) );
    }
    else  // if ( valueType == wxPG_VARIANT_TYPE_LONG )
    {
        SetValue( (long) newValue );
    }
}

bool wxPGProperty::SetChoices( wxPGChoices& choices )
{
    m_choices.Assign(choices);

    {
        // This may be needed to trigger some initialization
        // (but don't do it if property is somewhat uninitialized)
        wxVariant defVal = GetDefaultValue();
        if ( defVal.IsNull() )
            return false;

        SetValue(defVal);
    }

    return true;
}


const wxPGEditor* wxPGProperty::GetEditorClass() const
{
    const wxPGEditor* editor;

    if ( !m_customEditor )
    {
        editor = DoGetEditorClass();
    }
    else
        editor = m_customEditor;

    //
    // Maybe override editor if common value specified
    if ( GetDisplayedCommonValueCount() )
    {
        // TextCtrlAndButton -> ComboBoxAndButton
        if ( editor->IsKindOf(CLASSINFO(wxPGTextCtrlAndButtonEditor)) )
            editor = wxPG_EDITOR(ChoiceAndButton);

        // TextCtrl -> ComboBox
        else if ( editor->IsKindOf(CLASSINFO(wxPGTextCtrlEditor)) )
            editor = wxPG_EDITOR(ComboBox);
    }

    return editor;
}

bool wxPGProperty::HasVisibleChildren() const
{
    unsigned int i;

    for ( i=0; i<GetChildCount(); i++ )
    {
        wxPGProperty* child = Item(i);

        if ( !child->HasFlag(wxPG_PROP_HIDDEN) )
            return true;
    }

    return false;
}

bool wxPGProperty::PrepareValueForDialogEditing( wxPropertyGrid* propGrid )
{
    return propGrid->EditorValidate();
}


bool wxPGProperty::RecreateEditor()
{
    wxPropertyGrid* pg = GetGrid();
    wxASSERT(pg);

    wxPGProperty* selected = pg->GetSelection();
    if ( this == selected )
    {
        pg->DoSelectProperty(this, wxPG_SEL_FORCE);
        return true;
    }
    return false;
}


void wxPGProperty::SetValueImage( wxBitmap& bmp )
{
    delete m_valueBitmap;

    if ( &bmp && bmp.Ok() )
    {
        // Resize the image
        wxSize maxSz = GetGrid()->GetImageSize();
        wxSize imSz(bmp.GetWidth(),bmp.GetHeight());

        if ( imSz.x != maxSz.x || imSz.y != maxSz.y )
        {
            // Create a memory DC
            wxBitmap* bmpNew = new wxBitmap(maxSz.x,maxSz.y,bmp.GetDepth());

            wxMemoryDC dc;
            dc.SelectObject(*bmpNew);

            // Scale
            // FIXME: This is ugly - use image or wait for scaling patch.
            double scaleX = (double)maxSz.x / (double)imSz.x;
            double scaleY = (double)maxSz.y / (double)imSz.y;

            dc.SetUserScale(scaleX,scaleY);

            dc.DrawBitmap( bmp, 0, 0 );

            m_valueBitmap = bmpNew;
        }
        else
        {
            m_valueBitmap = new wxBitmap(bmp);
        }

        m_flags |= wxPG_PROP_CUSTOMIMAGE;
    }
    else
    {
        m_valueBitmap = NULL;
        m_flags &= ~(wxPG_PROP_CUSTOMIMAGE);
    }
}


wxPGProperty* wxPGProperty::GetMainParent() const
{
    const wxPGProperty* curChild = this;
    const wxPGProperty* curParent = m_parent;

    while ( curParent && !curParent->IsCategory() )
    {
        curChild = curParent;
        curParent = curParent->m_parent;
    }

    return (wxPGProperty*) curChild;
}


const wxPGProperty* wxPGProperty::GetLastVisibleSubItem() const
{
    //
    // Returns last visible sub-item, recursively.
    if ( !IsExpanded() || !GetChildCount() )
        return this;

    return Last()->GetLastVisibleSubItem();
}


bool wxPGProperty::IsVisible() const
{
    const wxPGProperty* parent;

    if ( HasFlag(wxPG_PROP_HIDDEN) )
        return false;

    for ( parent = GetParent(); parent != NULL; parent = parent->GetParent() )
    {
        if ( !parent->IsExpanded() || parent->HasFlag(wxPG_PROP_HIDDEN) )
            return false;
    }

    return true;
}

wxPropertyGrid* wxPGProperty::GetGridIfDisplayed() const
{
    wxPropertyGridPageState* state = GetParentState();
    wxPropertyGrid* propGrid = state->GetGrid();
    if ( state == propGrid->GetState() )
        return propGrid;
    return NULL;
}


int wxPGProperty::GetY2( int lh ) const
{
    const wxPGProperty* parent;
    const wxPGProperty* child = this;

    int y = 0;

    for ( parent = GetParent(); parent != NULL; parent = child->GetParent() )
    {
        if ( !parent->IsExpanded() )
            return -1;
        y += parent->GetChildrenHeight(lh, child->GetIndexInParent());
        y += lh;
        child = parent;
    }

    y -= lh;  // need to reduce one level

    return y;
}


int wxPGProperty::GetY() const
{
    return GetY2(GetGrid()->GetRowHeight());
}

// This is used by Insert etc.
void wxPGProperty::AddChild2( wxPGProperty* prop, int index, bool correct_mode )
{
    if ( index < 0 || (size_t)index >= m_children.size() )
    {
        if ( correct_mode ) prop->m_arrIndex = m_children.size();
        m_children.push_back( prop );
    }
    else
    {
        m_children.insert( m_children.begin()+index, prop);
        if ( correct_mode ) FixIndexesOfChildren( index );
    }

    prop->m_parent = this;
}

// This is used by properties that have fixed sub-properties
void wxPGProperty::AddChild( wxPGProperty* prop )
{
    wxASSERT_MSG( prop->GetBaseName().length(),
                  "Property's children must have unique, non-empty names within their scope" );

    prop->m_arrIndex = m_children.size();
    m_children.push_back( prop );

    int custImgHeight = prop->OnMeasureImage().y;
    if ( custImgHeight < 0 /*|| custImgHeight > 1*/ )
        prop->m_flags |= wxPG_PROP_CUSTOMIMAGE;

    prop->m_parent = this;
}

void wxPGProperty::RemoveChild( wxPGProperty* p )
{
    wxArrayPGProperty::iterator it;
    wxArrayPGProperty& children = m_children;

    for ( it=children.begin(); it != children.end(); it++ )
    {
        if ( *it == p )
        {
            m_children.erase(it);
            break;
        }
    }
}

void wxPGProperty::AdaptListToValue( wxVariant& list, wxVariant* value ) const
{
    wxASSERT( GetChildCount() );
    wxASSERT( !IsCategory() );

    *value = GetValue();

    if ( !list.GetCount() )
        return;

    wxASSERT( GetChildCount() >= (unsigned int)list.GetCount() );

    bool allChildrenSpecified;

    // Don't fully update aggregate properties unless all children have
    // specified value
    if ( HasFlag(wxPG_PROP_AGGREGATE) )
        allChildrenSpecified = AreAllChildrenSpecified(&list);
    else
        allChildrenSpecified = true;

    wxVariant childValue = list[0];
    unsigned int i;
    unsigned int n = 0;

    //wxLogDebug(wxT(">> %s.AdaptListToValue()"),GetBaseName().c_str());

    for ( i=0; i<GetChildCount(); i++ )
    {
        const wxPGProperty* child = Item(i);

        if ( childValue.GetName() == child->GetBaseName() )
        {
            //wxLogDebug(wxT("  %s(n=%i), %s"),childValue.GetName().c_str(),n,childValue.GetType().c_str());

            if ( childValue.GetType() == wxPG_VARIANT_TYPE_LIST )
            {
                wxVariant cv2(child->GetValue());
                child->AdaptListToValue(childValue, &cv2);
                childValue = cv2;
            }

            if ( allChildrenSpecified )
                ChildChanged(*value, i, childValue);
            n++;
            if ( n == (unsigned int)list.GetCount() )
                break;
            childValue = list[n];
        }
    }
}


void wxPGProperty::FixIndexesOfChildren( size_t starthere )
{
    size_t i;
    for ( i=starthere;i<GetChildCount();i++)
        Item(i)->m_arrIndex = i;
}


// Returns (direct) child property with given name (or NULL if not found)
wxPGProperty* wxPGProperty::GetPropertyByName( const wxString& name ) const
{
    size_t i;

    for ( i=0; i<GetChildCount(); i++ )
    {
        wxPGProperty* p = Item(i);
        if ( p->m_name == name )
            return p;
    }

    // Does it have point, then?
    int pos = name.Find(wxS('.'));
    if ( pos <= 0 )
        return (wxPGProperty*) NULL;

    wxPGProperty* p = GetPropertyByName(name. substr(0,pos));

    if ( !p || !p->GetChildCount() )
        return NULL;

    return p->GetPropertyByName(name.substr(pos+1,name.length()-pos-1));
}

wxPGProperty* wxPGProperty::GetPropertyByNameWH( const wxString& name, unsigned int hintIndex ) const
{
    unsigned int i = hintIndex;

    if ( i >= GetChildCount() )
        i = 0;

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
    };

    return NULL;
}

int wxPGProperty::GetChildrenHeight( int lh, int iMax_ ) const
{
    // Returns height of children, recursively, and
    // by taking expanded/collapsed status into account.
    //
    // iMax is used when finding property y-positions.
    //
    unsigned int i = 0;
    int h = 0;

    if ( iMax_ == -1 )
        iMax_ = GetChildCount();

    unsigned int iMax = iMax_;

    wxASSERT( iMax <= GetChildCount() );

    if ( !IsExpanded() && GetParent() )
        return 0;

    while ( i < iMax )
    {
        wxPGProperty* pwc = (wxPGProperty*) Item(i);

        if ( !pwc->HasFlag(wxPG_PROP_HIDDEN) )
        {
            if ( !pwc->IsExpanded() ||
                 pwc->GetChildCount() == 0 )
                h += lh;
            else
                h += pwc->GetChildrenHeight(lh) + lh;
        }

        i++;
    }

    return h;
}

wxPGProperty* wxPGProperty::GetItemAtY( unsigned int y, unsigned int lh, unsigned int* nextItemY ) const
{
    wxASSERT( nextItemY );

    // Linear search at the moment
    //
    // nextItemY = y of next visible property, final value will be written back.
    wxPGProperty* result = NULL;
    wxPGProperty* current = NULL;
    unsigned int iy = *nextItemY;
    unsigned int i = 0;
    unsigned int iMax = GetChildCount();

    while ( i < iMax )
    {
        wxPGProperty* pwc = Item(i);

        if ( !pwc->HasFlag(wxPG_PROP_HIDDEN) )
        {
            // Found?
            if ( y < iy )
            {
                result = current;
                break;
            }

            iy += lh;

            if ( pwc->IsExpanded() &&
                 pwc->GetChildCount() > 0 )
            {
                result = (wxPGProperty*) pwc->GetItemAtY( y, lh, &iy );
                if ( result )
                    break;
            }

            current = pwc;
        }

        i++;
    }

    // Found?
    if ( !result && y < iy )
        result = current;

    *nextItemY = iy;

    /*
    if ( current )
        wxLogDebug(wxT("%s::GetItemAtY(%i) -> %s"),this->GetLabel().c_str(),y,current->GetLabel().c_str());
    else
        wxLogDebug(wxT("%s::GetItemAtY(%i) -> NULL"),this->GetLabel().c_str(),y);
    */

    return (wxPGProperty*) result;
}

void wxPGProperty::Empty()
{
    size_t i;
    if ( !HasFlag(wxPG_PROP_CHILDREN_ARE_COPIES) )
    {
        for ( i=0; i<GetChildCount(); i++ )
        {
            delete m_children[i];
        }
    }

    m_children.clear();
}

void wxPGProperty::ChildChanged( wxVariant& WXUNUSED(thisValue),
                                 int WXUNUSED(childIndex),
                                 wxVariant& WXUNUSED(childValue) ) const
{
}

bool wxPGProperty::AreAllChildrenSpecified( wxVariant* pendingList ) const
{
    unsigned int i;

    const wxVariantList* pList = NULL;
    wxVariantList::const_iterator node;

    if ( pendingList )
    {
        pList = &pendingList->GetList();
        node = pList->begin();
    }

    for ( i=0; i<GetChildCount(); i++ )
    {
        wxPGProperty* child = Item(i);
        const wxVariant* listValue = NULL;
        wxVariant value;

        if ( pendingList )
        {
            const wxString& childName = child->GetBaseName();

            for ( ; node != pList->end(); node++ )
            {
                const wxVariant& item = *((const wxVariant*)*node);
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
        if ( child->GetChildCount() )
        {
            const wxVariant* childList = NULL;

            if ( listValue && listValue->GetType() == wxPG_VARIANT_TYPE_LIST )
                childList = listValue;

            if ( !child->AreAllChildrenSpecified((wxVariant*)childList) )
                return false;
        }
    }

    return true;
}

wxPGProperty* wxPGProperty::UpdateParentValues()
{
    wxPGProperty* parent = m_parent;
    if ( parent && parent->HasFlag(wxPG_PROP_COMPOSED_VALUE) &&
         !parent->IsCategory() && !parent->IsRoot() )
    {
        wxString s;
        parent->GenerateComposedValue(s, 0);
        parent->m_value = s;
        return parent->UpdateParentValues();
    }
    return this;
}

bool wxPGProperty::IsTextEditable() const
{
    if ( HasFlag(wxPG_PROP_READONLY) )
        return false;

    if ( HasFlag(wxPG_PROP_NOEDITOR) &&
         (GetChildCount() ||
          wxString(GetEditorClass()->GetClassInfo()->GetClassName()).EndsWith(wxS("Button")))
       )
        return false;

    return true;
}

// Call for after sub-properties added with AddChild
void wxPGProperty::PrepareSubProperties()
{
    wxPropertyGridPageState* state = GetParentState();

    wxASSERT(state);

    if ( !GetChildCount() )
        return;

    wxByte depth = m_depth + 1;
    wxByte depthBgCol = m_depthBgCol;

    FlagType inheritFlags = m_flags & wxPG_INHERITED_PROPFLAGS;

    wxByte bgColIndex = m_bgColIndex;
    wxByte fgColIndex = m_fgColIndex;

    //
    // Set some values to the children
    //
    size_t i = 0;
    wxPGProperty* nparent = this;

    while ( i < nparent->GetChildCount() )
    {
        wxPGProperty* np = nparent->Item(i);

        np->m_parentState = state;
        np->m_flags |= inheritFlags; // Hideable also if parent.
        np->m_depth = depth;
        np->m_depthBgCol = depthBgCol;
        np->m_bgColIndex = bgColIndex;
        np->m_fgColIndex = fgColIndex;

        // Also handle children of children
        if ( np->GetChildCount() > 0 )
        {
            nparent = np;
            i = 0;

            // Init
            nparent->SetParentalType(wxPG_PROP_AGGREGATE);
            nparent->SetExpanded(false);
            depth++;
        }
        else
        {
            // Next sibling
            i++;
        }

        // After reaching last sibling, go back to processing
        // siblings of the parent
        while ( i >= nparent->GetChildCount() )
        {
            // Exit the loop when top parent hit
            if ( nparent == this )
                break;

            depth--;

            i = nparent->GetIndexInParent() + 1;
            nparent = nparent->GetParent();
        }
    }
}

// Call after fixed sub-properties added/removed after creation.
// if oldSelInd >= 0 and < new max items, then selection is
// moved to it. Note: oldSelInd -2 indicates that this property
// should be selected.
void wxPGProperty::SubPropsChanged( int oldSelInd )
{
    wxPropertyGridPageState* state = GetParentState();
    wxPropertyGrid* grid = state->GetGrid();

    PrepareSubProperties();

    wxPGProperty* sel = (wxPGProperty*) NULL;
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

// -----------------------------------------------------------------------
// wxPGRootProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPGRootProperty,none,TextCtrl)
IMPLEMENT_DYNAMIC_CLASS(wxPGRootProperty, wxPGProperty)


wxPGRootProperty::wxPGRootProperty()
    : wxPGProperty()
{
#ifdef __WXDEBUG__
    m_name = wxS("<root>");
#endif
    SetParentalType(0);
    m_depth = 0;
}


wxPGRootProperty::~wxPGRootProperty()
{
}


// -----------------------------------------------------------------------
// wxPropertyCategory
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPropertyCategory,none,TextCtrl)
IMPLEMENT_DYNAMIC_CLASS(wxPropertyCategory, wxPGProperty)

void wxPropertyCategory::Init()
{
    // don't set colour - prepareadditem method should do this
    SetParentalType(wxPG_PROP_CATEGORY);
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


wxPropertyCategory::~wxPropertyCategory()
{
}


wxString wxPropertyCategory::GetValueAsString( int ) const
{
    return wxEmptyString;
}

int wxPropertyCategory::GetTextExtent( const wxWindow* wnd, const wxFont& font ) const
{
    if ( m_textExtent > 0 )
        return m_textExtent;
    int x = 0, y = 0;
	((wxWindow*)wnd)->GetTextExtent( m_label, &x, &y, 0, 0, &font );
    return x;
}

void wxPropertyCategory::CalculateTextExtent( wxWindow* wnd, const wxFont& font )
{
    int x = 0, y = 0;
	wnd->GetTextExtent( m_label, &x, &y, 0, 0, &font );
    m_textExtent = x;
}

// -----------------------------------------------------------------------
// wxPGAttributeStorage
// -----------------------------------------------------------------------

wxPGAttributeStorage::wxPGAttributeStorage()
{
}

wxPGAttributeStorage::~wxPGAttributeStorage()
{
    wxPGHashMapS2P::iterator it;

    for ( it = m_map.begin(); it != m_map.end(); it++ )
    {
        wxVariantData* data = (wxVariantData*) it->second;
        data->DecRef();
    }
}

void wxPGAttributeStorage::Set( const wxString& name, const wxVariant& value )
{
    wxVariantData* data = value.GetData();

    // Free old, if any
    wxPGHashMapS2P::iterator it = m_map.find(name);
    if ( it != m_map.end() )
    {
        ((wxVariantData*)it->second)->DecRef();

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

#endif  // wxUSE_PROPGRID
