#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include "include/json.hpp" // Include the JSON header
#include <random>
#include <sstream>

using namespace std;
using json = nlohmann::json;

// ANSI color codes   //Reference stackOverflow website
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";

// Function to print a box with menu options
string getUserName(const long long &phone)
{
    ifstream inputFile("users.json");
    json data;
    string name = "Unknown";

    if (inputFile.is_open())
    {
        inputFile >> data;
        inputFile.close();

        for (const auto &user : data)
        {
            if (user["phoneNumber"] == phone)
            {
                name = user["name"];
                break;
            }
        }
    }

    return name;
}

string generateUUID()
{ // I copied this code from stackoverflow (https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++)
    {
        ss << dis(gen);
    }
    ss << "-";
    for (int i = 0; i < 4; i++)
    {
        ss << dis(gen);
    }
    ss << "-4";
    for (int i = 0; i < 3; i++)
    {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++)
    {
        ss << dis(gen);
    }
    ss << "-";
    for (int i = 0; i < 12; i++)
    {
        ss << dis(gen);
    }
    return ss.str();
}

// Function to print the menu box

void printMenuBox(bool loggedIn, const long long &phone)
{
    const int width = 40;
    const string border = BLUE + string(width, '-') + RESET;

    cout << border << endl;
    cout << BLUE << "|" << RESET << setw(width - 2) << " " << BLUE << "|" << RESET << endl;

    if (loggedIn)
    {
        // Get user info
        string userName = getUserName(phone);
        userName[0] = toupper(userName[0]);

        // Display user info
        cout << BLUE << "| " << BLUE << BOLD << "Welcome, " << userName << RESET;
        cout << setw(width - 2 - (11 + userName.length())) << " " << BLUE << "|" << RESET << endl;
        // Display additional user info like salary if needed
    }

    cout << BLUE << "| " << GREEN << BOLD << "------ Main Menu ------" << RESET << setw(width - 2 - 23) << " " << BLUE << "|" << RESET << endl;
    cout << BLUE << "|" << RESET << setw(width - 2) << " " << BLUE << "|" << RESET << endl;

    if (!loggedIn)
    {
        cout << BLUE << "| " << GREEN << "1. Register User" << RESET << setw(width - 2 - 16) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << GREEN << "2. Login" << RESET << setw(width - 2 - 8) << " " << BLUE << "|" << RESET << endl;
    }
    else
    {
        cout << BLUE << "| " << GREEN << "1. Add Expense" << RESET << setw(width - 2 - 14) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << GREEN << "2. Display All Expenses" << RESET << setw(width - 2 - 23) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << GREEN << "3. Show Analytics" << RESET << setw(width - 2 - 17) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << GREEN << "4. Search" << RESET << setw(width - 2 - 9) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << GREEN << "5. Update" << RESET << setw(width - 2 - 9) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << GREEN << "6. Delete" << RESET << setw(width - 2 - 9) << " " << BLUE << "|" << RESET << endl;
        cout << BLUE << "| " << RED << "7. Logout" << RESET << setw(width - 2 - 9) << " " << BLUE << "|" << RESET << endl;
    }

    cout << BLUE << "|" << RESET << setw(width - 2) << "  " << BLUE << "|" << RESET << endl;
    cout << border << endl;
    cout << "Select an option: ";
}

