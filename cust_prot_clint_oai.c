#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    int type;
    int length;
    char payload[BUFFER_SIZE];
} CustomMessage;

int main() {
    int sockfd;
    CustomMessage msg;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    msg.type = 1;  // Request type
    const char *request = "Hello from client";
    msg.length = strlen(request);
    strncpy(msg.payload, request, BUFFER_SIZE);

    sendto(sockfd, &msg, sizeof(int) * 2 + msg.length, MSG_CONFIRM,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Request sent to server\n");

    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, &msg, sizeof(CustomMessage), MSG_WAITALL,
                     (struct sockaddr *)&servaddr, &len);
    msg.payload[msg.length] = '\0';  // Null-terminate the payload

    printf("Received from server: Type: %d, Length: %d, Payload: %s\n",
           msg.type, msg.length, msg.payload);

    close(sockfd);
    return 0;
}