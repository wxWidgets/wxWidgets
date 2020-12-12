/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/spinctrl.cpp
// Purpose:     wxSpinCtrl
// Author:      Robert
// Modified by:
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"    // for wxEVT_TEXT
    #include "wx/math.h"        // wxRound()
    #include "wx/utils.h"
    #include "wx/wxcrtvararg.h"
#endif

#include "wx/private/spinctrl.h"

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_value_changed(GtkSpinButton* spinbutton, wxSpinCtrlGTKBase* win)
{
    if (g_blockEventsOnDrag)
        return;

    if (wxIsKindOf(win, wxSpinCtrl))
    {
        wxSpinEvent event(wxEVT_SPINCTRL, win->GetId());
        event.SetEventObject( win );
        event.SetPosition(static_cast<wxSpinCtrl*>(win)->GetValue());
        event.SetString(gtk_entry_get_text(GTK_ENTRY(spinbutton)));
        win->HandleWindowEvent( event );
    }
    else // wxIsKindOf(win, wxSpinCtrlDouble)
    {
        wxSpinDoubleEvent event( wxEVT_SPINCTRLDOUBLE, win->GetId());
        event.SetEventObject( win );
        event.SetValue(static_cast<wxSpinCtrlDouble*>(win)->GetValue());
        event.SetString(gtk_entry_get_text(GTK_ENTRY(spinbutton)));
        win->HandleWindowEvent( event );
    }
}
}

//-----------------------------------------------------------------------------
//  "changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_changed(GtkSpinButton* spinbutton, wxSpinCtrl* win)
{
    wxCommandEvent event( wxEVT_TEXT, win->GetId() );
    event.SetEventObject( win );
    event.SetString(gtk_entry_get_text(GTK_ENTRY(spinbutton)));
    event.SetInt(win->GetValue());
    win->HandleWindowEvent( event );
}
}

// ----------------------------------------------------------------------------
// wxSpinCtrlEventDisabler: helper to temporarily disable GTK+ events
// ----------------------------------------------------------------------------

class wxSpinCtrlEventDisabler
{
public:
    explicit wxSpinCtrlEventDisabler(wxSpinCtrlGTKBase* spin)
        : m_spin(spin)
    {
        m_spin->GtkDisableEvents();
    }

    ~wxSpinCtrlEventDisabler()
    {
        m_spin->GtkEnableEvents();
    }

private:
    wxSpinCtrlGTKBase* const m_spin;

    wxDECLARE_NO_COPY_CLASS(wxSpinCtrlEventDisabler);
};

//-----------------------------------------------------------------------------
// wxSpinCtrlGTKBase
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxSpinCtrlGTKBase, wxSpinCtrlBase)
    EVT_CHAR(wxSpinCtrlGTKBase::OnChar)
wxEND_EVENT_TABLE()

bool wxSpinCtrlGTKBase::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,  const wxSize& size,
                        long style,
                        double min, double max, double initial, double inc,
                        const wxString& name)
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxSpinCtrlGTKBase creation failed") );
        return false;
    }

    m_widget = gtk_spin_button_new_with_range(min, max, inc);
    g_object_ref(m_widget);

    gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_widget), initial);

    gfloat align;
    if ( HasFlag(wxALIGN_RIGHT) )
        align = 1.0;
    else if ( HasFlag(wxALIGN_CENTRE) )
        align = 0.5;
    else
        align = 0.0;

    gtk_entry_set_alignment(GTK_ENTRY(m_widget), align);

    GtkSetEntryWidth();

    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget),
                              (int)(m_windowStyle & wxSP_WRAP) );

    g_signal_connect_after(m_widget, "value_changed", G_CALLBACK(gtk_value_changed), this);
    g_signal_connect_after(m_widget, "changed", G_CALLBACK(gtk_changed), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    if (!value.empty())
    {
        SetValue(value);
    }

    return true;
}

