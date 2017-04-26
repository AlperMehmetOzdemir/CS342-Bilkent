//celik koseoglu
//cs342 - project 4

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/fdtable.h>
#include <linux/fs_struct.h>


struct task_struct *task;
struct fdtable *files_table;
struct path files_path;

int open_file_count;

struct vm_area_struct *vma; //will be used to iterate along virtual memory blocks
struct list_head *cas; //will be used to iterate along children and sibling processes

int i = 3; //counter for loop ops. first 3 files open the standard input, standard output and the standard error with file descriptors 0,1,2 respectively. we don't want them. so start from 3

static int pid = 0; //what is the pid of a kernel process? if you don't give it an argument pid will be 0. I wonder what'll happen
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "pid of the process to inspect");

int proc_init (void) {

	//FIND THE TASK STRUCT USING PID
	task = pid_task(find_vpid(pid), PIDTYPE_PID);

	//PRINT INFORMATION ABOUT THE PROCESS
	printk(KERN_INFO "Process: [%s] with pid: [%d]\n",task->comm, task->pid);

	//PRINT INFORMATION ABOUT ITS PARENT PROCESS (IF HAS ANY)
	if (task->parent != NULL)
		printk(KERN_INFO "Parent Process: [%s] with pid: [%d]\n",task->parent->comm, task->parent->pid);
	else
		printk(KERN_INFO "No parent process\n");

	//PRINT INFORMATION ABOUT THE CHILDREN PROCESS (IF HAS ANY)

    //read_lock (&tasklist_lock);

	struct list_head *temp;
	struct task_struct *my;

	list_for_each(temp, &task->children) { //the first pointer is used to point to the current entry. it is a temp value which changes with every iteration

        my = list_entry(temp, struct task_struct, sibling);
        printk(KERN_INFO "Child Process: [%s] with pid: [%d]\n",my->comm, my->pid);
	}

	//read_unlock (&tasklist_lock);

	//LIST OPEN FILES
	open_file_count = atomic_read(&(task->files->count));
	if (task->files == NULL)
		printk(KERN_INFO "Files are null\n");

	printk(KERN_INFO "File count of the process: %d\n", open_file_count);

	while (i < open_file_count) {
		char *buf = (char *)kmalloc(100*sizeof(char), GFP_KERNEL); //who decided to change parameter places here? took me hours to figure out
		if (!buf)
			printk(KERN_INFO "Cannot allocate space on kernel memory to write open file paths.\n");
		else { //f.ed my way up to the top.
			if (task->files->fd_array[i]->f_path.dentry == NULL)
				printk(KERN_INFO "Dentry null.\n");
			else {
				char *path_of_file = dentry_path_raw(task->files->fd_array[i]->f_path.dentry, buf, 100*sizeof(char));
				if (strlen(path_of_file) > 1) {
					printk(KERN_INFO "open file at path: [%s]\n", path_of_file);
					struct kstat ks;
					vfs_getattr(&task->files->fd_array[i]->f_path, &ks);
					printk(KERN_INFO "size of file: %lld bytes\n", ks.size);
					printk(KERN_INFO "fist block address in dec: %lld\n", ks.blocks);
					printk(KERN_INFO "block size: %lu bytes\n", ks.blksize);
				}
				
			}
		}
		i++;
	}

	//VIRTUAL MEMORY LAYOUT
	unsigned long total_memory_in_bytes = 0;
	int first = 0;
	if (task->mm && task->mm->mmap) //I wonder if it is possible for these to be null though...
    	for (vma = task->mm->mmap; vma; vma = vma->vm_next) {
    		total_memory_in_bytes += vma->vm_end - vma->vm_start;
    		//uncomment the following lines to for plain annoyance
			if (first == 0) {
				printk(KERN_INFO "There are too many blocks. Printing information about just the first one to avoid confusion. Check report about this.");
    			printk(KERN_INFO "Start address of block in dec: %lu\n", vma->vm_start);
    			printk(KERN_INFO "End address of block in dec: %lu\n", vma->vm_end);
    			printk(KERN_INFO "Total size of block: %luK\n", vma->vm_end - vma->vm_start);
    			first = 1;
			}
		}

    printk(KERN_INFO "Total memory used by process: %luK\n", total_memory_in_bytes / 1024); //divide by 2¹⁰ to get in KB

    return 0;
}

void proc_cleanup(void) {
    printk(KERN_INFO "Ellie is exiting\n");
}

MODULE_LICENSE("GPL");   
module_init(proc_init);
module_exit(proc_cleanup);
