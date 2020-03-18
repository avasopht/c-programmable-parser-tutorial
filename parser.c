/** License is MIT.

    Note: this is a work in progress.

    # How to write a simple yet powerful text configurable parser in C.

    A parser interprets an input and produces a tree representing its
    structure.

    For example, the arithmetic expression $(3 + 2 * 7) using the BODMAS rule
    would be represented with the following s-expression:

    ``` (+ 3 (* 2 7))
    ```

    The above expression states that $(3) is added to the result of $(2 * 7).

    You could achieve this in a variety of ways, such as writing a parser that
    first looked for the "high priority" expressions first (multiplication),
    working its way down the priority chain.

    The rules describing the structure of a document to parse is called a
    *grammar*, and you are going to learn how to create a parser that is
    'programmed' by a grammar you can read from a text file.

    So let's get started.

    ## Describing grammars with lists

    We will describe grammars with s-expressions. Not only are they easy to
    parse, it's easy to extend them to include more complex logic.

    For example, a grammar for arithmetic expressions can be expressed as:

    ``` (:rule 'expression (:option (:sequence 'expression 'operand
                                               'expression)
                                 'integer))
	(:rule 'operand (:option "*" "/" "+" "-"))
        (:rule 'integer (:one-or-more (:character-range "0" "9")))
    ```
    
    Here an 'expression is either a sequence of [ 'expression, 'operand,
    'expression ], or a 'integer.

    An 'operand is either a "*", "/", "+" or "-" character.

    An 'integer is one or more characters from the range of "0"-"9".

 */

#include <stdbool.h>

typedef enum RefType RefType;
typedef struct Ref Ref;
typedef struct List List;
typedef struct ParseResult ParseResult;

/* Parse functions can accept arguments, such as the range of characters
   in :character-range */
typedef ParseResult *(*ParseFn) (const char *text, List *args);

/** Each parser returns NULL on failure or a parse result containing
    a pointer to the text that follows it, and its parse tree.

    This is the s-expression for a parse tree for the character "t":

    `(:character "t")`
 */

enum RefType {
    REF_INT,
    REF_LITERAL,
    REF_LIST,
    REF_PARSER
};

struct Ref {
    RefType type;
    union {
        List *list;
        const char *literal;
        int integer;
        ParseFn parser;
    };
};

/**
 * A Ref instance is used by List to hold its data (which may hold an integer,
 * literal string, list, or a parser).
 */

struct List {
    List *next;
    Ref *data;
};

struct ParseResult {
    const char *remaining;
    List *tree;
};

/**
 * A parse result holds a pointer to the text that follows the portion of
 * text it has parsed.
 *
 * For example, a number parser given "123abc" would return a parse tree
 * for "123", and a pointer to the remaining "abc", allowing parsers to
 * be chained together.
 */

/** These are the key structures we will need for parsing. They are pretty
    self explanatory, but pay special attention to the `data` member in `List`,
    which may contain a `List` pointer, a string literal, or parser function.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool list_data_is_literal (List *l)
{
  assert(l && l->data);
  return l->data->type == REF_LITERAL && l->data->literal;
}

List *make_list ()
{
  List *l = calloc (1, sizeof (List));
  assert(l);
  return l;
}

Ref *make_ref (RefType type)
{
  Ref *ref = calloc (1, sizeof (List));
  assert(ref);
  ref->type = type;
  return ref;
}

Ref *make_literal (const char *literal)
{
  Ref *ref = make_ref (REF_LITERAL);
  ref->literal = literal;
  return ref;
}

bool streq (const char *first, const char *second)
{
  return strcmp (first, second) == 0;
}

ParseResult *make_result ()
{
  ParseResult *result = calloc (1, sizeof (ParseResult));
  return result;
}

ParseResult *parse_single_char (const char *in, List *character_param)
{
  /**
   * parse_single_char demonstrates the simplest of parsers that accepts
   * parses a single character_param.
   *
   * There is a lot of code involved for such a simple process, but most of
   * your parsers are going to be written in s-expressions that can do away
   * with all of the boilerplate.
   */

  assert(list_data_is_literal (character_param));
  assert(!character_param->next);
  assert(strlen (character_param->data->literal) > 0);
  /**
   * These asserts declare that character_param is a string literal with at
   * least one character, such as ("abcdef").
   */

  char ch = character_param->data->literal[0];
  const char *in_ptr = in;
  if (*in_ptr++ != ch)
    {
      return NULL;
    }

  // Return parse tree result (:character "t")
  ParseResult *result = make_result ();
  {
    result->remaining = in_ptr;
    List *tree = make_list ();
    result->tree = tree;

    tree->data = make_literal (":character");
    List *char_list = make_list ();
    char *char_str = calloc (2, sizeof (char));
    char_str[0] = ch;
    char_list->data = make_literal (char_str);
    tree->next = char_list;
  }
  return result;
}

void test_parse_char ()
{
  List *char_args = make_list ();
  char_args->data = make_literal ("t");
  ParseResult *result = parse_single_char ("two", char_args);
  assert(streq (":character", result->tree->data->literal));
  assert(streq ("t", result->tree->next->data->literal));
  assert(streq ("wo", result->remaining));
  printf ("PASSED::test_parse_char\n");
}

int main ()
{
  test_parse_char ();
}
