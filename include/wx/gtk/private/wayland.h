///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/wayland.h
// Purpose:     Wayland-specific wxGTK private declarations
// Author:      Vadim Zeitlin
// Created:     2025-08-22
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_WAYLAND_H_
#define _WX_GTK_PRIVATE_WAYLAND_H_

#include "wx/string.h"

#include "wx/gtk/private/wrapgdk.h"

// Use Wayland protocols directly if we have wayland-client and a new enough
// GTK version to compile the code using it.
#if defined(GDK_WINDOWING_WAYLAND) && GTK_CHECK_VERSION(3,20,0)
    #ifdef wxHAVE_WAYLAND_CLIENT
        #define wxHAVE_WAYLAND_PROTOCOLS
    #endif
#endif

#ifdef wxHAVE_WAYLAND_PROTOCOLS

#include <memory>
#include <vector>

#include <wayland-client-protocol.h> // Core protocols.

#include "wx/protocols/pointer-warp-v1-client-protocol.h"

wxGCC_WARNING_SUPPRESS(cast-qual)

#ifdef wxHAVE_WAYLAND_SESSION_MANAGEMENT
#include "wx/protocols/xdg-session-management-v1-client-protocol.h"
#endif // wxHAVE_WAYLAND_SESSION_MANAGEMENT

// This one is different from wxHAVE_WAYLAND_XXX because it's always supported
// and hence there is no need to detect it in the build system.
//
// But we only need it when using the toplevel drag protocol, so we define it
// ourselves, as either 0 or 1.
#ifdef wxHAVE_WAYLAND_TOPLEVEL_DRAG
    #include "wx/protocols/xdg-toplevel-drag-v1-client-protocol.h"

    #define wxUSE_WAYLAND_DATA_DEVICE_MANAGER 1
#endif // wxHAVE_WAYLAND_TOPLEVEL_DRAG

wxGCC_WARNING_RESTORE()

#ifndef wxUSE_WAYLAND_DATA_DEVICE_MANAGER
    #define wxUSE_WAYLAND_DATA_DEVICE_MANAGER 0
#endif

namespace wxWayland
{

template <typename T>
struct wl_deleter;
#define wxDEFINE_WL_DELETER(type, func) \
    template <> \
    struct wl_deleter<type> \
    { \
        void operator()(type* p) \
        { \
            if ( p ) \
                func(p); \
        } \
    }

#if wxUSE_WAYLAND_DATA_DEVICE_MANAGER
wxDEFINE_WL_DELETER(wl_data_device, wl_data_device_release);
wxDEFINE_WL_DELETER(wl_data_device_manager, wl_data_device_manager_destroy);
wxDEFINE_WL_DELETER(wl_data_offer, wl_data_offer_destroy);
#endif // wxUSE_WAYLAND_DATA_DEVICE_MANAGER
wxDEFINE_WL_DELETER(wl_data_source, wl_data_source_destroy);
wxDEFINE_WL_DELETER(wl_pointer, wl_pointer_release);
wxDEFINE_WL_DELETER(wl_registry, wl_registry_destroy);
wxDEFINE_WL_DELETER(wl_seat, wl_seat_release);
wxDEFINE_WL_DELETER(wp_pointer_warp_v1, wp_pointer_warp_v1_destroy);
#ifdef wxHAVE_WAYLAND_SESSION_MANAGEMENT
wxDEFINE_WL_DELETER(xdg_session_manager_v1, xdg_session_manager_v1_destroy);
wxDEFINE_WL_DELETER(xdg_session_v1, xdg_session_v1_destroy);
wxDEFINE_WL_DELETER(xdg_toplevel_session_v1, xdg_toplevel_session_v1_destroy);
#endif // wxHAVE_WAYLAND_SESSION_MANAGEMENT
#ifdef wxHAVE_WAYLAND_TOPLEVEL_DRAG
wxDEFINE_WL_DELETER(xdg_toplevel_drag_manager_v1, xdg_toplevel_drag_manager_v1_destroy);
wxDEFINE_WL_DELETER(xdg_toplevel_drag_v1, xdg_toplevel_drag_v1_destroy);
#endif // wxHAVE_WAYLAND_TOPLEVEL_DRAG

template <typename T>
using wl_unique_ptr = std::unique_ptr<T, wl_deleter<T>>;

// Represents Wayland seat object.
class Seat
{
public:
    // Ctor takes the seat itself (taking ownership of it) and its ID,
    // confusingly called "name" in Wayland.
    //
    // It also installs a listener to get the seat capabilities (and its actual
    // name, even though this is not used for anything currently).
    Seat(wl_seat* seat_, uint32_t id_);

