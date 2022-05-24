#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "../utils/utils.h"

int main(int argc, char **argv)
{
  int socket_desc;
  struct sockaddr_in server;
  char line[LINE_SIZE];
  char reponse[REPLY_SIZE];

  printf("\n-- Welcome to the yacDB console ! -- \n");
  printf("Available command are :\n\n");
  printf("crt IDENT_table with IDENT_col=TYPE, ...\n");
  printf("sel IDENT_col, ... from IDENT_table\n");
  printf("add IDENT_col=VAL, .. in IDENT_table\n\n");
  printf("type q or exit to quit the console\n\n");
  while (1)
  {
    memset(line, 0, LINE_SIZE);
    memset(reponse, 0, REPLY_SIZE);
    printf("-> ");
    // Retrieving the command line
    fgets(line, LINE_SIZE, stdin);

    if (line[0] == '\n') // if strats with a \n, nothing to do, restart
    {
      continue;
    }
    line[strlen(line) - 1] = '\0'; // remove trailing \n

    DEBUG_PRINT("<- %s-%ld\n", line, strlen(line));

    if (!strcmp(line, "q") || !strcmp(line, "exit"))
    {
      break;
    }

    // Création d'une socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
      exit_msg("Création de socket", 1);

    // Informations sur le serveur
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Connection au serveur
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
      exit_msg("Connection de la socket", 1);
    }

    // Envoi des données
    if (send(socket_desc, line, LINE_SIZE, 0) < 0)
    {
      exit_msg("Envoi de données", 1);
    }

    // Réception du message de retour
    if (recv(socket_desc, reponse, REPLY_SIZE, 0) < 0)
    {
      exit_msg("Réception du message", 1);
    }
    close(socket_desc);
    printf("%s\n", reponse);
  }
  DEBUG_PRINT("Clean exit\n");

  return 0;
}
