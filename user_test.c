#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "test.h"

#define CHAR_DEVICE_NODE "/dev/demo_test"

#define THREAD_OPER    (operation == TH1_START || operation == TH1_TH2_START || operation == TH1_STOP || operation == TH1_TH2_STOP || operation == TH1_PAUSE || operation == TH1_TH2_PAUSE || operation == TH1_CONTINUE || operation == TH1_TH2_CONTINUE || operation == TH2_START || operation == TH1_TH2_START || operation == TH2_STOP || operation == TH1_TH2_STOP || operation == TH2_PAUSE || operation == TH1_TH2_PAUSE || operation == TH2_CONTINUE || operation == TH1_TH2_CONTINUE || operation == 0)
int main(int argc ,char const *argv[])
{
    int fd;
    int retval = RET_CRT;
    int operation = 0;
    fd = open("/dev/demo_test",O_RDWR);
    if(fd<0)
    {
        printf("open %s failed!\n",CHAR_DEVICE_NODE);
	return RET_ERR;
    }
   
    /*ioctl*/	
    while(1)
    {
	do{
	      puts("please enter you want operation:");
	      puts("1: thread1 print start   | 2:thread2 print start    | 12: thread1 and thread2 print start (or restart)");
	      puts("3: thread1 print stop    | 4: thread2 print stop    | 34: thread1 and thread2 print stop");
	      puts("5: thread1 print pause   | 6: thread2 print pause   | 56: thread1 and thread2 print pause");
	      puts("7: thread1 print continue| 8: thread2 print continue| 78: thread1 and thread2 print continue");
	      puts("0: exit!\n");
	      scanf("%d",&operation);
	      getchar();
	      printf("your operation is %d.\n",operation);
	}while(!THREAD_OPER);
	if(operation == 0)
	{
	    operation = TH1_TH2_STOP;
            retval = ioctl(fd,DEV_FIFO_SETVALUE,&operation);
	    return RET_CRT;
	}
        retval = ioctl(fd,DEV_FIFO_SETVALUE,&operation);
        if(retval<RET_CRT)
        {
            printf("ioctl failed!\n");
	    return RET_ERR;
        }
        printf("ioctl %s successfully!\n",CHAR_DEVICE_NODE);
    }

    /*close*/
    close(fd);
    return RET_CRT;
}
