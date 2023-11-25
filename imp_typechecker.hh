#ifndef IMP_TYPECHECKER
#define IMP_TYPECHECKER

#include <unordered_map>

#include "imp.hh"
#include "type_visitor.hh"
#include "environment.hh"

using namespace std;

class ImpTypeChecker : public TypeVisitor {
public:
  ImpTypeChecker();

  bool isInsideLoop() { return loopCounter > 0; }
  void enterLoop() { loopCounter++; }
  void exitLoop() { if(loopCounter > 0) loopCounter--; }

private:
  Environment<ImpType> env;
  ImpType booltype;
  ImpType inttype;
  int loopCounter;

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
  void visit(DoWhileStatement*);
  void visit(ForStatement*);
  void visit(ContinueStatement*);
  void visit(BreakStatement*);
  
  ImpType visit(BinaryExp* e);
  ImpType visit(UnaryExp* e);
  ImpType visit(NumberExp* e);
  ImpType visit(BoolConstExp* e);
  ImpType visit(IdExp* e);
  ImpType visit(ParenthExp* e);
  ImpType visit(CondExp* e);
};


#endif

