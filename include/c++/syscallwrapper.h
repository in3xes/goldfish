/*
 * syscallwrapper.h
 *
 * Created on: Sept 25, 2012
 * Author: doslab
 */

#ifndef __SYSCALLWRAPPERS__
#define __SYSCALLWRAPPERS__

#include <c++/begin_include.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <c++/end_include.h>

namespace Wrappers
{

class MessageFilter
{
public:
 
        char *filename;
        long fd;
        int filename_len;

        MessageFilter(char *filename, int filename_len, long fd);
        MessageFilter() {}
        ~MessageFilter() {}
	virtual int filter_connect(unsigned int fd,char* socket_name);
//	virtual int filter_sock_accept(unsigned int fd, struct sockaddr *add);
	virtual int filter_open(const char* filename,int flags,umode_t mode);
	virtual int filter_read(char*filename,unsigned int fd,char *buf,size_t count);
	virtual int filter_write(char* filename,unsigned int fd,char *buf,size_t count);
	virtual int filter_sendmsg(char* filename,int fd,const char* msg_buffer,size_t msg_buffer_size);
	virtual int filter_sock_send(char*filename, int fd,struct sockaddr *address,int *len);
	virtual int filter_sock_recv(char* filename,int fd,struct sockaddr *add,int* len);
	virtual int filter_binder(int fromPid,int toPid);
};

class SysCallWrapper
{
public:
        MessageFilter *filter;


        SysCallWrapper();
	~SysCallWrapper();

        int add_filter(MessageFilter *filter);
        int remove_filter();

        int connect(int fd, const char *socket_name);
//	int sock_accept(unsigned int fd,struct sockaddr* add);
        int sendmsg(char* filename,int fd, const char *msg_buffer, size_t msg_buffer_size);
        int shutdown(int fd, int how);
	int open(const char* filename,int flags,umode_t mode);
	int read(char*filename,unsigned int fd,char __user* buf,size_t count);
	int write(char* filename,unsigned int fd,char* buf,size_t count);
	//char* getFilename(unsigned int fd);
	int sock_send(char*filename,int fd,struct sockaddr *address,int* len);
	int sock_recv(char* filename,int fd,struct sockaddr* add,int*len);
	int binder(int fromPid,int toPid);
};

}

#endif
