#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/workqueue.h>

static DECLARE_WAIT_QUEUE_HEAD(waitqueue);
static int condition = 0;
/* declare a work queue*/
static struct work_struct work;

static void work_handler(struct work_struct *work) { 
    pr_info("Waitqueue module handler %s\n", __FUNCTION__);
    msleep(3000);
    pr_info("Wake up the sleeping process\n");
    condition = 1;
    wake_up_interruptible(&waitqueue);
}

static int __init waitqueue_init(void) {
    pr_info("Wait queue init\n");
    INIT_WORK(&work, work_handler);
    schedule_work(&work);
    pr_info("Going to sleep %s\n", __FUNCTION__);
    wait_event_interruptible(waitqueue, condition != 0);
    pr_info("woken up by the work job\n");
    return 0;
}

void waitqueue_exit(void) {
        pr_info("waitq ueue cleanup\n");
}

module_init(waitqueue_init);
module_exit(waitqueue_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bi Laizi <bilaizi1030@126.com>");
MODULE_AUTHOR("Bi Peng <2512075115@qq.com>");
MODULE_DESCRIPTION("wait queue Module");
