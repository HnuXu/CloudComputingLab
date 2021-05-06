Lab2文件夹包括：

1.  /src/index.html  get请求的html文件

2.  main.cc  负责创建tcp连接和多个线程

3.  function.cc  负责处理不同的http请求

4.  Makefile  将main.cc和function.cc编译生成一个可执行文件httpserver

5.  client.c  测试使用的客户端程序

6.  性能分析报告pdf文档


------------------------------------------------



通过make指令生成可执行文件httpserver

使用

    ./httpserver

运行服务器

可以添加 --ip 和 --port 参数指定服务器的ip地址和端口号，默认为 127.0.0.1:8888

通过curl可以向服务器发送get和post请求，或者使用client.c编译生成测试程序

因为Apache Bench和http_host和Web Bench工具测试时都有一些问题，性能报告中使用client.c进行测试
