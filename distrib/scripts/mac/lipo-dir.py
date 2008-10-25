import os, sys, shutil

outputdir = "."
ppc_basedir = "."
intel_basedir = "."

def lipo_walker(data, dirname, names):
    global outputdir
    global ppc_basedir
    global intel_basedir
    
    for name in names:
        fullpath = os.path.join(dirname, name)
        intel_fullpath = fullpath.replace(ppc_basedir, intel_basedir)
        outputfile = fullpath.replace(ppc_basedir, outputdir)
        outdir = os.path.dirname(outputfile)
        if not os.path.exists(outdir):
            os.makedirs(outdir)
        
        # this call will only succeed if the file is a binary that can
        # be lipoed.
        if not os.path.islink(fullpath) and os.system("lipo -info %s" % fullpath) == 0:
            if os.system("lipo -output %s -create %s %s" % (outputfile, fullpath, intel_fullpath)) == 0:
                print "Successfully created %s" % outputfile
        else:
            if os.path.islink(fullpath):
                if os.path.exists(outputfile):
                    os.remove(outputfile)
                linkto = os.readlink(fullpath)
                
                if linkto.startswith(ppc_basedir):
                    linkto = linkto.replace(ppc_basedir, outputdir)
                elif linkto.startswith(intel_basedir):
                    linkto = linkto.replace(intel_basedir, outputdir)
                
                os.symlink(linkto, outputfile)
            
            elif not os.path.isdir(fullpath):
                shutil.copy(fullpath, outputfile)
            
        
if __name__ == "__main__":
    if len(sys.argv) < 4:
        print "Usage: %s <ppcdir> <inteldir> <outputdir>"
        print ""
        print "Takes a directory containing a Mac ppc application, and a directory"
        print "containing a Mac intel application, and merges them into a universal"
        print "binary."
        sys.exit(1)
        
    ppc_basedir = sys.argv[1]
    intel_basedir = sys.argv[2]
    outputdir = sys.argv[3]
        
    os.path.walk(ppc_basedir, lipo_walker, None)
    
