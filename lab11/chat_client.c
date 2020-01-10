#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 30000
#endif
#define BUF_SIZE 128


/* get and send username */
int send_username(int sock_fd){
    // get username from stdin
    char username[BUF_SIZE + 1];
    int nread;
    if((nread = read(STDIN_FILENO, username, BUF_SIZE)) < 0) {
        perror("client read username");
        exit(1);
    }
    username[nread] = '\0';

    // send username to server
    int nwrite = write(sock_fd, username, nread);
    if(nwrite != nread) {
        perror("client: write user");
        close(sock_fd);
        exit(1);
    }

    return 0;
}

int main(void) {
    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 1) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        exit(1);
    }

    // Get and send username
    send_username(sock_fd);

	// // Get the user to provide a name.
    // char buf[2 * BUF_SIZE + 1];            // 2x to allow for usernames
    // printf("Please enter a username: ");
    // fflush(stdout);
    // int num_read = read(STDIN_FILENO, buf, BUF_SIZE);
    // buf[num_read] = '\0';
    // write(sock_fd, buf, num_read);

    // Read input from the user, send it to the server, and then accept the
    // echo that returns. Exit when stdin is closed.
    char buf[BUF_SIZE + 1];

    // fd_set
    int max_fd = STDIN_FILENO;
    if(sock_fd > max_fd) max_fd = sock_fd;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock_fd, &fds);
    FD_SET(STDIN_FILENO, &fds);

    while (1) {
        // make copy to select from
        fd_set listen_fds = fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1){
            perror("client: select");
            exit(1);
        }
        
        if(FD_ISSET(sock_fd, &listen_fds)) {
            int num_read = read(sock_fd, buf, BUF_SIZE);
            buf[num_read] = '\0';
            printf("Received from server: %s", buf);
        } 

        if(FD_ISSET(STDIN_FILENO, &listen_fds)) {
            send_username(sock_fd);
        }
        // int num_read = read(STDIN_FILENO, buf, BUF_SIZE);
        // if (num_read == 0) {
        //     break;
        // }
        // buf[num_read] = '\0';         
		// // Should really send '\r\n'
        // int num_written = write(sock_fd, buf, num_read);
        // if (num_written != num_read) {
        //     perror("client: write");
        //     close(sock_fd);
        //     exit(1);
        // }

        // num_read = read(sock_fd, buf, BUF_SIZE);
        // buf[num_read] = '\0';
        // printf("Received from server: %s", buf);
    }

    close(sock_fd);
    return 0;
}
