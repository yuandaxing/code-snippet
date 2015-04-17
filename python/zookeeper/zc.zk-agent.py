from zc.zk import ZooKeeper, OPEN_ACL_UNSAFE, FailedConnect
import socket
from ip import get_lan_ip
from process_checker import findProcess, CheckRedis
import time
from daemon import Daemon
import sys
import logging


zklist ="localhost:2181"
st = 2000
groupPath = '/redis/dispatcher'
processName = "redis-server"
redis_port = 7316

class RedisAgent(Daemon):
    def run(self):
        while True:
            try :
                zk = ZooKeeper(zklist, st);
                if not zk.exists(groupPath):
                    zk.create_recursive(groupPath, 'redis-cluster', OPEN_ACL_UNSAFE)
                hostName = socket.gethostname()
                agentName, redisName = "agent-"+hostName, "redis-" + hostName
                agentPath, redisPath = groupPath + "/" + agentName, groupPath + "/" + redisName
                zk.create(agentPath, get_lan_ip(), ephemeral = True)
                while True:
                    if CheckRedis(processName, redis_port) :
                        if redisName not in zk.children(groupPath):
                            zk.create(redisPath, get_lan_ip(), ephemeral = True)
                    else:
                         if redisName in zk.children(groupPath):
                             zk.delete(redisPath)
                    time.sleep(1)
            except FailedConnect as f:
                logging.log(logging.ERROR, f)
            except Exception as e:
                logging.log(logging.ERROR, e)
            zk.close()
            time.sleep(1)

if __name__ == "__main__":
    daemon = RedisAgent('/tmp/redis-agent.pid')
    formatStr = '%(asctime)s %(levelname)s %(message)s'
    logging.basicConfig(format = formatStr,
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        filename = './agent.log',
                        level = logging.INFO)

    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        elif 'restart' == sys.argv[1]:
            daemon.restart()
        else:
            print "Unknown command"
            sys.exit(2)
        sys.exit(0)
    else:
        print "usage: %s start|stop|restart" % sys.argv[0]
        sys.exit(2)
