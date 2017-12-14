///////////////////////////////////////////////////////////////////////////////
// Name:        tests/persistence/testpersistence.h
// Purpose:     Fixture for wxPersistentObject unit tests
// Author:      wxWidgets Team
// Created:     2017-08-23
// Copyright:   (c) 2017 wxWidgets Team
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_TESTS_PERSIST_TESTPERSISTENCE_H
#define WX_TESTS_PERSIST_TESTPERSISTENCE_H

#include "wx/app.h"
#include "wx/config.h"

#define PO_PREFIX           "/Persistent_Options"

class PersistenceTests
{
public:
    PersistenceTests()
        : m_cleanup(false)
    {
        wxTheApp->SetAppName("wxPersistenceTests");
    }

    // The tests using this fixture should call this method when they don't
    // need the values saved into wxConfig any more.
    void EnableCleanup()
    {
        m_cleanup = true;
    }

    ~PersistenceTests()
    {
        if ( m_cleanup )
        {
            wxConfig::Get()->DeleteGroup(PO_PREFIX);
        }
    }

private:
    bool m_cleanup;
};

#endif // WX_TESTS_PERSIST_TESTPERSISTENCE_H
