#----------------------------------------------------------------------------
# On MSW add the directory where the wxWidgets catalogs were installed
# to the default catalog path.
if wx.Platform == "__WXMSW__":
    import os
    _localedir = os.path.join(os.path.split(__file__)[0], "locale")
    Locale.AddCatalogLookupPathPrefix(_localedir)
    del os

#----------------------------------------------------------------------------
