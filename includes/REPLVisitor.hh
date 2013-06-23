#include <list>
#include <string>
#include <utility>

#include <castel/ast/expr/Binary.hh>
#include <castel/ast/expr/Variable.hh>
#include <castel/ast/stmt/decl/Variables.hh>
#include <castel/ast/stmt/Expression.hh>
#include <castel/ast/tools/Hold.hh>
#include <castel/ast/tools/List.hh>
#include <castel/ast/tools/Visitor.hh>
#include <castel/ast/Statement.hh>
#include <castel/ast/Token.hh>

class REPLVisitor : public castel::ast::tools::Visitor
{

public:

    inline std::list< std::string > const & symbols( void ) const;

public:

    inline void run( castel::ast::tools::List< castel::ast::Statement > & statements );

public:

    virtual inline void visit( castel::ast::stmt::decl::Variables & variablesDeclarationAst );

protected:

    virtual inline void defaultAction( castel::ast::Token & tokenAst );

private:

    castel::ast::tools::List< castel::ast::Statement > mStatements;

    castel::ast::tools::List< castel::ast::Statement >::iterator mCurrent;

    std::list< std::string > mSymbols;

};

std::list < std::string > const & REPLVisitor::symbols( void ) const
{
    return mSymbols;
}

void REPLVisitor::run( castel::ast::tools::List< castel::ast::Statement > & statements )
{
    // @todo Is it right to make a temporary move ?
    mStatements = std::move( statements );
    mCurrent = mStatements.begin( );

    for ( ; mCurrent != mStatements.end( ); ++ mCurrent )
        ( * mCurrent )->accept( * this );

    statements = std::move( mStatements );
}

void REPLVisitor::visit( castel::ast::stmt::decl::Variables & variablesDeclarationAst )
{
    castel::ast::tools::List< castel::ast::Statement > replacements;

    for ( auto const & variable : variablesDeclarationAst.variables( ) ) {
        mSymbols.push_back( variable->name( ) );

        if ( variable->initializer( ) ) {
            castel::ast::tools::Hold< castel::ast::expr::Binary > assignment( new castel::ast::expr::Binary( castel::ast::expr::Binary::Operator::Assignment ) );
            assignment->leftOperand( castel::ast::tools::Hold< castel::ast::expr::Variable >( new castel::ast::expr::Variable( variable->name( ) ) ) );
            assignment->rightOperand( std::move( variable->initializer( ) ) );

            castel::ast::tools::Hold< castel::ast::Statement > statement( new castel::ast::stmt::Expression( std::move( assignment ) ) );
            replacements.push_back( std::move( statement ) );
        }
    }


    mStatements.splice( mCurrent, std::move( replacements ) );
    mCurrent = mStatements.erase( mCurrent );
}

void REPLVisitor::defaultAction( castel::ast::Token & )
{
    return ;
}
