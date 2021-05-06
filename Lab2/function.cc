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