double wxSpinCtrlGTKBase::DoGetValue() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    // Get value directly from current control text, just as
    // gtk_spin_button_update() would do. Calling gtk_spin_button_update() causes
    // a redraw, which causes an idle event, so if GetValue() is called from
    // a UI update handler, you get a never ending sequence of idle events. It
    // also forces the text into valid range, which wxMSW GetValue() does not do.
    static unsigned sig_id;
    if (sig_id == 0)
        sig_id = g_signal_lookup("input", GTK_TYPE_SPIN_BUTTON);
    double value;
    int handled = 0;
    g_signal_emit(m_widget, sig_id, 0, &value, &handled);
    if (!handled)
        value = g_strtod(gtk_entry_get_text(GTK_ENTRY(m_widget)), NULL);
    GtkAdjustment* adj =
        gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(m_widget));
    const double lower = gtk_adjustment_get_lower(adj);
    const double upper = gtk_adjustment_get_upper(adj);
    if (value < lower)
        value = lower;
    else if (value > upper)
        value = upper;

    return value;
}

double wxSpinCtrlGTKBase::DoGetMin() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    double min = 0;
    gtk_spin_button_get_range( GTK_SPIN_BUTTON(m_widget), &min, NULL);
    return min;
}

double wxSpinCtrlGTKBase::DoGetMax() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    double max = 0;
    gtk_spin_button_get_range( GTK_SPIN_BUTTON(m_widget), NULL, &max);
    return max;
}

double wxSpinCtrlGTKBase::DoGetIncrement() const
{
    wxCHECK_MSG( (m_widget != NULL), 0, wxT("invalid spin button") );

    double inc = 0;
    gtk_spin_button_get_increments( GTK_SPIN_BUTTON(m_widget), &inc, NULL);
    return inc;
}

bool wxSpinCtrlGTKBase::GetSnapToTicks() const
{
    wxCHECK_MSG(m_widget, false, "invalid spin button");

    return gtk_spin_button_get_snap_to_ticks( GTK_SPIN_BUTTON(m_widget) ) != 0;
}

void wxSpinCtrlGTKBase::SetValue( const wxString& value )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    double n;
    if ( wxSscanf(value, "%lg", &n) == 1 )
    {
        // a number - set it, let DoSetValue round for int value
        DoSetValue(n);
        return;
    }

    // invalid number - set text as is (wxMSW compatible)
    wxSpinCtrlEventDisabler disable(this);
    gtk_entry_set_text( GTK_ENTRY(m_widget), wxGTK_CONV( value ) );
}

void wxSpinCtrlGTKBase::DoSetValue( double value )
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    wxSpinCtrlEventDisabler disable(this);
    gtk_spin_button_set_value( GTK_SPIN_BUTTON(m_widget), value);
}

void wxSpinCtrlGTKBase::SetSnapToTicks(bool snap_to_ticks)
{
    wxCHECK_RET( (m_widget != NULL), "invalid spin button" );

    gtk_spin_button_set_snap_to_ticks( GTK_SPIN_BUTTON(m_widget), snap_to_ticks);
}

void wxSpinCtrlGTKBase::SetSelection(long from, long to)
{
    // translate from wxWidgets conventions to GTK+ ones: (-1, -1) means the
    // entire range
    if ( from == -1 && to == -1 )
    {
        from = 0;
        to = INT_MAX;
    }

    gtk_editable_select_region( GTK_EDITABLE(m_widget), (gint)from, (gint)to );
}

void wxSpinCtrlGTKBase::DoSetRange(double minVal, double maxVal)
{
    wxCHECK_RET( (m_widget != NULL), wxT("invalid spin button") );

    // Negative values in the range are allowed only if base == 10
    if ( !wxSpinCtrlImpl::IsBaseCompatibleWithRange(minVal, maxVal, GetBase()) )
    {
        return;
    }

    wxSpinCtrlEventDisabler disable(this);
    gtk_spin_button_set_range( GTK_SPIN_BUTTON(m_widget), minVal, maxVal);

    InvalidateBestSize();

    GtkSetEntryWidth();
}

