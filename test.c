#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

int createSocket(const char *interface);
int receivePacket(int rawSocket, unsigned char *buffer, int bufferSize);
void returnRawPacket(int rawSocket, unsigned char *buffer, int bufferSize);
void returnIPdata(int rawSocket, unsigned char *buffer, int bufferSize);

int main(int argc, char *argv[]) {
    int rawSocket;
    unsigned char buffer[65536];

    rawSocket = createSocket("wlp0s20f3");
    if (rawSocket < 0) {
        return 1;
    }

    if (strcmp(argv[1], "--raw") == 0){
        returnRawPacket(rawSocket, buffer, sizeof(buffer));
    }
    else if (strcmp(argv[1], "--ip-only") == 0) {
        returnIPdata(rawSocket, buffer, sizeof(buffer));
    }
    else {
        printf("%s", "error\n");
    }

    return 0;
}

int createSocket(const char *interface) {
    int rawSocket;
    struct sockaddr_ll saddr;
    struct ifreq ifr;

    rawSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (rawSocket < 0) {
        perror("Socket creation failed");
        return -1; // Indicate error
    }

    // Bind to the specified interface
    memset(&saddr, 0, sizeof(saddr));
    memset(&ifr, 0, sizeof(ifr));
    strncpy((char *)ifr.ifr_name, interface, IFNAMSIZ - 1);

    if (ioctl(rawSocket, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX failed");
        close(rawSocket);
        return -1;
    }

    saddr.sll_family = AF_PACKET;
    saddr.sll_ifindex = ifr.ifr_ifindex;
    saddr.sll_protocol = htons(ETH_P_ALL);

    if (bind(rawSocket, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("Bind failed");
        close(rawSocket);
        return -1;
    }

    printf("Raw socket created and bound successfully to interface: %s\n", interface);
    return rawSocket;
}
int receivePacket(int rawSocket, unsigned char *buffer, int bufferSize) {
    struct sockaddr saddr_in;
    int saddr_len = sizeof(saddr_in);
    int data_size = recvfrom(rawSocket, buffer, bufferSize, 0, &saddr_in, (socklen_t *)&saddr_len);

    if (data_size < 0) {
        perror("recvfrom failed");
        return -1;
    }

    return data_size;
}
void returnRawPacket(int rawSocket, unsigned char *buffer, int bufferSize) {
    int data_size;

    while (1) {
        data_size = receivePacket(rawSocket, buffer, bufferSize);

        if (data_size < 0) {
            break;
        }

        printf("\nReceived packet (size: %d bytes):\n", data_size);

        for (int i = 0; i < data_size; i++) {
            printf("%02x ", buffer[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    }
    close(rawSocket);
}
void returnIPdata(int rawSocket, unsigned char *buffer, int bufferSize) {
    int data_size;

    while (1) {
        data_size = receivePacket(rawSocket, buffer, bufferSize);

        if (data_size < 0) {
            break;
        }

        printf("\nReceived packet (size: %d bytes):\n", data_size);
        printf("Source IP: ");
        for (int i = 0; i < data_size; i++) {
            if (i == 26 || i == 27 || i == 28 || i == 29) {
                if (i == 29){
                    printf("%d", buffer[i]);
                }
                else{ 
                    printf("%d.", buffer[i]);
                }
            }
        }
        printf("\nDestination IP: ");
        for (int i = 0; i < data_size; i++) {
            if (i == 30 || i == 31 || i == 32 || i == 33) {
                if (i == 33){
                    printf("%d", buffer[i]);
                }
                else{ 
                    printf("%d.", buffer[i]);
                }
            }
        }
        printf("\n");
    }
    close(rawSocket);
}