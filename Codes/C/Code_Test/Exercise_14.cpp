#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdlib>
using namespace std;
int main()
{
    string s;   // s is the given answer
    char guess; // the character you guess
    string array[100];
    int chances = 6;
    int count = 0;
    unsigned int seed;

    seed = time(0);
    srand(seed);
    ifstream dictionary;
    dictionary.open("dictionary.txt");// the words in game are from the dictionary.txt
    if (!dictionary)
    {
        cout << "Cannot open the file!" << endl;
    }
    else
    {
        int i = 0;
        while (dictionary >> s)// use >> to input the word
        {
            array[i] = s;
            i++;
        }
        dictionary.close();// close the file
    }
    int array_index = rand()%50+1; // the dictionary have 50 words
    string word = array[array_index]; // randomly choose one word
    // choose a word randomly from the dictionary.txt
    //---------------------------------------------------------------

    int size = word.length();// use .length calculate the length
    char word_1[20] = ""; // initialize the char array with empty (which variable length array cannot be)
    while (count != size && chances != 0)
    {
        //count is the correct guess numbers, which max is the size
        //chances is 6 (fixed)
        cout << "Please input the character you guess:" << endl;
        cin >> guess;
        for (int i = 0; i < size; i++)
        {
            if (guess == word[i])
            {
                count++;
                word_1[i] = guess;
                
                for (int j = i + 1; j < size; j++)
                {
                    if (guess == word[j])
                    {
                        count++;

                        word_1[j] = guess;
                        
                    }
                    else
                    {
                        if (!isalpha(word_1[j]))
                        {
                            word_1[j] = '_';
                        }

                        
                    }
                }
                for (int i = 0; i < size; i++)
                {
                    cout << word_1[i];
                }
                cout << endl;
                break;
            }
            else
            {
                if (!isalpha(word_1[i]))
                {
                    word_1[i] = '_';
                }
               
                if (i == size - 1)
                {
                    chances--;
                    cout<<"Wrong answer! The left chance: "<<chances<<endl;
                    switch (chances) // using switch() to justify the
                    // step of hanging man
                    {
                    case 5:
                        cout << "  0" << endl;
                        break;
                    case 4:
                        cout << "  0" << endl << '/' << endl;
                        break;
                    case 3:
                        cout << "  0" << endl << '/' << "[ ]" << endl;
                        break;
                    case 2:
                        cout << "  0" << endl << '/' << "[ ]" << "\\" << endl;
                        break;
                    case 1:
                        cout << "  0" << endl << '/' << "[ ]" << "\\" << '\n'
                        << " /" << endl;
                        break;
                    
                    default:
                        cout << "  0" << endl << '/' << "[ ]" << "\\" << '\n'
                        << " /" << " \\" << endl;
                        break;
                    }
                    
                }
            }
        }
    }
    if (count == size)
    {
        cout << "You Win!" << endl;
    }
    else
    {
        cout << "You Lost!" << endl;
        cout <<"The answer is: "<<word<<endl;
    }

    return 0;
}