void wxSpinCtrlGTKBase::DoSetIncrement(double inc)
{
    wxCHECK_RET( m_widget, "invalid spin button" );

    wxSpinCtrlEventDisabler disable(this);

    // With GTK2, gtk_spin_button_set_increments() does not emit the GtkAdjustment
    // "changed" signal, which is needed to properly update the state of the control
    GtkAdjustment* adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(m_widget));
    gtk_adjustment_set_step_increment(adj, inc);
}

void wxSpinCtrlGTKBase::GtkDisableEvents()
{
    g_signal_handlers_block_by_func(m_widget, (void*)gtk_value_changed, this);
    g_signal_handlers_block_by_func(m_widget, (void*)gtk_changed, this);
}

void wxSpinCtrlGTKBase::GtkEnableEvents()
{
    g_signal_handlers_unblock_by_func(m_widget, (void*)gtk_value_changed, this);
    g_signal_handlers_unblock_by_func(m_widget, (void*)gtk_changed, this);
}

void wxSpinCtrlGTKBase::OnChar( wxKeyEvent &event )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid spin ctrl") );

    if (event.GetKeyCode() == WXK_RETURN)
    {
        wxWindow *top_frame = wxGetTopLevelParent(m_parent);

        if ( GTK_IS_WINDOW(top_frame->m_widget) )
        {
            GtkWindow *window = GTK_WINDOW(top_frame->m_widget);
            if ( window )
            {
                GtkWidget* widgetDef = gtk_window_get_default_widget(window);

                if ( widgetDef )
                {
                    gtk_widget_activate(widgetDef);
                    return;
                }
            }
        }
    }

    if ((event.GetKeyCode() == WXK_RETURN) && (m_windowStyle & wxTE_PROCESS_ENTER))
    {
        wxCommandEvent evt( wxEVT_TEXT_ENTER, m_windowId );
        evt.SetEventObject(this);
        GtkSpinButton *gsb = GTK_SPIN_BUTTON(m_widget);
        wxString val = wxGTK_CONV_BACK( gtk_entry_get_text( &gsb->entry ) );
        evt.SetString( val );
        if (HandleWindowEvent(evt)) return;
    }

    event.Skip();
}

GdkWindow *wxSpinCtrlGTKBase::GTKGetWindow(wxArrayGdkWindows& windows) const
{
#ifdef __WXGTK3__
    GTKFindWindow(m_widget, windows);
#else
    GtkSpinButton* spinbutton = GTK_SPIN_BUTTON(m_widget);

    windows.push_back(spinbutton->entry.text_area);
    windows.push_back(spinbutton->panel);
#endif

    return NULL;
}

wxSize wxSpinCtrlGTKBase::DoGetSizeFromTextSize(int xlen, int ylen) const
{
    wxASSERT_MSG( m_widget, wxS("GetSizeFromTextSize called before creation") );

    // This is a bit stupid as we typically compute xlen by measuring some
    // string of digits in the first place, but there doesn't seem to be
    // anything better to do (unless we add some GetSizeFromNumberOfDigits()).
    const double widthDigit = GetTextExtent("0123456789").GetWidth() / 10.0;
    const int numDigits = wxRound(xlen / widthDigit);

    const gint widthChars = gtk_entry_get_width_chars(GTK_ENTRY(m_widget));
    gtk_entry_set_width_chars(GTK_ENTRY(m_widget), numDigits);

    wxSize tsize = GTKGetPreferredSize(m_widget);

    gtk_entry_set_width_chars(GTK_ENTRY(m_widget), widthChars);

    // Check if the user requested a non-standard height.
    if ( ylen > 0 )
        tsize.IncBy(0, ylen - GetCharHeight());

    return tsize;
}

// static
wxVisualAttributes
wxSpinCtrlGTKBase::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_spin_button_new_with_range(0, 100, 1), true);
}

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

