#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fcntl.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/pagemap.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include "test.h"

struct semaphore sem1;
struct semaphore sem2;

struct timer_list mytimer1;
struct timer_list mytimer2;

struct task_struct *task1 = NULL;
struct task_struct *task2 = NULL;

static int g_knum;                               //kernel receive value
static int g_operation1;                         //user cmd
static int g_operation2;                         
static int g_timer1_flag = TIMER_DEL;            //judge timer is open? 1:open
static int g_timer2_flag = TIMER_DEL;

static int g_number1 = NUM_INIT_VAL;             //number count,init is 0
static int g_number2 = NUM_INIT_VAL;

static int g_first_start_flag1 = NO_FIRST_START; //judge thread first start, 1:first
static int g_first_start_flag2 = NO_FIRST_START;

static int g_num_flag1;                          //record indirect value
static int g_num_flag2;

static int g_last_comm1;                         //record last cmd
static int g_last_comm2;                         

static int g_stop_flag1 = THREAD_STOP;           //judge thread print stop,1:stop
static int g_stop_flag2 = THREAD_STOP;

struct demo_dev{
    struct cdev cdev;
};

struct device *class_dev = NULL;
struct class *cls;

#define DEMO_MAJOR 236
#define DEMO_MINOR 0

static void print1(struct timer_list *data)
{
    if(g_operation1 == TH1_PAUSE || g_operation1 == TH1_TH2_PAUSE)  //pause print
    {
        return;
    }
    else if(g_operation1 == TH1_START || g_operation1 == TH1_TH2_START)
    {
        printk("this is thread1, now g_number1 is %d\n",g_number1);
        g_number1++;
        mod_timer(&mytimer1, jiffies + HZ);
    }
    else if(g_operation1 == TH1_CONTINUE || g_operation1 == TH1_TH2_CONTINUE)
    {
	    g_operation1 = TH1_START;
        mod_timer(&mytimer1,jiffies);
    }
    if(g_number1 == NUM_MAX_VAL)
    {
        g_number1 = NUM_INIT_VAL;
	    printk("g_number1 set 1\n");
    }
    return;
}

