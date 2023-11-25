# Proyecto de compiladores

# Integrantes

| <a href="https://github.com/jmac-94" target="_blank">**Josué Mauricio Arriaga Colchado**</a> | <a href="https://github.com/Diegospf12" target="_blank">**Diego Pacheco Ferrel**</a> | <a href="https://github.com/NicolasArroyo" target="_blank">**Nicolas Mateo Arroyo Chávez**</a> |
| :----------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------: |
| <img src="https://avatars.githubusercontent.com/u/83974555?v=4" alt="drawing" width="200"/> | <img src="https://avatars.githubusercontent.com/u/94090499?v=4" alt="drawing" width="200"/> | <img src="https://avatars.githubusercontent.com/u/83975293?v=4" alt="drawing" width="200"/> |

## Introducción

Este proyecto se enfoc en implementar un compilador con las etapas de Análisis Sintáctico, Semántico, Intérprete y Generación de código para el lenguaje definido por la siguiente gramática:

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
