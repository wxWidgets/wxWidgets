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

class wxTLWGeometry : public wxTLWGeometryGeneric
{
    typedef wxTLWGeometryGeneric BaseType;
public:
    virtual bool Save(const Serializer& ser) const override
    {
        if ( !wxTLWGeometryGeneric::Save(ser) )
            return false;

        // Don't save the decoration sizes if we don't really have any values
        // for them.
        if ( m_decorSize.left || m_decorSize.right ||
                m_decorSize.top || m_decorSize.bottom )
        {
            ser.SaveField("decor_l", m_decorSize.left);
            ser.SaveField("decor_r", m_decorSize.right);
            ser.SaveField("decor_t", m_decorSize.top);
            ser.SaveField("decor_b", m_decorSize.bottom);
        }

        return true;
    }

    virtual bool Restore(Serializer& ser) override
    {
        if ( !wxTLWGeometryGeneric::Restore(ser) )
            return false;

        ser.RestoreField("decor_l", &m_decorSize.left);
        ser.RestoreField("decor_r", &m_decorSize.right);
        ser.RestoreField("decor_t", &m_decorSize.top);
        ser.RestoreField("decor_b", &m_decorSize.bottom);

        return true;
    }

    virtual bool GetFrom(const wxTopLevelWindow* tlw) override
    {
        if ( !wxTLWGeometryGeneric::GetFrom(tlw) )
            return false;

        m_decorSize = tlw->m_decorSize;

        return true;
    }

    virtual bool ApplyTo(wxTopLevelWindow* tlw) override
    {
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
};

#endif // _WX_GTK_PRIVATE_TLWGEOM_H_
