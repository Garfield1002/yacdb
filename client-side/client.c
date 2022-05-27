/*begin hide*/
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
/*end hide*/
#include "../utils/utils.h"

int main(int argc, char** argv) {
/*begin hide*/
  int socket_desc;
  struct sockaddr_in server;
  char message[LINE_SIZE];
  char reponse[REPLY_SIZE];

  //Création d'une socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) exit_msg("Création de socket", 1);

  //Informations sur le serveur
  server.sin_addr.s_addr = inet_addr(SERVER_IP);
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);

  //Connection au serveur
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
    exit_msg("Connection de la socket", 1);
  }

  printf("Bonjour, quelle est votre commande?\n");
  while(1) {
    memset(message, 0, LINE_SIZE);
    //Récupération des commandes
    fgets(message, LINE_SIZE, stdin);

    if (DEBUG) printf("client: Commande envoyée: %s", message);
    //Envoi des données
    if(send(socket_desc, message, LINE_SIZE, 0) < 0) {
      exit_msg("Envoi de données", 1);
    }

    //Réception du message de retour
    if (recv(socket_desc, reponse, REPLY_SIZE, 0) < 0) {
      exit_msg("Réception du message", 1);
    }
    printf("%s", reponse);
    printf("\n\nCommande suivante:\n");
  }
  close(socket_desc);
  /*end hide*/
  return 0;
}
