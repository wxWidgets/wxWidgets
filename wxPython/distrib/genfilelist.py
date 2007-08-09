"""
Walk a directory tree and output a filename list suitable for use
in an RPM spec.

Usage:  genfilelist.py [-r] build_root filespec(s)

"""


import sys, os, glob, stat


def walktree(names, buildroot, recurse):
    for name in names:
        isdir = os.path.isdir(name)
        printfilename(name, buildroot, isdir)
        if isdir and recurse:
            walktree([os.path.join(name, x) for x in os.listdir(name)], buildroot, recurse)


def printfilename(name, buildroot, isdir):
    s = os.lstat(name)
    realname = name[len(buildroot):]
    if isdir:
        fmt = "%%dir %%attr(%o, root, root) %s"
    else:
        fmt = "%%attr(%o, root, root) %s"
    print fmt % (s[stat.ST_MODE] & 0777, realname)


def main(args):
    if args[0] == '-r':
        recurse = 1
        args = args[1:]
    else:
        recurse = 0

    if len(args) < 2:
        print __str__
        sys.exit(1)

    buildroot = args[0]
    for arg in args[1:]:
        if arg[0] == '/':
            arg = arg[1:]
        walktree(glob.glob(os.path.join(buildroot, arg)), buildroot, recurse)

if __name__ == "__main__":
    main(sys.argv[1:])


