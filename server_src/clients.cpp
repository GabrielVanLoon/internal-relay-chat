#include "clients.h"

// Array com os structs dos clientes
client *cl_arr[MAX_CLIENTS];

/**
 * @functon CHANNEL_is_valid_nickname
 * @param { string } nickname nome a ser checado
 * 
 * Checa se o nome é valido.
 */
bool clt_is_valid_nickname(string nickname) {
    // if(is_valid_nickname() == false) return false;
    if(clt_get_by_nickname(nickname) != NULL && nickname != "invalid")
        return false;
    else return true;
}

// @Comentários em "clients.h"
client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket){
    client* cl = (client*) malloc(sizeof(client));
    string nickname = "invalid";
    struct hostent *host_entry; 
    char hostbuffer[256];
    
    // Recuperando o Ipv4
    gethostname(hostbuffer, sizeof(hostbuffer));
    host_entry = gethostbyname(hostbuffer); 
    if(host_entry == NULL)
        return NULL;

    cl->cl_address = address;
    cl->cl_socket = socket;
    cl->cl_id = id;
    cl->ip_address = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0])); 
    cl->sv_socket = sv_socket;
    cl->channel = NULL;
    cl->nickname = nickname;
    return cl;
}

// @Comentários em "clients.h"
int clt_add_queue(client* cl, pthread_mutex_t* mutex){

    pthread_mutex_lock(mutex);

    // Adiciona o cliente no primeiro espaco disponivel
    int index;
    for(index = 0; index < MAX_CLIENTS; index++){
        if(!cl_arr[index]){
            cl_arr[index] = cl;
            break;
        }
    }

    pthread_mutex_unlock(mutex);

    // Numero maximo de clientes ja foi atingido...
    if(index == MAX_CLIENTS)
        return FAIL;

    return SUCCESS;
}

// @Comentários em "clients.h"
void clt_destruir(int id){
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] != NULL && cl_arr[i]->cl_id == id) {
            close(cl_arr[i]->cl_socket);
            cl_arr[i] = NULL;
            free(cl_arr[i]);
            break;
        }
    }
}

// @Comentários em "clients.h"
void clt_destruir_clientes() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] != NULL) {
            close(cl_arr[i]->cl_socket);
            free(&(*cl_arr[i]));
        }
    }
}

// @Comentários em "clients.h"
client* clt_remove_queue(int id, pthread_mutex_t* mutex){
    
    pthread_mutex_lock(mutex);  

    // Procura pelo cliente, atraves de seu id, e o retira do array
    client* temp = NULL;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] && cl_arr[i]->cl_id == id) {
            temp = cl_arr[i];
            cl_arr[i] = NULL;
            break;
        }
    }

    pthread_mutex_unlock(mutex);

    return temp;
}

bool clt_validate_nickname(string nickname) 
{
    return true;
}

client* clt_get_by_nickname(string cli_nickname) 
{
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] && cl_arr[i]->nickname == cli_nickname) {
            return cl_arr[i];
        }
    }
    return NULL;
}

// @Comentários em "clients.h"
client* clt_get_by_id(int cli_id) 
{
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] && cl_arr[i]->cl_id == cli_id) {
            return cl_arr[i];
        }
    }
    return NULL;
}

// @Comentários em "clients.h"
bool clt_send_message(int cl_socket, const char* buffer) {
    int try_send = 0;

    // Tenta enviar a mensagem até receber confirmação. Se após 
    while(send(cl_socket, buffer, strlen(buffer), 0) < 0 && try_send < 6) {
        try_send += 1;
    }
    return (try_send == 6) ? false : true;
}

// @Comentários em "clients.h"
void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer){

    // Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo)
    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, ">> <CLIENTE %d>: %s\n", id_cur, buffer);

    pthread_mutex_lock(mutex);

    // Envio da mensagem aos clientes
    // Restringir para broadcast 
    for(int i = 0; i < max_conn; i++){
        if(cl_arr[i] != NULL) {
            if(clt_send_message(cl_arr[i]->cl_socket, msg_buffer)) {
                msg_send_cliente(id_cur, cl_arr[i]->cl_id);
            } else {
                msg_client_no_response(cl_arr[i]->cl_id);
                clt_destruir(cl_arr[i]->cl_id);
            } 
        }
    }

    pthread_mutex_unlock(mutex);
}

// @Comentários em "clients.h"
int clt_read_buffer(client* cl, char* buffer) {
    
    // Comando: /ping
    if(strncmp(buffer, "/ping", 5) == 0) {
        
        char log[] = ">> <SERVER>: pong\n";
        memset(buffer, '\0', BUFFER_SIZE);
        msg_info_ping(cl->cl_id);

        if(clt_send_message(cl->cl_socket, log)) {
            msg_info_pong(cl->cl_id);
        } 
        return PING;

    // Comando: /quit
    } else if(strncmp(buffer, "/quit", 5) == 0) {
        memset(buffer, '\0', BUFFER_SIZE);
        return QUIT;
    } 

    // Mensagem a ser enviada para os outros clientes
    return CONTINUE;
}

// @Comentários em "clients.h"
void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex){

    int shift;
    char* pack; 
    
    // Buffer com as mensagens dos clientes 
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    // Recupera as informacoes do cliente...
    client* cl = clt_get_by_id(id_cur);

    // Cliente indisponível
    if(cl == NULL) {
        return;
    }

    // Mensagem com as informacoes do cliente
    msg_info_client(id_cur, cl->cl_socket, cl->cl_address);

    while(true){

        // Mensagem recebida da usuário!
        if(recv(cl->cl_socket, buffer, BUFFER_SIZE, 0) > 0){
            
            int input = //TODO
            clt_read_buffer(cl, buffer);
            switch(input) {
                // case CONTINUE:
                //     msg_recv_cliente(id_cur, buffer);

                //     shift = 0;
                //     pack = (char*)malloc(sizeof(char)*BUFFER_SIZE);
                //     while((shift = decode_message(buffer, pack, shift)) > 0) {
                //         clt_send_message_all(id_cur, max_conn, mutex, pack);
                //     }
                    
                //     free(pack);
                //     bzero(buffer, BUFFER_SIZE);
                //     break;
                // case QUIT:
                //     msg_cliente_desconexao(cl->cl_id);
                //     return;
                // default: break;
            }
        }
        // Ocorreu um erro na conexao...
        else{
            return;
        }
    }
}


// @Comentários em "client.h"
int decode_message(char* buffer, char* pack, int index) {

    if(buffer == NULL || *buffer == '\n' || *buffer == '\0')
        return 0;
    
    bzero(pack, BUFFER_SIZE);

    int i = 0, j = 0;
    for(i = index; buffer[i] != '\n' && i < BUFFER_SIZE-1; ++i) {
        pack[j++] = buffer[i];
    }

    pack[j] = '\n';
    pack[j+1] = '\0';

    if(buffer[i] != '\n' || i >= BUFFER_SIZE) 
        return 0;
    return i + 1;
}
