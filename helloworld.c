#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init helloworld(void) {
    pr_info("Start of hello world!\n");
    return 0;
}

static void __exit helloworld_exit(void) {
    pr_info("End of the hello world\n");
}

module_init(hellowolrd_init);
module_exit(hellowolrd_exit);
MODULE_AUTHOR("Bi Laizi <2512075115@qq.com>");
MODULE_LICENSE("GPL");
