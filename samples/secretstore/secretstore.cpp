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


#include "wx/secretstore.h"

#include "wx/init.h"
#include "wx/crt.h"
#include "wx/log.h"

#include <cctype>

bool g_verbose_ {false};
bool g_binary_ {false};

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

bool Load(wxSecretStore& store, const wxString& service)
{
    wxString user;
    wxSecretValue secret;
    if ( !store.Load(service, user, secret) )
    {
        wxFprintf(stderr, "Failed to load the password for %s.\n", service);
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

bool Delete(wxSecretStore& store, const wxString& service)
{
    if ( !store.Delete(service) )
    {
        wxFprintf(stderr, "Password for %s not deleted.\n", service);
        return false;
    }

    wxPrintf("Stored password for %s deleted.\n", service);

    return true;
}

static bool PrintResult(bool ok)
{
    wxPuts(ok ? "ok" : "ERROR");
    return ok;
}

static void PrintSecrets(const wxSecretValue& expected, const wxSecretValue& loaded)
{
    wxPrintf("Expected: size=%ld data=", expected.GetSize());
    if (g_binary_)
    {
        wxPrintf("[");
        size_t n = expected.GetSize();
        const char* p = static_cast<const char *>(expected.GetData());
        for (size_t i=0; i<n ;++i)
            if (std::isprint(p[i]))
                wxPrintf("%c", p[i]);
            else
                wxPrintf("\\x%hhx", p[i]);
        wxPrintf("]\n");
    }
    else
    {
        wxPrintf("\"%s\"\n", expected.GetAsString());
    }
    wxPrintf("Loaded: size=%ld data=", loaded.GetSize());
    if (g_binary_)
    {
        wxPrintf("[");
        size_t n = loaded.GetSize();
        const char* p = static_cast<const char *>(loaded.GetData());
        for (size_t i=0; i<n ;++i)
          if (std::isprint(p[i]))
              wxPrintf("%c", p[i]);
          else
              wxPrintf("\\x%hhx", p[i]);
        wxPrintf("]\n");
    }
    else
    {
        wxPrintf("\"%s\"\n", loaded.GetAsString());
    }
}

bool SelfTest(wxSecretStore& store, const wxString& service)
{
    wxPrintf("Running the tests with %s secrets...\n", g_binary_ ? "binary" : "text");

    const wxString userTest("test");
    const wxSecretValue secret1 = g_binary_ ? wxSecretValue(11, "secret\x1\x86\x2\x99\x3") : wxSecretValue("secret");

    wxPrintf("Storing the password:\t");
    bool ok = store.Save(service, userTest, secret1);
    if ( !PrintResult(ok) )
    {
        // The rest of the tests will probably fail too, no need to continue.
        wxPrintf("Bailing out.\n");
        return false;
    }

    wxPrintf("Loading the password:\t");
    wxSecretValue secret;
    wxString user;
    ok = PrintResult(store.Load(service, user, secret) &&
                        user == userTest &&
                            secret == secret1);
    if (!ok && g_verbose_)
    {
        PrintSecrets(secret1, secret);
    }

    // Overwriting the password should work.
    const wxSecretValue secret2 = g_binary_ ? wxSecretValue(11, "privet\x1\x86\x2\x99\x3") : wxSecretValue("privet");

    wxPrintf("Changing the password:\t");
    if ( PrintResult(store.Save(service, user, secret2)) )
    {
        wxPrintf("Reloading the password:\t");
        if ( !PrintResult(store.Load(service, user, secret) &&
                            secret == secret2) )
        {
            ok = false;
            if (g_verbose_)
            {
                PrintSecrets(secret2, secret);
            }
        }
    }
    else
        ok = false;

    wxPrintf("Deleting the password:\t");
    if ( !PrintResult(store.Delete(service)) )
        ok = false;

    // This is supposed to fail now.
    wxPrintf("Deleting it again:\t");
    if ( !PrintResult(!store.Delete(service)) )
        ok = false;

    // And loading should fail too.
    wxPrintf("Loading after deleting:\t");
    if ( !PrintResult(!store.Load(service, user, secret)) )
        ok = false;

    if ( ok )
        wxPrintf("All tests passed!\n");

    return ok;
}

void usage(char **argv)
{
  wxFprintf(stderr,
            "Usage: %s save <service> <user>\n"
            "   or  %s {load|delete} <service>\n"
            "   or  %s [options] selftest <service>\n"
            "\n"
            "Sample showing wxSecretStore class functionality.\n"
            "Specify one of the commands to perform the corresponding\n"
            "function call. The \"service\" argument is mandatory for\n"
            "all commands, \"save\" also requires \"user\" and will\n"
            "prompt for password.\n\n"
            "options:\n"
            "\t-v\trun verbose (possibly shows secrets on errors)\n"
            "\t-b\trun selftest using binary secrets (otherwise uses text strings)\n\n",
            argv[0], argv[0], argv[0]);
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

    wxString operation;
    wxString service;
    wxString user;

    for (int arg=1; arg<argc ;++arg)
    {
      if (argv[arg][0] == '-')
      {
        if (argv[arg][1] == 'v' && argv[arg][2] == '\0')
          g_verbose_ = true;
        else if (argv[arg][1] == 'b' && argv[arg][2] == '\0')
          g_binary_ = true;
        else
        {
          wxFprintf(stderr,
                    "ERROR: Unknown switch : %s\n\n",
                    argv[arg]);
          usage(argv);
          return EXIT_SYNTAX;
        }
      }
      else if (operation.IsEmpty())
      {
        operation = argv[arg];
      }
      else if (service.IsEmpty())
      {
        service = argv[arg];
      }
      else if (operation == "save" && user.IsEmpty())
      {
        user = argv[arg];
      }
      else
      {
        wxFprintf(stderr,
                  "ERROR: Unknown argument : %s\n\n",
                  argv[arg]);
        usage(argv);
        return EXIT_SYNTAX;
      }
    }

    if (operation.IsEmpty() || service.IsEmpty())
    {
      if (operation.IsEmpty())
        wxFprintf(stderr,
                  "ERROR: Missing required operation argument\n\n");
      else
        wxFprintf(stderr,
                  "ERROR: Missing required service argument\n\n");
      usage(argv);
      return EXIT_SYNTAX;
    }

    wxSecretStore store = wxSecretStore::GetDefault();
    wxString errmsg;
    if ( !store.IsOk(&errmsg) )
    {
        wxFprintf(stderr, "Failed to create default secret store (%s)\n",
                  errmsg);
        return EXIT_FAILURE;
    }

    bool ok;
    if ( operation == "save" )
    {
        if (user.IsEmpty())
        {
            wxFprintf(stderr,
                      "ERROR: Missing required user argument\n\n");
            usage(argv);
            return EXIT_SYNTAX;
        }
        ok = Save(store, service, user);
    }
    else if ( operation == "load" )
    {
        ok = Load(store, service);
    }
    else if ( operation == "delete" )
    {
        ok = Delete(store, service);
    }
    else if ( operation == "selftest" )
    {
        ok = SelfTest(store, service);
    }
    else
    {
        wxFprintf(stderr,
                  "Unknown operation \"%s\", expected \"save\", \"load\" or "
                  "\"delete\".\n\n",
                  operation);
        usage(argv);
        return EXIT_SYNTAX;
    }

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
