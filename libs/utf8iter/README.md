# UTF8 Iterator

This library is an iterator for UTF8 chains, in addition to converting characters from UTF8 to Unicode and vice versa.

https://github.com/adrianwk94/utf8-iterator

## How to use the library?

Using UTF8 Iterator is very easy, it consists of a structure and two functions.

```c
#include "utf-8.h"
#include <stdio.h>

int main() {

    const char* String = "Hello World, こんにちは世界, привет мир.";

    utf8_iter ITER;
    utf8_init(&ITER, String);

    while (utf8_next(&ITER)) {

        printf("Character: %s \t Codepoint: %u\n", utf8_getchar(&ITER), ITER.codepoint);

    }
    return 0;
}
```

**`utf8_iter`** is the structure, and contains important and useful data.

* **`ptr`** is the original pointer to the character string, it is assigned by `utf8_init()`.
* **`codepoint`** is the current character in Unicode.
* **`size`** is the size in bytes of the current character.
* **`position`** is the current position in the string.
* **`next`** is the next position in the string.
* **`count`** is the number of characters currently.
* **`length`** is the length of the string with `strlen()`

**`utf8_init(iter, string)`** is used to start or restart the iterator. The first argument is a pointer to the Iterator, and the second argument is the character string.

**`utf8_initEx(iter, string, length)`** works the same as `utf8_init`, but allows the user to set a maximum length for the string.

**`utf8_next(iter)`** checks the string, the size of the **next** character and converts the character to Unicode. `Return: 1 -> Continue, 0 -> End or Error.`

**`utf8_previous(iter)`** check the string, the size of the **previous** character and converts the character to Unicode. `Return: 1 -> Continue, 0 -> End or Error.`

**`utf8_getchar(iter)`** allows to obtain the character in UTF8 `(char*)` in the Iterator position.

### Other functions

These functions do not require the use of the Iterator:

* **`utf8_len(string)`** returns the number of **unicode** characters in the `string`. It is different from `strlen()`
* **`utf8_nlen(string, end)`** returns the number of **unicode** characters in the `string` to `end`. It is different from `strnlen()`
* **`utf8_to_unicode(char*)`** returns the codepoint in **unicode**.
* **`unicode_to_utf8(codepoint)`** returns the pointer to a string with the character in **UTF8**.

For internal use or advanced users:

* **`utf8_charsize(char*)`** returns the size in bytes of the provided character.
* **`unicode_charsize(codepoint)`** returns the size in bytes that a Unicode character occupies in a UTF8 string.
* **`utf8_converter(char*, size)`** this function converts a UTF8 character to Unicode. This function does not perform the size check. Requires the user to provide the character size.
* **`unicode_converter(codepoint, size)`** this function converts a Unicode character to UTF8. Like `utf8_converter(...)`, it requires you to provide the size of the character.

## Compile Example

To compile in GCC, use the following commands within the library folder:

```
mkdir Build
gcc -Isource/ -Wall main/main.c source/utf-8.c -o build/utf-8

In Windows: build\utf-8.exe
In Mac and Linux: ./build/utf-8
```
