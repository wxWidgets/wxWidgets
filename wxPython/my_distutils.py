
import sys, os, string

from distutils.msvccompiler import MSVCCompiler
from distutils.bcppcompiler import BCPPCompiler

from distutils.errors import \
     DistutilsExecError, DistutilsPlatformError, \
     CompileError, LibError, LinkError
from distutils.ccompiler import \
     CCompiler, gen_preprocess_options, gen_lib_options

#----------------------------------------------------------------------

class MyMSVCCompiler(MSVCCompiler):

    ##------------------------------------------------------------
    ## Override the entire compile method just to add flags to the
    ## RC command.  There should be an easier way to do this from
    ## distutils directly or in a derived class...
    ##------------------------------------------------------------

    def compile (self,
                 sources,
                 output_dir=None,
                 macros=None,
                 include_dirs=None,
                 debug=0,
                 extra_preargs=None,
                 extra_postargs=None):

        (output_dir, macros, include_dirs) = \
            self._fix_compile_args (output_dir, macros, include_dirs)
        (objects, skip_sources) = self._prep_compile (sources, output_dir)

        if extra_postargs is None:
            extra_postargs = []

        pp_opts = gen_preprocess_options (macros, include_dirs)
        compile_opts = extra_preargs or []
        compile_opts.append ('/c')
        if debug:
            compile_opts.extend (self.compile_options_debug)
        else:
            compile_opts.extend (self.compile_options)

        for i in range (len (sources)):
            src = sources[i] ; obj = objects[i]
            ext = (os.path.splitext (src))[1]

            if skip_sources[src]:
                self.announce ("skipping %s (%s up-to-date)" % (src, obj))
            else:
                self.mkpath (os.path.dirname (obj))

                if ext in self._c_extensions:
                    input_opt = "/Tc" + os.path.abspath(src)
                elif ext in self._cpp_extensions:
                    input_opt = "/Tp" + os.path.abspath(src)
                elif ext in self._rc_extensions:
                    # compile .RC to .RES file
                    input_opt = src
                    output_opt = "/fo" + obj
                    try:
                        self.spawn ([self.rc] + pp_opts +         ### RPD changed this line only
                                    [output_opt] + [input_opt])
                    except DistutilsExecError, msg:
                        raise CompileError, msg
                    continue
                elif ext in self._mc_extensions:

                    # Compile .MC to .RC file to .RES file.
                    #   * '-h dir' specifies the directory for the
                    #     generated include file
                    #   * '-r dir' specifies the target directory of the
                    #     generated RC file and the binary message resource
                    #     it includes
                    #
                    # For now (since there are no options to change this),
                    # we use the source-directory for the include file and
                    # the build directory for the RC file and message
                    # resources. This works at least for win32all.

                    h_dir = os.path.dirname (src)
                    rc_dir = os.path.dirname (obj)
                    try:
                        # first compile .MC to .RC and .H file
                        self.spawn ([self.mc] +
                                    ['-h', h_dir, '-r', rc_dir] + [src])
                        base, _ = os.path.splitext (os.path.basename (src))
                        rc_file = os.path.join (rc_dir, base + '.rc')
                        # then compile .RC to .RES file
                        self.spawn ([self.rc] +
                                    ["/fo" + obj] + [rc_file])

                    except DistutilsExecError, msg:
                        raise CompileError, msg
                    continue
                else:
                    # how to handle this file?
                    raise CompileError (
                        "Don't know how to compile %s to %s" % \
                        (src, obj))

                output_opt = "/Fo" + obj
                try:
                    self.spawn ([self.cc] + compile_opts + pp_opts +
                                [input_opt, output_opt] +
                                extra_postargs)
                except DistutilsExecError, msg:
                    raise CompileError, msg

        return objects

    # compile ()



