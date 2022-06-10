#ifndef _TEST_H
#define _TEST_H

#define DEV_FIFO_TYPE 'k'

#define DEV_FIFO_CLEAN _IO(DEV_FIFO_TYPE,0)
#define DEV_FIFO_GETVALUE _IOR(DEV_FIFO_TYPE,1,int)
#define DEV_FIFO_SETVALUE _IOW(DEV_FIFO_TYPE,2,int)

#define TH1_START        1
#define TH2_START        2
#define TH1_TH2_START    12
#define TH1_STOP         3
#define TH2_STOP         4
#define TH1_TH2_STOP     34
#define TH1_PAUSE        5
#define TH2_PAUSE        6
#define TH1_TH2_PAUSE    56
#define TH1_CONTINUE     7
#define TH2_CONTINUE     8
#define TH1_TH2_CONTINUE 78

#define THREAD1_OPER    g_knum == TH1_START || g_knum == TH1_STOP || g_knum == TH1_PAUSE || g_knum == TH1_CONTINUE 

#define THREAD2_OPER    g_knum == TH2_START || g_knum == TH2_STOP || g_knum == TH2_PAUSE || g_knum == TH2_CONTINUE 

#define THREAD1_2_OPER  g_knum == TH1_TH2_START || g_knum == TH1_TH2_STOP || g_knum == TH1_TH2_PAUSE || g_knum == TH1_TH2_CONTINUE 

#define TIMER_ADD        1
#define TIMER_DEL        0

#define NUM_INIT_VAL    1
#define NUM_MAX_VAL     100

#define THREAD_STOP     1
#define THREAD_NO_STOP  0

#define NO_FIRST_START  0
#define FIRST_START     1

#define RET_ERR         -1
#define RET_CRT         0
#endif
