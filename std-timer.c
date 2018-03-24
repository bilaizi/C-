#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

static struct timer_list std_timer;
static void std_timer_callback(struct cn_msg *msg, struct netlink_skb_parms *nsp)
{
	pr_info("%s: %lu: idx=%x, val=%x, seq=%u, ack=%u, len=%d: %s.\n",
	        __func__, jiffies, msg->id.idx, msg->id.val,
	        msg->seq, msg->ack, msg->len,
	        msg->len ? (char *)msg->data : "");
}


static int __init std_timer_init(void) {
    pr_info("Start of hello world!\n");
    return 0;
}

static void __exit std_timer_exit(void) {
    int retval;
    retval = del_timer( &my_timer );
    if(retval)
        pr_info("The timer is still in use...\n");
    pr_info("Timer module unloaded\n");
    return;
}

module_init(std_timer_init);
module_exit(std_timer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bi Laizi <bilaizi1030@126.com>");
MODULE_AUTHOR("Bi Peng <2512075115@qq.com>");
MODULE_DESCRIPTION("std timer Module");
