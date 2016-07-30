#pragma once

#include <ttp/ttpsolution.hpp>
#include <ttp/ttpinstance.hpp>

enum KPSolverMethod
{
    GREEDY,
    DYNAMIC,
    FPTAS
};

class Optimizer
{
public:
    TTPInstance* problem;
    TTPSolution sol;

    Optimizer();
    virtual ~Optimizer();
    void setUp( TTPInstance* p );
    TTPSolution run();

private:
    std::vector< unsigned short > getKP( const KPSolverMethod& method,
                                         const std::vector< Item >& items );
    std::vector< Item > reevaluateItems( const std::vector< unsigned long >& tour );
    std::vector< unsigned long > getLinkernTour(
            std::string lkOutFileNamePath,
            bool delFile = false,
            const std::string& opts = std::string( "" )
    );
};
