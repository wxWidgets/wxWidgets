///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/private/variant.h
// Purpose:     RAII wrapper for working with GVariant
// Author:      Vadim Zeitlin
// Created:     2024-04-11
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_VARIANT_H_
#define _WX_GTK_PRIVATE_VARIANT_H_

// ----------------------------------------------------------------------------
// wxGtkVariant: RAII wrapper for GVariant
// ----------------------------------------------------------------------------

class wxGtkVariant
{
public:
    wxGtkVariant() = default;

    explicit wxGtkVariant(GVariant* variant) : m_variant(variant) { }

    wxGtkVariant(wxGtkVariant&& other) noexcept
        : m_variant(other.m_variant)
    {
        other.m_variant = nullptr;
    }

    wxGtkVariant& operator=(wxGtkVariant&& other) noexcept
    {
        if ( this != &other )
        {
            m_variant = other.m_variant;
            other.m_variant = nullptr;
        }

        return *this;
    }

    ~wxGtkVariant()
    {
        if ( m_variant )
            g_variant_unref(m_variant);
    }

    // Check if we have a valid GVariant.
    explicit operator bool() const { return m_variant != nullptr; }

    // Return the pointer to the internally stored pointer. This should only be
    // used to initialize the object by passing it to some GTK function.
    GVariant **ByRef()
    {
        wxASSERT_MSG( !m_variant, "shouldn't be already initialized" );

        return &m_variant;
    }

    // Wrappers for a few functions used in our code.
    guint32 GetUint32() const { return g_variant_get_uint32(m_variant); }
    wxGtkVariant GetVariant() const { return wxGtkVariant{g_variant_get_variant(m_variant)}; }

    // Wrapper for generic g_variant_get(): this is still as type-unsafe as the
    // original C function.
    template <typename... Args>
    void Get(const gchar* format_string, Args*... args) const
    {
        g_variant_get(m_variant, format_string, args...);
    }

    // Yield ownership of the GVariant to the caller.
    GVariant* Release()
    {
        GVariant* const variant = m_variant;
        m_variant = nullptr;
        return variant;
    }

private:
    GVariant* m_variant = nullptr;

    // For now we just don't support copying at all for simplicity, it could be
    // implemented later if needed.
    wxDECLARE_NO_COPY_CLASS(wxGtkVariant);
};

#endif // _WX_GTK_PRIVATE_VARIANT_H_
