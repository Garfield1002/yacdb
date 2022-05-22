
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#include <string.h>
#include "../utils/utils.h"
#include "parser/include/main_parser.h"

int nb_threads = 0;
sem_t semaphore;

// Configuration de la socket réseau, retourne le file descriptor de la socket
int configure_socket()
{
  int socket_desc;
  /*begin hide*/
  struct sockaddr_in server;

  // Création de la socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1)
    exit_msg("Création de la socket: ", 1);
  puts("INIT: Socket crée");

  // Configuration des informations concernant sur la socket
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(SERVER_PORT);

  // Association de la socket a un port et adresse
  if (bind(socket_desc, (struct sockaddr *)&server,
           sizeof(struct sockaddr_in)) < 0)
  {
    exit_msg("Bind de la socket: ", 1);
  }
  puts("INIT: Socket configurée");

  // Passage de la socket à l'état "listen"
  if (listen(socket_desc, 5) < 0)
    exit_msg("Ecoute de la socket: ", 1);
  puts("INIT: Socket à l'écoute!\n");
  /*end hide*/
  return socket_desc;
}

/*begin hide*/
void exit_thread(int socket_desc)
{
  nb_threads--;
  puts("Terminaison d'une connexion client");
  printf("Nombre de threads actifs: %d\n\n", nb_threads);
  close(socket_desc);
  pthread_exit(NULL);
}
/*end hide*/

// Passage des commandes à la base de données par un pipe
// Renvoi des réponses au client par la socket réseau
void process_communication(int socket_desc)
{
  /*begin hide*/
  char **args;
  int fds[2];
  char line[LINE_SIZE];
  char answer[REPLY_SIZE];
  int ready;
  // rfds contient les descripteurs à surveiller
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(socket_desc, &rfds);

  // on configure le timeout à 60s
  struct timeval timeout;
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;


  while (1)
  {
    // Attend jusqu'à 60s qu'il y ait un message dans la socket
    ready = select(socket_desc + 1, &rfds, NULL, NULL, &timeout);
    if (ready == -1)
    {
      perror("select()");
      exit_thread(socket_desc);
    }
    else if (ready == 0)
    {
      puts("Client timeout");
      exit_thread(socket_desc);
    }

    // Lecture dans la socket
    memset(line, 0, LINE_SIZE);
    ready = read(socket_desc, line, LINE_SIZE);
    if (DEBUG)
      printf("server: Commande reçue: %s\n\n", line);

    if (ready < 0 || strlen(line) == 0)
    {
      // Déconnexion
      exit_thread(socket_desc);
    }

    // pipe(fds);
    // On incrémente le sémaphore
    if (sem_wait(&semaphore) < 0)
      exit_msg("Attente du sémaphore: ", 1);
    if (DEBUG)
      puts("server: prise du sémaphore");
    // sleep(10);
    // pid_t pid = fork();
    // if (pid == -1) exit_msg("Fork: ", 1);

    // if (pid == 0) {
    // Processus fils

    // Connection du pipe à stdout
    // dup2(fds[1], STDOUT_FILENO);
    // close(fds[0]);
    // close(fds[1]);


    // execv(parser_bin, args);
    // } else {
    // Processus père

    // Connection du pipe à stdin
    // close(fds[1]);
    // wait(NULL);

    instr *instr = parse_instr(line);
    if (instr)
      dump_instr(instr);

    // On rend le sémaphore
    if (sem_post(&semaphore) < 0)
      exit_msg("Retour du sémaphore: ", 1);
    if (DEBUG)
      puts("server: libération du sémaphore");

    // Lecture du processus bdd
    memset(answer, 0, REPLY_SIZE);
    read(fds[0], answer, REPLY_SIZE);
    if (DEBUG)
      printf("server: Réponse reçue: \n%s\n", answer);
    // Passage des messages à la socket
    write(socket_desc, answer, REPLY_SIZE);
    close(fds[0]);
    // }
  }
  /*end hide*/
}

/*begin hide*/
void *process_communication_thread(void *arg)
{
  int i = *((int *)arg);
  process_communication(i);
}
/*end hide*/

int main(int argc, char **argv)
{
  int socket_desc;
  int new_socket;
  /*begin hide*/
  int c;
  struct sockaddr_in client;
  pthread_t thread;
  /*end hide*/

  // Configuration de la socket serveur
  socket_desc = configure_socket();

  /*begin hide*/
  // Configuration du sémaphore
  if (sem_init(&semaphore, 0, 1) < 0)
    exit_msg("Initialisation du sémaphore: ", 1);
  /*end hide*/

  // Réception des connections réseaux entrantes
  /*begin hide*/
  c = sizeof(struct sockaddr_in);
  while (1)
  {
    new_socket = accept(socket_desc, (struct sockaddr *)&client, &c);
    if (new_socket < 0)
      exit_msg("Acception de la socket: ", 1);
    puts("Connection entrante détectée =)");
    // On lance le thread
    if (pthread_create(&thread, NULL,
                       process_communication_thread, (void *)&new_socket) != 0)
    {
      exit_msg("Création de thread: ", 0);
      break;
    }
    nb_threads++;
    puts("Thread assigné");
    printf("Nombre de threads actifs: %d\n\n", nb_threads);
  }
  /*end hide*/

  // Libération des ressources
  /*begin hide*/
  sem_destroy(&semaphore);
  close(socket_desc);

  /*end hide*/
  return 0;
}
