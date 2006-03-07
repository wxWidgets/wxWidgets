import sys, os, string, time, shutil
#import ReleaseForge

from taskrunner import Job, Task, TaskRunner, Config

# read in the build settings...

CFGFILE = "./scripts/build-environ.cfg"
config = Config()
config.read(CFGFILE)

config.WXWIN = os.path.abspath("..")

class Job(Job):
    LOGBASE = "./tmp"

# ensure the staging area exists
if not os.path.exists(config.STAGING_DIR):
    os.makedirs(config.STAGING_DIR)

# Figure out the wxPython version number, possibly adjusted for being a daily build
if config.KIND == "daily":
    t = time.localtime()
    config.DAILY = time.strftime("%Y%m%d")   # should it include the hour too?  2-digit year?
    file("DAILY_BUILD", "w").write(config.DAILY)
    # stamp the date on daily builds
    config.BUILD_VERSION=config.BUILD_VERSION + "-" + config.DAILY

# Let the user override build machine names, etc., etc. with their own
# config file settings
myconfig = None
myconfig_file = os.path.expanduser("~/wxrelease-environ.cfg")
if os.path.exists(myconfig_file):
    myconfig = Config()
    myconfig.read(myconfig_file)

# TODO: Set up different environs for daily, release, etc.
# so that people can have different configs for different builds

# prepare the environment file
config_env = config.asDict()
config_env.update(os.environ)
if myconfig:
    config_env.update(myconfig.asDict())
    
tasks = Task([ Job("pre-flight", "./scripts/pre-flight.sh", env=config_env), 
               Job("win_build", "./scripts/build-windows.sh", env=config_env),
               Job("lin_build", "./scripts/build-linux.sh", env=config_env),
               Job("mac_build", "./scripts/build-mac.sh", env=config_env),
             ])
             
print "Build getting started at: ", time.ctime()


# Run the first task, which will create the docs and sources tarballs
tr = TaskRunner(tasks)
rc = tr.run()

if rc == 0 and config.delete_temps == "yes":
    shutil.rmtree(config.WX_TEMP_DIR)

# cleanup the DAILY_BUILD file
if config.KIND == "daily":
    os.unlink("DAILY_BUILD")
    
print "Build finished at: ", time.ctime()
sys.exit(0)
