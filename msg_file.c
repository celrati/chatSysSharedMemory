#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "msg_file.h"
#include <sys/mman.h>
#include <stdarg.h>
#include "lib/bit.h" // for the bitwise stuffs..
#include "lib/helper.h" // for the constructors..
#include "data/data_handler.h"
#include "lib/subs.h"

MESSAGE * msg_connect(const char *nom, int options, ...){
    // size_t nb_msg, size_t len_max, variable number of arguments 

    MESSAGE * message = (MESSAGE*)malloc(sizeof(MESSAGE)); // allocate the memory for the MESSAGE ..
    // if the user want to create an anonym FILE
    if(nom == NULL){
        puts("NOTIF 102: try to create/connect to anonym FILE");
        link_message_to_shared_memory_anonym(message);  

        // we have to check if we need to init the header or its already init
        if(message->file_message->header.first == 0  && message->file_message->header.last == 0){
        puts("NOTIF 103: try to init header to anonym FILE");

            va_list valist; // just to loop the list of the var arg
            va_start(valist, options);  

            size_t nb_msg = va_arg(valist, size_t);
            size_t len_max = va_arg(valist, size_t);
            init_header(&(message->file_message->header),len_max,nb_msg);

            strcpy(message->file_message->header.name_room, "");
        }
        message->file_message->header.nbr_process++;
        return message;
    }


    if(is_bit_O_CREAT(options)){  // here we have to create the new FILE..
        // if we have the flag O_CREAT then we have 4 arguments..
        
        va_list valist; // just to loop the list of the var arg
        va_start(valist, options);  

        size_t nb_msg = va_arg(valist, size_t);
        size_t len_max = va_arg(valist, size_t);   

        if(is_bit_O_EXCL(options)){  // to check if we have both flags O_CREAT and O_EXCL           
            // we're gonna check if the shared memory exist, if it exists return NULL else create new FILE
            if(link_message_to_shared_memory_if_it_exist(nom,message,options)){ // this function return 1 or 0
                
                strcpy(message->file_message->header.name_room,nom);
                
                init_header(&(message->file_message->header),len_max,nb_msg);
                message->file_message->header.nbr_process++;
                return message;
            }else{
                return NULL;
            }
        }
        // here we're gonna create a simple FILE..
        link_message_to_shared_memory(nom,message,options);     
        init_header(&(message->file_message->header),len_max,nb_msg);
        message->file_message->header.nbr_process++;
        strcpy(message->file_message->header.name_room,nom);       
    }else{
        // here we have just two arguments, we're gonna connect the process to the existing FILE..
        connect_to_existing_file(nom,message,options);
        message->file_message->header.nbr_process++; 
    }
    return message;
}


int msg_disconnect(MESSAGE *file){
    int tmp = file->file_message->header.nbr_process - 1;
    if(tmp < 0) {
        puts("Erreur msg_disconnect, trying to dc from an empty file !\n");
        return -1;
    }else {
        if(strcmp(file->file_message->header.name_room,"") != 0){
            char name[2 << 8];
            strcpy(name, file->file_message->header.name_room);
            if(tmp == 0){
                int unmap_result = munmap(file->file_message, sizeof(File_message));
                if (unmap_result != 0) {
                    perror("Erreur de munmap");
                    return -1;
                }else{
                    printf("Succesfully disconected from file [%s]\n", name);
                }
                msg_unlink(name);
            }
            if(tmp > 0){
                file->file_message->header.nbr_process--;
                int unmap_result2 = munmap(file->file_message, sizeof(File_message));
                if (unmap_result2 != 0) {
                    perror("Erreur de munmap");
                    file->file_message->header.nbr_process++;
                    return -1;
                }else{
                    printf("Succesfully disconected from file [%s]\n", name);
                }
            }
        }else{
            int unmap_result = munmap(file->file_message, sizeof(File_message));
            if (unmap_result != 0) {
                perror("Erreur de munmap");
                return -1;
            }else{
                printf("Succesfully disconected from the anonym file\n");
            }
            
        }
        return 0;
    }
    return -1;
}


int msg_unlink(const char *nom){
    int res = shm_unlink(nom);
    if(res != 0){
        printf("Error, unlinking file [%s]\n", nom);
        return -1;
    }else {
        printf("The last process just disconnected, file [%s] unlinked successfully\n", nom);
        return 0;
    }
    return -1;
}


int msg_send(MESSAGE *file, const char *msg, size_t len){
    // just for test for now..
    // handle the semaphore later..

    if(len > file->file_message->header.len_max_msg){
        errno = EMSGSIZE;
        return -1;
    }


    if( sem_wait(&(file->file_message->header.mutex_write_acces)) < 0 ){ // seme for write wait
        perror("sem_wait error for WRITE for SEND function"); return -1;
    }

    while(file->file_message->header.number_of_messages_right_now == file->file_message->header.len_max_msg){}  // loop to wait until the queue is not full

     // critical section begin ..
         
        push(file,msg);
        printf(" SEND %s SUCCESFUL\n",msg);
        send_signal(file);
    // critical section end ..


    if( sem_post(&(file->file_message->header.mutex_write_acces)) < 0 ){  // seme for write post
        perror("sem_post error"); return -1;
    }

    if(file->file_message->header.subs_on == 1){
        send_signal(file);
    }

    return 0;
}


