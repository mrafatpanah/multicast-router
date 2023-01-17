#include "util.h"

int str_separate(char *srcstr, char sep, char ***output)
{	
	int len = strlen(srcstr);

	int numparts = 0;

	char temp[100];
	strcpy(temp, srcstr);

	char **currentpart;

	int i;
	for (i = 0; i < len; i++)
	{
		if (temp[i] == sep)
		{
			temp[i] = '\0';	
			numparts++;
		}
	}

	numparts++;
	
	*output = malloc(numparts * sizeof(char *));

	if (*output == NULL)
	{
		perror("malloc() call in str_separate");
		exit(1);
	}

	currentpart = *output;
	*currentpart = temp;
	
	for (i = 0; i < len; i++)
	{
		if (temp[i] == '\0') {
			currentpart++;
			*currentpart = &(temp[i+1]);
		}
	}
	return numparts; 
}

int encapsulate_packet(char *data, char *ip, char **packet)
{
	int ip_len = strlen(ip);
	char temp[32];

	if (ip_len > 9)
		sprintf(temp, "%d%s", ip_len, ip);
	else
		sprintf(temp, "0%d%s", ip_len, ip);
	*packet = malloc(PACKETSIZE * sizeof(char *));
	strcat(*packet, temp);
	strcat(*packet, data);
	return 0;
}

int safe_free(char **ptr)
{
    strcpy(*ptr, "0");
    free(*ptr);
}

int decapsulate_packet(char *packet, char **data, char **ip)
{
	char temp[32]="";
	int packet_len = strlen(packet);
	strncpy(temp, packet, 2);
	int ip_len = atoi(temp);
	memset(temp, 0, 32);
	strncpy(temp, packet+2, ip_len);
	*ip = malloc(ip_len * sizeof(char *));
	strcpy(*ip, temp);
	int data_len = packet_len - ip_len - 2;
	*data = malloc((data_len) * sizeof(char*));
	strncpy(*data, packet + ip_len + 2, data_len);
	return 0;
}