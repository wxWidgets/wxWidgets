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

wxDEFINE_WL_DELETER(wl_pointer, wl_pointer_release);
wxDEFINE_WL_DELETER(wl_registry, wl_registry_destroy);
wxDEFINE_WL_DELETER(wl_seat, wl_seat_release);
wxDEFINE_WL_DELETER(wp_pointer_warp_v1, wp_pointer_warp_v1_destroy);

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
};

extern Globals WLGlobals;

} // namespace wxWayland

#endif // wxHAVE_WAYLAND_PROTOCOLS

#endif // _WX_GTK_PRIVATE_WAYLAND_H_
