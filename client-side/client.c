#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "../utils/utils.h"

void send_line_receive_response(char *line, char *response)
{
  int socket_desc;
  struct sockaddr_in server;
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
  if (recv(socket_desc, response, REPLY_SIZE, 0) < 0)
  {
    exit_msg("Réception du message", 1);
  }
  close(socket_desc);
}

void file_management(char *path)
{
  FILE *f = fopen(path, "r");
  if (f == NULL)
  {
    printf("ERROR while opening the file\n");
    return;
  }
  char line[LINE_SIZE];
  char response[REPLY_SIZE];
  memset(line, 0, LINE_SIZE);
  while (fgets(line, LINE_SIZE, f))
  {
    if (strlen(line) == LINE_SIZE - 1 && line[LINE_SIZE - 2] != '\n')
    {
      printf("Line too long ! The line on your file cannot exceed %d characters.\nLine is: %s\n", LINE_SIZE - 1, line);
      fclose(f);
      return;
    }
    if (strlen(line) == 1) // Empty line
      continue;
    send_line_receive_response(line, response);
    printf("%s\n", response);
  }
  fclose(f);
  return;
}

int main(int argc, char **argv)
{
  char line[LINE_SIZE];
  char response[REPLY_SIZE];
  if (argc == 2)
  {
    printf("  YACDB file mode\n");
    file_management(argv[1]);

    return 0;
  }
  printf("\n-- Welcome to the yacDB console ! -- \n");
  printf("Available command are :\n\n");
  printf("crt IDENT_table with IDENT_col=TYPE, ...\n");
  printf("sel IDENT_col, ... from IDENT_table\n");
  printf("add IDENT_col=VAL, .. in IDENT_table\n\n");
  printf("type q or exit to quit the console\n\n");
  while (1)
  {
    memset(line, 0, LINE_SIZE);
    memset(response, 0, REPLY_SIZE);
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
    send_line_receive_response(line, response);
    printf("%s\n", response);
  }
  DEBUG_PRINT("Clean exit\n");

  return 0;
}
