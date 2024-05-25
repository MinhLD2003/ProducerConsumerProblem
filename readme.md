# Producer and Consumer Problem
### Description
	- A classical synchronization problem in OS.
	- Producers are adding data to a data structure - buffer(shared memory) , while consumers are reading from, at the same time.
	- The core of problem is coordinating the producers to only push data if there's space and the consumers to only take out data from buffer if there exists data in the buffer.
	- We will use two synchronization primitives , mutexes/semaphors and condition variables for the solution.
	- Standard counting semaphores are used for 'empty' and 'full', and a mutex lock is used to represent 'mutex'. The producer and consumer (running as separate threads) move items to and from a buffer that is synchronized with the 'empty', 'full', and 'mutex' structures. This is done through the use of Linux Pthreads.
	- The 'main()' function initializes the buffer and creates the separate producer and consumer threads. Once it has created the producer and consumer threads, the 'main()' function will sleep for a period of time, and upon awakening will terminate the application.
    - The 'main()' function will be passed three parameters on the command line:
      - How long to sleep before terminating.
	  - The number of producer threads.
      - The number of consumer threads.
    - main() pseudocode: 
      - Get commad line args argv[1], argv[2], argv[3].
      - Initialize buffer.
      - Create producer thread(s).
      - Create consumer thread(s).
      - Sleep.
      - Exit.
    - The producer thread(s) will alternate between sleeping for a random period of time and inserting a random integer into the buffer. Random numbers will be produced using the 'rand()' function, which produces random integers. The consumer thread(s) will also sleep for a random period of time, and upon awakening will attempt to remove an item from the buffer.
    - Print statements related to produced and consumed items are printed as each producer and consumer completes an action.