from distutils.file_util import write_file
class MyBCPPCompiler(BCPPCompiler):

    ##------------------------------------------------------------
    ## Override the entire compile method just to add flags to the
    ## RC command.  There should be an easier way to do this from
    ## distutils directly or in a derived class...
    ##------------------------------------------------------------

    def compile (self,
                 sources,
                 output_dir=None,
                 macros=None,
                 include_dirs=None,
                 debug=0,
                 extra_preargs=None,
                 extra_postargs=None):

        (output_dir, macros, include_dirs) = \
            self._fix_compile_args (output_dir, macros, include_dirs)
        (objects, skip_sources) = self._prep_compile (sources, output_dir)

        if extra_postargs is None:
            extra_postargs = []

        pp_opts = gen_preprocess_options (macros, include_dirs)
        compile_opts = extra_preargs or []
        compile_opts.append ('-c')
        if debug:
            compile_opts.extend (self.compile_options_debug)
        else:
            compile_opts.extend (self.compile_options)

        for i in range (len (sources)):
            src = sources[i] ; obj = objects[i]
            ext = (os.path.splitext (src))[1]

            if skip_sources[src]:
                self.announce ("skipping %s (%s up-to-date)" % (src, obj))
            else:
                src = os.path.normpath(src)
                obj = os.path.normpath(obj)
                self.mkpath(os.path.dirname(obj))

                if ext == '.res':
                    # This is already a binary file -- skip it.
                    continue # the 'for' loop
                if ext == '.rc':
                    # This needs to be compiled to a .res file -- do it now.
                    try:
                        self.spawn (["brcc32"] + pp_opts + ["-fo"] +
				[obj] + [src]) ### RPD changed this lines only
                    except DistutilsExecError, msg:
                        raise CompileError, msg
                    continue # the 'for' loop

                # The next two are both for the real compiler.
                if ext in self._c_extensions:
                    input_opt = ""
                elif ext in self._cpp_extensions:
                    input_opt = "-P"
                else:
                    # Unknown file type -- no extra options.  The compiler
                    # will probably fail, but let it just in case this is a
                    # file the compiler recognizes even if we don't.
                    input_opt = ""

                output_opt = "-o" + obj

                # Compiler command line syntax is: "bcc32 [options] file(s)".
                # Note that the source file names must appear at the end of
                # the command line.
                try:
                    self.spawn ([self.cc] + compile_opts + pp_opts +
                                [input_opt, output_opt] +
                                extra_postargs + [src])
                except DistutilsExecError, msg:
                    raise CompileError, msg

        return objects

    # compile ()

    ####################################################################
    # Now we need to replace cw32mt library used by default by distutils
    # with cw32mti library as in wxWindows DLL make file
    # Othervise we obtain Windows "Core dump" ;-).
    #
    #		Evgeny A Cherkashin <eugeneai@icc.ru>
    #
    ####################################################################

    def link (self,
              target_desc,
              objects,
              output_filename,
              output_dir=None,
              libraries=None,
              library_dirs=None,
              runtime_library_dirs=None,
              export_symbols=None,
              debug=0,
              extra_preargs=None,
              extra_postargs=None,
              build_temp=None):

        # XXX this ignores 'build_temp'!  should follow the lead of
        # msvccompiler.py

        (objects, output_dir) = self._fix_object_args (objects, output_dir)
        (libraries, library_dirs, runtime_library_dirs) = \
            self._fix_lib_args (libraries, library_dirs, runtime_library_dirs)

        if runtime_library_dirs:
            self.warn ("I don't know what to do with 'runtime_library_dirs': "
                       + str (runtime_library_dirs))

        if output_dir is not None:
            output_filename = os.path.join (output_dir, output_filename)

        if self._need_link (objects, output_filename):

            # Figure out linker args based on type of target.
            if target_desc == CCompiler.EXECUTABLE:
                startup_obj = 'c0w32'
                if debug:
                    ld_args = self.ldflags_exe_debug[:]
                else:
                    ld_args = self.ldflags_exe[:]
            else:
                startup_obj = 'c0d32'
                if debug:
                    ld_args = self.ldflags_shared_debug[:]
                else:
                    ld_args = self.ldflags_shared[:]


            # Create a temporary exports file for use by the linker
            if export_symbols is None:
                def_file = ''
            else:
                head, tail = os.path.split (output_filename)
                modname, ext = os.path.splitext (tail)
                temp_dir = os.path.dirname(objects[0]) # preserve tree structure
                def_file = os.path.join (temp_dir, '%s.def' % modname)
                contents = ['EXPORTS']
                for sym in (export_symbols or []):
                    contents.append('  %s=_%s' % (sym, sym))
                self.execute(write_file, (def_file, contents),
                             "writing %s" % def_file)

            # Borland C++ has problems with '/' in paths
            objects2 = map(os.path.normpath, objects)
            # split objects in .obj and .res files
            # Borland C++ needs them at different positions in the command line
            objects = [startup_obj]
            resources = []
            for file in objects2:
                (base, ext) = os.path.splitext(os.path.normcase(file))
                if ext == '.res':
                    resources.append(file)
                else:
                    objects.append(file)


            for l in library_dirs:
                ld_args.append("/L%s" % os.path.normpath(l))
            ld_args.append("/L.") # we sometimes use relative paths

            # list of object files
            ld_args.extend(objects)

            # XXX the command-line syntax for Borland C++ is a bit wonky;
            # certain filenames are jammed together in one big string, but
            # comma-delimited.  This doesn't mesh too well with the
            # Unix-centric attitude (with a DOS/Windows quoting hack) of
            # 'spawn()', so constructing the argument list is a bit
            # awkward.  Note that doing the obvious thing and jamming all
            # the filenames and commas into one argument would be wrong,
            # because 'spawn()' would quote any filenames with spaces in
            # them.  Arghghh!.  Apparently it works fine as coded...

            # name of dll/exe file
            ld_args.extend([',',output_filename])
            # no map file and start libraries
            ld_args.append(',,')

            for lib in libraries:
                # see if we find it and if there is a bcpp specific lib
                # (xxx_bcpp.lib)
                libfile = self.find_library_file(library_dirs, lib, debug)
                if libfile is None:
                    ld_args.append(lib)
                    # probably a BCPP internal library -- don't warn
                    #    self.warn('library %s not found.' % lib)
                else:
                    # full name which prefers bcpp_xxx.lib over xxx.lib
                    ld_args.append(libfile)

            # some default libraries
            ld_args.append ('import32')
            ld_args.append ('cw32mti')	### mt->mti (as in wx2)

            # def file for export symbols
            ld_args.extend([',',def_file])
            # add resource files
            ld_args.append(',')
            ld_args.extend(resources)


            if extra_preargs:
                ld_args[:0] = extra_preargs
            if extra_postargs:
                ld_args.extend(extra_postargs)

            self.mkpath (os.path.dirname (output_filename))
            try:
                self.spawn ([self.linker] + ld_args)
            except DistutilsExecError, msg:
                raise LinkError, msg

        else:
            self.announce ("skipping %s (up-to-date)" % output_filename)

    # link ()



