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
#include <time.h>

#define N_THREADS 5 // Quantidade fixa de Threads
#define N_MAX_CLIENTS 5 // Número máximo de clientes
#define PORTA_SOCKET 5000 // Porta de conexão do socket

int buffer_server = 0;

void *calc_horas(void *arg)
{
    int *pvalor = arg;

    printf("Thread server %d executando....\n", *pvalor);

    while (1)
    {
        sleep(1);
        if (buffer_server > 0)
        {

            char buffer_resposta[256];
            char buffer_solicitacao[256];

            int sockEntrada = buffer_server;
            buffer_server = 0;

            while (1)
            {
                printf("Socked: %d - Thread: %d \n", sockEntrada, *pvalor);
                //Le o que vem do cliente
                read(sockEntrada, buffer_solicitacao, sizeof(buffer_solicitacao));

                printf("%s",buffer_solicitacao);

                if (strcmp(buffer_solicitacao, "sair") != 0)
                {
                    printf("Resposta enviada!\n");
                    const time_t timer = time(NULL);
                    strcpy(buffer_resposta, ctime(&timer));

                    send(sockEntrada, buffer_resposta, sizeof(buffer_resposta),0);
                }
                else
                {

                    close(sockEntrada);
                    pthread_exit((void *)0);
                }
            }
        }
    }

    printf("Thread server %d finalizando....\n", *pvalor);
}

int config_server_socket()
{

    int sockfd;
    struct sockaddr_in serverAddr;

    //Cria o socket
    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Erro no Socket!\n");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    //Seta os IPS
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Define a porta
    serverAddr.sin_port = htons(PORTA_SOCKET);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("Erro no Socket!\n");
        exit(1);
    }
    //Fica na escuta de ate n clientes
    if (listen(sockfd, N_MAX_CLIENTS) < 0)
    {
        printf("Erro no Socket!\n");
        exit(1);
    }
    return sockfd;
}

void *receber_mensagens(void *arg)
{

    int *sockfd = arg;

    printf("Thread server receber_mensagens executando....\n");

    while (1)
    {

        struct sockaddr_in clientAddr;
        unsigned int clntLen;
        clntLen = sizeof(clientAddr);
        pthread_t thread;

        //Fica no aguardo da conexao do cliente
        int client_socked = accept(*sockfd, (struct sockaddr *)&clientAddr, &clntLen);

        if (client_socked < 0)
        {
            printf("Erro no Socket!\n");
            exit(1);
        }
        else
        {
            printf("Conexão bem sucedida !\n");
            buffer_server = client_socked;
        }
    }

    printf("Thread server receber_mensagens finalizada!\n");
}

void main()
{

    pthread_t thread_receber, threads[N_THREADS];
    int x, i, vet_threads[N_THREADS];

    struct sockaddr_in serverAddr;
    int sockfd = config_server_socket();

    pthread_create(&thread_receber, NULL, receber_mensagens, (void *)&sockfd);

    for (x = 0; x < N_THREADS; x++)
    {
        vet_threads[x] = x + 1;
        pthread_create(&threads[x], NULL, calc_horas, (void *)&vet_threads[x]);
    }

    pthread_join(thread_receber, NULL);

    for (x = 0; x < N_THREADS; x++)
    {
        pthread_join(threads[x], NULL);
    }
}
