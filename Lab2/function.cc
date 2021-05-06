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

void post_func(string name,string id,int sockfd)//POST请求处理
{
	string entity="<html><title>POST Method</title><body bgcolor=ffffff>\nYour Name: "+name+"\nID: "+id+"\n<hr><em>Http Web server</em>\n</body></html>\n";
	int entity_len=entity.length();
	stringstream ss;
	ss<<entity_len;
	string str;
	ss>>str;
	string tmp="HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-Length: "+str+"\r\n\r\n";
	string message=tmp+entity;
	char send_buf[1024];
	sprintf(send_buf,"%s",message.c_str());
	write(sockfd,send_buf,strlen(send_buf));
}

void function(char* recv_buf,int connfd)
{
	string buffer;
	bool flag;
	flag=true;//持久连接标志
	buffer=string(recv_buf);
	//处理http请求报文
	while(buffer.find("HTTP/1.1")!=string::npos)//判断buffer中有没有完整报文
    {
		int message_end=0;//请求除主体外报文尾部         
		if((message_end=buffer.find("\r\n\r\n"))!=-1)//判断是否有请求体
		{
			string request="";//请求报文
			message_end+=4;
			request=buffer.substr(0,message_end);
			int message_begin=request.find("Content-Length");
			int entity_pos=request.length();//实体主体起始位置
			if(message_begin!=-1)//存在请求体
			{
				string num;
				message_begin+=15;
				while(request[message_begin]!='\r')
				{
					num+=request[message_begin++];
				}
				int entity_len=atoi(num.c_str());
				if((buffer.length()-request.length())>=entity_len)//有主体
				{
					request+=buffer.substr(request.length(),entity_len);
					message_end+=entity_len;
				}
				else continue;
			}
			buffer=buffer[message_end];//得到完整请求报文
			string method,url;
			message_end=0;
			while(request[message_end]!=' ')
			{
				method+=request[message_end++];
			}
			if(method!="GET"&&method!="POST")
			{
				error_func(method,url,connfd);
				continue;
			}
			++message_end;
			while(request[message_end]!=' ')
			{
				url+=request[message_end++];
			}
			++message_end;//提取url
			if(method=="GET")
			{
				get_func(method,url,connfd);
			}
			else if(method=="POST")
			{                                
				if(url!="/Post_show")
				{
					error_func(method,url,connfd);
					continue;
				}
				string entity=request.substr(entity_pos,request.length()-entity_pos);
				int name_pos=entity.find("Name="),id_pos=entity.find("&ID=");
				printf("Name=%d,id=%d",name_pos,id_pos);
				if(name_pos==-1||id_pos==-1||id_pos<=name_pos)
				{
					error_func(method,url,connfd);
					continue;
				}
				string name,id;                        
				name=entity.substr(name_pos+5,id_pos-name_pos-5);
				id=entity.substr(id_pos+4);
				post_func(name,id,connfd);
			}
		}
	}
}
