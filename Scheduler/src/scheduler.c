#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Brandon Ahrens CS 410 Programming Assignment 3.
//Will get the average wait times and turnaround times from a txt file with processes burst times listed
//using either FCFS or Round Robin. User can pick which one by using the argument line.
//./scheduler <option> <filename>
//Options are:
//	0: FCFS
//	1: Round Robin



//a process. Built so it can be included in a doubly linked list.
//dynamicBurst is used to know when a process has finished. burst never changes.
struct task {
	int pid;
	int burst;
	int waitTime;
	int turnaroundTime;
	int dynamicBurst;
	struct task *next;
	struct task *prev;
};


//used to house the processes (tasks). Will be a doubly linked list.
struct list {
	struct task *head;
	struct task *tail;
};


//prints the list of processes
void print(struct list *thislist){
	struct task * current = thislist->head;
	while(current!=NULL){
		printf("Pid: %d\tBurst time: %d\n", current->pid, current->burst);
		current=current->next;
	}
	printf("\n");
}


//inserts task into list
void insert(struct list *thislist, int pidNum, int burstTime){
	//create a new task
	struct task *tmp = malloc(sizeof(struct task));
	tmp->pid = pidNum;
	tmp->burst = burstTime;
	tmp->dynamicBurst = burstTime;
	tmp->waitTime = 0;
	tmp->turnaroundTime = 0;
	tmp->next = NULL;
	tmp->prev = NULL;

	struct task *current = thislist->head;

	//if the list is empty
	//head points to the new task (only task)
	if(current == NULL){
		thislist->head = tmp;
		return;
	}

	//find the last node
	while(current->next!=NULL){
		current = current->next;
	}

	//the last task in list will now be new task coming in.
	tmp->prev = current; //set the previous task for new task coming in.
	current->next = tmp;

	//tail points to last node
	thislist->tail = tmp;

	return;
}

//delete a task from the doubly linked list
void delete_task(struct list *thislist, struct task *thisTask){

	//see if task is tail or head first
	//if task is head, then will have no prev
	if (thisTask->prev == NULL){
		struct task *tmp = thisTask->next;
		thislist->head = tmp;
		tmp->prev = NULL; //make new head have NULL for prev
		
	} //if task is tail, will have no next
	else if (thisTask->next == NULL){
		struct task *tmp = thisTask->prev;
		thislist->tail = tmp;
		tmp->next = NULL; //make new tail have NULL for next
	} //if task is not a head or tail node.
	else {
		struct task *prevTask = thisTask->prev; //get previous task of task to be deleted
		struct task *nextTask = thisTask->next; //get next task of task to be deleted
		prevTask->next = nextTask; //point prev task of task to be deleted to next task of task to be deleted
		nextTask->prev = prevTask; //point next task of task to be deleted to prev task of task to be deleted
	}

	free(thisTask); //deletes task
}

//performs FCFS scheduling on processes in list.
void perform_FCFS_scheduling(struct list *thislist){

	struct task *current = thislist->head; //capture head task
	struct task *prevTask = current->prev; //used to get burstime and turnaround time of prev task
	int total_wait_time = 0; //used to get wait time of all processes.
	int total_turnaround_time = 0; //used to get turnaround time of all processes.
	int processCount = thislist->tail->pid + 1; //used to calc average wait time and turnaround time.

	//iterate through list of processes (tasks) and get their wait times and turnaround times.
	while (current!=NULL){
		if (prevTask == NULL){ //if task is head		
			current->waitTime = 0;
			current->turnaroundTime = current->burst;
		} //if task is not head
		else {		
			current->waitTime = prevTask->burst + prevTask->waitTime;
			current->turnaroundTime = prevTask->turnaroundTime + current->burst;
		}

		//print process
		printf("P%d ", current->pid);
		
		//calc total wait time
		total_wait_time = total_wait_time + current->waitTime;

		//calc total turnaround time
		total_turnaround_time = total_turnaround_time + current->turnaroundTime;

		//set new previous task
		prevTask = current;

		//set new current task
		current = current->next;
	}
	printf("\n");

	//print average wait time
	double average_wait_time = (double)total_wait_time / processCount;
	printf("Average wait time: %f\n", average_wait_time);

	//print average turnaround time
	double average_turnaround_time = (double)total_turnaround_time / processCount;
	printf("Average turnaround time: %f\n", average_turnaround_time);
	
}


