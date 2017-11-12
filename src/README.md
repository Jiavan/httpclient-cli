# httpclient-cli
Linux下用C语言实现发送HTTP请求并获取html文档内容的CLI程序

# 理论基础
## HTTP请求报文格式
一个HTTP请求报文由请求行（request line）、请求头部（header）、空行和请求数据4个部分组成，下图给出了请求报文的一般格式。

![src/http-req.png](src/http-req.png)

## 基于TCP协议的网络通信流程

![src/tcp.gif](src/tcp.gif)

## 相关数据结构
### sockaddr_in

```c
// 在netinet/in.h中定义
struct sockaddr_in {
    short int sin_family; /* Address family */
    unsigned short int sin_port; /* Port number */
    struct in_addr sin_addr; /* Internet address */
    unsigned char sin_zero[8]; /* Same size as struct sockaddr */
};
struct in_addr {
    unsigned long s_addr;
};

// sin_family指代协议族，在socket编程中只能是AF_INET
// sin_port存储端口号（使用网络字节顺序）
// sin_addr存储IP地址，使用in_addr这个数据结构
// sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节。
// s_addr按照网络字节顺序存储IP地址
```

### hostent
hostent是host entry的缩写，该结构记录主机的信息，包括主机名、别名、地址类型、地址长度和地址列表。之所以主机的地址是一个列表的形式，原因是当一个主机有多个网络接口时，自然有多个地址。

数据结构定义如下：
```c
struct hostent{
    char * h_name;
    char ** h_aliases;
    short h_addrtype;
    short h_length;
    char ** h_addr_list;
};
#define h_addr h_addr_list[0]

// h_name – 地址的正式名称。
// h_aliases – 空字节-地址的预备名称的指针。
// h_addrtype –地址类型; 通常是AF_INET。
// h_length – 地址的比特长度。
// h_addr_list – 零字节-主机网络地址指针。网络字节顺序。
// h_addr - h_addr_list中的第一地址。
```

用途，在`netdb.h`头中定义了一个`gethostbyname(char *name)`函数，会返回一个hostent结构，参数name可以为一个主机名或IPv4/IPv6的IP地址

## 相关函数
### socket
```
socket（建立一个socket通信）
相关函数
accept，bind，connect，listen
表头文件
#include<sys/types.h>
#include<sys/socket.h>
定义函数
int socket(int domain,int type,int protocol);
函数说明
socket()用来建立一个新的socket，也就是向系统注册，通知系统建立一通信端口。参数domain 指定使用何种的地址类型，完整的定义在/usr/include/bits/socket.h 内，底下是常见的协议:
PF_UNIX/PF_LOCAL/AF_UNIX/AF_LOCAL UNIX 进程通信协议
PF_INET?AF_INET Ipv4网络协议
PF_INET6/AF_INET6 Ipv6 网络协议
PF_IPX/AF_IPX IPX-Novell协议
PF_NETLINK/AF_NETLINK 核心用户接口装置
PF_X25/AF_X25 ITU-T X.25/ISO-8208 协议
PF_AX25/AF_AX25 业余无线AX.25协议
PF_ATMPVC/AF_ATMPVC 存取原始ATM PVCs
PF_APPLETALK/AF_APPLETALK appletalk（DDP）协议
PF_PACKET/AF_PACKET 初级封包接口
参数
type有下列几种数值:
SOCK_STREAM 提供双向连续且可信赖的数据流，即TCP。支持
OOB 机制，在所有数据传送前必须使用connect()来建立连线状态。
SOCK_DGRAM 使用不连续不可信赖的数据包连接
SOCK_SEQPACKET 提供连续可信赖的数据包连接
SOCK_RAW 提供原始网络协议存取
SOCK_RDM 提供可信赖的数据包连接
SOCK_PACKET 提供和网络驱动程序直接通信。
protocol用来指定socket所使用的传输协议编号，通常此参考不用管它，设为0即可。
返回值
成功则返回socket处理代码，失败返回-1。
错误代码
EPROTONOSUPPORT 参数domain指定的类型不支持参数type或protocol指定的协议
ENFILE 核心内存不足，无法建立新的socket结构
EMFILE 进程文件表溢出，无法再建立新的socket
EACCESS 权限不足，无法建立type或protocol指定的协议
ENOBUFS/ENOMEM 内存不足
EINVAL 参数domain/type/protocol不合法
```

### htons
```
htons（将16位主机字符顺序转换成网络字符顺序）
相关函数
htonl，ntohl，ntohs
表头文件
#include<netinet/in.h>
定义函数
unsigned short int htons(unsigned short int hostshort);
函数说明
htons()用来将参数指定的16位hostshort转换成网络字符顺序。
返回值
返回对应的网络字符顺序。
```

### connect
```
connect（建立socket连线）
相关函数
socket，bind，listen
表头文件
#include<sys/types.h>
#include<sys/socket.h>
定义函数
int connect (int sockfd,struct sockaddr * serv_addr,int addrlen);
函数说明
connect()用来将参数sockfd 的socket 连至参数serv_addr 指定的网络地址。结构sockaddr请参考bind()。参数addrlen为sockaddr的结构长度。
返回值
成功则返回0，失败返回-1，错误原因存于errno中。
错误代码
EBADF 参数sockfd 非合法socket处理代码
EFAULT 参数serv_addr指针指向无法存取的内存空间
ENOTSOCK 参数sockfd为一文件描述词，非socket。
EISCONN 参数sockfd的socket已是连线状态
ECONNREFUSED 连线要求被server端拒绝。
ETIMEDOUT 企图连线的操作超过限定时间仍未有响应。
ENETUNREACH 无法传送数据包至指定的主机。
EAFNOSUPPORT sockaddr结构的sa_family不正确。
EALREADY socket为不可阻断且先前的连线操作还未完成。
```

