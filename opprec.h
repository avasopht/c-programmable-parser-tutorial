/* Usage: opprec [OPTION] [FILE]
 * Applies precedence of operators
 * Example opprec --local-file opprec.in
 *
 * Loading file:
 *   -l --local-file        Loads file from local directory
 *
 * When FILE is empty with no options for loading files, read standard input.
 */

#ifndef OPPREC_H
#define OPPREC_H

#include "externc.h"
EXTERN_C(begin)

#include <stdio.h>

extern int opprec(FILE* in, FILE* out);
extern int opprec_main(int argc, const char** argv);

EXTERN_C(end)
#endif //OPPREC_H
