#pragma once

#include <limits>
#include <random>
#include <cmath>    // std::nextafter()

/** Seed. */
static std::random_device rd;
/** Seed being used to feed the random number generator. */
static std::mt19937 mt = std::mt19937( rd() );

/**
 * @brief genRealRandNumber     Generate a random real number between
 *                              inside the range [min, max].
 *
 * @param min                   Lowest value that can be generated.
 *
 * @param max                   Highest value that can be generated.
 *
 * @return                      The random real number generated.
 */
template< typename T >
T genRealRandNumber( const T min, const T max )
{
    // Note: uniform_real_distribution does [min, max),
    // but we want to do [min, max].
    // Pass the next largest value instead.
    std::uniform_real_distribution< T >dist( min,
                                             std::nextafter( max,
                                                             std::numeric_limits< T >::max() ) );

    return dist( mt );
}

/**
 * @brief genIntRandNumber      Generate a random integer number between
 *                              inside the range [min, max].
 *
 * @param min                   Lowest value that can be generated.
 *
 * @param max                   Highest value that can be generated.
 *
 * @return                      The random integer number generated.
 */
template< typename T >
T genIntRandNumber( const T min, const T max )
{
    // Generate a uniform integer random distribution on [min, max]
    std::uniform_int_distribution< T >dist( min, max );

    return dist( mt );
}

template< class T_ind >
std::vector< T_ind > getBestNSols( const std::vector< T_ind >& sols,
                                          const unsigned numSols )
{
    std::vector< T_ind > partialSortedSols = sols;
    std::partial_sort( partialSortedSols.begin(),
                       partialSortedSols.begin() + numSols,
                       partialSortedSols.end(),
                       []( const T_ind& a, const T_ind& b ) -> bool
                       {
                           return a.perf > b.perf;
                       });

    return std::vector< T_ind >( partialSortedSols.begin(),
                                 partialSortedSols.begin() + numSols );
}

template< class T_ind >
std::vector< T_ind > getWorstNSols( const std::vector< T_ind >& sols,
                                           const unsigned numSols )
{
    std::vector< T_ind > partialSortedSols = sols;
    std::partial_sort( partialSortedSols.begin(),
                       partialSortedSols.begin() + numSols,
                       partialSortedSols.end(),
                       []( const T_ind& a, const T_ind& b ) -> bool
                       {
                           return a.perf < b.perf;
                       });

    return std::vector< T_ind >( partialSortedSols.begin(),
                                 partialSortedSols.begin() + numSols );
}
