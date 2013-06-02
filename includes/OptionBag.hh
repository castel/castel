#pragma once

#include <string>
#include <vector>

class OptionBag
{

public:

    OptionBag( int argc, char ** argv );

public:

    inline bool checkSyntax( void ) const;

    inline bool emitLLVM( void ) const;

    inline bool help( void ) const;

public:

    inline std::string const & inputFile( void ) const;

    inline std::vector< std::string > const & arguments( void ) const;

public:

    inline std::string const & usageString( void ) const;

private:

    bool mCheckSyntax;

    bool mEmitLLVM;

    bool mHelp;

private:

    std::string mInputFile;

    std::vector< std::string > mArguments;

private:

    std::string mUsageString;

};

bool OptionBag::checkSyntax( void ) const
{
    return mCheckSyntax;
}

bool OptionBag::emitLLVM( void ) const
{
    return mEmitLLVM;
}

bool OptionBag::help( void ) const
{
    return mHelp;
}

std::string const & OptionBag::inputFile( void ) const
{
    return mInputFile;
}

std::vector< std::string > const & OptionBag::arguments( void ) const
{
    return mArguments;
}

std::string const & OptionBag::usageString( void ) const
{
    return mUsageString;
}
