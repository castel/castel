#include <llvm/Value.h>
#include <p9/ast/Token.hh>

#include "CompilerVisitor.hh"

CompilerVisitor::CompilerVisitor( void )
: mContext( )
, mBuilder( mContext )
{
}

llvm::Value * CompilerVisitor::codegen( p9::ast::Token & token )
{
    token.accept( *this );
    return mValue.release( );
}
