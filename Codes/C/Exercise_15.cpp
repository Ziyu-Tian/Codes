#include <bits/stdc++.h>
#include "addressbook.hpp"
using namespace std;
int main()
{
    addressBook book;
    // define the structure type 'addressBook, named 'book'
    book.size = 0;
    // initialize the size

    while (true)
    {
        int select = 0;
        showMenu();
        // print the menu with instructions
        cin >> select;
        switch (select)
        {
        case 0:
            cout << "Exit Finished!" << endl;
            return 0;
            break;
        case 1:
            addPerson(&book);
            break;
        case 2:
            showPerson(&book);
            break;
        case 3:
            deletePerson(&book);
            break;
        case 4:
            sortPerson(&book);
            break;
        default:
            break;
        }
    }
    return 0;
}