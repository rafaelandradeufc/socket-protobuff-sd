#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <ctype.h>

#define PORTA_SOCKET 5000              // Porta de conexão do socket
#define IP_SOCKET_SERVIDOR "127.0.0.1" // Endereço de IP do servidor se for local
//#define IP_SOCKET_SERVIDOR "192.168.0.101" // Endereço de IP do servidor se for externo

char buffer_client[256];
char buffer_resposta[256];

void *iniciar(void *arg)
{

    int opc;
    char pessoa[256];
    char aux[256];

    printf("Thread 1: terminal_cliente executando....\n");

    while (1)
    {
        printf("Digite uma opcao!\n");
        printf("1.Inserir\n2.Buscar\n3.Atualizar\n4.Excluir\n5.Sair\n");
        printf("\n->");
        scanf("%d", &opc);

        switch (opc)
        {
        case 1:
            system("clear");
            strcpy(pessoa, "inserir@@");
            printf("\ncpf: ");
            scanf("%s", aux);
            strcat(pessoa, aux);
            strcpy(aux, "");
            strcat(pessoa, "/");

            printf("\nNome: ");
            scanf("%s", aux);
            strcat(pessoa, aux);
            strcpy(aux, "");
            strcat(pessoa, "/");

            printf("\nTelefones: ");
            scanf("%s", aux);
            strcat(pessoa, aux);
            strcpy(aux, "");
            strcat(pessoa, ";");

            strcpy(buffer_client, pessoa);
            fflush(stdin);
            strcpy(pessoa, "");

            break;

        case 2:

            system("clear");
            strcpy(pessoa, "buscar@@");
            printf("\nDigite o CPF:");
            scanf("%s", aux);
            strcat(pessoa, aux);
            strcat(pessoa, ";");

            strcpy(buffer_client, pessoa);
            fflush(stdin);
            strcpy(pessoa, "");
            break;

        case 3:

            system("clear");
            strcpy(pessoa, "buscar@@");
            printf("\nDigite o CPF:");
            scanf("%s", aux);
            strcat(pessoa, aux);
            strcat(pessoa, ";");



            break;
        case 4:

            break;

        case 5:
            exit(0);
            break;

        default:
            break;
        }
    }

    printf("Thread 1: terminal_cliente finalizado!\n");
}

void *enviar_receber(void *arg)
{
    int *clienteSockfd = arg;
    

    printf("Thread 2: envia_recebe iniciou!\n");

    while (1)
    {

        if (strcmp(buffer_client, "") != 0)
        {

            send(*clienteSockfd, buffer_client, sizeof(buffer_client), 0);
            //read(*clienteSockfd, buffer_resposta, sizeof(buffer_resposta));

            //printf("HORAS: %s\n", buffer_resposta);

            strcpy(buffer_client, "");
        }
    }

    printf("Thread 2: envia_recebe finalizado!\n");
}

int config_client_socket()
{

    int clienteSockfd;
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(serv_addr);

    clienteSockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clienteSockfd < 0)
    {
        printf("Erro no Socket\n");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //Define o IP do servidor
    serv_addr.sin_addr.s_addr = inet_addr(IP_SOCKET_SERVIDOR);
    //Define a porta de conexao
    serv_addr.sin_port = htons(PORTA_SOCKET);
    //Faz a conexao com o servidor
    if (connect(clienteSockfd, (struct sockaddr *)&serv_addr, addrlen) < 0)
    {
        printf("Erro noiniciar Socket\n");
        exit(1);
    }
    return clienteSockfd;
}

void main()
{

    pthread_t thread0, thread1;
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(serv_addr);
    int descritorCliente;
    descritorCliente = config_client_socket();

    pthread_create(&thread1, NULL, enviar_receber, (void *)&descritorCliente);
    pthread_create(&thread0, NULL, iniciar, NULL);

    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
}
