#include <llvm/ADT/APFloat.h>
#include <llvm/Constants.h>
#include <p9/ast/expr/Number.hh>

#include "CompilerVisitor.hh"

void CompilerVisitor::visit( p9::ast::expr::Number & number )
{
    mValue.reset( llvm::ConstantFP::get( mContext, llvm::APFloat( number.value( ) ) ) );
}
