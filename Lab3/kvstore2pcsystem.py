#!/user/bin/env python
#-*- coding:utf-8 -*-
import socket
try:
    import configparser
except:
    import ConfigParser as configparser
import threading
conf = configparser.ConfigParser()

import sys
import threading
ip=[]
port=[]

def R_Conf(argv,ROLE=0,par_num=0):
    conf_file_name=argv[2]
    for line in open(conf_file_name):
        line=line.strip('\n')
        conf.append(line)
    role=conf[0].split(' ')
    if role[1]=="participant":
        ROLE=0 
        ip_port_arr=conf[1].split(' ')
        ip_port=ip_port_arr[1].split(':')
        ip.append(ip_port[0])
        port.append(ip_port[1])
    elif role[1]=="coordinator":
        par_num=len(conf)-2
        ROLE=1
        for x in range (par_num+1) :
            ip_port_arr=conf[x+1].split(' ')
            ip_port=ip_port_arr[1]. split(':')
            ip.append(ip_port[0])
            port.append(ip_port[1])
    return ROLE,par_num##ROLE和参与者数量##
ROLE,par_num=R_Conf(sys.argv,0,0)




if ROLE==0:
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.bind((ip[0],int(port[0])))  #绑定ip和端口到套接字
    KV_Store={}

    s_ping = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_ping.setsockopt( socket.SOL_SOCKET,socket.So_REUSEADDR, 1)
    s_ping.bind(("127.0.0.1", 10000))  
    s_ping.settimeout(12) 
    data = "+UP Participant"+ port[0]
    s.sendto(data.encode(),("127.0.0.1" ,8001))
    p("[->]", thread_id, data.encode())  
    pongThead = threading.Thread(target=pongHeartBeat,args=(ip[0],port[0])) 
    pongThead.start()

    while True :
        message,srcip=s.recvfrom(1024)
        if message.decode('utf-8')=="Are u prepared":
            s.sendto("Yes".encode( 'utf-8' ),srcip)
        para=s.recv(1024).decode('utf-8').split()

        if para[0]=="SET":
            KV_Store[para[1]]=para[2]
            s.sendto("ACK")
        if para[0]=='GET' and para[-1]=='0':
            if(KV_Store.get(para[1],0)!=0):
                s.sendto(KV_Store[para[1]].encode('utf-8'),srcip)
            else:
                s.sendto(("nil").encode( 'utf-8' ),srcip) #键不存在
        if para[0]=='DEL' and para[-1]=='0':
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
            s.sendto(str(z).encode('utf-8'),srcip)
    s.close

elif ROLE==1:
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s_connect_participant=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.bind((ip[0],int(port[0])))

    s_ping = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_ping.setsockopt( socket.SOL_SOCKET,socket.So_REUSEADDR, 1)
    s_ping.bind(("127.0.0.1", 10000))  
    s_ping.settimeout(12) 

    para=message.decode('utf-8' ).split()
    if para[0] == "+UP":
        id=para[2]
        participants[id] = 1
        par_num += 1
        pingThread = threading.Thread(target=pingHeartBeat, args=(s_ping, srcip, par_num,)) 
        pingThread.start()
    else: 
        for x in range(par_num):
            s_connect_participant.sendto("Are u prepared".encode('utf-8'),(ip[x + 1],int(port[x + 1])))
            #p('[->]',thread_id, "Are u prepared" +""+ ip[x + 1] + ":"+ port[x + 1])
            temp_m = s_connect_participant.recv( 1024).decode('utf-8')
            #p('[<-]', thread_id, temp_m)
            if temp_m == "No":
                response = 0

    while True:
        message,srcip=s.recvfrom(1024)
        response=1
        for x in range(par_num):
            s_connect_participant.sendto("Are u prepared".encode('utf-8'),(ip[x+1],int(port[x+1])))
            temp_m=s_connect_participant.recv(1024).decode('utf-8')
            if temp_m=="No":
                response=0
        if response==0:
            s.sendto('-Error\\r\\n'.encode('utf-8' ),srcip)
        else:
            para=message.decode('utf-8').split()
            if para[0].upper()=='SET':                
                s.sendto('+OK\\r\\n'.encode('utf-8'),srcip)
            # contact participants

            for x in range(par_num):
                if para[0].upper() == "GET":
                    only_get_once =message. decode('utf-8') +" "+ str(x)
                    s_connect_participant.sendto(only_get_once.encode('utf-8'),(ip[x + 1],int(port[x + 1])))
                   ### p('[->]',thread_id, only_get_once +" "+ ip[x + 1] +":"+ port[x + 1])
                    if x==0:    #暂定将第个参与者发回的信息发送给client
                        get_value = s_connect_participant.recv(1024).decode('utf-8')
                        ###p('[<-]', thread_id, get_value)
                        s1 = get_value.split(' ')
                        response_code ='*'+ str(len(s1)) + '\\r\\n'
                        for x in s1:
                            response_code+='$'+str(len(x)) + '\\r\\n' + x + '\\r\\n'
                        s.sendto( response_code.encode( 'utf-8'), srcip)   #发回给client
                        ###p('[->]',thread_id, response_code + " "+ srcip[0] + ":"+ srcip[1] )
        
            if para[0].upper=="DEL":
                s_connect_participant.sendto(message('utf-8'))
                get_num = s_connect_participant.recv(1024).decode('utf-8')
                response_code+=':'+get_num + '\\r\\n' 
                s.sendto(response_code.encode( 'utf-8'), srcip) 

            if para[0].upper=="DEL":    
                for x in range(par_num):  
                    s_connect_participant.sendto(message,(ip[x + 1],int(port[x + 1])))
                    if x==0:
                        get_num = s_connect_participant.recv(1024).decode('utf-8')
                        response_code+=':'+get_num + '\\r\\n' 

                        s.sendto(response_code.encode('utf-8'), srcip)

#def  pongHeartBeat

#def  pingHeartBeat




