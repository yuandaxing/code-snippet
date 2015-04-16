import subprocess
import time

def findProcess(processName, port):
    ps= subprocess.Popen("ps -ef | grep " + processName + "| awk '{ print $NF }' | grep " + str(port),
                         shell=True, stdout=subprocess.PIPE)
    output = ps.stdout.read()
    ps.stdout.close()
    ps.wait()
    return output

def CheckRedis(processName, redis_port):
    for idx in xrange(3):
        try:
            return findProcess(processName, redis_port) == ''
        except  Exception as e:
            print 'check iteration %d, exceptions: %s' % (idx, e)
            time.sleep(1)
    return False

def isProcessRunning( processId):
    output = findProcess( processId )
    if re.search(processId, output) is None:
        return true
    else:
        return False
