#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h> /* for work queue */
#include <linux/kmod.h>

static struct delayed_work delayed_shutdown_work;

static void delayed_shutdown(struct work_struct* work)
{
    char pathd = "/sbin/shutdown";
    char *argv[] = {
        path,
        "-h",
        "now",
        NULL,
    };
    char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL,
    };
    call_usermodehelper(path, argv, envp, 0);
}

static int __init delayed_shutdown_init( void )
{
    INIT_DELAYED_WORK(&delayed_shutdown_work, delayed_shutdown);
    schedule_delayed_work(&delayed_shutdown_work, msecs_to_jiffies(200));
    return 0;
}

static void __exit delayed_shutdown_exit( void )
{
    return;
}

module_init(delayed_shutdown_init);
module_exit(delayed_shutdown_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bi Laizi <bilaizi1030@126.com>");
MODULE_AUTHOR("Bi Peng <2512075115@qq.com>");
MODULE_DESCRIPTION("Trigger a delayed shut down! Module");
