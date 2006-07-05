#----------------------------------------------------------------------
# Name:        taskrunner.py
# Purpose:     Classes that can manage running of external processes,
#              either consecutively, simultaneously, or both, and can
#              log the output of those jobs
#
# Author:      Robin Dunn
#
# Created:     05-Nov-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

import sys
import os
import signal
import select
import fcntl
from subprocess import Popen, PIPE, STDOUT


__all__ = ["Job", "Task", "TaskRunner"]

#----------------------------------------------------------------------

# For environment settings
class Config:
    def asDict(self):
        return self.__dict__.copy()

    def write(self, filename="config", outfile=None):
        if outfile is None:
            f = file(filename, "w")
        else:
            f = outfile
        for k, v in self.__dict__.items():
            f.write('%s="%s"\n' % (k, v))
            
    def read(self, filename="config"):
        myfile = open(filename, "r")
        for line in myfile.readlines():
            line = line.strip()
            if len(line) > 0 and line[0] == "#":
                continue # it's a comment, move on
            data = line.split("=")
            if len(data) == 2:
                self.__dict__[data[0].strip()] = data[1].strip()
        myfile.close()

class Job(object):
    """
    Each Job is a monitor wrapped around an externally executing
    process.  It handles starting the process, polling if it is still
    running, reading and logging it's output, and killing it if
    needed.
    """

    LOGBASE="."
    
    def __init__(self, label, command, args=[], env=os.environ):
        self.label = label
        self.command = command
        self.args = args
        self.env = env
        self.proc = None
        if self.label:
            if not os.path.exists(self.LOGBASE):
                os.makedirs(self.LOGBASE)
            self.log = file("%s/%s.log" % (self.LOGBASE, label), "w", 0)

    def start(self):
        self.proc = Popen([self.command] + self.args, # the command and args to execute
                          stdout=PIPE, stderr=STDOUT, env=self.env,
                          bufsize=0 # line-buffered
                          )
        # put the file in non-blocking mode
        #flags = fcntl.fcntl (self.proc.stdout, fcntl.F_GETFL, 0)
        #flags = flags | os.O_NONBLOCK
        #fcntl.fcntl (self.proc.stdout, fcntl.F_SETFL, flags)
        

    def stop(self):
        if self.proc is not None and self.proc.returncode is None:
            os.kill(self.proc.pid, signal.SIGTERM)
            self.logLines()


    def fileno(self):
        if self.proc is not None:
            return self.proc.stdout.fileno()
        else:
            return -1

        
    def logLines(self):
        if self.proc is not None:
            while self.linesAvailable():
                line = self.proc.stdout.readline()
                if not line: break
                if self.label:
                    self.log.write(line)
                    line = "** %s: %s" % (self.label, line)
                sys.stdout.write(line)


    def linesAvailable(self):
        if self.proc is None:
            return False
        ind, outd, err = select.select([self], [], [], 0)
        if ind:
            return True
        else:
            return False


    def finished(self):
        if self.proc is None:# or self.linesAvailable():
            return False
        return self.proc.poll() is not None


    def wait(self):
        if self.proc is None: return None
        return self.proc.wait()


    def poll(self):
        if self.proc is None: return None
        return self.proc.poll()


    def returnCode(self):
        if self.proc is None: return None
        return self.proc.returncode
    

#----------------------------------------------------------------------

class Task(object):
    """
    This class helps manage the running of a Task, which is a simply a
    sequence of one or more Jobs, where subesquent jobs are not
    started until prior ones are completed.
    """
    def __init__(self, jobs=[]):
        if type(jobs) != list:
            jobs = [jobs]
        self.jobs = jobs[:]
        self.active = 0

    def append(self, job):
        self.jobs.append(job)

    def activeJob(self):
        if self.active > len(self.jobs)-1:
            return None
        else:
            return self.jobs[self.active]

    def next(self):
        self.active += 1
        if self.active < len(self.jobs):
            self.jobs[self.active].start()

#----------------------------------------------------------------------

class TaskRunner(object):
    """
    Manages the running of multiple tasks.
    """
    def __init__(self, tasks=[]):
        if type(tasks) != list:
            tasks = [tasks]
        self.tasks = tasks[:]

    def append(self, task):
        self.tasks.append(task)

    def run(self):
        # start all the active jobs
        for task in self.tasks:
            task.activeJob().start()

        try:
            # loop, getting output from the jobs, etc.
            while True:
                # get all active Jobs
                jobs = [t.activeJob() for t in self.tasks if t.activeJob()]
                if not jobs:
                    break

                # wait for a job to have output ready, then log it
                input, output, err = select.select(jobs, [], [], 1)
                for job in input:
                    job.logLines()

                # check for finished jobs
                for task in self.tasks:
                    job = task.activeJob()
                    if job and job.finished():
                        if job.returnCode() != 0:
                            rc = job.returnCode()
                            print "JOB RETURNED FAILURE CODE! (%d)" % rc
                            self.stopAllJobs()
                            return rc
                        else:
                            task.next()
        except KeyboardInterrupt:
            print "STOPPING JOBS..."
            self.stopAllJobs()
            return 1

        except:
            print "Unknown exception..."
            self.stopAllJobs()
            raise

        return 0

    
    def stopAllJobs(self):
        for task in self.tasks:
            job = task.activeJob()
            if job:
                job.stop()

#----------------------------------------------------------------------


if __name__ == "__main__":

    j1 = Job("label1", ["./tmp/job-1.py", "TEST-1"])
    j2 = Job("label2", ["./tmp/job-2.sh", "TEST-2"])

    t1 = Task()
    t1.append(j1)
    t1.append(j2)

    j3 = Job("task2a", ["./tmp/job-1.py", "TASK-2a"])
    j4 = Job("task2b", ["./tmp/job-2.sh", "TASK-2b"])

    t2 = Task()
    t2.append(j4)
    t2.append(j3)

    t3 = Task([Job("error", ["./tmp/job-3.sh", "TASK-3"])])

    tr = TaskRunner()
    tr.append(t1)
    tr.append(t2)
    tr.append(t3)

    for task in tr.tasks:
        for job in task.jobs:
            print job.label

    print tr.run()

