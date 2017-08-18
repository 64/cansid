#include "minunit.h"
#include "cansid.h"

struct cansid_state state;

void test_setup(void) {
	state = cansid_init();
}

MU_TEST(init) {
	mu_assert_int_eq(state.state, CANSID_ESC);
	mu_assert_int_eq(state.style, 0x0F);
}

MU_TEST(state_change) {
	cansid_process(&state, '\x1B');
	mu_assert_int_eq(state.state, CANSID_BRACKET);
	cansid_process(&state, '[');
	mu_assert_int_eq(state.state, CANSID_PARSE);
}

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(init);
}

int main(int argc, char *argv[]) {
	MU_SUITE_CONFIGURE(test_setup, NULL);
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return minunit_fail != 0;
}
