#define _GNU_SOURCE
#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
using namespace std;

typedef void (*spawn_proc_pt)(void *data);
static void start_worker_processes(int n);
//pid_t spawn_process(spawn_proc_pt proc, void *data, char* name);
pid_t spawn_process(void *data, char* name);
static void worker_process_cycle(void *data);

int main(int argc, char const *argv[])
{
    start_worker_processes(4);
    wait(NULL);
    return 0;
}


void start_worker_processes(int n)
{
    int i = 0;
    for(i=0; i<n; i++) {
        //spawn_process(worker_process_cycle, (void*)(intptr_t) i, "worker process");
        spawn_process((void*)(intptr_t) i, "worker process");
    }
}

spawn_process((void*) data, char* name)
{
    pid_t pid;
    pid = fork();

    switch(pid){
        case -1:
            fprintf(stderr, "fork() failed while spawning \"%s\"\n", name);
            return -1;
        case 0:
            worker_process_cycle(data);
            return 0;
        default:
            break;
    }
    printf("start %s %ld\n",name,(long int)pid);
    return pid;
}

/*
pid_t spawn_process(spawn_proc_pt proc, void *data, char* name)
{
    pid_t pid;
    pid = fork();

    switch(pid){
        case -1:
            fprintf(stderr, "fork() failed while spawning \"%s\"\n", name);
            return -1;
        case 0:
            proc(data);
            return 0;
        default:
            break;
    }
    printf("start %s %ld\n",name,(long int)pid);
    return pid;
}
*/

static void worker_process_init(int worker)
{
    cpu_set_t cpu_affinity;
    
    CPU_ZERO(&cpu_affinity);
    CPU_SET(worker % CPU_SETSIZE, &cpu_affinity);
    if(sched_setaffinity(0, sizeof(cpu_set_t), &cpu_affinity) == -1)
    {
        sprintf(stderr, "sched_setaffinity() failed\n");
    }
}

void worker_process_cycle(void *data)
{
    int worker = (intptr_t) data;
    worker_process_init(worker);
    while(1){
        //epoll
        sleep(1);
        printf("pid %ld, doing ...\n",(long int getpid()));
    }
}