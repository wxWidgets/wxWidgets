#ifdef __UNIX__
  #include <Carbon.r>
#else
  #include <Types.r>
  #if UNIVERSAL_INTERFACES_VERSION > 0x320
    #include <ControlDefinitions.r>
  #endif
#endif

resource 'ldes' ( 128 )
{
	versionZero 
	{
		0 , 
		0 ,
		0 , 
		0 , 
		hasVertScroll , 
		noHorizScroll , 
		0 , 
		noGrowSpace , 
	}
} ;

resource 'CURS' (9, "Hand") {
	$"0380 04C0 04C0 04C0 04C0 04C0 74F8 9CAE"
	$"4CAB 240B 2403 1003 0803 0806 0406 0406",
	$"0380 07C0 07C0 07C0 07C0 07C0 77F8 FFFE"
	$"7FFF 3FFF 3FFF 1FFF 0FFF 0FFE 07FE 07FE",
	{0, 7}
};

resource 'CURS' (10, "SizeWE") {
	$"0000 0180 0180 0180 0180 1188 318C 7FFE"
	$"318C 1188 0180 0180 0180 0180",
	$"03C0 03C0 03C0 03C0 1BD8 3BDC 7BFE FFFF"
	$"7FFE 3BDC 1BD8 03C0 03C0 03C0 03C0",
	{7, 7}
};

resource 'CURS' (11, "Size NS") {
	$"0000 0080 01C0 03E0 0080 0080 0080 3FFE"
	$"3FFE 0080 0080 0080 03E0 01C0 0080",
	$"0080 01C0 03E0 07F0 07F0 0180 7FFF 7FFF"
	$"7FFF 7FFF 01C0 07F0 07F0 03E0 01C0 0080",
	{7, 7}
};