static void print2(struct timer_list *data)
{
    if(g_operation2 == TH2_PAUSE || g_operation2 == TH1_TH2_PAUSE) //pause print
    {
        return;
    }
    if(g_operation2 == TH2_START || g_operation2 == TH1_TH2_START)
    {
        printk("this is thread2, now g_number2 is %d\n",g_number2);
        g_number2++;
        mod_timer(&mytimer2, jiffies + 2*HZ);
    }
    else if(g_operation2 == TH2_CONTINUE || g_operation2 == TH1_TH2_CONTINUE)
    {
	    g_operation2 = TH2_START;
        mod_timer(&mytimer2,jiffies);
    }
    if(g_number2 == NUM_MAX_VAL)
    {
        g_number2 = NUM_INIT_VAL;
	    printk("g_number2 set 1\n");
    }
    return;
}
static int thread1(void *arg)
{
    int retval = RET_CRT;
    if(arg == NULL)
    {
	    printk("arg1 is NULL!\n");
        return RET_ERR;
    }
    g_operation1 = *(int*)arg;
    if(g_operation1 == TH1_START || g_operation1 == TH1_TH2_START)
    {
        if(g_last_comm1 == TH1_START)
        {
            retval = down_trylock(&sem1);
        }
        if(g_timer1_flag == TIMER_ADD)                   //if mytimer exist ,restart 
        {
            if(g_last_comm1 == TH1_CONTINUE)
            {
                retval = down_trylock(&sem1);
            }
            up(&sem1);
            g_last_comm1 = TH1_START;
            del_timer(&mytimer1);
            g_timer1_flag = TIMER_DEL;
            add_timer(&mytimer1);
            g_timer1_flag = TIMER_ADD;
            g_number1 = NUM_INIT_VAL;                    //g_number1 init
            g_stop_flag1 = THREAD_NO_STOP;               //thread print is starting
            g_first_start_flag1 = FIRST_START;
            printk("g_number1 set 1\n");
            printk("thread1 restart print!\n");
        }
        else if(g_timer1_flag == TIMER_DEL)
        {
            up(&sem1);
            g_last_comm1 = TH1_START;
            add_timer(&mytimer1);
            g_timer1_flag = TIMER_ADD;
            g_stop_flag1 = THREAD_NO_STOP;
            g_first_start_flag1 = FIRST_START;
            printk("thread1 start print!\n");
        }
    } 
    else if(g_operation1 == TH1_STOP || g_operation1 == TH1_TH2_STOP)
    {
	    if(g_last_comm1 == TH1_PAUSE)
	    {
	        up(&sem1);
	    }
	    retval = down_trylock(&sem1);
	    if(retval != RET_CRT)
	    {
	        printk("thread1 not start print, stop fail!\n");
	        return RET_ERR;
	    }
	    g_last_comm1 = TH1_STOP;
        del_timer(&mytimer1);
	    g_timer1_flag = TIMER_DEL;
	    g_number1 = NUM_INIT_VAL;
	    g_stop_flag1 = THREAD_STOP;
        g_first_start_flag1 = NO_FIRST_START;
	    printk("thread1 stop print!\n");
    }
    else if(g_operation1 == TH1_PAUSE || g_operation1 == TH1_TH2_PAUSE)
    {       
	    if(g_last_comm1 == TH1_PAUSE && g_stop_flag1 == THREAD_NO_STOP)
	    {
	        printk("Thread1 already PAUSE!\n");
	        up(&sem1);
	    }
        retval = down_trylock(&sem1);
        if(retval != RET_CRT)
        {
            printk("thread1 not start print, pause fail!\n");
            return RET_ERR;
        }
        g_last_comm1 = TH1_PAUSE;
        del_timer(&mytimer1);
        g_timer1_flag = TIMER_DEL;
        g_first_start_flag1 = NO_FIRST_START;
        printk("thread1 pause print!\n");
    }
    else if(g_operation1 == TH1_CONTINUE || g_operation1 == TH1_TH2_CONTINUE)
    {
        if(g_first_start_flag1 == FIRST_START)
        {
            g_operation1 = TH1_START;
            g_last_comm1 = TH1_START;
            retval = down_trylock(&sem1);
        }
        if(g_stop_flag1 == THREAD_STOP)
        {
            retval = down_trylock(&sem1);
            if(retval != RET_CRT)
            {
                printk("thread1 not start print, can't continue, please start!\n");
                return RET_ERR;
            }
        }
        if(g_last_comm1 == TH1_CONTINUE)
        {
            retval = down_trylock(&sem1);
        }
        up(&sem1);
        if(g_first_start_flag1 == NO_FIRST_START)
            g_last_comm1 = TH1_CONTINUE;
        add_timer(&mytimer1);
        g_timer1_flag = TIMER_ADD;
        g_first_start_flag1 = NO_FIRST_START;
        printk("thread1 continue print!\n");
    }
    return retval;
}

