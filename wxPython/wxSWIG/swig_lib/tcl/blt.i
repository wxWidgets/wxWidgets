// Initialization code for BLT
%{
#ifdef __cplusplus
extern "C" {
#endif
extern int Blt_Init(Tcl_Interp *);
#ifdef __cplusplus
}
#endif
%}

#ifdef AUTODOC
%subsection "blt.i"
%text %{
This module initializes the BLT package.  This is usually done in
combination with the wish.i or similar module.  For example :

      %include wish.i        // Build a new wish executable
      %include blt.i         // Initialize BLT
%}
#endif

%init %{
	if (Blt_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
        }
%}

