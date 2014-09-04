/*
 * SerialCommunication.h
 *
 *  Created on: Jan 26, 2014
 *      Author: isl
 */

#ifndef SERIALCOMMUNICATION_H_
#define SERIALCOMMUNICATION_H_

class SerialCommunication {
public:
	SerialCommunication(char* serialPort, int baudrate);
	int readUntil(unsigned char* buf, char until);
	int readByte(unsigned char* byte);
	int writeLen(unsigned char* buf, int len);
	int writeByte(unsigned char* buf);
	int init();
	int disconnect();
	virtual ~SerialCommunication();
private:
	char  m_port[30];
	int m_baudrate;
	int m_fd;

};

#endif /* SERIALCOMMUNICATION_H_ */
