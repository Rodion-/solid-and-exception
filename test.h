#include <gtest/gtest.h>
#include "command.h"
#include <queue>

TEST( test1 , test )
{
    CommandConnect Connect;

    EXPECT_ANY_THROW (  Connect.exequte() );
    EXPECT_THROW( Connect.exequte() , Exception );
}

TEST( test2 , test )
{
    ExceptionHandler exceptionHandler;

    static std::queue<std::unique_ptr<ICommand> > poolcmd;

    static ExceptionStaus estatus = ExceptionStaus :: FAIL;


    exceptionHandler.Setup( std::make_unique<CommandOne>()->type() ,
                            Exception::ConnectionTimeout ,
                            [](){ std::cout<<"Exception One" <<  std::endl; poolcmd.push(std::make_unique<CommandTwo>() ) ; }
                           );

    exceptionHandler.Setup( std::make_unique<CommandTwo>()->type() ,
                            Exception::ConnectionTimeout ,
                            []() { std::cout<<"Exception Two" <<  std::endl;
                            std::unique_ptr<ICommand> Logg = std::make_unique<CommandLog>();
                            Logg->exequte();
                            estatus = ExceptionStaus :: PASSED ;} );

    std::unique_ptr<ICommand> cmd = std::make_unique<CommandOne>();

    try
    {
        cmd->exequte();
    }
    catch ( Exception ex )
    {
        exceptionHandler.Handler( std::move( cmd ) , ex );
    }

    cmd = std::move( poolcmd.front() );

    poolcmd.pop();

    try
    {
        cmd->exequte();
    }
    catch ( Exception ex )
    {
        exceptionHandler.Handler( std::move( cmd ) , ex );
    }

    EXPECT_EQ( estatus , ExceptionStaus :: PASSED );
}

TEST( test3 , test )
{
    ExceptionHandler exceptionHandler;

    static std::queue<std::unique_ptr<ICommand> > poolcmd;

    static ExceptionStaus estatus = ExceptionStaus :: FAIL;

    exceptionHandler.Setup( std::make_unique<CommandConnect>()->type() ,
                            Exception::ConnectionTimeout ,
                            []() { std::cout<<"Exception Connect" <<  std::endl; poolcmd.push( std::make_unique<CommandOne>() ) ;}
                           );

    exceptionHandler.Setup( std::make_unique<CommandOne>()->type() ,
                            Exception::ConnectionTimeout ,
                            [](){ std::cout<<"Exception One" <<  std::endl; poolcmd.push(std::make_unique<CommandTwo>() ) ; }
                           );

    exceptionHandler.Setup( std::make_unique<CommandTwo>()->type() ,
                            Exception::ConnectionTimeout ,
                            []() { std::cout<<"Exception Two" <<  std::endl;
                            std::unique_ptr<ICommand> Logg = std::make_unique<CommandLog>();
                            Logg->exequte();
                            estatus = ExceptionStaus :: PASSED ;} );


    poolcmd.push( std::make_unique<CommandConnect>()  );

    while( poolcmd.size() )
    {
        std::unique_ptr<ICommand>  cmd = std::move( poolcmd.front() );

        poolcmd.pop();

        try
        {

            cmd->exequte();
        }
        catch ( Exception ex )
        {
            exceptionHandler.Handler( std::move( cmd ) , ex );
        }
    }

    EXPECT_EQ( estatus , ExceptionStaus :: PASSED );
}
