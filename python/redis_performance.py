import redis
import random
import timeit

def GenStr(L):
    cand = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890'
    LEN = len(cand)
    return ''.join(cand[random.randint(0, LEN-1)] for i in xrange(L))

def GenKV(num, klen, vlen):
    ret = {}
    for i in xrange(num):
        ret[GenStr(klen)] = GenStr(vlen)
    return ret
def PipeSet(client, dic):
    byteCounts, byteLimits = 0, 1000000
    with client.pipeline() as pipe:
        for k, v in dic.iteritems():
            pipe.set(k, v)
            byteCounts += len(k) + len(v)
            if byteCounts > byteLimits:
                pipe.execute()
                byteCounts = 0
        pipe.execute()
        byteCounts = 0

def test(num, klen, vlen, host, port):
    dic, failed = GenKV(num, klen, vlen), 0
    client = redis.Redis(host, port)
    print '------------------------------'
    print '%d set tests in total' % (len(dic))
    start = timeit.default_timer()
    # for k, v in dic.iteritems():
    #     if client.set(k, v) == False:
    #         failed += 1
    PipeSet(client, dic)
    end = timeit.default_timer()
    print 'set time cost ' + str(end - start)
    print "failed %d tests" % (failed)
    print '-----------------------------'

    failed = 0
    print 'get %d tests ' % len(dic)
    start = timeit.default_timer()
    for k, v in dic.iteritems():
        val = dic.get(k)
        if val != v:
            failed += 1
    end = timeit.default_timer()
    print 'get time cost %f' % (end - start)
    print 'failed %d cases' % failed
    print '-----------------------------'


test(10000, 20, 50, "10.0.2.140", 2005)
