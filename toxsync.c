#include "toxsync.h"

int ROW, COL;
Tox *tox;
char* user_input;
char* lines_on_screen;

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
	unsigned char* binary_string = hex_string_to_bin("B98A2CEAA6C6A2FADC2C3632D284318B60FE5375CCB41EFA081AB67F500C1B0B");
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

void append_user_input(int c) {
	
}

void evaluate_input() {
}

void loop() {
	int keep_running = 1;	
	while(keep_running) {
		tox_do(tox);

		/* Print disconnected/connected in top right */
		char* message;
		int type;
		if (tox_isconnected(tox)) {
			type = 1;
			message = "[ Connected ]";
		} else {
			type = 2;
			message = "[ Disconnected ]";
		}
		attron(COLOR_PAIR(type) | A_BOLD);
		mvprintw(0, COL-strlen(message), "%s", message);
		attroff(COLOR_PAIR(type) | A_BOLD);

		/* Process user input */
		int c = getch();
		if (c == ERR) // nodelay() in main() sets getch() to ERR if no user input was available
			continue;	
		else if (c == 3) // Ctrl-C
			keep_running = 0;	
		else if (isalnum(c) | c == 32 | ispunct(c)) // alphanumeric, space, punctuation
			append_user_input(c);	
		else if (c == 13) // enter/return
			evaluate_input();

		refresh();
		usleep(50000);	
	}
}

int main() {
	initscr();
	raw();
	noecho();
	nodelay(stdscr, TRUE);
	getmaxyx(stdscr, ROW, COL);
	
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	int res = init_tox();

	/* Failed to init tox in some way */
	if (res == -1)
		return -1;

	print_help();
	print_bottom_bar();

	loop();	

	shutdown();

	return 0;
}
