#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAXBUFLEN 20000
#define FILE_TO_WRITE "received_file.json"

int server_fd, new_socket;
char file_type[1024] = {0};
long int bufsize;
char buf[MAXBUFLEN];
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[20000] = {0};
char hello[] = "Hello from server";
FILE *fd;

void socket_create() {
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    // perror("socket failed");
    std::cerr << "socket failed" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void socket_options() {
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    std::cerr << "setsockopt error" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void bind_to_port() {
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "bind failed" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void listen_on_port() {
  if (listen(server_fd, 3) < 0) {
    std::cerr << "error listening on port" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void accept_connection() {
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    std::cerr << "error accepting request" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void size_of_data() {
  int i = 0;
  while (buf[i] != '\0') {
    i++;
  }
  bufsize = sizeof(char) * i;
  std::cout << "size of data: " << bufsize << " bytes" << std::endl;
}

int main(int argc, char const *argv[]) {
  std::string str;
  char file_path[MAXBUFLEN];

  /*Creating socket file descriptor*/
  socket_create();

  /*Setting socket options (optional)*/
  socket_options();

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  /*Forcefully attaching socket to the port 8080*/
  bind_to_port();

  /*Listen on port*/
  listen_on_port();

  /*Accept connection requests*/
  accept_connection();

  /*Send and receive data and messages*/
  read(new_socket, buffer, 1024);
  std::cout << buffer << std::endl;

  send(new_socket, hello, strlen(hello), 0);
  std::cout << "Hello message sent" << std::endl;

  std::cout << "Select type of file to receive:\n 'text' for text file, 'json' "
               "for json file, 'cpp' for cpp file , 'srec' for srec file"
            << std::endl;

  std::cin >> file_type;
  send(new_socket, file_type, strlen(file_type), 0);

  read(new_socket, buf, 4835);
  std::cout << buf << std::endl;

  size_of_data();

  /*Writing received data into a text file*/
  str = file_type;
  if(str == "deb")
  {
    fd = fopen("received.zip", "wb");
    fwrite(buf, sizeof(char), 4835, fd);
    fclose(fd);
  }

  if(str == "json")
  {
    fd = fopen("received_file.json", "w");
    fwrite(buf, sizeof(char), bufsize, fd);
    fclose(fd);
  }

  if(str == "text")
  {
    fd = fopen("received_file.txt", "w");
    fwrite(buf, sizeof(char), bufsize, fd);
    fclose(fd);
  }
  
  if (str == "srec")
{
    fd = fopen("/root/server/server1/bootparam_sa0.srec", "wb");
    fwrite(buf, sizeof(char), bufsize, fd);
    fclose(fd);
  }

  return 0;
}