### send
```
send（经socket传送数据）
相关函数
sendto，sendmsg，recv，recvfrom，socket
表头文件
#include<sys/types.h>
#include<sys/socket.h>
定义函数
int send(int s,const void * msg,int len,unsigned int falgs);
函数说明
send()用来将数据由指定的socket 传给对方主机。参数s为已建立好连接的socket。参数msg指向欲连线的数据内容，参数len则为数据长度。参数flags一般设0，其他数值定义如下
MSG_OOB 传送的数据以out-of-band 送出。
MSG_DONTROUTE 取消路由表查询
MSG_DONTWAIT 设置为不可阻断运作
MSG_NOSIGNAL 此动作不愿被SIGPIPE 信号中断。
返回值
成功则返回实际传送出去的字符数，失败返回-1。错误原因存于errno
错误代码
EBADF 参数s 非合法的socket处理代码。
EFAULT 参数中有一指针指向无法存取的内存空间
ENOTSOCK 参数s为一文件描述词，非socket。
EINTR 被信号所中断。
EAGAIN 此操作会令进程阻断，但参数s的socket为不可阻断。
ENOBUFS 系统的缓冲内存不足
ENOMEM 核心内存不足
EINVAL 传给系统调用的参数不正确。
```

### recv
```
recv（经socket接收数据）
相关函数
recvfrom，recvmsg，send，sendto，socket
表头文件
#include<sys/types.h>
#include<sys/socket.h>
定义函数
int recv(int s,void *buf,int len,unsigned int flags);
函数说明
recv()用来接收远端主机经指定的socket传来的数据，并把数据存到由参数buf 指向的内存空间，参数len为可接收数据的最大长度。
参数
flags一般设0。其他数值定义如下:
MSG_OOB 接收以out-of-band 送出的数据。
MSG_PEEK 返回来的数据并不会在系统内删除，如果再调用recv()会返回相同的数据内容。
MSG_WAITALL强迫接收到len大小的数据后才能返回，除非有错误或信号产生。
MSG_NOSIGNAL此操作不愿被SIGPIPE信号中断返回值成功则返回接收到的字符数，失败返回-1，错误原因存于errno中。
错误代码
EBADF 参数s非合法的socket处理代码
EFAULT 参数中有一指针指向无法存取的内存空间
ENOTSOCK 参数s为一文件描述词，非socket。
EINTR 被信号所中断
EAGAIN 此动作会令进程阻断，但参数s的socket为不可阻断
ENOBUFS 系统的缓冲内存不足。
ENOMEM 核心内存不足
EINVAL 传给系统调用的参数不正确
```

# 核心代码
Step 0: 建立socket
建立一个sock连接
```c
socketid = socket(AF_INET, SOCK_STREAM, 0);
```

Step 1: 设置连接信息结构
```c
memset(&sockinfo, 0, sizeof(struct sockaddr_in));
sockinfo.sin_family = AF_INET;
sockinfo.sin_addr.s_addr = *((unsigned long *)purl->h_addr_list[0]);
sockinfo.sin_port = htons(PORT);
```

Step 2: 构造http请求
```c
memset(request, 0, BUFFSIZE);
strcat(request, "GET ");
strcat(request, GET);
strcat(request, " HTTP/1.1\r\n");
// 以上为http请求行信息
strcat(request, "HOST: ");
strcat(request, host);
strcat(request, "\r\n");
strcat(request, "User-Agent: ");
strcat(request, "2333 Browser");
strcat(request, "\r\n");
strcat(request, "Author: ");
strcat(request, "By Jiavan&Kellen&LZY");
strcat(request, "\r\n");
strcat(request,"Cache-Control: no-cache\r\n\r\n");
```

Step 3: 连接到远端服务器
```c
// 成功则返回0，失败返回-1
// 由于历史的原因，套接字函数中（如connect，bind等）使用的参数类型大多是sockaddr类型的。而如今进行套接字编程的时候大都使用sockaddr_in进行套接字地址填充
// http://blog.csdn.net/lgp88/article/details/7171924
connectid = connect(socketid, (struct sockaddr*)&sockinfo, sizeof(sockinfo));
```

Step 4: 发送get请求
```c
res = send(socketid, request, strlen(request), 0);
if (res == -1) {
    printf("向服务器发送GET请求失败\n");
    exit(1);
}
```

Step 5: 阻塞接收服务器响应
```c
// 接收的数据大小可能会大于缓冲区，可循环接收，当bufflen为0即读取完毕
int bufflen = recv(socketid, text, TEXT_BUFFSIZE, 0);
```

Step 6: 输出重定向

# 参考文章
- http://net.pku.edu.cn/~yhf/linux_c/
- http://blog.csdn.net/xrb66/article/details/6048399
- http://blog.csdn.net/gogor/article/details/5896931

# License
[WTFPL](http://www.wtfpl.net/txt/copying/)
