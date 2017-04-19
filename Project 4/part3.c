#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

MODULE_LICENSE("GPL"); //also keeps the np license warning from printing on insmod
MODULE_AUTHOR("Citadel");
MODULE_DESCRIPTION("I think we are basically reading PCBs.");

static int pid = 0; //what is the pid of a kernel process? if you don't give it an argument pid will be 0. I wonder what'll happen
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "we will be taking pid as an argument");

static int __init hello_init(void) //function names can be changes as of Linux 2.4
{
    printk(KERN_INFO "Hello world!\n");
    printk(KERN_INFO "Ellie says the pid is: %hd\n", pid);
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);