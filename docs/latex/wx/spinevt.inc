%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Name:        spinevt.inc
%% Purpose:     wxSpinEvent documentation shared between spinevt.tex and
%%              spinbutt.tex
%% Author:      Vadim Zeitlin
%% Modified by:
%% Created:     26.05.02 (extracted from spinbutt.tex)
%% RCS-ID:      $Id$
%% Copyright:   (c) wxWindows team
%% License:     wxWindows license
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\wxheading{Event handling}

To process input from a spin button, use one of these event handler macros to
direct input to member functions that take a 
\helpref{wxSpinEvent}{wxspinevent} argument:

\twocolwidtha{7cm}
\begin{twocollist}
\twocolitem{{\bf EVT\_SPIN(id, func)}}{Generated whenever an arrow is pressed.}
\twocolitem{{\bf EVT\_SPIN\_UP(id, func)}}{Generated when left/up arrow is pressed.}
\twocolitem{{\bf EVT\_SPIN\_DOWN(id, func)}}{Generated when right/down arrow is pressed.}
\end{twocollist}%

Note that if you handle both SPIN and UP or DOWN events, you will be notified
about each of them twice: first the UP/DOWN event will be receieved and then,
if it wasn't vetoed, the SPIN event will be sent.
