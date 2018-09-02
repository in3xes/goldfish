#include<c++/begin_include.h>
#include<linux/fcntl.h>
//#include<linux/fs.h>
#include<asm/uaccess.h>
#include<linux/smswrapper.h>
#include<c++/syscallwrapper_sms.h>
using namespace SMSWrappers;
#include<c++/end_include.h>


#define NETLINK_NITRO 17
#define COUNTER_FILTER true

int sms_counter = 3;
int smscount=0;

const char *RILD_FILE = "/dev/socket/rild";
const int RILD_FILENAME_LEN = 16;


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LKM in c++");
MODULE_AUTHOR("MOOL");

extern SysCallWrapper_sms *wrappers;

class SMSMessageFilter:public MessageFilter_sms
{

public:
	static const char *RILD_FILE;
	static const int RILD_FILENAME_LEN;
protected:
	MessageFilter_sms *filter;
    int sentSMSCount;
public:

	SMSMessageFilter(MessageFilter_sms *filter);
	~SMSMessageFilter();

	int add_fd(long fd, char *filename, int filename_len);
    int remove_fd(long fd);
    int call_filter(int fd, const char *msg_buf, size_t count);
};

const char *SMSMessageFilter::RILD_FILE = RILD_FILE;
const int SMSMessageFilter::RILD_FILENAME_LEN = RILD_FILENAME_LEN;

SMSMessageFilter::SMSMessageFilter(MessageFilter_sms *filter)
{
	this->sentSMSCount = 0;
	this->filter = filter;
	printk("In SMSMessageFilter constructor :::;\n");
}

int SMSMessageFilter::add_fd(long fd, char *filename, int filename_len)
{
    if(filter->filename == NULL && 
					0==strncmp(filename, SMSMessageFilter::RILD_FILE,
					filename_len > SMSMessageFilter::RILD_FILENAME_LEN? SMSMessageFilter::RILD_FILENAME_LEN: filename_len))
		return filter->add_fd(fd, filename, filename_len);

	return 0;
}

int SMSMessageFilter::remove_fd(long fd)
{
	if(filter->fd == fd)
		return filter->remove_fd(fd);
	
	return 0;
} 

int SMSMessageFilter::call_filter(int fd, const char *msg_buffer, size_t count)
{
	printk("------msg-type: %d----------\n", count);

    int msg_type = -1, d;
	#ifdef __LITTLE_ENDIAN
		if(count>4) {
		    d=msg_buffer[3];
		    d=d<<8|msg_buffer[2];
		    d=d<<8|msg_buffer[1];
		    d=d<<8|msg_buffer[0];
		}
	#else
		if(count>4) {
		    d=msg_buffer[0];
		    d=d<<8|msg_buffer[1];
		    d=d<<8|msg_buffer[2];
		    d=d<<8|msg_buffer[3];
		}
	#endif
		msg_type = d;
		//printk("------msg-type: %d-----------\n", msg_type);
		if(msg_type == 25) {
				if(COUNTER_FILTER == true) {
						printk("Counter is %d\n", sms_counter);
						if(smscount >= sms_counter) {
							if(sms_counter != 0)
							{
									printk("Failed! cannot send more than %d messages\n", smscount);
									return -EFAULT;
							}

						}
						smscount++;
						printk("Sending SMS\n");
				}	
				else {
                    /*
					struct file *f;
					char buf[1];
					mm_segment_t fs;
					int i;

					for(int i=0; i<1; i++)
							buf[i] = 0;


					f = filp_open("/data/data/com.example.in3xes.demo/files/sms_allowed.txt", O_RDONLY, 0);

					if(f == NULL) {
						printk("failed opening file\n");
						return -EFAULT;
					}
					else {
						fs = get_fs();
						set_fs(get_ds());
						f->f_op->read(f, buf, 1, &f->f_pos);
						set_fs(fs);
						if(buf[0] != '1') {
							printk("SMS is restricted, not sending\n");
							return -EFAULT;
						}
					}
					printk("Sending SMS");
					filp_close(f, NULL);
                    */
				}
		} 
	return 1;
}


extern "C" {

	int  smswrapper_init()
	{
       printk("---------------------------------------------------\n");

        MessageFilter_sms *filter;        	

        if(wrappers==NULL)
	    wrappers = new SysCallWrapper_sms();
		filter = new SMSMessageFilter(wrappers->filter);
		wrappers->add_filter(filter);        
        printk("In smswrapper Init function: %d\n", wrappers);
		return 0; 
	}
} 
