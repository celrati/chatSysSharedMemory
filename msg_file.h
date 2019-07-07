#ifndef MSG_FILE
#define MSG_FILE


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>


#define SIZE_FILE  1024 // max size

typedef struct Subscriber Subscriber;
struct Subscriber{
  pid_t pid_process;
  int signal_to_send;
};


typedef struct Header Header;//struct en-tete
struct Header{

    #define SIZE_NAME_ROOM 2 << 8
    char name_room[SIZE_NAME_ROOM];  // the name of the FILE
    size_t len_max_msg;
    size_t capacity;
    int first; // index of the first message..
    int last;  // index of the last message..
    int number_of_messages_right_now;
    int nbr_process;

    // semaphores.. to handle later :/
    sem_t mutex_write_acces; // done
    sem_t mutex_read_acces; // done


    Subscriber subscriber; // just one subsriber at time
    int subs_on; // 0 or 1 
    
  };

#define ULTIMATE_SIZE 2 << 8
typedef struct Array_messages Array_messages;
struct Array_messages{
  int size_message;
  pid_t pid_user; // the pid of the sender..
  char message[ ULTIMATE_SIZE ]; // maximal size of message..
};


typedef struct File_message File_message;
struct File_message{
  Header header;
  Array_messages messages[ ULTIMATE_SIZE ]; // 2 ^ 4 messages
};


#define READ 0x1
#define WRITE 0x2
#define READ_WRITE 0x4

typedef struct MESSAGE MESSAGE;
struct MESSAGE{
  int type_file;
  File_message * file_message; // the shared memory
};

typedef struct MSG_BULLE MSG_BULLE;
struct MSG_BULLE{
  char * message_content;
  int message_size;
  pid_t pid_user;
};



MESSAGE * msg_connect(const char *nom, int options, ... ); //return null en cas d'echec, return pointeur vers un objet MESSAGE


int msg_disconnect(MESSAGE *file); //return 0 si OK et -1 sinon
int msg_unlink(const char *nom); //return 0 si OK et -1 sinon


/* Si la longueur du message est plus grande que la longueur maximale supportée par la file les
** deux fonctions msg_send et msg_trysend retournent immédiatement −1 et mettent EMSGSIZE dans errno
*/

int msg_send(MESSAGE *file, const char *msg, size_t len); //return 0 si OK et -1 sinon 
int msg_trysend(MESSAGE *file, const char *msg, size_t len); //return 0 si OK et -1 sinon


ssize_t msg_receive(MESSAGE *file, char *msg, size_t len); //retournent le nombre d’octets du message lu si OK et -1 sinon
ssize_t msg_tryreceive(MESSAGE *file, char *msg, size_t len); //retournent le nombre d’octets du message lu si OK et -1 sinon

size_t msg_message_size(MESSAGE *file); //return la taille maximale d’un message
size_t msg_capacite(MESSAGE *file); //return  le nombre maximal de messages dans la file
size_t msg_nb(MESSAGE *file); //return le nombre de messages actuellement dans la file
size_t msg_nb_left(MESSAGE * file); // return le nombre des messages vides resté dans la file a remplir..


/**
*  functions for extensions 6.1
*   readv writev
*/

int writev(MESSAGE * file, char ** list_messages, int how_many_messages); // return 0  or -1 
char ** readv(MESSAGE * file, int how_many_messages);


#endif