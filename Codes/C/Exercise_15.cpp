#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;
#define MAX 1000
void showMenu();
struct Person
{
    string firstName, lastName, phoneNum, address;
    int y, m, d;
};
struct addressBook
{
    struct Person personalInfo[MAX];
    int size;
};
void addPerson(addressBook *book);
void showPerson(addressBook *book);
void showMenu();
int main()
{
    addressBook book;
    book.size = 0;

    while (true)
    {
        int select = 0;
        showMenu();
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
        default:
            break;
        }
    }
    return 0;
}

void showMenu()
{
    cout << "Input the following number to execute the address book:" << endl;
    cout << "0: Exit the address book" << endl;
    cout << "1: Add the person" << endl;
    cout << "2: Show the person" <<endl;
}

void addPerson(addressBook *book)
{
    if (book->size > MAX || book->size == MAX)
    {
        cout << "Exceed the max number!" << endl;
        return;
    }
    else
    {
        string f_name;
        cout << "Please input your first name:" << endl;
        cin >> f_name;
        book->personalInfo[book->size].firstName = f_name;

        string l_name;
        cout << "Please input your last name:" << endl;
        cin >> l_name;
        book->personalInfo[book->size].lastName = l_name;

        string p_number;
        cout << "Please input your phone number:" << endl;
        cin >> p_number;
        book->personalInfo[book->size].phoneNum = p_number;

        string yourAddress;
        cout << "Please input your address:" << endl;
        cin >> yourAddress;
        book->personalInfo[book->size].address = yourAddress;

        int year, month, date;
        cout << "Please input your date of birth (YY MM DD):" << endl;
        cin >> year >> month >> date;
        book->personalInfo[book->size].y = year;
        book->personalInfo[book->size].m = month;
        book->personalInfo[book->size].d = date;

        book->size++;

        cout << "Add person successfully!" << endl;
        system( "read -n 1 -s -p \"Press any key to continue...\"" );
        //system("pause") for windows
    }
}

void showPerson(addressBook *book)
{
    if (book->size==0)
    {
        cout << "No person recorded." << endl;
    }
    else
    {
        for (int i = 0; i < book->size; i++)
        {
            cout << "First Name: "<< book->personalInfo[i].firstName << endl;
            cout << "Last Name: " << book->personalInfo[i].lastName << endl;
            cout << "Phone Number: " << book->personalInfo[i].phoneNum << endl;
            cout << "Address: " << book->personalInfo[i].address << endl;
            cout << "Date of Birth: "
            << book->personalInfo[i].y << "/"
            << book->personalInfo[i].m << "/"
            << book->personalInfo[i].d << endl;

        }
        system( "read -n 1 -s -p \"Press any key to continue...\n\"" );
        //system("pause") for windows
    }
    
}