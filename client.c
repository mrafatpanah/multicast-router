#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define GROPU_MAX 10

struct Group{
    char* name;
    char* IP;
};

struct Client{
    char* name;
    char* server_ip;
    char* router_ip;
    char* router_port;
    char* ip;
    FILE* file;
    struct Group groups[GROPU_MAX];
    int groupNum;
};

void splitStr(char str[], char newString[3][10]){
   int n=0,i,j=0;
	for(i=0;strlen(str);i++)
	{
		if(str[i]!=' '){
			newString[n][j++]=str[i];
		}
		else{
			newString[n][j++]='\0';//insert NULL
			n++;
			j=0;
		}
		if(str[i]=='\0')
		    break;
	}
}

char* getToken(int n, char* command, int line_len, char* newString){
    // char* newString = malloc(line_len * sizeof(char));
    int len = 0;
    for (int i = 0; i<line_len ; i++){
        if(command[i] != ' '){
            newString[len] = command[i];
            len++;
        }
        else{
            memset(newString, 0, line_len);
            len = 0;
            if(n==1){
                return newString;
            }
            n--;
        }
    }
}

char* setFile(struct Client client){

    char *fileName = malloc(strlen(client.router_ip) + strlen(client.router_port) + 1);
    strcpy(fileName , client.router_ip);
    strcat(fileName, client.router_port);

    return fileName;
}

void leaveGroup(char* group, struct Client client){
    for (int i = 0; i < client.groupNum ; i++){
        if(client.groups[i].IP = group){
            client.groups[i] = client.groups[client.groupNum];
            break;
        }
    }
    client.groupNum--;
}

void printGroupList(char* fileName){
    char buf[1024];
    FILE *file;
    size_t nread;

    file = fopen(fileName, "r");
    if (file) {
        while ((nread = fread(buf, 1, sizeof buf, file)) > 0)
            fwrite(buf, 1, nread, stdout);
        if (ferror(file)) {
            printf("there is an error\n");
        }
        fclose(file);
    }
}
void connect(char* fileName, struct Client client){

    FILE *file;
    file = fopen(fileName, "a");

    if(file == NULL){
        printf("router doesn't exist!\n");
        return;
    }
    
    char* command = "connect ";
    char *message = malloc(strlen(client.ip) + strlen(command) + strlen(client.router_port)+ 10);
    strcpy(message, client.ip);
    strcat(message, " ");
    strcat(message, command);
    strcat(message, " ");
    strcat(message, client.router_port);
    strcat(message, "\n");
    fputs(message, file);
    free(message);
    fclose(file);

}

void sendMessage(struct Client client, char* command){
    char* fileName = setFile(client);
    FILE* file = fopen(fileName, "a");

    if(file==NULL){
        printf("port doesn't exist!\n");
    }

    char *message = malloc(strlen(client.ip) + strlen(command) + 2);
    strcpy(message, client.ip);
    strcat(message, " ");
    strcat(message, command);
    strcat(message, "\n");
    fputs(message, file);
    free(message);
    fclose(file);
}

int handle_command(char *command,int line_len, struct Client client){
    
    FILE *file;
    
    if(strstr(command,"set ip") != NULL){
        // ip has beet set  
        char* ip = malloc(line_len * sizeof(char));
        getToken(3, command, line_len, ip);
        client.ip = ip;
        free(ip);
        

    }    
    else if(strstr(command, "get group list") != NULL){
        
        char *fileName = client.router_ip;
        printGroupList(fileName);
           
    }
    else if(strstr(command, "join") != NULL){

        char* group = malloc(line_len * sizeof(char));
        getToken(2, command, line_len, group);
        client.groups[client.groupNum].IP = group;
        client.groupNum++;
        free(group);
        sendMessage(client, command);
    
    }
    else if(strstr(command, "create group") != NULL){

        char* group = malloc(line_len * sizeof(char));
        getToken(3, command, line_len, group);
        client.groups[client.groupNum].IP = group;
        client.groupNum++;
        free(group);
        
        sendMessage(client, command);

    }
    else if(strstr(command, "leave") != NULL){

        char* group = malloc(line_len * sizeof(char));
        getToken(2, command, line_len, group);
        leaveGroup(group, client);
        free(group);

        sendMessage(client, command);

    }
    else if(strstr(command, "send file") != NULL){
        
        char* fileName = setFile(client);
        file = fopen(fileName, "a");

        if(file==NULL){
            printf("port doesn't exist!\n");
        }
        
        strcat(command, " ");
        strcat(command, client.ip);
        fputs(command, file);
        fclose(file);    

    }
    else if(strstr(command, "send message") != NULL){

        char *message = malloc(strlen(client.ip) + strlen(command) + 1);

        strcpy(message, client.ip);
        strcat(message, " ");
        strcat(message, command);
        char* fileName = setFile(client);
        
        file = fopen(fileName, "a");
        if(file==NULL){
            printf("port doesn't exist!\n");
        }
        
        fputs(message, file);
        free(message);
        fclose(file); 

    }
    else if(strstr(command, "show group") == 0){
        for(int i=0; i<=client.groupNum ; i++){
            printf("%s %s\n",client.groups[i].IP, client.groups[i].name);
        }
    }
    // else if(strcmp(command, "sync") == 0){

    // }
    else if(strstr(command, "sign out") == 0){
        return 0;
    }
    return 1;
}


int main(int argc, char* argv[]){
    struct Client client;
    
    client.ip = argv[1];
    client.server_ip = argv[2];
    client.router_ip = argv[3];
    client.router_port =argv[4];
    client.groupNum = 0;

    client.file = fopen(client.ip, "ab+");

    printf("%s\t", client.ip);
    printf("%s\t", client.server_ip);
    printf("%s\t", client.router_ip);
    printf("%s", client.router_port);
    printf("\n");

    char *fileName = setFile(client);
    printf("connecting to router and port %s\n", fileName);
    connect(fileName, client);

    while(1){

        if(client.file != NULL){

            char* message;
            size_t l = 0;
            ssize_t line_l;

            line_l = getline(&message, &l, client.file);

            if(message != NULL && line_l > 0){
                printf("you got this message: ");
                printf("%s\n", message);
            }
         }

        char* command;

        size_t len = 0;
        ssize_t line_len;

        line_len = getline(&command,&len, stdin );

        if(command != NULL){

            int ret = handle_command(command, line_len, client);
            if(ret == 0){
                break;
            }
        }

    }
    fclose(client.file);
    return 0;
}