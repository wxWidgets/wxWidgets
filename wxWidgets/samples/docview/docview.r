#ifdef __DARWIN__
#  include <Carbon.r>
#else
#  include <Types.r>
#endif

resource 'BNDL' (128) {
	'WXMA',
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'ICN#',
		{	/* array IDArray: 3 elements */
			/* [1] */
			0, 128,
			/* [2] */
			1, 129,
			/* [3] */
			2, 130,
		},
		/* [2] */
		'FREF',
		{	/* array IDArray: 3 elements */
			/* [1] */
			0, 128,
			/* [2] */
			1, 129,
			/* [3] */
			2, 130,
		}
	}
};

resource 'FREF' (128) {
	'APPL',
	0,
	""
};

resource 'FREF' (129) {
	'TEXT',
	1,
	""
};

resource 'FREF' (130) {
	'WXMB',
	2,
	""
};
