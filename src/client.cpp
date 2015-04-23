#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "command.h"

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

void run(int socket);

void test() {
  char buffer[256];
  Command command;
  while (true) {
    printf("> ");
    fflush(stdout);
    fgets(buffer, 255, stdin);
    command = Command::Parse(std::string(buffer));
    puts(command.String().c_str());
    if (command.OpCode() == 3)
      break;
  }
  exit(0);
}

int main(int argc, char *argv[])
{
  test();
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");
  run(sockfd);
  return 0;
}

void run(int sockfd) {
  char buffer[256];
  int n;
  std::map<std::string, int>::iterator it;
  while (true) {
    printf("> ");
    fflush(stdout);
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    Command command = Command::Parse(std::string(buffer));
    if (command.OpCode() != -1) {
      const char *msg = command.String().c_str();
      n = write(sockfd, msg, strlen(msg));
      if (n < 0)
        error("ERROR writing to socket");
      bzero(buffer, 256);
      n = read(sockfd, buffer, 255);
      if (n < 0)
        error("ERROR reading from socket");
      puts(buffer);
      if (command.OpCode() == 3) {
        break;
      }
    } else {
      puts("Invalid command");
    }
  }
  close(sockfd);
}
