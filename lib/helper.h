#ifndef HELPER_H
#define HELPER_H


#include "../msg_file.h"
#include "bit.h"


void init_header(Header * header, size_t len_max_msg, size_t capacity);

void link_message_to_shared_memory(const char * name,MESSAGE * message,int option);

void connect_to_existing_file(const char * name, MESSAGE * message, int option);

int link_message_to_shared_memory_if_it_exist(const char * name,MESSAGE * message,int option);


void link_message_to_shared_memory_anonym(MESSAGE * message);

#endif