#include "optimizer.hpp"

#include <cstdio> // std::remove()
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#include <paal/dynamic/knapsack_0_1_fptas.hpp>
#include <paal/dynamic/knapsack_0_1.hpp>
#include <paal/greedy/knapsack_0_1_two_app.hpp>

#include "utils.hpp"

std::vector< unsigned long > Optimizer::getLinkernTour(
        std::string lkOutFileNamePath,
        bool delFile,
        const std::string& opts)
{
    std::string command;
    std::vector< unsigned long > tour;

    tour.reserve( this->problem->numCities + 1 );

#ifdef _MSC_VER
    command = "linkern.exe";
#else
    command = "./linkern";
#endif // _MSC_VER
    // Write result in a file;
    command += std::string( " -o " ) + lkOutFileNamePath;
    // Run silently.
    command += " -Q";
    // Add more user options.
    command += " " + opts;
    command += " " + this->problem->probFileNamePath;

    int c_i = std::system( command.c_str() );

    std::ifstream lkOutFile( lkOutFileNamePath );
    std::string tmpStirng;
    if( lkOutFile )
    {
        // Discard the first line.
        std::getline( lkOutFile, tmpStirng );
        for( std::size_t i = 0; i < this->problem->numCities; i++ )
        {
            std::getline( lkOutFile, tmpStirng );
            std::size_t p = tmpStirng.find_first_of( " " );
            tour.push_back( std::stoul( tmpStirng.substr( 0, p ) ) + 1 );  // + 1 because linkern writes the cities with
                                                                           // indexes starting from 0.
        }
        tour.push_back( 1 );
    }
    else
    {
        throw std::invalid_argument( std::string( "Cannot read the linkern output file " ) + lkOutFileNamePath );
    }

    lkOutFile.close();
    if( delFile )
    {
        std::remove( lkOutFileNamePath.c_str() );
    }

    return tour;
}

std::vector< unsigned short > Optimizer::getKP( const KPSolverMethod& method,
                                                const std::vector< Item >& items )
{
    auto w = []( Item i ) -> int { return (int)i.weight; };
    auto p = []( Item i ) -> int { return (int)i.profit; };
    std::vector< Item > paalResult;

    std::vector< Item > itemsCopy( items );

    switch( method )
    {
    case KPSolverMethod::GREEDY:
    {
        paal::knapsack_0_1_two_app(
            itemsCopy,
            this->problem->knapCapacity,
            std::back_inserter( paalResult ),
            p,
            w
        );

        break;
    }
    case KPSolverMethod::DYNAMIC:
    {
        paal::knapsack_0_1(
            itemsCopy,
            (this->problem->knapCapacity),
            std::back_inserter( paalResult ),
            p,
            w
        );

        break;
    }
    case KPSolverMethod::FPTAS:
    {
        double epsilon = 1.0 / 4.0;
        paal::knapsack_0_1_on_value_fptas(
            epsilon,
            itemsCopy,
            this->problem->knapCapacity,
            std::back_inserter( paalResult ),
            w,
            p
        );

        break;
    }
    default:
    {
        return std::vector< unsigned short >( 0 );
    }
    }

    std::vector< unsigned short > res( this->problem->numItems, 0 );
    for( Item& r : paalResult )
    {
        res[ r.index - 1 ] = 1;
    }

    return res;
}

std::vector< Item > Optimizer::reevaluateItems( const std::vector< unsigned long >& tour )
{
    std::vector< Item > newItems( this->problem->items );
    double totalDist = 0.0;

    for( unsigned long i = 0; i < this->problem->numCities - 1; i++ )
    {
        double dist = this->problem->cities[ tour[ i ] - 1 ]
                .euclDistTo( this->problem->cities[ tour[ i + 1 ] - 1 ] );
        totalDist += dist;

        for( unsigned long j = 0; j < this->problem->numItemsPerCity; j++ )
        {
            const unsigned long itemPos =
                    ( this->problem->numItemsPerCity * ( tour[ i + 1 ] - 1 - 1 ) ) + j;
            unsigned long p = newItems[ itemPos ].profit + newItems[ itemPos ].profit / std::sqrt( totalDist );
            newItems[ itemPos ].profit = p;
        }
    }

    return newItems;
}

Optimizer::Optimizer() {}

Optimizer::~Optimizer() {}

void Optimizer::setUp( TTPInstance* p )
{
    this->problem = p;
}

TTPSolution Optimizer::run()
{
    TTPSolution sol;

    std::string lkOutName = std::string( "lk_" ) + this->problem->probFileName + ".tour.temp";
    int opt_I = genIntRandNumber( 0, 4 );
    int opt_K = genIntRandNumber( 0, 3 );
    int opt_s = genIntRandNumber( 0, std::numeric_limits< int >::max() );
    std::string lkOpts = std::string( " -I " ) + std::to_string( opt_I ) +
                                      " -K "   + std::to_string( opt_K ) +
                                      " -s "   + std::to_string( opt_s ) + " ";

    sol.features.tour = getLinkernTour( lkOutName, true, lkOpts );

    sol.features.pickingPlan = getKP( KPSolverMethod::GREEDY, this->reevaluateItems( sol.features.tour ) /*this->problem->items*/ );

    this->problem->evaluateSolution( sol );

    this->sol = sol;
    return this->sol;
}
