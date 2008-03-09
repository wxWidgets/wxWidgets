/////////////////////////////////////////////////////////////////////////////
// Name:        snglinst.h
// Purpose:     documentation for wxSingleInstanceChecker class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSingleInstanceChecker
    @wxheader{snglinst.h}

    wxSingleInstanceChecker class allows to check that only a single instance of a
    program is running. To do it, you should create an object of this class. As
    long as this object is alive, calls to
    wxSingleInstanceChecker::IsAnotherRunning from
    other processes will return @true.

    As the object should have the life span as big as possible, it makes sense to
    create it either as a global or in wxApp::OnInit. For
    example:

    @code
    bool MyApp::OnInit()
    {
        const wxString name = wxString::Format("MyApp-%s", wxGetUserId().c_str());
        m_checker = new wxSingleInstanceChecker(name);
        if ( m_checker-IsAnotherRunning() )
        {
            wxLogError(_("Another program instance is already running, aborting."));

            delete m_checker; // OnExit() won't be called if we return @false
            m_checker = @NULL;

            return @false;
        }

        ... more initializations ...

        return @true;
    }

    int MyApp::OnExit()
    {
        delete m_checker;

        return 0;
    }
    @endcode

    Note using wxGetUserId to construct the name: this
    allows different user to run the application concurrently which is usually the
    intended goal. If you don't use the user name in the wxSingleInstanceChecker
    name, only one user would be able to run the application at a time.

    This class is implemented for Win32 and Unix platforms (supporting @c fcntl()
    system call, but almost all of modern Unix systems do) only.

    @library{wxbase}
    @category{misc}
*/
class wxSingleInstanceChecker
{
public:
    /**
        Like Create() but without
        error checking.
    */
    wxSingleInstanceChecker(const wxString& name,
                            const wxString& path = wxEmptyString);

    /**
        Destructor frees the associated resources.
        Note that it is not virtual, this class is not meant to be used polymorphically
    */
    ~wxSingleInstanceChecker();

    /**
        Initialize the object if it had been created using the default constructor.
        Note that you can't call Create() more than once, so calling it if the
        @ref wxsingleinstancechecker() "non default ctor"
        had been used is an error.
        
        @param name
            must be given and be as unique as possible. It is used as the
            mutex name under Win32 and the lock file name under Unix.
            GetAppName() and wxGetUserId()
            are commonly used to construct this parameter.
        @param path
            is optional and is ignored under Win32 and used as the directory to
            create the lock file in under Unix (default is
            wxGetHomeDir())
        
        @returns Returns @false if initialization failed, it doesn't mean that
                 another instance is running - use  IsAnotherRunning()
                 to check for it.
    */
    bool Create(const wxString& name,
                const wxString& path = wxEmptyString);

    /**
        Returns @true if another copy of this program is already running, @false
        otherwise.
    */
    bool IsAnotherRunning() const;
};
