#include <p9/ast/stmt/Return.hh>

#include "CompilerVisitor.hh"

void CompilerVisitor::visit( p9::ast::stmt::Return & astReturnStatement )
{
    if ( astReturnStatement.returnValue( ) ) {
        astReturnStatement.returnValue( )->accept( *this );
        mBuilder.CreateRet( mValue.release( ) );
    } else {
        mBuilder.CreateRetVoid( );
    }
}
