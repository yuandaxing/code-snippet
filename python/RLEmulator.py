#!/usr/bin/python
#usage ./RLEmulator.py nroot nleaf nconn percentage=20
import random
import sys

if len(sys.argv) < 4 :
    print 'usage: ./RLEmulator.py nRoot nLeaf nConnPerLeaf percentage(default=20), iteration(default=1)'
    exit(-1)

nR, nL, nC, per, it = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]), \
                  0.2 if len(sys.argv) < 5 else int(sys.argv[4]) / 100.0,  \
                  1 if len(sys.argv) < 6 else int(sys.argv[5])

print "parameter nRoot: %d, nLeaf: %d, nConn: %d, percentage: %f" % (nR, nL, nC, per)
print 'connection per root: %d' % (nL * nC / nR)

if nC > nR :
    print 'connection per leaf should not exceed the number of root'
    exit(-1)

upper = nC * (1+per)
print 'upper %d' % upper
for k in xrange(it):
    print 'iteration : %d' % k
    hosts = [random.Random(1000*i+random.randint(0, 999)) for i in xrange(nR)]
    leafs = [0 for i in xrange(nL)]

    for h in hosts:
        temp = [i for i in xrange(len(leafs))]
        for i in xrange(nC * nL / nR):
            choice = h.randint(0, len(temp) - 1)
            leafs[temp.pop(choice)] += 1

    for i in xrange(len(leafs)):
        if leafs[i] > upper:
            print 'leafs: %d, nConn: %d, not balance' % (i, leafs[i])

print 'Connections Sum: assume: %d, actual: %d' % (nL*nC, \
                                                   reduce(lambda x, y: x+y, leafs, 0))