#----------------------------------------------------------------------
# Hack this module and class into the distutils...

from distutils import ccompiler

ccompiler.default_compiler['nt'] = 'my_msvc'
ccompiler.compiler_class['my_msvc'] = ('my_distutils',
                                       'MyMSVCCompiler',
                                       'My MSVC derived class')


ccompiler.compiler_class['my_bcpp'] = ('my_distutils',
                                       'MyBCPPCompiler',
                                       'My BCPP derived class')

# make it look like it is part of the package...
import my_distutils
sys.modules['distutils.my_distutils'] = my_distutils


#----------------------------------------------------------------------
# Run SWIG the way I want it done

def run_swig(files, dir, gendir, package, USE_SWIG, force, swig_args, swig_deps=[]):
    from distutils.file_util import copy_file
    from distutils.dep_util import newer
    from distutils.spawn import spawn

    sources = []

    for file in files:
        basefile = os.path.splitext(file)[0]
        i_file   = os.path.join(dir, file)
        py_file  = os.path.join(dir, gendir, basefile+'.py')
        cpp_file = os.path.join(dir, gendir, basefile+'.cpp')

        sources.append(cpp_file)

        if USE_SWIG:
            for dep in swig_deps:
                if newer(dep, py_file) or newer(dep, cpp_file):
                    force = 1
                    break

            if force or newer(i_file, py_file) or newer(i_file, cpp_file):
                # we need forward slashes here even on win32
                cpp_file = string.join(string.split(cpp_file, '\\'), '/')
                i_file = string.join(string.split(i_file, '\\'), '/')

                cmd = ['swig'] + swig_args + ['-I'+dir, '-c', '-o', cpp_file, i_file]
                spawn(cmd, verbose=1)

        # copy the generated python file to the package directory
        copy_file(py_file, package, update=not force, verbose=0)


    return sources



#----------------------------------------------------------------------
# Update local copies of wxWindows contrib files


def contrib_copy_tree(src, dest, verbose=0):
    from distutils.dir_util import mkpath, copy_tree

    mkpath(dest, verbose=verbose)
    copy_tree(src, dest, update=1, verbose=verbose)

