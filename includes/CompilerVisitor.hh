#pragma once

#include <memory>

#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Value.h>
#include <p9/ast/expr/Binary.hh>
#include <p9/ast/expr/Number.hh>
#include <p9/utils/Visitor.hh>

class CompilerVisitor : public p9::utils::Visitor
{

public:

    CompilerVisitor( void );

public:

    llvm::Value * codegen( p9::ast::Token & token );

public:

    virtual void visit( p9::ast::expr::Binary & );
    virtual void visit( p9::ast::expr::Number & );

private:

    llvm::LLVMContext mContext;
    llvm::IRBuilder< > mBuilder;
    std::auto_ptr< llvm::Value > mValue;

};
