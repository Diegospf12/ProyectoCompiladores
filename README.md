# Proyecto de compiladores

# Integrantes

| <a href="https://github.com/jmac-94" target="_blank">**Josué Mauricio Arriaga Colchado**</a> | <a href="https://github.com/Diegospf12" target="_blank">**Diego Pacheco Ferrel**</a> | <a href="https://github.com/NicolasArroyo" target="_blank">**Nicolas Mateo Arroyo Chávez**</a> |
| :----------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------: |
| <img src="https://avatars.githubusercontent.com/u/83974555?v=4" alt="drawing" width="200"/> | <img src="https://avatars.githubusercontent.com/u/94090499?v=4" alt="drawing" width="200"/> | <img src="https://avatars.githubusercontent.com/u/83975293?v=4" alt="drawing" width="200"/> |

## **Introducción**

Este proyecto se enfoc en implementar un compilador con las etapas de Análisis Sintáctico, Semántico, Intérprete y Generación de código para el lenguaje **IMP0** definido por la siguiente sintáxis:

```bnf
Program ::= Body
Body ::= VarDecList StatementList
VarDecList ::= (VarDec)*
VarDec ::= “var” Type VarList “;”
Type ::= id
VarList :: id (“,” id)*
StatementList ::= Stm (“;” Stm)* ...
Stm ::= id “=” Exp |
        “print” “(“ Exp “)” |
        “if” Exp “then” Body [“else” Body] “endif” |
        “while” Exp “do” Body “endwhile”
        “for” id “:” Exp “,” Exp “do” Body “endfor”
Exp ::= BExp
BExp :: CExp ((‘and’|’or’) BExp)?
CExp ::= AExp ((‘<’|’<=’|’==’) AExp)?
AExp ::= Term (( ́+ ́ |  ́- ́) Term)*
Term ::= Factor (( ́* ́| ́/ ́) Factor)*
FExp ::= Unary (“**” FExp”)?
Unary ::= (‘!’|’-‘)? Factor
Factor ::= num |  ́( ́ Exp  ́) ́ | id
           “ifexp”  ́( ́ Exp  ́, ́ Exp  ́, ́ Exp  ́) ́
```

## **Comentarios**

Agregaremos comentarios de una sola línea en cualquier punto del programa. Los comentarios deberán empezar con **//** y acabar con el fin de línea **"\n"**.

Para esto se modificó el **Scanner**, más especificamente el método __Token* Scanner::nextToken()__ en el archivo **imp_parser.cpp**, la modificación fue la siguiente:

```cpp
Token* Scanner::nextToken() {
 ...............
  } else if (strchr("()+-*/;=<,!:", c)) {
    switch(c) {
  ..............   
    case '/':
      c = nextChar();
      if (c == '/') {
        while (c != '\n' && c != '\0') c = nextChar();
        if (c == '\0') token = new Token(Token::END);
        token = nextToken();
      } else {
        rollBack(); 
        token = new Token(Token::DIV); 
      }
      break; 
   .......
  return token;
}
```
