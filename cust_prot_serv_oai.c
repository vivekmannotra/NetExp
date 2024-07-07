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

void handle_message(CustomMessage *msg) {
    printf("Message Type: %d\n", msg->type);
    printf("Message Length: %d\n", msg->length);
    printf("Message Payload: %s\n", msg->payload);
}

int main() {
    int sockfd;
    CustomMessage msg;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    len = sizeof(cliaddr);
    int n = recvfrom(sockfd, &msg, sizeof(CustomMessage), MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
    msg.payload[msg.length] = '\0';  // Null-terminate the payload

    handle_message(&msg);

    // Send a response back to the client
    msg.type = 2;  // Response type
    const char *response = "Hello from server";
    msg.length = strlen(response);
    strncpy(msg.payload, response, BUFFER_SIZE);
    sendto(sockfd, &msg, sizeof(int) * 2 + msg.length, MSG_CONFIRM,
           (const struct sockaddr *)&cliaddr, len);

    printf("Response sent to client\n");

    close(sockfd);
    return 0;
}