
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include <string.h>
#include "../utils/utils.h"
#include "core/include/core.h"

int nb_threads = 0;
sem_t semaphore;
int socket_desc;

void sigintHandler(int sig_num)
{

  sem_destroy(&semaphore);
  close(socket_desc);

  printf("\nSERVER SHUTDOWN\n");
  exit(0);
}

// Configuration de la socket réseau, retourne le file descriptor de la socket
int configure_socket()
{
  struct sockaddr_in server;

  // Création de la socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1)
    exit_msg("Création de la socket: ", 1);
  puts("INIT: Socket crée");

  // Configuration des informations concernant sur la socket
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(0x7f000001);
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
  return socket_desc;
}

void exit_thread(int socket_desc)
{
  nb_threads--;
  puts("Terminaison d'une connexion client");
  printf("Nombre de threads actifs: %d\n\n", nb_threads);
  close(socket_desc);
  pthread_exit(NULL);
}

// Passage des commandes à la base de données par un pipe
// Renvoi des réponses au client par la socket réseau
void process_communication(int thread_socket)
{
  char **args;
  int fds[2];
  char line[LINE_SIZE];
  char answer[REPLY_SIZE];
  int ready;
  // rfds contient les descripteurs à surveiller
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(thread_socket, &rfds);

  // on configure le timeout à 60s
  struct timeval timeout;

  while (1)
  {
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    // Wait until socket ready, with timeout
    ready = select(thread_socket + 1, &rfds, NULL, NULL, &timeout);
    if (ready == -1)
    {
      perror("select()");
      exit_thread(thread_socket);
    }
    else if (ready == 0)
    {
      puts("Client timeout");
      exit_thread(thread_socket);
    }

    // Lecture dans la socket
    memset(line, 0, LINE_SIZE);
    ready = read(thread_socket, line, LINE_SIZE);

    DEBUG_PRINT("server: Commande reçue: --%s--\n\n", line);

    if (ready < 0 || strlen(line) == 0)
    {
      // Déconnexion
      exit_thread(thread_socket);
    }

    // Taking the semaphore
    if (sem_wait(&semaphore) < 0)
      exit_msg("Attente du sémaphore: ", 1);
    DEBUG_PRINT("server: prise du sémaphore");

    pipe(fds);                     // In via fds[1], out via fds[0]
    FILE *f = fdopen(fds[1], "w"); // file for easy response management

    // Core processing
    db_handle(f, line);


    fclose(f);

    // Free the semaphore
    if (sem_post(&semaphore) < 0)
      exit_msg("Retour du sémaphore: ", 1);
    DEBUG_PRINT("server: libération du sémaphore");

    // Reading answer from pipe
    memset(answer, 0, REPLY_SIZE);
    read(fds[0], answer, REPLY_SIZE);
    DEBUG_PRINT("server: Réponse reçue: \n%s\n", answer);

    // Writing answer to client
    write(thread_socket, answer, REPLY_SIZE);
    close(fds[0]);
    close(fds[1]);
  }
}

void *process_communication_thread(void *arg)
{
  int i = *((int *)arg);
  process_communication(i);
}

int main(int argc, char **argv)
{
  signal(SIGINT, sigintHandler); // close socket on interrupt
  int new_socket;
  int c;
  struct sockaddr_in client;
  pthread_t thread;

  // Configuration de la socket serveur
  socket_desc = configure_socket();

  init_database();

  // Configuration du sémaphore
  if (sem_init(&semaphore, 0, 1) < 0)
    exit_msg("Initialisation du sémaphore: ", 1);

  // Réception des connections réseaux entrantes
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

  // Libération des ressources
  sem_destroy(&semaphore);
  close(socket_desc);

  return 0;
}
