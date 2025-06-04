#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h> // Needed for sockaddr_ll and ETH_P_ALL
#include <net/ethernet.h> // Needed for ETH_P_ALL

int makeSocket(const char *interface);

int main(int argc, char *argv[]) {
    int rawSocket;
    int i;
    unsigned char buffer[65536];
    struct sockaddr saddr_in;
    int saddr_len = sizeof(saddr_in);
    int data_size;
    long n; 
    
    char *argument = argv[1];
    /*Check and see if there is arguments*/
    if (argc > 1) {
        switch (argument[0]) {
            case '-':
                switch (argument[1]) {
                case 'a':
                    printf("Option a selected\n");
                    break;
                case 'r':
                    printf("Option b selected\n");
                    break;
                case 'h':
                    printf("Option c selected\n");
                    break;
                default:
                    printf("Invalid option\n");
                }
            break;
        }
    }
    return 0;
    // Create a raw socket
    rawSocket = makeSocket("wlp0s20f3");

    while (1) {
        data_size = recvfrom(rawSocket, buffer, sizeof(buffer), 0, &saddr_in, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("recvfrom failed");
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


/*         for (int i = 0; i < data_size; i++) {
            printf("%02x ", buffer[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        } 
        printf("\n");
    } */
    // Example of closing the socket.
    close(rawSocket);

    return 0; // Exit successfully
    }

int makeSocket(const char *interface) {
    int rawSocket;
    struct sockaddr_ll saddr;
    struct ifreq ifr;

    rawSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (rawSocket < 0) {
        perror("Socket Creation Failed");
        return -1;
    }

    /*Bind the raw socket to interface
    memset(&saddr, 0, sizeof(saddr));
    memset(&ifr, 0, sizeof(ifr));
    strncpy((char *)ifr.ifr_name, interface, IFNAMSIZ -1);

    if (ioctl(rawSocket, SIOGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOGIFINDEX failed");
        close(rawSocket);
        return -1;
    }

    saddr.sll_family = AF_PACKET;
    saddr.sll_ifindex = ifr.ifr_ifindex;
    saddr.sll_protocol = htons(ETH_P_ALL);

    if (bind(rawSocket, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("Socket Bind Failed");
        close(rawSocket);
        return -1;
    }

    return rawSocket;
}
*****/