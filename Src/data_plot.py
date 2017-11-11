from __future__ import division
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

with open('timed_data.txt', encoding="utf-8") as f:
    data=[float(i) for i in f]

"""
n_size = int(data.pop(0))
#n = int(data.pop(0))
p_size = int(data.pop(0))
q_size = int(data.pop(0))
r_size = int(data.pop(0))

nrange = [ data.pop(0) for i in range(n_size)]
prange = [ data.pop(0) for i in range(p_size)]
qrange = [ data.pop(0) for i in range(q_size)]
rrange = [ data.pop(0) for i in range(q_size)]
"""
#runNum = int(data.pop(0))

data =( np.array(data) )
Q = np.where(data<0)
print ( Q )
#Q = np.ndarray.tolist ( np.where(data) )
B = np.split(data ,  np.array([1,8]))
#B= np.split(data, np.where(data[:]==-1 ))

#np.histogram(data)
#plt.hist(data,100)
#plt.show()
#with open('cdata.txt') as f:
#    floats = map(float, f)
#data = np.array(floats)
"""
import codecs
with codecs.open('cdata.txt', "r",encoding='utf-8', errors='ignore') as fdata:
    print ("hi")
"""
#tdata = np.diff(data[:runNum])
#s = np.std(tdata)
#m = np.mean(tdata)
#b = (s-m)/(s+m)
#print (b)