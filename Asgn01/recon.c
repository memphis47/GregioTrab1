#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define MAX_STRING_ARRAY 1
#define IP_FIELD_SIZE 16

#define IP_REGEX "^([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))\\.(([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|(([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))\\-([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))))$"
#define PORT_REGEX "(^(([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-6])|(([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-6])\\-([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-6])))$)"

#define SOCK_FAMILY AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define SOCK_TCP 6

/**
* Função que retorna o rannge da entrada str.
**/
int getRange(char *str, char *splitter){
    char *token = strtok(str, splitter);
    char *last = malloc(sizeof(str));
    while(token) {
        strcpy(last,token);
        token = strtok(NULL, splitter);
    }
    int range;
    if(strcmp(str,last) == 0){
        range = 0;
    }
    else{
        range = atoi(last);
    }
    free (last);
    return range;
}

/**
*   Função que remove o ultimo campo do IP.
**/
char * ipSplit(char *str){
    int ipLen = strlen(str);
    char *splittedIp;
    int split = 0;
    int aux = 0;
    int i;
    for(i = 0; i < ipLen; i++) {
        if(str[i] == '.')
            split = aux + 1;
        aux ++;
    }
    splittedIp =(char *) malloc(split + 1);
    memcpy(splittedIp, str, split);
    splittedIp[split] = '\0';
    return splittedIp;
}

/**
* Função que retorna o ultimo campo do IP, eg: se str = 192.168.0.25 retorna o valor 25
**/
int getLastField(char *str){
    int ipLen = strlen(str);
    char *splittedIp;
    int aux = 0;
    int i;
    char* field;
    field = (char *) malloc(4);
    for(i = 0; i < ipLen; i++) {
        if(str[i] == '.')
            aux = 0;
        else{
            field[aux] = str[i];
            aux ++;
        }
    }
    field[aux]='\0';
    return atoi(field);
}

/**
*   Gera os ips a serem testados.
**/
int generateIPs(char ***ips, int ipField, int range, char* ip){
    char *charRangeaux;
    int i,j;
    int rangeAux = ipField;
    charRangeaux = malloc(4);
    for(i = 0; i < MAX_STRING_ARRAY; i++){
        ips[i] = calloc(range, sizeof(char *));
        for(j = 0; j <= (range - ipField); j++){
            ips[i][j] = (char *) calloc(IP_FIELD_SIZE, sizeof(char *));
            strcat(ips[i][j], ip);
            sprintf(charRangeaux, "%d", rangeAux);
            strcat(ips[i][j], charRangeaux);
            rangeAux++;
        }
    }
    return 1;
}

/**
*   Função que faz a validação da entrada strValidate de acordo com o regex definido no pattern.
**/
int regexValidation(char *strValidate, char * pattern){
    regex_t reg;

    /* compila a ER passada em argv[1]
     * em caso de erro, a função retorna diferente de zero */
    if (regcomp(&reg , pattern, REG_EXTENDED|REG_NOSUB) != 0) {
        fprintf(stderr,"erro regcomp\n");
        exit(1);
    }
    /* tenta casar a ER compilada (&reg) com a entrada (argv[2])
     * se a função regexec retornar 0 casou, caso contrário não */
    if ((regexec(&reg, strValidate, 0, (regmatch_t *)NULL, 0)) == 0)
        return 1;
    else
        return 0;
}

/**
*   Função que Imprime o horário.
**/
void printTimestamp() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char buf[255];

    strftime(buf, sizeof(buf), "%c", &tm);
    printf("%s", buf);
}

/**
*   Função que testa os IPs definidos em ips, utilizando o range de porta definidos em portRange.
**/
int startTestConnection(char ***ips, int nIPS, int port, int portRange){
    int mySocket;
    int i, j;
    struct sockaddr_in connection;
    struct servent *servico;
    int connector;
    int len;
    char recebe[1024];
    char *pos;

    for(i = 0; i <= nIPS; i++){
        for(j = port; j <= portRange; j++){
            mySocket = socket(SOCK_FAMILY, SOCKET_TYPE, SOCK_TCP);
            connection.sin_family = SOCK_FAMILY;
            connection.sin_port = htons(j);
            connection.sin_addr.s_addr = inet_addr(ips[0][i]);

            bzero(&(connection.sin_zero),8);
            connector = connect(mySocket, (struct sockaddr * ) &connection, sizeof(connection));

            if(connector >= 0) {
                /* escreve qualquer coisa, já que alguns serviços só enviam
                 * o banner depois de alguma escrita */
                len = write(mySocket, "teste", strlen("teste"));

                /* tenta realizar uma leitura, possivelmente capturando uma
                 * mensagem de banner */
                bzero(recebe, sizeof(recebe));
                len = read(mySocket, recebe, 1024);

                /* ignora o \n e tudo que vem depois dele */
                if ((pos=strchr(recebe, '\n')) != NULL)
                    *pos = '\0';

                printf("%s\t%d\t%s\n", ips[0][i], j, recebe);
            }

            close(mySocket);
        }
    }
}

int main(int argc, char **argv){
    char *ip;
    int ipRange;
    int ipLastField;
    int porta;
    int portaRange;

    
    printf("Varredura iniciada em: ");
    printTimestamp();
    printf("\n");
    /* verifica o range de ips */
    if(argv[1] != NULL){
    
        if(regexValidation(argv[1], IP_REGEX)) {
            printf("IP: %s\n", argv[1]);
            ip = malloc(strlen(argv[1]));
            strcpy(ip, argv[1]);
            ipRange = getRange(ip, "-");
            ipLastField = getLastField(ip);
            if(ipRange < ipLastField && ipRange != 0){
                printf("\nErro:\nValor do range do ip menor do que valor do campo ip\n");
                return 0;
            }
            else{
                if(ipRange == 0)
                    ipRange = ipLastField;
            }
            ip = ipSplit(ip);
        }
        else{
            printf("\nFormato do IP inválido\n");
            return 0;
        }
    }
    else{
        printf("\nErro:\nValor do ip não pode ser nulo\n");
        return 0;
    }

    /* verifica o range de portas */
    if(argv[2] != NULL){
        if(regexValidation(argv[2], PORT_REGEX)){
            printf("Portas: %s\n", argv[2]);

            char *strPorta = malloc(strlen(argv[2]));
            strcpy(strPorta, argv[2]);
            portaRange = getRange(strPorta, "-");
            porta = atoi(strPorta);
            if(portaRange < porta && portaRange != 0){
                printf("\nErro:\nValor do range da porta menor do que valor do campo porta\n");
                return 0;
            }
            else{
                if(portaRange == 0)
                    portaRange = porta;
            }
        }
        else{
            printf("\nFormato da porta inválido\n");
            return 0;
        }
    }
    else{
        porta = 0;
        portaRange = 65536;
        printf("Portas: 0-65536\n");
    }

    printf("---\n");
    
    /* cria uma lista de ips e portas a serem varridos */
    char ***ips = calloc(1, sizeof(char **));
    generateIPs(ips, ipLastField, ipRange, ip);

    /* inicia a varredura */
    startTestConnection(ips, ipRange - ipLastField, porta, portaRange);
}
