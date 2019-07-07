#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include "../lib/subs.h"

typedef struct dict_entry_s {
    char *key;
    MESSAGE * value;
} dict_entry_s;

typedef struct dict_s {
    int len;
    int cap;
    dict_entry_s *entry;
} dict_s, *dict_t;

int dict_find_index(dict_t dict, const char *key) {
    for (int i = 0; i < dict->len; i++) {
        if (!strcmp(dict->entry[i].key, key)) {
            return i;
        }
    }
    return -1;
}

MESSAGE * dict_find(dict_t dict, const char *key, int def) {
    int idx = dict_find_index(dict, key);
    return idx == -1 ? NULL : dict->entry[idx].value;
}

void dict_add(dict_t dict, const char *key, MESSAGE * value) {
   int idx = dict_find_index(dict, key);
   if (idx != -1) {
       dict->entry[idx].value = value;
       return;
   }
   if (dict->len == dict->cap) {
       dict->cap *= 2;
       dict->entry = realloc(dict->entry, dict->cap * sizeof(dict_entry_s));
   }
   dict->entry[dict->len].key = strdup(key);
   dict->entry[dict->len].value = value;
   dict->len++;
}

dict_t dict_new(void) {
    dict_s proto = {0, 10, malloc(10 * sizeof(dict_entry_s))};
    dict_t d = malloc(sizeof(dict_s));
    *d = proto;
    return d;
}

void dict_free(dict_t dict) {
    for (int i = 0; i < dict->len; i++) {
        free(dict->entry[i].key);
    }
    free(dict->entry);
    free(dict);
}

int TO_SEND = 0;
MESSAGE *current_room;
dict_t rooms;

void almost_c99_signal_handler(int sig){
  	switch(sig){
		case SIGABRT:
		fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
		break;
		case SIGFPE:
		fputs("Caught SIGFPE: arithmetic exception, such as divide by zero\n",
			stderr);
		break;
		case SIGILL:
		fputs("Caught SIGILL: illegal instruction\n", stderr);
		break;
		case SIGINT:
		fputs("Caught SIGINT: probably a CTRL+C\n",
			stderr);
		break;
		case SIGSEGV:
		fputs("Caught SIGSEGV: segfault, The file is unusable\n", stderr);
		break;
		case SIGTERM:
		default:
		fputs("Caught SIGTERM: a termination request was sent to the program\n",
			stderr);
		break;
  	}
  	_Exit(1);
}

void set_signal_handler(){
  signal(SIGABRT, almost_c99_signal_handler);
  signal(SIGFPE,  almost_c99_signal_handler);
  signal(SIGILL,  almost_c99_signal_handler);
  signal(SIGINT,  almost_c99_signal_handler);
  signal(SIGSEGV, almost_c99_signal_handler);
  signal(SIGTERM, almost_c99_signal_handler);
}

void handlerSubscriber(int signum){
	//printf("\033[H\033[J"); // to clear the screen..
	printf("\n new message arrived !! ! \n ");
}


void setSignal(){
	struct sigaction action;
	action.sa_handler = handlerSubscriber;
	sigaction(SIGUSR1, &action, NULL);
}
 
static char const * icky_global_program_name;

int commands(const char *nom){
    if(strcmp(nom, "CREATE") == 0) return 0;
    else if(strcmp(nom, "CONNECT") == 0) return 1;
    else if(strcmp(nom, "DISCONNECT") == 0) return 2;
    else if(strcmp(nom, "SEND") == 0) return 3;
    else if(strcmp(nom, "RECEIVE") == 0) return 4;
    else if(strcmp(nom, "SENDLOOP") == 0) return 5;
    else if(strcmp(nom, "RECEIVELOOP") == 0) return 6;
    else if(strcmp(nom, "WRITEV") == 0) return 7;
    else if(strcmp(nom, "READV") == 0) return 8;
    else if(strcmp(nom, "SUBSCRIBE") == 0) return 9;
    else if(strcmp(nom, "UNSUBSCRIBE") == 0) return 10;
    else return 11;
}

