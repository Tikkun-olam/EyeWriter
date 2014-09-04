/*
 * SerialCommunication.cpp
 *
 *  Created on: Jan 26, 2014
 *      Author: isl
 */

#include "SerialCommunication.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

#include <string.h>
SerialCommunication::SerialCommunication(char* serialPort, int baudrate) {
	strcpy(m_port, serialPort);
	m_baudrate = baudrate;
	m_fd = -1;

// m_fd=fd;
}

int SerialCommunication::init() {
	struct termios toptions;

	int fd;

	//fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",

	//        serialport,baud);

	//fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);

	fd = open(m_port, O_RDWR | O_NOCTTY);

	if (fd == -1) {

		perror("init_serialport: Unable to open port ");

		return -1;

	}

	if (tcgetattr(fd, &toptions) < 0) {

		perror("init_serialport: Couldn't get term attributes");

		return -1;

	}

	speed_t brate = m_baudrate; // let you override switch below if needed

	switch (m_baudrate) {

	case 4800:
		brate = B4800;
		break;

	case 9600:
		brate = B9600;
		break;

		// if you want these speeds, uncomment these and set #defines if Linux

		//#ifndef OSNAME_LINUX

		//    case 14400:  brate=B14400;  break;

		//#endif

	case 19200:
		brate = B19200;
		break;

		//#ifndef OSNAME_LINUX

		//    case 28800:  brate=B28800;  break;

		//#endif

		//case 28800:  brate=B28800;  break;

	case 38400:
		brate = B38400;
		break;

	case 57600:
		brate = B57600;
		break;

	case 115200:
		brate = B115200;
		break;

	}

	cfsetispeed(&toptions, brate);

	cfsetospeed(&toptions, brate);

	// 8N1

	toptions.c_cflag &= ~PARENB;

	toptions.c_cflag &= ~CSTOPB;

	toptions.c_cflag &= ~CSIZE;

	toptions.c_cflag |= CS8;

	// no flow control

	toptions.c_cflag &= ~CRTSCTS;

	toptions.c_cflag |= CREAD | CLOCAL; // turn on READ & ignore ctrl lines

	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw

	toptions.c_oflag &= ~OPOST; // make raw

	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html

	toptions.c_cc[VMIN] = 0;

	toptions.c_cc[VTIME] = 20;

	if (tcsetattr(fd, TCSANOW, &toptions) < 0) {

		perror("init_serialport: Couldn't set term attributes");

		return -1;

	}
	m_fd = fd;
	return 0;
}

int SerialCommunication::readUntil(unsigned char* buf, char until) {
	char b[1];

	int i = 0;

	do {

		int n = read(m_fd, b, 1); // read a char at a time

		if (n == -1)
			return -1; // couldn't read

		if (n == 0) {

			usleep(10 * 1000); // wait 10 msec try again

			continue;

		}

		buf[i] = b[0];
		i++;

	} while (b[0] != until);

	buf[i] = 0; // null terminate the string

	return i;

}
int SerialCommunication::readByte(unsigned char* byte) {
	char b[1];
	int n = read(m_fd, b, 1); // read a char at a time
	if (n == -1)
		return -1; // couldn't read
	if (n == 0)
		return 0;
	byte[0] = b[0];
	return 1;
}
int SerialCommunication::writeLen(unsigned char* buf, int len) {
	int n = write(m_fd, buf, len);
	if (n != len)
		return -1;
	return n;
}
int SerialCommunication::writeByte(unsigned char* b) {
	int n = write(m_fd, b, 1);
	if (n != 1)
		return -1;
	return 0;

}
int SerialCommunication::disconnect() {
	if(m_fd == -1)
		return -1;
	close(m_fd);
	return 0;
}
SerialCommunication::~SerialCommunication() {
	disconnect();
}

