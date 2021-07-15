// header C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// header linux
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    // nek gaono inputan atau kurang inputane
    if(argc != 3){
		printf("ikilo formate: %s <port> <baud>\n", argv[0]);
		return 1;
	}

    // set port
    int serial_port = open(argv[1], O_RDWR);

    struct termios tty;

    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }



    // konfigurasi -----------------------------> (have no idea jujur ae)
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); 

    tty.c_lflag = 0;
    tty.c_oflag = 0;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 30;

    int speed = atoi(argv[2]);

    if (speed == 0){
        cfsetospeed (&tty, B0);
        cfsetispeed (&tty, B0);
    }
    else if (speed == 50){
        cfsetospeed (&tty, B50);
        cfsetispeed (&tty, B50);
    }
    else if (speed == 75){
        cfsetospeed (&tty, B75);
        cfsetispeed (&tty, B75);
    }
    else if (speed == 110){
        cfsetospeed (&tty, B110);
        cfsetispeed (&tty, B110);
    }
    else if (speed == 134){
        cfsetospeed (&tty, B134);
        cfsetispeed (&tty, B134);
    }
    else if (speed == 150){
        cfsetospeed (&tty, B150);
        cfsetispeed (&tty, B150);
    }
    else if (speed == 200){
        cfsetospeed (&tty, B200);
        cfsetispeed (&tty, B200);
    }
    else if (speed == 300){
        cfsetospeed (&tty, B300);
        cfsetispeed (&tty, B300);
    }
    else if (speed == 600){
        cfsetospeed (&tty, B600);
        cfsetispeed (&tty, B600);
    }
    else if (speed == 1200){
        cfsetospeed (&tty, B1200);
        cfsetispeed (&tty, B1200);
    }
    else if (speed == 1800){
        cfsetospeed (&tty, B1800);
        cfsetispeed (&tty, B1800);
    }
    else if (speed == 2400){
        cfsetospeed (&tty, B2400);
        cfsetispeed (&tty, B2400);
    }
    else if (speed == 4800){
        cfsetospeed (&tty, B4800);
        cfsetispeed (&tty, B4800);
    }
    else if (speed == 9600){
        cfsetospeed (&tty, B9600);
        cfsetispeed (&tty, B9600);
    }
    else if (speed == 19200){
        cfsetospeed (&tty, B19200);
        cfsetispeed (&tty, B19200);
    }
    else if (speed == 38400){
        cfsetospeed (&tty, B38400);
        cfsetispeed (&tty, B38400);
    }
    else if (speed == 57600){
        cfsetospeed (&tty, B57600);
        cfsetispeed (&tty, B57600);
    }
    else if (speed == 115200){
        cfsetospeed (&tty, B115200);
        cfsetispeed (&tty, B115200);
    }
    else if (speed == 230400){
        cfsetospeed (&tty, B230400);
        cfsetispeed (&tty, B230400);
    }
    else if (speed == 921600){
        cfsetospeed (&tty, B921600);
        cfsetispeed (&tty, B921600);
    }
    else {
        cfsetospeed (&tty, ((speed*B9600)/9600));
        cfsetispeed (&tty, ((speed*B9600)/9600));
    }

    
    
    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }


    // baca data serial (ancer"e kui 0x0D 0x0A)
    char wadah_bacaan[256];
    memset(wadah_bacaan, 0, sizeof(wadah_bacaan));

    int num_byte = read(serial_port, wadah_bacaan, sizeof(wadah_bacaan));

    if (num_byte < 0) {
        printf("Error reading: %s", strerror(errno));
        return 1;
    }

    while(1){
        printf("%s\n", wadah_bacaan);
    }

    close(serial_port);
    return 0;
}