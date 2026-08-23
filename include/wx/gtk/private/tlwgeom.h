///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/tlwgeom.h
// Purpose:     wxGTK-specific wxTLWGeometry class.
// Author:      Vadim Zeitlin
// Created:     2018-04-29
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_TLWGEOM_H_
#define _WX_GTK_PRIVATE_TLWGEOM_H_

#define wxPERSIST_TLW_XDG_SESSION_ID "xdg_session_id"

class wxTLWGeometry : public wxTLWGeometryGeneric
{
    typedef wxTLWGeometryGeneric BaseType;
public:
    virtual bool Save(Store& store) const override
    {
        if ( wxTopLevelWindow::HasWaylandXDGSessionManagement() )
        {
            return store.SaveString(wxPERSIST_TLW_XDG_SESSION_ID, m_xdgSessionId);
        }

        if ( !wxTLWGeometryGeneric::Save(store) )
            return false;

        // Don't save the decoration sizes if we don't really have any values
        // for them.
        if ( m_decorSize.left || m_decorSize.right ||
                m_decorSize.top || m_decorSize.bottom )
        {
            store.SaveValue("decor_l", m_decorSize.left);
            store.SaveValue("decor_r", m_decorSize.right);
            store.SaveValue("decor_t", m_decorSize.top);
            store.SaveValue("decor_b", m_decorSize.bottom);
        }

        return true;
    }

    virtual bool Restore(const Store& store) override
    {
        if ( wxTopLevelWindow::HasWaylandXDGSessionManagement() )
        {
            store.RestoreString(wxPERSIST_TLW_XDG_SESSION_ID, &m_xdgSessionId);

            // When using Wayland XDG session management, we need our ApplyTo()
            // to be called, even if we don't have any saved state yet, so
            // always return true, even if the session ID is empty.
            return true;
        }

        if ( !wxTLWGeometryGeneric::Restore(store) )
            return false;

        store.RestoreValue("decor_l", &m_decorSize.left);
        store.RestoreValue("decor_r", &m_decorSize.right);
        store.RestoreValue("decor_t", &m_decorSize.top);
        store.RestoreValue("decor_b", &m_decorSize.bottom);

        return true;
    }

    virtual bool GetFrom(const wxTopLevelWindow* tlw) override
    {
        // If we are using Wayland XDG session management protocol, we just
        // need to save the session ID.
        if ( wxTopLevelWindow::HasWaylandXDGSessionManagement() )
        {
            m_xdgSessionId = tlw->GetWaylandXDGSessionId();

            // Note that it shouldn't be empty here: when XDG session
            // management protocol is supported, the compositor should have
            // assigned a session ID for the associated TLW.

            return true;
        }

        if ( !wxTLWGeometryGeneric::GetFrom(tlw) )
            return false;

        m_decorSize = tlw->m_decorSize;

        return true;
    }

    virtual bool ApplyTo(wxTopLevelWindow* tlw) override
    {
        if ( wxTopLevelWindow::HasWaylandXDGSessionManagement() )
        {
            // As mentioned in Restore(), we need to call ApplyTo() even if we
            // don't have any saved state yet in order to register this window
            // with the session manager, so always do it, even if the session
            // ID is empty.
            tlw->SetWaylandXDGSessionId(m_xdgSessionId);

            // However only return true if we did have a session ID to restore
            // and return false for the first run to give the application a
            // possibility to set the default size in this case.
            //
            // Note that returning true if we do have a session ID is
            // optimistic, we don't know if the geometry will be really
            // restored by the compositor (this will happen later, when the
            // window is shown), but we have to assume that it will be, as
            // always returning false from here would be definitely wrong as it
            // would cause the application to always overwrite the possibly
            // saved size with the default.
            return !m_xdgSessionId.empty();
        }

        // Don't overwrite the current decoration size if we already have it.
        if ( !tlw->m_decorSize.left && !tlw->m_decorSize.right &&
                !tlw->m_decorSize.top && !tlw->m_decorSize.bottom )
        {
            tlw->m_decorSize = m_decorSize;
        }

        return BaseType::ApplyTo(tlw);
    }

private:
    wxTopLevelWindow::DecorSize m_decorSize;

    wxString m_xdgSessionId;
};

#endif // _WX_GTK_PRIVATE_TLWGEOM_H_
