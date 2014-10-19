#include "toxsync.h"

int ROW, COL;
Tox *tox;


void print_title() {
	attron(A_BOLD | A_UNDERLINE);
	printw("ToxSync");
	attroff(A_BOLD | A_UNDERLINE);
}

void print_bottom_bar() {
	move(ROW-2, 0);
	for (int i = 0; i < COL; i++)
		addch(ACS_HLINE);
}

void print_help() {
	clear();
	print_title();
	mvprintw(2, 0, "%s", "Commands");
	mvprintw(3, 3, "%s", "/help - prints this message");
	mvprintw(4, 3, "%s", "/id - print your tox id");
	mvprintw(5, 3, "%s", "/add <tox id> - adds tox id to sycn list");
	mvprintw(6, 3, "%s", "/sync - syncs files");
	mvprintw(7, 3, "%s", "/quit - exits toxsync");
	refresh();
}

void save_data() {
	FILE* file = fopen("toxsync_save", "w");

	if (file) {
		size_t size = tox_size(tox);
		uint8_t data[size];
		tox_save(tox, data);

		fwrite(data, sizeof(uint8_t), size, file);
		fclose(file);
	}
}

void load_data() {
	FILE* file = fopen("toxsync_save", "r");

	if (file) {
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		rewind(file);

		uint8_t data[size];

		fread(data, sizeof(uint8_t), size, file);

		tox_load(tox, data, size);

		fclose(file);
	} 
	
	else 
		save_data();
	
}

void _callback_friend_request(Tox* tox, const uint8_t* public_key, const uint8_t* data, uint16_t length, 
								void* userdata) {
}

void _callback_file_data(Tox* tox, int friendnumber, uint8_t filenumber, const uint8_t* data, uint16_t length, 
						void* userdata) {
}

void _callback_file_control(Tox* tox, int friednumber, uint8_t send_receive, uint8_t filenumber, 
							uint8_t control_type, const uint8_t* data, uint16_t length, void* userdata) {
}

void _callback_file_send_request(Tox* tox, int friednumber, uint8_t filenumber, uint64_t filesize, 
						const uint8_t* filename, uint16_t filename_length, void* userdata) {
}

uint8_t* hex_string_to_bin(char* string) {
	size_t i, len = strlen(string) / 2;
	uint8_t* ret = malloc(len);
	char* pos = string;

	for (i = 0; i < len; ++i, pos += 2)
		sscanf(pos, "%2hhx", &ret[i]);

	return ret;
}

/* Allocates new tox structure and bootstraps 
 * Returns 0 on success
 * Returns -1 on failure
 */
int init_tox() {
	Tox_Options options;
	options.ipv6enabled = 1;
	options.udp_disabled = 0;
	options.proxy_enabled = 0;

	tox = tox_new(&options);

	/* Failed creating new tox */
	if (!tox)
		return -1;

	load_data();

	tox_callback_friend_request(tox, _callback_friend_request, NULL);
	tox_callback_file_data(tox, _callback_file_data, NULL);
	tox_callback_file_control(tox, _callback_file_control, NULL);
	tox_callback_file_send_request(tox, _callback_file_send_request, NULL);

	char* address = "37.59.102.176";
	uint16_t port = 33445;
	unsigned char* binary_string = hex_string_to_bin("B98A2CEAA6C6A2FADC2C3632D284318B60FE5375CCB41EFA081AB67F500C1BOB");
	int res = tox_bootstrap_from_address(tox, address, port, binary_string);
	free(binary_string);

	/* Failed bootstrapping */
	if (res != 1)
		return -1;

	return 0;
}

void shutdown() {
	getch();
	save_data();
	tox_kill(tox);
	endwin();
}

int main() {
	initscr();
	getmaxyx(stdscr, ROW, COL);

	int res = init_tox();

	if (res == -1)
		return -1;

	print_help();
	print_bottom_bar();
	refresh();

	shutdown();

	return 0;
}
