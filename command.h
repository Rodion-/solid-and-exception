#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <memory>
#include <unordered_map>

class ICommand
{
    public:

    virtual std::string type( void )=0;
    virtual void exequte( void ) {}
};

enum class Exception
{
    ArgumentException = 0,
    ConnectionTimeout,
    TYPE2Exception,
    TYPE3Exception,
};


enum class ExceptionStaus
{
    PASSED = 0,
    FAIL
};



class   CommandLog : public ICommand
{
    std::string type_is;

    public:

    CommandLog() : type_is( "Logg"){}
    ~CommandLog(){}

    std::string type() override
    {
        return type_is;
    }

    void exequte( void ) override
    {
        std::cout<< "Logg excequte" << std::endl;
    }
};

class   CommandConnect : public ICommand
{
    std::string type_is;

    public:

    CommandConnect() : type_is( "Connect"){}
    ~CommandConnect(){}

    std::string type() override
    {
        return type_is;
    }

    void exequte( void ) override
    {
        throw Exception::ConnectionTimeout;
    }
};

class   CommandTwo : public ICommand
{
    std::string type_is;

    public:

    CommandTwo() : type_is( "CommandTwo"){}
    ~CommandTwo(){}

    std::string type() override
    {
        return type_is;
    }

    void exequte( void ) override
    {
        throw Exception::ConnectionTimeout;
    }
};
class   CommandOne: public ICommand
{
    std::string type_is;

    public:

    CommandOne() : type_is( "CommandOne"){}
    ~CommandOne(){}

    std::string type() override
    {
        return type_is;
    }

    void exequte( void ) override
    {
        throw Exception::ConnectionTimeout;
    }
};

class ExceptionHandler
{
    using ExHdlrArgScd = std::unordered_map< Exception , void(*)(void)  > ;

    using ExHdlr = std::unordered_map< std::string , std::unique_ptr<ExHdlrArgScd> >;

    ExHdlr handler;

    public:

    ExceptionHandler(){}
    ~ExceptionHandler(){}

    void Setup( std::string cmd_type ,
                Exception ex ,
                void(*hdlr)(void) )
    {

        auto argscd = handler.find( cmd_type );
        if (argscd == handler.end())
        {
            std::unique_ptr<ExHdlrArgScd> ArgScd = std::make_unique<ExHdlrArgScd>();

            ArgScd->insert( { ex , hdlr } );
            handler.insert( { cmd_type , std::move( ArgScd ) } );
        }
    }

    void  Handler( std::unique_ptr<ICommand> cmd , Exception ex )
    {
        auto argscd = handler.find( cmd->type() );
        if (argscd != handler.end())
        {
            auto hdlr = argscd->second->find( ex );

            if( hdlr != argscd->second->end() )
            {
                hdlr->second();
            }
        }
    }
};


#endif // COMMAND_H
