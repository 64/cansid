#include <stddef.h>
#include <stdint.h>

#include "cansid.h"

#define ESC '\x1B'

struct cansid_state cansid_init(void) {
	struct cansid_state rv = {
		.state = CANSID_ESC,
		.style = 0x0F
	};
	return rv;
}

struct color_char cansid_process(struct cansid_state *state, char x) {
	struct color_char rv = {
		.style = state->style,
		.ascii = '\0'
	};
	switch (state->state) {
		case CANSID_ESC:
			if (x == ESC)
				state->state = CANSID_BRACKET;
			break;
		case CANSID_BRACKET:
			if (x == '[')
				state->state = CANSID_PARSE;
			break;
		case CANSID_PARSE:
			break;
		default:
			break;
	}
	return rv;
}
