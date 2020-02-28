/** Published under MIT license by Keldon Alleyne (github.com/avasopht)

    Note: excuse the differences in code style, this is imported from another project.
 */
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ioutil.h"

int scan_char(FILE* in)
{
  char next;
  if (fscanf(in, " %c ", &next)!=EOF) {
    return next;
  }
  return EOF;
}
bool scan_char_is(FILE* in, char c)
{
  return scan_char(in)==c;
}
void require_char(FILE* in, char c)
{
  if (!scan_char_is(in, c)) {
    fprintf(stderr, "[#%d::Error] Expected '%c'\n", __LINE__, c);
    exit(-1);
  }
}
int scan_char_in(FILE* in, const char* needle)
{
  int next = scan_char(in);
  if (!strchr(needle, next)) {
    fprintf(stderr, "[#%d::Error] Expected character in \"%s\", got %c instead.\n", __LINE__, needle, next);
    exit(-1);
  }
  return next;
}
int scan_int(FILE* in)
{
  int value;
  assert(EOF!=fscanf(in, " %d ", &value)); // NOLINT(cert-err34-c)
  return value;
}
