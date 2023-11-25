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

## **1) Comentarios**

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

## **2) Generacuón de código para AND/OR y For loops**

### AND/OR

Para la generación de código de constantes booleanas **(AND/OR)** se modificó el método __int ImpCodeGen::visit(BoolConstExp* e)__ en el archivo **imp_codegen.cpp**, la modificación fue la siguiente:

```cpp
int ImpCodeGen::visit(BoolConstExp* e) {
  codegen(nolabel,"push",e->b ? 1 : 0);
  return 0;
}
```

Además, se añadieron dos cases en el switch del método __int ImpCodeGen::visit(BinaryExp* e)__ en el archivo **imp_codegen.cpp**, la modificación fue la siguiente:

```cpp
int ImpCodeGen::visit(BinaryExp* e) {
  e->left->accept(this);
  e->right->accept(this);
  string op = "";
  switch(e->op) {
  case PLUS: op =  "add"; break;
  case MINUS: op = "sub"; break;
  case MULT:  op = "mul"; break;
  case DIV:  op = "div"; break;
  case LT:  op = "lt"; break;
  case LTEQ: op = "le"; break;
  case EQ:  op = "eq"; break;
  case AND: op = "and"; break;    // Para el codegen de AND
  case OR:  op = "or"; break;     // Para el codegen de OR
  default: cout << "binop " << Exp::binopToString(e->op) << " not implemented" << endl;
  }
  codegen(nolabel, op);
  return 0;
}
```

### FOR Loops

Para la generación de código de For Loops se modificó el método __int ImpCodeGen::visit(ForStatement* s)__ en el archivo **imo_codegen.coo**, la modificación fue la siguiente:

```cpp
int ImpCodeGen::visit(ForStatement* s) {
  string l1 = next_label();
  string l2 = next_label();

  s->e1->accept(this);
  codegen(nolabel, "store", direcciones.lookup(s->id));
  codegen(l1, "skip");
  codegen(nolabel, "load", direcciones.lookup(s->id));
  s->e2->accept(this);
  codegen(nolabel, "le");
  codegen(nolabel, "jmpz", l2);
  s->body->accept(this);
  codegen(nolabel, "load", direcciones.lookup(s->id));
  codegen(nolabel, "push", "1");
  codegen(nolabel, "add");
  codegen(nolabel, "store", direcciones.lookup(s->id));
  codegen(nolabel, "goto", l1);
  codegen(l2, "skip");

  return 0;
}
```

## **Sentencia Do-While**

Para incluir la sentencia **Do-While** en nuestro lenguaje **IMP0** se tuvo que hacer cambios en la sintáxis actual, se añadio una nueva producción a **Stm** la cual es:

```bnf
Stm ::= ... | "do" Body "while" Exp "enddo"?
```





