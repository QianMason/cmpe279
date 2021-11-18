// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#define PORT 8080


int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";\
    struct passwd *user_data = getpwnam("nobody");
    if (argc == 1) {


        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        if (bind(server_fd, (struct sockaddr *)&address,
        sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        //fork here

        int pid = fork();
        if (pid < 0) {
            perror("error in fork");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            printf("Waiting for child process");
            wait(NULL);
        } else {
            uid_t pw_uid = user_data->pw_uid;
            if (pw_uid < 0) {
                printf("Insufficient privileges.");
                perror("Privilege error");
                exit(EXIT_FAILURE);
            } else {
                char stringSocket[16]; 
                sprintf(stringSocket, "%d", new_socket);
                char *params[] = {"child", stringSocket};
                if (execvp(argv[0], params) == -1) {
                    perror("execvp error");
                    exit(EXIT_FAILURE);
                }
            }
        }
    } else {
        sscanf(argv[1], "%d", &new_socket);
        valread = read(new_socket, buffer, 1024);
        printf("Read %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    }
    return 0;
}
