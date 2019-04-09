/* TCP回射服务端程序*/


#include<unistd.h>//for read write
#include<netinet/in.h> //for sockaddr_in
#include<arpa/inet.h> //for inet_pton
#include<stdio.h>  //for stdion stdout
#include<errno.h>  //for  errno
#include<sys/wait.h> //for wait
//#include<bits/sigaction.h> //for sigaction //包含多了会发生冲突
#include<signal.h> //for signal
#include<string.h>
#include<iostream>

using namespace std;
#define MAXLINE 100
typedef void sigfun(int);

int SERVER_PORT = 21;
int LISTENQ = 20;

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	while ((n = read(sockfd, buf, MAXLINE)) > 0)
	{
		write(sockfd, buf, n);

		if (n < 0 && errno == EINTR)
			goto again;
		else if (n < 0)
		{
			cout << "error" << endl;
		}
	}
}

sigfun * signal(int signo, sigfun * func)
{

	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);//mask设为空集，意味着在该信号处理函数运行期间，不阻塞额外的信号
	act.sa_flags = 0;
	/*
	当阻塞于某个慢系统调用的一个进程捕获某个信号且响应信号处理函数返回时
	该系统调用可能返回一个EINTR错误。
	有些内核自动重启某些被中断的系统调用，不过为了方便移植，当我们编写捕获信号的程序时，要对慢系统调用返回EINTR错误有所准备
	*/
	if (signo == SIGALRM)
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif // SA_INTERRUPT

	}
	else
	{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif // SA_RESTART
	}
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);

}
void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	/*
	pid = wait(&stat);      //僵死进程会占用内核空间
	printf("child %d terminated\n", pid);
	使用wait并不足以防止出现僵死进程。因为信号处理函数对同一种信号只处理一次，因为Unix信号一般是不排队的。
	所以是结果是不确定的。
	*/

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("child %d terminated\n", pid);
	}
	/*
	正确做法应该是用waitpid，并指定WHOHANG选项，告知在拥有尚未终止的子进程在运行时不要阻塞
	*/
	return;
}



int main()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t client;
	sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);
	bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	signal(SIGCHLD, sig_chld);
	while (true)
	{
		client = sizeof(cliaddr);
		connfd = accept(listenfd, (sockaddr *)&servaddr, &client);
		if ((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			//      cout<<"child has been closed!"<<endl;
			exit(0);
			//子进程结束后，依然处于僵死状态，占用系统资源
		}
		close(connfd);//如果不关闭，则会一直处于close_wait状态
	}
	return 0;
}
