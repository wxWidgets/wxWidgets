// Initialization code for Tix

%{
#ifdef __cplusplus
extern "C" {
#endif
extern int Tix_Init(Tcl_Interp *);
#ifdef __cplusplus
}
#endif
%}

#ifdef AUTODOC
%subsection "tix.i"
%text %{
This module initializes the Tix extension.  This is usually done in
combination with the wish.i or similar module.  For example :

      %include wish.i        // Build a new wish executable
      %include tix.i         // Initialize Tix
%}
#endif

%init %{
	if (Tix_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
        }
%}

