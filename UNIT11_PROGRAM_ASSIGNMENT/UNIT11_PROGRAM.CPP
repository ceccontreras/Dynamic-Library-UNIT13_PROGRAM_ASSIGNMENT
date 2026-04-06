// ============================================================
// Program:  UNIT11_PROGRAM.CPP
// Author:   Carlos Campos
// Date:     April 3, 2026
// Purpose:  Unit 11 Banking System — This program uses inheritance
//           and composition using a bankAccount base class and
//           two derived classes (checkingAccount, savingsAccount).
//           Features a 9-option menu, input validation, virtual
//           method overrides, and file I/O.
// ============================================================

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

// BASE CLASS: bankAccount

class bankAccount {
protected:
    int    accountNumber;
    double balance;

public:
    // Constructors
    bankAccount() : accountNumber(0), balance(0.0) {}
    bankAccount(int accNum, double bal) : accountNumber(accNum), balance(bal) {}

    // Setters & Getters
    void   setAccountNumber(int accNum) { accountNumber = accNum; }
    int    getAccountNumber() const     { return accountNumber; }
    double getBalance()       const     { return balance; }

    // Operations
    void deposit(double amount) {
        if (amount > 0.0)
            balance += amount;
    }

    virtual void withdraw(double amount) {
        if (amount > 0.0 && amount <= balance)
            balance -= amount;
        else
            cout << "  Insufficient funds or invalid amount.\n";
    }

    // Virtual display &  file output
    virtual void printAccountInfo() const {
        cout << fixed << setprecision(2);
        cout << "  Account Number : " << accountNumber << "\n"
             << "  Balance        : $" << balance       << "\n";
    }

    virtual void saveToFile(ofstream &outFile) const {
        outFile << fixed << setprecision(2);
        outFile << "  Account Number : " << accountNumber << "\n"
                << "  Balance        : $" << balance       << "\n";
    }

    virtual ~bankAccount() {}
};


// DERIVED CLASS: checkingAccount

class checkingAccount : public bankAccount {
private:
    double interestRate;    // stored as a percentage %
    double minimumBalance;
    double serviceCharges;

public:
    // Constructors
    checkingAccount() : bankAccount(), interestRate(0.0),
                        minimumBalance(0.0), serviceCharges(0.0) {}

    checkingAccount(int accNum, double bal, double intRate,
                    double minBal, double svcCharge)
        : bankAccount(accNum, bal),
          interestRate(intRate),
          minimumBalance(minBal),
          serviceCharges(svcCharge) {}

    // Getters
    double getInterestRate()  const { return interestRate;  }
    double getMinimumBalance()const { return minimumBalance; }
    double getServiceCharges()const { return serviceCharges; }

    // Business logic
    bool isBalanceBelowMinimum() const { return balance < minimumBalance; }

    void postInterest() {
        balance += balance * (interestRate / 100.0);
        cout << fixed << setprecision(2);
        cout << "  Interest Posted. New balance: $" << balance << "\n";
    }

    // writeCheck is just a labeled withdraw for checks
    void writeCheck(double amount) {
        cout << "  Writing check for $" << fixed << setprecision(2) << amount << "...\n";
        withdraw(amount);   // calls overridden withdraw below
    }

    // Override: apply service charge when balance drops below minimum
    void withdraw(double amount) override {
        if (amount <= 0.0) {
            cout << "  Invalid amount.\n";
            return;
        }
        if (amount > balance) {
            cout << "  Insufficient funds.\n";
            return;
        }
        balance -= amount;
        cout << fixed << setprecision(2);
        cout << "  Withdrawal successful. New balance: $" << balance << "\n";

        if (isBalanceBelowMinimum()) {
            balance -= serviceCharges;
            cout << "  ** Balance below minimum ($" << minimumBalance
                 << "). Applying service charge of $" << serviceCharges
                 << ". New balance: $" << balance << "\n";
        }
    }

    // Override: print full checking info
    void printAccountInfo() const override {
        cout << fixed << setprecision(2);
        cout << "  --- Checking Account ---\n"
             << "  Account Number  : " << accountNumber  << "\n"
             << "  Balance         : $" << balance        << "\n"
             << "  Interest Rate   : "  << interestRate   << "%\n"
             << "  Minimum Balance : $" << minimumBalance << "\n"
             << "  Service Charges : $" << serviceCharges << "\n";
        if (isBalanceBelowMinimum())
            cout << "  ** WARNING: Balance is below minimum!\n";
    }

    void saveToFile(ofstream &outFile) const override {
        outFile << fixed << setprecision(2);
        outFile << "--- Checking Account ---\n"
                << "Account Number  : " << accountNumber  << "\n"
                << "Balance         : $" << balance        << "\n"
                << "Interest Rate   : "  << interestRate   << "%\n"
                << "Minimum Balance : $" << minimumBalance << "\n"
                << "Service Charges : $" << serviceCharges << "\n";
    }
};


// DERIVED CLASS: savingsAccount

class savingsAccount : public bankAccount {
private:
    double interestRate;    // stored as a percentage %

public:
    // Constructors
    savingsAccount() : bankAccount(), interestRate(0.0) {}
    savingsAccount(int accNum, double bal, double intRate)
        : bankAccount(accNum, bal), interestRate(intRate) {}

