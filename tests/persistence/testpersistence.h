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
#include "wx/persist.h"

#define PO_PREFIX           "/Persistent_Options"

class PersistenceTests
{
public:
    PersistenceTests()
        : m_managerOld(&wxPersistenceManager::Get())
    {
        // Install our custom manager, using custom config object, for the test
        // duration.
        wxPersistenceManager::Set(m_manager);
    }

    // Access the config object used for storing the settings.
    const wxConfigBase& GetConfig() const
    {
        return *m_manager.GetConfig();
    }

    ~PersistenceTests()
    {
        wxPersistenceManager::Set(*m_managerOld);
    }

private:
    class TestPersistenceManager : public wxPersistenceManager
    {
    public:
        TestPersistenceManager()
            : m_config("PersistenceTests", "wxWidgets")
        {
        }

        ~TestPersistenceManager() wxOVERRIDE
        {
            m_config.DeleteAll();
        }

        wxConfigBase* GetConfig() const wxOVERRIDE
        {
            return const_cast<wxConfig*>(&m_config);
        }

    private:
        wxConfig m_config;
    };

    wxPersistenceManager *m_managerOld;
    TestPersistenceManager m_manager;
};

#endif // WX_TESTS_PERSIST_TESTPERSISTENCE_H
