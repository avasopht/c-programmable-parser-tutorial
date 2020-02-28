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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

extern int opprec(FILE* in, FILE* out);
extern int opprec_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif //OPPREC_H
