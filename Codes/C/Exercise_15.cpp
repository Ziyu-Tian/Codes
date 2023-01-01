#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
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
int isExist(addressBook *book, string name);
void deletePerson(addressBook *book);
void sortPerson(addressBook *book);
bool cmp_1(Person a, Person b)
{
    if (a.y != b.y)
    {
        return a.y < b.y;
    }
    else
    {
        if (a.m != b.m)
        {
            return a.m < b.m;
        }
        else
        {
            return a.d < b.d;
        }
    }
}
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

void showMenu()
{
    cout << "Input the following number to execute the address book:" << endl;
    cout << "0: Exit the address book" << endl;
    cout << "1: Add the person" << endl;
    cout << "2: Show the person" << endl;
    cout << "3: Delete the person" << endl;
    cout << "4: Sort and show the person" << endl;
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
        cin.get();
        cin.ignore();
        // system("read -n 1 -s -p \"Press any key to continue...\n\"");
        // system("pause") for windows
    }
}

void showPerson(addressBook *book)
{
    if (book->size == 0)
    {
        cout << "No person recorded." << endl;
    }
    else
    {
        for (int i = 0; i < book->size; i++)
        {
            cout << "First Name: " << book->personalInfo[i].firstName << endl;
            cout << "Last Name: " << book->personalInfo[i].lastName << endl;
            cout << "Phone Number: " << book->personalInfo[i].phoneNum << endl;
            cout << "Address: " << book->personalInfo[i].address << endl;
            cout << "Date of Birth: "
                 << book->personalInfo[i].y << "/"
                 << book->personalInfo[i].m << "/"
                 << book->personalInfo[i].d << endl;
            cout << "----------------------------" << endl;
        }
        cin.get();
        //system("read -n 1 -s -p \"Press any key to continue...\n\"");
        // system("pause") for windows
    }
}

int isExist(addressBook *book, string f_name, string l_name)
{
    for (int i = 0; i < book->size; i++)
    {
        if ((book->personalInfo[i].firstName == f_name) && (book->personalInfo[i].lastName == l_name))
        {
            return i;
        }
    }
    return -1;
}

void deletePerson(addressBook *book)
{
    string f_name, l_name;
    cout << "Please input the first name: " << endl;
    cin >> f_name;
    cout << "Please input the last name: " << endl;
    cin >> l_name;
    int value = isExist(book, f_name, l_name);
    if (value != -1)
    {
        for (int i = value; i < book->size; i++)
        {
            book->personalInfo[i] = book->personalInfo[i + 1];
        }
        book->size--;
        cout << "Delete successfully!" << endl;
    }

    else
    {
        cout << " The name undefined!" << endl;
    }

    cin.get();
    //system("read -n 1 -s -p \"Press any key to continue...\n\"");
}

void sortPerson(addressBook *book)
{

    sort(book->personalInfo, book->personalInfo + book->size, cmp_1);
    cout << "Sort finished!" << endl;
    cin.get();
    //system("read -n 1 -s -p \"Press any key to continue...\n\"");
}
