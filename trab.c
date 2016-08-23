#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getRange(char *str, char *splitter){
    char *token = strtok(str, splitter);
    char *last = malloc(sizeof(str));
    while(token) {
        puts(token);
        strcpy(last,token);
        token = strtok(NULL, splitter);
    }
    int range = atoi(last);
    free (last);
    return range;
}

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

int main(int argc, char **argv){
    char *ip;
    int ipRange;
    int ipLastField;
    int porta;
    int portaRange;
  
    if(argv[1] != NULL){
        ip = malloc(strlen(argv[1]));
        strcpy(ip, argv[1]);
        ipRange = getRange(ip, "-");
        ipLastField = getLastField(ip);
        if(ipRange < ipLastField){
            printf("\nErro:\nValor do range do ip menor do que valor do campo ip\n");
            return 0;
        }
        ip = ipSplit(ip);
        printf("\nIP: %s\n", ip);
        printf("Last field: %d\n", ipLastField);
        printf("Range: %d\n\n", ipRange);
    }
    else{
        printf("\nErro:\nValor do ip não pode ser nulo\n");
        return 0; 
    }
    if(argv[2] != NULL){
        char *strPorta = malloc(strlen(argv[2]));
        strcpy(strPorta, argv[2]);
        portaRange = getRange(strPorta, "-");
        porta = atoi(strPorta);
        if(portaRange < porta){
            printf("\nErro:\nValor do range da porta menor do que valor do campo porta\n");
            return 0;
        }
        printf("Porta: %d\n", porta);
        printf("Range: %d\n\n", portaRange); 
    }
   
}