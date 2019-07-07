#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include "../lib/subs.h"
 
int TO_SEND = 0;

void handler(int signum){
	printf("\033[H\033[J"); // to clear the screen..
	printf("new message arrived !");
	TO_SEND = 1;
}

void setSignal(){
	struct sigaction action;
	action.sa_handler = handler;
	sigaction(SIGUSR1, &action, NULL);
}


int main(){



	MESSAGE * li =  msg_connect("/hi5", O_CREAT | O_RDWR, SIZE_FILE, 2 << 8);  // 2 messages  of 2^8 bytes
	puts("you are connected to the room ");

	while(1){
		msg_send(li,"ohayoo1",7); // send message to the file..
		sleep(1); // wait a little bit..
		msg_send(li,"ohayoo2",7); // send message to the file..
		sleep(1); // wait a little bit..
		msg_send(li,"ohayoo3",7); // send message to the file..
		sleep(1); // wait a little bit..
		msg_send(li,"ohayoo4",7); // send message to the file..

	}
}