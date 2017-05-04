/////////////////////////////////////////////////////////////////////////////
// Name:        filepicker.h
// Purpose:     interface of wxFilePickerCtrl
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#define wxFLP_OPEN                    0x0400
#define wxFLP_SAVE                    0x0800
#define wxFLP_OVERWRITE_PROMPT        0x1000
#define wxFLP_FILE_MUST_EXIST         0x2000
#define wxFLP_CHANGE_DIR              0x4000
#define wxFLP_SMALL                   wxPB_SMALL
#define wxFLP_USE_TEXTCTRL            (wxPB_USE_TEXTCTRL)
#define wxFLP_DEFAULT_STYLE           (wxFLP_OPEN|wxFLP_FILE_MUST_EXIST)

#define wxDIRP_DIR_MUST_EXIST         0x0008
#define wxDIRP_CHANGE_DIR             0x0010
#define wxDIRP_SMALL                  wxPB_SMALL
#define wxDIRP_USE_TEXTCTRL           (wxPB_USE_TEXTCTRL)
#define wxDIRP_DEFAULT_STYLE          (wxDIRP_DIR_MUST_EXIST)

wxEventType wxEVT_FILEPICKER_CHANGED;
wxEventType wxEVT_DIRPICKER_CHANGED;


/**
    @class wxFilePickerCtrl

    This control allows the user to select a file to open or save. The generic 
    implementation contains a "Browse" button which, when pressed, shows a 
    wxFileDialog for selecting a file.

    @beginStyleTable
    @style{wxFLP_DEFAULT_STYLE}
           Includes @c wxFLP_OPEN, @c wxFLP_FILE_MUST_EXIST and @c
           wxFLP_USE_TEXTCTRL for all platforms except wxGTK, which does not 
           include @c wxFLP_USE_TEXTCTRL.
    @style{wxFLP_USE_TEXTCTRL}
           Add a synchronized wxTextCtrl to the left of the file picker 
           button. It allows the user to enter or modify a filepath.
           See wxPickerBase for additional functions to modify this text 
           control.
    @style{wxFLP_OPEN}
           Configure the picker for selecting a file to open.
    @style{wxFLP_SAVE}
           Configure the picker for entering a file to save to. User will 
           not be asked for confirmation if the file already exists.
    @style{wxFLP_OVERWRITE_PROMPT}
           Ask user to confirm overwriting the entered file if it already 
           exists. Can only be combined with @c wxFLP_SAVE.
    @style{wxFLP_FILE_MUST_EXIST}
           Prevent the user from entering an unexisting file in the 
           wxFileDialog. Can only be combined with @c wxFLP_OPEN.
           This style does not guarantee that the filepath entered by the user
           in the wxTextCtrl (@c wxFLP_USE_TEXTCTRL) actually exists.
    @style{wxFLP_CHANGE_DIR}
           Change current working directory on each user file selection change.
    @style{wxFLP_SMALL}
           Use smaller button with "..." label instead of the default "Browse" 
           button. Since 2.9.3.
    @endStyleTable


    @beginEventEmissionTable{wxFileDirPickerEvent}
    @event{EVT_FILEPICKER_CHANGED(id, func)}
        Handle a wxEVT_FILEPICKER_CHANGED event, which is generated when a 
        different file is selected using the picker button or wxTextCtrl (see 
        @c wxFLP_USE_TEXTCTRL).
    @endEventTable

    @library{wxcore}
    @category{pickers}
    @appearance{filepickerctrl}

    @see wxFileDialog, wxFileDirPickerEvent
*/
class wxFilePickerCtrl : public wxPickerBase
{
public:

    /**
        Default constructor.

        @see Create()
    */
    wxFilePickerCtrl();
    
