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

#define BUFF_SIZE   520
#define SERVER	"172.16.0.255"



void sendMessage(int sock, struct sockaddr_in servaddr, char* buffer, size_t length)
{    
    //Sending
    CHECK(sendto(sock, buffer, length, 0, (struct sockaddr*) &servaddr, sizeof(servaddr)), "Send problem");
    
    printf("LIFX message sent!\n");    
    return;
}

void printMessage(char buffer[], int i)
{
	printf("Response:\n");
	for (i = 0; i < BUFF_SIZE; i++)
	{
    	printf("%02X", buffer[i]);
	}
	printf("\n");
}

int main()
{
    int sockfd;
    int n;
    int broadcast_enable=1;   
    struct sockaddr_in servaddr;
    struct sockaddr_in client_addr;
    socklen_t addr_len=sizeof(client_addr);

    char buffer[BUFF_SIZE];
    size_t length;
    
    //Create socket
    CHECK(sockfd=socket(AF_INET, SOCK_DGRAM,0), "DGRAM socket");
    //enable Broadcast
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(int) );
	struct timeval tv = {
    	.tv_sec = 5
	};
	//set timeout to 5 sec
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER, &servaddr.sin_addr);
    servaddr.sin_port = htons(PORT);

	//power on
    length = buildLIFX_PowerMessage(buffer, 1); //1 - on, 0 - off
    sendMessage(sockfd, servaddr, buffer, length);

	//get statistics
    length = buildLIFX_GetPowerMessage(buffer); //Get power-status
    sendMessage(sockfd, servaddr, buffer, length);
    n = recvfrom(sockfd, (char *)buffer, BUFF_SIZE - 1, 
                MSG_WAITALL, (struct sockaddr *) &client_addr,
                &addr_len);
    printMessage(buffer,n);
    length = buildLIFX_GetLightStatus(buffer); //Get light-status
    sendMessage(sockfd, servaddr, buffer, length);
    n = recvfrom(sockfd, (char *)buffer, BUFF_SIZE - 1, 
                MSG_WAITALL, (struct sockaddr *) &client_addr,
                &addr_len);
    printMessage(buffer,n);
    length = buildLIFX_GetVersionMessage(buffer); //Get light-status
    sendMessage(sockfd, servaddr, buffer, length);
        n = recvfrom(sockfd, (char *)buffer, BUFF_SIZE - 1, 
                MSG_WAITALL, (struct sockaddr *) &client_addr,
                &addr_len);
    printMessage(buffer,n);

	//set Color values
    length = buildLIFX_ColorMessage(buffer, "red", 100); //in percent
    sendMessage(sockfd, servaddr, buffer, length);
    sleep(2);
    length = buildLIFX_ColorMessage(buffer, "blue", 100); //in percent
    sendMessage(sockfd, servaddr, buffer, length);
    sleep(2);
    length = buildLIFX_ColorMessage(buffer, "green", 100); //in percent
    sendMessage(sockfd, servaddr, buffer, length);
    sleep(2);
    length = buildLIFX_ColorMessage(buffer, "magenta", 10); //in percent  
    sendMessage(sockfd, servaddr, buffer, length);
    sleep(5);  
    length = buildLIFX_PowerMessage(buffer, 0); //1 - on, 0 - off
    sendMessage(sockfd, servaddr, buffer, length);

    close(sockfd);
    return 0;
}
