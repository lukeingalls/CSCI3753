#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

asmlinkage long sys_cs3753_add(int x, int y, int *result) {
	printk(KERN_INFO "The first number is %d\nThe second number is %d\n", x, y);		
	int res = x + y;
	printk(KERN_NOTICE "The result is %d\n", res);
	printk(KERN_INFO "Storing it at address %p\n", result);
	copy_to_user(result, &res, sizeof(int));

	return 0;
}