//performs round robin schedulig on processes in list.
void perform_round_robin_scheduling(struct list *thislist){
	
	struct task *current = thislist->head; //capture head task
	struct task *prevTask = current->prev; //used to capture previous task
	int timeSlice = 10; //time slice in ms to do round robin approach
	int total_wait_time = 0; //used to get wait time of all processes.
	int total_turnaround_time = 0; //used to get turnaround time of all processes.
	int tempTurnAroundTime = 0; //used to hold turn around time of a process.
	int processCount = thislist->tail->pid + 1;//used to calc average wait time and turnaround time.
	int completedProcesses = 0; //used to know when all processes have been completed.

	//iterate through list of processes and get their wait times and turnaround times.
	while(completedProcesses != processCount){

		//print process being worked on
		printf("P%d ", current->pid);

		//subtract time slice off of burst
		current->dynamicBurst = current->dynamicBurst - timeSlice;

		//if process was finished
		if (current->dynamicBurst <= 0){
			completedProcesses++;

			//calc total turn around time.
			tempTurnAroundTime = tempTurnAroundTime + (current->dynamicBurst + timeSlice);
			total_turnaround_time = total_turnaround_time + tempTurnAroundTime;

			//calc total wait time
			total_wait_time = total_wait_time + (tempTurnAroundTime - current->burst);

			//set new  previous task
			prevTask = current;

			//if task is tail that is being deleted, set current task to head.
			if (current->next == NULL){
				current = thislist->head;
			}
			else{
				//get next task
				current = current->next;
			}
			
			//if there is more than 1 task in round robin
			if (current->next != NULL || current->prev != NULL){
				//delete task
				delete_task(thislist, prevTask);

				//reset prevtask
				prevTask = current->prev;
			}			
		}
		else {
			
			//add to temp turn around time
			tempTurnAroundTime = tempTurnAroundTime + timeSlice;

			// get new previous task
			prevTask = current;

			//if process was tail, go back to head
			if (current->next == NULL){
				current = thislist->head;
			}
			else{
				//get new current task
				current = current->next;
			}
		}
	}
	printf("\n");

	//print average wait time
	double average_wait_time = (double)total_wait_time / processCount;
	printf("Average wait time: %f\n", average_wait_time);

	//print average turnaround time
	double average_turnaround_time = (double)total_turnaround_time / processCount;
	printf("Average turnaround time: %f\n", average_turnaround_time);
	
}



int main(int argc, char *argv[]){

	if (argc == 3 && (strcmp(argv[1], "0")==0 || strcmp(argv[1], "1")==0)){

		//make list
		struct list * processList = malloc(sizeof(struct list));
		processList->head = NULL;
		processList->tail = NULL;

		//gets file to read from argument line
		FILE *input = fopen(argv[2], "r");
		int burst_time;
		int processCount = 0;
		
		//fill the list with the processes
		while(fscanf(input, "%d", &burst_time)!=EOF){

			//insert process in list
			insert(processList, processCount, burst_time);

			//increase processCount for next process
			processCount++;
		}

		//check if file was empty.
		if (processCount != 0){
			//if user chose FCFS
			if (strcmp(argv[1], "0")==0){
				//perform FCFS scheduling
				perform_FCFS_scheduling(processList);
			}
			else if(strcmp(argv[1], "1")==0){
				//perform round robin scheduling
				perform_round_robin_scheduling(processList);
			}
		}
		else{
			printf("Error: No processes to work on from file %s\n", argv[2]);
		}		
	
	}
	else{
		printf("Usage: ./scheduler <option> <filename>\n");
		printf("Available options:\n\t0: FCFS\n\t1: Round robin\n");
	}
	return 0;

}
