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

#define BUFF_SIZE 520

void initLIFX_Header(lx_protocol_header_t* hd, int type)
{
    int i = 0;
    //set to Zero
    memset(hd,0,sizeof(lx_protocol_header_t));
    
    //Setting the header
    //---frame---
    hd->protocol = 1024;
    hd->addressable = 1;
    hd->tagged = 1;
    hd->origin = 0;
    hd->source = 1;
      
    //---frame address--- //d0:73:d5:01:c5:e1
    // hd.target[0] = 208;
    // hd.target[1] = 115;
    // hd.target[2] = 213;
    // hd.target[3] = 1;
    // hd.target[4] = 197;
    // hd.target[5] = 225;
    // hd.target[6] = 0;
    // hd.target[7] = 0;
    
    // normally not necessary, because struct has been set to Zero
    for (i=0; i<6; i++)
    {
        hd->reserved[i] = 0;
    }
    
    hd->res = 0;
    hd->ack_required = 0;
    hd->res_required = 0;
    hd->sequence = 2;
    
    //---protocol header---
    hd->resa = 0;
    hd->type = type;
    hd->resb = 0;
}

int buildLIFX_ColorMessage(char* buffer, char* color, int brightness)
{
    int offset;
    lx_protocol_header_t hd;
    lx_color_t lx_color;
    
    initLIFX_Header(&hd, HEADER_TYPE_SET_COLOR);

    //---payload---    
    //set to Zero
    memset(&lx_color,0,sizeof(lx_color_t));
    
    //set hsbk values
    if(strcmp(color,"red")==0) 
    {
        lx_color.hue = 0;
    } 
    else if (strcmp(color,"yellow")==0) 
    {
        lx_color.hue = 11000; 
    }
    else if (strcmp(color,"green")==0) 
    {
        lx_color.hue = 22000; 
    }
    else if (strcmp(color,"blue")==0) 
    {
        lx_color.hue = 44000; 
    }
    else  
    {
        printf("Error: Colorvalue not found!!!\nDefault to red\n");
        lx_color.hue = 0; 
    }
    
    lx_color.saturation = 65535;
    lx_color.brightness = (int)(655.35*brightness);
    lx_color.kelvin = 4000;
    lx_color.duration = 100; //time between color changes im Milliseconds
    
    // we copy the payload into a buffer (serialisation)
    // LIFX requires     
    offset = sizeof(hd);
    
    // send color message
    memcpy(buffer + offset, &lx_color, sizeof(lx_color));
    offset = offset + sizeof(lx_color);
     
    //---frame size---
    // only now do we have the complete framesize
    // so we can also copy the frame header
    hd.size = offset;
    memcpy(buffer, &hd, sizeof(hd));
    return offset;
}

int buildLIFX_PowerMessage(char* buffer, int state)
{
    int offset;
    lx_protocol_header_t hd;

    initLIFX_Header(&hd, HEADER_TYPE_SET_POWER);

    //---payload---
    //switch light on or of
    if(state == 0) 
    {
        lx_powerlevel = 0; //0 - off, 65535 - on
    } 
    else if (state==1) 
    {
        lx_powerlevel = 65535; //0 - off, 65535 - on
    }
    else  
    {
        printf("Error: Powerstate not defined!!!\nDefault to off\n");
        lx_powerlevel = 0;
    }
    
    // we copy the payload into a buffer (serialisation)
    // LIFX requires     
    offset = sizeof(hd);

    // send power message
    memcpy(buffer + offset, &lx_powerlevel, sizeof(lx_powerlevel));
    offset = offset + sizeof(lx_powerlevel);

    //---frame size---
    // only now do we have the complete framesize
    // so we can also copy the frame header
    hd.size = offset;
    memcpy(buffer, &hd, sizeof(hd));
    return offset;
}

void sendMessage(int s, struct sockaddr_in svc, char* buffer, int length)
{    
    int l_svc=sizeof(svc); 
    //Sending
    CHECK(sendto(s, buffer, length, 0, (struct sockaddr*) &svc, l_svc), "Send problem");
    
    printf("LIFX message sent\n");    
    return;
}

int main()
{
    int sockfd;
    int broadcast_enable=1;   
    struct sockaddr_in svc;

    char buffer[BUFF_SIZE];
    int length;
    
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