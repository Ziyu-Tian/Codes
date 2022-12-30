#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;
void inverse(double a[][3], int n); // the prototype of inverse function
int main()
{
    unsigned seed;  // the seed to generate random number stream
    int Line;       // Line is the size of matrix
    seed = time(0); // use the system time as a seed
    srand(seed);    // produce the random number
    cout << "Please input the dimension of the matrix:" << endl;
    cin >> Line;
    double a[Line][3]; // the matrix (less order 3)
    cout << "This is the original matrix:" << endl;

    for (int i = 0; i < Line; i++) // initializing the matrix
    {
        for (int j = 0; j < Line; j++)
        {
            a[i][j] = rand() % 10 + 1; // the size of the random num (1-10, which easy to check)
        }
    }
    for (int i = 0; i < Line; i++) // print the origin matrix
    {
        for (int j = 0; j < Line; j++)
        {
            printf("%.2f ", a[i][j]);
        }
        printf("\n");
    }
    cout << endl;
    cout << "This is the inverse matrix:" << endl;
    inverse(a, Line);              // call the inverse function
    for (int i = 0; i < Line; i++) // print the inverse function
    {
        for (int j = 0; j < Line; j++)
        {
            printf("%5.4f ", a[i][j]); // 5 digits in total and 4 digits after dot
        }
        printf("\n");
    }
    return 0;
}

void inverse(double a[][3], int n) // n is the order of the matrix, a is the two-dimension matrix
{
    if (n == 2) // the condition of 2x2 matrix
    {
        double det = a[0][0] * a[1][1] - a[1][0] * a[0][1]; // the determinant

        double a_1 = a[0][0];
        a[0][0] = (a[1][1] / det);
        a[1][1] = (a_1 / det);
        a[1][0] = (-a[1][0] / det);
        a[0][1] = (-a[0][1] / det);
    }
    else
    {
        if (n == 3) // 3x3 matrix
        {
            double det;
            det = a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) -
                  a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]) +
                  a[2][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
            double I[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // Initialize the identity matrix
            if (det == 0)                                       // no inverse if det = 0
            {
                cout << "No inverse matrix!" << endl;
            }

            else
            {
                double temp_1 = a[0][0];    // use a temp variable to store a[0][0]
                for (int i = 0; i < 3; i++) // Make the a[0][0] position is '1'
                {
                    a[0][i] = a[0][i] / temp_1;
                    I[0][i] = I[0][i] / temp_1;
                }

                double temp_2 = a[1][0];    // temp_2 of a[1][0]
                for (int i = 0; i < 3; i++) // Make the a[1][0] position '1'
                {
                    a[1][i] = a[1][i] - temp_2 * a[0][i];
                    I[1][i] = I[1][i] - temp_2 * I[0][i];
                }

                double temp_3 = a[2][0];
                for (int i = 0; i < 3; i++) // Make the a[2][0] position '0'
                {
                    a[2][i] = a[2][i] - temp_3 * a[0][i];
                    I[2][i] = I[2][i] - temp_3 * I[0][i];
                }

                double m_1[3] = {a[1][0], a[1][1], a[1][2]};
                double m_2[3] = {I[1][0], I[1][1], I[1][2]};
                if (a[1][1] == 0) // Justify if the a[1][1] is 0, if so , exchange the line 2 and 3
                {
                    for (int j = 0; j < 3; j++)
                    {
                        a[1][j] = a[2][j];
                        I[1][j] = I[2][j];
                    }
                    for (int i = 0; i < 3; i++)
                    {
                        a[2][i] = m_1[i];
                        I[2][i] = m_2[i];
                    }
                }
                double temp_4 = a[1][1];
                for (int i = 0; i < 3; i++) // make a[1][1] is '1'
                {
                    a[1][i] = a[1][i] / temp_4;
                    I[1][i] = I[1][i] / temp_4;
                }

                double temp_5 = a[2][1];
                for (int i = 0; i < 3; i++) // Make a[2][1] is '0'
                {
                    a[2][i] = a[2][i] - temp_5 * a[1][i];
                    I[2][i] = I[2][i] - temp_5 * I[1][i];
                }

                double temp_6 = a[2][2];
                for (int i = 0; i < 3; i++) // Make a[2][2] is '1'
                {
                    a[2][i] /= temp_6;
                    I[2][i] /= temp_6;
                }

                double temp_7 = a[1][2];
                for (int i = 0; i < 3; i++) // make a[1][2] is '0'
                {
                    a[1][i] = a[1][i] - temp_7 * a[2][i];
                    I[1][i] = I[1][i] - temp_7 * I[2][i];
                }

                double temp_8 = a[0][2];
                for (int i = 0; i < 3; i++) // make a[0][2] is '0'
                {
                    a[0][i] = a[0][i] - temp_8 * a[2][i];
                    I[0][i] = I[0][i] - temp_8 * I[2][i];
                }

                double temp_9 = a[0][1];
                for (int i = 0; i < 3; i++) // make a[0][1] is '0'
                {
                    a[0][i] = a[0][i] - temp_9 * a[1][i];
                    I[0][i] = I[0][i] - temp_9 * I[1][i];
                }
                for (int i = 0; i < 3; i++) // make sure the '0' position print correct
                {
                    for (int j = 0; j < 3; j++)
                    {
                        if (a[i][j] == 0)
                        {
                            a[i][j] = 0;
                        }
                    }
                }
                for (int i = 0; i < 3; i++) // exchange the identity matrix and origin matrix
                {
                    for (int j = 0; j < 3; j++)
                    {
                        a[i][j] = I[i][j];
                    }
                }
            }
        }
        else

            cout << "Unavailable Input"; // The order larger than 3 will be invalid
    }
}