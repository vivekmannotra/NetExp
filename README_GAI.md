# README GAI

**Explanation:**

1. **Include Headers:** The code starts by including the necessary headers for networking, I/O, and string manipulation.
2. **Define Port:** Define the port number you want to use.
3. **Create Socket:**
   - `socket(AF_INET, SOCK_STREAM, 0)`: Creates a TCP socket (`SOCK_STREAM`) for IPv4 (`AF_INET`).
4. **Set Socket Options (Optional):**
   - `setsockopt(..., SO_REUSEADDR, ...)`: This is optional but useful. It allows you to reuse the port even if it's still in a TIME_WAIT state from a previous execution.
5. **Bind to Address and Port:**
   - Create a `sockaddr_in` struct (`address`) to specify the address and port to bind to:
     - `sin_family = AF_INET`: IPv4
     - `sin_addr.s_addr = INADDR_ANY`: Bind to all available network interfaces on your machine.
     - `sin_port = htons(PORT)`: Use the specified port number (converted to network byte order).
   - `bind(server_fd, ...)`: Bind the created socket to the address and port.
6. **Listen for Connections:**
   - `listen(server_fd, 3)`:  Put the socket into listening mode. The second argument (3 in this case) specifies the maximum number of pending connections allowed in the queue.
7. **Accept Connection:**
   - `accept(server_fd, ...)`: This function blocks until a client tries to connect. Once a client connects, it accepts the connection and returns a new socket file descriptor (`new_socket`) that you'll use for communication with that specific client.
8. **Send and Receive Data:**
   - `send(new_socket, ...)`:  Sends data through the connected socket.
   - `read(new_socket, ...)`:  Receives data from the connected socket.
9. **Close Sockets:**
   - `close(new_socket)`: Close the client-specific socket.
   - `close(server_fd)`: Close the main server socket.

**To Compile and Run:**

1. **Save:** Save the code as a `.cpp` file (e.g., `server.cpp`).
2. **Compile:**
   ```bash
   g++ server.cpp -o server
   ```
3. **Run:**
   ```bash
   ./server
   ```

Now you have a simple server listening on the specified port. You can test it with a client program (like `telnet` or write another program) to connect to that port and send/receive data. 


If you're thinking of crafting your own network packets at a lower level! This is common for situations where TCP's overhead is undesirable (like high-performance applications or implementing custom protocols) or where UDP is a better fit.

Here's a breakdown of how you can achieve this, along with a C++ example for sending raw Ethernet frames:

**Key Concepts:**

* **Raw Sockets:**  These sockets bypass the typical transport layer protocols (TCP/UDP). You'll construct packets at the network layer (IP) or even the link layer (Ethernet).
* **Packet Structures:**  You need to define the structure of your packets according to the protocol you're using.
* **Endianness:**  Network byte order is big-endian, so you'll need to handle endianness conversions if your system uses little-endian.

**Example: Sending Raw Ethernet Frames (C++)**

```c++
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_ether.h> // For Ethernet structures
#include <net/if.h> // For if_nametoindex
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>

int main() {
    int sockfd;
    struct ifreq if_idx;
    struct sockaddr_ll socket_address;

    char ifName[] = "eth0"; // Replace with your interface name
    char sendbuf[BUF_SIZ];
    char recvbuf[BUF_SIZ];
    struct ether_header *eh = (struct ether_header *) sendbuf;
    struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
    struct udphdr *udph = (struct udphdr *) (sendbuf + sizeof(struct iphdr) + sizeof(struct ether_header));

    // Create a raw socket 
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // Get the index of the interface to send on
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        perror("SIOCGIFINDEX failed");
        return 1;
    }

    // Construct the Ethernet header
    memset(sendbuf, 0, BUF_SIZ);
    /* Ethernet header */
    eh->ether_shost[0] = 0x00;
    eh->ether_shost[1] = 0x00;
    eh->ether_shost[2] = 0x00;
    eh->ether_shost[3] = 0x00;
    eh->ether_shost[4] = 0x00;
    eh->ether_shost[5] = 0x01;

    eh->ether_dhost[0] = 0x00;
    eh->ether_dhost[1] = 0x00;
    eh->ether_dhost[2] = 0x00;
    eh->ether_dhost[3] = 0x00;
    eh->ether_dhost[4] = 0x00;
    eh->ether_dhost[5] = 0x02;

    eh->ether_type = htons(ETH_P_IP);

    /* IP header */
    iph->ihl = 5;
    iph->version = 4;
    iph->ttl = 20;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    iph->id = htons(54321); // some number
    iph->protocol = IPPROTO_UDP;
    iph->saddr = inet_addr("192.168.1.101"); // Spoofed address
    iph->daddr = inet_addr("192.168.1.1");
    iph->check = in_cksum((unsigned short *)iph, sizeof(struct iphdr)); // checksum

    /* UDP header */
    udph->source = htons(atoi("3423"));
    udph->dest = htons(atoi("9876"));
    udph->len = htons(sizeof(struct udphdr)); // UDP header size

    // Send the packet
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen = ETH_ALEN;
    /* MAC - begin */
    socket_address.sll_addr[0] = 0x00;
    socket_address.sll_addr[1] = 0x00;
    socket_address.sll_addr[2] = 0x00;
    socket_address.sll_addr[3] = 0x00;
    socket_address.sll_addr[4] = 0x00;
    socket_address.sll_addr[5] = 0x02;
    /* MAC - end */
    socket_address.sll_family = AF_PACKET;
    if (sendto(sockfd, sendbuf, iph->tot_len, 0, 
              (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
        perror("sendto failed");
    }

    close(sockfd);
    return 0;
}
```


**Important Notes:**

* **Permissions:** You'll likely need root privileges to create raw sockets.
* **Interface Name:** Replace `"eth0"` (or `en0` on macOS) with the actual name of your network interface.
* **Packet Structure:** You'll need to manually construct the headers and payloads of your packets according to the specific protocols you're working with. Refer to protocol specifications (RFCs) for details.
* **Error Handling:** Always include robust error handling in your code to catch potential issues.
* **Legal and Ethical Considerations:**  Using raw sockets can potentially be misused. Make sure you understand the implications and use this technique responsibly and ethically.

This code provides a foundation for building your custom networking solutions. However, remember that raw socket programming can be complex, so thorough understanding of networking concepts is crucial. 

Here are some additional tips to help you test and debug the raw socket code:

**Testing and Debugging:**

* **Network Analyzer (like Wireshark or tcpdump):** Use a network analyzer on the receiving machine (or even the same machine if you're sending to a local address) to:
    - Verify that your packets are being sent correctly.
    - Inspect the packet structure, including the headers you constructed.
    - Identify any errors in the packet format.
* **Loopback Interface (`lo`):**  For initial testing, consider sending packets to the loopback interface (`127.0.0.1` or the interface name `lo`) to avoid complications with physical network configurations.
* **Printing Values:**  Print out important values at various points in your code (e.g., socket file descriptors, buffer contents, return values of system calls) to help identify errors.
* **Error Handling:**  Pay close attention to the return values of functions like `socket()`, `bind()`, `sendto()`, etc.  Handle errors gracefully and provide informative error messages.

**Example `tcpdump` command:**

To capture packets on interface `eth0` with destination port 9876 (as in the example code):

```bash
sudo tcpdump -i eth0 udp port 9876 -vv 
```

**Additional Resources:**

* **Beej's Guide to Network Programming:** [https://beej.us/guide/bgnet/](https://beej.us/guide/bgnet/) (A classic and very helpful resource)
* **Linux man pages:** `man 7 packet`, `man 7 raw`
