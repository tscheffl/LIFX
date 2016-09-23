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


void sendMessage(int , struct sockaddr_in);

int main()
{
    int s;
    int yes=1;   
    struct sockaddr_in svc;
    
    //Create socket
    CHECK(s=socket(AF_INET, SOCK_DGRAM,0), "DGRAM socket");
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int) );
    
    
    svc.sin_family = AF_INET;
    svc.sin_addr.s_addr = inet_addr("192.168.0.255");
    svc.sin_port = htons(PORT);
    bzero(&svc.sin_zero,8);
    
    sendMessage(s, svc);
    
    
    close(s);
    return 0;
}

void sendMessage(int s, struct sockaddr_in svc)
{
    
    char buffer[BUFF_SIZE];
    int offset;
    
    lx_protocol_header_t hd;
    lx_color_t color;
    
    int i = 0,j=0;
    int l_svc=sizeof(svc);

    
    //set to Zero
    memset(&hd,0,sizeof(lx_protocol_header_t));
    memset(&color,0,sizeof(lx_color_t));
    
    //Setting the header
    //---frame---
    hd.protocol = 1024;
    hd.addressable = 1;
    hd.tagged = 1;
    hd.origin = 0;
    hd.source = 1;
    
    
    
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
        hd.reserved[i] = 0;
    }
    
    hd.res = 0;
    hd.ack_required = 0;
    hd.res_required = 0;
    hd.sequence = 2;
    
    //---protocol header---
    hd.resa = 0;
    
    /*** Device messages ***/
    
    //	hd.type = 2;  	/*GetService Message*/
    //	hd.type = 20;  	/*GetPowerStatus Message*/
    //	hd.type = 21;  	/*SetPowerStatus Message*/
    
    /*** Light messages ***/
    
    //	hd.type = 101;  	/*GetLightStatus Message*/
    //	hd.type = 102;  	/*SetColor Message*/
    //	hd.type = 116;  	/*GetLightLevel Status Message*/
    //	hd.type = 117;  	/*SetLightLevel Message*/
    
    hd.type = 102;
    hd.resb = 0;
    
    //---payload---
    
    //switch light on or of
    lx_powerlevel = 65535; //0 - off, 65535 - on
    
    //set hsbk values
    color.hue = 11000;
    color.saturation = 65535;
    color.brightness = 65535;
    color.kelvin = 4000;
    color.duration = 1000;
    
    // we copy the payload into a buffer (serialisation)
    // LIFX requires     
    offset = sizeof(hd);
    
    // send color message
    memcpy(buffer + offset, &color, sizeof(color));
    offset = offset + sizeof(color);
    
    // send power message
    //    memcpy(buffer + offset, &lx_powerlevel, sizeof(lx_powerlevel));
    //    offset = offset + sizeof(lx_powerlevel);

    
    //---frame size---
    // only now do we have the complete framesize
    // so we can also copy the frame header
    hd.size = offset;
    memcpy(buffer, &hd, sizeof(hd));
    
    //Sending
    CHECK(sendto(s, buffer, offset, 0, (struct sockaddr*) &svc, l_svc), "Send problem");    
    
    printf("LIFX message sent\n");
    
    return;
}