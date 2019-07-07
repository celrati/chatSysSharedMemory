#include "subs.h"


void subscribe(MESSAGE * file, pid_t pid, int signal_type){
	
	if(file->file_message->header.subs_on == 0){
		file->file_message->header.subscriber.pid_process = pid;
		file->file_message->header.subscriber.signal_to_send = signal_type;

		file->file_message->header.subs_on = 1; // set the flag on..	
		msync(file->file_message, sizeof(File_message), MS_SYNC);
		printf("Succesfully subscribed to room [%s]\n", file->file_message->header.name_room);

	}else{
		puts("EROR 602: THERE IS ALREADY A SUBSCRIBER !");
	}
}

void unsubscribe(MESSAGE * file, pid_t pid){
	if(file->file_message->header.subs_on == 1){
		if(file->file_message->header.subscriber.pid_process == pid){
			file->file_message->header.subs_on = 0; // set the flag on..	
			msync(file->file_message, sizeof(File_message), MS_SYNC);
			printf("Succesfully unsubscribed from room [%s]\n", file->file_message->header.name_room);

		}else{	
			puts("EROR 722: THERE IS NO SUBSCRIBER !");
		}
	}else{
		puts("EROR 702: THERE IS NO SUBSCRIBER !");
	}
}

void send_signal(MESSAGE * file){
    if(file->file_message->header.subs_on == 1){
		kill(file->file_message->header.subscriber.pid_process,
                file->file_message->header.subscriber.signal_to_send);   
        unsubscribe(file,file->file_message->header.subscriber.pid_process); 	
    }
    
}