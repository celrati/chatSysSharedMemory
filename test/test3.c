#include <stdio.h>
#include <stdlib.h>
#include "../lib/subs.h"

int main(){


//send a lot of message writev
	MESSAGE * li =  msg_connect("/hi5", O_CREAT | O_RDWR, SIZE_FILE, 2 << 8);  // 2 messages  of 2^8 bytes
	puts("you are connected to the room ");


	char ** listMessage = (char**)malloc(sizeof(char*)*3);

	for(int i=0; i < 3 ; i++){
		listMessage[i] = (char*)malloc(sizeof(char)*64);
	}

	strcpy(listMessage[0],"hi");
	strcpy(listMessage[1],"hi2");
	strcpy(listMessage[2],"hi3");


	writev(li,listMessage,3);



// for receiving with readv

	char ** messagesToRead;
	messagesToRead = readv(li,3);

	printf("%s",messagesToRead[0]);
	printf("%s",messagesToRead[1]);
	printf("%s",messagesToRead[2]);


	return 0;
}