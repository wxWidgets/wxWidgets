/////////////////////////////////////////////////////////////////////////////
// Name:        caret.h
// Purpose:     documentation for wxCaret class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCaret
    @wxheader{caret.h}

    A caret is a blinking cursor showing the position where the typed text will
    appear. The text controls usually have a caret but wxCaret class also allows
    to use a caret in other windows.

    Currently, the caret appears as a rectangle of the given size. In the future,
    it will be possible to specify a bitmap to be used for the caret shape.

    A caret is always associated with a window and the current caret can be
    retrieved using wxWindow::GetCaret. The same caret
    can't be reused in two different windows.

    @library{wxcore}
    @category{misc}

    @seealso
    wxCaret::GetBlinkTime
*/
class wxCaret
{
public:
    //@{
    /**
        Create the caret of given (in pixels) width and height and associates it
        with the given window.
    */
    wxCaret();
    wxCaret(wxWindow* window, int width, int height);
    wxCaret(wxWindowBase* window, const wxSize& size);
    //@}

    //@{
    /**
        Create the caret of given (in pixels) width and height and associates it
        with the given window (same as constructor).
    */
    bool Create(wxWindowBase* window, int width, int height);
    bool Create(wxWindowBase* window, const wxSize& size);
    //@}

    /**
        Returns the blink time which is measured in milliseconds and is the time elapsed
        between 2 inversions of the caret (blink time of the caret is the same
        for all carets, so this functions is static).
    */
    static int GetBlinkTime();

    //@{
    /**
        Get the caret position (in pixels).
        
        @b GetPosition()
        
        Returns a Wx::Point
        
        @b GetPositionXY()
        
        Returns a 2-element list
         @c ( x, y )
    */
    void GetPosition(int* x, int* y);
    wxPoint GetPosition();
    //@}

    //@{
    /**
        Get the caret size.
        
        @b GetSize()
        
        Returns a Wx::Size
        
        @b GetSizeWH()
        
        Returns a 2-element list
         @c ( width, height )
    */
    void GetSize(int* width, int* height);
    wxSize GetSize();
    //@}

    /**
        Get the window the caret is associated with.
    */
    wxWindow* GetWindow();

    /**
        Same as wxCaret::Show(@false).
    */
    void Hide();

    /**
        Returns @true if the caret was created successfully.
    */
    bool IsOk();

    /**
        Returns @true if the caret is visible and @false if it is permanently
        hidden (if it is is blinking and not shown currently but will be after the
        next blink, this method still returns @true).
    */
    bool IsVisible();

    //@{
    /**
        Move the caret to given position (in logical coordinates).
    */
    void Move(int x, int y);
    void Move(const wxPoint& pt);
    //@}

    /**
        Sets the blink time for all the carets.
        
        @remarks Under Windows, this function will change the blink time for all
                 carets permanently (until the next time it is called),
                 even for the carets in other applications.
        
        @see GetBlinkTime()
    */
    static void SetBlinkTime(int milliseconds);

    //@{
    /**
        Changes the size of the caret.
    */
    void SetSize(int width, int height);
    void SetSize(const wxSize& size);
    //@}

    /**
        Shows or hides the caret. Notice that if the caret was hidden N times, it
        must be shown N times as well to reappear on the screen.
    */
    void Show(bool show = true);
};
