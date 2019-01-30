# Barrier-Synchronization

Akshaya Nagarajan Jainesh Doshi

The goal of this assignment is to introduce OpenMP, MPI, and barrier synchronization concepts. We implemented several barriers using OpenMP and MPI, and synchronized between multiple threads and machines.

OpenMP allows you to run parallel algorithms on shared-memory multiprocessor/multicore machines. For this assignment we implemented two spin barriers using OpenMP. MPI allows you to run parallel algorithms on distributed memory systems, such as computer clusters or other distributed systems. We implemented two spin barriers using MPI. Finally, we chose one of our OpenMP barrier implementations and one of our MPI barrier implementations, and combined the two in an MPI-OpenMP combined program in order to synchronize between multiple cluster nodes that are each running multiple threads.

We ran experiments to evaluate the performance of our barrier implementations. We ran our OpenMP barriers on an 8-way SMP (symmetric multi-processor) system, and our MPI and MPI-OpenMP combined experiments on a (up to) 24 12-core cluster (24 nodes, and each node has two six-core processors).

We created a write-up that explains what we did, presents our experimental results, and most importantly, analyzes our results to explain the trends and phenomona we saw. Please go through the Readme's inside to get information on how to compile and run the codes.
