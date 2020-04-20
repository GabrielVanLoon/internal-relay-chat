//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
     
#define TRUE   1  
#define FALSE  0  
int PORT = 9003;  

fd_set readfds;         //set of socket descriptors  

int addrlen, new_socket , client_socket[30] ,  
        max_clients = 30 , activity, i , valread , sd;   
int max_sd;   
        
char buffer[4096];      // data buffer of 1K   
char message[4096] = "Welcome !!!";  // Serve message  

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void sucess(const char *msg) {
    printf("%s\n", msg);
    fflush(stdout);
}

void create_socket(int *master_socket, int num_listen) {
    int opt = TRUE;  
    struct sockaddr_in address;   

    //create a master socket  
    if((*master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        error("[x] Socket creating failed");   
    }   
     
    //set master socket to allow multiple connections ,  
    if(setsockopt(*master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )   
    {   
        error("[x] setsockopt");   
    }   
     
    //type of socket created  
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 9002  
    if (bind(*master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)   
    {   
        error("[x] bind failed on this port");   
    }   
    printf("[+] Listener on port %d\n", PORT);   
         
    //try to specify maximum of num_list pending connections for the master socket  
    if (listen(*master_socket, num_listen) < 0)   
    {   
        error("[x] Listen");   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    sucess("[+] Waiting for connections ...");  

    return;
}   

void build_sets(int master_socket) {
    
    //clear the socket set  
    FD_ZERO(&readfds);   
    //add master socket to set  
    FD_SET(master_socket, &readfds);   
    max_sd = master_socket;   
            
    //add child sockets to set  
    for (int i = 0 ; i < max_clients ; i++)   
    {   
        //socket descriptor  
        sd = client_socket[i];   
                
        //if valid socket descriptor then add to read list  
        if(sd > 0)   
            FD_SET( sd , &readfds);   
                
        //highest file descriptor number, need it for the select function  
        if(sd > max_sd)   
            max_sd = sd;   
    } 
    return;
}

int main(int argc , char *argv[])   
{   
    int master_socket, num_listen; 
    struct sockaddr_in address;   
    
    if(argc < 2)
    {
        error("[x] No port given.");
    }

    PORT = atoi(argv[1]);
    create_socket(&master_socket, num_listen);  
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   

    while(TRUE)   
    {      
        build_sets(master_socket);

        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   

            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port));   
           
            //send new connection greeting message  
            if(send(new_socket, message, strlen(message), 0) != strlen(message))   
            {   
                perror("send");   
            }   
                 
            puts("Welcome message sent successfully");   
                 
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if(client_socket[i] == 0)   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   

                    // FD_SET
                    // FD_SET(new_socket, &readfds);

                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   

            if(sd == master_socket) continue;
                 
            if (FD_ISSET(sd , &readfds))   
            {    
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read(sd , buffer, sizeof(buffer))) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , 
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    for(int i = valread; i <= 4096; i++){
                        buffer[i] = '\0';
                    }  
                    printf("Client(%d): %s\n", sd, buffer);
                    for(int i = 0; i < max_clients; i++){
                        if(client_socket[i] > 0 && client_socket[i] != sd){
                            send(client_socket[i] , &buffer , sizeof(buffer) , 0 );
                        }
                    }
                }   
            }   
        }   
    }   
         
    return 0;   
}   