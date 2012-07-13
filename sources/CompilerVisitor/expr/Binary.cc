#include <p9/lexer/Type.hh>

#include "CompilerVisitor.hh"

void CompilerVisitor::visit( p9::ast::expr::Binary & binaryExpression )
{
    switch ( binaryExpression.type( ) ) {

    case p9::lexer::T_Add:
        break;

    case p9::lexer::T_Substract:
        break;

    case p9::lexer::T_Multiply:
        break;

    case p9::lexer::T_Divide:
        break;

    case p9::lexer::T_Modulo:
        break;

    }
}
