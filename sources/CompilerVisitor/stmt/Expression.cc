#include "CompilerVisitor.hh"

void CompilerVisitor::visit( p9::ast::stmt::Expression & astExpressionStatement )
{
    if ( astExpressionStatement.next( ) ) {
        astExpressionStatement.next( )->accept( *this );
    }
}
