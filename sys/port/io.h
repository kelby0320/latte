#ifndef IO_H
#define IO_H

/**
 * @brief Read byte from io port
 * 
 * @param port  Port number
 * @return unsigned char Value read
 */
unsigned char
insb(unsigned short port);

/**
 * @brief Read word from io port
 * 
 * @param port  Port number
 * @return unsigned short   Value read
 */
unsigned short
insw(unsigned short port);

/**
 * @brief Write byte to io port
 * 
 * @param port  Port number
 * @param val   Value to write
 */
void
outb(unsigned short port, unsigned char val);

/**
 * @brief Write word to io port
 * 
 * @param port  Port number
 * @param val   Value to write
 */
void
outw(unsigned short port, unsigned short val);

#endif