extern "C"
{

static gboolean
wx_gtk_spin_input(GtkSpinButton* spin, gdouble* val, wxSpinCtrl* win)
{
    // We might use g_ascii_strtoll() here but it's 2.12+ only, so use our own
    // wxString function even if this requires an extra conversion.
    const wxString
        text(wxString::FromUTF8(gtk_entry_get_text(GTK_ENTRY(spin))));

    long lval;
    if ( !text.ToLong(&lval, win->GetBase()) )
        return FALSE;

    *val = lval;

    return TRUE;
}

static gint
wx_gtk_spin_output(GtkSpinButton* spin, wxSpinCtrl* win)
{
    const gint val = gtk_spin_button_get_value_as_int(spin);

    gtk_entry_set_text
    (
        GTK_ENTRY(spin),
        wxSpinCtrlImpl::FormatAsHex(val, win->GetMax()).utf8_str()
    );

    return TRUE;
}

} // extern "C"

void wxSpinCtrl::GtkSetEntryWidth()
{
    const int minVal = static_cast<int>(DoGetMin());
    const int maxVal = static_cast<int>(DoGetMax());

    gtk_entry_set_width_chars
    (
        GTK_ENTRY(m_widget),
        wxSpinCtrlImpl::GetMaxValueLength(minVal, maxVal, GetBase())
    );
}

bool wxSpinCtrl::SetBase(int base)
{
    // Currently we only support base 10 and 16. We could add support for base
    // 8 quite easily but wxMSW doesn't support it natively so don't bother
    // with doing something wxGTK-specific here.
    if ( base != 10 && base != 16 )
        return false;

    if ( base == m_base )
        return true;

    // For negative values in the range only base == 10 is allowed
    if ( !wxSpinCtrlImpl::IsBaseCompatibleWithRange(static_cast<int>(DoGetMin()), static_cast<int>(DoGetMax()), base) )
        return false;

    m_base = base;

    // We need to be able to enter letters for any base greater than 10.
    gtk_spin_button_set_numeric( GTK_SPIN_BUTTON(m_widget), m_base <= 10 );

    if ( m_base != 10 )
    {
        g_signal_connect( m_widget, "input",
                              G_CALLBACK(wx_gtk_spin_input), this);
        g_signal_connect( m_widget, "output",
                              G_CALLBACK(wx_gtk_spin_output), this);
    }
    else
    {
        g_signal_handlers_disconnect_by_func(m_widget,
                                             (gpointer)wx_gtk_spin_input,
                                             this);
        g_signal_handlers_disconnect_by_func(m_widget,
                                             (gpointer)wx_gtk_spin_output,
                                             this);
    }

    InvalidateBestSize();

    GtkSetEntryWidth();

    // Update the displayed text after changing the base it uses.
    SetValue(GetValue());

    return true;
}

//-----------------------------------------------------------------------------
// wxSpinCtrlDouble
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxSpinCtrlDouble, wxSpinCtrlGTKBase);

void wxSpinCtrlDouble::GtkSetEntryWidth()
{
    const unsigned digits = GetDigits();
    const int lenMin = wxString::Format("%.*f", digits, GetMin()).length();
    const int lenMax = wxString::Format("%.*f", digits, GetMax()).length();

    gtk_entry_set_width_chars(GTK_ENTRY(m_widget), wxMax(lenMin, lenMax));
}

unsigned wxSpinCtrlDouble::GetDigits() const
{
    wxCHECK_MSG( m_widget, 0, "invalid spin button" );

    return gtk_spin_button_get_digits( GTK_SPIN_BUTTON(m_widget) );
}

void wxSpinCtrlDouble::SetDigits(unsigned digits)
{
    wxCHECK_RET( m_widget, "invalid spin button" );

    wxSpinCtrlEventDisabler disable(this);
    gtk_spin_button_set_digits( GTK_SPIN_BUTTON(m_widget), digits);

    InvalidateBestSize();

    GtkSetEntryWidth();
}

#endif // wxUSE_SPINCTRL
