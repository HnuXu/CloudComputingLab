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


void error_func(string method,string url,int sockfd)
{//处理错误请求的函数
	string entity;
	if(method!="GET"&&method!="POST")
    {//GET和POST之外的请求，不处理
		entity="<htmL><title>501 Not Implemented</title><body bgcolor=ffffff>\n Not Implemented\n<p>Does not implement this method: "+method+"\n,<hr><em>HTTP Web Server </em>\n</body></html>\n";
		int entity_len=entity.length();
		stringstream ss;
		ss<<entity_len;
		string str;
		ss>>str;
		string tmp="Http/1.1 501 Not Implemented\r\nContent-type: text/html\r\nContent-Length:"+str+"\r\n\r\n";
		string message=tmp+entity;
		char send_buf[1024];
		sprintf(send_buf,"%s",message.c_str());
		write(sockfd,send_buf,strlen(send_buf));            
	}
	else
    {//GET或POST请求不满足要求
		if(method=="GET")
        {
			entity="<html><title>GET 404 Not Found</title><body bgcolor=ffffff>\n Not Found\n<p>Couldn't find this file: "+url+"\n<hr><em>HTTP Web Sever</em>\n</body></html>\n";
		}
		else if(method=="POST")
        {
			entity="<html><title>POST 404 Not Found</title><body bgcolor=ffffff>\n Not Found\n<hr><em>HTTP Web Sever</em>\n</body></html>\n";
		}
		int entity_len=entity.length();
		stringstream ss;
		ss<<entity_len;
		string str;
		ss>>str;
		string tmp="Http/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-Length:"+str+"\r\n\r\n";
		string message=tmp+entity;
		char send_buf[1024];
		sprintf(send_buf,"%s",message.c_str());
		write(sockfd,send_buf,strlen(send_buf));
	}
}

void get_func(string method,string url,int sockfd)//GET请求处理
{
	int len=url.length();
	string tmp="./src";
	if(url.find(".")==string::npos)//url中不存在‘.’
    {
		if(url[len-1]=='/'||url.length()==0)
        {
			tmp+=url+"index.html";
		}
		else tmp+=url+"./index.html";
	}
	else tmp+=url;
	int fd=open(tmp.c_str(),O_RDONLY);
	if(fd>=0)
    {//打开了文件
		int tlen=tmp.length();
        struct stat stat_buf;
   		fstat(fd,&stat_buf);
		char outstring[1024];
        sprintf(outstring,"Http/1.1 200 OK\r\nContent-Length:%d\r\nContent-Type: text/html\r\n\r\n",stat_buf.st_size);
		write(sockfd,outstring,strlen(outstring));
		sendfile(sockfd,fd,0,stat_buf.st_size);
	}
	else
    {//出现错误，打不开文件
		error_func(method,url,sockfd);
	}
}