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
#include <c++/end_include.h>

namespace SMSWrappers
{

class MessageFilter_sms
{
public:
 
        char *filename;
        long fd;
        int filename_len;

        MessageFilter_sms(char *filename, int filename_len, long fd);
        MessageFilter_sms() {}
        ~MessageFilter_sms() {}

	virtual int add_fd(long fd, char *filename, int filename_len);

        virtual int remove_fd(long fd);
      
        /* @call_filter
 	 * fd - incoming file descriptor, if the message filter is handle multiple messages 
 	 *      then the action should be taken only when fd matches this->fd
 	 * msg_buf - message buffer sent 
 	 * count - message buffer size
 	 * */ 
        virtual int call_filter(int fd, const char *msg_buf, size_t count);
};

class SysCallWrapper_sms
{
public:
        MessageFilter_sms *filter;

        SysCallWrapper_sms();
	~SysCallWrapper_sms();

        int add_filter(MessageFilter_sms *filter);
        int remove_filter();

        int connect(int fd, const char *socket_name);
        int sendmsg(int fd, const char *msg_buffer, size_t msg_buffer_size);
        int shutdown(int fd, int how);
	
};

}

#endif
