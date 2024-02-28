// ******************************* sendPITScmd.c *****************************
//
// Send a Pi In The Sky uplink command to the gateway where it will be
// forwarded to the PITS tracker in the next uplink period.
//
// Reads the 1st line form the file passed as an argument
// Opens a TCP/IP socket to the gateway
// Sends the line read to the gateway
//
// Usage: sendPITScmd file
// Where file contains a single line (including newline)
// See https://github.com/daveake/HAB-Documentation/blob/main/LoRa%20Gateway%20Protocols.pdf
// and https://www.daveakerman.com/?p=2790
// for more details
//

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 6004
#define ADDRESS "127.0.0.1"

int main(int argc, char *argv[])
{
        FILE *fp;          // input file pointer
        int fd;            // socket to send on
        char linebuf[128]; // line to send
        int st;            // connect() status
        int len;           // length of transfer
        struct sockaddr_in serv_addr;

        if (argc != 2)
        {
             printf("%s usage: %s filename\n",argv[0],argv[0]);
             exit(1);
        }

        if ((fp = fopen(argv[1],"r")) == NULL)
        {
             printf("file %s did not open for read\n",argv[1]);
             exit(1);
        }

        if (fgets(linebuf,sizeof(linebuf),fp) == NULL)
        {
             printf("Cannot read a line from %s\n",argv[1]);
             exit(1);
        }

        fclose(fp); // all done with file
        printf("read \"%s\" from %s\n",linebuf,argv[1]);

        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
             printf("Cannot create socket\n");
             exit(1);
        }

        memset((char*)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) <= 0)
        {
             printf("Invalid address/ Address not supported \n");
             exit(1);
        }

        /* connect to server */
        if (st = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
             printf("socket connect failed status:%d\n",st);
             exit(1);
        }

        // send the line to the socket
        len = write(fd,linebuf,strlen(linebuf));
        if (len != strlen(linebuf))
        {
             printf("Only %d of %d bytes written\n",len,strlen(linebuf));
             exit(1);
        }

        printf("\"%s\" sent OK\n",linebuf);
        close(fd);
}
