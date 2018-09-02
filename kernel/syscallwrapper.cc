/*
 * syscallwrapper.cc
 *
 * Created on: Sept 25, 2012
 * Author: doslab
 */

#include <c++/begin_include.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
//#include <linux/gfp.h>
#include <linux/socket.h>
//#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/kconfig.h>
#include <c++/syscallwrapper.h>
/*#include <asm/current.h>
#include <linux/fdtable.h>
#include <linux/fs_struct.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/sched.h>
#include <linux/rbtree.h>*/
#include <c++/end_include.h>

using namespace Wrappers;

//char* getFilename(unsigned int fd);


SysCallWrapper::SysCallWrapper()
{
	filter = new MessageFilter();
}

SysCallWrapper::~SysCallWrapper()
{
	delete filter;
}

int SysCallWrapper::connect(int fd, const char *socket_name)
{
//	filter->add_fd(fd, socket_name, strlen(socket_name));
	if(filter!=NULL)
		filter->filter_connect(fd,socket_name);
	return 0;
}

int SysCallWrapper::sendmsg(char* filename,int fd, const char *msg_buffer, size_t msg_buffer_size)
{
        int error=0;
        if(filter!=NULL)
            error = filter->filter_sendmsg(filename,fd, msg_buffer, msg_buffer_size);

        return error;
}

int SysCallWrapper::shutdown(int fd, int how)
{
        //filter->remove_fd(fd); 
	return 0;
}

int SysCallWrapper::open(const char* filename,int flags,umode_t mode)
{
	if(filter!=NULL)	
		return	filter->filter_open(filename,flags,mode);
	return 0;
}

int SysCallWrapper::read(char*filename,unsigned int fd,char __user *buf,size_t count)
{
	if(filter!=NULL)
		return filter->filter_read(filename,fd,buf,count);
	return 0;
}

int SysCallWrapper::write(char* filename,unsigned int fd,char *buf,size_t count)
{
	if(filter!=NULL)
		return filter->filter_write(filename,fd,buf,count);
	return 0;
}

int SysCallWrapper::sock_send(char* filename,int fd,struct sockaddr *address,int *len)
{
	if(filter!=NULL)
		return filter->filter_sock_send(filename,fd,address,len);
	return 0;
}


int SysCallWrapper::sock_recv(char* filename,int fd,struct sockaddr* add,int*len)
{
        if(filter!=NULL)
                return filter->filter_sock_recv(filename,fd,add,len);
        return 0;
}

/*int SysCallWrapper::sock_accept(unsigned int fd,struct sockaddr *add)
{
	if(filter!=NULL)
		return filter->filter_sock_accept(fd,add);
	return 0;
}*/

int SysCallWrapper::binder(int fromPid,int toPid)
{
	if(filter!=NULL)
		return filter->filter_binder(fromPid,toPid);
	return 0;
}


int SysCallWrapper::add_filter(MessageFilter *filter)
{
	this->filter = filter;
	return 0;
}

int SysCallWrapper::remove_filter()
{
	delete filter;
	return 0;
}


MessageFilter::MessageFilter(char *filename, int filename_len, long fd)
{
        this->filename = filename;
        this->filename_len = filename_len;
        this->fd = fd;
}

int MessageFilter::filter_connect(unsigned int fd,char* socket_name)
{
	return 0;
}

int MessageFilter::filter_open(const char* filename,int flags,umode_t mode)
{
	return 0;
}

int MessageFilter::filter_read(char* filename,unsigned int fd,char *buf,size_t count)
{
	return 0;
}

int MessageFilter::filter_write(char* filename,unsigned int fd,char *buf,size_t count)
{
	return 0;
}

int MessageFilter::filter_sendmsg(char* filename, int fd,const char* msg_buffer,size_t msg_buffer_size)
{
	return 0;
}

int MessageFilter::filter_sock_send(char* filename,int fd,struct sockaddr *address,int* len)
{
	return 0;
}

int MessageFilter::filter_sock_recv(char* filename,int fd,struct sockaddr* add,int*len)
{
        return 0;
}

/*int MessageFilter::filter_sock_accept(unsigned int fd,struct sockaddr *add)
{
	return 0;
}*/

int MessageFilter::filter_binder(int fromPid,int toPid)
{
	return 0;
}


