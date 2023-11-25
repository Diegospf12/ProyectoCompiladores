# Proyecto de compiladores

# Integrantes

| <a href="https://github.com/jmac-94" target="_blank">**Josué Mauricio Arriaga Colchado**</a> | <a href="https://github.com/Diegospf12" target="_blank">**Diego Sebastián Pacheco Ferrel**</a> | <a href="https://github.com/NicolasArroyo" target="_blank">**Nicolas Mateo Arroyo Chávez**</a> |
| :----------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------: |
| <img src="https://avatars.githubusercontent.com/u/83974555?v=4" alt="drawing" width="200"/> | <img src="https://avatars.githubusercontent.com/u/94090499?v=4" alt="drawing" width="200"/> | <img src="https://avatars.githubusercontent.com/u/83975293?v=4" alt="drawing" width="200"/> |

## **Introducción**

Este proyecto se enfocó en implementar un compilador con las etapas de Análisis Sintáctico, Semántico, Intérprete y Generación de código para el lenguaje **IMP0** definido por la siguiente sintáxis:

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

## **2) Generación de código para AND/OR y For loops**

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

## **3) Sentencia Do-While**

Para incluir la sentencia **Do-While** en nuestro lenguaje **IMP0** se tuvo que hacer cambios en la sintáxis actual, se añadio una nueva producción a **Stm** la cual es:

```bnf
Stm ::= ... | "do" Body "while" Exp "enddo"?
```

### Parser
Ahora para poder hacer su análisis sintáctico se tuvo que modificar el método __Stm* Parser::parseStatement()__ en el archivo **imp_parser.cpp**, la modificación fue la siguiente:

```cpp
Stm* Parser::parseStatement() {
  Stm* s = NULL;
  Exp* e;
  Body *tb, *fb;
  .......
  } else if (match(Token::DO)) {
    tb = parseBody();
    if (!match(Token::WHILE)) {
      cout << "Error: esperaba while" << endl;
      exit(0);
    }
    e = parseExp();
    if (!match(Token::ENDDO)) s = new DoWhileStatement(e,tb);    // "enddo"?
    else s = new DoWhileStatement(e,tb);
  }
  .......
  else {
    cout << "No se encontro Statement" << endl;
    exit(0);
  }
  return s;
}
```

### AST
Se definio la clase DoWhileStatement en el archivo **imp.hh** para poder recorrerlo en el AST:

```cpp
class DoWhileStatement : public Stm {
public:
  Exp* cond;
  Body *body;
  DoWhileStatement(Exp* c, Body* b);
  int accept(ImpVisitor* v);
  void accept(TypeVisitor* v);
  ~DoWhileStatement();
};
```

Finalmente, se implementó el constructor y destructor de la clase y los métodos **accept** en el archivo **imp.cpp**:

```cpp
// Constructor
DoWhileStatement::DoWhileStatement(Exp* c,Body *b):cond(c),body(b) { }

// Destructor
DoWhileStatement::~DoWhileStatement() { delete body; delete cond; }

// accepts
int DoWhileStatement::accept(ImpVisitor* v) {
  return v->visit(this);
}

void DoWhileStatement::accept(TypeVisitor* v) {
  return v->visit(this);
}
```

### Interpreter
El siguiente paso fue implementar el interprete de la sentencia, para eso se definió el método **visit** en el archivo **imp_interpreter.hh**:

```cpp
class ImpInterpreter : public ImpVisitor {
private:
  Environment<int> env;

public:
  int interpret(Program*);
  int visit(Program*);
  int visit(Body*);
  int visit(VarDecList*);
  int visit(VarDec*);
  int visit(StatementList*);
  int visit(AssignStatement*);
  int visit(PrintStatement*);
  int visit(IfStatement*);
  int visit(WhileStatement*);
  int visit(DoWhileStatement*);   // new
  int visit(ForStatement*);
```

Finalmente, se implementó el método en el archivo **imp_interpreter.cpp**

```cpp
int ImpInterpreter::visit(DoWhileStatement* s) {
  do {
    s->body->accept(this);
  } while (s->cond->accept(this));
  return 0;
}
```

### Printer
Para el printer de forma similar al interprete, se añadio un nuevo método en el archivo **imp.printer.hh**:

```cpp
class ImpPrinter : public ImpVisitor {
public:
  void print(Program*);
  int visit(Program*);
  int visit(Body*);
  int visit(VarDecList*);
  int visit(VarDec*);
  int visit(StatementList*);
  int visit(AssignStatement*);
  int visit(PrintStatement*);
  int visit(IfStatement*);
  int visit(WhileStatement*);
  int visit(DoWhileStatement*);   //new
  int visit(ForStatement*);
  
  int visit(BinaryExp* e);
  int visit(UnaryExp* e);
  int visit(NumberExp* e);
  int visit(BoolConstExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
  int visit(CondExp* e);
};
```

Y luego se implementó el método en el archivo **imp_printer.cpp**:

```cpp
int ImpPrinter::visit(DoWhileStatement* s) {
  cout << "do" << endl;
  s->body->accept(this);
  cout << "while (";
  s->cond->accept(this);
  cout << ")";
  return 0;
}
```


### Typecheker
Para el typechecking se uso la siguiente definición:

```bnf
tcheck(env, DoWhileStatement(cond,body))  ifi
        tcheck(env, body) && tcheck(env, cond) = bool
```

