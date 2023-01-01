#include <bits/stdc++.h>
using namespace std;
#define MAX 1000
// define the max capacity of the address book

// --------------------------------------------
// structure prototype and definition 
struct Person
// 'Person' contain the information of one person 
{
    string firstName, lastName, phoneNum, address;
    int y, m, d;
};
struct addressBook
// addressBook contain a 'Person' type pointer 'personalInfo' to store the 
// data of each person
{
    struct Person personalInfo[MAX];
    int size;
};
// ---------------------------------------------------

// The prototype of all function used 
void showMenu();
void addPerson(addressBook *book);
void showPerson(addressBook *book);
int isExist(addressBook *book, string name);
void deletePerson(addressBook *book);
void sortPerson(addressBook *book);
bool cmp_1(Person a, Person b);
bool cmp_2(Person a, Person b);
bool cmp_3(Person a, Person b);
bool cmp_4(Person a, Person b);

//----------------------------------------------------

// The compare function needed for STL Qsort

bool cmp_1(Person a, Person b) // compare_rule 1, according to the date of birth
// first year, second month, then date
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

bool cmp_2(Person a, Person b) // compare rule_2, according to the name order
{
    int temp_1 = a.firstName.compare(b.firstName);
    int temp_2 = a.lastName.compare(b.lastName);
    if (temp_1 != 0)
    {
        return temp_1< 0;
    }
    else
    {
        return temp_2 < 0;
    }
    
}

bool cmp_3(Person a, Person b) // compare rule_3, according to address order
{
    int temp = a.address.compare(b.address);
    return temp < 0;
}

bool cmp_4(Person a, Person b) // compare rule_4, according to phone number
{
    int temp = a.phoneNum.compare(b.address);
    return temp < 0;
}

//-------------------------------------------------

void showMenu() // the main menu of instructions
{
    cout << "Input the following number to execute the address book:" << endl;
    cout << "0: Exit the address book" << endl;
    cout << "1: Add the person" << endl;
    cout << "2: Show the person" << endl;
    cout << "3: Delete the person" << endl;
    cout << "4: Sort the person" << endl;
}

void addPerson(addressBook *book) // the function to add person
// the parameter is the address of addressBook
{   
    // Justify whether it is overflow
    if (book->size > MAX || book->size == MAX)
    {
        cout << "Exceed the max number!" << endl;
        return;
    }
    else
    {
        // store the personal info

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

        cout << "Add person successfully! Press ENTER to continue..." << endl;
        cin.get();
        cin.ignore();
        // cin.get() receive a ENTER and cin.ignore() clear the flush
        // these two functions make 'Suspend the program, press ENTER to continue' possible
    }
}

void showPerson(addressBook *book)
// function to show the contents of addressBook
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
        cin.ignore();
    }
}

int isExist(addressBook *book, string f_name, string l_name)
// Judge whether the elements you want to delete exist
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
// the function to delete the info of one person
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
            // move forward the array, then size--, the element shill be deleted
        }
        book->size--;
        cout << "Delete successfully! Press ENTER to continue..." << endl;
    }

    else
    {
        cout << " The name undefined!" << endl;
    }
    
    cin.get();
    cin.ignore();
}

void sortPerson(addressBook *book)
// the function to sort the order of address book
{
    int n;
    // input instructions
    cout << "Sorting by the date of birth type '1'" << endl
         << "Sorting by the name type '2'" << endl
         << "Sorting by the address type '3'" << endl
         << "Sorting by the phone number type '4'" << endl;
    cin >> n;
    // using switch to decide which method used ib sorting
    switch (n)
    {
    case 1:
        sort(book->personalInfo, book->personalInfo + book->size, cmp_1);
        break;
    case 2:
        sort(book->personalInfo,book->personalInfo+book->size, cmp_2);
        break;
    case 3:
        sort(book->personalInfo,book->personalInfo+book->size, cmp_3);
        break;
    case 4:
        sort(book->personalInfo,book->personalInfo+book->size, cmp_4);
        break;
    default:
        cout << "Invalid Input!" << endl;
        break;
    }
    cout << "Sort finished! Press ENTER to continue..." << endl;
    
    cin.get();
    cin.ignore();
}
