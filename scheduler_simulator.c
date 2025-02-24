#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MAX_PROCESSES 10

typedef struct
{
    int pid;             // Process ID
    int arrival_time;    // Arrival time
    int burst_time;      // Burst time
    int priority;        // Priority (lower number = higher priority)
    int waiting_time;    // Waiting time
    int turnaround_time; // Turnaround time
    int remaining_time;  // Remaining time (for Round Robin)
} Process;

Process processes[MAX_PROCESSES];
int n;

// Function to input process details
void inputProcesses()
{
    printf("Enter the number of processes : ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        printf("\nEnter details for process %d :\n", i + 1);
        processes[i].pid = i + 1;
        printf("Arrival Time : ");
        scanf("%d", &processes[i].arrival_time);
        printf("Burst Time : ");
        scanf("%d", &processes[i].burst_time);
        printf("Priority : ");
        scanf("%d", &processes[i].priority);
        processes[i].remaining_time = processes[i].burst_time;
    }
}

// Function to display results
void displayResults()
{
    printf("\nPID\tArrival\tBurst\tPriority\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++)
    {
        printf("%d\t%d\t%d\t%d\t\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority,
               processes[i].waiting_time,
               processes[i].turnaround_time);
    }
}

// Function to display Gantt Chart
void displayGanttChart(int order[], int count)
{
    printf("\nGantt Chart:\n");
    for (int i = 0; i < count; i++)
    {
        if (order[i] == -1)
        {
            printf("IDLE | ");
        }
        else
        {
            printf("P%d | ", processes[order[i]].pid);
        }
    }
    printf("\n");
}

// FCFS Scheduling Algorithm
void fcfs(int gantt_order[], int *gantt_count)
{
    int current_time = 0;
    *gantt_count = 0;

    for (int i = 0; i < n; i++)
    {
        if (current_time < processes[i].arrival_time)
        {
            gantt_order[(*gantt_count)++] = -1; // IDLE time
            current_time = processes[i].arrival_time;
        }
        gantt_order[(*gantt_count)++] = i;
        processes[i].waiting_time = current_time - processes[i].arrival_time;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
        current_time += processes[i].burst_time;
    }
}

// SJF Scheduling Algorithm
void sjf(int gantt_order[], int *gantt_count)
{
    int current_time = 0;
    int completed = 0;
    *gantt_count = 0;

    while (completed < n)
    {
        int shortest = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if (shortest == -1 || processes[i].burst_time < processes[shortest].burst_time)
                {
                    shortest = i;
                }
            }
        }
        if (shortest == -1)
        {
            gantt_order[(*gantt_count)++] = -1; // IDLE time
            current_time++;
        }
        else
        {
            gantt_order[(*gantt_count)++] = shortest;
            processes[shortest].waiting_time = current_time - processes[shortest].arrival_time;
            processes[shortest].turnaround_time = processes[shortest].waiting_time + processes[shortest].burst_time;
            current_time += processes[shortest].burst_time;
            processes[shortest].remaining_time = 0;
            completed++;
        }
    }
}

// Round Robin Scheduling Algorithm
void roundRobin(int gantt_order[], int *gantt_count, int time_quantum)
{
    int current_time = 0;
    int completed = 0;
    *gantt_count = 0;

    while (completed < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                int execute_time = (processes[i].remaining_time > time_quantum) ? time_quantum : processes[i].remaining_time;
                gantt_order[(*gantt_count)++] = i;
                processes[i].remaining_time -= execute_time;
                current_time += execute_time;

                if (processes[i].remaining_time == 0)
                {
                    completed++;
                    processes[i].turnaround_time = current_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                }
            }
        }
    }
}

// Priority Scheduling Algorithm
void priorityScheduling(int gantt_order[], int *gantt_count)
{
    int current_time = 0;
    int completed = 0;
    *gantt_count = 0;

    while (completed < n)
    {
        int highest_priority = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if (highest_priority == -1 || processes[i].priority < processes[highest_priority].priority)
                {
                    highest_priority = i;
                }
            }
        }
        if (highest_priority == -1)
        {
            gantt_order[(*gantt_count)++] = -1; // IDLE time
            current_time++;
        }
        else
        {
            gantt_order[(*gantt_count)++] = highest_priority;
            processes[highest_priority].waiting_time = current_time - processes[highest_priority].arrival_time;
            processes[highest_priority].turnaround_time = processes[highest_priority].waiting_time + processes[highest_priority].burst_time;
            current_time += processes[highest_priority].burst_time;
            processes[highest_priority].remaining_time = 0;
            completed++;
        }
    }
}

// Multithreading function to simulate process execution
void *executeProcess(void *arg)
{
    Process *p = (Process *)arg;
    printf("Process %d is running...\n", p->pid);
    sleep(p->burst_time); // Simulate execution time
    printf("Process %d has completed.\n", p->pid);
    pthread_exit(NULL);
}

// Function to simulate processes using multithreading
void simulateProcesses()
{
    pthread_t threads[MAX_PROCESSES];
    for (int i = 0; i < n; i++)
    {
        pthread_create(&threads[i], NULL, executeProcess, (void *)&processes[i]);
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

// Function to reset remaining time for all processes
void resetRemainingTime()
{
    for (int i = 0; i < n; i++)
    {
        processes[i].remaining_time = processes[i].burst_time;
    }
}

// Main function
int main()
{
    int choice;
    int time_quantum;
    int gantt_order[100]; // Array to store the order of processes in the Gantt chart
    int gantt_count;      // Number of entries in the Gantt chart

    inputProcesses();

    while (1)
    {
        printf("\n--- Process Scheduler Simulator ---\n");
        printf("1. FCFS\n");
        printf("2. SJF\n");
        printf("3. Round Robin\n");
        printf("4. Priority Scheduling\n");
        printf("5. Exit\n");
        printf("Select an option : ");
        scanf("%d", &choice);

        if (choice == 5)
        {
            printf("Exiting the program. Goodbye!\n");
            break;
        }

        switch (choice)
        {
        case 1:
            fcfs(gantt_order, &gantt_count);
            break;
        case 2:
            sjf(gantt_order, &gantt_count);
            break;
        case 3:
            printf("Enter time quantum for Round Robin : ");
            scanf("%d", &time_quantum);
            roundRobin(gantt_order, &gantt_count, time_quantum);
            break;
        case 4:
            priorityScheduling(gantt_order, &gantt_count);
            break;
        default:
            printf("Invalid choice! Please try again.\n");
            continue;
        }

        displayResults();
        displayGanttChart(gantt_order, gantt_count);
        printf("\nSimulating processes using multithreading...\n");
        simulateProcesses();

        // Reset remaining time for all processes
        resetRemainingTime();
    }

    return 0;
}