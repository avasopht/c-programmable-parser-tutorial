// Published under MIT by Keldon Alleyne (github.com/avasopht)
#include "opprec.h"
#include "ioutil.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

struct OpNode {
  int op;
  int value;    /* Valid values start at 1 */
  struct OpNode* parent;
  struct OpNode* left;
  struct OpNode* right;
};

struct OpComparison {
  int character;
  int op_cmp[UCHAR_MAX];
};

struct OpPrecedence {
  /* Lower index = higher precedence */
  struct OpComparison operators[UCHAR_MAX];
  int operator_count;
  int char_idx_map[UCHAR_MAX];
  struct OpNode* root;
};

struct OpNode* read_op_node(struct OpPrecedence* p_precedence, FILE* in);
static bool should_swap(const struct OpPrecedence* precedence, const struct OpNode* op);
static struct OpPrecedence* get_precedence(FILE* in);
static void swap_precedence(struct OpNode* op);
static int OpPrecedence_cmp(const struct OpPrecedence* self, int first, int second);

static void OpPrecedence_add(struct OpPrecedence* self, int operator)
{
  int new_idx = self->operator_count++;
  struct OpComparison* cmp = &self->operators[new_idx];
  self->char_idx_map[operator] = new_idx;
  cmp->character = operator;
  for (int higher_idx = 0; higher_idx<new_idx; ++higher_idx) {
    struct OpComparison* higher = &self->operators[higher_idx];
    higher->op_cmp[operator] = new_idx-higher_idx;
    cmp->op_cmp[higher->character] = higher_idx-new_idx;
  }
}
void print_expression(FILE* out, struct OpNode* op)
{
  assert(op);
  if (op->op!='\0') {
    fprintf(out, "( $%c ", op->op);
    if (op->left) { print_expression(out, op->left); }
    if (op->right) { print_expression(out, op->right); }
    fprintf(out, ") ");
  } else if (op->value>0) {
    fprintf(out, "%%%d ", op->value);
  } else {
    assert(false);
  }
}
void fix_precedence(struct OpPrecedence* precedence, struct OpNode* op)
{
  assert(precedence);
  assert(op);

  while(should_swap(precedence, op)) {
    swap_precedence(op);
    if(!op->parent) {
      precedence->root = op;
    }
  }

  if(op->op && op->right && op->right->op) {
    fix_precedence(precedence, op->right);
  }
}
void OpNode_free(struct OpNode* op)
{
  assert(op);
  if(op->left) { OpNode_free(op->left); }
  if(op->right) { OpNode_free(op->right); }
  op->left = NULL;
  op->right = NULL;
  free(op);
}
int opprec(FILE* in, FILE* out)
{
  (void) out;
  struct OpPrecedence* precedence = get_precedence(in);
  precedence->root = read_op_node(precedence, in);
  if (precedence->root) {
    fix_precedence(precedence, precedence->root);
    print_expression(out, precedence->root);
    OpNode_free(precedence->root);
    precedence->root = NULL;
  }
  free(precedence);
  return 0;
}
struct OpNode* read_op_node(struct OpPrecedence* precedence, FILE* in)
{
  struct OpNode* ptr = calloc(1, sizeof(struct OpNode));
  int next = scan_char_in(in, "(%");
  switch (next) {
  case '(': {
    require_char(in, '$');
    int op_char = scan_char(in);
    assert(op_char!=EOF);
    assert(op_char<=UCHAR_MAX);
    assert(precedence->char_idx_map[op_char]>=0);
    ptr->op = (char) op_char;
    ptr->left = read_op_node(precedence, in);
    ptr->left->parent = ptr;
    ptr->right = read_op_node(precedence, in);
    ptr->right->parent = ptr;
    require_char(in, ')');
    break;
  }
  case '%': {
    ptr->value = scan_int(in);
    break;
  }
  default:assert(false);
  }
  return ptr;
}

struct OpPrecedence* get_precedence(FILE* in)
{
  struct OpPrecedence* precedence = calloc(1, sizeof(struct OpPrecedence));
  for (int i = 0; i<UCHAR_MAX; ++i) { precedence->char_idx_map[i] = -1; }
  require_char(in, '(');
  while (true) {
    int next = scan_char_in(in, "$)");
    switch (next) {
    case '$': {
      int opp = scan_char(in);
      assert(opp!=EOF);
      OpPrecedence_add(precedence, (char) opp);
      break;
    }
    case ')':return precedence;
    default:assert(false);
    }
  }
}
int OpPrecedence_cmp(const struct OpPrecedence* self, int first, int second)
{
  assert(self->char_idx_map[first]>=0);
  int first_idx = self->char_idx_map[first];
  return self->operators[first_idx].op_cmp[second];
}
bool should_swap(const struct OpPrecedence* precedence, const struct OpNode* op) {
  /* Should swap if:
   * 1. This and parent are operators
   * 2. This is the right child of its parent
   * 3. This has lower precedence than its parent
   */
  if(!op->op || !op->parent || !op->op) {
    return false;
  }


  bool is_right_child = op->parent->right == op;
  bool is_lower_than_parent = OpPrecedence_cmp(precedence, op->op, op->parent->op) < 0;
  return is_right_child && is_lower_than_parent;
}
void swap_precedence(struct OpNode* op)
{
  /** This is basically a left rotation. Returns true if node has no parent (is root) */
  struct OpNode* root = op->parent;
  struct OpNode* pivot = root->right;
  assert(pivot == op);
  /* Exchange parents */
  pivot->parent = root->parent;
  root->parent = pivot;
  if(pivot->parent) {
    if(pivot->parent->left == root) {
      pivot->parent->left = pivot;
    } else {
      assert(pivot->parent->right == root);
      pivot->parent->right = pivot;
    }
  }

  root->right = pivot->left;
  pivot->left = root;
}
int opprec_main(int argc, const char** argv)
{
  FILE* in = stdin;
  bool custom_in = false;
  FILE* out = stdout;

  const int FIRST_ARG = 1;
  for (int i = FIRST_ARG; i<argc; ++i) {
    if (0==strcmp("--local-file", argv[i]) && i+1<argc) {
      ++i;
      in = fopen(argv[i], "r");
      assert(in);
      custom_in = true;
    }
  }

  int result = opprec(in, out);

  if (custom_in) {
    fclose(in);
  }
  return result;
}
