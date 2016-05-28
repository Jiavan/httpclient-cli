// 建立socket所需头文件
// #include<sys/types.h>
// #include<sys/socket.h>
// 包含sockaddr_in的头
// #include <netinet/in.h>
// inet_pton所需头
// #include <arpa/inet.h>
// write 函数
// #include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT 80
#define IP "61.147.124.120"
int main(int argc, char const *argv[]) {

	// int socket(int domain,int type,int protocol)
	// 参数domain 指定使用何种的地址类型，以下为IPv4
	// SOCK_STREAM 提供双向连续且可信赖的数据流
	// protocol用来指定socket所使用的传输协议编号
	// return 成功则返回socket处理代码，失败返回-1
	int socketid = socket(AF_INET, SOCK_STREAM, 0);
	char str1[4096], str2[4096], *str;
	int len, res;

	// sockaddr_in 保存链接信息的结构体
	struct sockaddr_in serveraddr;

	if (socketid < 0) {
		printf("创建网络链接失败，即将退出-network error\n");
		exit(1);
	} else {
		printf("创建网络链接成功\n");
	}

	// struct sockaddr_in 在netinet/in.h中定义
	// {
	// 	short sin_family;/*Address family一般来说AF_INET（地址族）PF_INET（协议族）*/
	// 	unsigned short sin_port;/*Port number(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)*/
	// 	struct in_addr sin_addr;/*IP address in network byte order（Internet address）*/
	// 	unsigned char sin_zero[8];/*Same size as struct sockaddr没有实际意义,只是为了　跟SOCKADDR结构在内存中对齐*/
	// };

	// 将链接信息结构体清空，构造链接信息
	// inet_pton 将点分十进制IP转换为二进制
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IP, &serveraddr.sin_addr) < 0) {
		printf("IP转换失败\n");
		exit(1);
	}

	// 成功则返回0，失败返回-1
	// 由于历史的原因，套接字函数中（如connect，bind等）使用的参数类型大多是sockaddr类型的。而如今进行套接字编程的时候大都使用sockaddr_in进行套接字地址填充
	// http://blog.csdn.net/lgp88/article/details/7171924
	if (connect(socketid, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
		printf("不能连接到远端服务器\n");
		exit(1);
	}
	printf("与服务器建立socket链接成功\n");

	memset(str2, 0, 4096);
	strcat(str2, "jiavan.com");
	str = (char *)malloc(128);
	len = strlen(str2);
	sprintf(str, "%d", len);

	memset(str1, 0, 4096);
    strcat(str1, "POST /webservices/qqOnlineWebService.asmx/qqCheckOnline HTTP/1.1\n");
    strcat(str1, "Host: www.webxml.com.cn\n");
    strcat(str1, "Content-Type: application/x-www-form-urlencoded\n");
    strcat(str1, "Content-Length: ");
    strcat(str1, str);
    strcat(str1, "\n\n");

    strcat(str1, str2);
    strcat(str1, "\r\n\r\n");
    printf("%s\n",str1);

    // write()会返回实际写入的字节数。当有错误发生时则返回-1，错误代码存入errno中
    res = write(socketid, str1, strlen(str1));
    if (res < 0) {
    	printf("发送消息失败，错误码: %d，错误信息: %s\n", errno, strerror(errno));
    	exit(1);
    } else {
    	printf("发送消息成功，共发送了%d byte\n", res);
    }

	return 0;
}




 // int cc;
 //        if(-1 == (cc = connect(cfd,(struct sockaddr*)&cadd,(socklen_t)sizeof(cadd))))
 //        {
 //                printf("connect failed of client!\n");
 //                exit(1);
 //        }
 //        printf("connect success!\n");
 
 //        //向服务器发送url请求的request
 //        int cs;
 //        if(-1 == (cs = send(cfd,request,strlen(request),0)))
 //        {
 //                printf("向服务器发送请求的request失败!\n");
 //                exit(1);
 //        }
 //        printf("发送成功,发送的字节数:%d\n",cs);
 
 //        //客户端接收服务器的返回信息
 //        memset(text,0,BUFSIZE);
 //        int cr;
 //        if(-1 == (cr = recv(cfd,text,BUFSIZE,0)))
 //        {
 //                printf("recieve failed!\n");
 //                exit(1);
 //        }
 //        else
 //        {
 //                printf("receive succecc!\n");
 //        }
 //        close(cfd);
 // }