#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Value.h>
#include <p9/ast/Token.hh>

#include "CompilerVisitor.hh"

CompilerVisitor::CompilerVisitor( llvm::LLVMContext & context, llvm::IRBuilder< > & builder )
: mContext( context )
, mBuilder( builder )
{
}

llvm::Value * CompilerVisitor::codegen( p9::ast::Token & token )
{
    token.accept( *this );

    return mValue.release( );
}
