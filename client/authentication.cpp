#include "authentication.h"





// reading from the credentials.txt file which contains the username and password for the already registered users

// return the content of the credentials.txt in a form of an unordered_map





unordered_map<string, string> readCredentialsFile(void)

{

    unordered_map<string, string> umap;

    string line;





    ifstream fin("credentials.txt");

    if (!fin.is_open()) {

        cerr << "Error: Unable to open file for reading" << endl;

        exit(1);

    }



    else {

        while (getline(fin, line)) {

            istringstream iss(line);

            string username;

            string password;

            if (iss >> username >> password) {

                umap[username] = password;

            }

        }

    }



    fin.close();



    return umap;

}





void authenticate(unordered_map<string, string> umap)

{

    string user_username;

    string user_password;

    int number = 1;





    // authenticate users based on their entered username and password 

    // ask users to re-enter their wrong username or password



    bool result;

    do

    {

        cout << "Enter the username:" << endl;

        cin >> user_username;



        cout << "Enter the password:" << endl;

        cin >> user_password;





        // Validate user input

        if (umap.find(user_username) == umap.end()) {

            cerr << "Error: User not found\n";

            result = false;

        }

        else

        {

            string actualpassword = umap[user_username];

            if (actualpassword == user_password) {

                result = true;

            }

            else {

                cerr << "Error: Password not correct\n";

                result = false;

            }

        }

    } while (result == false);





}



