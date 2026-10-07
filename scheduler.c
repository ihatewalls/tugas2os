#include <stdio.h>
#include <string.h>
#include <stdbool.h>
int main(){
	int processes;
	int quantum;
	int total = 0;
	int totalbursttime = 0;
	printf("Jumlah Proses: ");
	scanf("%d", &processes);
	bool valid = false;
	while(!valid){
		printf("Time Quantum > 0: ");
		scanf("%d", &quantum);
		if(quantum > 0){
			valid = true;
		}
	}
	int arrivals[processes];
	int bursts[processes];
	int biggestarrival = 0;
	int smallestarrival = 2147483647;
	int biggestarrivalburst = 0;
	for(int i = 0; i < processes; i++){
		valid = false;
		while(!valid){
			printf("P%d - Masukkan Arrival dan Burst Time: ", i+1);
			scanf("%d %d", &arrivals[i], &bursts[i]);
			if(arrivals[i] >=0 && bursts[i] >= 1){
				valid = true;
			}
		}
		if(arrivals[i] > biggestarrival){
			biggestarrival = arrivals[i];
			biggestarrivalburst = bursts[i];
		}
		if(arrivals[i] < smallestarrival){
			smallestarrival = arrivals[i];
		}
		total += bursts[i];
		totalbursttime += bursts[i];
	}
	if(total < biggestarrival + biggestarrivalburst){
		total = total + biggestarrival;
	}
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
	int burstamounts = 0;
	int tempbursts[processes];

	for(int i = 0; i < processes; i++){
		tempbursts[i] = bursts[i];
	}
	for(int i = 0; i< processes; i++){
		burstamounts += (tempbursts[i] + quantum - 1)/quantum;
	}
	int order[burstamounts];
	int timeorder[burstamounts];
	int completions[processes];
	int downtime[biggestarrival];
	int downtimes[biggestarrival];
	int remainings[burstamounts];
	for(int i = 0; i<sizeof(downtime)/4;i++){
		downtime[i] = -1;
		downtimes[i] = 0;
	}
	int downtimecounter = 0;
	int downtimetimecounter = 0;
	int downtimeamountcounter = 0;
	int timecount = 0;
	int count = 0;
	int queue[processes];
	int starts[processes];
	int tempstarts[processes];
	int waits[processes];
	int initiated[processes];
	for(int i = 0; i<processes; i++){
		queue[i] = 0;
		starts[i] = -1;
		waits[i] = 0;
		tempstarts[i] = -1;
		completions[i] = 0;
		initiated[i] = -1;
	}
	int front = 0;
	int back = 0;
	int start = 0;
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
	while(timecount < total){
		if(count == burstamounts){
			break;
		}
		if(queue[front] != 0){
			order[count] = queue[front];
			if(tempstarts[queue[front]-1] != -1){
				waits[queue[front]-1] = waits[queue[front]-1] + timecount - tempstarts[queue[front]-1] - quantum;
			}
			tempstarts[queue[front]-1] = timecount;
			if(starts[queue[front]-1] == -1){
				starts[queue[front]-1] = timecount;
			}
			if(tempbursts[queue[front]-1] < quantum){
				timecount += tempbursts[queue[front]-1];
				timeorder[count] = tempbursts[queue[front]-1];
				tempbursts[queue[front]-1] = 0;
				remainings[count] = 0;
			} else {
				timecount += quantum;
				timeorder[count] = quantum;
				tempbursts[queue[front]-1] = tempbursts[queue[front]-1] - quantum;
				remainings[count] = tempbursts[queue[front]-1];
			}
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
			int tempback = back;
			if(tempbursts[queue[front]-1] != 0){
				queue[back] = queue[front];	
				back = (back+1)%processes;
			} else {
				completions[queue[front]-1] = timecount;
			}
			if(tempback != front){
				queue[front] = 0;
			}
			front = (front+1)%processes;
			count++;
		} else {
			downtime[downtimecounter] = timecount;
			timecount++;
			downtimecounter++;
			downtimetimecounter++;
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
							downtimes[downtimeamountcounter] = downtimetimecounter;
							downtimeamountcounter++;
							downtimetimecounter = 0;
						}
					} 
				}
			}
		}
	}
	total = timecount;
	int othertimecounter = 0;
	int gantttimecounter = 0;
	int downtimecountercounter = 0;
	int basepadding = (71-burstamounts)/burstamounts;
	char str2[100];
	for(int i = 0; i < burstamounts; i++){
		for(int j = 0; j < sizeof(downtime)/4; j++){
			if(downtime[j] == othertimecounter){
				printf("|  %*s", basepadding, "");
				othertimecounter += downtimes[downtimecountercounter];
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
		char str[100];
		snprintf(str, sizeof(str), "P%d", order[i]);
		int left = (basepadding - strlen(str))/2;
		int right = basepadding - strlen(str) - left;
		snprintf(str2, sizeof(str2), "%*s%s%*s",left, "", str, right, "");
		printf("|%s",str2);
		othertimecounter += timeorder[i];
	}
	printf("|\n");
	for(int i = 0; i < burstamounts; i++){
		for(int j = 0; j < sizeof(downtime)/4; j++){
			if(downtime[j] == gantttimecounter){
				printf("%d%*s", gantttimecounter, basepadding, "");
				gantttimecounter += downtimes[downtimecountercounter];
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
		char str3[100];
		snprintf(str3, sizeof(str3), "%d", gantttimecounter);
		int padding = strlen(str2) - strlen(str3) + 1;
		printf("%d%*s", gantttimecounter, padding, "");
		gantttimecounter += timeorder[i];
	
	}
	printf("%d\n", gantttimecounter);
	printf("=======================================================================\n"
"QUANTUM AND PREEMPTION INFORMATION\n"
"=======================================================================\n");
int anothercounter = 0;
int preempcounter = 0;
for(int i = 0; i< burstamounts;i++){
	for(int j = 0; j < sizeof(downtime)/4; j++){
			if(downtime[j] == anothercounter){
				anothercounter += downtimes[downtimecountercounter];
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
	if(remainings[i] != 0){
	printf("t=%d : quantum P%d habis (sisa BT=%d) -> READY\n", timeorder[i] + anothercounter, order[i], remainings[i]);
	preempcounter++;
	}
	anothercounter += timeorder[i];
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
int totalwait = 0;
int totaltat = 0;
int totalrt = 0;
for(int i = 0; i<processes; i++){
	totalwait += waits[i] + starts[i] - arrivals[i];
	totalrt += starts[i] - arrivals[i];
	totaltat += completions[i] - arrivals[i];
}
float awt = (float) totalwait / processes;
float atat = (float) totaltat /processes;
float art = (float) totalrt /processes;
int switches = 0;
for(int i = 0; i < burstamounts - 1; i++){
	if(order[i] != order[i+1]){
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
	printf("P%d : NEW -> READY (t=%d) -> ", i+1, arrivals[i]);
	int counttime = 0;
	for(int j = 0; j < burstamounts; j++){
		for(int k = 0; k < sizeof(downtime)/4; k++){
			if(downtime[k] == counttime){
				counttime += downtimes[downtimecountercounter];
				downtimecountercounter = (downtimecountercounter + 1)%downtimeamountcounter;
			}
		}
		if(i+1 == order[j]){
			if(completions[i] - counttime > quantum){
				printf("RUNNING (t=%d) -> READY (t=%d) -> ", counttime, counttime + quantum);
			} else {
				printf("RUNNING (t=%d) -> TERMINATED (t=%d)\n", counttime, completions[i]);
			}
		}
		counttime += timeorder[j];
	}
}

return 0;
}


