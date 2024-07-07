# OAI README

### Explanation

1. **Server Code**:
    - Creates a socket using `socket()`.
    - Binds the socket to an address and port using `bind()`.
    - Listens for incoming connections using `listen()`.
    - Accepts a connection using `accept()`.
    - Reads data from the client using `read()`.
    - Sends a response to the client using `send()`.
    - Closes the connection using `close()`.

2. **Client Code**:
    - Creates a socket using `socket()`.
    - Connects to the server using `connect()`.
    - Sends data to the server using `send()`.
    - Reads the response from the server using `read()`.
    - Closes the socket using `close()`.

### Compilation

To compile these codes, you might use:

```sh
gcc -o server server.c
gcc -o client client.c
```

### Running

1. First, run the server:
    ```sh
    ./server
    ```

2. Then, run the client in another terminal:
    ```sh
    ./client
    ```

You should see the interaction between the server and client on your terminal.

The sample program provided sends and receives data in the form of raw bytes over a TCP connection. TCP (Transmission Control Protocol) is one of the core protocols of the Internet Protocol Suite and offers reliable, ordered, and error-checked delivery of a stream of bytes between applications running on hosts communicating over an IP network.

### How Data is Sent and Received

1. **Format**: The data is sent and received as plain text strings (raw bytes). For example, in the server code, the server sends the string "Hello from server" to the client, and the client sends "Hello from client" to the server.

2. **Protocol**: The communication is based on the TCP protocol, which handles breaking down the data into packets, ensuring they are delivered, checking for errors, and reassembling them in the correct order on the receiving end.

It's important to note that the example code doesn't specify a higher-level protocol like HTTP or FTP. It simply sends and receives raw strings. 

### TCP/IP Protocol

- **TCP**: Transmission Control Protocol provides a reliable connection-oriented service. It ensures that data sent from the sender to the receiver is received accurately and in the correct order.
- **IP**: Internet Protocol handles addressing and routing the packets between the sender and the receiver.

### Explanation via Code
In the code provided, functions like `send()` and `read()` operate on raw byte streams.

#### Sending Data:
```c
// Server code snippet
const char *hello = "Hello from server";
send(new_socket, hello, strlen(hello), 0);  // Sending a string
```

```c
// Client code snippet
const char *hello = "Hello from client";
send(sock, hello, strlen(hello), 0);  // Sending a string
```

#### Receiving Data:
```c
// Server code snippet
char buffer[BUFFER_SIZE] = {0};
int valread = read(new_socket, buffer, BUFFER_SIZE);  // Receiving data into buffer
printf("Received from client: %s\n", buffer);  // Interpreting it as a string
```

```c
// Client code snippet
char buffer[BUFFER_SIZE] = {0};
int valread = read(sock, buffer, BUFFER_SIZE);  // Receiving data into buffer
printf("Received from server: %s\n", buffer);  // Interpreting it as a string
```

### Advantages of TCP
- **Reliability**: TCP ensures that no data is lost or duplicated and that the integrity of the data is maintained.
- **Order**: TCP ensures that bytes are received in the order they were sent.
- **Error Checking**: TCP includes mechanisms for error checking of packets.

### Custom Protocols and Formats

For more complex needs, you can define your own application-layer protocol on top of TCP. For example:
- You could define a protocol where each message starts with a fixed-size header specifying the length of the message body.
- You could use serialization formats such as JSON, XML, or Protocol Buffers to encode your data.

### Example: Using a Simple Custom Protocol

Suppose you want each message to start with a 4-byte integer indicating the length of the message. Here's how you might modify the code:

1. **Sending Data with Length Prefix**:
    ```c
    // Client code
    int length = strlen(hello);
    send(sock, &length, sizeof(length), 0);  // Send the length
    send(sock, hello, length, 0);  // Send the message
    ```

2. **Receiving Data with Length Prefix**:
    ```c
    // Server code
    int length;
    read(new_socket, &length, sizeof(length));  // Read the length
    read(new_socket, buffer, length);  // Read the message
    buffer[length] = '\0';  // Null-terminate the string
    printf("Received from client: %s\n", buffer);
    ```

This ensures that the server knows exactly how many bytes to read for each message, even if the message size varies.

### Conclusion

The provided example uses a basic TCP connection to send and receive plain text data. For more sophisticated communication, you can create custom protocols or use existing ones layered on top of TCP.

### Alternatives to TCP
- **UDP (User Datagram Protocol)**: A connectionless protocol that is faster but less reliable than TCP. Typically used for applications where speed is more critical than reliability (e.g., real-time video streaming, online gaming).
- **HTTP/HTTPS**: An application-layer protocol used for transferring hypermedia documents on the web.
- **WebSockets**: A protocol providing full-duplex communication channels over a single TCP connection, useful for real-time web applications.
- **Custom Protocols**: You can define your own protocol tailored to the specific needs of your application.

### Using UDP with Sockets

Here's an example of a simple UDP client and server:

#### UDP Server

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    len = sizeof(cliaddr);  // Length of client's address

    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
    buffer[n] = '\0';  // Null-terminate the received data
    printf("Received from client: %s\n", buffer);

    const char *hello = "Hello from server";
    sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
           (const struct sockaddr *)&cliaddr, len);
    printf("Hello message sent to client\n");

    close(sockfd);
    return 0;
}
```

#### UDP Client

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    const char *hello = "Hello from client";
    sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Hello message sent to server\n");

    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                     (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';  // Null-terminate the received data
    printf("Received from server: %s\n", buffer);

    close(sockfd);
    return 0;
}
```

### Custom Protocol Example

Suppose you want to define a simple custom application-layer protocol over UDP. Let's say each message starts with a 4-byte integer indicating the type of the message and a 4-byte integer indicating the length of the message. The rest of the message is the payload.

Here's how you can implement that:

#### Custom Protocol Server

```c
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
```

#### Custom Protocol Client

```c
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
```

### Explanation

- **CustomMessage Structure**: Encapsulates the message type, message length, and the payload.
- **Server**: Handles incoming messages, processes them based on type, and sends a response.
- **Client**: Creates a message with a custom type and payload, sends it to the server, and waits for a response.

### Running the Example

1. Compile the server and client:
    ```sh
    gcc -o custom_server custom_server.c
    gcc -o custom_client custom_client.c
    ```

2. Run the server:
    ```sh
    ./custom_server
    ```

3. Run the client in another terminal:
    ```sh
    ./custom_client
    ```

This setup sends custom-formatted messages over UDP, allowing both the client and server to understand the structure of the messages. You can further extend and modify this protocol to meet your specific needs.