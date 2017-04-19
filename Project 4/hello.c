#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

MODULE_LICENSE("GPL"); //also keeps the np license warning from printing on insmod
MODULE_AUTHOR("Lakshmanan");
MODULE_DESCRIPTION("A Simple Hello World module");

static int __init hello_init(void) //function names can be changes as of Linux 2.4
{
    printk(KERN_INFO "Hello world!\n");
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);

/*
 * to run the kernel module
 * $ make
 * $ insmod hello.ko #inserts the module into the kernel
 * $ dmesg | tail -1 #prints the message from our module
 * $ rmmod hello.ko #removes the module from the kernel
 * $ dmesg | tail -1 #prints the message from our module after removal
*/