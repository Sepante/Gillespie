from __future__ import division
import numpy as np


with open('cdata.txt', encoding="utf-8") as f:
    data=[float(i) for i in f]

data = 100*np.array(data)
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
data = np.diff(data)
s = np.std(data)
m = np.mean(data)
b = (s-m)/(s+m)
print (b)