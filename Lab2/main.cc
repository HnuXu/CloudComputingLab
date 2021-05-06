#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>					
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>					
#include <pthread.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include<fcntl.h>
#include <linux/tcp.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <getopt.h>
using namespace std;
pthread_mutex_t mutex;
pthread_mutex_t lock;
int counter=0;

int main(int argc, char *argv[])
{
	int sockfd = 0;	//套接字
	int connfd = 0;
	int err_log = 0;
	struct sockaddr_in server_addr;	//服务器地址结构体
	unsigned short port = 8888; //监听端口

	pthread_t pt;
	
	pthread_mutex_init(&mutex, NULL); //初始化互斥锁，互斥锁默认是打开的

	printf("TCP Server Started at port %d!\n", port);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);//创建TCP套接字
	if(sockfd < 0)
	{
		perror("socket error");
		exit(-1);
	}
	
	bzero(&server_addr, sizeof(server_addr));// 初始化服务器地址
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Binding server to port %d\n", port);
	printf("Binding server to ip %s\n",server_addr.sin_addr.s_addr);
	
    int opt;
    int digit_optind = 0;
    int option_index = 0;
    char *string = "a:b:d";
    static struct option long_options[] =
    {
        {"ip",required_argument,NULL,'r'},
		{"port",required_argument,NULL,'r'},
		{NULL,0,NULL,0},
	};
	int i=1;
	while((opt =getopt_long_only(argc,argv,string,long_options,&option_index))!= -1)
	{  
		if(strcmp(argv[i],"--port")==0)
		{
			port=atoi(optarg);
			server_addr.sin_port = htons(port);
		}
		printf("opt = %c\t\t",opt);
		printf("optarg = %s\t\t",optarg);
		printf("argv[i] =%s\t\t",argv[i]);
		if(i==1)
		{
			i=optind;
		}
		printf("option_index = %d\n", option_index);
	}

	//绑定
	err_log = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(err_log != 0)
	{
		perror("bind");
		close(sockfd);		
		exit(-1);
	}
	
	//监听，套接字变被动
	err_log = listen(sockfd, 10);
	if( err_log != 0)
	{
		perror("listen");
		close(sockfd);		
		exit(-1);
	}
	
	printf("Waiting client...\n");



	close(sockfd);
	return 0;
}
