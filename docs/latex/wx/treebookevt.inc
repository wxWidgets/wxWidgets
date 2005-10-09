\wxheading{Event handling}

To process input from a treebook control, use the following event handler macros
to direct input to member functions that take a \helpref{wxTreebookEvent}{wxtreebookevent} argument.

\twocolwidtha{10cm}
\begin{twocollist}\itemsep=0pt
\twocolitem{{\bf EVT\_TREEBOOK\_PAGE\_CHANGED(id, func)}}{The page selection was changed. Processes a wxEVT\_COMMAND\_TREEBOOK\_PAGE\_CHANGED event.}
\twocolitem{{\bf EVT\_TREEBOOK\_PAGE\_CHANGING(id, func)}}{The page selection is about to be changed. Processes a wxEVT\_COMMAND\_TREEBOOK\_PAGE\_CHANGING event. This event can be \helpref{vetoed}{wxnotifyeventveto}.}
\twocolitem{{\bf EVT\_TREEBOOK\_NODE\_COLLAPSED(id, func)}}{The page node is going to be collapsed. Processes a wxEVT\_COMMAND\_TREEBOOK\_NODE\_COLLAPSED event.}
\twocolitem{{\bf EVT\_TREEBOOK\_NODE\_EXPANDED(id, func)}}{The page node is going to be expanded. Processes a wxEVT\_COMMAND\_TREEBOOK\_NODE\_EXPANDED event.}
\end{twocollist}
