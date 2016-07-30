#include "grasp.hpp"

#include "mutationmethods.hpp"

Grasp::Grasp()
{

}

Grasp::~Grasp()
{

}

void Grasp::loadInstance( const std::string &file )
{
    this->prob.readProblem( file );
    if( this->prob.isLoaded )
    {
        this->optr.setUp( &( this->prob ) );
    }
}

void Grasp::loadConfig( const std::string& file )
{
    this->conf = GraspConfig::readConfigFromFile( file );
}

void Grasp::optimize()
{
    if( !this->prob.isLoaded )
    {
        throw std::logic_error( "Problem is not loaded yet!" );
    }


    std::chrono::steady_clock::time_point startTime, endTime;
    this->execStats.execTime = std::chrono::milliseconds( 0 );
    this->execStats.nItersProc = 0;

    while( this->execStats.execTime < this->conf.MAX_EXEC_TIME )
    {
        startTime = std::chrono::steady_clock::now();

        this->optr.run();

        endTime = std::chrono::steady_clock::now();

        if( this->optr.sol > this->execStats.bestSol )
        {
            this->execStats.bestSol = this->optr.sol;
        }

        this->execStats.execTime += std::chrono::duration_cast
                < std::chrono::microseconds >( endTime - startTime );

        int kpIterWithNoImprov = 0;
        TTPSolution bestSolOptr = this->optr.sol;
        while( this->execStats.execTime < this->conf.MAX_EXEC_TIME &&
               kpIterWithNoImprov < this->conf.MAX_KP_ITERS_WITHOUT_IMPROV )
        {
            TTPSolution currSol = bestSolOptr;

            startTime = std::chrono::steady_clock::now();

            currSol.features.pickingPlan = MutationMethods::bitFlip( bestSolOptr.features.pickingPlan,
                                                                     this->conf.ALPHA_PROB );
            this->prob.evaluateSolution( currSol );

            endTime = std::chrono::steady_clock::now();

            if( currSol.perf > bestSolOptr.perf )
            {
                kpIterWithNoImprov = 0;
                bestSolOptr = currSol;
                if( bestSolOptr.perf > this->execStats.bestSol.perf )
                {
                    this->execStats.bestSol = bestSolOptr;
                }
            }
            else
            {
                kpIterWithNoImprov++;
            }

            this->execStats.execTime += std::chrono::duration_cast
                    < std::chrono::microseconds >( endTime - startTime );
        }

        this->execStats.nItersProc++;
    }
}
