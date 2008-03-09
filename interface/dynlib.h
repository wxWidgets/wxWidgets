/////////////////////////////////////////////////////////////////////////////
// Name:        dynlib.h
// Purpose:     documentation for wxDynamicLibraryDetails class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDynamicLibraryDetails
    @wxheader{dynlib.h}

    This class is used for the objects returned by
    wxDynamicLibrary::ListLoaded method and
    contains the information about a single module loaded into the address space of
    the current process. A module in this context may be either a dynamic library
    or the main program itself.

    @library{wxbase}
    @category{FIXME}
*/
class wxDynamicLibraryDetails
{
public:
    /**
        Retrieves the load address and the size of this module.
        
        @param addr
            the pointer to the location to return load address in, may be
            @NULL
        @param len
            pointer to the location to return the size of this module in
            memory in, may be @NULL
        
        @returns @true if the load address and module size were retrieved, @false
                 if this information is not available.
    */
    bool GetAddress(void** addr, size_t len) const;

    /**
        Returns the base name of this module, e.g. @c kernel32.dll or
        @c libc-2.3.2.so.
    */
    wxString GetName() const;

    /**
        Returns the full path of this module if available, e.g.
        @c c:\windows\system32\kernel32.dll or
        @c /lib/libc-2.3.2.so.
    */
    wxString GetPath() const;

    /**
        Returns the version of this module, e.g. @c 5.2.3790.0 or
        @c 2.3.2. The returned string is empty if the version information is not
        available.
    */
    wxString GetVersion() const;
};


/**
    @class wxDllLoader
    @wxheader{dynlib.h}

    @b Deprecation note: This class is deprecated since version 2.4 and is
    not compiled in by default in version 2.6 and will be removed in 2.8. Please
    use wxDynamicLibrary instead.

    wxDllLoader is a class providing an interface similar to Unix's @c dlopen(). It
    is used by the wxLibrary framework and manages the actual
    loading of shared libraries and the resolving of symbols in them. There are no
    instances of this class, it simply serves as a namespace for its static member
    functions.

    Please note that class wxDynamicLibrary provides
    alternative, friendlier interface to wxDllLoader.

    The terms @e DLL and @e shared library/object will both be used in the
    documentation to refer to the same thing: a @c .dll file under Windows or
    @c .so or @c .sl one under Unix.

    Example of using this class to dynamically load the @c strlen() function:

    @code
    #if defined(__WXMSW__)
        static const wxChar *LIB_NAME = _T("kernel32");
        static const wxChar *FUNC_NAME = _T("lstrlenA");
    #elif defined(__UNIX__)
        static const wxChar *LIB_NAME = _T("/lib/libc-2.0.7.so");
        static const wxChar *FUNC_NAME = _T("strlen");
    #endif

        wxDllType dllHandle = wxDllLoader::LoadLibrary(LIB_NAME);
        if ( !dllHandle )
        {
            ... error ...
        }
        else
        {
            typedef int (*strlenType)(char *);
            strlenType pfnStrlen = (strlenType)wxDllLoader::GetSymbol(dllHandle,
    FUNC_NAME);
            if ( !pfnStrlen )
            {
                ... error ...
            }
            else
            {
                if ( pfnStrlen("foo") != 3 )
                {
                    ... error ...
                }
                else
                {
                    ... ok! ...
                }
            }

            wxDllLoader::UnloadLibrary(dllHandle);
        }
    @endcode

    @library{wxbase}
    @category{appmanagement}
*/
class wxDllLoader
{
public:
    /**
        Returns the string containing the usual extension for shared libraries for the
        given systems (including the leading dot if not empty).
        For example, this function will return @c ".dll" under Windows or (usually)
        @c ".so" under Unix.
    */
    static wxString GetDllExt();

    /**
        This function returns a valid handle for the main program itself. Notice that
        the @NULL return value is valid for some systems (i.e. doesn't mean that
        the function failed).
        @b NB: This function is Unix specific. It will always fail under Windows
        or OS/2.
    */
    wxDllType GetProgramHandle();

    /**
        This function resolves a symbol in a loaded DLL, such as a variable or
        function name.
        Returned value will be @NULL if the symbol was not found in the DLL or if
        an error occurred.
        
        @param dllHandle
            Valid handle previously returned by
            LoadLibrary
        @param name
            Name of the symbol.
    */
    void* GetSymbol(wxDllType dllHandle, const wxString& name);

    /**
        This function loads a shared library into memory, with @a libname being the
        name of the library: it may be either the full name including path and
        (platform-dependent) extension, just the basename (no path and no extension)
        or a basename with extension. In the last two cases, the library will be
        searched in all standard locations.
        Returns a handle to the loaded DLL. Use @a success parameter to test if it
        is valid. If the handle is valid, the library must be unloaded later with
        UnloadLibrary().
        
        @param libname
            Name of the shared object to load.
        @param success
            May point to a bool variable which will be set to @true or
            @false; may also be @NULL.
    */
    wxDllType LoadLibrary(const wxString& libname,
                          bool* success = NULL);

    /**
        This function unloads the shared library. The handle @a dllhandle must have
        been returned by LoadLibrary() previously.
    */
    void UnloadLibrary(wxDllType dllhandle);
};


