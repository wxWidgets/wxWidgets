\wxheading{Event table macros}

To process a scroll event, use these event handler macros to direct input to
member functions that take a wxScrollEvent argument. You can use
{\tt EVT\_COMMAND\_SCROLL...} macros with window IDs for when intercepting
scroll events from controls, or {\tt EVT\_SCROLL...} macros without window IDs
for intercepting scroll events from the receiving window -- except for this,
the macros behave exactly the same.

\twocolwidtha{9cm}
\begin{twocollist}\itemsep=0pt
\twocolitem{{\bf EVT\_SCROLL(func)}}{Process all scroll events.}
\twocolitem{{\bf EVT\_SCROLL\_TOP(func)}}{Process wxEVT\_SCROLL\_TOP scroll-to-top events (minimum position).}
\twocolitem{{\bf EVT\_SCROLL\_BOTTOM(func)}}{Process wxEVT\_SCROLL\_BOTTOM scroll-to-bottom events (maximum position).}
\twocolitem{{\bf EVT\_SCROLL\_LINEUP(func)}}{Process wxEVT\_SCROLL\_LINEUP line up events.}
\twocolitem{{\bf EVT\_SCROLL\_LINEDOWN(func)}}{Process wxEVT\_SCROLL\_LINEDOWN line down events.}
\twocolitem{{\bf EVT\_SCROLL\_PAGEUP(func)}}{Process wxEVT\_SCROLL\_PAGEUP page up events.}
\twocolitem{{\bf EVT\_SCROLL\_PAGEDOWN(func)}}{Process wxEVT\_SCROLL\_PAGEDOWN page down events.}
\twocolitem{{\bf EVT\_SCROLL\_THUMBTRACK(func)}}{Process wxEVT\_SCROLL\_THUMBTRACK thumbtrack events (frequent events
sent as the user drags the thumbtrack).}
\twocolitem{{\bf EVT\_SCROLL\_THUMBRELEASE(func)}}{Process wxEVT\_SCROLL\_THUMBRELEASE thumb release events.}
\twocolitem{{\bf EVT\_SCROLL\_CHANGED(func)}}{Process wxEVT\_SCROLL\_CHANGED end of scrolling events (MSW only).}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL(id, func)}}{Process all scroll events.}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_TOP(id, func)}}{Process wxEVT\_SCROLL\_TOP scroll-to-top events (minimum position).}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_BOTTOM(id, func)}}{Process wxEVT\_SCROLL\_BOTTOM scroll-to-bottom events (maximum position).}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_LINEUP(id, func)}}{Process wxEVT\_SCROLL\_LINEUP line up events.}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_LINEDOWN(id, func)}}{Process wxEVT\_SCROLL\_LINEDOWN line down events.}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_PAGEUP(id, func)}}{Process wxEVT\_SCROLL\_PAGEUP page up events.}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_PAGEDOWN(id, func)}}{Process wxEVT\_SCROLL\_PAGEDOWN page down events.}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_THUMBTRACK(id, func)}}{Process wxEVT\_SCROLL\_THUMBTRACK thumbtrack events (frequent events
sent as the user drags the thumbtrack).}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_THUMBRELEASE(func)}}{Process wxEVT\_SCROLL\_THUMBRELEASE thumb release events.}
\twocolitem{{\bf EVT\_COMMAND\_SCROLL\_CHANGED(func)}}{Process wxEVT\_SCROLL\_CHANGED end of scrolling events (MSW only).}
\end{twocollist}%



\wxheading{The difference between {\tt EVT\_SCROLL\_THUMBRELEASE} and {\tt EVT\_SCROLL\_CHANGED}}

The {\tt EVT\_SCROLL\_THUMBRELEASE} event is only emitted when actually dragging
the thumb using the mouse and releasing it (This {\tt EVT\_SCROLL\_THUMBRELEASE}
event is also followed by an {\tt EVT\_SCROLL\_CHANGED} event).

The {\tt EVT\_SCROLL\_CHANGED} event also occurs when using the keyboard to
change the thumb position, and when clicking next to the thumb (In all these
cases the {\tt EVT\_SCROLL\_THUMBRELEASE} event does not happen).

In short, the {\tt EVT\_SCROLL\_CHANGED} event is triggered when scrolling/
moving has finished independently of the way it had started. Please see the
widgets sample ("Slider" page) to see the difference between {\tt
EVT\_SCROLL\_THUMBRELEASE} and {\tt EVT\_SCROLL\_CHANGED} in action.

