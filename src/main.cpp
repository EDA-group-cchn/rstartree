#include <iostream>
#include <cassert>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>


#include "rstartree.h"

using namespace std;

typename RStarTree<>::BoundingBox GetRandomBB() {
  double a = rand() % 10000, b = rand() % 10000;
  return {{a, a + rand() % 5}, {b, b + rand() % 5}};
}

void Testing() {
  RStarTree<> rtree(4, 10);
  RStarTree<>::BoundingBox bb1{{1, 3}, {2, 5}}, bb2{{1, 4}, {1, 2}},
      bb3{{2, 3}, {3, 4}};
  
  RStarTree<>::BoundingBox s1,s2;
  RStarTree<>::BoundingBox r1{{1,4},{1,5}}, r2{{1,4},{1,4}}, r3{{1,4},{1,5}};
  s1 = bb1+bb2;
  s2 = bb2+bb3;
  
  // test sum for s1
  assert(s1.GetIntervalStart(0) == r1.GetIntervalStart(0));
  assert(s1.GetIntervalStart(1) == r1.GetIntervalStart(1));
  assert(s1.GetIntervalEnd(0) == r1.GetIntervalEnd(0));
  assert(s1.GetIntervalEnd(1) == r1.GetIntervalEnd(1));
  // test sum for s2
  assert(s2.GetIntervalStart(0) == r2.GetIntervalStart(0));
  assert(s2.GetIntervalStart(1) == r2.GetIntervalStart(1));
  assert(s2.GetIntervalEnd(0) == r2.GetIntervalEnd(0));
  assert(s2.GetIntervalEnd(1) == r2.GetIntervalEnd(1));
    
  r1+=r2;
  // test += 
  assert(r1.GetIntervalStart(0) == r3.GetIntervalStart(0));
  assert(r1.GetIntervalStart(1) == r3.GetIntervalStart(1));
  assert(r1.GetIntervalEnd(0) == r3.GetIntervalEnd(0));
  assert(r1.GetIntervalEnd(1) == r3.GetIntervalEnd(1));

  assert(bb1.HyperArea() == 6);
  assert(bb2.HyperArea() == 3);
  assert(bb3.HyperArea() == 1);

  assert(bb1 % bb2);
  assert(bb1 % bb3);
  assert(not (bb2 % bb3));

  assert(bb1.Overlap(bb2) == 0);
  assert(bb1.Overlap(bb3) == 1);
  assert(bb2.Overlap(bb3) == 0);

  rtree.Insert(bb1, 1);
  rtree.Insert(bb2, 2);
  rtree.Insert(bb3, 3);

  rtree.Insert({{5, 6}, {5, 6}}, 4);
  rtree.Insert({{8, 9}, {8, 9}}, 5);
  rtree.Insert({{12, 13}, {12, 13}}, 6);
  rtree.Insert({{15, 16}, {15, 16}}, 7);


  assert(rtree.Intersect(bb1).size() == 3);
  assert(rtree.Intersect({{0, 20}, {0, 20}}).size() == 7);
  rtree.Delete(bb1,1);
  assert(rtree.Intersect({{0, 20}, {0, 20}}).size() == 6);
  rtree.Delete(bb3,3);
  assert(rtree.Intersect({{0, 20}, {0, 20}}).size() == 5);
  rtree.Delete(bb2,2);
  assert(rtree.Intersect({{0, 20}, {0, 20}}).size() == 4);
  rtree.Delete({{5, 6}, {5, 6}}, 4);
  assert(rtree.Intersect({{0, 20}, {0, 20}}).size() == 3);

  for (size_t i = 0; i < 4000; ++i) {
    RStarTree<>::BoundingBox bb = GetRandomBB();
    rtree.Insert(bb, 8 + i);
    assert(rtree.Intersect(bb).size() != 0);
  }

}
void dostuff(int);
void consult_tree(int,double,double,double,double,double);
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[]) {
  Testing();

  int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } 
     close(sockfd);
  return 0;
}
void dostuff (int sock)
{
   int n,op;
   char buffer[256];
   RStarTree<> rtree;
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   std::stringstream ss;
   ss<<buffer;
   ss>>op;
   double v[5];
   for (int i = 0; ss!=0; ++i)
   {
     ss>>v[i];
   }
   switch(op)
   {
   		case 0:
   			std::cout<<"buscar";
   		case 1:
   			rtree.Insert({{v[0], v[1]}, {v[2], v[3]}}, v[4]);
   			n = write(sock,"element inserted",18);
   			if (n < 0) error("ERROR writing to socket");
   			break;
   		case 2:
   			rtree.Delete({{v[0], v[1]}, {v[2], v[3]}}, v[4]);
   			n = write(sock,"element deleted",18);
   			if (n < 0) error("ERROR writing to socket");
   			break;
   		case 3:
   			std::cout<<"salir";
   		


   }
   
   
}
