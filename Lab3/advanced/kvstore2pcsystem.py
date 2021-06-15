#!/user/bin/env python
#-*- coding:utf-8 -*-
import socket
import threading
import sys
import time


ip=[]
port=[]
conf=[]

def R_Conf(argv,ROLE=0,par_num=0):
    conf_file_name=argv[2]
    for line in open(conf_file_name):
        line=line.strip('\n')
        conf.append(line)
    role=conf[0].split(' ')
    if role[1]=="participant\r":
        ROLE=0 
        ip_port_arr=conf[1].split(' ')
        ip_port=ip_port_arr[1].split(':')
        ip.append(ip_port[0])
        port.append(ip_port[1])
    elif role[1]=="coordinator\r":
        par_num=len(conf)-2
        ROLE=1
        for x in range (par_num+1) :
            ip_port_arr=conf[x+1].split(' ')
            ip_port=ip_port_arr[1]. split(':')
            ip.append(ip_port[0])
            port.append(ip_port[1])
    return ROLE,par_num##ROLE和参与者数量##
                        
def  cHeartBeat():
    chb = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    chb.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR, 1)
    chb.bind(("127.0.0.1", 10000))
    while True:  
        for x in range(3):
            chb.sendto("mess0".encode('utf-8'),("127.0.0.1",x+10001))
            print "send"+str(x+10001)
            pa=chb.recv(1024).decode('utf-8')  
            if pa=="mess":
                print "recvvv"
            time.sleep(1) 

def  pHeartBeat(x):
    phb = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    phb.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR, 1)
    phb.bind(("127.0.0.1", int(x)+1999))  
    print int(x)+1999
    while True:
        pa,sip=phb.recvfrom(1024)
        if pa=="mess0":
            phb.sendto("mess".encode('utf-8'),sip)
            print "aaaaa"


ROLE,par_num=R_Conf(sys.argv,0,0)

if ROLE==0:
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.bind((ip[0],int(port[0])))  #绑定ip和端口到套接字
    KV_Store={}

    pongThead = threading.Thread(target=pHeartBeat,args=(port[0],)) 
    pongThead.start()

    while True :
        message,srcip=s.recvfrom(1024)
        if message.decode('utf-8')=="Are u prepared":
            s.sendto("Yes".encode( 'utf-8' ),srcip)
        para=s.recv(1024).decode('utf-8').split()

        if para[0].upper()=="SET":
            KV_Store[para[1]]=para[2]
        if para[0].upper()=='GET'and para[-1]=='0':
            if(KV_Store.get(para[1],0)!=0):
                s.sendto(KV_Store[para[1]].encode('utf-8'),srcip)
            else:
                s.sendto(("nil").encode( 'utf-8' ),srcip) #键不存在
        if para[0].upper()=='DEL'and para[-1]=='0':
            z=0
            i=1
            while i<len(para):
                if(KV_Store.get(para[i],0)!=0):
                    del KV_Store[para[i]]
                    z+=1
                    i+=1
                else:
                    i+=1
                    continue
	    print z
            s.sendto(str(z).encode('utf-8'),srcip)
    s.close

elif ROLE==1:
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s_connect_participant=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.bind((ip[0],int(port[0])))
    participants=[]
    par_num=0

    pingThread = threading.Thread(target=cHeartBeat)#, args=(s_ping, srcip, par_num,)) 
    pingThread.start()

    while True:
        message,srcip=s.recvfrom(1024)
        response=1
        for x in range(par_num):
            s_connect_participant.sendto("Are u prepared".encode('utf-8'),(ip[x+1],int(port[x+1])))
            temp_m=s_connect_participant.recv(1024).decode('utf-8')
	    print temp_m
            if temp_m=="No":
                response=0
        if response==0:
            s.sendto('-Error\\r\\n'.encode('utf-8' ),srcip)
        else:
            para=message.decode('utf-8').split()
            if para[0].upper()=='SET':
		for x in range(par_num):
		    s_connect_participant.sendto(message,(ip[x+1],int(port[x+1])))
                s.sendto('+OK\\r\\n'.encode('utf-8'),srcip)
            # contact participants
            if para[0].upper() == "GET":
		for x in range(par_num):
                    only_get_once =message.decode('utf-8') +" "+ str(x)
                    s_connect_participant.sendto(only_get_once.encode('utf-8'),(ip[x + 1],int(port[x + 1])))
                   ### p('[->]',thread_id, only_get_once +" "+ ip[x + 1] +":"+ port[x + 1])
                    if x==0:    #暂定将第1个参与者发回的信息发送给client
                        get_value = s_connect_participant.recv(1024).decode('utf-8')
                        ###p('[<-]', thread_id, get_value)
                        s1 = get_value.split(' ')
                        response_code ='*'+ str(len(s1)) + '\\r\\n'
                        for x in s1:
                            response_code+='$'+str(len(x)) + '\\r\\n' + x + '\\r\\n'
                        s.sendto(response_code.encode('utf-8'), srcip)   #发回给client
                        ###p('[->]',thread_id, response_code + " "+ srcip[0] + ":"+ srcip[1] )
	    if para[0].upper()=="DEL":    
                for x in range(par_num):  
                    del_mess=message.decode('utf-8') +" "+ str(x)
		    s_connect_participant.sendto(del_mess.encode('utf-8'),(ip[x + 1],int(port[x + 1])))
		    if(x==0):
                        get_num = s_connect_participant.recv(1024).decode('utf-8')
                        response_code=':'+get_num + '\\r\\n' 
                        s.sendto(response_code.encode('utf-8'), srcip)



