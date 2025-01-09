#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <cctype> // For isalnum
#include <fstream> // For file handling
using namespace std;

enum AccountType { SAVINGS, CURRENT };

struct BankAccount {
    int accountNumber;
    string accountHolderName;
    double balance;
    string accountHolderUsername;
    string accountHolderPassword;
    AccountType accountType;
    BankAccount* next;
};

struct TreeNode {
    BankAccount* account;
    TreeNode* left;
    TreeNode* right;
};

class Bank {
private:
    BankAccount* accountHead = nullptr;
    TreeNode* root = nullptr;
    stack<string> actionStack;
    queue<string> requestQueue;

    // Function to validate input for special characters
    bool isValidInput(const string& input) {
        for (char c : input) {
            if (!isalnum(c) && c != ' ') { // Allow alphanumeric characters and spaces
                return false;
            }
        }
        return true;
    }
    void loadAccountsFromFile() {
        ifstream inFile("accounts.txt");
        if (!inFile) {
            cout << "No account data found. Starting fresh.\n";
            return;
        }
        while (true) {
            BankAccount* newAccount = new BankAccount;
            string accountType;
            inFile >> newAccount->accountNumber;
            if (inFile.eof()) break; // Break if end of file is reached
            inFile.ignore(); // Ignore newline
            getline(inFile, newAccount->accountHolderName);
            inFile >> newAccount->balance;
            inFile >> newAccount->accountHolderUsername;
            inFile >> newAccount->accountHolderPassword;
            inFile >> accountType;
            newAccount->accountType = (accountType == "SAVINGS") ? SAVINGS : CURRENT;
            newAccount->next = nullptr;

            // Insert into linked list
            if (!accountHead) {
                accountHead = newAccount;
            }
            else {
                BankAccount* temp = accountHead;
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = newAccount;
            }
        }
        inFile.close();
        cout << "Accounts loaded from file successfully.\n";
    }

    // Function to save accounts to a file
    void saveAccountsToFile() {
        ofstream outFile("accounts.txt");
        BankAccount* temp = accountHead;
        while (temp) {
            outFile << temp->accountNumber << endl;
            outFile << temp->accountHolderName << endl;
            outFile << temp->balance << endl;
            outFile << (temp->accountType == SAVINGS ? "SAVINGS" : "CURRENT") << endl;
            outFile << temp->accountHolderUsername << endl;
            outFile << temp->accountHolderPassword << endl;
            temp = temp->next;
        }
        outFile.close();
        cout << "Accounts saved to file successfully.\n";
    }

public:
    Bank() {
        loadAccountsFromFile(); // Load accounts when the bank object is created
    }

    ~Bank() {
        saveAccountsToFile(); // Save accounts when the bank object is destroyed
    }

    void createAccount() {
        BankAccount* newAccount = new BankAccount;
        cout << "\nEnter Account Number: ";
        cin >> newAccount->accountNumber;

        cout << "Enter Account Holder Name: ";
        cin.ignore();
        getline(cin, newAccount->accountHolderName);

        cout << "Enter Initial Deposit Amount: $";
        cin >> newAccount->balance;

        // Validate username input
        while (true) {
            cout << "Enter Account Holder Username: ";
            cin >> newAccount->accountHolderUsername;
            if (isValidInput(newAccount->accountHolderUsername)) {
                break;
            }
            else {
                cout << "Invalid username. Please use alphanumeric characters only.\n";
            }
        }

        // Validate password input
        while (true) {
            cout << "Enter Account Holder Password: ";
            cin >> newAccount->accountHolderPassword;
            if (isValidInput(newAccount->accountHolderPassword)) {
                break;
            }
            else {
                cout << "Invalid password. Please use alphanumeric characters only.\n";
            }
        }

        int type;
        cout << "Enter Account Type (1 for Savings, 2 for Current): ";
        cin >> type;
        newAccount->accountType = (type == 1) ? SAVINGS : CURRENT;
        newAccount->next = nullptr;

        if (!accountHead) {
            accountHead = newAccount;
        }
        else {
            BankAccount* temp = accountHead;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newAccount;
        }
        insertAccountInBST(root, newAccount);
        cout << "\nAccount created successfully!\n";
        actionStack.push("Account Created: " + newAccount->accountHolderName);
    }

    void insertAccountInBST(TreeNode*& node, BankAccount* account) {
        if (node == nullptr) {
            node = new TreeNode;
            node->account = account;
            node->left = node->right = nullptr;
        }
        else if (account->accountNumber < node->account->accountNumber) {
            insertAccountInBST(node->left, account);
        }
        else {
            insertAccountInBST(node->right, account);
        }
    }

