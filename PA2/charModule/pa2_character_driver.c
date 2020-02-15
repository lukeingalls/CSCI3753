

#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 1024
#define DEVICE_NAME "pa2_char_driver"
#define MAJOR_NUMBER 245

MODULE_LICENSE("GPL");
MODULE_AUTHOR("luin3949@colorado.edu");


/* Define device_buffer and other global data structures you will need here */
static char *device_buffer;
static int open_count = 0;
static int close_count = 0;

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	int max = BUFFER_SIZE - *offset;
	int bytes_to_read = (max >= length) ? length : max;
	int bytes_read = bytes_to_read - copy_to_user(buffer, device_buffer, bytes_to_read);
	*offset += bytes_read;
	printk(KERN_ALERT "The device driver read %d bytes. The offset is now %d.\n", bytes_read, *offset);
	return bytes_read;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	int max = BUFFER_SIZE - *offset;
	int bytes_to_write = (max >= length) ? length : max;
	int bytes_write = bytes_to_write - copy_from_user(buffer, device_buffer, bytes_to_write);
	*offset += bytes_write;
	printk(KERN_ALERT "The device driver read %d bytes. The offset is now %d.\n", bytes_write, *offset);
	return bytes_write;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "The device driver is open!\n");
	open_count++;
	printk(KERN_ALERT "The device driver has been opened %d times!\n", open_count);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "The device driver is closed!\n");
	close_count++;
	printk(KERN_ALERT "The device driver has been closed %d times!\n", close_count);
	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t new_offset = 0;

	switch(whence) {
		case 0: //SEEK_SET_
			new_offset = offset;
			break;
		case 1: //SEEK_CUR
			new_offset = pfile->f_pos + offset;
			break;
		case 2: //SEEK_END
			new_offset = BUFFER_SIZE - offset;
			break;
	}
	if (new_offset < 0 || new_offset > BUFFER_SIZE) {
		printk(KERN_ALERT "ERROR: The offset value described is not within the buffer! No changes made.");
	} else {
		printk(KERN_ALERT "The offset is now %d", new_offset);
		pfile->f_pos = new_offset;
	}
	return 0;
}

struct file_operations pa2_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
	.write = pa2_char_driver_write,
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.read = pa2_char_driver_read,
	.llseek = pa2_char_driver_seek
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	int result = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &pa2_char_driver_file_operations);
	
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
	kfree(device_buffer);
}

/* add module_init and module_exit to point to the corresponding init and exit function*/

module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
