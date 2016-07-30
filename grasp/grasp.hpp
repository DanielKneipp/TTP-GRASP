#pragma once

#include <chrono>
#include <string>
#include <limits>

#include "optimizer.hpp"

struct GraspConfig
{
    unsigned MAX_KP_ITERS_WITHOUT_IMPROV;
    std::chrono::microseconds MAX_EXEC_TIME;
    double ALPHA_PROB;
    std::string CONFIG_NAME;

    GraspConfig()
        : MAX_KP_ITERS_WITHOUT_IMPROV( 0 )
        , MAX_EXEC_TIME( 0 )
        , ALPHA_PROB( 0 )
        , CONFIG_NAME( "" )
    {}

    std::string toString() const
    {
        return std::string( "MAX_KP_ITERS_WITHOUT_IMPROV: " ) + std::to_string( this->MAX_KP_ITERS_WITHOUT_IMPROV ) + "\n"
             + std::string( "MAX_EXEC_TIME: " ) + std::to_string( std::chrono::duration_cast< std::chrono::milliseconds >( this->MAX_EXEC_TIME ).count() ) + "\n"
             + std::string( "ALPHA_PROB: " ) + std::to_string( this->ALPHA_PROB ) + "\n"
             + std::string( "CONFIG_NAME: " ) + this->CONFIG_NAME;
    }

    std::string getStringSpecs()
    {
        return std::string( "-t=" ) + std::to_string( std::chrono::duration_cast< std::chrono::milliseconds >( this->MAX_EXEC_TIME ).count() ) +
                            "-i=" + std::to_string( this->MAX_KP_ITERS_WITHOUT_IMPROV ) +
                            "-a=" + std::to_string( this->ALPHA_PROB );
    }

    static GraspConfig readConfigFromFile( const std::string& fileNamePath )
    {
        std::ifstream file( fileNamePath );
        GraspConfig conf;
        std::string tmpString;

        unsigned short count = 0;

        if( file.is_open() )
        {
            file >> tmpString;
            while( !file.eof() )
            {
                if( tmpString == "MAX_KP_ITERS_WITHOUT_IMPROV:" )
                {
                    file >> tmpString;
                    conf.MAX_KP_ITERS_WITHOUT_IMPROV = static_cast< unsigned >( std::stoul( tmpString ) );
                    count++;
                } else if( tmpString == "MAX_EXEC_TIME:" )
                {
                    file >> tmpString;
                    conf.MAX_EXEC_TIME = std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::milliseconds( std::stoul( tmpString ) ) );
                    count++;
                } else if( tmpString == "ALPHA_PROB:" )
                {
                    file >> tmpString;
                    conf.ALPHA_PROB = std::stod( tmpString );
                    count++;
                } else if( tmpString == "CONFIG_NAME:" )
                {
                    file >> tmpString;
                    conf.CONFIG_NAME = tmpString;
                    count++;
                }

                file >> tmpString;
            }
        }
        if( count < 3 )
        {
            throw std::invalid_argument( std::string( "The configuration file " ) + fileNamePath +
                                         " does not have enough parameters to fill TTPGAConfig" );
        }

        return conf;
    }
};

struct ExecStats
{
    std::chrono::microseconds execTime;
    unsigned nItersProc;
    TTPSolution bestSol;

    ExecStats()
        : execTime( 0 )
        , nItersProc( 0 )
    {
        this->bestSol.perf = -std::numeric_limits< double >::infinity();
    }

    std::string toString()
    {
        return std::string( "Execution time: " ) + std::to_string( this->execTime.count() ) + "\n" +
               std::string( "Number of processed iterations: " ) + std::to_string( this->nItersProc ) +
               std::string( "Best solution: " ) + this->bestSol.toString();
    }
};

class Grasp
{
private:
    Optimizer optr;

public:
    TTPInstance prob;
    GraspConfig conf;
    ExecStats execStats;

    Grasp();
    virtual ~Grasp();
    void loadInstance( const std::string& file );
    void loadConfig( const std::string& file );
    void optimize();
};

