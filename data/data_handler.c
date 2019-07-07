#include "data_handler.h"




void writeMessageAt(const char* chat, MESSAGE * message, int pos){
	int size_message = strlen(chat);
	message->file_message->messages[pos].size_message = size_message;
	message->file_message->messages[pos].pid_user = getpid();
	strcpy(message->file_message->messages[pos].message,chat);

}


void writeMessageAtEnd(const char* chat, MESSAGE * message){
	//int size_message = strlen(chat);
	int last = message->file_message->header.last;

	writeMessageAt(chat,message,last);
	move_index_last_to_right(message);
}



MSG_BULLE *  readMessageAt( MESSAGE * message, int pos){
	int size_message = message->file_message->messages[pos].size_message;
	char * message_content = (char*)malloc(sizeof(char)*size_message + 1);
	strcpy(message_content,message->file_message->messages[pos].message);

	MSG_BULLE * ret = (MSG_BULLE*)malloc(sizeof(MSG_BULLE));

	ret->message_content = message_content;
	ret->message_size = size_message;
	ret->pid_user = message->file_message->messages[pos].pid_user;

	return ret;
}



MSG_BULLE *  readMessageAtEnd( MESSAGE * message){
	int last = message->file_message->header.last;
	return readMessageAt( message,last - 1);
}


void deleteMessageAtFirst(MESSAGE * message){
	move_index_first_to_right(message);
}


void deleteMessageAtEnd(MESSAGE * message){
	move_index_last_to_left(message);
}



void show_all_messages_of_file(MESSAGE * message){
	int start_index = message->file_message->header.first;
	int stop_index = message->file_message->header.last;
	printf("¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ Room : %s ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ \n", message->file_message->header.name_room);
	printf("room capacity : %ld ¤¤¤¤¤\n ",message->file_message->header.capacity);
	printf("room length Max of each message : %ld ¤¤¤¤¤ \n ",message->file_message->header.len_max_msg);

	int total_messages_now = msg_nb(message);
	printf("how many messages are in the room right now  : %d ¤¤¤¤¤ \n ",total_messages_now);

	printf("¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤BEGIN_CHAT¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤ \n");
	for(int i = start_index ; i < stop_index ; i++){
		MSG_BULLE * to_read = readMessageAt(message,i);
		printf("pid { %d } : %s . \n" ,to_read->pid_user,to_read->message_content);
	}
	printf("¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤END_CHAT¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤\n");
}




/**
*    some functions to handle the indexes..
*
**/


void move_index_last_to_right(MESSAGE * message){
	if(message->file_message->header.last == message->file_message->header.len_max_msg - 1 ){
		message->file_message->header.last = 0 ;
	}else{
		message->file_message->header.last += 1;
	}
}

void move_index_last_to_left(MESSAGE * message){
	message->file_message->header.last -= 1;
}

void move_index_first_to_right(MESSAGE * message){
	if(message->file_message->header.first == message->file_message->header.len_max_msg - 1 ){
		message->file_message->header.first = 0 ;
	}else{
		message->file_message->header.first += 1;
	}
}

void move_index_first_to_left(MESSAGE * message){
	message->file_message->header.first -= 1;
}




/**
*    function to handle the FIFO... 
* 
**/

MSG_BULLE * pull(MESSAGE * message){
	
	if(isQueueEmpty(message)){
		puts("ERROR 203: QUEUE IS EMPTY. YOU CANT DO PULL !");
		//return NULL;
	}
	if(message->file_message->header.number_of_messages_right_now == 0){
		return NULL;
	}
	int first_element = message->file_message->header.first;
	deleteMessageAtFirst(message); // just do move cursor first right..
	MSG_BULLE * msg_bulle = readMessageAt(message,first_element);
	message->file_message->header.number_of_messages_right_now--;
	// we have to do the shift later << << << <<
	msync(message->file_message, sizeof(File_message), MS_SYNC);

	return msg_bulle;
}



void push(MESSAGE * message, const char * chat){
	
	if(isQueueFull(message)){
		puts("ERROR 303: QUEUE IS FULL. YOU CANT DO PUSH !");	
		return;
	}
	
	writeMessageAtEnd(chat,message); // push the chat at the end of the queue...
	message->file_message->header.number_of_messages_right_now++;
	msync(message->file_message, sizeof(File_message), MS_SYNC);
	
}


int isQueueFull(MESSAGE * message){
	if( message->file_message->header.last == message->file_message->header.first &&
		message->file_message->header.number_of_messages_right_now != 0
	 ) return 1;
	return 0;
}

int isQueueEmpty(MESSAGE * message){
	if( message->file_message->header.number_of_messages_right_now == 0) return 1;
	return 0;
}




/**
*      function for debugging..
*
**/

void dd1(char ** a,int a1){   // function for printing an array of strings of size a1
	int i;
	for(i = 0 ; i < a1 ; puts(a[i++])); 
}
