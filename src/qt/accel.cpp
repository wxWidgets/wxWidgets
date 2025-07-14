/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/accel.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_ACCEL

#include "wx/accel.h"
#include "wx/qt/private/converter.h"
#include <QtCore/QVariant>
#include <QShortcut>

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class wxAccelRefData : public wxObjectRefData
{
    public:
        wxAccelRefData()
        {
        }

        wxAccelRefData(const wxAccelRefData& data)
        : wxObjectRefData()
            , m_accels(data.m_accels)
        {
        }

        std::vector<wxAcceleratorEntry> m_accels;
};

// macro which can be used to access wxAccelRefData from wxAcceleratorTable
#define M_ACCELDATA ((wxAccelRefData *)m_refData)


// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxAcceleratorTable, wxObject );

QShortcut *ConvertAccelerator( const wxAcceleratorEntry& e, QWidget *parent )
{
    // TODO: Not all keys have the same string representation in wx and qt
    const auto keySeq = QKeySequence(wxQtConvertString( e.ToString() ));

#if QT_VERSION_MAJOR >= 6
    const auto s = new QShortcut( keySeq, (QObject*)parent );
#else
    const auto s = new QShortcut( keySeq, parent );
#endif


    // Set a property to save wx Command to send when activated
    s->setProperty( "wxQt_Command", e.GetCommand() );

    return s;
}

wxAcceleratorTable::wxAcceleratorTable()
{
}

wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    if ( n == 0 )
    {
        // This is valid but useless.
        return;
    }

    wxCHECK_RET( n > 0, "Invalid number of accelerator entries" );

    m_refData = new wxAccelRefData;

    for ( int i = 0; i < n; i++ )
    {
        M_ACCELDATA->m_accels.emplace_back( entries[i] );
    }
}

wxVector<QShortcut*> wxAcceleratorTable::ConvertShortcutTable( QWidget *parent ) const
{
    wxVector<QShortcut*> shortcuts;

    for ( const auto& accel : M_ACCELDATA->m_accels )
    {
        shortcuts.push_back(ConvertAccelerator(accel, parent));
    }

    return shortcuts;
}

wxObjectRefData *wxAcceleratorTable::CreateRefData() const
{
    return new wxAccelRefData;
}

wxObjectRefData *wxAcceleratorTable::CloneRefData(const wxObjectRefData *data) const
{
    return new wxAccelRefData(*static_cast<const wxAccelRefData*>(data));
}

bool wxAcceleratorTable::IsOk() const
{
    return (m_refData != nullptr);
}

#endif // wxUSE_ACCEL
