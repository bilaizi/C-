#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int a = 1;
static int b[3] = {0, 1, 2};
static char *c = "hello world!";

module_param(a, int, S_IRUGO);;
module_param_array(b, int,NULL, S_IWUSR|S_IRUSR);
module_param(c, charp, S_IRUGO)
    
MODULE_PARM_DESC(a,"this is a int variable");
MODULE_PARM_DESC(b,"this is a array of int");
MODULE_PARM_DESC(c,"this is a char pointer variable");
MODULE_INFO(my_field_name, "What eeasy value");

static int __init helloworld_init(void) {
    pr_info("Start of hello world with parameters!\n");
    pr_info("The *a* parameter: %d\n", a);
    pr_info("The *b* parameter: %d, %d, %d\n", b[0], b[1], b[2]);
    pr_info("The *c* parameter: %s\n", c);
    return 0;
}

static void __exit helloworld_exit(void) {
    pr_info("End of the hello world\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_AUTHOR("Bi Laizi <2512075115@qq.com>");
MODULE_LICENSE("GPL");

/*
$ modinfo ./helloworld-params.ko
//To load the hellomodule-params.ko module and feed our parameter, we do the following:
insmod hellomodule-params.ko a=1 b=0,1,2 c="hello world!"
*/
