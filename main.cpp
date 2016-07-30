#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <cmath>    // std::sqrt()
#include <ctime>    // std::time_t, std::localtime()

#include <grasp/grasp.hpp>
#include <grasp/utils.hpp>

#include <ttp/ttpsolution.hpp>

enum ErrorTypes
{
    NUM_ARGS    = -1,
    EXCEPTION   = -2
};

static std::string getSuffixDateTime()
{
    std::stringstream out;
    std::chrono::time_point< std::chrono::system_clock > now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t( now );
    out << std::put_time( std::localtime( &now_t ), "%Y-%m-%d_%H-%M-%S" );
    return out.str();
}

void printHelp()
{
    std::cout << "Usage: ttpgrasp <TTP-instance-file>"
              << " <GRASP-config-file>"
              << " <number-of-executions>"
              << " [<standard-output-file>]" << std::endl;
}

int main( int argc, char *argv[] )
{
    if( argc < 4 )
    {
        printHelp();
        return ErrorTypes::NUM_ARGS;
    }

    std::streambuf* stcout = std::cout.rdbuf();
    std::ofstream coutFile;

    if( argc >= 5 )
    {
        try
        {
            coutFile = std::ofstream( argv[ 4 ], std::ios::app );
            std::cout.rdbuf( coutFile.rdbuf() );
        }
        catch( std::exception& e )
        {
            std::cerr << e.what() << std::endl;
            return ErrorTypes::EXCEPTION;
        }
    }

    unsigned numExecs = 0;
    try
    {
        numExecs = std::stoi( argv[ 3 ] );
    }
    catch( std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return ErrorTypes::EXCEPTION;
    }

    std::cout << "\n\n=================================" << std::endl
              << "-------- GRASP INITIATED ---------" << std::endl;

    Grasp gSolver;
    gSolver.loadInstance( argv[ 1 ] );
    gSolver.loadConfig( argv[ 2 ] );
    std::vector< TTPSolution > bestSols;
    bestSols.reserve( numExecs );

    std::cout << "Instance: " << gSolver.prob.probFileName << std::endl;
    std::cout << gSolver.conf.toString() << std::endl;

    for( unsigned i = 1; i <= numExecs; i++ )
    {
        std::cout << "\n==================================" << std::endl
                  << "------- EXECUTION " << i << " STARTED ------" << std::endl;

        std::cout << "Date and time: " << getSuffixDateTime() << std::endl;

        try
        {
            gSolver.optimize();

            TTPSolution sol = gSolver.execStats.bestSol;
            bestSols.push_back( sol );
            std::cout << "Best solution: " << sol.toString() << std::endl;

            std::cout << "Total execution time: " << std::chrono::duration_cast< std::chrono::milliseconds >( gSolver.execStats.execTime ).count()
                                                  << " milliseconds" << std::endl;
            std::cout << "Number of processed iterations: " << gSolver.execStats.nItersProc << std::endl;
        }
        catch( std::exception& e )
        {
            std::cerr << e.what() << std::endl;
            return ErrorTypes::EXCEPTION;
        }

        std::cout << "-------  EXECUTION " << i << " DONE  -------" << std::endl
                  << "==================================" << std::endl;
    }

    std::vector< double > perfOfTheBests;
    perfOfTheBests.reserve( bestSols.size() );
    for( TTPSolution& bs : bestSols )
    {
        perfOfTheBests.push_back( bs.perf );
    }

    double sumFits = std::accumulate( std::begin( perfOfTheBests ), std::end( perfOfTheBests ), 0.0 );
    double meanFits = sumFits / perfOfTheBests.size();
    double accum = 0.0;
    std::for_each( std::begin( perfOfTheBests ), std::end( perfOfTheBests ), [ & ]( const double d )
                                                                                   {
                                                                                       accum += ( d - meanFits ) * ( d - meanFits );
                                                                                   });
    double stdevPerfs = std::sqrt( accum / ( perfOfTheBests.size() - 1 ) );

    std::cout << "\nPerformance of the best solutions: " << std::endl;
    for( std::size_t i = 1; i <= bestSols.size(); i++ )
    {
        std::cout << "    Execution " << i << ": " << bestSols[ i - 1 ].perf << std::endl;
    }

    std::cout << "\nPerformance of the best solution of all executions: " << getBestNSols( bestSols, 1 )[ 0 ].perf << std::endl;
    std::cout << "Performance of the worst solution of all executions: " << getWorstNSols( bestSols, 1 )[ 0 ].perf << std::endl;
    std::cout << "Average performance: " << meanFits << std::endl;
    std::cout << "Sample standard deviation: " << stdevPerfs << std::endl;

    std::cout << "\n----------  GRASP DONE  ----------" << std::endl
              << "==================================" << std::endl;

    std::cout.rdbuf( stcout );

    return 0;
}

