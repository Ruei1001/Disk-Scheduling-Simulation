#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define request 1000
int disk_head;


int request_queue[request];
typedef struct {
    volatile int lock;
    volatile int reader_count;
} Mutex;

struct mapped
{
    pthread_mutex_t mutex; 
};
#define MMAP_NAME "testmap"
#define LENGTH sizeof(struct mapped)

void FCFS(struct mapped * mapping);
void SSTF(struct mapped * mapping);
void SCAN(struct mapped * mapping);
void CSCAN(struct mapped * mapping);
void LOOK(struct mapped * mapping);
void CLOOk(struct mapped * mapping);
void OPT(struct mapped * mapping);

int find_min();
int find_max();

int main(int argc, char ** argv)
{
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();

    //int n, rc;
    //int rc;
    disk_head = atoi(argv[1]);
    printf("Disk head at : %d \n", disk_head);
    unlink(MMAP_NAME);
    //int fd = open(MMAP_NAME, O_CREAT|O_RDWR, 00600);
    //rc = ftruncate(fd, LENGTH);

    struct mapped *mapping = (struct mapped *)mmap(NULL, LENGTH, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);

    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mapping->mutex, &mutexattr);

    srand(time(NULL));
    for(int i = 0; i<request; i++)
    {
        request_queue[i] = rand()%5000;
    }
    disk_head = rand()%5000;
    pid_t c1,c2,c3,c4,c5,c6,c7;
    (c1 = fork())&&(c2 = fork())&&(c3 = fork())&&(c4 = fork())&&(c5 = fork())&&(c6 = fork())&&(c7 = fork());
    if(c1 == 0)
        FCFS(mapping);
    else if(c2 == 0)
        SSTF(mapping);
    else if(c3 == 0)
        SCAN(mapping);
    else if(c4 == 0)
        CSCAN(mapping);
    else if(c5 == 0)
        LOOK(mapping);
    else if(c6 == 0)
        CLOOk(mapping);
    else if(c7 == 0)
        OPT(mapping);
    else
    {
        for (int i = 0; i< 7; i++)
        {
            waitpid(-1,NULL,0);
        }
        end = clock();
        cpu_time_used = ((double) (end - start));
        printf("CPU used time : %f\n", cpu_time_used);
    }

}

int find_min()
{
    int min = 5000;
    int index;
    for (int i = 0; i<request; i++)
    {  
        if (request_queue[i] < min)
        {
            min = request_queue[i];
            index = i;
        }
    }
    return index;
}

int find_max()
{
    int max = 0;
    int index;
    for (int i = 0; i<request; i++)
    {  
        if (request_queue[i] > max)
        {
            max = request_queue[i];
            index = i;
        }
    }
    return index;
}

