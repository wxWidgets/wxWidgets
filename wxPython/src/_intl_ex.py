#----------------------------------------------------------------------------
# wxGTK sets the locale when initialized.  Doing this at the Python
# level should set it up to match what GTK is doing at the C level.
if wx.Platform == "__WXGTK__":
    try:
        import locale
        locale.setlocale(locale.LC_ALL, "")
    except:
        pass

# On MSW add the directory where the wxWindows catalogs were installed
# to the default catalog path.
if wx.Platform == "__WXMSW__":
    import os
    localedir = os.path.join(os.path.split(__file__)[0], "locale")
    Locale_AddCatalogLookupPathPrefix(localedir)
    del os

#----------------------------------------------------------------------------
