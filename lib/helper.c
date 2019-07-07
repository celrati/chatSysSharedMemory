#include "helper.h"


void init_header(Header * header, size_t len_max_msg, size_t capacity){
	// constructor!!
	header->len_max_msg = len_max_msg;
	header->capacity = capacity;
	header->first = 0; // in the beginning we have the first message is at index 0
	header->last = 0; // in the beginning we have the last message is at index 0
	header->subs_on = 0;
	header->number_of_messages_right_now = 0;
	header->nbr_process = 0;
	strcpy(header->name_room,"INIT_NAME_START");
}

void link_message_to_shared_memory(const char * name, MESSAGE * message,int option){
	int fd;
	
	if(is_bit_O_WRONLY(option)){

		fd = shm_open(name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR );
		puts("READ");
		message->type_file = READ;
	}
	if(is_bit_O_RDONLY(option)){
		fd = shm_open(name, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR );
		puts("WRITE");
		message->type_file = WRITE;
	}
	if(is_bit_O_RDWR(option)){
		fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
		puts("WRITE READ");
		message->type_file = READ_WRITE;

	}

	if(fd<0){
	    perror("shm_open");
	    exit(2);
  	}

  	ftruncate(fd, sizeof(File_message) ); // to fix the size to a MESSAGE struct.. 

  	message->file_message = mmap(NULL, sizeof(File_message), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  	
 	if(message->file_message == MAP_FAILED) {
       perror("erreur de mmap");
       exit(1);
 	 }
 	 //message->file_message->header.name_room = (char*)malloc(64);
 	 strcpy(message->file_message->header.name_room,name);



 	 // init the semaphores..
 	 if ( sem_init(&(message->file_message->header.mutex_write_acces), 1, 1) == -1){
 	 	// semaphore for the write acces..
 	 	perror("SEMEAPHORE WRITE ERROR ");
 	 	exit(0);
 	 }
 	 if ( sem_init(&(message->file_message->header.mutex_read_acces), 1, 1) == -1){
 	 	// semaphore for the write acces..
 	 	perror("SEMEAPHORE READ ERROR ");
 	 	exit(0);
 	 }


}


int link_message_to_shared_memory_if_it_exist(const char * name,MESSAGE * message,int option){
	// it return 0 or 1
	int fd;
	
	if(is_bit_O_WRONLY(option)){

		fd = shm_open(name, O_CREAT |O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR );
		puts("READ");
		if(fd == - 1){
			puts("ERROR 002:  SHARED MEMORY EXIST");
			return 0;
		}
		message->type_file = WRITE;
	}
	if(is_bit_O_RDONLY(option)){
		fd = shm_open(name, O_CREAT |O_EXCL | O_RDONLY, S_IRUSR | S_IWUSR );
		if(fd == - 1){
			puts("ERROR 002:  SHARED MEMORY EXIST");
			return 0;
		}
		puts("WRITE");
		message->type_file = READ;
	}
	if(is_bit_O_RDWR(option)){
		fd = shm_open(name, O_CREAT |O_EXCL | O_RDWR, S_IRUSR | S_IWUSR );
		if(fd == - 1){
			puts("ERROR 002:  SHARED MEMORY EXIST");
			return 0;
		}
		puts("WRITE READ");
		message->type_file = READ_WRITE;

	}

	if(fd<0){
	    perror("shm_open");
	    exit(2);
  	}

  	ftruncate(fd, sizeof(File_message) ); // to fix the size to a MESSAGE struct.. 

  	message->file_message = mmap(NULL, sizeof(File_message), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  	
 	if(message->file_message == MAP_FAILED) {
       perror("erreur de mmap");
       exit(1);
 	 }
 	// message->file_message->header.name_room = (char*)malloc(64);

 	 strcpy(message->file_message->header.name_room,name);
	
 	 // init the semaphores..
 	 if ( sem_init(&(message->file_message->header.mutex_write_acces), 1, 1) == -1){
 	 	// semaphore for the write acces..
 	 	perror("SEMEAPHORE WRITE ERROR ");
 	 	exit(0);
 	 }
 	 if ( sem_init(&(message->file_message->header.mutex_read_acces), 1, 1) == -1){
 	 	// semaphore for the write acces..
 	 	perror("SEMEAPHORE READ ERROR ");
 	 	exit(0);
 	 }

 	 
 	 return 1;
}


void connect_to_existing_file(const char * name, MESSAGE * message, int option){
	int fd;
	
	if(is_bit_O_WRONLY(option)){
		fd = shm_open(name,  O_WRONLY, S_IRUSR | S_IWUSR );

	}
	if(is_bit_O_RDONLY(option)){
		fd = shm_open(name, O_RDONLY, S_IRUSR | S_IWUSR );

	}
	if(is_bit_O_RDWR(option)){
		fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR );

	}

	if(fd<0){
	    perror("shm_open");
	    exit(2);
  	}
	ftruncate(fd, sizeof(File_message) );
	if(is_bit_O_WRONLY(option)){
		message->file_message = mmap(NULL, sizeof(File_message), PROT_WRITE, MAP_SHARED, fd, 0);
		
	}
	if(is_bit_O_RDONLY(option)){
		message->file_message = mmap(NULL, sizeof(File_message),  PROT_READ , MAP_SHARED, fd, 0);

	}
	if(is_bit_O_RDWR(option)){
		message->file_message = mmap(NULL, sizeof(File_message),  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	}

 	if(message->file_message == MAP_FAILED) {
       perror("erreur de mmap");
       exit(1);
 	 }
}



void link_message_to_shared_memory_anonym(MESSAGE * message){

	message->file_message = mmap(NULL,(2<<17), PROT_WRITE | PROT_READ , MAP_ANON | MAP_SHARED, -1, 0);
	

	if( message->file_message == MAP_FAILED) { 
		perror("mmap"); 
		exit(1);
	}

	
	// semaphores init ..
 	if( sem_init(&(message->file_message->header.mutex_write_acces), 1, 1) == -1){
 	 	// semaphore for the write acces..
 	 	perror("SEMEAPHORE WRITE ERROR ");
 	 	exit(0);
 	 }
 	 if ( sem_init(&(message->file_message->header.mutex_read_acces), 1, 1) == -1){
 	 	// semaphore for the write acces..
 	 	perror("SEMEAPHORE READ ERROR ");
 	 	exit(0);
 	 }


}
