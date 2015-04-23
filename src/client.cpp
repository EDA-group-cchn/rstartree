#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <map>
#include <sstream>

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

void run(int socket);

int main(int argc, char *argv[])
{
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

struct Command {
  int op_code;
  double x1, x2, y1, y2;
  size_t value;
  static std::map<std::string, int> operations;
  static Command parse(const std::string s) {
    Command ans;
    ans.op_code = -1;
    return ans;
  }
  std::string toString() {
    std::stringstream ss;
    ss << op_code << x1 << x2 << y1 << y2 << value;
    return ss.str();
  }
};

std::map<std::string, int> Command::operations = {
    {"SELECT", 0},
    {"INSERT", 1},
    {"DELETE", 2},
    {"EXIT", 3}
};

void run(int sockfd) {
  char buffer[256];
  int n;
  std::map<std::string, int>::iterator it;
  while (true) {
    printf("> ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    Command command = Command::parse(std::string(buffer, 255));
    if (command.op_code != -1) {
      if (command.op_code == 3)
        break;
      const char *msg = command.toString().c_str();
      n = write(sockfd, msg, strlen(msg));
      if (n < 0)
        error("ERROR writing to socket");
      bzero(buffer, 256);
      n = read(sockfd, buffer, 255);
      if (n < 0)
        error("ERROR reading from socket");
      puts(buffer);
    } else {
      puts("Invalid command");
    }
  }
  close(sockfd);
}
