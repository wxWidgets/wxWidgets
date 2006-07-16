import os, glob, shutil

wx_dir = os.environ["WXWIN"]
locale_dir = os.path.join(wx_dir, "locale")
print "WX Locale dir is: " + locale_dir
for mo in glob.glob(os.path.join(locale_dir, "*.mo")):
    mofile = os.path.basename(mo)
    moname = os.path.splitext(mofile)[0]
    modir = os.path.join(locale_dir, moname, "LC_MESSAGES")
    print "Making: " + modir
    if not os.path.exists(modir):
        os.makedirs(modir)
    shutil.copyfile(os.path.join(locale_dir, mofile), os.path.join(modir, "wxstd.mo"))
    