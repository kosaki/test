Here the program to reproduce the failure:

1.) make forkoff
2.) forkoff ./forkoff 18 288 100  //create 288 processes that each allocate 18GB and loop 100 times

