#include "minunit.h"
#include "cansid.h"

#define feed(x) mu_assert_int_eq('\0', cansid_process(&state, x).ascii)
#define yields_self(x) mu_assert_int_eq(x, cansid_process(&state, x).ascii)
#define state(x) mu_assert_int_eq(CANSID_ ## x, state.state)
#define causes_reset(x) do { mu_assert_int_eq(x, cansid_process(&state, x).ascii); state(ESC); mu_assert_int_eq(state.style, state.next_style); } while (0)

struct cansid_state state;

void test_setup(void) {
	state = cansid_init();
}

MU_TEST(init) {
	mu_assert_int_eq(state.style, 0x0F);
	mu_assert_int_eq(state.next_style, state.style);
	state(ESC);
}

MU_TEST(state_change) {
	feed('\x1B'); state(BRACKET);
	feed('['); state(PARSE);
	feed('3'); state(FGCOLOR);
	feed('1'); state(ENDVAL);
	feed(';'); state(PARSE);
	feed('4'); state(BGCOLOR);
	feed('2'); state(ENDVAL);
	feed(';'); state(PARSE);
	feed('='); state(EQUALS);
	feed('1'); state(ENDVAL);
	feed('m'); state(ESC);
}

MU_TEST(fail_parse) {
	feed('\x1B');
	causes_reset(']');

	feed('\x1B');
	feed('[');
	causes_reset('m');

	feed('\x1B');
	feed('[');
	causes_reset(';');

	feed('\x1B');
	feed('[');
	causes_reset('2');

	feed('\x1B');
	feed('[');
	causes_reset('a');

	feed('\x1B');
	feed('[');
	feed('3');
	feed('1');
	causes_reset('1');

	feed('\x1B');
	feed('[');
	feed('3');
	feed('1');
	causes_reset('[');

	feed('\x1B');
	feed('[');
	feed('=');
	causes_reset('=');

	feed('\x1B');
	feed('[');
	feed('=');
	causes_reset(';');
}

MU_TEST(succeed_parse) {
	yields_self('m');
	yields_self('[');
	yields_self(';');
	yields_self('a');

	feed('\x1B');
	feed('[');
	feed('3');
	feed('1');
	feed('m');

	yields_self('m');
	yields_self('[');
	yields_self(';');
	yields_self('a');
}

MU_TEST(fg_colors) {
	const unsigned char lookup_table[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
	for (int i = 0; i < 8; i++) {
		feed('\x1B');
		feed('[');
		feed('3');
		feed(i + '0');
		mu_assert_int_eq(lookup_table[i], state.next_style & 0x07);
		unsigned char style_temp = state.style;
		feed('m');
		mu_assert_int_eq(lookup_table[i], state.style & 0x07);
		mu_assert_int_eq(lookup_table[i], cansid_process(&state, 'm').style & 0x07);
		mu_check(state.style != style_temp);
	}
}

MU_TEST(bg_colors) {
	const unsigned char lookup_table[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
	state.style = 0x10; // Otherwise the mu_check fails on the first time
	for (int i = 0; i < 8; i++) {
		feed('\x1B');
		feed('[');
		feed('4');
		feed(i + '0');
		mu_assert_int_eq(lookup_table[i] << 4, state.next_style & 0xF0);
		unsigned char style_temp = state.style;
		feed('m');
		mu_assert_int_eq(lookup_table[i] << 4, state.style & 0xF0);
		mu_assert_int_eq(lookup_table[i] << 4, cansid_process(&state, 'm').style & 0xF0);
		mu_check(state.style != style_temp);
	}
}

MU_TEST(reset_state) {
	feed('\x1B');
	feed('[');
	feed('3');
	feed('1');
	feed(';');
	feed('4');
	feed('1');
	feed('m');
	mu_assert_int_eq(4 | (1 << 3) | (4 << 4), state.style);
	feed('\x1B');
	feed('[');
	feed('0');
	feed('m');
	mu_assert_int_eq(0x0F, state.style);
}

MU_TEST(bright_bit) {
	state.style = 0x00;
	feed('\x1B');
	feed('[');
	feed('3');
	feed('1');
	feed(';');
	feed('1');
	feed('m');
	mu_assert_int_eq(4 | (1 << 3), state.style);

	feed('\x1B');
	feed('[');
	feed('3');
	feed('1');
	feed(';');
	feed('=');
	feed('1');
	feed('m');
}

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(init);
	MU_RUN_TEST(state_change);
	MU_RUN_TEST(fail_parse);
	MU_RUN_TEST(succeed_parse);
	MU_RUN_TEST(fg_colors);
	MU_RUN_TEST(bg_colors);
	MU_RUN_TEST(reset_state);
}

int main(int argc, char *argv[]) {
	MU_SUITE_CONFIGURE(test_setup, NULL);
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return minunit_fail != 0;
}