Se declaró el método **visit** en los archivos **type_visitor.hh** y **imp_typechecker.hh**:

**type_visitor.hh**
```cpp
class TypeVisitor {
public:
  virtual void visit(Program* p) = 0;
  virtual void visit(Body* b) = 0;
  virtual void visit(VarDecList* e) = 0;
  virtual void visit(VarDec* e) = 0;
  virtual void visit(StatementList* e) = 0;
  virtual void visit(AssignStatement* e) = 0;
  virtual void visit(PrintStatement* e) = 0;
  virtual void visit(IfStatement* e) = 0;
  virtual void visit(WhileStatement* e) = 0;
  virtual void visit(DoWhileStatement* e) = 0;    // new
  virtual void visit(ForStatement* e) = 0;
  
  virtual ImpType visit(BinaryExp* e) = 0;
  virtual ImpType visit(UnaryExp* e) = 0;
  virtual ImpType visit(NumberExp* e) = 0;
  virtual ImpType visit(BoolConstExp* e) = 0;
  virtual ImpType visit(IdExp* e) = 0;
  virtual ImpType visit(ParenthExp* e) = 0;
  virtual ImpType visit(CondExp* e) = 0;
};
```

**imp_typechecker.hh**
```cpp
class ImpTypeChecker : public TypeVisitor {
public:
  ImpTypeChecker();
private:
  Environment<ImpType> env;
  ImpType booltype;
  ImpType inttype;

public:
  void typecheck(Program*);
  void visit(Program*);
  void visit(Body*);
  void visit(VarDecList*);
  void visit(VarDec*);
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(PrintStatement*);
  void visit(IfStatement*);
  void visit(WhileStatement*);
  void visit(DoWhileStatement*);   // new
  void visit(ForStatement*);
  
  ImpType visit(BinaryExp* e);
  ImpType visit(UnaryExp* e);
  ImpType visit(NumberExp* e);
  ImpType visit(BoolConstExp* e);
  ImpType visit(IdExp* e);
  ImpType visit(ParenthExp* e);
  ImpType visit(CondExp* e);
};
```

Y finalmente se implementó el método en el archivo **imp_typechecker.cpp**:

```cpp
void ImpTypeChecker::visit(DoWhileStatement* s) {
  if (!s->cond->accept(this).match(booltype)) {
    cout << "Condicional en DoWhileStm debe de ser: " << booltype << endl;
    exit(0);
  }  
  s->body->accept(this);
 return;
}
```

### Codegen
Para la generación de código se uso la definición:

```bnf
codegen(addr, DoWhileStatement(cond, body)) =
        LENTRY: skip
        codegen(addr, body)
        codegen(addr, cond)
        jmpz LEND
        goto LENTRY
        LEND: skip
```

Se añadio un nuevo método **visit** en los archivos **imp_visitor.hh** y **imp_codegen.hh**:

**imp_visitor.hh**
```cpp
class ImpVisitor {
public:
  virtual int visit(Program* p) = 0;
  virtual int visit(Body* b) = 0;
  virtual int visit(VarDecList* e) = 0;
  virtual int visit(VarDec* e) = 0;
  virtual int visit(StatementList* e) = 0;
  virtual int visit(AssignStatement* e) = 0;
  virtual int visit(PrintStatement* e) = 0;
  virtual int visit(IfStatement* e) = 0;
  virtual int visit(WhileStatement* e) = 0;
  virtual int visit(DoWhileStatement* e) = 0;    // new
  virtual int visit(ForStatement* e) = 0;
  
  virtual int visit(BinaryExp* e) = 0;
  virtual int visit(UnaryExp* e) = 0;
  virtual int visit(NumberExp* e) = 0;
  virtual int visit(BoolConstExp* e) = 0;
  virtual int visit(IdExp* e) = 0;
  virtual int visit(ParenthExp* e) = 0;
  virtual int visit(CondExp* e) = 0;
};
```

**imp_codegen.hh**
```cpp
class ImpCodeGen : public ImpVisitor {
public:
  void codegen(Program*, string outfname);
  int visit(Program*);
  int visit(Body*);
  int visit(VarDecList*);
  int visit(VarDec*);
  int visit(StatementList*);
  int visit(AssignStatement*);
  int visit(PrintStatement*);
  int visit(IfStatement*);
  int visit(WhileStatement*);
  int visit(DoWhileStatement*);    // new
  int visit(ForStatement*);
  
  int visit(BinaryExp* e);
  int visit(UnaryExp* e);
  int visit(NumberExp* e);
  int visit(BoolConstExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
  int visit(CondExp* e);

private:
  std::ostringstream code;
  string nolabel;
  int current_label;
  Environment<int> direcciones;
  int siguiente_direccion, mem_locals;
  void codegen(string label, string instr);
  void codegen(string label, string instr, int arg);
  void codegen(string label, string instr, string jmplabel);
  string next_label();
};
```

Y finalmente, se implementó el método en el archivo **imp_codegen.cpp**:

```cpp
int ImpCodeGen::visit(DoWhileStatement* s) {
  string l1 = next_label();
  string l2 = next_label();

  codegen(l1, "skip");
  s->body->accept(this);
  s->cond->accept(this);
  codegen(nolabel, "jmpz", l2);
  codegen(nolabel, "goto", l1);
  codegen(l2, "skip");

  return 0;
}
```

Con todos estos cambios, la nueva sentencia Do-While es recocida por nuestro Compilador.


