#include <stdexcept>
#include <vector>

#include <llvm/BasicBlock.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Type.h>
#include <p9/ast/expr/Function.hh>

#include "CompilerVisitor.hh"

void CompilerVisitor::visit( p9::ast::expr::Function & function )
{
    if ( ! function.statements( ) )
        throw std::runtime_error( "Missing body" );

    llvm::Type * doubleType = llvm::Type::getDoubleTy(mContext);

    llvm::Type * returnType = doubleType;
    std::vector< llvm::Type * > parametersTypes;

    llvm::FunctionType * functionType = llvm::FunctionType::get( returnType, parametersTypes, false );
    llvm::Function * llvmFunction = llvm::Function::Create( functionType, llvm::Function::ExternalLinkage );

    llvm::BasicBlock * basicBlock = llvm::BasicBlock::Create( mContext, "", llvmFunction );
    mBuilder.SetInsertPoint( basicBlock );

    function.statements( )->accept( *this );

    mValue.reset( llvmFunction );
}
