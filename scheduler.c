#include <stdio.h>
#include <string.h>
#include <stdbool.h>
int main(){
	// Input variables
	int processes;
	int quantum;
	// Process Input
	printf("Jumlah Proses: ");
	scanf("%d", &processes);
	// Input validation variable for time quantum
	bool valid = false;
	// Loop to find a valid quantum
	while(!valid){
		printf("Time Quantum > 0: ");
		scanf("%d", &quantum);
		if(quantum > 0){
			valid = true;
		}
	}
	int total = 0; // Variable used to decide loop amount
	int totalbursttime = 0; // Variable used to calculate total burst time
	// Input Storage Variables
	int arrivals[processes]; 
	int bursts[processes];
	// Variables to account for abnormal cases
	int biggestarrival = 0;
	int smallestarrival = 2147483647;
	int biggestarrivalburst = 0;
	// Arrival and Burst Input Loop
	for(int i = 0; i < processes; i++){
		valid = false; // Reuse validation variable
		while(!valid){
			printf("P%d - Masukkan Arrival dan Burst Time: ", i+1);
			scanf("%d %d", &arrivals[i], &bursts[i]);
			if(arrivals[i] >=0 && bursts[i] >= 1){
				valid = true;
			}
		}
		if(arrivals[i] > biggestarrival){ // Saves biggest arrival time and burst of process with biggest arrival time
			biggestarrival = arrivals[i];
			biggestarrivalburst = bursts[i];
		}
		if(arrivals[i] < smallestarrival){ // Saves smallest arrival time 
			smallestarrival = arrivals[i];
		}
		// Currently both of these calculate total burst time
		total += bursts[i]; 
		totalbursttime += bursts[i];
	}
	// Case: Very high arrival time, bigger than total burst time
	if(total < biggestarrival + biggestarrivalburst){
		total = total + biggestarrival;
	} 
	// Case: Smallest arrival is not 0
	if(total < smallestarrival + totalbursttime){
		total = smallestarrival + totalbursttime;
	}
	printf("=======================================================================\n"
	"PROCESS INPUT\n"
	"=======================================================================\n"
	"Time Quantum : %d\n"
	"PID                  Arrival Time           Burst Time\n"
	"-----------------------------------------------------------------------\n", quantum);

	for(int i = 0; i < processes; i++){
		printf("P%d                              %d                    %d\n", i+1, arrivals[i], bursts[i]);
	}

	printf("=======================================================================\n\n"
	"=======================================================================\n"
	"CPU EXECUTION TIMELINE (GANTT CHART)\n"
	"=======================================================================\n");
	
	int burstamounts = 0; // Variable to store amount of bursts
	int tempbursts[processes]; // Used to track remaining burst time of each process within the loop

	for(int i = 0; i < processes; i++){
		tempbursts[i] = bursts[i]; // Copy
	}
	for(int i = 0; i< processes; i++){
		burstamounts += (tempbursts[i] + quantum - 1)/quantum; // Burst amount of each process is calculated with ceil(bursttime/quantum) and summed up
	}
	int order[burstamounts]; // Stores process run order
	int timeorder[burstamounts]; // Stores time each process takes in order
	int completions[processes]; // Stores completion time of each process
	int downtime[biggestarrival]; // Keeps track of what time is downtime
	int downtimes[biggestarrival]; // Keeps track of length of downtimes
	int remainings[burstamounts]; // Used to store remaining time of each process after running in order
	// init to avoid unwanted behavior
	for(int i = 0; i<sizeof(downtime)/4;i++){
		downtime[i] = -1;
		downtimes[i] = 0;
	}
	int downtimecounter = 0; // used to increment downtime array
	int downtimetimecounter = 0; // counts length of downtimes
	int downtimeamountcounter = 0; // counts how many downtimes
	int timecount = 0; // counts how much time has elapsed
	int count = 0; // counts how many bursts have elapsed
	int queue[processes]; // circular array as a queue
	int starts[processes]; // Keeps track of starting time of each process
	int tempstarts[processes]; // Keeps track of temporary starting times to calculate wait time
	int waits[processes]; // Keeps track of wait time of each process
	int initiated[processes]; // Keeps track of which process have been initiated into the queue
	// init to avoid unwanted behavior
	for(int i = 0; i<processes; i++){
		queue[i] = 0;
		starts[i] = -1;
		waits[i] = 0;
		tempstarts[i] = -1;
		completions[i] = 0;
		initiated[i] = -1;
	}
	int front = 0; // front of queue
	int back = 0; // back of queue
	int start = 0; // pointer for initiated array
	// initial init of queue
	for(int i = 0; i < processes; i++){
		// only added if arrival time is met
		if(arrivals[i] <= timecount){
			// only added if not finished yet
			if(tempbursts[i] != 0){
				bool beeninitiated = false; // validation for initiated
				for(int j = 0; j < processes; j++){
					if(initiated[j] == i+1){
						beeninitiated = true;
					}
				}
				if(!beeninitiated){ // only adds if has never been added before
					// enqueue
					queue[back] = i+1;
					back = (back+1)%processes;
					// initiated array update
					initiated[start] = i+1; 
					start++;
				}
			} 
		}
	}
	while(timecount < total){ // will loop until timecount has met the max
		if(count == burstamounts){ // certain scenarios may cause total to be more than needed, ends loop if burst amounts has met the max
			break;
		}
		if(queue[front] != 0){ // runs if queue is not empty
			order[count] = queue[front]; // Saves front process to track order
			if(tempstarts[queue[front]-1] != -1){ // Only tracks after process has been added to queue once
				waits[queue[front]-1] = waits[queue[front]-1] + timecount - tempstarts[queue[front]-1] - quantum; // wait time + (current time - (last time the process started + quantum) (aka last time process ended)) 
			}
			tempstarts[queue[front]-1] = timecount; // Saves temporary start
			if(starts[queue[front]-1] == -1){ // Only tracks if process hasnt ever been added to queue
				starts[queue[front]-1] = timecount; // Saves start time
			}
			if(tempbursts[queue[front]-1] < quantum){ // If remaining burst time < quantum -> process finished
				timecount += tempbursts[queue[front]-1]; // increment timecount by the remaining amount
				timeorder[count] = tempbursts[queue[front]-1]; // saves remaining amount of time to order
				// finished = 0
				tempbursts[queue[front]-1] = 0; 
				remainings[count] = 0;
			} else {
				timecount += quantum; // increments by quantum
				timeorder[count] = quantum; // saves quantum amount of time to order
				tempbursts[queue[front]-1] = tempbursts[queue[front]-1] - quantum; // decrements by quantum
				remainings[count] = tempbursts[queue[front]-1]; // Saves remaining time to order
			}
			// runs init loop again in case process can be added
			for(int i = 0; i < processes; i++){
				if(arrivals[i] <= timecount){
					if(tempbursts[i] != 0){
						bool beeninitiated = false;
						for(int j = 0; j < processes; j++){
							if(initiated[j] == i+1){
								beeninitiated = true;
							}
						}
						if(!beeninitiated){
							queue[back] = i+1;
							back = (back+1)%processes;
							initiated[start] = i+1;
							start++;
						}
					} 
				}
			}
			int tempback = back; // temporarily saves value of back
			if(tempbursts[queue[front]-1] != 0){ // if current process not finished, send to back
				queue[back] = queue[front];	
				back = (back+1)%processes;
			} else {
				completions[queue[front]-1] = timecount; // if finished, saves completion time
			}
			if(tempback != front){ // makes sure back != front
				queue[front] = 0; // to avoid unwanted behavior
			}
			front = (front+1)%processes;
			count++; // increment burst amount count
		} else { // queue empty = downtime
			downtime[downtimecounter] = timecount; // adds current time to downtime list
			timecount++; // increment to allow for another check
			downtimecounter++; // increment pointer
			downtimetimecounter++; // tracks how long current downtime lasts
			// runs init again to check if any processes can be added to queue
			for(int i = start; i < processes; i++){
				if(arrivals[i] <= timecount){
					if(tempbursts[i] != 0){
						bool beeninitiated = false;
						for(int j = 0; j < processes; j++){
							if(initiated[j] == i+1){
								beeninitiated = true;
							}
						}
						if(!beeninitiated){
							queue[back] = i+1;
							back = (back+1)%processes;
							initiated[start] = i+1;
							start++;
							// if added, no longer downtime
							downtimes[downtimeamountcounter] = downtimetimecounter; // Saves length of previous downtime
							downtimeamountcounter++; // Increases down time amount
							downtimetimecounter = 0; // Resets length of current downtime
						}
					} 
				}
			}
		}
	}
	total = timecount; // abnormal cases might cause a difference
	int gantttimecounter = 0; // used to print time partitions
	int downtimecountercounter = 0; // used to increment through downtimes array
	int basepadding = (71-burstamounts)/burstamounts; // padding 
	char str2[100]; // saves string to be used as padding later
	for(int i = 0; i < burstamounts; i++){ // loops burst amounts times
		for(int j = 0; j < sizeof(downtime)/4; j++){ // checks if current time is downtime
			if(downtime[j] == gantttimecounter){ 
				int left = (basepadding - 1)/2;
				int right = basepadding - 1 - left;
				printf("|%*sI%*s", left, "", right, ""); // prints I as in IDLE
				gantttimecounter += downtimes[downtimecountercounter]; // increases gantt counter by the downtime amount 
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter; 
			}
		}
		char str[100]; // building string to calculate padding
		snprintf(str, sizeof(str), "P%d", order[i]);
		int left = (basepadding - strlen(str))/2;
		int right = basepadding - strlen(str) - left;
		snprintf(str2, sizeof(str2), "%*s%s%*s",left, "", str, right, "");
		printf("|%s",str2); // padded string
		gantttimecounter += timeorder[i]; // increments time based on order
	}
	printf("|\n");
	gantttimecounter = 0; // reset to be used again
	for(int i = 0; i < burstamounts; i++){
		for(int j = 0; j < sizeof(downtime)/4; j++){ // downtime checker
			if(downtime[j] == gantttimecounter){
				printf("%d%*s",gantttimecounter, basepadding, ""); 
				gantttimecounter += downtimes[downtimecountercounter]; // increases gantt counter by the downtime amount
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
		char str3[100]; // building string to calculate padding
		snprintf(str3, sizeof(str3), "%d", gantttimecounter);
		int padding = strlen(str2) - strlen(str3) + 1;
		printf("%d%*s", gantttimecounter, padding, "");
		gantttimecounter += timeorder[i]; // increments time based on order
	
	}
	printf("%d\n", gantttimecounter); // last time doesnt get printed by loop so here it is
	printf("=======================================================================\n"
"QUANTUM AND PREEMPTION INFORMATION\n"
"=======================================================================\n");
int anothercounter = 0; // another time counter
int preempcounter = 0; // counts how many preemps occur
for(int i = 0; i< burstamounts;i++){
	for(int j = 0; j < sizeof(downtime)/4; j++){ // downtime checker
			if(downtime[j] == anothercounter){
				anothercounter += downtimes[downtimecountercounter]; // increases time counter by downtime length
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
	if(remainings[i] != 0){ // if the process finished then process isnt sent to ready queue so no print
	printf("t=%d : quantum P%d habis (sisa BT=%d) -> READY\n", timeorder[i] + anothercounter, order[i], remainings[i]);  
	preempcounter++; // sent to ready queue -> preemp
	}
	anothercounter += timeorder[i]; // increments time based on order
}
printf("Total Preemption: %d\n", preempcounter);
printf("=======================================================================\n"
"SCHEDULING TABLE\n"
"=======================================================================\n""PID          AT       BT       CT      TAT       WT       RT\n"
"-----------------------------------------------------------------------\n"
);
for(int i = 0; i<processes; i++){
	printf("P%d            %d        %d        %d        %d        %d        %d\n", i+1, arrivals[i], bursts[i], completions[i], completions[i] - arrivals[i], waits[i] + starts[i] - arrivals[i], starts[i] - arrivals[i]); 
}
// Variables used to calculate totals
int totalwait = 0; 
int totaltat = 0;
int totalrt = 0;
for(int i = 0; i<processes; i++){
	totalwait += waits[i] + starts[i] - arrivals[i];
	totalrt += starts[i] - arrivals[i];
	totaltat += completions[i] - arrivals[i];
}
// Variables used to store averages
float awt = (float) totalwait / processes;
float atat = (float) totaltat /processes;
float art = (float) totalrt /processes;
int switches = 0; // counts context switches
for(int i = 0; i < burstamounts - 1; i++){
	if(order[i] != order[i+1]){ // if a process changed to a different process -> context switch
		switches++;
	}
}
		
printf("========================================================================\n\n"
"=======================================================================\n"
"SCHEDULING PERFORMANCE\n"
"=======================================================================\n"
"Average Waiting Time    : %.2f\n"
"Average Turnaround Time : %.2f\n"
"Average Response Time   : %.2f\n\n"
"=======================================================================\n"
"CPU UTILIZATION AND THROUGHPUT\n"
"=======================================================================\n"
"CPU Utilization : %.2f%\n"
"Throughput      : %.2f process/time unit\n\n"
"=======================================================================\n"
"CONTEXT SWITCH INFORMATION\n"
"=======================================================================\n"
"Total Context Switch : %d\n\n"
"=======================================================================\n"
"PROCESS STATE TRANSITIONS\n"
"=======================================================================\n", awt, atat, art,((float) totalbursttime/total) * 100, (float) processes/total, switches);
for(int i = 0; i < processes; i++){
	printf("P%d : NEW -> READY (t=%d) -> ", i+1, arrivals[i]); // prints NEW -> READY, doesnt need loop only happens once
	int counttime = 0; // another time counter
	for(int j = 0; j < burstamounts; j++){ // goes through the entire order to check
		for(int k = 0; k < sizeof(downtime)/4; k++){ // downtime checker
			if(downtime[k] == counttime){
				counttime += downtimes[downtimecountercounter]; // increases time counter by downtime length
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
		if(i+1 == order[j]){ // checks if current process is referenced
			if(completions[i] - counttime > quantum){ // if completion time - current time < quantum -> finished, else goes back to ready queue
				printf("RUNNING (t=%d) -> READY (t=%d) -> ", counttime, counttime + quantum);
			} else {
				printf("RUNNING (t=%d) -> TERMINATED (t=%d)\n", counttime, completions[i]);
			}
		}
		counttime += timeorder[j]; // increments time based on order
	}
}

return 0;
}


