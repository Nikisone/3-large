//
//  main.c
//  Tehtävä 3
//
//  Created by Olli-Pekka Pennanen on 20.3.2021.
//

#include <stdio.h>
#include <sys/socket.h>  // defines socket, connect, ...
#include <netinet/in.h>  // defines sockaddr_in
#include <string.h>      // defines memset
#include <stdio.h>       // defines printf, perror, ...
#include <arpa/inet.h>   // inet_pton, ...
#include <unistd.h>

#define MAXLINE 1000

unsigned int blockSize(char* b)
{
    unsigned int s = 0;
    
    s += (b[0] << 24) & 0xFF000000;
    s += (b[1] << 16) & 0xFF0000;
    s += (b[2] << 8) & 0xFF00;
    s += b[3] & 0xFF;
        
    return s;
}

int main(int argc, const char **argv) {
    
    int sockfd;
    //char recvline[MAXLINE + 1];
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;
    
    const char *address = "130.233.154.208";
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        return 1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5000);
    
    if (inet_pton(AF_INET, address, &servaddr.sin_addr) <= 0)
    {
        fprintf(stderr, "inet_pton error for %s\n", address);
        return 1;
    }
    
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect error");
        return 1;
    }
    
    send(sockfd, "786719\n", strlen("786719\n"), 0);
    send(sockfd, "3-large\n", strlen("3-large\n"), 0);

    long bytes = 0;
    unsigned int len = -1;

    while (len != 0) {
        bytes = recv(sockfd, recvline, 500, 0);
        len = blockSize(recvline);

        while (bytes < len) {
            bytes += recv(sockfd, recvline, 500, 0);
        }

        char buffer[4] = {0};
        
        buffer[0] = ((len & 0xFF000000) >> 24) & 0xFF;
        buffer[1] = ((len & 0xFF0000) >> 16) & 0xFF;
        buffer[2] = ((len & 0xFF00) >> 8) & 0xFF;
        buffer[3] = len & 0xFF;
    
        send(sockfd, buffer, 4, 0);
    }

    return 0;
}

