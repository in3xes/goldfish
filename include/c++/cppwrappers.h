

long sys_open_wrapper(const char __user *filename, int flags, int mode);

ssize_t sys_read_wrapper(unsigned int fd, char __user *buf, size_t count);

ssize_t sys_write_wrapper(unsigned int fd, const char __user *buf, size_t count);
