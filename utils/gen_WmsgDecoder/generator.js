
const fs = require('fs');
const path = require('path');

const debug = false;

// Read wm_message_ids.h and generate the core of
// a switch/case to translate those to legible names.
// The result is written to the OUTFILE.

let input = fs.readFileSync("../../src/msw/wm_message_ids.h", "utf8");

let define_re = /^#define +([\w\d]+) ([()+\w\d\s]+) *(?:\/\/.*)?/gm;

let defs = {};
let to_calc ={};

{
	let m;
	while ((m = define_re.exec(input))) {
		if (debug) console.log(m[1], m[2]);
		let v = m[2].trim();
		defs[m[1]] = v;

		let i = Number(v);
		if (!isNaN(i)) {
			defs[m[1]] = i;
		} else {
			to_calc[m[1]] = v;
		}
	}
}

if (debug) console.log({defs, to_calc})

for (let r = 5; r > 0; r--) {
	let cnt = 0;
	for (let k in to_calc) {
		cnt++;

		let v = to_calc[k];
		if (debug) console.log({k, v, cnt});

		v = v.replace(/[()]/g, '');

		let m = /^([\w\d]+) *\+ *([\w\d]+)$/.exec(v);
		if (m) {
			let v2 = Number(m[2]);
			let v1 = Number(defs[m[1]]);
			if (debug) console.log("ADDITION:", m, {v1, v2});
			if (isNaN(v1))
				throw new Error(`constant ${m[1]} is not #define'd`);
			v = v1 + v2;
			defs[k] = v;
			delete to_calc[k];
			continue;
		}

		m = /^([\w\d]+)$/.exec(v);
		if (m) {
			let v1 = Number(defs[m[1]]);
			if (debug) console.log("ALIAS:", m, {v1});
			if (isNaN(v1))
				throw new Error(`constant ${m[1]} is not #define'd`);
			v = v1;
			defs[k] = v;
			delete to_calc[k];
			continue;
		}
	}
	if (cnt == 0) 
		break;
}

if (debug) console.log({defs, to_calc})

// now gather the IDs per numeric value

let index = {};

for (let k in defs) {
	if (!index[defs[k]])
		index[defs[k]] = [k];
	else 
		index[defs[k]].push(k);
}

if (debug) console.log({defs, index})

// generate source code:

let outSrc = [`
// This headerfile has been generated using the gen_WmsgDecoder/generator script,
// using the src/msw/wm_message_ids.h headerfile as source (which itself is derived
// off Windows' CommCtrl.h and WinUser.h to get all known MSWindows Message IDs).
//
// DO NOT manually edit this file, but adjust the mentioned source headerfile
// and/or script when you want this stuff changed.

`];

function reduce(v) {
	// make resulting output shorter when all IDs have a common suffix:
	let suffix;
	for (let k in v) {
		let el = v[k];
		let m = /^([A-Z]+)_([A-Z]+$)/i.exec(el);
		if (m) {
			if (!suffix) {
				if (debug) console.log("DETECTED SUFFIX:", {m});
				suffix = m[2];
			}
			else if (suffix !== m[2] ) {
				if (debug) console.log("REJECTED SUFFIX:", {suffix, m});
				suffix = null;
				break;
			}
		}
		else {
			if (debug) console.log("NO SUFFIX:", {suffix, el});
			suffix = null;
			break;
		}
	}

	if (suffix) {
		let prefix = [];
		for (let k in v) {
			let m = /^([A-Z]+)_([A-Z]+)$/i.exec(v[k]);
			prefix.push(m[1]);
		}
		return `[${ prefix.join('/') }]_${suffix}`;
	}

	return v.join('/');
}

for (let k in index) {
	let v = index[k];

	if (v.length === 1) {
		outSrc.push(`    case ${k}: return wxT("${v[0]}");`);
	}
	else {
		outSrc.push(`    case ${k}: return wxT("${ reduce(v) }");`);
	}
}

if (debug) console.log(outSrc.join('\n'))


fs.writeFileSync("../../src/msw/wm_message_id_decoder.inc.h", outSrc.join('\n'), "utf8");

console.log("Done. Generated file 'wm_message_id_decoder.inc.h'.");