    // Getter
    double getInterestRate() const { return interestRate; }

    // Business logic
    void postInterest() {
        balance += balance * (interestRate / 100.0);
        cout << fixed << setprecision(2);
        cout << "  Interest Posted. New balance: $" << balance << "\n";
    }

    // Override: savings accounts cannot go negative
    void withdraw(double amount) override {
        if (amount <= 0.0) {
            cout << "  Invalid amount.\n";
            return;
        }
        if (amount > balance) {
            cout << "  Insufficient funds. Withdrawal denied.\n";
            return;
        }
        balance -= amount;
        cout << fixed << setprecision(2);
        cout << "  Withdrawal successful. New balance: $" << balance << "\n";
    }

    // Override: print full savings info
    void printAccountInfo() const override {
        cout << fixed << setprecision(2);
        cout << "  --- Savings Account ---\n"
             << "  Account Number  : " << accountNumber << "\n"
             << "  Balance         : $" << balance       << "\n"
             << "  Interest Rate   : "  << interestRate  << "%\n";
    }

    void saveToFile(ofstream &outFile) const override {
        outFile << fixed << setprecision(2);
        outFile << "--- Savings Account ---\n"
                << "Account Number  : " << accountNumber << "\n"
                << "Balance         : $" << balance       << "\n"
                << "Interest Rate   : "  << interestRate  << "%\n";
    }
};



// INPUT VALIDATION HELPER

double getValidatedAmount(const string &prompt) {
    double amount;
    while (true) {
        cout << prompt;
        if (cin >> amount && amount > 0.0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return amount;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid input. Please enter a positive dollar amount.\n";
    }
}

int getValidatedMenuChoice(int lo, int hi) {
    int choice;
    while (true) {
        cout << "  Enter choice (" << lo << "-" << hi << "): ";
        if (cin >> choice && choice >= lo && choice <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid choice. Try again.\n";
    }
}


// MENU DISPLAY

void displayMenu() {
    cout << "\n"
         << "============================================\n"
         << "          BANKING SYSTEM MAIN MENU         \n"
         << "============================================\n"
         << "  1. View Checking Account Info\n"
         << "  2. Deposit to Checking Account\n"
         << "  3. Withdraw / Write Check (Checking)\n"
         << "  4. Post Interest (Checking)\n"
         << "  5. View Savings Account Info\n"
         << "  6. Deposit to Savings Account\n"
         << "  7. Withdraw from Savings Account\n"
         << "  8. Post Interest (Savings)\n"
         << "  9. Exit and Save Account Summary\n"
         << "============================================\n";
}



// SAVE BOTH ACCOUNTS TO FILE

void saveSummaryToFile(const checkingAccount &chk, const savingsAccount &sav) {
    ofstream outFile("AccountSummary.txt");
    if (!outFile) {
        cout << "  ERROR: Could not open AccountSummary.txt for writing.\n";
        return;
    }

    outFile << "============================================\n"
            << "          ACCOUNT SUMMARY REPORT           \n"
            << "============================================\n\n";
    chk.saveToFile(outFile);
    outFile << "\n";
    sav.saveToFile(outFile);
    outFile << "\n============================================\n"
            << "  Report generated on exit.\n"
            << "============================================\n";

    outFile.close();
    cout << "  Account summary saved to AccountSummary.txt\n";
}



// MAIN — menu loop

int main() {
    // Initialize accounts with sample starting values from the assignment
    checkingAccount checking(1001, 500.00, 2.0, 300.00, 25.00);
    savingsAccount  savings (2001, 1000.00, 3.0);

    double amount;
    int    choice;

    cout << "============================================\n"
         << "    Welcome to the Banking System       \n"
         << "============================================\n";

    do {
        displayMenu();
        choice = getValidatedMenuChoice(1, 9);

        switch (choice) {
            // CHECKING
            case 1:
                cout << "\n";
                checking.printAccountInfo();
                break;

            case 2:
                amount = getValidatedAmount("  Enter deposit amount for Checking: $");
                checking.deposit(amount);
                cout << fixed << setprecision(2);
                cout << "  Deposited $" << amount
                     << "  |  New balance: $" << checking.getBalance() << "\n";
                break;

            case 3:
                amount = getValidatedAmount("  Enter check/withdrawal amount: $");
                checking.writeCheck(amount);
                break;

            case 4:
                cout << "\n  Posting interest to Checking Account...\n";
                checking.postInterest();
                break;

            // SAVINGS
            case 5:
                cout << "\n";
                savings.printAccountInfo();
                break;

            case 6:
                amount = getValidatedAmount("  Enter deposit amount for Savings: $");
                savings.deposit(amount);
                cout << fixed << setprecision(2);
                cout << "  Deposited $" << amount
                     << "  |  New balance: $" << savings.getBalance() << "\n";
                break;

            case 7:
                amount = getValidatedAmount("  Enter withdrawal amount for Savings: $");
                savings.withdraw(amount);
                break;

            case 8:
                cout << "\n  Posting interest to Savings Account...\n";
                savings.postInterest();
                break;

            // EXIT
            case 9:
                cout << "\n  Saving account summaries...\n";
                saveSummaryToFile(checking, savings);
                cout << "  Thank you for using the Banking System. Goodbye!\n\n";
                break;
        }

    } while (choice != 9);

    return 0;
}
