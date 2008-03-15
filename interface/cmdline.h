/////////////////////////////////////////////////////////////////////////////
// Name:        cmdline.h
// Purpose:     interface of wxCmdLineParser
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCmdLineParser
    @wxheader{cmdline.h}

    wxCmdLineParser is a class for parsing the command line.

    It has the following features:

    - distinguishes options, switches and parameters
    - allows option grouping
    - allows both short and long options
    - automatically generates the usage message from the command line description
    - checks types of the options values (number, date, ...).

    To use it you should follow these steps:

    -# @ref wxCmdLineParser::construction construct an object of this class
    giving it the command line to parse and optionally its description or use
    @c AddXXX() functions later
    -# call @c Parse()
    -# use @c Found() to retrieve the results

    In the documentation below the following terminology is used:

    - @e switch
    This is a boolean option which can be given or not, but
    which doesn't have any value. We use the word switch to distinguish such boolean
    options from more generic options like those described below. For example,
    @c -v might be a switch meaning "enable verbose mode".
    - @e option
    Option for us here is something which comes with a value 0
    unlike a switch. For example, @c -o:filename might be an option which allows
    to specify the name of the output file.
    - @e parameter
    This is a required program argument.



    @library{wxbase}
    @category{appmanagement}

    @see wxApp::argc and wxApp::argv, console sample
*/
class wxCmdLineParser
{
public:
    //@{
    /**
        Specifies both the command line (in Windows format) and the
        @ref setdesc() "command line description".
    */
    wxCmdLineParser();
    wxCmdLineParser(int argc, char** argv);
    wxCmdLineParser(int argc, wchar_t** argv);
    wxCmdLineParser(const wxString& cmdline);
    wxCmdLineParser(const wxCmdLineEntryDesc* desc);
    wxCmdLineParser(const wxCmdLineEntryDesc* desc, int argc,
                    char** argv);
    wxCmdLineParser(const wxCmdLineEntryDesc* desc,
                    const wxString& cmdline);
    //@}

    /**
        Frees resources allocated by the object.
        @b NB: destructor is not virtual, don't use this class polymorphically.
    */
    ~wxCmdLineParser();

    /**
        Add an option @a name with an optional long name @a lng (no long name if
        it is empty, which is default) taking a value of the given type (string by
        default) to the command line description.
    */
    void AddOption(const wxString& name,
                   const wxString& lng = wxEmptyString,
                   const wxString& desc = wxEmptyString,
                   wxCmdLineParamType type = wxCMD_LINE_VAL_STRING,
                   int flags = 0);

    /**
        Add a parameter of the given @a type to the command line description.
    */
    void AddParam(const wxString& desc = wxEmptyString,
                  wxCmdLineParamType type = wxCMD_LINE_VAL_STRING,
                  int flags = 0);

    /**
        Add a switch @a name with an optional long name @a lng (no long name if it
        is empty, which is default), description @a desc and flags @a flags to the
        command line description.
    */
    void AddSwitch(const wxString& name,
                   const wxString& lng = wxEmptyString,
                   const wxString& desc = wxEmptyString,
                   int flags = 0);

    /**
        Returns @true if long options are enabled, otherwise @false.
        
        @see EnableLongOptions()
    */
    bool AreLongOptionsEnabled() const;

    /**
        Before Parse() can be called, the command line
        parser object must have the command line to parse and also the rules saying
        which switches, options and parameters are valid - this is called command line
        description in what follows.
        You have complete freedom of choice as to when specify the required information,
        the only restriction is that it must be done before calling
        Parse().
        To specify the command line to parse you may use either one of constructors
        accepting it (@c wxCmdLineParser(argc, argv) or @c wxCmdLineParser(const
        wxString) usually)
        or, if you use the default constructor, you can do it later by calling
        SetCmdLine().
        The same holds for command line description: it can be specified either in
        the @ref wxcmdlineparserctor() constructor (with or without
        the command line itself) or constructed later using either
        SetDesc() or combination of
        AddSwitch(),
        AddOption() and
        AddParam() methods.
        Using constructors or SetDesc() uses a (usually
        @c const static) table containing the command line description. If you want
        to decide which options to accept during the run-time, using one of the
        @c AddXXX() functions above might be preferable.
    */


    /**
        Breaks down the string containing the full command line in words. The words are
        separated by whitespace. The quotes can be used in the input string to quote
        the white space and the back slashes can be used to quote the quotes.
    */
    static wxArrayString ConvertStringToArgs(const wxChar cmdline);

