#!/usr/bin/python
import time

from pyobarr import *

class testclass(object):
	def __init__(self):
		self.teststr = 'abcdef'
		self.otherstr = 'xzlksdf'
	def funcfun(self, other):
		return other
a = obarr()
a.append('xxx')
for x in range(1000000):
	a.append(testclass())
	
for i in range(0, 50000):
	a.delete(i)
print "Done"
for x in range(1000000):
	a[x] = testclass()
print "Reallocated"

time.sleep(10)
a.clear()
print "Deallocated"
for x in range(1000000):
	a.append(testclass())
print "Reallocated"
time.sleep(10)


time.sleep(100)