static int thread2(void *arg)
{
    int retval = RET_CRT;
    if(arg == NULL)
    {
	    printk("arg2 is NULL!\n");
        return RET_ERR;
    }
    g_operation2 = *(int*)arg;
    if(g_operation2 == TH2_START || g_operation2 == TH1_TH2_START)
    {
        if(g_last_comm2 == TH2_START)
        {
            retval = down_trylock(&sem2);
        }
        if(g_timer2_flag == TIMER_ADD)              //if mytimer exist ,restart 
        {
            if(g_last_comm2 == TH2_CONTINUE)
            {
            retval = down_trylock(&sem2);
            }
            up(&sem2);
            g_last_comm2 = TH2_START;
            del_timer(&mytimer2);
            g_timer2_flag = TIMER_DEL;
            add_timer(&mytimer2);
            g_timer2_flag = TIMER_ADD;
            g_number2 = NUM_INIT_VAL;
            g_stop_flag2 = THREAD_NO_STOP;
            g_first_start_flag2 = FIRST_START;
            printk("g_number2 set 1\n");
            printk("thread2 restart print!\n");
        }
        else if(g_timer2_flag == TIMER_DEL)
        {
            up(&sem2);
            g_last_comm2 = TH2_START;
            add_timer(&mytimer2);
            g_timer2_flag = TIMER_ADD;
            g_stop_flag2 = THREAD_NO_STOP;
            g_first_start_flag2 = FIRST_START;
            printk("thread2 start print!\n");
        }
    } 
    else if(g_operation2 == TH2_STOP || g_operation2 == TH1_TH2_STOP)
    {
        if(g_last_comm2 == TH2_PAUSE)
        {
            up(&sem2);
        }
        retval = down_trylock(&sem2);
        if(retval != RET_CRT)
        {
            printk("thread2 not start print, stop fail!\n");
            return RET_ERR;
        }
        g_last_comm2 = TH2_STOP;
        printk("thread2 stop print!\n");
        del_timer(&mytimer2);
        g_timer2_flag = TIMER_DEL;
        g_number2 = NUM_INIT_VAL;
        g_stop_flag2 = THREAD_STOP;
        g_first_start_flag2 = NO_FIRST_START;
    }
    else if(g_operation2 == TH2_PAUSE || g_operation2 == TH1_TH2_PAUSE)
    {   
        if(g_last_comm2 == TH2_PAUSE && g_stop_flag2 == 0)
        {
            printk("Thread2 already PAUSE!\n");
            up(&sem2);
        }
        retval = down_trylock(&sem2);
        if(retval != RET_CRT)
        {
            printk("thread2 not start print, stop fail!\n");
            return RET_ERR;
        }
        g_last_comm2 = TH2_PAUSE;
        del_timer(&mytimer2);
        g_timer2_flag = TIMER_DEL;
        g_first_start_flag2 = NO_FIRST_START;
        printk("thread2 pause print!\n");
    }
    else if(g_operation2 == TH2_CONTINUE || g_operation2 == TH1_TH2_CONTINUE)
    {
        if(g_first_start_flag2 == FIRST_START)
        {
            g_operation2 = TH2_START;
            g_last_comm2 = TH2_START;
            retval = down_trylock(&sem2);
        }
        if(g_stop_flag2 == THREAD_STOP)
        {
            retval = down_trylock(&sem2);
            if(retval != RET_CRT)
            {
                printk("thread2 not start print, can't continue, please start!\n");
                return RET_ERR;
            }
        }
        if(g_last_comm2 == TH2_CONTINUE)
        {
            retval = down_trylock(&sem2);
        }
        up(&sem2);
        if(g_first_start_flag2 == NO_FIRST_START)
            g_last_comm2 = TH2_CONTINUE;
        add_timer(&mytimer2);
        g_timer2_flag = TIMER_ADD;
        g_first_start_flag2 = NO_FIRST_START;
        printk("thread2 continue print!\n");
    }
    return retval;
}
static int demo_release(struct inode *inode,struct file *filp)
{
    printk(KERN_ERR "DEMO:Line %d, function %s() has been invoked!\n",__LINE__,__func__);
    return RET_CRT;
}
static int demo_open(struct inode *inode,struct file *filp)
{
    struct demo_dev *dev;
    printk(KERN_ERR "DEMO:Line %d, function %s() has been invoked!\n",__LINE__,__func__);
    dev = container_of(inode->i_cdev,struct demo_dev,cdev);
    return RET_CRT;
}


