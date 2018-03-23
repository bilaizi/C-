#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int myint = 1;
static int myarr[3] = {0, 1, 2};
static char *mystr = "hello";

module_param(myint, int, S_IRUGO);;
module_param_array(myarr, int,NULL, S_IWUSR|S_IRUSR);
module_param(mystr, charp, S_IRUGO)
    
MODULE_PARM_DESC(myint,"this is my int variable");
MODULE_PARM_DESC(myarr,"this is my array of int");
MODULE_PARM_DESC(mystr,"this is my char pointer variable");
MODULE_INFO(my_field_name, "What eeasy value");

static int __init helloworld_init(void) {
    pr_info("Start of hello world with parameters!\n");
    pr_info("The *myint* parameter: %d\n", myint);
    pr_info("The *myarr* parameter: %d, %d, %d\n", myarr[0], myarr[1], myarr[2]);
    pr_info("The *mystr* parameter: %s\n", mystr);
    return 0;
}

static void __exit helloworld_exit(void) {
    pr_info("End of the hello world\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_AUTHOR("Bi Laizi <2512075115@qq.com>");
MODULE_LICENSE("GPL");
