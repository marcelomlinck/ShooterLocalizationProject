/*
Author:		Marcelo Melo Linck
University: 	PUCRS (Porto Alegre) / MSU (Bozeman, MT)
Version:	1.0
Description:	This program reads from serial input and uses it as std input for the postproc.c program.
Update: 	27/07/2013
*/

#include <unistd.h>

#include <fcntl.h>

#include <termios.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>


int main(int argc, char **argv) {

    int fd;
    ssize_t len;
    unsigned char buf[8192];
    struct termios tio;
    int nval, aval, tval;

	if(argc > 1) { 
			printf("opening %s…\n", argv[1]);
		    fd = open(argv[1], O_RDONLY | O_NONBLOCK);
		    if(fd < 0) {
		    	perror(NULL);
		    	exit(1);
		    }
	} else { 
		printf("need file to open\n");
		exit(1);
		
	}


    cfmakeraw(&tio);
    cfsetispeed(&tio,B115200);
    cfsetospeed(&tio, B115200);
    tcsetattr(fd,TCSANOW,&tio);

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(fd,&readfds);

	int i;
	int rval;

    do {
     	
     	if(select(fd+1, &readfds, NULL, NULL, NULL)) { 
	
			rval = read(fd, buf, 8192);
			if(rval > 0) 
				write(fileno(stdout), buf, rval);
				
         }	

    } while(1);

    close(fd);
}
