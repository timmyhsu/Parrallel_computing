# Let's calculate variance 
from mpi4py import MPI
import numpy as np
comm   = MPI.COMM_WORLD
npes = comm.Get_size()
myrank = comm.Get_rank()
recv = np.zeros(10 , dtype=float )
a = np.array( list (map ( lambda x : myrank + x  ,range(10) ) ) , dtype=float) # ; print(a)

comm.Reduce( [a , 10 ,MPI.DOUBLE ], recv , MPI.SUM ,  0 ) ; 

mu =  0
if myrank == 0 :
    mu = np.sum(recv)/50
    #print(recv)

mu =  comm.bcast( mu , root = 0 )
print(myrank , mu) ; print(a)
a = np.array( list (map ( lambda x : (x - mu)**2  , a ) ) , dtype=float)   ; print(a)
comm.Reduce( [a , 10 ,MPI.DOUBLE ], recv , MPI.SUM ,  0 )

if myrank == 0 : print( "%.5f" % (np.sum(recv)/50) ) 

