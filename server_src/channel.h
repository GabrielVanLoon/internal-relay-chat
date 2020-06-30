#ifndef CHANNEL_CONN_H
#define CHANNEL_CONN_H 8080

    /**
     * Bibliotecas importadas
     * ----------------------
     * 
     * Devido ao uso da biblioteca "pthread.h", é necessário que o código
     * seja compilado com o parâmetro -pthread junto ao g++
    */
    
        #include <stdio.h> /* sprintf */
        #include <string.h> /* memset */
        #include <stdlib.h> /* atoi */
        #include <netdb.h> /* gethostbyname */
        #include <netinet/in.h> /* struct sockaddr_in */
        #include <unistd.h> /* close */
        #include <sys/socket.h>
        #include <sys/types.h>
        #include <pthread.h> /* pthread_t */
        #include <iostream>
        #include <map>
        #include <queue>
        #include <set>
<<<<<<< HEAD
        #include <string> 	/* string */

        using namespace std;
=======

        using namepsace std;
>>>>>>> eaf110708d37bfb46f7d9d0709dd56ca9cf0e312

    /**
     * Bibliotecas do própio projeto
     * -----------------------------
    */
        #include "clients.h"
        #include "message.h"

    /**
     * Constantes e Macros
     * -------------------
     * 
     * ERROS
     * Valores que indicam o local/motivo do erro
     * Obs.: Por padrão, o valor de SUCESS será 0 e valores de erro
     * serão maiores ou iguais a 1
     * 
     * SUCCESS e FAIL
     * Valores genéricos de sucesso e erro
     * 
     * ID_INITIALAZED
     * Id do primeiro cliente do CHANNEL. Após o estabelecimento da conexão,
     * com estre cliente, o valor será incrementado a cada nova conexão
     * 
     * MAX_CLIENTS
     * Número máximo de clientes aceitos no servidor
    */

        #define ERRO_MAX_CLIENTS 1
        #define ERRO_SOCKET 2
        #define ERRO_MULTIPLE_CONNECTION 3
        #define ERRO_BIND 4
        #define ERRO_LISTEN 5
        #define ERRO_PORT 6
        #define ERRO_MAX_CLIENT_REACHED 7
        #define ERROR_CREATE_CHANNEL 8
        #define ERROR_USER_BANNED 9
        #define SUCCESS 0
        #define FAIL 1
        
        #define ID_INITIALAZED 10
        
        #define MAX_SIZE_NAME 30
        #define MAX_CLIENTS 20

        using namespace std;

    /**
     * @struct
     * typedef struct _CHANNEL_conn CHANNEL_conn
     * ---------------------------------------
     * 
     * O canal recebe a mensagem do cliente e envia para os membros do canal.
     * 
     * atributos do canal
     * --------------------
     * - participants: porta em que está conectado o servidor.
     * - size_channel: socket(valor) utilizado para abrir o servidor.
     * - name_channel: número máximo de conexões permitidas do servidor, a qual foi exigida
     *   por quem iniciou o servidor.
     * 
     * buffers de conexão
     * ------------------
     */
    typedef struct _CHANNEL_conn
    {
        // atributos do canal
        string name_channel;
        string name_admin;

        int size_participants;
        map<string, int> participants;                    // nickname, cl_socket 
        map<int, string> arrived;                         // position arrived, string client

        // TODO: Use the ipv4
        set<string> notAllowedParticipants;            // <nickname> dos clientes banidos.

        // buffers de conexão
        char *send_buff, *recv_buff;
        int  send_buff_size, recv_buff_size;
    } CHANNEL_conn;

    /**
     * @function
     * CHANNEL_conn conn_criar_CHANNEL(int cl_socket)
     * 
     * cl_socket: o id socket cliente
     * -------------------------------
     * 
     * Funcao responsavel por criar um canal e retorná-lo
     * São inicialiazadas os atributos do Canal
     * 
     * return: CHANNEL_conn com atributos inicializados
    */
   
    // CHANNEL_conn* conn_criar_CHANNEL(string name_channel, client* clt);
    // void CHANNEL_destroy(CHANNEL_conn* channel);
    // void CHANNEL_destroy_all();
    // int CHANNEL_remove_user(CHANNEL_conn* channel, client* clt);
    // int CHANNEL_add_user(CHANNEL_conn* channel, client* clt); 
    // void CHANNEL_join(string name_channel, client* clt); 
    // int CHANNEL_kick_user(CHANNEL_conn* channel, client* clt, string kick_nickname); 
    // int CHANNEL_unkick_user(CHANNEL_conn* channel, client* clt); 
    // bool CHANNEL_send_message(int cl_socket, char* buffer); 
    // void CHANNEL_send_message_all(CHANNEL_conn* channel, char* buffer);
    // void CHANNEL_send_message_one(CHANNEL_conn* channel, client* clt, char* buffer);
    // int CHANNEL_broadcast(CHANNEL_conn* channel, client* clt, int type, char* buffer); 
#endif