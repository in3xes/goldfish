/*
 * syscallwrapper.cc
 *
 * Created on: Sept 25, 2012
 * Author: doslab
 */

#include <c++/begin_include.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/socket.h>
#include <asm/uaccess.h>
#include <c++/syscallwrapper_sms.h>
#include <c++/end_include.h>

using namespace SMSWrappers;

SysCallWrapper_sms::SysCallWrapper_sms()
{
	filter = new MessageFilter_sms();
}

SysCallWrapper_sms::~SysCallWrapper_sms()
{
	delete filter;
}

int SysCallWrapper_sms::connect(int fd, const char *socket_name)
{
	filter->add_fd(fd, socket_name, strlen(socket_name));
	return 0;
}

int SysCallWrapper_sms::sendmsg(int fd, const char *msg_buffer, size_t msg_buffer_size)
{
        int error=0;
        if(filter!=NULL)
            error = filter->call_filter(fd, msg_buffer, msg_buffer_size);

        return error;
}

int SysCallWrapper_sms::shutdown(int fd, int how)
{
        filter->remove_fd(fd); 
	return 0;
}

int SysCallWrapper_sms::add_filter(MessageFilter_sms *filter)
{
	this->filter = filter;
	return 0;
}

int SysCallWrapper_sms::remove_filter()
{
	delete filter;
	return 0;
}

MessageFilter_sms::MessageFilter_sms(char *filename, int filename_len, long fd)
{
        this->filename = filename;
        this->filename_len = filename_len;
        this->fd = fd;
}

int MessageFilter_sms::call_filter(int fd, const char *buf, size_t count)
{
        return 0; 
}

int MessageFilter_sms::add_fd(long fd, char *filename, int filename_len)
{
	this->fd = fd;
	this->filename = filename;
	this->filename_len = filename_len;
}

int MessageFilter_sms::remove_fd(long fd)
{
	if(this->fd == fd) {
		this->fd = -1;
		this->filename = NULL;
		this->filename_len = 0;
	}
}

extern "C" {
#define EXPORT(sym) \
extern void sym (void); \
EXPORT_SYMBOL(sym)

extern long sys_sendmsg_internal(int fd, struct user_msghdr __user * msg, unsigned flags);
extern long __sys_sendmsg(int fd, struct user_msghdr __user * msg, unsigned flags);
extern int sys_shutdown_internal(int fd, int how);


SysCallWrapper_sms *wrappers = NULL;
EXPORT_SYMBOL(wrappers);
EXPORT(_ZN8Wrappers14SysCallWrapperD1Ev);
EXPORT(_ZN8Wrappers14SysCallWrapper10add_filterEPNS_13MessageFilterE);
//EXPORT_SYMBOL(_ZTIN8Wrappers13MessageFilterE);
EXPORT(_ZN8Wrappers14SysCallWrapper13remove_filterEv);
EXPORT(_ZN8Wrappers14SysCallWrapperC1Ev);
//EXPORT(__cxa_end_cleanup);
//EXPORT(_ZTVN10__cxxabiv120__si_class_type_infoE);
//EXPORT(_ZdlPv);
//EXPORT(_Znwj);
//EXPORT(__gxx_personality_v0);

int sys_connect_wrapper_sms(int fd, const char *socket_name)
{
	if(wrappers!=NULL)
		wrappers->connect(fd, socket_name);
}

int sys_sendmsg_wrapper_sms(int fd, struct user_msghdr __user *msg, unsigned int flags)
{
    //printk("---------------------------------------------------\n");
    //printk("In sys_sendmsg_wrapper_sms\n");
    //printk("---------------------------------------------------\n");

	if(wrappers!=NULL) {
		struct user_msghdr msg_sys;
		struct iovec iov;
		int iovec_size;
		char *msg_buffer;
		int error = 0;

		iovec_size = sizeof(struct iovec);
		msg_buffer = __kmalloc(iovec_size, GFP_KERNEL); 

		copy_from_user(&msg_sys, msg, sizeof(struct user_msghdr));
		if(msg_sys.msg_iovlen>0) {
		    copy_from_user(&iov, msg_sys.msg_iov, sizeof(struct iovec));
		    copy_from_user(msg_buffer, iov.iov_base, iovec_size);
		    if(wrappers!=NULL)
			    error = wrappers->sendmsg(fd, msg_buffer, iovec_size);
		}

		kfree(msg_buffer);
		if(error<0)
		    return error;
	}
        return sys_sendmsg_internal(fd, msg, flags);
}

int sys_shutdown_wrapper_sms(int fd, int how)
{
        if(wrappers!=NULL) {
            wrappers->shutdown(fd, how);
        }
        return sys_shutdown_internal(fd, how);
}


}
