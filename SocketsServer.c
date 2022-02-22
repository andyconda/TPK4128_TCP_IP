  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>

  void error(char *msg)
  {
      perror(msg);
      exit(1);
  }

int main(int argc, char *argv[])
{
        int sockfd, newsockfd, portno, clilen;
        // sockfd stores value returned by socket system call
        // newsockfd stores value returned by accept system call
        // portno is port number
        // clilen is size of client address
        char buffer[256]; // The server reads characters from socket connection into this
        struct sockaddr_in serv_addr, cli_addr;
        // defines server and client address structures using the sock_addr_in internet address structure
        int n; // number of characters read or written by read() and write()
        
        if (argc < 2) { 
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }
        
        sockfd = socket(AF_INET, SOCK_STREAM, 0); // blir socket(IPv4, StreamSocket, TCP)
        if (sockfd < 0) 
           error("ERROR opening socket");
           
        bzero((char *) &serv_addr, sizeof(serv_addr));
        // bzero sets all values in a buffer to zero. Takes two arugment, pointer to the buffer and size of buffer
        // altså intialisere den serv_addr til 0
        
        portno = atoi(argv[1]); // retrieves portnumber used to call program. atoi() converts from string of digits to integer
        
        //assigns values to serv_addr
        serv_addr.sin_family = AF_INET; // IPv4
        serv_addr.sin_port = htons(portno); // htons() converts port no from host byte order to network byte order. Host og network bytes bruker forskjellig rekkefølge i minnet
        serv_addr.sin_addr.s_addr = INADDR_ANY; // Henter IP-addressen for serveren fra symbolic constant IN_ADDR_ANY
        
        // binds socket to socket address
        if (bind(sockfd, (struct sockaddr *) &serv_addr, // må typecaste &serv_addr fra type sockaddr til type sockaddr_in
                 sizeof(serv_addr)) < 0) 
                 error("ERROR on binding"); // throws error if result from bind is < 0 altså failed
        
        listen(sockfd,5); // begynner å høre på socketen. 5 er typisk verdi for backlog queue
        
        clilen = sizeof(cli_addr) ; // oppbevarer størrelsen på clientaddress
        
        // blokkerer prosessen til connection har blitt etablert med en server
        // returnerer ny file descriptor newsockfd, og alle kommunikasjon på forbindelsen bør gjøres på newsockfd fra nå
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 
        if (newsockfd < 0) 
             error("ERROR on accept");
             
        bzero(buffer,256); // initialiserer bufferen til 0
        n = read(newsockfd,buffer,255); // leser opp til 255 bytes på bufferen fra newsockfd. returnerer antall characters lest til n. read() blokkerer i likhet med accept() prosessen til det er noe å lese
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer); // printer fra bufferen
        n = write(newsockfd,"I got your message",18); // skriver melding på forbindelsen til klienten
        if (n < 0) error("ERROR writing to socket");
        return 0; 
}


