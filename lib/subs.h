#ifndef SUBSC_H
#define SUBSC_H

#include "../data/data_handler.h"


void subscribe(MESSAGE * file, pid_t pid, int signal_type);
void unsubscribe(MESSAGE * file, pid_t pid);

void send_signal(MESSAGE * file);


#endif