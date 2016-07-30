#include "ttpsolution.hpp"

TTPSolution::TTPSolution() {}

TTPSolution::~TTPSolution() {}

std::string TTPSolution::toString() const
{
    std::string outPut;

    outPut += "{\n";

    outPut += "\"features\" : " + this->features.toString() + ",\n";

    outPut += "\"performance\" : " + std::to_string( this->perf ) + "\n";

    outPut += "}";

    return outPut;
}
