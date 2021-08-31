#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

pthread_t pt_rcv, pt_snd;
struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[24];    /* message data */
};
void *pthread_rcv(void *arg);
void *pthread_snd(void *arg);
int main(char argc, char **argv)
{
    unlink("msg.dat");
    int fd = open("msg.dat", O_CREAT);
    if(fd < 0){
        perror("O_CREAT file err");
        return -1;
    }
    key_t key = ftok("msg.dat", 1024);
    if(key == -1){
        perror("ftok err");
        return -1;
    }
    int msgid = msgget(key, IPC_CREAT);
    if(msgid == -1){
        perror("msgget err");
        return -1;
    }

    pthread_create(&pt_rcv, NULL, pthread_rcv, (void *)msgid); 
    pthread_create(&pt_snd, NULL, pthread_snd, (void *)msgid); 

    pthread_join(pt_rcv, NULL);
    pthread_join(pt_snd, NULL);
}



void *pthread_rcv(void *arg)
{   
    struct msgbuf recvbuf = {
        .mtype = 0,

    };
    while(1){
        recvbuf.mtype = 0;
        memset(recvbuf.mtext, 0, sizeof(recvbuf.mtext));
        ssize_t size = msgrcv((int)arg, &recvbuf, sizeof(recvbuf.mtext), recvbuf.mtype, 0);
        printf("recv:\n type = %d, msg = %s\r\n", (int)recvbuf.mtype, recvbuf.mtext);
    }
}



void *pthread_snd(void *arg)
{
    struct msgbuf sndbuf = {
        .mtype = 2,
        .mtext = "this 2 type msg"
    };
    while(1){
        memset(sndbuf.mtext, 0, sizeof(sndbuf.mtext));
        sndbuf.mtype++;
        sprintf(sndbuf.mtext, "this %d type msg", (int)sndbuf.mtype);
        int ret = msgsnd((int)arg, &sndbuf, sizeof(sndbuf.mtext), 0);
        //printf("snd type[%d] msg: %s\r\n", sndbuf.mtype, sndbuf.mtext);
        sleep(1);
    }
}
