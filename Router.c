#include "util.h"


Router router;

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

char* setFile(Router router, int i){

    char *fileName = malloc(strlen(router.router_ip) + strlen(router.router_ports[i].port_ip) + 1);
    strcpy(fileName , router.router_ip);
    strcat(fileName, router.router_ports[i].port_ip);

    return fileName;
}

int handle_command(char *command,int line_len)
{
    if(strstr(command,"Connect") != NULL) {
        // ip has beet set  
        char* target_ip = malloc(line_len * sizeof(char));
        getToken(2, command, line_len, target_ip);
        char* target_port = malloc(line_len * sizeof(char));
        getToken(3, command, line_len, target_port);
        char* link = malloc(line_len * sizeof(char));
        getToken(4, command, line_len, link);

        router.learned_ips++;
        router.table[router.learned_ips] = (char*)malloc ((strlen(target_ip) * 3 + 1) * sizeof(char));
        char fname[64] = "";
        strcpy(fname, target_ip);
        strcat(fname, " ");
        strcat(fname, router.router_ip);
        strcpy(router.table[router.learned_ips], fname);

        char fname1[64];
        strcpy(fname1, router.router_ip);
        strcat(fname1, target_ip); 
        
        router.port_used++;
        router.router_ports[router.port_used].port_ip = (char*)malloc (strlen(fname1) * sizeof(char));
        strcpy(router.router_ports[router.port_used].port_ip, fname1);
        FILE *fd;
        fd = fopen(fname1, "w+");
        fclose(fd);
        // }
        free(link);
        free(target_port);
        free(target_ip);
    }

    else if(strstr(command, "Show") != NULL) {
        printRouterInfo();
    }

    else if(strstr(command, "Disconnect") != NULL) {
        char* target_ip = malloc(line_len * sizeof(char));
        getToken(2, command, line_len, target_ip);
        int i;
        for (i=CLIENT_NUM; i<router.port_used; i++) {
            if (strstr(target_ip, router.router_ports[i].port_ip) != NULL) {
                strcpy(router.router_ports[i].port_ip, "");
                break;
            }
        }
        free(target_ip);
    }

    // else if(strstr(command, "Group") != NULL) {
    //     char* group_ip = malloc(line_len * sizeof(char));
    //     getToken(2, command, line_len, group_ip);
    //     // TODO: fill group table;
    // }

    // else if (strstr(command, "ChangeCost") != NULL) {
    //     char* link = malloc(line_len * sizeof(char));
    //     getToken(2, command, line_len, link);
    //     char* cost = malloc(line_len * sizeof(char));
    //     getToken(3, command, line_len, cost);

    //     // TODO: ADD cost to table

    //     free(link);
    //     free(cost);
    // }
    return 1;
}

void printRouterInfo()
{
    printf("Table info:\n");
    int i;
    for (i=0; i<router.learned_ips; i++) {
        printPortInfo(router.table[i]);
    }
    printf("Multicast table info:\n");
    for (i=0; i<router.multi_set; i++) {
        printMultiInfo(router.multiTable[i]);
    }
    return;
}

void printPortInfo(char* info)
{
    int line_len = strlen(info);
    char target_ip[64], port[64];
    getToken(1, info, line_len, target_ip);
    getToken(2, info, line_len, port);
    printf(" IP:%s     file(port): %s \n", target_ip, port);
    return; 
}

void printMultiInfo(char* info)
{
    int i,j,k;
    for (i=0; i<router.port_used;i++) {
        for (j=0; j<router.router_ports[i].filled; j++) {
            if (strcmp(info, router.router_ports[i].groups[j]) == 0) {
                for (k=0; k<router.learned_ips; k++) {
                    char fname[64] = "";
                    if (strstr(router.router_ports[i].port_ip, router.table[k])) {
                        char *index = strchr(router.table[k], " ");
                        strncpy(fname, router.table[k], index - router.table[k]);
                        strcat(fname, router.router_ip);
                    }
                printf("groupIP: %s     port: %s",info, fname);
                }
            }
        }
    }
    return; 
}

// void filename_generator(char filename[64], char* port_name)
// {
//     char tmp[64] = "";
//     sprintf(tmp, "%s%s", router.router_ip, port_name);
//     memset(filename, 0, 64);
//     strcpy(filename, tmp);
//     return;
// }

void openPortFiles(Router* router)
{
    int i;
    char portname[4] = "";
    FILE *fd;
    char filename[64];
    for(i=1; i<=CLIENT_NUM; i++) {
        sprintf(portname, "%d", i);
        sprintf(filename, "%s%s", router->router_ip, portname);
        fd = fopen(filename, "w+");
        router->router_ports[router->port_used].port_ip = (char*)malloc ( strlen(portname) * sizeof(char));
        strcpy(router->router_ports[router->port_used].port_ip, portname);
        router->port_used++;
        fclose(fd);
        memset(portname, 0, 4);
        memset(filename, 0, 64);
    }
}


// void extract_port(char *info, char **port)
// {
//     char *index = strchr(info, " ");
//     int port_len = strlen(index+1);
//     *port = malloc(port_len * sizeof(char *)+1);
//     strncpy(port, index + 1, port_len);
//     return;
// }

void uni_sendpacket(char* target_ip, int j, char *message)
{
    char fname[64] = "";
    strcpy(fname, target_ip);
    // char *port;
    // extract_port(router.table[j], &port);
    strcat(fname, router.router_ip);
    FILE *target_fd;
    target_fd = fopen(fname, "a");
    fputs(message, target_fd);
    // free(port);
    fclose(target_fd);
    return;
}

