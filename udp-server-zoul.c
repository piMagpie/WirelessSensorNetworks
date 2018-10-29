#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h> /*ipv6*/
#include <netinet/in.h> /*ipv6*/
#include <string.h>
#include <stdlib.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 5678

void diep(char *s)
{
  perror(s);
  exit(1);
}

int main(void)
{
  /*struct sockaddr_in si_me, si_other;*/
  struct sockaddr_in6 si_me; /*creates address structure*/
  struct sockaddr_in6 si_other; /*creates address structure*/
  int s, i, slen=sizeof(si_other);
  char buf[BUFLEN];
  char straddr[INET6_ADDRSTRLEN];

  if ((s=socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP))==-1) /* create the socket*/
    diep("socket");

  /*memset((char *) &si_me, 0, sizeof(si_me));*/
  memset((char *)&si_me, 0, sizeof(si_me));
  si_me.sin6_family = AF_INET6;
  /*si_me.sin6_addr.s_addr = htonl(INADDR_ANY);*/
  si_me.sin6_addr = in6addr_any;
  si_me.sin6_port = htons(PORT); /* udp server port*/
  
  if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me))==-1)
      diep("bind");
 
  for (i=0; i<NPACK; i++) {
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)==-1)
      diep("recvfrom()");
    /*printf("Received packet from %s:%d\nData: %s\n\n", */
    printf("Received packet from %s\nData: %s\n\n", 
            inet_ntop(AF_INET6, &si_other.sin6_addr, straddr, sizeof(straddr)), buf);
            /*inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);*/
  }
  
  close(s);
  return 0;
  }