#ifndef CANSID_H
#define CANSID_H

struct cansid_state {
	enum {
		CANSID_ESC,
		CANSID_BRACKET,
		CANSID_PARSE,
	} state;
	unsigned char style;
};

struct color_char {
	unsigned char style;
	unsigned char ascii;
};

struct cansid_state cansid_init(void);
struct color_char cansid_process(struct cansid_state *state, char x);

#endif
