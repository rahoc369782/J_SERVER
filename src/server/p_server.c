#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void test(void)
{
    printf("test function");
}

void convert_ip(unsigned char *addr, unsigned int net_byte)
{
    unsigned char bytes[4];
    bytes[0] = (net_byte >> 24) & 0xFF;
    bytes[1] = (net_byte >> 16) & 0xFF;
    bytes[2] = (net_byte >> 8) & 0xFF;
    bytes[3] = net_byte & 0xFF;
    printf("IP Address: %u.%u.%u.%u\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    addr = bytes;
}

int p_bind(int _soc, struct sockaddr *server, int s_size)
{
    int _bind = bind(_soc, (struct sockaddr *)&server, s_size);
    if (_bind < 0)
    {
        return 1;
    };
}