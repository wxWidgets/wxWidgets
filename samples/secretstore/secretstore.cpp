/////////////////////////////////////////////////////////////////////////////
// Name:        secretstore.cpp
// Purpose:     wxWidgets sample showing the use of wxSecretStore class
// Author:      Vadim Zeitlin
// Created:     2016-05-27
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/secretstore.h"

#include "wx/init.h"
#include "wx/crt.h"
#include "wx/log.h"

bool Save(wxSecretStore& store, const wxString& service, const wxString& user)
{
    char password[4096];
    wxPrintf("Enter the password for %s/%s (echoing NOT disabled): ",
             service, user);

    if ( !wxFgets(password, WXSIZEOF(password), stdin) )
    {
        wxFprintf(stderr, "Password not stored.\n");
        return false;
    }

    size_t size = wxStrlen(password);
    if ( size )
    {
        // Strip trailing new line.
        --size;
        password[size] = 0;
    }

    wxSecretValue secret(size, password);

    // The password data was copied into wxSecretValue, don't leave it lying
    // around in the stack unnecessarily.
    wxSecretValue::Wipe(size, password);

    if ( !store.Save(service, user, secret) )
    {
        wxFprintf(stderr,
                  "Failed to save the password for %s/%s.\n",
                  service, user);
        return false;
    }

    wxPrintf("Password for %s/%s saved.\n",
            service, user);

    return true;
}

bool Load(wxSecretStore& store, const wxString& service, const wxString& user)
{
    wxSecretValue secret = store.Load(service, user);
    if ( !secret.IsOk() )
    {
        wxFprintf(stderr,
                  "Failed to load the password for %s/%s.\n",
                  service, user);
        return false;
    }

    // Create a temporary variable just to make it possible to wipe it after
    // using it.
    wxString str(secret.GetAsString());

    const size_t size = secret.GetSize();
    wxPrintf("Password for %s/%s is %zu bytes long: \"%s\"\n",
             service, user, size, str);

    wxSecretValue::WipeString(str);

    return true;
}

bool Delete(wxSecretStore& store, const wxString& service, const wxString& user)
{
    if ( !store.Delete(service, user) )
    {
        wxFprintf(stderr,
                  "Password for %s/%s not deleted.\n",
                  service, user);
        return false;
    }

    wxPrintf("Stored password for %s/%s deleted.\n",
             service, user);

    return true;
}

static bool PrintResult(bool ok)
{
    wxPuts(ok ? "ok" : "ERROR");
    return ok;
}

bool SelfTest(wxSecretStore& store, const wxString& service, const wxString& user)
{
    wxPrintf("Running the tests...\n");

    const wxSecretValue secret1(6, "secret");

    wxPrintf("Storing the password:\t");
    bool ok = store.Save(service, user, secret1);
    if ( !PrintResult(ok) )
    {
        // The rest of the tests will probably fail too, no need to continue.
        wxPrintf("Bailing out.\n");
        return false;
    }

    wxPrintf("Loading the password:\t");
    wxSecretValue secret = store.Load(service, user);
    ok = secret.IsOk() &&
            secret.GetSize() == secret1.GetSize() &&
                memcmp(secret.GetData(), secret1.GetData(), secret1.GetSize()) == 0;
    if ( !PrintResult(secret == secret1) )
        ok = false;

    // Overwriting the password should work.
    const wxSecretValue secret2(6, "privet");

    wxPrintf("Changing the password:\t");
    if ( PrintResult(store.Save(service, user, secret2)) )
    {
        wxPrintf("Reloading the password:\t");
        secret = store.Load(service, user);
        if ( !PrintResult(secret == secret2) )
            ok = false;
    }
    else
        ok = false;

    wxPrintf("Deleting the password:\t");
    if ( !PrintResult(store.Delete(service, user)) )
        ok = false;

    // This is supposed to fail now.
    wxPrintf("Deleting it again:\t");
    if ( !PrintResult(!store.Delete(service, user)) )
        ok = false;

    // And loading should fail too.
    wxPrintf("Loading after deleting:\t");
    if ( !PrintResult(!store.Load(service, user).IsOk()) )
        ok = false;

    if ( ok )
        wxPrintf("All tests passed!\n");

    return ok;
}

int main(int argc, char **argv)
{
    // To complement the standard EXIT_{SUCCESS,FAILURE}.
    const int EXIT_SYNTAX = 2;

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
        return EXIT_FAILURE;
    }

    if ( argc != 4 )
    {
        wxFprintf(stderr,
                  "Usage: %s {save|load|delete|selftest} <service> <user>\n"
                  "\n"
                  "Sample showing wxSecretStore class functionality.\n"
                  "Specify one of the commands to perform the corresponding\n"
                  "function call. The \"service\" and \"user\" arguments are\n"
                  "mandatory, \"save\" will also prompt for password.\n",
                  argv[0]);
        return EXIT_SYNTAX;
    }

    wxSecretStore store = wxSecretStore::GetDefault();
    if ( !store.IsOk() )
    {
        wxFprintf(stderr, "Failed to create default secret store.\n");
        return EXIT_FAILURE;
    }

    const wxString operation = argv[1];
    const wxString service = argv[2];
    const wxString user = argv[3];

    bool ok;
    if ( operation == "save" )
    {
        ok = Save(store, service, user);
    }
    else if ( operation == "load" )
    {
        ok = Load(store, service, user);
    }
    else if ( operation == "delete" )
    {
        ok = Delete(store, service, user);
    }
    else if ( operation == "selftest" )
    {
        ok = SelfTest(store, service, user);
    }
    else
    {
        wxFprintf(stderr,
                  "Unknown operation \"%s\", expected \"save\", \"load\" or "
                  "\"delete\".\n",
                  operation);
        return EXIT_SYNTAX;
    }

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
