This project simulates various disk scheduling algorithms using a shared memory mechanism (mmap) and multiple processes (fork). Each algorithm moves the disk head to service I/O requests based on different strategies.

Features
Disk Scheduling Algorithms Implemented:
First-Come, First-Serve (FCFS)
Shortest Seek Time First (SSTF)
SCAN
C-SCAN
LOOK
C-LOOK
Optimized Disk Scheduling (OPT)
Project Structure
bash
Copy code
.
├── main.c          # Main program with disk scheduling simulation
└── README.md       # Project documentation
Requirements
GCC (GNU Compiler Collection) to compile the program.
POSIX-compliant operating system (e.g., Linux) for mmap, pthread, and fork.
Compilation
To compile the program, run the following command in your terminal:

gcc -o disk_simulation main.c -pthread

After compiling, you can run the program by providing the initial disk head position as a command-line argument. The program generates a set of random I/O requests and simulates the disk head movements for each scheduling algorithm.


./disk_simulation <disk_head_position>
Example:

./disk_simulation 1500
This will set the initial disk head position to 1500 and simulate the disk scheduling algorithms.

Disk Scheduling Algorithms
1. FCFS (First-Come, First-Serve)
This algorithm services I/O requests in the order they arrive, without any optimization for seek time.

2. SSTF (Shortest Seek Time First)
This algorithm selects the request that is closest to the current position of the disk head to minimize seek time.

3. SCAN
The disk arm moves towards one end of the disk, servicing requests in that direction, then reverses direction at the end of the disk.

4. C-SCAN (Circular SCAN)
Similar to SCAN, but instead of reversing direction, the disk head moves to the other end and starts servicing requests again.

5. LOOK
LOOK is similar to SCAN, but the disk arm only goes as far as the furthest request in each direction, then reverses.

6. C-LOOK
Similar to C-SCAN, but the head only moves between the furthest requests, not all the way to the disk's edge.

7. OPT (Optimized Disk Scheduling)
This strategy attempts to minimize seek time by dynamically adjusting the disk head's movement based on future requests.

How It Works
The program creates 7 child processes using fork(), each simulating one of the disk scheduling algorithms.
Shared memory is used (mmap) for synchronization between processes.
Each algorithm computes the total number of movements for servicing 1000 random I/O requests, and the results are printed.
Output
The program prints the total number of movements for each algorithm. For example:


Disk head at : 1500
FCFS moved: 21.54 times
SSTF moved: 12.67 times
SCAN moved: 16.32 times
C-SCAN moved: 18.91 times
LOOK moved: 14.52 times
C-LOOK moved: 17.34 times
Optimized moved: 10.11 times
CPU used time : 1523.000000

Notes
The mmap function is used to create shared memory that allows multiple processes to coordinate their output.
The use of pthread_mutex ensures that each process can safely print its results without race conditions.
The program measures the total CPU time used to complete all simulations.
