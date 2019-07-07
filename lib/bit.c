
#include "bit.h"

int is_bit_O_CREAT(int option){
	if( option & O_CREAT) return 1;
	return 0;
}
int is_bit_O_EXCL(int option){
	if( option & O_EXCL ) return 1;
	return 0;
}
int is_bit_O_WRONLY(int option){
	if( option & O_WRONLY ) return 1;
	return 0;	
}
int is_bit_O_RDONLY(int option){
	if( option & O_RDONLY ) return 1;
	return 0;		
}
int is_bit_O_RDWR(int option){
	if( option & O_RDWR ) return 1;
	return 0;		
}

