#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "../msg_file.h"





void writeMessageAt(const char* chat, MESSAGE * message, int pos);
void writeMessageAtEnd(const char* chat, MESSAGE * message);

MSG_BULLE * readMessageAt( MESSAGE * message, int pos);
MSG_BULLE * readMessageAtEnd( MESSAGE * message);

void deleteMessageAtFirst(MESSAGE * message);
void deleteMessageAtEnd(MESSAGE * message);


void show_all_messages_of_file(MESSAGE * message);


void move_index_last_to_right(MESSAGE * message);
void move_index_last_to_left(MESSAGE * message);

void move_index_first_to_right(MESSAGE * message);
void move_index_first_to_left(MESSAGE * message);

MSG_BULLE * pull(MESSAGE * message); // pull the first element from the queue..
void push(MESSAGE * message, const char * chat); // push the chat message at the end of the queue..


int isQueueFull(MESSAGE * message);
int isQueueEmpty(MESSAGE * message);



/**
*      funcion for debugging..
*
**/

void dd1(char ** a,int a1);

#endif