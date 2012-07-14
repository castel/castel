#include <stdexcept>
#include <vector>

#include <llvm/Value.h>
#include <p9/ast/expr/Call.hh>

#include "CompilerVisitor.hh"

void CompilerVisitor::visit( p9::ast::expr::Call & call )
{
    if ( ! call.callee( ) )
        throw std::runtime_error( "Missing callee" );

    llvm::BasicBlock * basicBlock = mBuilder.GetInsertBlock( );
    call.callee( )->accept( *this );
    llvm::Value * callee = mValue.release( );
    mBuilder.SetInsertPoint( basicBlock );

    std::vector< llvm::Value * > arguments;
    for ( auto & item : call.arguments( ) ) {
        if ( ! item.value( ) )
            throw std::runtime_error( "Missing expression" );
        item.value( )->accept( *this );
        arguments.push_back( mValue.release( ) );
    }

    mValue.reset( mBuilder.CreateCall( callee, arguments ) );
}