class User
{
    string name;
    string email;
    long long phoneNumber;
    string password;
    string confirmPassword;
    string address;
    double monthlyIncome;

public:
    void registerUser()
    {
        cout << BLUE << "------ User Registration ------" << RESET << "\n";

        cout << "Enter your name: ";
        getline(cin, name);

        // Email Validation
        do
        {
            cout << "Enter your email: ";
            getline(cin, email);

            if (!isEmailValid(email))
            {
                cout << RED << "Invalid email format. Please try again." << RESET << "\n";
            }
        } while (!isEmailValid(email));

        // Phone Number
        do
        {
            cout << "Enter your phone number (10 digits): ";
            cin >> phoneNumber;
            cin.ignore();

            if (!isPhoneNumberValid(phoneNumber))
            {
                cout << RED << "Invalid phone number. Please enter a valid 10-digit number." << RESET << "\n";
            }
            else if (isUserAlreadyExist(phoneNumber))
            {
                cout << RED << "User already exists with this phone number. Please enter a different number." << RESET << "\n";
            }
        } while (!isPhoneNumberValid(phoneNumber) || isUserAlreadyExist(phoneNumber));

        // Password Validation
        do
        {
            cout << "Enter your password (minimum 6 characters): ";
            getline(cin, password);

            cout << "Confirm your password: ";
            getline(cin, confirmPassword);

            if (password != confirmPassword)
            {
                cout << RED << "Passwords do not match. Please try again." << RESET << "\n";
            }
            else if (!isPasswordStrong(password))
            {
                cout << RED << "Password is not strong enough. Please try again." << RESET << "\n";
            }
        } while (password != confirmPassword || !isPasswordStrong(password));

        cout << "Enter your address: ";
        getline(cin, address);

        cout << "Enter your monthly income: ";
        cin >> monthlyIncome;
        cin.ignore();

        // Save to JSON file
        saveToFile();
    }

    bool isEmailValid(const string &email)
    {
        const regex pattern(R"((\w+)(\.{0,1})(\w*)@(\w+)\.(\w+))");
        return regex_match(email, pattern);
    }

    bool isPhoneNumberValid(long long phoneNum)
    {
        return to_string(phoneNum).length() == 10;
    }

    bool isPasswordStrong(const string &password)
    {
        return password.length() >= 6;
    }

