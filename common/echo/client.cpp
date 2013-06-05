
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int do_client(int fd);

int main(int argc , char *argv[])
{
	if(argc < 3)
	{
		printf("Usage : ./client , ip , port\n");
		return -1;
	}

	int fd = socket(AF_INET , SOCK_STREAM , 0);
	if(fd < 0)
	{
		perror("socket error : ");
		return -1;
	}

	struct sockaddr_in serv;
	memset(&serv , 0 , sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(atoi(argv[2]));
	if(inet_pton(AF_INET , argv[1] , &(serv.sin_addr)) < 0)
	{
		perror("inet_pton error : ");
		return -1;
	}

	if(connect(fd , (struct sockaddr *)&serv , sizeof(serv)) < 0)
	{
		perror("connect error : ");
		return -1;
	}

	do_client(fd);

	close(fd);
	return 0;
}

struct Header
{
	int cmd ;
	int len ; 
	int error ;
	int para1 ; 
	int para2 ;
};

int do_client(int fd)
{
	int end_times = 0;
	char buf[1024];
	while(1)
	{
		char *str = fgets(buf , 1024 , stdin);
		if(NULL == str)
		{
			++ end_times;
			if(end_times > 1)
				return 0;
		}
		
		int len = 0;
		if(str != NULL)
		{
			len = strlen(str);
			str[len - 1] = '\0';
			len --;
			printf("Get line from stdin : %s and length %d\n" , str , len);
		}
		else 
		{
			len = 0;
			printf("Empty input !\n");
		}

		struct Header head;
		memset(&head , 0 , sizeof(head));
		head.len = len;

		if(write(fd , &head , sizeof(head)) != sizeof(head))
		{
			perror("Write head error : ");
			return -1;
		}
		if(str != NULL)
		{
			if(write(fd , str , len) != len)
			{
				perror("write data error : ");
				return -1;
			}
		}


		if(read(fd , &head , sizeof(head)) != sizeof(head))
		{
			perror("read head error : ");
			return -1;
		}

		len = head.len;
		len = read(fd , buf , len);
		if(len < 0)
		{
			perror("read data error : ");
			return -1;
		}

		buf[len] = '\0';

		printf("Read from server : \"%s\"\n" , buf);
	}

	return 0;
}
