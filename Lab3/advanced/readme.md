advanced版本的代码组成和执行方式与basic版本相同

不同的之处在于，协调者进程和参与者进程各有一个线程负责发送/接收周期性的心跳信息：

![avatar](https://github.com/HnuXu/test/blob/main/a1.png)

在保持心跳的基础上，advanced版本可以实现SET指令：

![avatar](https://github.com/HnuXu/test/blob/main/a2.png)
![avatar](https://github.com/HnuXu/test/blob/main/a3.png)

但它并不包含更多的功能

即，协调者无法通过长时间不回复心跳包的行为判断参与者故障并将其排除

除此之外，对于GET和DEL指令，这个版本的代码也都还存在一些问题
