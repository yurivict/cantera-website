#include "cantera/base/Solution.h"
#include "cantera/thermo.h"
#include <iostream>

using namespace Cantera;

// The actual code is put into a function that can be called from the main program.
void simple_demo()
{
    // Create a new Solution object
    auto sol = newSolution("h2o2.yaml", "ohmech", "None");
    auto gas = sol->thermo();

    // Set the thermodynamic state by specifying T (500 K) P (2 atm) and the mole
    // fractions. Note that the mole fractions do not need to sum to 1.0 - they will
    // be normalized internally. Also, the values for any unspecified species will be
    // set to zero.
    gas->setState_TPX(500.0, 2.0*OneAtm, "H2O:1.0, H2:8.0, AR:1.0");

    // Print a summary report of the state of the gas.
    std::cout << gas->report() << std::endl;
}

// The main program just calls function simple_demo within a 'try' block, and catches
// CanteraError exceptions that might be thrown.
int main()
{
    try {
        simple_demo();
    } catch (CanteraError& err) {
        std::cout << err.what() << std::endl;
    }
}