    /**
        Constructor, creating and showing the file picker control.

        @param parent
            Parent window. Should not be @NULL.
        @param id
            Control identifier. wxID_ANY indicates a default value.
        @param path
            The initial filepath selected in the wxFileDialog (and wxTextCtrl 
            if @c wxFLP_USE_TEXTCTRL was set). Must be a valid file path or an 
            empty string.
        @param message
            wxFileDialog caption, e.g. "Select a file".
        @param wildcard
            A string describing which files with matching file extensions 
            should be shown in the wxFileDialog. Files not matching this 
            wildcard will be hidden. E.g. use "PNG files (*.png)|*.png" to only
            show files with a .png extension (see Remarks below for more 
            examples). Note that setting this wildcard does mean that the 
            entered filepath is validated: it can still have an extension not 
            listed in the wildcard.
        @param pos
            Initial control position on parent.
        @param size
            Initial control size.
        @param style
            Control style. See @c wxFLP_* styles above.
        @param validator
            Control validator. Note that this parameter is not passed to the 
            wxTextCtrl if @c wxFLP_USE_TEXTCTRL style is used.
        @param name
            Control name.

        @remarks
            Wildcards are in the form of: "Description (*.ext)|*.ext", where 
            "Description (*.ext)" is visible to the user, and "*.ext" is used 
            internally to match the file extension.
            For example: use "PNG files (*.png)|*.png" to show a single file 
            extension on a single line in the wxFileDialog choice filter; 
            use "TIFF files (*.tif;*.tiff)|*.tif;*.tiff" for multiple 
            file extensions on a single line; and use
            "XML files (*.xml)|*.xml|PNG files (*.png)|*.png" with a '|' 
            separator for multiple file types on multiple lines.
    */
    wxFilePickerCtrl(wxWindow* parent, wxWindowID id,
                     const wxString& path = wxEmptyString,
                     const wxString& message = wxFileSelectorPromptStr,
                     const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxFLP_DEFAULT_STYLE,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxFilePickerCtrlNameStr);

    /**
        Creates the picker control for two-step construction. This method 
        should be called if the default constructor was used for the control 
        creation. Its parameters have the same meaning as for the non-default 
        constructor. 

        @return @true if the control was successfully created or @false if
                creation failed.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& path = wxEmptyString,
                const wxString& message = wxFileSelectorPromptStr,
                const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxFLP_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxFilePickerCtrlNameStr);

    /**
        Returns the absolute path of the currently selected file as a 
        wxFileName object. This object can be used, for example, to easily 
        extract the filename.
        
        @see GetPath()
    */
    wxFileName GetFileName() const;
    
    /**
        Returns the absolute path of the currently selected file.
    */
    wxString GetPath() const;
    
    /**
        Sets the absolute path of the currently selected file.

        @param filename
            wxFileName object with a full path to a file.

        @see SetPath()
    */
    void SetFileName(const wxFileName& filename);

    /**
        Sets the default directory to open when browsing for files. 
        
        This function is useful for pickers without an initial file path, to 
        improve folder navigation speed for the user.

        @param dir
            String containing an absolute directory path.

        @remarks
            The directory specified by this function will be ignored if an 
            initial file path was set.

        @since 2.9.4

        @see SetPath(), SetFileName()
    */
    void SetInitialDirectory(const wxString& dir);

    /**
        Sets the absolute path of the currently selected file.

        @param filename
            String containing the absolute path to a file.

        @remarks 
            If this control uses the @c wxFLP_FILE_MUST_EXIST style without @c 
            wxFLP_USE_TEXTCTRL, then the @a filename must be an existing file. 
            The native wxGTK implementation will ignore @a filename if the 
            file it points to does not exist. 
            The generic implementation, used under other platforms, accepts 
            unexisting file paths. Do not rely on this behaviour, as it may
            change in future versions.
    */
    void SetPath(const wxString& filename);
};



/**
    @class wxDirPickerCtrl

    This control allows the user to select a directory. The generic 
    implementation contains a button which, when pressed, shows a wxDirDialog 
    for selecting a directory.

    @beginStyleTable
    @style{wxDIRP_DEFAULT_STYLE}
           Includes @c wxDIRP_DIR_MUST_EXIST and @c wxDIRP_USE_TEXTCTRL for 
           all platforms except wxGTK, which does not include @c 
           wxDIRP_USE_TEXTCTRL.
    @style{wxDIRP_USE_TEXTCTRL}
           Add a synchronized wxTextCtrl to the left of the directory picker 
           button. It allows the user to enter or modify a directory path.
           See wxPickerBase for additional functions to modify this text 
           control.
    @style{wxDIRP_DIR_MUST_EXIST}
           Prevent the user from entering an unexisting directory in the
           wxDirDialog. This style does not guarantee that the directory 
           entered by the user in the wxTextCtrl (@c wxDIRP_USE_TEXTCTRL) 
           actually exists.
           Note that wxGTK always uses this style if @c wxDIRP_USE_TEXTCTRL is 
           not set. The native wxGTK implementation does not support selecting 
           unexisting directories.
    @style{wxDIRP_CHANGE_DIR}
           Change current working directory on each user directory selection 
           change.
    @style{wxDIRP_SMALL}
           Use smaller button with "..." label instead of the default "Browse" 
           button. Since 2.9.3.
    @endStyleTable

    @beginEventEmissionTable{wxFileDirPickerEvent}
    @event{EVT_DIRPICKER_CHANGED(id, func)}
        Handle a wxEVT_DIRPICKER_CHANGED event, which is generated when a 
        different directory is selected using the picker button or wxTextCtrl 
        (see @c wxDIRP_USE_TEXTCTRL).
    @endEventTable


    @library{wxcore}
    @category{pickers}
    @appearance{dirpickerctrl}

    @see wxDirDialog, wxFileDirPickerEvent
*/
class wxDirPickerCtrl : public wxPickerBase
{
public:

