#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <tox/tox.h>

void print_title();
void print_bottom_bar();
void print_help();
void save_data();
void load_data();

void _callback_friend_request(Tox* tox, const uint8_t* public_key, const uint8_t* data, uint16_t length, 
								void* userdata);

void _callback_file_data(Tox* tox, int friendnumber, uint8_t filenumber, const uint8_t* data, uint16_t length,
							void* userdata);

void _callback_file_control(Tox* tox, int friendnumber, uint8_t send_receive, uint8_t filenumber, 
							uint8_t control_type, const uint8_t* data, uint16_t length, void* userdata);

void _callback_file_send_request(Tox* tox, int friendnumber, uint8_t filenumber, uint64_t filesize,
						const uint8_t* filename, uint16_t filename_length, void* userdata);