    void viewAccountDetails() {
        int accNo;
        cout << "\nEnter Account Number to View Details: ";
        cin >> accNo;
        BankAccount* temp = accountHead;
        bool found = false;
        while (temp) {
            if (temp->accountNumber == accNo) {
                cout << "\nAccount Number: " << temp->accountNumber << endl;
                cout << "Account Holder Name: " << temp->accountHolderName << endl;
                cout << "Balance: $" << temp->balance << endl;
                cout << "Account Type: " << (temp->accountType == SAVINGS ? "Savings" : "Current") << endl;
                found = true;
                break;
            }
            temp = temp->next;
        }
        if (!found) {
            cout << "\nAccount not found.\n";
        }
    }

    void depositMoney() {
        int accNo;
        double amount;
        cout << "\nEnter Account Number to Deposit Money: ";
        cin >> accNo;
        cout << "Enter Amount to Deposit: $";
        cin >> amount;
        requestQueue.push("Deposit Request for Account No: " + to_string(accNo));
        processDeposit(accNo, amount);
    }

    void processDeposit(int accNo, double amount) {
        BankAccount* temp = accountHead;
        bool found = false;
        while (temp) {
            if (temp->accountNumber == accNo) {
                temp->balance += amount;
                cout << "\nAmount Deposited Successfully! New Balance: $" << temp->balance << endl;
                found = true;
                break;
            }
            temp = temp->next;
        }
        if (!found) {
            cout << "\nAccount not found.\n";
        }
    }

    void withdrawMoney() {
        int accNo;
        double amount;
        cout << "\nEnter Account Number to Withdraw Money: ";
        cin >> accNo;
        cout << "Enter Amount to Withdraw: $";
        cin >> amount;
        requestQueue.push("Withdrawal Request for Account No: " + to_string(accNo));
        processWithdrawal(accNo, amount);
    }

    void processWithdrawal(int accNo, double amount) {
        BankAccount* temp = accountHead;
        bool found = false;
        while (temp) {
            if (temp->accountNumber == accNo) {
                if (temp->balance >= amount) {
                    temp->balance -= amount;
                    cout << "\nAmount Withdrawn Successfully! New Balance: $" << temp->balance << endl;
                }
                else {
                    cout << "Insufficient balance for withdrawal.\n";
                }
                found = true;
                break;
            }
            temp = temp->next;
        }
        if (!found) {
            cout << "\nAccount not found.\n";
        }
    }

    bool validateAdminLogin() {
        string username = "admin";
        string password = "admin123";
        string enteredUsername, enteredPassword;
        cout << "\nEnter Admin Username: ";
        cin >> enteredUsername;
        cout << "Enter Admin Password: ";
        cin >> enteredPassword;
        return (enteredUsername == username && enteredPassword == password);
    }

    bool validateAccountHolderLogin() {
        string username, password;
        cout << "\nEnter Account Holder Username: ";
        cin >> username;
        cout << "Enter Account Holder Password: ";
        cin >> password;
        BankAccount* temp = accountHead;
        while (temp) {
            if (temp->accountHolderUsername == username && temp->accountHolderPassword == password) {
                return true;
            }
            temp = temp->next;
        }
        cout << "Invalid Username or Password.\n";
        return false;
    }

    void processRequests() {
        while (!requestQueue.empty()) {
            cout << "\nProcessing Request: " << requestQueue.front() << endl;
            requestQueue.pop();
        }
    }

    void showRecentActions() {
        cout << "\nRecent Actions:\n";
        while (!actionStack.empty()) {
            cout << actionStack.top() << endl;
            actionStack.pop();
        }
    }

    void viewAccountsByType(AccountType type) {
        BankAccount* temp = accountHead;
        bool found = false;
        while (temp) {
            if (temp->accountType == type) {
                cout << "\nAccount Number: " << temp->accountNumber << endl;
                cout << "Account Holder Name: " << temp->accountHolderName << endl;
                cout << "Balance: $" << temp->balance << endl;
                cout << "Account Type: " << (temp->accountType == SAVINGS ? "Savings" : "Current") << endl;
                found = true;
            }
            temp = temp->next;
        }
        if (!found) {
            cout << "\nNo accounts found for this type.\n";
        }
    }

    void listAllAccountHolders() {
        BankAccount* temp = accountHead;
        if (!temp) {
            cout << "\nNo accounts available.\n";
        }
        else {
            cout << "\nList of All Account Holders:\n";
            while (temp) {
                cout << "Account Holder: " << temp->accountHolderName << " (Account No: " << temp->accountNumber << ")\n";
                temp = temp->next;
            }
        }
    }

