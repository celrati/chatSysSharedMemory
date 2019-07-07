#include <stdio.h>
#include <stdlib.h>
#include "../lib/subs.h"

MESSAGE * li = NULL;


void handlerSubscriber(int signum){
	//printf("\033[H\033[J"); // to clear the screen..
	printf("\n new message arrived !! ! \n ");
}


void setSignal(){
	struct sigaction action;
	action.sa_handler = handlerSubscriber;
	sigaction(SIGUSR1, &action, NULL);
}


int main(){

	// pour implementer les signaux
    li =  msg_connect("/hi5", O_RDWR);
    puts("you are connected to the room ");
	subscribe(li,getpid(),SIGUSR1);
	setSignal();

    while(1){
  	
    }


}