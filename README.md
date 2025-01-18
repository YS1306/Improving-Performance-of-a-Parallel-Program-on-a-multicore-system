## Introduction:
In this assignment, we investigate the performance issues encountered when a parallel
implementation using the performant_thread.so library unexpectedly performed worse than its
serial counterpart. The Weather Department needed faster processing of weather data, and the
library promised full parallelism with no thread contention. However, the test program
designed to leverage this library did not deliver the anticipated speedup. Our task is to analyze
the code using profiling tools, identify the bottlenecks, and make minimal yet effective changes
to achieve the desired performance improvement.

## Analysis

Upon analysing the code, we observed that each variable of the Sensor Readings struct is being
accessed exclusively by a separate thread, implying no direct data sharing between the threads
with respect to this structure. However, since the variables are part of a single structure, they
have been allocated consecutive bytes within the physical address space. This was verified by
printing the addresses of the variables as shown below:

From this data, it is evident that almost as many times as we access the L1 cache (a private
cache), we are also accessing the L3 cache. This indicates that for each access, the entire cache
block is fetched from a lower-level cache.
Since the variables are mapped to consecutive virtual addresses, they will be mapped to
consecutive physical addresses in the physical address space. map to the same cache block.
Given that each variable is independently accessed by different threads, whenever one thread
modifies a variable within the struct, the entire cache block is invalidated for all other threads.
This invalidation forces the modifier thread to write back the latest copy into the L3 Cache and
all other threads to fetch the modified block again from the L3 cache, even though there was
no need for this. This unnecessary cache block invalidation, write back and reloading leads to
an increase in overall execution time, a phenomenon known as False Sharing.
This behaviour results in multiple accesses to the lower-level cache and repeated unnecessary
accesses to the L1 Cache to retrieve the most recent copy of the block, thereby causing
performance degradation, despite the lack of true data sharing among threads.


## Improving the performance:

For improving the overall performance of the program, we should map these variables to
different cache blocks to avoid the problem of False Sharing. In that case, the threads will not
end invalidating their copies of the same cache line again and again.
For this purpose, we have used the following approach:1. Here, each variable is of size 1 byte, so to make them get allocated into different blocks,
we need to have at least 63 more bytes between each variable.
2. For this, we add padding of 63 bytes after each variable, so that the next variable is
allocated the 64th virtual address after the current virtual address. Then, each of these
addresses will fall into different cache blocks, thus eliminating the possibility of false
sharing between the threads.
This approach ensures that there is no possibility of a thread leading to invalidation of local
copies of the same cache block of other threads.

## Results:

![Chart](https://github.com/YS1306/Improving-Performance-of-a-Parallel-Program-on-a-multicore-system/blob/master/Chart3a.png)
Fig 5.1. Comparison of Cache Accesses for each variable.


The above data shows a drastic decrease in the no. of cache accesses for each of the variable
addresses. This means that for every variable there were on an average 50 times more no. of
cache accesses just because of false sharing.
The below table shows the improvement in the program execution after adding padding
between the variables:

# Performance Comparison: Original vs Modified Program

| Metric                  | Original Program         | Modified Program         |
|-------------------------|--------------------------|--------------------------|
| **IPC (Instructions per Cycle)** | 0.66                   | 2.89                     |
| **Cache References**     | 1,618,505,978            | 43,970,601               |
| **Execution Time**       | 20.0423 seconds          | 4.5 seconds              |

## Summary:
- The modified program exhibits a significant increase in IPC (from 0.66 to 2.89), indicating better instruction throughput.
- Cache references have decreased drastically from over 1.6 billion to just under 44 million, showing better memory access optimization.
- The execution time has been reduced substantially, from 20.04 seconds to 4.5 seconds, reflecting an improvement of almost ### 4x.
