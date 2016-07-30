#ifndef TTPINDIVIDUAL_HPP
#define TTPINDIVIDUAL_HPP

#include <string>

#include "solution.hpp"

#include "ttpsolutionfeatures.hpp"

class TTPSolution : public Solution< TTPIndividualFeatures >
{
public:
    /**
     * @brief TTPIndividual     Constructor.
     */
    TTPSolution();
    /**
     * @brief ~TTPIndividual    Destructor.
     */
    ~TTPSolution();
    /**
     * @brief toString      Convert the individual into a textual
     *                      representation.
     *
     * @return              String that represents the individual.
     */
    std::string toString() const ;
};

#endif // TTPINDIVIDUAL_HPP
