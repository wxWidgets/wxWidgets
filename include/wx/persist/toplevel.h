///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/toplevel.h
// Purpose:     persistence support for wxTLW
// Author:      Vadim Zeitlin
// Created:     2009-01-19
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_TOPLEVEL_H_
#define _WX_PERSIST_TOPLEVEL_H_

#include "wx/persist/window.h"

#include "wx/toplevel.h"

// ----------------------------------------------------------------------------
// string constants used by wxPersistentTLW
// ----------------------------------------------------------------------------

// we use just "Window" to keep configuration files and such short, there
// should be no confusion with wxWindow itself as we don't have persistent
// windows, just persistent controls which have their own specific kind strings
#define wxPERSIST_TLW_KIND "Window"

// ----------------------------------------------------------------------------
// wxPersistentTLW: supports saving/restoring window position and size as well
//                  as maximized/iconized/restore state
// ----------------------------------------------------------------------------

class wxPersistentTLW : public wxPersistentWindow<wxTopLevelWindow>
{
public:
    wxPersistentTLW(wxTopLevelWindow *tlw)
        : wxPersistentWindow<wxTopLevelWindow>(tlw),
          m_store{this}
    {
    }

    virtual void Save() const override
    {
        const wxTopLevelWindow * const tlw = Get();

        tlw->SaveGeometry(m_store);
    }

    virtual bool Restore() override
    {
        wxTopLevelWindow * const tlw = Get();

        return tlw->RestoreToGeometry(m_store);
    }

    virtual wxString GetKind() const override { return wxASCII_STR(wxPERSIST_TLW_KIND); }

private:
    class PersistentStore : public wxTopLevelWindow::GeometryStore
    {
    public:
        explicit PersistentStore(wxPersistentTLW *pers)
            : m_pers(pers)
        {
        }

        virtual bool SaveValue(const wxString& name, int value) override
        {
            return m_pers->SaveValue(name, value);
        }

        virtual bool RestoreValue(const wxString& name, int* value) const override
        {
            return m_pers->RestoreValue(name, value);
        }

    private:
        wxPersistentTLW* const m_pers;
    };

    // This is mutable because we need to be able to call SaveValue() from
    // const Save(). It is fine for the object which doesn't have any internal
    // state anyhow.
    mutable PersistentStore m_store;
};

inline wxPersistentObject *wxCreatePersistentObject(wxTopLevelWindow *tlw)
{
    return new wxPersistentTLW(tlw);
}

#endif // _WX_PERSIST_TOPLEVEL_H_
