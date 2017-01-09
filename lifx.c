/* Code inspired by this source:
 * https://community.lifx.com/t/recvfrom-doesnt-answer-the-right-infos/912
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "lifx.h"
#include "lifx-lib.h"

#define BUFF_SIZE 520


void sendMessage(int s, struct sockaddr_in svc, char* buffer, size_t length)
{    
    //Sending
    CHECK(sendto(s, buffer, length, 0, (struct sockaddr*) &svc, sizeof(svc)), "Send problem");
    
    printf("LIFX message sent\n");    
    return;
}

int main()
{
    int sockfd;
    int broadcast_enable=1;   
    struct sockaddr_in svc;

    char buffer[BUFF_SIZE];
    size_t length;
    
    //Create socket
    CHECK(sockfd=socket(AF_INET, SOCK_DGRAM,0), "DGRAM socket");
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(int) );
    
    svc.sin_family = AF_INET;
    svc.sin_addr.s_addr = inet_addr("192.168.0.255");
    svc.sin_port = htons(PORT);
    bzero(&svc.sin_zero,8);

    length = buildLIFX_PowerMessage(buffer, 1); //1 - on, 0 - off
    sendMessage(sockfd, svc, buffer, length);
    length = buildLIFX_ColorMessage(buffer, "red", 100); //in percent
    sendMessage(sockfd, svc, buffer, length);
    sleep(2);
    length = buildLIFX_ColorMessage(buffer, "blue", 100); //in percent
    sendMessage(sockfd, svc, buffer, length);
    sleep(2);
    length = buildLIFX_ColorMessage(buffer, "green", 100); //in percent
    sendMessage(sockfd, svc, buffer, length);
    sleep(2);
    length = buildLIFX_ColorMessage(buffer, "magenta", 10); //in percent  
    sendMessage(sockfd, svc, buffer, length);
    sleep(5);  
    length = buildLIFX_PowerMessage(buffer, 0); //1 - on, 0 - off
    sendMessage(sockfd, svc, buffer, length);

    close(sockfd);
    return 0;
}
