/** Published under MIT license by Keldon Alleyne (github.com/avasopht)
 */
#ifndef IOUTIL_H
#define IOUTIL_H

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int scan_char(FILE* in);
int scan_int(FILE* in);
bool scan_char_is(FILE* in, char c);
void require_char(FILE* in, char c);
int scan_char_in(FILE* in, const char* needle);

#ifdef __cplusplus
}
#endif
#endif //IOUTIL_H
