import subprocess
import time
import logging


def findProcess(pidfile):
    '''
    find process according to the pidfile, the pid file must have the pid
    check the alive with ps -p pid
    throw filenot exist or process exceptions
    return, for example, (True, pid)
    '''
    with open(pidfile, "r") as f:
        pid = f.read().strip()
        if not pid : return (False, '')
        ps= subprocess.Popen("ps -p " + pid,
                         shell=True, stdout=subprocess.PIPE)
        return (ps.wait() == 0, pid)

def CheckRedis(pidfile):
    for idx in xrange(3):
        try:
            return findProcess(pidfile)
        except  Exception as e:
            logging.log(logging.ERROR, 'check iteration %d, exceptions: %s' % (idx, e))
            time.sleep(1)
    return (False, '')
