#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init helloworld_init(void) {
    pr_info("Start of hello world!\n");
    return 0;
}

static void __exit helloworld_exit(void) {
    pr_info("End of the hello world\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bi Laizi <bilaizi1030@126.com>");
MODULE_AUTHOR("Bi Peng <2512075115@qq.com>");
MODULE_DESCRIPTION("Hello, world! Module");
/*
$ cat /proc/sys/kernel/printk
4	4	1	7
$ 
# echo <level> > /proc/sys/kernel/printk
Makefile
obj-m := helloworld.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
all default: modules
install: modules_install
modules modules_install help clean:
    $(MAKE) -C $(KERNELDIR) M=$(shell pwd) $@
*/
