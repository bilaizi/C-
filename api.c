unsigned long copy_from_user(void* to, const void __user* from, unsigned long n)
unsigned long copy_to_user(void __user* to, const void* from, unsigned long n)
ssize_t (*read)(struct file* filp, char __user* buf, size_t count, loff_t* pos);
ssize_t (*write)(struct file* filp, const char __user* buf, size_t count, loff_t* pos)
loff_t  (*llseek)(struct file* filp, loff_t offset, int whence)
unsigned int (*poll) (struct file*, struct poll_table_struct*);