    /**
        wxCmdLineParser has several global options which may be changed by the
        application. All of the functions described in this section should be called
        before Parse().
        First global option is the support for long (also known as GNU-style) options.
        The long options are the ones which start with two dashes (@c "--") and look
        like this: @c --verbose, i.e. they generally are complete words and not some
        abbreviations of them. As long options are used by more and more applications,
        they are enabled by default, but may be disabled with
        DisableLongOptions().
        Another global option is the set of characters which may be used to start an
        option (otherwise, the word on the command line is assumed to be a parameter).
        Under Unix, @c '-' is always used, but Windows has at least two common
        choices for this: @c '-' and @c '/'. Some programs also use @c '+'.
        The default is to use what suits most the current platform, but may be changed
        with SetSwitchChars() method.
        Finally, SetLogo() can be used to show some
        application-specific text before the explanation given by
        Usage() function.
    */


    /**
        Identical to @ref enablelongoptions() EnableLongOptions(@false).
    */
    void DisableLongOptions();

    /**
        Enable or disable support for the long options.
        As long options are not (yet) POSIX-compliant, this option allows to disable
        them.
        
        @see Customization() and AreLongOptionsEnabled()
    */
    void EnableLongOptions(bool enable = true);

    //@{
    /**
        Returns @true if an option taking a date value was found and stores the
        value in the provided pointer (which should not be @NULL).
    */
    bool Found(const wxString& name) const;
    bool Found(const wxString& name, wxString* value) const;
    bool Found(const wxString& name, long* value) const;
    bool Found(const wxString& name, double* value) const;
    bool Found(const wxString& name, wxDateTime* value) const;
    //@}

    /**
        Returns the value of Nth parameter (as string only).
    */
    wxString GetParam(size_t n = 0u) const;

    /**
        Returns the number of parameters found. This function makes sense mostly if you
        had used @c wxCMD_LINE_PARAM_MULTIPLE flag.
    */
    size_t GetParamCount() const;

    /**
        After calling Parse() (and if it returned 0),
        you may access the results of parsing using one of overloaded @c Found()
        methods.
        For a simple switch, you will simply call
        Found() to determine if the switch was given
        or not, for an option or a parameter, you will call a version of @c Found()
        which also returns the associated value in the provided variable. All
        @c Found() functions return @true if the switch or option were found in the
        command line or @false if they were not specified.
    */


    /**
        Parse the command line, return 0 if ok, -1 if @c "-h" or @c "--help"
        option was encountered and the help message was given or a positive value if a
        syntax error occurred.
        
        @param giveUsage
            If @true (default), the usage message is given if a
            syntax error was encountered while parsing the command line or if help was
            requested. If @false, only error messages about possible syntax errors
            are given, use Usage to show the usage message
            from the caller if needed.
    */
    int Parse(bool giveUsage = true);

    /**
        After the command line description was constructed and the desired options were
        set, you can finally call Parse() method.
        It returns 0 if the command line was correct and was parsed, -1 if the help
        option was specified (this is a separate case as, normally, the program will
        terminate after this) or a positive number if there was an error during the
        command line parsing.
        In the latter case, the appropriate error message and usage information are
        logged by wxCmdLineParser itself using the standard wxWidgets logging functions.
    */


    //@{
    /**
        Set command line to parse after using one of the constructors which don't do it.
    */
    void SetCmdLine(int argc, char** argv);
    void SetCmdLine(int argc, wchar_t** argv);
    void SetCmdLine(const wxString& cmdline);
    //@}

    /**
        Construct the command line description
        Take the command line description from the wxCMD_LINE_NONE terminated table.
        Example of usage:
    */
    void SetDesc(const wxCmdLineEntryDesc* desc);

    /**
        @a logo is some extra text which will be shown by
        Usage() method.
    */
    void SetLogo(const wxString& logo);

    /**
        @a switchChars contains all characters with which an option or switch may
        start. Default is @c "-" for Unix, @c "-/" for Windows.
    */
    void SetSwitchChars(const wxString& switchChars);

    /**
        Give the standard usage message describing all program options. It will use the
        options and parameters descriptions specified earlier, so the resulting message
        will not be helpful to the user unless the descriptions were indeed specified.
        
        @see SetLogo()
    */
    void Usage() const;
};