    bool isUserAlreadyExist(long long phoneNum)
    {
        ifstream inputFile("users.json");
        json data;

        if (inputFile.is_open())
        {
            if (inputFile.peek() == ifstream::traits_type::eof())
            {
                inputFile.close();
                ofstream outputFile("users.json");
                outputFile << "[]"; 
                outputFile.close();
                return false;
            }

            inputFile >> data;
            inputFile.close();

            for (auto &user : data)
            {
                if (user["phoneNumber"] == phoneNum)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void saveToFile()
    {
        string filePath = "users.json";

        ifstream inputFile(filePath);
        json data;

        if (inputFile.is_open())
        {
            inputFile >> data;
            inputFile.close();
        }

        json newUser = {
            {"name", name},
            {"email", email},
            {"phoneNumber", phoneNumber},
            {"password", password},
            {"address", address},
            {"monthlyIncome", monthlyIncome}};

        data.push_back(newUser);

        ofstream outputFile(filePath);
        outputFile << data.dump(4); 
        outputFile.close();

        cout << GREEN << "User details saved to JSON file." << RESET << "\n";
    }

    void displayUsers()
    {
        ifstream inputFile("users.json");
        json data;

        if (inputFile.is_open())
        {
            inputFile >> data;
            inputFile.close();

            cout << BLUE << "------ Registered Users ------" << RESET << "\n";
            for (auto &user : data)
            {
                cout << "Name: " << CYAN << user["name"] << RESET << "\n";
                cout << "Email: " << CYAN << user["email"] << RESET << "\n";
                cout << "Phone Number: " << CYAN << user["phoneNumber"] << RESET << "\n";
                cout << "Address: " << CYAN << user["address"] << RESET << "\n";
                cout << "Monthly Income: Rs." << CYAN << user["monthlyIncome"] << RESET << "\n\n";
            }
        }
        else
        {
            cout << RED << "No users found." << RESET << "\n";
        }
    }

    bool loginUser(long long phoneNum, string pass)
    {
        ifstream inputFile("users.json");
        json data;

        if (inputFile.is_open())
        {
            inputFile >> data;
            inputFile.close();

            for (auto &user : data)
            {
                if (user["phoneNumber"] == phoneNum && user["password"] == pass)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void addExpense(bool loggedIn, const long long &phone)
    {
        if (!loggedIn)
        {
            cout << RED << "You're not logged in. Please login first to create any expenses." << RESET << "\n";
            return;
        }

        if (to_string(phone).length() != 10)
        {
            cout << RED << "Phone Number is invalid." << RESET << "\n";
            return;
        }

        const int width = 50;
        const string border = BLUE + string(width, '-') + RESET;

        cout << border << "\n";
        cout << BLUE << "|" << RESET << setw(width - 2) << " " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << GREEN << BOLD << "------ Add Expense ------" << RESET << setw(width - 2 - 22) << " " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "|" << RESET << setw(width - 2) << " " << BLUE << "|" << RESET << "\n";
        cout << border << "\n";

        double amount;
        int categoryIndex, paymentMode;
        string date, place, notes;

        // Array of standard categories
        vector<string> categories = {
            "Food & Dining", "Groceries", "Utilities", "Rent/Mortgage",
            "Transportation", "Health & Medical", "Entertainment", "Insurance",
            "Education", "Shopping", "Travel", "Savings & Investments",
            "Debt Payments", "Gifts & Donations", "Personal Care", "Subscriptions",
            "Taxes", "Childcare", "Household Items", "Miscellaneous"};

        cout << GREEN << "Enter expense amount (Rs.): " << RESET;
        cin >> amount;
        cin.ignore();

        cout << "\n"
             << BLUE << string(width, '-') << RESET << "\n";
        cout << BOLD << "Select an expense category:\n"
             << RESET;
        for (int i = 0; i < categories.size(); ++i)
        {
            cout << setw(2) << i + 1 << ". " << categories[i] << "\n";
        }

       
        do
        {
            cout << BLUE << "\nEnter category number (1-" << categories.size() << "): " << RESET;
            cin >> categoryIndex;
            if (categoryIndex < 1 || categoryIndex > categories.size())
            {
                cout << RED << "Invalid selection. Please select a number between 1 and " << categories.size() << ".\n"
                     << RESET;
            }
        } while (categoryIndex < 1 || categoryIndex > categories.size());

        string category = categories[categoryIndex - 1];
        cin.ignore();

        cout << BLUE << string(width, '-') << RESET << "\n";
        cout << GREEN << "Enter expense date (YYYY-MM-DD): " << RESET;
        getline(cin, date);

        cout << BLUE << string(width, '-') << RESET << "\n";
        cout << GREEN << "Enter expense place: " << RESET;
        getline(cin, place);

        cout << BLUE << string(width, '-') << RESET << "\n";
        cout << BOLD << "Select payment mode:\n"
             << RESET;
        do
        {
            cout << "1. Online\n";
            cout << "2. Cash (Offline)\n";
            cout << BLUE << "Enter payment mode number (1-2): " << RESET;
            cin >> paymentMode;
            if (paymentMode < 1 || paymentMode > 2)
            {
                cout << RED << "Please select 1 or 2 only.\n"
                     << RESET;
            }
        } while (paymentMode < 1 || paymentMode > 2);

        cin.ignore();

        cout << BLUE << string(width, '-') << RESET << "\n";
        cout << GREEN << "Enter notes (Press Enter to skip): " << RESET;
        getline(cin, notes);

        
        string filePath = "expenses.json";
        ifstream inputFile(filePath);
        json expenses;

        if (inputFile.is_open())
        {
            inputFile >> expenses;
            inputFile.close();
        }

        json newExpense = {
            {"_id", generateUUID()},
            {"phoneNumber", phone},
            {"amount", amount},
            {"category", category},
            {"date", date},
            {"place", place},
            {"paymentMode", (paymentMode == 1) ? "online" : "cash"},
            {"notes", notes}};

        expenses.push_back(newExpense);

        ofstream outputFile(filePath);
        outputFile << expenses.dump(4); 
        outputFile.close();

        cout << BLUE << string(width, '-') << RESET << "\n";
        cout << GREEN << "Expense added successfully." << RESET << "\n";
    }

    void displayExpenses(long long phoneNum)
    {
        ifstream inputFile("expenses.json");
        json expenses;

        if (inputFile.is_open())
        {
            inputFile >> expenses;
            inputFile.close();

            const int width = 50;
            const string border = BLUE + string(width, '-') + RESET;

            cout << BLUE << "------ Expenses ------" << RESET << "\n";
            bool expensesFound = false;

            for (auto &expense : expenses)
            {
                if (expense["phoneNumber"] == phoneNum)
                {
                    expensesFound = true;

                    
                    cout << border << "\n";

                    cout << BLUE << "| " << RESET << setw(width - 2) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << GREEN << BOLD << "Expense ID: " << RESET << expense["_id"] << setw(width - 2 - 21) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << RESET << setw(width - 2) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << GREEN << "Amount: Rs." << RESET << CYAN << expense["amount"] << setw(width - 2 - 19) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << GREEN << "Category: " << RESET << CYAN << expense["category"] << setw(width - 2 - 20) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << GREEN << "Date: " << RESET << CYAN << expense["date"] << setw(width - 2 - 17) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << GREEN << "Place: " << RESET << CYAN << expense["place"] << setw(width - 2 - 18) << " " << BLUE << "|\n";
                    cout << BLUE << "| " << GREEN << "Payment Mode: " << RESET << CYAN << expense["paymentMode"] << setw(width - 2 - 25) << " " << BLUE << "|\n";

                   
                    if (expense.contains("notes") && !expense["notes"].empty())
                    {
                        cout << BLUE << "| " << GREEN << "Notes: " << RESET << CYAN << expense["notes"] << setw(width - 2 - 18) << " " << BLUE << "|\n";
                    }
                    else
                    {
                        cout << BLUE << "| " << GREEN << "Notes: " << RESET << CYAN << "None" << setw(width - 2 - 18) << " " << BLUE << "|\n";
                    }

                    cout << BLUE << "| " << RESET << setw(width - 2) << " " << BLUE << "|\n";
                    cout << border << "\n\n";
                }
            }

            if (!expensesFound)
            {
                cout << RED << "No expenses found for this phone number." << RESET << "\n";
            }
        }
        else
        {
            cout << RED << "Error: Could not open expenses file." << RESET << "\n";
        }
    }

    void showAnalytics(long long phoneNum)
    {
        ifstream inputFile("expenses.json");
        json expenses;

        if (!inputFile.is_open())
        {
            cout << RED << "No expense data found!" << RESET << endl;
            return;
        }

        inputFile >> expenses;
        inputFile.close();

        double totalExpenses = 0.0;
        unordered_map<string, double> categoryTotals;
        unordered_map<string, int> categoryCounts;
        int totalExpensesCount = 0;
        double maxExpense = 0.0;
        string highestCategory;
        string mostFrequentCategory;
        string largestExpenseDate;
        unordered_map<string, double> monthlyExpenses;
        unordered_map<string, int> monthlyCounts;
        string highestMonth;

        for (const auto &expense : expenses)
        {
            if (expense["phoneNumber"] == phoneNum)
            {
                double amount = expense["amount"];
                string category = expense["category"];
                string date = expense["date"];    // Format YYYY-MM-DD
                string month = date.substr(0, 7); 

                totalExpenses += amount;
                totalExpensesCount++;

            
                categoryTotals[category] += amount;
                categoryCounts[category]++;

                // Monthly breakdown
                monthlyExpenses[month] += amount;
                monthlyCounts[month]++;

                // Largest single expense
                if (amount > maxExpense)
                {
                    maxExpense = amount;
                    largestExpenseDate = date;
                }
            }
        }

        if (totalExpensesCount == 0)
        {
            cout << RED << "No expenses found for this user." << RESET << endl;
            return;
        }

        
        for (const auto &entry : categoryTotals)
        {
            string category = entry.first;
            double total = entry.second;

            if (total > categoryTotals[highestCategory])
            {
                highestCategory = category;
            }
            if (categoryCounts[category] > categoryCounts[mostFrequentCategory])
            {
                mostFrequentCategory = category;
            }
        }

        // Find the month with the highest spending
        for (const auto &entry : monthlyExpenses)
        {
            string month = entry.first;
            double total = entry.second;

            if (total > monthlyExpenses[highestMonth])
            {
                highestMonth = month;
            }
        }

        const int width = 60;
        const string border = BLUE + string(width, '-') + RESET;

       
        cout << border << endl;
        cout << BLUE << "| " << BOLD << "------ Expense Analytics ------" << RESET;
        cout << setw(width - 33) << BLUE << "|" << RESET << endl;
        cout << border << endl;

        cout << "| " << GREEN << setw(20) << left << "Total Expenses:" << RESET;
        cout << CYAN << "Rs. " << totalExpenses << RESET;
        cout << setw(width - 30) << " " << BLUE << "|" << RESET << endl;

        cout << "| " << GREEN << setw(20) << left << "Total Number of Expenses:" << RESET;
        cout << CYAN << totalExpensesCount << RESET;
        cout << setw(width - 27) << " " << BLUE << "|" << RESET << endl;

        cout << border << endl;

        cout << BLUE << "| " << BOLD << "Category Breakdown:" << RESET << endl;
        for (const auto &entry : categoryTotals)
        {
            string category = entry.first;
            double total = entry.second;
            double percentage = (total / totalExpenses) * 100;
            cout << "| " << setw(25) << left << category << ": " << CYAN << "Rs. " << total;
            cout << " (" << percentage << "%)" << RESET;
            cout << setw(width - (33 + category.length())) << BLUE << "|" << RESET << endl;
        }

        cout << border << endl;
        cout << BLUE << "| " << BOLD << "Monthly Breakdown:" << RESET << endl;
        for (const auto &entry : monthlyExpenses)
        {
            string month = entry.first;
            double total = entry.second;
            double avgMonthlyExpense = total / monthlyCounts[month];
            cout << "| " << setw(25) << left << month << ": " << CYAN << "Rs. " << total;
            cout << " (Avg: Rs. " << avgMonthlyExpense << ")" << RESET;
            cout << setw(width - (42 + month.length())) << BLUE << "|" << RESET << endl;
        }

        cout << border << endl;

        cout << "| " << GREEN << setw(25) << left << "Highest Spending Category:" << RESET;
        cout << CYAN << highestCategory << " (Rs. " << categoryTotals[highestCategory] << ")" << RESET;
        cout << setw(width - 48) << BLUE << "|" << RESET << endl;

        cout << "| " << GREEN << setw(25) << left << "Most Frequent Category:" << RESET;
        cout << CYAN << mostFrequentCategory << " (" << categoryCounts[mostFrequentCategory] << " times)" << RESET;
        cout << setw(width - 48) << BLUE << "|" << RESET << endl;

        cout << "| " << GREEN << setw(25) << left << "Single Largest Expense:" << RESET;
        cout << CYAN << "Rs. " << maxExpense << " on " << largestExpenseDate << RESET;
        cout << setw(width - 43) << BLUE << "|" << RESET << endl;

        cout << "| " << GREEN << setw(25) << left << "Highest Month:" << RESET;
        cout << CYAN << highestMonth << " (Rs. " << monthlyExpenses[highestMonth] << ")" << RESET;
        cout << setw(width - 44) << BLUE << "|" << RESET << endl;

        double avgExpense = totalExpenses / totalExpensesCount;
        cout << "| " << GREEN << setw(25) << left << "Average Expense:" << RESET;
        cout << CYAN << "Rs. " << avgExpense << RESET;
        cout << setw(width - 37) << BLUE << "|" << RESET << endl;

        cout << border << endl;
    }

    void deleteExpense(const long long &phoneNum, const string &id)
    {
        ifstream inputFile("expenses.json");
        json expenses;

        if (inputFile.is_open())
        {
            inputFile >> expenses;
            inputFile.close();
        }
        else
        {
            cout << RED << "Error opening expenses file." << RESET << "\n";
            return;
        }

        if (id == "*")
        {
            json newFile;
            ofstream outputFile("expenses.json");
            outputFile << newFile.dump(4);
            outputFile.close();
            cout << "All expenses are deleted successFully";
            return;
        }

        bool found = false;
        json updatedExpenses;

        for (const auto &expense : expenses)
        {
            if (expense["phoneNumber"] == phoneNum && expense["_id"] == id)
            {
                // If matching expense is found, set flag to true
                found = true;
            }
            else
            {
                // If not matching, add expense to updated list
                updatedExpenses.push_back(expense);
            }
        }

        if (found)
        {
           
            ofstream outputFile("expenses.json");
            outputFile << updatedExpenses.dump(4); 
            outputFile.close();

            cout << GREEN << "Expense deleted successfully with id" << id << RESET << "\n";
        }
        else
        {
            cout << RED << "Sorry, looks like something went wrong. You're trying to delete someone else's expense or the expense does not exist." << RESET << "\n";
        }
    }

    void updateExpense(const long long &phoneNum, const string &id)
    {
        ifstream inputFile("expenses.json");
        json expenses;

        if (inputFile.is_open())
        {
            inputFile >> expenses;
            inputFile.close();
        }
        else
        {
            cout << RED << "Error opening expenses file." << RESET << "\n";
            return;
        }

        bool found = false;
        for (auto &expense : expenses)
        {
            if (expense["phoneNumber"] == phoneNum && expense["_id"] == id)
            {
                found = true;

                cout << BLUE << "------ Update Expense ------" << RESET << "\n";
                cout << "Current details:\n";
                cout << "Amount: Rs." << CYAN << expense["amount"] << RESET << "\n";
                cout << "Category: " << CYAN << expense["category"] << RESET << "\n";
                cout << "Date: " << CYAN << expense["date"] << RESET << "\n";
                cout << "Place: " << CYAN << expense["place"] << RESET << "\n";
                cout << "Payment Mode: " << CYAN << expense["paymentMode"] << RESET << "\n";
                cout << "Notes: " << CYAN << expense["notes"] << RESET << "\n\n";

                double amount;
                string category, date, place, notes;
                int paymentMode;

                cout << "Enter new expense amount (Rs.): ";
                cin >> amount;
                cin.ignore(); 

                cout << "Enter new expense category: ";
                getline(cin, category);

                cout << "Enter new expense date (YYYY-MM-DD): ";
                getline(cin, date);

                cout << "Enter new expense place: ";
                getline(cin, place);

                cout << "Select new payment mode:\n";
                cout << "1. Online\n";
                cout << "2. Cash (Offline)\n";
                do
                {
                    cin >> paymentMode;
                    if (paymentMode < 1 || paymentMode > 2)
                    {
                        cout << "Please select 1 or 2 only.\n";
                    }
                } while (paymentMode < 1 || paymentMode > 2);
                cin.ignore();

                cout << "Enter new notes (Press enter to skip): ";
                getline(cin, notes);

               
                expense["amount"] = amount;
                expense["category"] = category;
                expense["date"] = date;
                expense["place"] = place;
                expense["paymentMode"] = (paymentMode == 1) ? "Online" : "Cash (Offline)";
                expense["notes"] = notes;

                break; 
            }
        }

        if (found)
        {
           
            ofstream outputFile("expenses.json");
            outputFile << expenses.dump(4); 
            outputFile.close();

            cout << GREEN << "Expense updated successfully." << RESET << "\n";
        }
        else
        {
            cout << RED << "Expense with the given ID not found or you do not have permission to update this expense." << RESET << "\n";
        }
    }

    void searchExpenses(const long long &phoneNum)
    {
        ifstream inputFile("expenses.json");
        json expenses;

        if (inputFile.is_open())
        {
            inputFile >> expenses;
            inputFile.close();
        }
        else
        {
            cout << RED << "Error opening expenses file." << RESET << "\n";
            return;
        }

        int searchOption;
        cout << BLUE << "------ Search Expenses ------" << RESET << "\n";
        cout << "1. Search by ID\n";
        cout << "2. Search by Category\n";
        cout << "3. Search by Payment Mode\n";
        cout << "4. Search by Date Range\n";
        cout << "5. Search by Amount Range\n";
        cout << "6. Search by Place\n";
        cout << "Select an option (1-6): ";
        cin >> searchOption;
        cin.ignore();

        switch (searchOption)
        {
        case 1:
        {
            string id;
            cout << "Enter Expense ID: ";
            cin >> id;
            cin.ignore();
            for (const auto &expense : expenses)
            {
                if (expense["phoneNumber"] == phoneNum && expense["_id"] == id)
                {
                    displaySingleExpense(expense);
                    return;
                }
            }
            cout << RED << "No expense found with the given ID." << RESET << "\n";
            break;
        }

        case 2:
        {
            string category;
            cout << "Enter Category: ";
            getline(cin, category);

            for (int i = 0; i < category.length(); i++)
            {
                category[i] = toupper(category[i]);
            }

            bool found = false;
            for (const auto &expense : expenses)
            {
                string expCat = expense["category"];
                for (int i = 0; i < expCat.length(); i++)
                {
                    expCat[i] = toupper(expCat[i]);
                }
                if (expense["phoneNumber"] == phoneNum && expCat == category)
                {
                    displaySingleExpense(expense);
                    found = true;
                }
            }
            if (!found)
                cout << RED << "No expenses found in the given category." << RESET << "\n";
            break;
        }

        case 3:
        {
            string paymentMode;
            cout << "Enter Payment Mode (online/cash): ";
            getline(cin, paymentMode);
            for(int i = 0; i < paymentMode.length(); i++){
                paymentMode[i] = toupper(paymentMode[i]);
            }
            bool found = false;
            for (const auto &expense : expenses)
            {
                string pay = expense["paymentMode"];

                for(int i = 0 ; i < pay.length(); i++){
                    pay[i] = toupper(pay[i]);
                }
                 
                if (expense["phoneNumber"] == phoneNum && pay == paymentMode)
                {
                    displaySingleExpense(expense);
                    found = true;
                }
            }
            if (!found)
                cout << RED << "No expenses found with the given payment mode." << RESET << "\n";
            break;
        }

        case 4:
        {
            string startDate, endDate;
            cout << "Enter Start Date (YYYY-MM-DD): ";
            getline(cin, startDate);
            cout << "Enter End Date (YYYY-MM-DD): ";
            getline(cin, endDate);
            bool found = false;
            for (const auto &expense : expenses)
            {
                if (expense["phoneNumber"] == phoneNum && expense["date"] >= startDate && expense["date"] <= endDate)
                {
                    displaySingleExpense(expense);
                    found = true;
                }
            }
            if (!found)
                cout << RED << "No expenses found within the given date range." << RESET << "\n";
            break;
        }

        case 5:
        {
            double minAmount, maxAmount;
            cout << "Enter Minimum Amount: ";
            cin >> minAmount;
            cout << "Enter Maximum Amount: ";
            cin >> maxAmount;
            cin.ignore();
            bool found = false;
            for (const auto &expense : expenses)
            {
                if (expense["phoneNumber"] == phoneNum && expense["amount"] >= minAmount && expense["amount"] <= maxAmount)
                {
                    displaySingleExpense(expense);
                    found = true;
                }
            }
            if (!found)
                cout << RED << "No expenses found within the given amount range." << RESET << "\n";
            break;
        }

        case 6:
        {
            string place;
            cout << "Enter Place: ";
            getline(cin, place);
            for (int i = 0; i < place.length(); i++)
            {
                place[i] = toupper(place[i]);
            }
            bool found = false;
            for (const auto &expense : expenses)
            {
                string expPlace = expense["place"];
                for (int i = 0; i < expPlace.length(); i++)
                {
                    expPlace[i] = toupper(expPlace[i]);
                }
                if (expense["phoneNumber"] == phoneNum && expPlace == place)
                {
                    displaySingleExpense(expense);
                    found = true;
                }
            }
            if (!found)
                cout << RED << "No expenses found in the given place." << RESET << "\n";
            break;
        }

        default:
            cout << RED << "Invalid option selected." << RESET << "\n";
            break;
        }
    }

    void displaySingleExpense(const json &expense)
    {
        cout << BLUE << "------ Expense Details ------" << RESET << "\n";
        cout << "ID: " << CYAN << expense["_id"] << RESET << "\n";
        cout << "Amount: Rs." << CYAN << expense["amount"] << RESET << "\n";
        cout << "Category: " << CYAN << expense["category"] << RESET << "\n";
        cout << "Date: " << CYAN << expense["date"] << RESET << "\n";
        cout << "Place: " << CYAN << expense["place"] << RESET << "\n";
        cout << "Payment Mode: " << CYAN << expense["paymentMode"] << RESET << "\n";
        cout << "Notes: " << CYAN << (expense["notes"] != "" ? expense["notes"] : "No Notes") << RESET << "\n\n";
    }
};

int main()
{
    User user;
    int option;
    string password;
    long long phoneNumber;
    bool loggedIn = false;

    while (true)
    {
        printMenuBox(loggedIn, phoneNumber);

        cin >> option;
        cin.ignore();

        switch (option)
        {
        case 1:
            if (!loggedIn)
            {
                cout << "Registering a new user...\n";
                user.registerUser();
            }
            else
            {
                user.addExpense(loggedIn, phoneNumber);
            }
            break;
        
        case 2:
            if (!loggedIn)
            {
                do
                {
                    cout << "Enter your phone number to login: ";
                    cin >> phoneNumber;
                    if (!user.isPhoneNumberValid(phoneNumber))
                    {
                        cout << RED << "phone number must contain 10 digit" << RESET << "\n";
                    }
                } while (!user.isPhoneNumberValid(phoneNumber));

                cout << BLUE << "Please enter the password" << RESET << "\n";
                cin >> password;

                cin.ignore();
                if (user.loginUser(phoneNumber, password))
                {
                    cout << GREEN << "Login successful." << RESET << "\n";
                    loggedIn = true;
                }
                else
                {
                    cout << RED << "User with this phone number & password does not exist." << RESET << "\n";
                }
            }
            else
            {
                user.displayExpenses(phoneNumber);
            }
            break;
    
        case 3:
            if (loggedIn)
            {
                user.showAnalytics(phoneNumber);
            }
            break;
    
        case 4:
            if (loggedIn)
            {
                user.searchExpenses(phoneNumber);
            }
            break;
    
        case 5:
            if (loggedIn)
            {
                string id;
                cout << "Enter the id of expense";
                getline(cin, id);
                user.updateExpense(phoneNumber, id);
            }
            break;

        case 6:
            if (loggedIn)
            {
                string id;
                cout << "Enter the id of Expenses to delte one or enter * for delete all";
                getline(cin, id);
                user.deleteExpense(phoneNumber, id);
            }
            break;

        case 7:
            if (loggedIn)
            {
                cout << GREEN << "Logging out..." << RESET << "\n";
                loggedIn = false;
            }
            else
            {
                cout << RED << "Invalid option. Please try again." << RESET << "\n";
            }
            break;

        default:
            cout << RED << "Invalid option. Please try again." << RESET << "\n";
            break;
        }
    }

    return 0;
}
