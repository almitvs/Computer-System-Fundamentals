CONTRIBUTIONS

Aidan Alme -- code and debugging
Bob Dong -- experiment and code

REPORT

Test run with threshold 2097152
real	0m0.578s
user	0m0.301s
sys	0m0.007s

Test run with threshold 1048576
real	0m0.162s
user	0m0.291s
sys	0m0.005s

Test run with threshold 524288
real	0m0.108s
user	0m0.306s
sys	0m0.010s

Test run with threshold 262144
real	0m0.080s
user	0m0.318s
sys	0m0.017s

Test run with threshold 131072
real	0m0.080s
user	0m0.361s
sys	0m0.039s

Test run with threshold 65536
real	0m0.081s
user	0m0.372s
sys	0m0.033s

Test run with threshold 32768
real	0m0.085s
user	0m0.385s
sys	0m0.055s

Test run with threshold 16384
real	0m0.093s
user	0m0.405s
sys	0m0.098s


With a high threshold, the program predominantly uses sequential sorting, resulting in less parallelism. This causes a higher real time due to the extensive and less parallelizable sorting operation, with substantial user time reflecting intensive computation, and low system time due to fewer context switches and I/O operations. Reducing the threshold increases parallelism, significantly decreasing real time as more computation can be executed concurrently. User time remains high because of the computational effort, while system time stays low due to minimal kernel overhead. Further threshold reduction enhances parallelism, lowering real time even more due to efficient parallel execution. User time remains high due to the computational intensity, while system time slightly increases with more context switches and I/O operations. As the threshold continues to decrease, parallel sorting is applied to more array parts, leading to a constant real time as the system efficiently handles parallel tasks. User time slightly rises due to the computation, while system time increases due to more frequent context switches and higher kernel operation overhead.


