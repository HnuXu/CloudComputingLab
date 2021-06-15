import socket

s=socket.socket(socket.AF_INET, socket.SOCK_DGRAM) #新建套接字
s.settimeout(4)  #coordinator dead设置协调者超时时问
while True :
    try:
        #  State1: init
        message = input('Client Sending:')
        s.sendto(message.encode('utf-8'),('127.0.0.1', 8001))
        # State2:Get response  将客户端输入的命令转发到协调者
        resp = s.recv(1024)
        print(resp)
        if len(resp) == 0:    #来等到协调者， 重新等待
            continue
        else :
            print(resp.decode('utf-8'))
    except Exception as e :
        print(repr(e))