#include <castel/ast/stmt/decl/Variables.hh>
#include <castel/ast/tools/Visitor.hh>
#include <castel/ast/Token.hh>

class REPLVisitor : public castel::ast::tools::Visitor
{

public:

    inline std::list< std::string > const & symbols( void ) const;

public:

    inline void run( castel::ast::Token & token );

public:

    virtual inline void visit( castel::ast::stmt::decl::Variables & variablesDeclarationAst );

protected:

    virtual inline void defaultAction( castel::ast::Token & tokenAst );

private:

    std::list< std::string > mSymbols;

};

std::list < std::string > const & REPLVisitor::symbols( void ) const
{
    return mSymbols;
}

void REPLVisitor::run( castel::ast::Token & token )
{
    token.accept( * this );
}

void REPLVisitor::visit( castel::ast::stmt::decl::Variables & variablesDeclarationAst )
{
    for ( auto const & variable : variablesDeclarationAst.variables( ) )
        mSymbols.push_back( variable.name( ) );

    variablesDeclarationAst.variables( nullptr );
}

void REPLVisitor::defaultAction( castel::ast::Token & )
{
    return ;
}
