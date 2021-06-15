# 实验文件说明

**Lab3的文件包括：**

    1.client.py               //客户端程序
   
    2.kvstore2pcsystem.py     //kv-store程序
    
    3.多个配置文件

<br/>

**配置文件格式：**

coordinator：

    mode coordinator 
    coordinator_info 127.0.0.1:8001
    participant_info 127.0.0.1:8002 
    participant_info 127.0.0.1:8003 
    participant_info 127.0.0.1:8004
    
第一行指明模式，第二行指定协调者信息，后面指定参与者的信息

participant：

    mode participant
    participant_info 127.0.0.1:8002
    coordinator_info 127.0.0.1:8001
    
第一行指明模式，第二行指定参与者（本人）信息，第三行指定所属协调者的信息

**配置文件中的注释行以“!”开头**

<br/>

**客户端:**

通过udp连接服务器，使用**简单的RESP阵列格式**向服务器发送指令，并收到同样**以简单RESP阵列格式编码的回复**

<br/>

**服务器：**

实验实现的是basic版本，协调者可以接受来自客户端的指令，并将存储内容同步到参与者。

当协调者中途出现故障（如掉线重启）时，只要参与者没有故障，不会影响到参与者中的数据。

同时，我们还实现了一个功能不太完善的advanced版本：协调者和参与者各有一个线程，每隔一段时间相互发送心跳包

不过简易的advanced版本只能在1个协调者和3个参与者之间进行心跳测试，当故障发生时，不能主动维护

简易的advanced版本代码在advanced文件夹中

<br/>

# 执行样例简述

运行client.py和kvstore2pcsystem.py，配置文件参照实验指导书：

    协调者为127.0.0.1:8001
    3个参与者分别为127.0.0.1:8002，127.0.0.1:8003，127.0.0.1:8004

<br/>

在客户端窗口输入指令，可以看到，SET、GET和DEL都做出了正确的反馈：

**set指令测试：**

SET a b

![avatar](https://github.com/HnuXu/test/blob/main/set1.png)
![avatar](https://github.com/HnuXu/test/blob/main/set2.png)

**get指令测试：**

GET a

![avatar](https://github.com/HnuXu/test/blob/main/get1.png)
![avatar](https://github.com/HnuXu/test/blob/main/get2.png)

**del指令测试：**

删除一个值：DEL a

![avatar](https://github.com/HnuXu/test/blob/main/del1.png)
![avatar](https://github.com/HnuXu/test/blob/main/del2.png)

删除多个值：DEL a c

![avatar](https://github.com/HnuXu/test/blob/main/del3.png)
![avatar](https://github.com/HnuXu/test/blob/main/del4.png)

**重新启动协调者测试：**

SET a b 

SET c d

![avatar](https://github.com/HnuXu/test/blob/main/c1.png)
![avatar](https://github.com/HnuXu/test/blob/main/c2.png)

杀死并重启协调器进程，GET a

![avatar](https://github.com/HnuXu/test/blob/main/c3.png)
![avatar](https://github.com/HnuXu/test/blob/main/c4.png)
![avatar](https://github.com/HnuXu/test/blob/main/c5.png)

<br/>

在运行过程中，kv-store采用2pc协议：协调者受到指令后先发送“Are u prepared”的命令询问参与者，都收到“Yes”回复后再将指令传送给参与者执行。