    void removeAccountHolder() {
        int accNo;
        cout << "\nEnter Account Number to Remove: ";
        cin >> accNo;
        BankAccount* temp = accountHead;
        BankAccount* prev = nullptr;
        bool found = false;
        while (temp) {
            if (temp->accountNumber == accNo) {
                if (prev) {
                    prev->next = temp->next;
                }
                else {
                    accountHead = temp->next; // Removing the head of the list
                }
                delete temp;
                cout << "\nAccount removed successfully!\n";
                actionStack.push("Account Removed: Account No " + to_string(accNo));
                found = true;
                break;
            }
            prev = temp;
            temp = temp->next;
        }
        if (!found) {
            cout << "\nAccount not found.\n";
        }
    }

    void updateAccountDetails() {
        int accNo;
        cout << "\nEnter Account Number to Update Details: ";
        cin >> accNo;
        BankAccount* temp = accountHead;
        bool found = false;
        while (temp) {
            if (temp->accountNumber == accNo) {
                found = true;
                cout << "\nAccount found. Choose detail to update:\n";
                cout << "1. Update Account Holder Name\n";
                cout << "2. Update Account Balance\n";
                cout << "3. Update Username\n";
                cout << "4. Update Password\n";
                cout << "Enter your choice: ";
                int choice;
                cin >> choice;
                switch (choice) {
                case 1:
                    cout << "Enter new Account Holder Name: ";
                    cin.ignore();
                    getline(cin, temp->accountHolderName);
                    break;
                case 2:
                    cout << "Enter new Account Balance: $";
                    cin >> temp->balance;
                    break;
                case 3:
                    while (true) {
                        cout << "Enter new Username: ";
                        cin >> temp->accountHolderUsername;
                        if (isValidInput(temp->accountHolderUsername)) {
                            break;
                        }
                        else {
                            cout << "Invalid username. Please use alphanumeric characters only.\n";
                        }
                    }
                    break;
                case 4:
                    while (true) {
                        cout << "Enter new Password: ";
                        cin >> temp->accountHolderPassword;
                        if (isValidInput(temp->accountHolderPassword)) {
                            break;
                        }
                        else {
                            cout << "Invalid password. Please use alphanumeric characters only.\n";
                        }
                    }
                    break;
                default:
                    cout << "Invalid choice.\n";
                }
                cout << "\nAccount details updated successfully!\n";
                actionStack.push("Account Updated: Account No " + to_string(accNo));
                break;
            }
            temp = temp->next;
        }
        if (!found) {
            cout << "\nAccount not found.\n";
        }
    }
};

// Admin menu
void adminView(Bank& bank) {
    int adminChoice;
    while (true) {
        cout << "\n===== Admin Menu =====\n";
        cout << "1. Create Account\n";
        cout << "2. View Account Details\n";
        cout << "3. View Recent Actions\n";
        cout << "4. View All Saving Accounts\n";
        cout << "5. View All Current Accounts\n";
        cout << "6. List All Account Holders\n";
        cout << "7. Remove Account Holder\n";
        cout << "8. Update Account Details\n";
        cout << "9. Exit to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> adminChoice;
        switch (adminChoice) {
        case 1:
            bank.createAccount();
            break;
        case 2:
            bank.viewAccountDetails();
            break;
        case 3:
            bank.showRecentActions();
            break;
        case 4:
            bank.viewAccountsByType(SAVINGS);
            break;
        case 5:
            bank.viewAccountsByType(CURRENT);
            break;
        case 6:
            bank.listAllAccountHolders();
            break;
        case 7:
            bank.removeAccountHolder();
            break;
        case 8:
            bank.updateAccountDetails();
            break;
        case 9:
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Account holder menu
void accountHolderView(Bank& bank) {
    int holderChoice;
    while (true) {
        cout << "\n===== Account Holder Menu =====\n";
        cout << "1. View Account Details\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Process Requests\n";
        cout << "5. Exit to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> holderChoice;
        switch (holderChoice) {
        case 1:
            bank.viewAccountDetails();
            break;
        case 2:
            bank.depositMoney();
            break;
        case 3:
            bank.withdrawMoney();
            break;
        case 4:
            bank.processRequests();
            break;
        case 5:
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    Bank bank;
    int choice;
    cout << "=====================================\n";
    cout << " BANK MANAGEMENT SYSTEM\n";
    cout << "=====================================\n";
    while (true) {
        cout << "\n1. Admin Login\n";
        cout << "2. Account Holder Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1: {
            if (bank.validateAdminLogin()) {
                adminView(bank);
            }
            else {
                cout << "Invalid Admin credentials.\n";
            }
            break;
        }
        case 2: {
            if (bank.validateAccountHolderLogin()) {
                accountHolderView(bank);
            }
            else {
                cout << "Invalid Username or Password.\n";
            }
            break;
        }
        case 3:
            cout << "\nExiting the system...\n";
            return 0;
        default:
            cout << "\nInvalid choice. Please try again.\n";
        }
    }
    return 0;
}