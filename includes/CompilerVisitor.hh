#pragma once

#include <memory>

#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Value.h>
#include <p9/ast/BinaryOperator.hh>
#include <p9/ast/Number.hh>
#include <p9/ast/Visitor.hh>

class CompilerVisitor : public p9::ast::Visitor
{

public:

    CompilerVisitor( void );

public:

    llvm::Value * codegen( p9::ast::Token & token );

public:

    virtual void visit( p9::ast::BinaryOperator & );
    virtual void visit( p9::ast::Number &         );

private:

    llvm::LLVMContext mContext;
    llvm::IRBuilder< > mBuilder;
    std::auto_ptr< llvm::Value > mValue;

};