static long demo_ioctl(struct file *filp,unsigned int cmd,unsigned long int arg)
{
    long err;
    int ret;
    void __user *argp = (void __user*)arg;
    int __user *p = argp;
    printk(KERN_ERR "DEMO:Line %d, function %s() has been invoked!\n",__LINE__,__func__);
    if(_IOC_TYPE(cmd)!=DEV_FIFO_TYPE)
    {
        pr_err("cmd %u,bad magic 0x%x/0x%x.\n",cmd,_IOC_TYPE(cmd),DEV_FIFO_TYPE);
	return -ENOTTY;
    }
    if(_IOC_DIR(cmd)&_IOC_READ)
    {
        ret = !access_ok((void __user*)arg, _IOC_SIZE(cmd));
    }
    else if(_IOC_DIR(cmd)&_IOC_WRITE)
    {
        ret = !access_ok((void __user*)arg, _IOC_SIZE(cmd));
    }
    if(ret)
    {
        pr_err("bad access %d.\n",ret);
	return -EFAULT;
    }

    switch(cmd)
    {
        case DEV_FIFO_CLEAN:
            printk("DEV_FIFO_CLEAN\n");
	    break;
//      case DEV_FIFO_GETVALUE:
//	        err = put_user(g_knum,p);
//          printk("DEV_FIFO_GETVALUE %d\n",g_knum);
//	    break;
	    case DEV_FIFO_SETVALUE:
	        err = get_user(g_knum,p);       //user->dev
	        printk("DEV_FIFO_SETVALUE1 %d\n",g_knum);
	    break;
	    default:
	        return -EINVAL;
    }
    if(THREAD1_OPER)
    {
        task1 = kthread_run(thread1,(void*)&g_knum,"task1");
	if(g_timer2_flag == 1)
	{
	    g_num_flag2 = TH2_CONTINUE;
	    task2 = kthread_run(thread2,(void*)&g_num_flag2,"task2");
	}
    }
    else if(THREAD2_OPER)
    {
        task2 = kthread_run(thread2,(void*)&g_knum,"task2");
	if(g_timer1_flag == TIMER_ADD)
	{    
	    g_num_flag1 = TH1_CONTINUE;
	    task1 = kthread_run(thread1,(void*)&g_num_flag1,"task1");
	}
    }
    else if(THREAD1_2_OPER)
    {
        task1 = kthread_run(thread1,(void*)&g_knum,"task1");
        task2 = kthread_run(thread2,(void*)&g_knum,"task2"); 
    }
    return RET_CRT; 
}

static struct file_operations demo_fops = 
{
    .owner = THIS_MODULE,
    .open = demo_open,
    .release = demo_release,
    .unlocked_ioctl = demo_ioctl,
};

static int __init hello_init(void)
{
    int retval = 0;
    dev_t devno = 0;
    printk(KERN_ERR "DEMO:Line %d, function %s() has been invoked!\n",__LINE__,__func__);
    retval = register_chrdev(DEMO_MAJOR,"demo_test",&demo_fops);   //register char device
    if(retval<0)
    {
        printk("register_chrdev fail \n");
        return retval;
    }
    cls = class_create(THIS_MODULE,"demo_testcls");                //Create a device category file
    if(IS_ERR(cls))
    {
        printk(KERN_ERR "class_create() failed for cls\n");
        retval = PTR_ERR(cls);
        goto out_err_2;
    }
    devno = MKDEV(DEMO_MAJOR,DEMO_MINOR);                          //Convert the primary and secondary device numbers to dev_t type      
    class_dev = device_create(cls,NULL,devno,NULL,"demo_test");    //create device file
    if(IS_ERR(class_dev))
    {
        printk(KERN_ERR "device_create() failed\n");
        retval = PTR_ERR(class_dev);
        goto out_err_1;
    }
    sema_init(&sem1,0);
    sema_init(&sem2,0);
    timer_setup(&mytimer1,print1,0);
    timer_setup(&mytimer2,print2,0);
//   mytimer1.expires = jiffies + HZ;
//   mytimer2.expires = jiffies + 2*HZ;
    return retval;

out_err_2:
    class_destroy(cls);
out_err_1:
    unregister_chrdev(DEMO_MAJOR,"demo_test");
    return retval;
}

static void __exit hello_exit(void)
{
    dev_t devno = MKDEV(DEMO_MAJOR, DEMO_MINOR);
    if(g_timer1_flag)
    {
        del_timer(&mytimer1);
	    g_timer1_flag = TIMER_DEL;
    }
    if(g_timer2_flag)
    {
        del_timer(&mytimer2);
	    g_timer2_flag = TIMER_DEL;
    }
    printk(KERN_ERR "DEMO:Line %d, function %s() has been invoked!\n",__LINE__,__func__);
    device_destroy(cls,devno);
    class_destroy(cls);
    unregister_chrdev(DEMO_MAJOR,"demo_test");
    return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");

