### HTTP 服务器
#描述：
这是一个加了EPOLL的多进程服务器，采用一个进程在等待请求和回送请求，多个worker来处理请求的内容的方式，集成了管道，共享内存，信号等机制，但没压力测试过。

#TODO：
未来还准备采用心跳监听等方式来维护服务器的稳定，更多详细的内容请参考 DOC 文件夹

#bug：
共享内存的维护有bug，导致刷新浏览器中的页面太快会导致页面异常

#安装：
使用 make 指令即可，运行得到的 HTTPserver 可执行文件，服务器将会开启

#配置：
一些基础配置如 IP:PORT 放在 /include/conf.h,计划中它将被移入 'httpsvr.conf' 中，但一直没精力去做.
