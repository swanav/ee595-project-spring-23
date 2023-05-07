from ctypes import *
from py_interface import *

mempool_key = 1234
mem_size = 4096
memblock_key = 2333
Init(mempool_key, mem_size)
v = ShmBigVar(memblock_key, c_int * 10)
with v as o:
    for i in range(10):
        o[i] = c_int(i)
    print(*o)
FreeMemory()
