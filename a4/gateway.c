#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>
#include "socket.h"  
#include "message.h"
#include "controller.h"

#define MAXFD(x,y) ((x) >= (y)) ? (x) : (y)

/* Add client into client_fds*/
int add_client(int client_fds[MAXDEV], int client){
	int i = 0;
	while (client_fds[i] != -1 && i < MAXDEV){
		i++;
	}
	if (i == MAXDEV){
		return -1;
	}
	client_fds[i] = client;
	return 1;
}

int main(int argc, char *argv[]){
	int port;
	struct cignal cig;
	// A buffer to store a serialized message
	char *cig_serialized = malloc(sizeof(char)*CIGLEN);
	// An array to registered sensor devices
	int device_record[MAXDEV] = {0};
	
	if(argc == 2){
		port = strtol(argv[1], NULL, 0);
	} else{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}

	int gatewayfd = set_up_server_socket(port);
	printf("\nThe Cignal Gateway is now started on port: %d\n\n", port);
	int peerfd;
	
	/* TODO: Implement the body of the server.  
	 *
	 * Use select so that the server process never blocks on any call except
	 * select. If no sensors connect and/or send messsages in a timespan of
	 * 5 seconds then select will return and print the message "Waiting for
	 * Sensors update..." and go back to waiting for an event.
	 * 
	 * The server will handle connections from devices, will read a message from
	 * a sensor, process the message (using process_message), write back
	 * a response message to the sensor client, and close the connection.
	 * After reading a message, your program must print the "RAW MESSAGE"
	 * message below, which shows the serialized message received from the *
	 * client.
	 * 
	 *  Print statements you must use:
     * 	printf("Waiting for Sensors update...\n");
	 * 	printf("RAW MESSAGE: %s\n", YOUR_VARIABLE);
	 */


	// The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = gatewayfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(gatewayfd, &all_fds);

	// set an array to store all connected clients
	int client_fds[MAXDEV];
	for (int i = 0; i < MAXDEV; i++){
		client_fds[i] = -1;
	}

	while(1) {
        // time counter
		struct timeval time;
		time.tv_sec = 5;
		time.tv_usec = 0;
		// select updates the fd_set it receives, so we always use a copy and retain the original.
		fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, &time);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }

		// if timeout
		if (nready == 0){
			printf("Waiting for Sensors update...\n");
			continue;
		}

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(gatewayfd, &listen_fds)) {
            peerfd = accept_connection(gatewayfd);
			max_fd = MAXFD(max_fd, peerfd); // update maxfd
            FD_SET(peerfd, &all_fds);
			if (add_client(client_fds, peerfd) == -1){
				fprintf(stderr, "Maximum number of connections reached\n");
				exit(1);
			}
		}

		// check all clients
		for (int j= 0; j < MAXDEV; j++){
			if (client_fds[j] > -1 && FD_ISSET(client_fds[j], &listen_fds)){ // if currently connected
				int nread = read(client_fds[j], cig_serialized, CIGLEN);
				printf("RAW MESSAGE: %s\n", cig_serialized);
				if (nread > 0){
					unpack_cignal(cig_serialized, &cig); // unpack received char
					if (process_message(&cig, device_record) == 1){
						cig_serialized = serialize_cignal(cig); // serialize struct for write
						write(client_fds[j], cig_serialized, CIGLEN);
						free(cig_serialized);
					} else {
						fprintf(stderr, "Invalid message");
						continue;
					}
				} else if (nread  == -1){
					perror("server: read");
					exit(1);
				}
				// remove this client from fdset and clear this client in client array
				FD_CLR(client_fds[j], &all_fds);
				close(client_fds[j]);
				client_fds[j] = -1;
			}
		}
	}
	return 0;
}
