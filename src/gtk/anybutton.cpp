/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/anybutton.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     1998-05-20 (extracted from button.cpp)
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef wxHAS_ANY_BUTTON

#ifndef WX_PRECOMP
    #include "wx/anybutton.h"
#endif

#include "wx/stockitem.h"

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/image.h"

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

extern "C"
{

static void
wxgtk_button_enter_callback(GtkWidget *WXUNUSED(widget), wxAnyButton *button)
{
    if ( button->GTKShouldIgnoreEvent() )
        return;

    button->GTKMouseEnters();
}

static void
wxgtk_button_leave_callback(GtkWidget *WXUNUSED(widget), wxAnyButton *button)
{
    if ( button->GTKShouldIgnoreEvent() )
        return;

    button->GTKMouseLeaves();
}

static void
wxgtk_button_press_callback(GtkWidget *WXUNUSED(widget), wxAnyButton *button)
{
    if ( button->GTKShouldIgnoreEvent() )
        return;

    button->GTKPressed();
}

static void
wxgtk_button_released_callback(GtkWidget *WXUNUSED(widget), wxAnyButton *button)
{
    if ( button->GTKShouldIgnoreEvent() )
        return;

    button->GTKReleased();
}

} // extern "C"

//-----------------------------------------------------------------------------
// wxAnyButton
//-----------------------------------------------------------------------------

void wxAnyButton::DoEnable(bool enable)
{
    // See wxWindow::DoEnable()
    if ( !m_widget )
        return;

    base_type::DoEnable(enable);

    gtk_widget_set_sensitive(gtk_bin_get_child(GTK_BIN(m_widget)), enable);

    if (enable)
        GTKFixSensitivity();

    GTKUpdateBitmap();
}

GdkWindow *wxAnyButton::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return gtk_button_get_event_window(GTK_BUTTON(m_widget));
}

// static
wxVisualAttributes
wxAnyButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_button_new());
}

// ----------------------------------------------------------------------------
// bitmaps support
// ----------------------------------------------------------------------------

void wxAnyButton::GTKMouseEnters()
{
    m_isCurrent = true;

    GTKUpdateBitmap();
}

void wxAnyButton::GTKMouseLeaves()
{
    m_isCurrent = false;

    GTKUpdateBitmap();
}

void wxAnyButton::GTKPressed()
{
    m_isPressed = true;

    GTKUpdateBitmap();
}

void wxAnyButton::GTKReleased()
{
    m_isPressed = false;

    GTKUpdateBitmap();
}

void wxAnyButton::GTKOnFocus(wxFocusEvent& event)
{
    event.Skip();

    GTKUpdateBitmap();
}

wxAnyButton::State wxAnyButton::GTKGetCurrentBitmapState() const
{
    if ( !IsThisEnabled() )
    {
        if ( m_bitmaps[State_Disabled].IsOk() )
            return State_Disabled;
    }
    else
    {
        if ( m_isPressed && m_bitmaps[State_Pressed].IsOk() )
            return State_Pressed;

        if ( m_isCurrent && m_bitmaps[State_Current].IsOk() )
            return State_Current;

        if ( HasFocus() && m_bitmaps[State_Focused].IsOk() )
            return State_Focused;
    }

    // Fall back on the normal state: which still might be different from
    // State_Normal for the toggle buttons, so the check for bitmap validity is
    // still needed.
    const State normalState = GetNormalState();
    if ( m_bitmaps[normalState].IsOk() )
        return normalState;

    // And if nothing else can (or should) be used, finally fall back to the
    // normal state which is the only one guaranteed to have a bitmap (if we're
    // using bitmaps at all and we're only called in this case).
    return State_Normal;
}

void wxAnyButton::GTKUpdateBitmap()
{
    // if we don't show bitmaps at all, there is nothing to update
    if ( m_bitmaps[State_Normal].IsOk() )
    {
        // if we do show them, this will return a state for which we do have a
        // valid bitmap
        State state = GTKGetCurrentBitmapState();

        GTKDoShowBitmap(m_bitmaps[state]);
    }
}

void wxAnyButton::GTKDoShowBitmap(const wxBitmapBundle& bitmap)
{
    wxCHECK_RET(bitmap.IsOk(), "invalid bitmap");

    GtkWidget* image = gtk_button_get_image(GTK_BUTTON(m_widget));
    if (image == NULL)
        image = gtk_bin_get_child(GTK_BIN(m_widget));

    wxCHECK_RET(WX_GTK_IS_IMAGE(image), "must have image widget");

    WX_GTK_IMAGE(image)->Set(bitmap);
}

wxBitmap wxAnyButton::DoGetBitmap(State which) const
{
    return m_bitmaps[which].GetBitmap(wxDefaultSize);
}

void wxAnyButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    switch ( which )
    {
        case State_Normal:
            if ( DontShowLabel() )
            {
                // we only have the bitmap in this button, never remove it but
                // do invalidate the best size when the bitmap (and presumably
                // its size) changes
                InvalidateBestSize();
            }
            // normal image is special: setting it enables images for the button and
            // resetting it to nothing disables all of them
            else
            {
                GtkWidget *image = gtk_button_get_image(GTK_BUTTON(m_widget));
                if ( image && !WX_GTK_IS_IMAGE(image) )
                {
                    // This must be the GtkImage created for stock buttons, we
                    // want to replace it with our own wxGtkImage as only it
                    // handles showing appropriately-sized bitmaps in high DPI.
                    image = NULL;
                }

                if ( image && !bitmap.IsOk() )
                {
                    gtk_container_remove(GTK_CONTAINER(m_widget), image);
                }
                else if ( !image && bitmap.IsOk() )
                {
                    image = wxGtkImage::New(this);
                    gtk_button_set_image(GTK_BUTTON(m_widget), image);

                    // Setting the image recreates the label, so we need to
                    // reapply the styles to it to preserve the existing text
                    // font and colour if they're different from defaults.
                    GTKApplyWidgetStyle();
                }
                else // image presence or absence didn't change
                {
                    // don't invalidate best size below
                    break;
                }

                InvalidateBestSize();
            }
            break;

        case State_Pressed:
            if ( bitmap.IsOk() )
            {
                if ( !m_bitmaps[which].IsOk() )
                {
                    // we need to install the callbacks to be notified about
                    // the button pressed state change
                    g_signal_connect
                    (
                        m_widget,
                        "pressed",
                        G_CALLBACK(wxgtk_button_press_callback),
                        this
                    );

                    g_signal_connect
                    (
                        m_widget,
                        "released",
                        G_CALLBACK(wxgtk_button_released_callback),
                        this
                    );
                }
            }
            else // no valid bitmap
            {
                if ( m_bitmaps[which].IsOk() )
                {
                    // we don't need to be notified about the button pressed
                    // state changes any more
                    g_signal_handlers_disconnect_by_func
                    (
                        m_widget,
                        (gpointer)wxgtk_button_press_callback,
                        this
                    );

                    g_signal_handlers_disconnect_by_func
                    (
                        m_widget,
                        (gpointer)wxgtk_button_released_callback,
                        this
                    );

                    // also make sure we don't remain stuck in pressed state
                    if ( m_isPressed )
                    {
                        m_isPressed = false;
                        GTKUpdateBitmap();
                    }
                }
            }
            break;

        case State_Current:
            // the logic here is the same as above for State_Pressed: we need
            // to connect the handlers if we must be notified about the changes
            // in the button current state and we disconnect them when/if we
            // don't need them any more
            if ( bitmap.IsOk() )
            {
                if ( !m_bitmaps[which].IsOk() )
                {
                    g_signal_connect
                    (
                        m_widget,
                        "enter",
                        G_CALLBACK(wxgtk_button_enter_callback),
                        this
                    );

                    g_signal_connect
                    (
                        m_widget,
                        "leave",
                        G_CALLBACK(wxgtk_button_leave_callback),
                        this
                    );
                }
            }
            else // no valid bitmap
            {
                if ( m_bitmaps[which].IsOk() )
                {
                    g_signal_handlers_disconnect_by_func
                    (
                        m_widget,
                        (gpointer)wxgtk_button_enter_callback,
                        this
                    );

                    g_signal_handlers_disconnect_by_func
                    (
                        m_widget,
                        (gpointer)wxgtk_button_leave_callback,
                        this
                    );

                    if ( m_isCurrent )
                    {
                        m_isCurrent = false;
                        GTKUpdateBitmap();
                    }
                }
            }
            break;

        case State_Focused:
            if ( bitmap.IsOk() )
            {
                Bind(wxEVT_SET_FOCUS, &wxAnyButton::GTKOnFocus, this);
                Bind(wxEVT_KILL_FOCUS, &wxAnyButton::GTKOnFocus, this);
            }
            else // no valid focused bitmap
            {
                Unbind(wxEVT_SET_FOCUS, &wxAnyButton::GTKOnFocus, this);
                Unbind(wxEVT_KILL_FOCUS, &wxAnyButton::GTKOnFocus, this);
            }
            break;

        default:
            // no callbacks to connect/disconnect
            ;
    }

    m_bitmaps[which] = bitmap;

#if GTK_CHECK_VERSION(3,6,0) && !defined(__WXGTK4__)
    // Allow explicitly set bitmaps to be shown regardless of theme setting
    if (gtk_check_version(3,6,0) == NULL && bitmap.IsOk())
        gtk_button_set_always_show_image(GTK_BUTTON(m_widget), true);
#endif

    // update the bitmap immediately if necessary, otherwise it will be done
    // when the bitmap for the corresponding state is needed the next time by
    // GTKUpdateBitmap()
    if ( bitmap.IsOk() && which == GTKGetCurrentBitmapState() )
    {
        GTKDoShowBitmap(bitmap);
    }
}

void wxAnyButton::DoSetBitmapPosition(wxDirection dir)
{
#ifdef __WXGTK210__
    if ( wx_is_at_least_gtk2(10) )
    {
        GtkPositionType gtkpos;
        switch ( dir )
        {
            default:
                wxFAIL_MSG( "invalid position" );
                wxFALLTHROUGH;

            case wxLEFT:
                gtkpos = GTK_POS_LEFT;
                break;

            case wxRIGHT:
                gtkpos = GTK_POS_RIGHT;
                break;

            case wxTOP:
                gtkpos = GTK_POS_TOP;
                break;

            case wxBOTTOM:
                gtkpos = GTK_POS_BOTTOM;
                break;
        }

        gtk_button_set_image_position(GTK_BUTTON(m_widget), gtkpos);

        // As in DoSetBitmap() above, the above call can invalidate the label
        // style, so reapply it to preserve its font and colour.
        GTKApplyWidgetStyle();

        InvalidateBestSize();
    }
#endif // GTK+ 2.10+
}

#endif // wxHAS_ANY_BUTTON
