#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define PROCESSINFO 335
#define DEST_SIZE 10



//obj-y :=  SystemCall.o
//core-y        += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ mysyscalls/
//335    common  SystemCall        __x64_sys_processinfo
//asmlinkage long sys_processinfo(char __user *, char __user *);


int main(int argc, char *argv[])
{
    char *dest;
    char *src;
    dest = (char *)malloc(10);
    src = (char *)malloc(10);

    if (argc == 1)
    {
        printf("Right Usage:\n
            \t-all prints some information (process id and its\n
            \targument/s) about all processes\n
            \t-p takes process id and prints the details of it\n
            \t-k takes process id");
        strcpy(src, "1");
        strcpy(dest, "1");
        syscall(PROCESSINFO, src, dest);
    }
    else if (argv[1][1] == 'a')
    {
        strcpy(src, "2");
        strcpy(dest, "2");
        printf("-all function starting please you see dmesg or kernel buffer ");
        syscall(PROCESSINFO, src, dest);
    }
    else if (argv[1][1] == 'p')
    {
        strcpy(src, argv[2]);
        strcpy(dest, "3");
         printf("-p function starting please you see dmesg or kernel buffer ");
         ret = syscall((PROCESSINFO,src,dest);
    }
    else if (argv[1][1] == 'k')
    {
        printf("-k function starting please you see dmesg or kernel buffer ");
        strcpy(src, argv[2]);
        strcpy(dest, "4");
        syscall(PROCESSINFO, src, dest);
    }
    else
    {
        printf("Error Message");
    }
    return 0;
}