extern "C" {
#define EXPORT(sym) \
extern void sym (void); \
EXPORT_SYMBOL(sym)

extern long sys_sendmsg_internal(int fd, struct user_msghdr __user * msg, unsigned flags);
//extern long __sys_sendmsg(int fd, struct msghdr __user * msg, unsigned flags);
extern long __sys_sendmsg(int fd, struct user_msghdr __user * msg, unsigned flags);
extern int sys_shutdown_internal(int fd, int how);
extern long do_sys_open(int dfd,const char* filename,int flags,umode_t mode);

SysCallWrapper *wrapper = NULL;
EXPORT_SYMBOL(wrapper);
//EXPORT(_ZN8Wrappers14SysCallWrapperD1Ev);
//EXPORT(_ZN8Wrappers14SysCallWrapper10add_filterEPNS_13MessageFilterE);
EXPORT_SYMBOL(_ZTIN8Wrappers13MessageFilterE);
//EXPORT(_ZN8Wrappers14SysCallWrapper13remove_filterEv);
//EXPORT(_ZN8Wrappers14SysCallWrapperC1Ev);
//EXPORT(__cxa_end_cleanup);
//EXPORT(_ZTVN10__cxxabiv120__si_class_type_infoE);
//EXPORT(_ZdlPv);
//EXPORT(_Znwj);
//EXPORT(__gxx_personality_v0);
//EXPORT(_Znaj);
EXPORT(_ZTVN8Wrappers13MessageFilterE);
//EXPORT(_ZdaPv);
//EXPORT(_ZTIi);
//EXPORT(_ZNKSt9type_infoeqERKS_);
//EXPORT(_ZN8Wrappers14SysCallWrapper4readEjPcj);
//EXPORT_SYMBOL(getFilename);

int sys_connect_wrapper(int fd, const char *socket_name)
{
	if(wrapper!=NULL)
		wrapper->connect(fd, socket_name);
}

/*int sock_accept_wrapper(unsigned int fd,struct sockaddr * add)
{
	if(wrapper!=NULL)
		return wrapper->sock_accept(fd,add);
	return 0;
}*/


int sys_sendmsg_wrapper(char*filename,int fd, struct user_msghdr __user *msg, unsigned flags)
{
	if(wrapper!=NULL) {
		struct user_msghdr msg_sys;
		struct iovec iov;
		int iovec_size;
		char *msg_buffer;
		int error = 0;

		iovec_size = sizeof(struct iovec);
		msg_buffer = __kmalloc(iovec_size, GFP_KERNEL); 

		copy_from_user(&msg_sys, msg, sizeof(struct msghdr));
		if(msg_sys.msg_iovlen>0) {
		    copy_from_user(&iov, msg_sys.msg_iov, sizeof(struct iovec));
		    copy_from_user(msg_buffer, iov.iov_base, iovec_size);
		    if(wrapper!=NULL)
			    error = wrapper->sendmsg(filename,fd, msg_buffer, iovec_size);
		}

		kfree(msg_buffer);
		if(error<0)
		    return error;
	}
        return sys_sendmsg_internal(fd, msg, flags);
		//return __sys_sendmsg(fd, msg, flags);

}

int sys_shutdown_wrapper(int fd, int how)
{
        if(wrapper!=NULL) {
            wrapper->shutdown(fd, how);
        }
        return sys_shutdown_internal(fd, how);
}


long sys_open_wrapper(int dfd,const char __user* filename, int flags,umode_t mode)
{
	int error=0;
	if(wrapper!=NULL)
	{
		char* name;
		int len;
		len = strlen_user(filename);
		name = (char*)kmalloc(1000*sizeof(char),GFP_KERNEL);
		copy_from_user(name,filename,len);
		error =	wrapper->open(name,flags,mode);
	}
	if(error<0)
		return error;
	return do_sys_open(dfd,filename,flags,mode);
}
long sys_read_wrapper(char* filename,unsigned int fd,char __user *buf, size_t count)
{
	if(wrapper!=NULL)
		return	wrapper->read(filename,fd,buf,count);
	return 0;
}

long sys_write_wrapper(char* filename,unsigned int fd,const char *buf,size_t count)
{
	if(wrapper!=NULL)
		return wrapper->write(filename,fd,buf,count);
	return 0;
}

int sock_send_wrapper(char*filename,int fd,struct sockaddr *address,int *len)
{
	if(wrapper!=NULL)
		return wrapper->sock_send(filename,fd,address,len);
	return 0;
}

int sock_recv_wrapper(char*filename,int fd,struct sockaddr *add,int* len)
{
	if(wrapper!=NULL)
		return wrapper->sock_recv(filename,fd,add,len);
	return 0;
}

int binder_wrapper(int fromPid,int toPid)
{
	if(wrapper!=NULL)
		return wrapper->binder(fromPid,toPid);
	return 0;
}


}