void broadcast(char *message)
{
    int i;
    char *srcIP = malloc(64 * sizeof(char));;
    getToken(1, message, 64, srcIP);
    for (i=0; i<router.learned_ips; i++) {
        char fname[64] = "";
        char *index = strchr(router.table[i], " ");
        strncpy(fname, router.table[i], index - router.table[i]);
        strcat(fname, router.router_ip);
        if (strstr(srcIP, fname) != NULL)
            continue;
        FILE *fd;
        printf("%s\n", fname);
        printf("%s\n", message);
        fd = fopen(fname, "a");
        fputs(message, fd);
        fclose(fd);
    }
    free(srcIP);
    return;
}

void multi_sendpacket(char* target_ip, char *message)
{
    int i,j;
    for (i=0; i<router.port_used; i++) {
        for (j=0; j<router.router_ports[i].filled; j++) {
            if (strcmp(target_ip, router.router_ports[i].groups[j]) == 0) {
                int k;
                for (k=0; k<router.learned_ips; k++) {
                    if (strstr(router.router_ports[i].port_ip, router.table[k])) {
                        char fname[64] = "";
                        char *index = strchr(router.table[k], " ");
                        strncpy(fname, router.table[k], index - router.table[k]);
                        strcat(fname, router.router_ip);
                        FILE *fd;
                        fd = fopen(fname, "a");
                        fputs(message, fd);
                        fclose(fd);
                    }
                }
            }
        }
    }
    return;
}

void handle_message(char* message, int line_l, Port port, Router router)
{

    if (strstr(message, "create group") != NULL) {

        char group_ip[16];
        getToken(4, message, line_l, group_ip);
        port.filled++;
        int i;
        bool has_set_group = false;
        for (i=0; i<router.multi_set; i++) {
            if (strcmp(router.multiTable[i], group_ip) == 0) {
                has_set_group = true;
                break;
            }
        }
        if (!has_set_group) {
            router.multi_set++;
            router.multiTable[router.learned_ips] = strdup(group_ip);
        }
        port.groups[port.filled] = strdup(group_ip);
        broadcast(message);
    }
    else if (strstr(message, "join") != NULL) {
        char group_ip[16];
        getToken(4, message, line_l, group_ip);
        port.filled++;
        port.groups[port.filled] = strdup(group_ip);
        broadcast(message);
    }
    else if (strstr(message, "leave") != NULL) {
        char group_ip[16];
        getToken(3, message, line_l, group_ip);
        for (int i=0; i<port.filled; i++) {
            if(strcmp(group_ip, port.groups[i]) == 0)
                port.groups[i] = strdup("");
            break;
        }
        broadcast(message);
    }
    else{
        char* target_ip = malloc(line_l * sizeof(char));
        getToken(2, message, line_l, target_ip);
        char* source_ip = malloc(line_l * sizeof(char));
        getToken(1, message, line_l, source_ip);
        
        bool packet_sent = false;
        for (int j=0; j<router.learned_ips; j++) {
            if(!strcmp(target_ip, router.router_ip)){
                packet_sent = true;
                break;
            }
            else if(strstr(target_ip, router.table[j]) != NULL) {
                uni_sendpacket(target_ip, j, message);
                packet_sent = true;
                break;
            }
        }
        for (int j=0; j<router.multi_set; j++) {
            if(strstr(target_ip, router.multiTable[j]) != NULL) {
                multi_sendpacket(target_ip, message);
                packet_sent = true;
                break;
            }
        }
        if (!packet_sent) {
            //learning source ip and add to table 
            char* src_ip = malloc(line_l * sizeof(char));
            getToken(1, message, line_l, src_ip);
            router.learned_ips++;
            router.table[router.learned_ips] = (char*)malloc ((strlen(src_ip) * 3 + 1) * sizeof(char));
            char fname[64] = "";
            strcpy(fname, src_ip);
            strcat(fname, " ");
            strcat(fname, router.router_ip);
            strcpy(router.table[router.learned_ips], fname);
            broadcast(message);
        }
    }
    return;
}


int main(int argc, char* argv[]) {
    Router router;

    strcpy(router.router_ip , argv[1]);
    router.port_used = 0;
    router.multi_set = 0;
    router.learned_ips = 0;

    FILE* self_fd;
    self_fd = fopen(router.router_ip, "w+");

    openPortFiles(&router);

    FILE* files[4];

    

    char filename[64];
    while(1)
    {
        // printf("hi\n");
        int i,j;
        for(i=0; i<router.port_used; i++) {
            // printf("hi1\n");

            char *filename = setFile(router, i);
            files[i] = fopen(filename, "a+");
            // printf("%s", router.router_ports[i].port_ip);
            if(files[i] != NULL){
                // printf("%s", filename);
                
                char* message;
                size_t l = 0;
                ssize_t line_l;
                line_l = getline(&message, &l, files[i]);
                if(line_l >= 0){
                    printf("%s\n", message);
                    handle_message(message, line_l, router.router_ports[i], router);
                }
                
            }
            fclose(files[i]);
        }


        // char* command;

        // size_t len = 0;
        // size_t line_len;

        // line_len = getline(&command, &len, stdin);

        // if(command != NULL){
        //     handle_command(command, line_len);
        // }
    }
    fclose(self_fd);
    return 0;
}