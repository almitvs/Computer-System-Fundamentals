AAA3 MS3
-------
Bob Dong
--further debugging
--testing

Aidan Alme
--further debugging
--testing 
-----------------------------

Best Cache Configuration:

***Intro:
	To begin with, we assume the total cache in the experiment is always 32 KB for the following reason.
	1) current intel cpu has 32 KB of L1 Data Cache per core.
	2) variation in total cache size would render our experiment meaningless, as we don't consider
   	speed in this project, so simply the bigger the cache size, the better the result would be.

	We tested:
	1) fully associative
	2) 2-way, 4-way, 8-way, 16-way associative
	3) direct-mapping

	Within these types, we tested a variation of block size from 8 bytes to 128 bytes,
	combined with a varition in allocating, writing policy, and evicting policy, which results
	in a total of 30 tests per type, and 150 tests in total. We run these test to make sure that a single
	varible could be identified and mixed varibles would not hurt the clearity of our overall analysis.

	We discovered that 8-way associative cache with 8 bytes block size, write-allocate allocating, lru evicting policy, and write-back writing policy
	has the best efficiency and performance.

***Data and Analysis:
	There are two parameters, in our case, to determine the performance of a simplified cache,
	total cycles and hit rate (both store and load hit rate). Total cycles measures the speed
	of total execuation, while hit rate, as the name suggests, measures the probablity of cpu
	finding the information from its cache. While hit rate is important, the total cycle is the
	most important parameter for our evalution because speed is the ultimate factor that measures efficiency,
	which is only directly  measured in total cycles in our simplified case.

	After thorough testings, we discovered that the 8-way associative cache with write-allocate, write-back, lru that has 8 bytes block size
	has the lowest total cycles, which is 7915974 for gcc.trace. The runner up is 16-way associative cache
	with 8 bytes block size and the same eviction, writing, and evicting policy, which is 7916379 for gcc.trace.

	However, a "lower total cycles" number does not suggest a higher hit rate. In fact, our data shows the opposite.
	For example, in the "fully associative, write-allocate, lru, and write-back" group for gcc.trace, a load hit rate increase from 98.66%
	to 99.67% causes the total cycles to increase from 7924181 to 13586265, a 71% increase! Of course, this is expected, because the method
	we uses to increase hit rate is to increase block size (and decrease number of block as a result of constant 32 KB total cache size), which
	increases time for the eviction policy since larger block takes longer time to evict.

	Interestingly, our experients suggests that lru is better than fifo in all cases, for it increases hit rate and has a lower total cycles. This
	might have been caused by the fact that the  code of those traces employees good temporal locality, so the least recently used policy benifits from it, as first-in-first-out
	does not care about recently-used bits. Similarly, "write-allocate, write-back" is better than "no-write-write, write-through", which is better than "write-allocate,
	write-through" in terms of total cycles, and this can be explained as write-back policy occupies less bandwidth than write-through since it does not write
	everything into the lower level memory and only write when required by evicting policy; "write-allocate, write-through" is simply stupid because both write-allocate
	and write-through takes a lot of total cycles to be loaded. In addition, "write-allocate" also increases hit rate, probably because the code employees good temporal locality.
	
	
***Limitation and Discussion:
	Our implementation is a very simplified model. It ignores the fact of monetary cost and search time of different policies. This could be potentially detrimental because search
	time of different policies could increase the actual total time of running a process and thus hurt the efficiency of the cache.

	Additionally, write-back requires a dirty bit, which could potentially decrease block size and hurt hit rate. And the code traced seem to employ good temporal locality, but we could
	not expect that in real life, so write-allocate and lru might not increases total hit rate and might hurt total speed.

	Also, the relationship between hit rate and total cycles in our model is overly simplified and rather counterintuitive; in real life, the cpu employ better mechanism that ensures
	that better hit rate means better performance, which is essentially why we need to take care of localities in the first place.
