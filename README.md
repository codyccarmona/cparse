The program makes the following assumptions about the input:

1. The start symbol is  the first character on  the first line.
2. Productions  start  at  the  beginning  of  each  of  the  following  lines  and  are  of  the  form N->rhs,
   where N is a nonterminal and rhs is a string of nonterminals and terminals. A "|" representing
   alternative rhs’s is not allowed.
3. A nonterminal is a single uppercase letter.
4. A terminal is a single character that is not an uppercase letter, @", or a "’".


The output consists of two parts:

1. The augmented grammar
2. The sets of LR(0) items


The program makes the following assumptions about the output.

1. The position is represented with a "@".
2. The augmented grammar start symbol is represented by a "’".
3. A set of items is represented by the form I#, where # is a number.  Sets are numbered starting
   from  zero.
4. The size of an item is no more than 20 characters (including a NULL character).