/**
    @class wxDynamicLibrary
    @wxheader{dynlib.h}

    wxDynamicLibrary is a class representing dynamically loadable library
    (Windows DLL, shared library under Unix etc.). Just create an object of
    this class to load a library and don't worry about unloading it -- it will be
    done in the objects destructor automatically.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxDynamicLibrary::CanonicalizePluginName
*/
class wxDynamicLibrary
{
public:
    //@{
    /**
        Constructor. Second form calls Load().
    */
    wxDynamicLibrary();
    wxDynamicLibrary(const wxString& name,
                     int flags = wxDL_DEFAULT);
    //@}

    /**
        Returns the platform-specific full name for the library called @e name. E.g.
        it adds a @c ".dll" extension under Windows and @c "lib" prefix and
        @c ".so", @c ".sl" or maybe @c ".dylib" extension under Unix.
        The possible values for @a cat are:
        
        
        wxDL_LIBRARY
        
        normal library
        
        wxDL_MODULE
        
        a loadable module or plugin
        
        @see CanonicalizePluginName()
    */
    static wxString CanonicalizeName(const wxString& name,
                                     wxDynamicLibraryCategory cat = wxDL_LIBRARY);

    /**
        This function does the same thing as
        CanonicalizeName() but for wxWidgets
        plugins. The only difference is that compiler and version information are added
        to the name to ensure that the plugin which is going to be loaded will be
        compatible with the main program.
        The possible values for @a cat are:
        
        
        wxDL_PLUGIN_GUI
        
        plugin which uses GUI classes (default)
        
        wxDL_PLUGIN_BASE
        
        plugin which only uses wxBase
    */
    static wxString CanonicalizePluginName(const wxString& name,
                                           wxPluginCategory cat = wxDL_PLUGIN_GUI);

    /**
        Detaches this object from its library handle, i.e. the object will not unload
        the library any longer in its destructor but it is now the callers
        responsibility to do this using Unload().
    */
    wxDllType Detach();

    /**
        Return a valid handle for the main program itself or @NULL if symbols
        from the main program can't be loaded on this platform.
    */
    static wxDllType GetProgramHandle();

    /**
        Returns pointer to symbol @a name in the library or @NULL if the library
        contains no such symbol.
        
        @see wxDYNLIB_FUNCTION
    */
    void* GetSymbol(const wxString& name) const;

    /**
        This function is available only under Windows as it is only useful when
        dynamically loading symbols from standard Windows DLLs. Such functions have
        either @c 'A' (in ANSI build) or @c 'W' (in Unicode, or wide
        character build) suffix if they take string parameters. Using this function you
        can use just the base name of the function and the correct suffix is appende
        automatically depending on the current build. Otherwise, this method is
        identical to GetSymbol().
    */
    void* GetSymbolAorW(const wxString& name) const;

    /**
        Returns @true if the symbol with the given @a name is present in the dynamic
        library, @false otherwise. Unlike GetSymbol(),
        this function doesn't log an error message if the symbol is not found.
        This function is new since wxWidgets version 2.5.4
    */
    bool HasSymbol(const wxString& name) const;

    /**
        Returns @true if the library was successfully loaded, @false otherwise.
    */
    bool IsLoaded() const;

    /**
        This static method returns an array containing the details
        of all modules loaded into the address space of the current project, the array
        elements are object of @c wxDynamicLibraryDetails class. The array will
        be empty if an error occurred.
        This method is currently implemented only under Win32 and Linux and is useful
        mostly for diagnostics purposes.
    */
    static wxDynamicLibraryDetailsArray ListLoaded();

    /**
        Loads DLL with the given @a name into memory. The @a flags argument can
        be a combination of the following bits:
        
        wxDL_LAZY
        
        equivalent of RTLD_LAZY under Unix, ignored elsewhere
        
        wxDL_NOW
        
        equivalent of RTLD_NOW under Unix, ignored elsewhere
        
        wxDL_GLOBAL
        
        equivalent of RTLD_GLOBAL under Unix, ignored elsewhere
        
        wxDL_VERBATIM
        
        don't try to append the appropriate extension to
        the library name (this is done by default).
        
        wxDL_DEFAULT
        
        default flags, same as wxDL_NOW currently
        
        wxDL_QUIET
        
        don't log an error message if the library couldn't be
        loaded.
        
        Returns @true if the library was successfully loaded, @false otherwise.
    */
    bool Load(const wxString& name, int flags = wxDL_DEFAULT);

    //@{
    /**
        Unloads the library from memory. wxDynamicLibrary object automatically calls
        this method from its destructor if it had been successfully loaded.
        The second version is only used if you need to keep the library in memory
        during a longer period of time than the scope of the wxDynamicLibrary object.
        In this case you may call Detach() and store
        the handle somewhere and call this static method later to unload it.
    */
    void Unload();
    static void Unload(wxDllType handle);
    //@}
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    When loading a function from a DLL you always have to cast the returned
    @c void * pointer to the correct type and, even more annoyingly, you have to
    repeat this type twice if you want to declare and define a function pointer all
    in one line
    This macro makes this slightly less painful by allowing you to specify the
    type only once, as the first parameter, and creating a variable of this type
    named after the function but with @c pfn prefix and initialized with the
    function @a name from the wxDynamicLibrary
    @e dynlib.

    @param type
        the type of the function
    @param name
        the name of the function to load, not a string (without quotes,
        it is quoted automatically by the macro)
    @param dynlib
        the library to load the function from
*/
#define wxDYNLIB_FUNCTION(type, name, dynlib)     /* implementation is private */

