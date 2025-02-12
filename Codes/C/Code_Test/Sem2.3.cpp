
#include <iostream>

using namespace std;

int find_maximum(int a[], int n);

int main()
{

    int c, size, location, maximum;
    int array[100];
    printf("Input number of elements in array:\n");

    cin>>size;

    //fflush(stdin);

    cout << "Enter " << size << " integers" << endl;

    //fflush(stdout);

    for(c=0; c<size; c++)
    {
        //cin >> array[i];
        //setbuf(stdin,NULL);

        //fflush(stdin);

        cin >> array[c];

        
    }

    location = find_maximum(array, size);
    maximum = array[location];
    // assign maximum to the array location returned by the find_maximum function
    cout << "Maximum element location = " << (location + 1) << " and value = " << maximum << endl;
    return 0;
}

int find_maximum(int a[], int n)
{
    int c =0;
    int index =0;
    int max = a[0];
    // declare c, max and index as integer types
    // assign max as the first element in array a index = 0;

    for (c = 1; c < n; c++)
    {
        if (a[c] > max)
        {
            // assign index to c
            c = index;
            // assign index to the cth element in array a
            a[c] = index;
        }
    }

    return index;
}
