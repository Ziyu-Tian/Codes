
#include <iostream>
#include <cstdlib>
#include <iomanip>
using namespace std;
// Size of a, b and c matrix sizes
#define NRA 62 /* number of rows in matrix A */
#define NCA 15 /* number of columns in matrix A */
#define NCB 7  /* number of columns in matrix B */
// main function
int main()
{
    // declare variables
    int i, j, k;
    double a[NRA][NCA], /* matrix A to be multiplied */
        b[NCA][NCB],    /* matrix B to be multiplied */
        c[NRA][NCB];    /* result matrix C */
    cout << "Initializing matrices" << endl;
    /*** Initialize matrices ***/
    // initial value of a matrix is a[i][j] = i+j
    for (i = 0; i < NRA; i++)
        for (j = 0; j < NCA; j++)
            a[i][j] = i + j;
    // initial value of b matrix is b[i][j] = i*j
    for (i = 0; i < NCA; i++)
        for (j = 0; j < NCB; j++)
            b[i][j] = i * j;
    // initial value of a matrix is c[i][j] = 0; this matrix will store the output
    for (i = 0; i < NRA; i++)
        for (j = 0; j < NCB; j++)
            c[i][j] = 0;
    /*** Do matrix multiply ***/
    cout << "Starting matrix multiply" << endl;
    for (i = 0; i < NRA; i++)
    {
        for (j = 0; j < NCB; j++)
            for (k = 0; k < NCA; k++)
                c[i][j] += a[i][k] * b[k][j];
    }
    /*** Print Results to Console Output***/
    cout << "******************************************************" << endl;
    cout << "Result Matrix c[][]: " << endl;
    for (i = 0; i < NRA; i++)
    {
        for (j = 0; j < NCB; j++)
        {
            cout << fixed << setprecision(2);
            cout << c[i][j] << "  "; // note the format specifier
        }
        cout << endl;
    }
    cout << "******************************************************" << endl;
    cout << "Done" << endl;
    return 0;
}
