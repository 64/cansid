# CANSID (C ANSI Driver)

This repository contains a simple C ANSI escape sequence parser. It is intended for use in my own hobby operating system, but can be adapted for different uses.

## Usage

First, call the function `cansid_init(void)` which returns a `struct cansid_state`:
```c
struct cansid_state state = cansid_init();
```
Then, whenever you receive a char that you want to run through the parser, hand it to `cansid_process(struct cansid_state *, char)`. This returns a `struct color_char`.
```c
char c = 'x'; // Whatever you want to parse
struct color_char ch = cansid_process(&state, c);
```
The returned struct indicates how you should print the character. It contains two fields: `style`, and `ascii`. The `style` field is arranged in [this format](http://wiki.osdev.org/Text_UI#Colours). The `ascii` field is simply the character which should be printed. If `ascii` is the NUL byte (i.e `0x00` or `\0`), then the character should not be outputted to the screen (and therefore the `style` field should be ignored too).

## Building

To add CANSID to your repository, simply place the files `cansid.c` and `cansid.h` in the appropriate locations. There are no dependencies and can even be compiled in a freestanding environment.

## Tests

Running tests can be done with `make test`.

## Contributing

Feel free to open an issue or a pull request if you would like to contribute or ask a question.