int msg_trysend(MESSAGE *file, const char *msg, size_t len){

    if(isQueueFull(file)){
        errno = EAGAIN;
        return -1;
    }

    if(len > file->file_message->header.len_max_msg){
        errno = EMSGSIZE;
        return -1;
    }


    if( sem_wait(&(file->file_message->header.mutex_write_acces)) < 0 ){ // seme for write wait
        perror("sem_wait error for WRITE for SEND function"); return -1;
    }

     // critical section begin ..
         
        push(file,msg);
        printf(" SEND %s SUCCESFUL\n",msg);
        send_signal(file);
    // critical section end ..


    if( sem_post(&(file->file_message->header.mutex_write_acces)) < 0 ){  // seme for write post
        perror("sem_post error"); return -1;
    }    

    return 0;
}


ssize_t msg_receive(MESSAGE *file, char *msg, size_t len){
    // just for test for now..
    // handle the semaphore later..

    if(len < file->file_message->header.len_max_msg){
        errno = EMSGSIZE;
        return -1;
    }

    if( sem_wait(&(file->file_message->header.mutex_read_acces)) < 0 ){ // seme for read wait
        perror("sem_wait error"); return -1;
    }

    while(file->file_message->header.number_of_messages_right_now == 0); // loop to wait until the queue is not empty

    // critical section begin ..

    MSG_BULLE * msg_bulle =  pull(file);
    if(msg_bulle != NULL){
            printf("receive : %s \n",msg_bulle->message_content);
            strcpy(msg,msg_bulle->message_content);
    }    
    // critical section end ..

    if( sem_post(&(file->file_message->header.mutex_read_acces)) < 0 ){  // seme for read post
        perror("sem_post error"); return -1;
    }


    return strlen(msg);
}


ssize_t msg_tryreceive(MESSAGE *file, char *msg, size_t len){
    if(isQueueEmpty(file)){
        errno = EAGAIN;
        return -1;       
    }
    if(len < file->file_message->header.len_max_msg){
        errno = EMSGSIZE;
        return -1;
    }

    if(isQueueEmpty(file)){ // error empty queue
        return -1;
    }

    if( sem_wait(&(file->file_message->header.mutex_read_acces)) < 0 ){ // seme for read wait
        perror("sem_wait error"); return -1;
    }

    

    // critical section begin ..

    MSG_BULLE * msg_bulle =  pull(file);
    if(msg_bulle != NULL){
            printf("receive : %s \n",msg_bulle->message_content);
            strcpy(msg,msg_bulle->message_content);
    }    
    // critical section end ..

    if( sem_post(&(file->file_message->header.mutex_read_acces)) < 0 ){  // seme for read post
        perror("sem_post error"); return -1;
    }


    return strlen(msg);
}


/**
*   helpers fucntions to check the state of the Room..
* 
**/


size_t msg_message_size(MESSAGE *file){
    return file->file_message->header.len_max_msg;
}



size_t msg_capacite(MESSAGE *file){
    return file->file_message->header.capacity;
}



size_t msg_nb(MESSAGE *file){  
    return file->file_message->header.number_of_messages_right_now;
}


size_t msg_nb_left(MESSAGE * file){
    return msg_capacite(file) - msg_nb(file);
}


/**
*   extension 6.1..
*
*/
int writev(MESSAGE * file, char ** list_messages, int how_many_messages){
    if(msg_nb_left(file) < how_many_messages){
        puts("ERROR 403: writev no space LEFT TO insert thoses messages !");
        return -1;
    }
    int i;
    for( i = 0 ; i < how_many_messages ; i++ ){
        //insert each message at the time with push
        push(file,list_messages[i]); // list_messages is an array of strings
    }
    printf("Succes, all messages sent");
    return 0;
}


char ** readv(MESSAGE * file, int how_many_messages){

    char ** ret = (char**)NULL ;
/*
    if(isQueueEmpty(file)){
        puts("ERROR 503: readv The main queue is empty you cant do (pull) ");
        return ret;
    }
*/
    /*
    int how_many_messages_in_the_queue = how_many_messages;

    if(msg_nb(file) < how_many_messages_in_the_queue){
        how_many_messages_in_the_queue = msg_nb(file); 
    }*/

    ret = (char**)malloc(how_many_messages * sizeof(char*) );

    int i;
    for(i = 0 ; i < how_many_messages ; i++){
        //MSG_BULLE * message_get = pull(file);        
        ret[i] = (char*)malloc(2<<8);
        //strcpy(ret[i],message_get->message_content);

        msg_receive(file,ret[i],2 << 8);
        //printf("received : %s \n",ret[i]);
    }

    return ret;
}