    /**
        Default constructor.

        @see Create()
    */
    wxDirPickerCtrl();
    
    /**
        Constructor, creating and showing the dir picker control.

        @param parent
            Parent window. Should not be @NULL.
        @param id
            Control identifier. wxID_ANY indicates a default value.
        @param path
            The initial directory shown in the wxFileDialog (and wxTextCtrl 
            if @c wxDIRP_USE_TEXTCTRL was set). Must be a valid directory path
            or an empty string.
        @param message
            wxDirDialog caption, e.g. "Select a directory".
        @param pos
            Initial control position on parent.
        @param size
            Initial control size.
        @param style
            Control style. See @c wxDIRP_* styles above.
        @param validator
            Control validator. Note that this parameter is not passed to the 
            wxTextCtrl if @c wxDIRP_USE_TEXTCTRL style is used.
        @param name
            Control name.
    */
    wxDirPickerCtrl(wxWindow* parent, wxWindowID id,
                    const wxString& path = wxEmptyString,
                    const wxString& message = wxDirSelectorPromptStr,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDIRP_DEFAULT_STYLE,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxDirPickerCtrlNameStr);

    /**
        Creates the picker control for two-step construction. This method 
        should be called if the default constructor was used for the control 
        creation. Its parameters have the same meaning as for the non-default 
        constructor. 

        @return @true if the control was successfully created or @false if
                creation failed.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& path = wxEmptyString,
                const wxString& message = wxDirSelectorPromptStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRP_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDirPickerCtrlNameStr);

    /**
        Returns the absolute path of the currently selected directory as a 
        wxFileName object. This object can be used, for example, to easily 
        extract the filename.
        
        @see GetPath()
    */
    wxFileName GetDirName() const;

    /**
        Returns the absolute path of the currently selected directory.
    */
    wxString GetPath() const;
    
    /**
        Sets the absolute path of the currently selected directory.

        @param filename
            wxFileName object with a path to a directory.

        @see SetPath()
    */
    void SetDirName(const wxFileName& dirname);
    
    /**
        Sets the default directory to open when browsing for files.

        This function is useful for pickers without an initial file path, to 
        improve folder navigation speed for the user.

        @param dir
            String containing an absolute directory path. 

        @remarks
            The directory specified by this function will be ignored if an 
            initial file path was set.

        @since 2.9.4

        @see SetPath(), SetDirName()
    */
    void SetInitialDirectory(const wxString& dir);

    /**
        Sets the absolute path of the currently selected directory.

        @remarks
            If the control uses the @c wxDIRP_DIR_MUST_EXIST style without
            @c wxDIRP_USE_TEXTCTRL: the @a dirname will be ignored for wxGTK
            if the directory does not exist.
    */
    void SetPath(const wxString& dirname);
};



/**
    @class wxFileDirPickerEvent

    This event class is used for the events generated by wxFilePickerCtrl and 
    wxDirPickerCtrl.

    @beginEventTable{wxFileDirPickerEvent}
    @event{EVT_FILEPICKER_CHANGED(id, func)}
        Generated whenever the selected file changes.
    @event{EVT_DIRPICKER_CHANGED(id, func)}
        Generated whenever the selected directory changes.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxFilePickerCtrl, wxDirPickerCtrl
*/
class wxFileDirPickerEvent : public wxCommandEvent
{
public:

    /**
        Default constructor.
    */
    wxFileDirPickerEvent();

    /**
        Constructor with picker related path string.

        @param type
            Control specific event types. Can be @c wxEVT_FILEPICKER_CHANGED or 
            @c wxEVT_DIRPICKER_CHANGED.
        @param generator
            Object creating the event.
        @param id
            wxWindowID of the window creating the event.
        @param path
            Newly selected absolute path to the file or directory.
    */
    wxFileDirPickerEvent(wxEventType type, wxObject* generator,
                         int id,
                         const wxString& path);

    /**
        Retrieve the new absolute path of the file/directory.
    */
    wxString GetPath() const;

    /**
        Set the absolute path of the file/directory.
    */
    void SetPath(const wxString& path);
};
