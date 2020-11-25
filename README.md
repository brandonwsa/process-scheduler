# process-scheduler
A process scheduler to determine average wait times and turnaround times from a list of bursts times provided to the program, written in C.

## How to Use
- First, you will need GCC installed to compile the program.
- Onced cloned, simply navigate your way to the scheduler.c file with cd and GCC compile it.
- When running the program, you will be prompted for which algorithm you would like to use and a txt file containing only numbers (burst times) each on a new line. IE prompt when running:
- ./scheduler option filename
 - Options are:
  -	0: FCFS
  -	1: Round Robin
