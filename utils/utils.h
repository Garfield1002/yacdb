#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>

const char* SERVER_IP = "127.0.0.1";

#define REPLY_SIZE 2000
#define LINE_SIZE 100
#define SERVER_PORT 4000
#define DEBUG 1

void exit_msg(char* msg, int err) {
  fprintf(stderr, "Error - %s: ", msg);
  if (err) perror(NULL);
  exit(1);
}

#endif