void FCFS(struct mapped * mapping)
{
    
    int head = disk_head;
    int count = 0;
    for (int i = 0; i<request; i++)
    {
        count += abs(head - request_queue[i]);
        head = request_queue[i];
    }
    

    pthread_mutex_lock(&mapping->mutex);
    printf("FCFS moved : %f times\n", count/100.0 );
    pthread_mutex_unlock(&mapping->mutex);
}
void SSTF(struct mapped * mapping)
{
    int req[5000] = {0};
    int count = 0;
    for (int i = 0; i<request; i++)
    {
        req[request_queue[i]]++;
    }
    int head = disk_head;
    if (req[head] > 0)
    {
        req[head]=0;
    }

    while(1)
    {
        int left, right;
        left = head-1;
        right = head+1;
        while(left >= 0)
        {
            if (req[left] > 0)
            {
                req[left]=0;
                break;
            }
            else
            {
                left--;
            }
        }
        while(right < 5000)
        {
            if (req[right] > 0)
            {
                req[right]=0;
                break;
            }
            else
            {
                right++;
            }
        }
        if (left < 0 && right >= 5000)
            break;
        if(right > 4999)
        {
            count = count + abs(head-left);
            head = left;
        }
        else if (left <0)
        {
            count = count + abs(head-right);
            head = right;
        }
        else if (right-head > head-left && left >= 0)
        {
            count = count + abs(head-left);
            head = left;
        }
        else
        {
            count = count + abs(right-head);
            head = right;
        }
    }
    pthread_mutex_lock(&mapping->mutex);
    printf("SSTF %f time\n", count/100.0);
    pthread_mutex_unlock(&mapping->mutex);
}
void SCAN(struct mapped * mapping)
{
    int req[5000] = {0};
    int count = 0;
    int move = 0;
    for (int i = 0; i<request; i++)
    {
        req[request_queue[i]]++;
    }
    int head = disk_head;
    if (req[head] > 0)
    {
        req[head] = 0;
    }
    while (head-1 >= 0)
    {
        head--;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    while (head + 1 < 5000)
    {
        head++;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    pthread_mutex_lock(&mapping->mutex);
    printf("SCAN moved : %f time\n", count/100.0);
    pthread_mutex_unlock(&mapping->mutex);
    //killpg(getpid(),SIGTERM);
}
void CSCAN(struct mapped * mapping)
{
    int req[5000] = {0};
    int count = 0;
    int move = 0;
    for (int i = 0; i<request; i++)
    {
        req[request_queue[i]]++;
    }
    int head = disk_head;
    if (req[head] > 0)
    {
        req[head] = 0;
    }
    while (head + 1 < 5000)
    {
        head++;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    head = 0;
    count += 4999;
    while(head+1 < disk_head)
    {
        head++;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    pthread_mutex_lock(&mapping->mutex);
    printf("CSCAN moved : %f time\n", count/100.0);
    pthread_mutex_unlock(&mapping->mutex);
    //killpg(getpid(),SIGTERM);
}
void LOOK(struct mapped * mapping)
{
    int req[5000] = {0};
    int count = 0;
    int move = 0;
    int max = request_queue[find_max()];
    int min = request_queue[find_min()];
    for (int i = 0; i<request; i++)
    {
        req[request_queue[i]]++;
    }
    int head = disk_head;
    if (head < min)
    {
        head = min;
        count = count + min-head;
    }

    if( head > max)
    {
        head = max;
        count = count + head - max;
    }

    if (req[head] > 0)
    {
        req[head] = 0;
    }

    while (head-1 >= min)
    {
        head--;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    while (head + 1 <= max)
    {
        head++;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    pthread_mutex_lock(&mapping->mutex);
    printf("LOOK moved : %f time\n", count/100.0);
    pthread_mutex_unlock(&mapping->mutex);
    //killpg(getpid(),SIGTERM);
}
void CLOOk(struct mapped * mapping)
{
    int req[5000] = {0};
    int count = 0;
    int move = 0;
    int max = request_queue[find_max()];
    int min = request_queue[find_min()];
    for (int i = 0; i<request; i++)
    {
        req[request_queue[i]]++;
    }
    int head = disk_head;
    if (head < min)
    {
        head = min;
        count = count + min-head;
    }

    if( head > max)
    {
        head = max;
        count = count + head - max;
    }

    if (req[head] > 0)
    {
        req[head] = 0;
    }

    while (head + 1 < max)
    {
        head++;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    head = min;
    count = count + max-min;
    move = 0;
    while (head-1 >= min)
    {
        head++;
        move++;
        if (req[head] > 0)
        {
            count += move;
            move = 0;
            req[head] = 0;
        }
    }
    
    pthread_mutex_lock(&mapping->mutex);
    printf("CLOOK moved : %f time\n", count/100.0);
    pthread_mutex_unlock(&mapping->mutex);
    //killpg(getpid(),SIGTERM);
}
void OPT(struct mapped * mapping)
{
    int req[5000] = {0};
    int count = 0;
    int move = 0;
    int max = request_queue[find_max()];
    int min = request_queue[find_min()];
    for (int i = 0; i<request; i++)
    {
        req[request_queue[i]]++;
    }
    int head = disk_head;

    if (abs(max-head) < abs(head-min))
    {
        /*move right*/
        if (head > max)
        {
            head = max;
            count = count + head-max;
        }

        if (req[head] > 0)
        {
            req[head] = 0;
        }

        while (head + 1 < max)
        {
            head++;
            move++;
            if (req[head] > 0)
            {
                count += move;
                move = 0;
                req[head] = 0;
            }
        }
        move = 0;
        while (head-1 >= min)
        {
            head--;
            move++;
            if (req[head] > 0)
            {
                count += move;
                move = 0;
                req[head] = 0;
            }
        }

    }
    else
    {
        /*move left*/
        if (head < min)
        {
            head = min;
            count = count + min-head;
        }

        if (req[head] > 0)
        {
            req[head] = 0;
        }

        while (head - 1 >= min)
        {
            head--;
            move++;
            if (req[head] > 0)
            {
                count += move;
                move = 0;
                req[head] = 0;
            }
        }
        move = 0;
        while (head+1 <= max)
        {
            head++;
            move++;
            if (req[head] > 0)
            {
                count += move;
                move = 0;
                req[head] = 0;
            }
        }

    }
    pthread_mutex_lock(&mapping->mutex);
    printf("Optimized moved : %f times\n", count/100.0);
    pthread_mutex_unlock(&mapping->mutex);
    //killpg(getpid(),SIGTERM);
}