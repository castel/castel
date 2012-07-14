#include <p9/ast/stmt/Return.hh>

#include "CompilerVisitor.hh"
#include <iostream>

void CompilerVisitor::visit( p9::ast::stmt::Return & astReturn )
{
    if ( astReturn.returnValue( ) ) {
        astReturn.returnValue( )->accept( *this );
        mValue.reset( mBuilder.CreateRet( mValue.release( ) ) );
    } else {
        mValue.reset( mBuilder.CreateRetVoid( ) );
    }
}
