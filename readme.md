License is MIT

Note: this is a work in progress. **Tutorial so far is in [parser.c](https://github.com/avasopht/c-programmable-parser-tutorial/blob/master/parser.c)**

# How to write a simple yet powerful text configurable parser in C.


A parser interprets an input and produces a tree representing its
structure.

For example, the arithmetic expression $$3 + 2 * 7$$ using the BODMAS rule
would be represented with the following s-expression:

```lisp
(+ 3 (* 2 7))
```

The above expression states that $$3$$ is added to the result of $$2 * 7$$.

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

```lisp
(:rule 'expression (:option (:sequence 'expression 'operand
'expression)
'integer))
(:rule 'operand (:option "*" "/" "+" "-"))
(:rule 'integer (:one-or-more (:character-range "0" "9")))
```

Here an 'expression is either a sequence of [ 'expression, 'operand,
'expression ], or a 'integer.

An 'operand is either a "*", "/", "+" or "-" character.

An 'integer is one or more characters from the range of "0"-"9".
