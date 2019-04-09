/* TCP回射客户程序*/

#include<unistd.h>//for read write
#include<netinet/in.h> //for sockaddr_in
#include<arpa/inet.h> //for inet_pton
#include<stdio.h>  //for stdion stdout
#include<string.h>
#include<iostream>

using namespace std;
#define MAXLINE 100

int SERVER_PORT = 21;
char SERVER_ADDR[] = "10.8.225.94";

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (fgets(sendline,MAXLINE,fp)!=NULL)
	{
	//	cout<<"fgets success!"<<endl;
		write(sockfd, sendline, strlen(sendline));
	//	cout<<"write success!"<<endl;
		if (read(sockfd, recvline, MAXLINE) == 0)
		{
			cout << "server terminated prematurely";
		}
	//	cout<<"read success!"<<endl;
		fputs(recvline, stdout);

	}
  //	cout<<"fgets error"<<endl;

}

int main()
{
	int sockfd;
	sockaddr_in servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &servaddr.sin_addr);
	connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));
	str_cli(stdin, sockfd);
//	cout<<"error"<<endl;
return 0;
