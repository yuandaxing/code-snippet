from daemon import Daemon
from ip import get_lan_ip
from process_checker import CheckRedis
from zc.zk import ZooKeeper, OPEN_ACL_UNSAFE, FailedConnect
import logging
import socket
import sys
import time
import traceback

zklist ="localhost:2181"
st = 2000
groupPath = '/redis/dispatcher'
processName = "redis-server"
pidfile = '/tmp/redis.pid'
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
                ip = get_lan_ip()
                logging.log(logging.INFO, "ip address %s" % ip)
                while True:
                    if agentName not in zk.children(groupPath):
                        zk.create(agentPath, ip, ephemeral = True)
                    found, pid = CheckRedis(pidfile)
                    logging.log(logging.INFO, "redis pid : %s %s" % (pid, "found" if found else "not found"))
                    if found and (redisName not in zk.children(groupPath)):
                        zk.create(redisPath, ip, ephemeral = True)
                    elif not found and (redisName in zk.children(groupPath)):
                        zk.delete(redisPath)
                    time.sleep(5)
            except Exception as e:
                try :
                    zk.close()
                except:
                    pass
                logging.log(logging.ERROR, traceback.format_exc())
            time.sleep(1)


if __name__ == "__main__":
    daemon = RedisAgent('/tmp/redis-agent.pid')
    formatStr = '%(asctime)s %(levelname)s %(filename)s [line:%(lineno)d] %(message)s'
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