void execute_command(const char *nom){
    int tmp = commands(nom);
    switch(tmp){
        case 0://CREATE
            printf("Enter room name : ");
            char name[2 << 8];
            scanf("%s", name);
            printf("Enter nb_msg: ");
            int nb_msg;
            scanf("%d", &nb_msg);
            puts("creating a new file...");
            dict_add(rooms, name, msg_connect(name, O_CREAT | O_RDWR, nb_msg, 2 << 8)); // 2 messages  of 2^8 bytes
            printf("you are connected to the room [%s]\n", name);
            break;
        case 1://CONNECT
            printf("Enter room name : ");
            char name2[2 << 8];
            scanf("%s", name2);
            //current_room =  msg_connect(name2, O_RDWR);
            dict_add(rooms, name2, msg_connect(name2, O_RDWR));
            if(dict_find(rooms, name2, 0) != NULL) printf("you are connected to the room [%s]\n", name2);
            else printf("Couldn't connect to the room !\n");
            break;
        case 2://DISCONNECT
            printf("Enter room name : ");
            char name3[2 << 8];
            scanf("%s", name3);
            msg_disconnect(dict_find(rooms, name3, 0));
            break;
        case 3://SEND
            printf("Enter room name : ");
            char name4[2 << 8];
            scanf("%s", name4);
            printf("Enter a message : ");
            char message[256];
            getchar();
            fgets(message, 256, stdin);
            message[strlen(message)-1] = '\0';
            msg_send(dict_find(rooms, name4, 0), message, strlen(message));
            break;
        case 4://RECEIVE
            printf("Enter room name : ");
            char name5[2 << 8];
            scanf("%s", name5);
            printf("[%s]", dict_find(rooms, name5, 0)->file_message->header.name_room);
            char * msg_to_read;
            msg_to_read = (char*)malloc(2<<16);
            msg_receive(dict_find(rooms, name5, 0), msg_to_read, 2 << 16);
            break;
        case 5://SENDLOOP
            printf("Enter room name : ");
            char name6[2 << 8];
            scanf("%s", name6);
            printf("Combien de temps la loop devra durer en seconde : ");
            int durer;
            scanf("%d", &durer);
            printf("Enter a message : ");
            char message2[256];
            getchar();
            fgets(message2, 256, stdin);
            message2[strlen(message2)-1] = '\0';
            int count = 0;
            while(count != durer){
                msg_send(dict_find(rooms, name6, 0), message2, strlen(message2));
                sleep(1);
                count++;
            }
            break;
        case 6://RECEIVELOOP
            printf("Enter room name : ");
            char name7[2 << 8];
            scanf("%s", name7);
            printf("Combien de temps la loop devra durer en seconde : ");
            int durer2;
            scanf("%d", &durer2);
            int count2 = 0;
            while(count2 != durer2){
                char * msg_to_read2;
                msg_to_read2 = (char*)malloc(2<<8);
                msg_receive(dict_find(rooms, name7, 0), msg_to_read2, 2 << 8);
                sleep(1);
                count2++;
            }
            break;
        case 7://WRITEV
            printf("Enter room name : ");
            char name8[2 << 8];
            scanf("%s", name8);
            printf("How many messages : ");
            int nbr;
            scanf("%d", &nbr);

            char ** listMessage = (char**)malloc(sizeof(char*)*nbr);
            for(int i = 0; i < nbr ; i++){
                listMessage[i] = (char*)malloc(sizeof(char)*(2 << 8));
            }
            int tmpp = 0;
            while(tmpp != nbr){
                printf("Enter message %d: ", tmpp+1);
                char message3[256];
                getchar();
                fgets(message3, 256, stdin);
                message3[strlen(message3)-1] = '\0';
                strcpy(listMessage[tmpp],message3);
                tmpp++;
            }
            writev(dict_find(rooms, name8, 0), listMessage, nbr);
            break;
        case 8://READV
            printf("Enter room name : ");
            char name9[2 << 8];
            scanf("%s", name9);
            printf("How many messages : ");
            int nbr2;
            scanf("%d", &nbr2);
            //char ** messagesToRead;
	        readv(dict_find(rooms, name9, 0), nbr2);
            /*for(int j = 0; j < strlen(messagesToRead); j++){
                printf("[%s] received: %s\n", name9, messagesToRead[j]);
            }*/
            break;
        case 9://SUBSCRIBE
            printf("Enter room name : ");
            char name10[2 << 8];
            scanf("%s", name10);
            subscribe(dict_find(rooms, name10, 0), getpid(), SIGUSR1);
            setSignal();
            break;
        case 10:
            printf("Enter room name : ");
            char name11[2 << 8];
            scanf("%s", name11);
            unsubscribe(dict_find(rooms, name11, 0), getpid());
            break;
        default:
            puts("Please type one of the commands listed above !\n");
            break;
    }
}

int main(int argc, char * argv[]){
	(void)argc;
	/* store off program path so we can use it later */
	icky_global_program_name = argv[0];
	set_signal_handler();

	puts("+-------------------------------------------------------+");
    puts("|  ###################################################  |");
    puts("|  # Welcome to Programmation Systeme test interface #  |");
    puts("|  ###################################################  |");
    puts("|                                                       |");
    puts("|+ use the commands listed to test features:            |");
    puts("| -CREATE : to create a new file                        |");
    puts("| -CONNECT : to connect into a file                     |");
    puts("| -DISCONNECT : to disconnect from a file               |");
    puts("| -SEND : to send one message                           |");
    puts("| -RECEIVE: to receive one message                      |");
    puts("| -SENDLOOP : to send a message none stop (loop mode)   |");
    puts("| -RECEIVELOOP : to receive messages loop(listener mode)|");
    puts("| -WRITEV : ecrire plusieures message d'un coup         |");
    puts("| -READV : lit un lot de message                        |");
    puts("| -SUBSCRIBE : s'abonner a une file, recevoir des notifs|");
    puts("| -UNSUBSCRIBE : unsubscribe d'une file                 |");
    puts("+-------------------------------------------------------+");

    rooms = dict_new();

    while(1){
        printf("Sys> ");
        char command[20];
        scanf("%s", command);
        execute_command(command);
    }
  	
  	return 0;
}