#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "comm.h"

static char buf[JOBS_AT_A_TIME * 256 + 1] = "";
static const char blank[] = " \t\n\r\0";

static int line_parser(char** s1, char** s2)
{
  int i;
  /* input offset */
  int offset = 0;
  /* input length */
  int len = strlen(buf);
  /* output pointer */
  char* o_pointer;

  for (i = 0; i < JOBS_AT_A_TIME; i ++) {
    /* prefix blanks */
    while (offset != len && index(blank, buf[offset]) != NULL)
      offset ++;
    if (offset == len) break;

    /* read to s1[i] */
    o_pointer = (char*) s1 + i * 256;
    while (offset != len && index(blank, buf[offset]) == NULL) {
      *o_pointer = buf[offset++];
      o_pointer ++;
    }
    if (offset == len) break;
    
    *o_pointer = '\0';

    /* intermedia blanks */
    while (offset != len && index(blank, buf[offset]) != NULL) 
      offset ++;
    
    if (offset == len) break;

    o_pointer = (char*) s2 + i * 256;
    while (offset != len && index(blank, buf[offset]) == NULL) {
      *o_pointer = buf[offset++];
      o_pointer ++;
    }
    *o_pointer = '\0';
    
    if (offset == len) {i ++; break;}

    while (offset != len && index(blank, buf[offset]) != NULL) 
      offset ++;

    if (offset == len) {i ++; break;}
  }

  return i;
}

static int sock_read(int sock)
{
  int c, total;

  c = 0;
  total = 0;
  bzero(buf, JOBS_AT_A_TIME * 256);
  do {
    c = read(sock, buf + total, JOBS_AT_A_TIME * 256 - total);
    total += c;
  } while (c > 0);

  if (total <= 0) {
    perror ("read nothing");
    return 0;
  } 

  buf[total] = '\0';

  return total;
}

int refill(const char* host, char** s1, char** s2)
{
  int sock;
  struct sockaddr_in servername;
  servername.sin_family = AF_INET;
  servername.sin_port = htons(PORT);
  struct hostent *hp = gethostbyname(host);
  bcopy(hp->h_addr, &(servername.sin_addr.s_addr), hp->h_length);

  /* Create the socket. */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
      perror ("Cannot create socket");
      return 0;
  }

  /* Connect to the server. */
  int r = connect(sock, (struct sockaddr*) &servername, sizeof(servername));
  if (r < 0) {
    printf("%d\n", errno);
    perror ("connect (client)");
    return 0;
  }

  int lines_read = 0;
  if (sock_read(sock)) 
    lines_read = line_parser(s1, s2);
  close(sock);

  return lines_read;
}