    // As Seat is not copyable, make it movable to allow storing it in vector.
    Seat(Seat&& other) = default;
    Seat& operator=(Seat&& other) = default;

    ~Seat() = default;

    void UpdateCapabilities(int capabilities);

    // Return the data device for this seat, creating it on demand, or null if
    // the data device manager global is not available.
    wl_data_device* GetDataDevice();


    // Most members are const after they're first set, but can't be really
    // const because we need to be able to move Seat objects.
    wl_unique_ptr<wl_seat> seat;
    uint32_t id;

    int capabilities = 0;
    wxString name;

    // This one can change as pointer can be added/removed dynamically by
    // UpdateCapabilities().
    wl_unique_ptr<wl_pointer> pointer;

    // Last serial of "pointer enter" event for this seat. We don't care about
    // which surface it happened for, we just need a valid serial to pass to
    // wp_pointer_warp_v1_warp_pointer().
    uint32_t lastEnterSerial = 0;

#ifdef wxHAVE_WAYLAND_TOPLEVEL_DRAG
    // Last serial of a "pointer button pressed" event for this seat: this is
    // the serial of the implicit grab which must be passed to
    // wl_data_device_start_drag().
    uint32_t lastButtonSerial = 0;

    // The data device for this seat: only created if it's actually needed,
    // i.e. if we start a drag using it, see GetDataDevice().
    wl_unique_ptr<wl_data_device> data_device;

    // The last data offer we were given: we don't use it at all, but it must
    // be destroyed to avoid memory leaks, so we need to store it as it's not
    // clear if we can destroy it immediately when it's offered.
    wl_unique_ptr<wl_data_offer> last_data_offer;
#endif // wxHAVE_WAYLAND_TOPLEVEL_DRAG
};


struct Globals
{
    // These functions are used by wxApp and are called after initializing and
    // before shutting down GTK, respectively.
    void Init();
    void Free();

    // wl_registry object used to receive global Wayland objects that we keep
    // alive to also receive the events about their removal.
    wl_unique_ptr<wl_registry> registry;

    // Use vector, not map, as we don't need to find seats neither by their ID
    // ("name" in Wayland terminology) nor by name (seat-specific unique
    // string) and using vector is simpler.
    //
    // Of course, the choice of container doesn't matter much in the by far the
    // most typical case when we have just one of them.
    std::vector<Seat> seats;

    // Optional pointer to the global pointer warp protocol object.
    wl_unique_ptr<wp_pointer_warp_v1> pointer_warp;

#if wxUSE_WAYLAND_DATA_DEVICE_MANAGER
    // Optional pointer to the global data device manager object: this one is
    // always supported, but we only bind it if we need it, i.e. if we can also
    // use the toplevel drag protocol below.
    wl_unique_ptr<wl_data_device_manager> data_device_manager;
#endif // wxUSE_WAYLAND_DATA_DEVICE_MANAGER

#ifdef wxHAVE_WAYLAND_TOPLEVEL_DRAG
    // Optional pointer to the global toplevel drag protocol object.
    wl_unique_ptr<xdg_toplevel_drag_manager_v1> toplevel_drag_manager;
#endif // wxHAVE_WAYLAND_TOPLEVEL_DRAG

#ifdef wxHAVE_WAYLAND_SESSION_MANAGEMENT
    // Optional pointer to the global session manager protocol object.
    wl_unique_ptr<xdg_session_manager_v1> session_manager;
#endif // wxHAVE_WAYLAND_SESSION_MANAGEMENT
};

extern Globals WLGlobals;

} // namespace wxWayland

#endif // wxHAVE_WAYLAND_PROTOCOLS

#endif // _WX_GTK_PRIVATE_WAYLAND_H_
