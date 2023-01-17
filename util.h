#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/in.h>
#include <unistd.h>
#include <stdbool.h>


#define PACKETSIZE  256
#define BUFSIZE     1024
#define CLIENT_NUM  4


typedef struct{
    char* name;
    char* IP;
} Group;

typedef struct {
    char name[64];
    char server_ip[32];
    char router_ip[32];
    char router_port[16];
    char IP[32];
    Group *group;
} Client;

typedef struct{
    char* port_ip;
    char* groups[10];
    int filled;
}Port;

typedef struct {
    char router_ip[32];
    Port router_ports[32];
    int port_used;
    char *table[100];
    int learned_ips;
    char *multiTable[100];
    int multi_set;
} Router;

int str_separate(char *srcstr, char sep, char ***output);
int encapsulate_packet(char *data, char *ip, char **packet);
int safe_free(char **ptr);
int decapsulate_packet(char *packet, char **data, char **ip);


