#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //defines the structure hostent
   
void error (char *msg)
{
        perror(msg);   
        exit(0);
}

// har mer utfyllende kommentarer i serverkoden. Ikke vits å kommentere dobbelt opp på ting som er like

int main(int argc, char *argv[])
{
     int sockfd, portno, n;
     struct sockaddr_in serv_addr; 
     struct hostent *server; // lager variablen server som en pointer til en structure av type hostent
     // hostent har følgende felt:
          // *h_name : navnet på hosten
          // **h_aliases : en liste av alternative navn på hosten
          // h_addr : addressetype. som regel AF_INET altså IPv4
          // h_length : length in bytes of the address
          // **h_addr_list, a pointer to a list of netwrok addresses for the named host
     
     char buffer[256];
     if (argc < 3) {
          fprintf(stderr,"usage %s hostname port\n", argv[0]);
          exit(0);
     }
     portno = atoi(argv[2]);
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
         error("ERROR opening socket");
         
     server = gethostbyname(argv[1]); // forsøker å hente hostent structure fra navnet til serveren som defineres i kallet på client
     // funksjonen *gethostbyname(char, *name) er definert i netdb.h til å returnere en struct hostent. Returner altså en pointer til en hostent structure for the named host.
     // hvis hostnavnet ikke er kjent lokalt bruker den DNS
     if (server == NULL) {
         fprintf(stderr,"ERROR, no such host\n");
         exit(0);
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     
     // siden hostent->h_addr er en character string. brukes bcopy(char *s1, char* s2, int length) til å kopiere length bytes fra s1 til s2
     bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
     serv_addr.sin_port = htons(portno);
     if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
         error("ERROR connecting");
     printf("Please enter the message: ");
     bzero(buffer,256);
     fgets(buffer,255,stdin); // fgets leser melding fra stdin altså terminal, og putter verdiene i buffer
     printf("%s", &buffer);
     char * name = &buffer;
     name[strlen(name)-1] = '@';
     
     for (int i = 0; i < strlen(name); i++)
     {   
          n = write(sockfd, &name[i], 1); // NB! Må bruke & for å hente ut et spesfikt element i buffer. 
          if (n < 0) 
               error("ERROR writing to socket");
          sleep(1);
 
     }
     //n = write(sockfd, NULL, 1); 
     char* nextletterstring[0];
     bzero(buffer,256);

     while ((n = read(sockfd, buffer, 50, 0)) > 0)
        {
                if ((strncmp(&buffer, "@", 1) == 0)) break;
                strncat(nextletterstring, &buffer, 1);
                printf("%s, %s\n", buffer, nextletterstring);
                if (n < 0) error("ERROR reading from socket");
                bzero(buffer,256); // 
        }   
     return 0;
}

