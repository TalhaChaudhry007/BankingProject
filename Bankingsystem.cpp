/**
 * Object Oriented Programming - Assignment 3
 * Banking Management System (Company Pending & Card Update)
 * * * UPDATES:
 * 1. Company Registration -> pending_companies.txt.
 * 2. Admin approves Company -> Moves to companies.txt & Assigns Card.
 * 3. Company Withdraw -> Checks for Card, allows creation if missing.x
 * 4. Admin 'New Requests' menu split into User/Company.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

//   Configuration and constants

const int MAX_LINES = 100;
const int MAX_TOKENS = 15;


//   Helper functions


unsigned long long seed = 0;

void mySrand() {
    seed = time(0);
}

int myRand() {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed / 65536) % 32768;
}

bool isCharDigit(char c) {
    return (c >= '0' && c <= '9');
}

bool isCharPrintable(char c) {
    return (c >= 32 && c <= 126);
}

void printTitle(string title) {
    cout << "\n" << endl;
    cout << "       " << title << endl;
    cout << "                                           " << endl;
}

void showCategoryLimits() {
    cout << "--- Account Tiers & Max Balance Limits ---\n";
    cout << "  [Bronze]   : 100,000,000 PKR\n";
    cout << "  [Gold]     : 1,000,000,000 PKR\n";
    cout << "  [Business] : 100,000,000,000 PKR\n";
    cout << "\n\n";
}

string cleanString(string s) {
    string res = "";

    for (int i = 0; i < s.length(); i++) {
        if (isCharPrintable(s[i])) {
            res += s[i];
        }
    }

    int start = 0;

    while (start < res.length() && res[start] == ' ') {
        start++;
    }

    long int end = res.length() - 1;

    while (end >= 0 && res[end] == ' ') {
        end--;
    }

    if (end < start) {
        return "";
    }

    return res.substr(start, end - start + 1);
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    return to_string(ltm->tm_mday) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(1900 + ltm->tm_year);
}

bool isNumeric(const string& s) {
    string clean = cleanString(s);

    if (clean.empty()) {
        return false;
    }

    for (int i = 0; i < clean.length(); i++) {
        if (!isCharDigit(clean[i])) {
            return false;
        }
    }

    return true;
}

double strToDouble(string s) {
    try {
        return stod(s);
    }

    catch (...) {
        return 0.0;
    }
}

int split(string s, char delimiter, string arr[], int maxTokens) {
    string token = "";
    int count = 0;
    s = cleanString(s);

    for (int i = 0; i < s.length(); i++) {
        if (s[i] == delimiter) {
            if (count < maxTokens) {
                arr[count++] = cleanString(token);
            }
            token = "";
        }

        else {
            token += s[i];
        }
    }

    if (count < maxTokens) {
        arr[count++] = cleanString(token);
    }

    return count;
}

//        OOP Class Hierarchy

class AccountType {
public:
    virtual string getName() const = 0;
    virtual double getDailyLimit() const = 0;
    virtual double getBalanceLimit() const = 0;
    virtual ~AccountType() {
    }
};

class Bronze : public AccountType {
    string getName() const override {
        return "Bronze";
    }

    double getDailyLimit() const override {
        return 100000.0;
    }

    double getBalanceLimit() const override {
        return 100000000.0;
    }
};

class Gold : public AccountType {
    string getName() const override {
        return "Gold";
    }

    double getDailyLimit() const override {
        return 500000.0;
    }

    double getBalanceLimit() const override {
        return 1000000000.0;
    }
};

class Business : public AccountType {
    string getName() const override {
        return "Business";
    }

    double getDailyLimit() const override {
        return 20000000.0;
    }

    double getBalanceLimit() const override {
        return 100000000000.0;
    }
};

class Account {
protected:
    string id, username, password;
    double balance;
    AccountType* type;

public:
    Account(string id, string user, string pass, double bal, string cat)
        : id(id), username(user), password(pass), balance(bal) {
        if (cat == "Bronze") {
            type = new Bronze();
        }

        else if (cat == "Gold") {
            type = new Gold();
        }

        else if (cat == "Business") {
            type = new Business();
        }

        else {
            type = new Bronze();
        }
    }

    virtual ~Account() {
        delete type;
    }

    string getId() const {
        return id;
    }

    string getUsername() const {
        return username;
    }

    double getBalance() const {
        return balance;
    }

    string getCategory() const {
        return type->getName();
    }

    double getDailyLimit() const {
        return type->getDailyLimit();
    }

    double getBalanceLimit() const {
        return type->getBalanceLimit();
    }

    virtual void displayInfo() const = 0;
};

class UserAccount : public Account {
    string fullName, address, cnic;

public:
    UserAccount(string id, string name, string addr, string cnic, string user, string pass, double bal, string cat)
        : Account(id, user, pass, bal, cat), fullName(name), address(addr), cnic(cnic) {
    }

    void displayInfo() const override {
        cout << id << " | " << fullName << " | " << cnic << " | " << (long long)balance << " PKR | " << getCategory() << endl;
    }
};

class CompanyAccount : public Account {
    string companyName, address, taxNumber;

public:
    CompanyAccount(string id, string name, string addr, string tax, string user, string pass, double bal, string cat)
        : Account(id, user, pass, bal, cat), companyName(name), address(addr), taxNumber(tax) {
    }

    void displayInfo() const override {
        cout << id << " | " << companyName << " | " << taxNumber << " | " << (long long)balance << " PKR | " << getCategory() << endl;
    }
};
=
//    Controller class

class BankSystem {
private:
    const string F_USER = "user.txt";
    const string F_PENDING = "pending_users.txt";
    const string F_COMP = "companies.txt";
    const string F_PENDING_COMP = "pending_companies.txt";
    const string F_EMP = "companies_employees.txt";
    const string F_ADMIN = "admin.txt";
    const string F_CARDS = "cards.txt";
    const string F_TRANS = "transactions.txt";
    const string F_LOAN = "loan.txt";
    const string F_FREEZE_ACC = "freeze_acc.txt";
    const string F_FREEZE_CARD = "freeze_cards.txt";
    const string F_SUSP_ACC = "suspicious_acc.txt";

    //  Manual File IO 

    int readLines(string filename, string arr[], int maxLines) {
        ifstream fin(filename);
        int count = 0;

        if (fin.is_open()) {
            string line;
            while (getline(fin, line) && count < maxLines) {
                string clean = cleanString(line);
                if (clean.length() > 1) {
                    arr[count++] = clean;
                }
            }
            fin.close();
        }

        return count;
    }

    void appendLine(string filename, string line) {
        ofstream fout(filename, ios::app);

        if (fout.is_open()) {
            fout << line << endl;
            fout.close();
        }

        else {
            cerr << "[ERROR] Cannot write to " << filename << endl;
        }
    }

    void writeLines(string filename, string arr[], int count) {
        ofstream fout(filename, ios::trunc);

        if (fout.is_open()) {
            for (int i = 0; i < count; i++) {
                fout << arr[i] << endl;
            }
            fout.close();
        }
    }

    //  Logic Helpers 

    string genId() {
        return to_string(myRand() % 900 + 100);
    }

    string genCard() {
        string s = "";

        for (int i = 0; i < 16; i++) {
            s += to_string(myRand() % 10);
        }

        return s;
    }

    bool isIdUnique(string id) {
        string lines[MAX_LINES], parts[MAX_TOKENS];

        int count;
        count = readLines(F_USER, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, MAX_TOKENS);

            if (parts[0] == id) {
                return false;
            }
        }
        count = readLines(F_PENDING, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, MAX_TOKENS);

            if (parts[0] == id) {
                return false;
            }
        }

        count = readLines(F_COMP, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, MAX_TOKENS);

            if (parts[0] == id) {
                return false;
            }
        }

        count = readLines(F_PENDING_COMP, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, MAX_TOKENS);

            if (parts[0] == id) {
                return false;
            }
        }
        return true;
    }

    bool isUsernameTaken(string u) {
        string lines[MAX_LINES], parts[MAX_TOKENS], t = cleanString(u);

        int count;
        count = readLines(F_USER, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            int p = split(lines[i], ',', parts, MAX_TOKENS);

            if (p > 4 && parts[4] == t) {
                return true;
            }
        }

        count = readLines(F_PENDING, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            int p = split(lines[i], ',', parts, MAX_TOKENS);

            if (p > 4 && parts[4] == t) {
                return true;
            }
        }

        count = readLines(F_COMP, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            int p = split(lines[i], ',', parts, MAX_TOKENS);

            if (p > 4 && parts[4] == t) {
                return true;
            }
        }

        count = readLines(F_PENDING_COMP, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            int p = split(lines[i], ',', parts, MAX_TOKENS);

            if (p > 4 && parts[4] == t) {
                return true;
            }
        }
        return false;
    }

    void logTransaction(string uid, double amt, string type, string targetId = "") {
        string log = getCurrentDate() + ", " + uid + ", " + to_string((long long)amt) + ", " + type;

        if (targetId != "") {
            log += ", " + targetId;
        }
        appendLine(F_TRANS, log);
    }

    double getLimitForCategory(string cat) {
        if (cat == "Business") {
            return 100000000000.0;
        }

        if (cat == "Gold") {
            return 1000000000.0;
        }

        return 100000000.0;
    }

    bool isAccountFrozen(string uid) {
        string lines[MAX_LINES], parts[5];
        int count = readLines(F_FREEZE_ACC, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, 5);

            if (parts[0] == uid) {
                return true;
            }
        }

        return false;
    }

    bool isCardFrozen(string cardNum) {
        string lines[MAX_LINES], parts[5];
        int count = readLines(F_FREEZE_CARD, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, 5);

            if (parts[0] == cardNum) {
                return true;
            }
        }

        return false;
    }

    bool isAccountSuspicious(string uid) {
        string lines[MAX_LINES], parts[5];
        int count = readLines(F_SUSP_ACC, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, 5);

            if (parts[0] == uid) {
                return true;
            }
        }

        return false;
    }

    void getAccountDetails(string uid, string& name, string& cnic) {
        string lines[MAX_LINES], parts[MAX_TOKENS];
        int count = readLines(F_USER, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, MAX_TOKENS);

            if (parts[0] == uid) {
                name = parts[1];
                cnic = parts[3];
                return;
            }
        }

        count = readLines(F_COMP, lines, MAX_LINES);

        for (int i = 0; i < count; i++) {
            split(lines[i], ',', parts, MAX_TOKENS);

            if (parts[0] == uid) {
                name = parts[1];
                cnic = parts[3];

                return;
            }
        }

        name = "Unknown";
        cnic = "N/A";
    }

    bool modifyBalance(string filename, string targetId, double amount, bool isDeposit) {
        string lines[MAX_LINES], parts[MAX_TOKENS];
        int count = readLines(filename, lines, MAX_LINES);
        bool success = false;
        string tId = cleanString(targetId);

        for (int i = 0; i < count; i++) {
            int pCount = split(lines[i], ',', parts, MAX_TOKENS);

            if (pCount > 7 && parts[0] == tId) {
                double curBal = strToDouble(parts[6]);
                string cat = cleanString(parts[7]);
                double limit = getBalanceLimitForCat(cat);

                if (isDeposit) {
                    double maxDeposit = limit - curBal;

                    if (amount > maxDeposit) {
                        cout << "\n[ERROR] Deposit Declined!\n";
                        cout << "Current Balance: " << (long long)curBal << " PKR\n";
                        cout << "Max Limit: " << (long long)limit << " PKR\n";
                        cout << "Capacity: " << (long long)maxDeposit << " PKR\n";

                        return false;
                    }

                    curBal += amount;
                    success = true;
                }

                else {
                    if (curBal < amount) {
                        cout << "\n[ERROR] Insufficient Funds.\n";

                        return false;
                    }

                    curBal -= amount;
                    success = true;
                }

                if (success) {
                    string newLine = "";

                    for (int j = 0; j < 6; j++) {
                        newLine += parts[j] + ", ";
                    }

                    newLine += to_string((long long)curBal) + ", " + parts[7];
                    lines[i] = newLine;
                }
            }
        }

        if (success) {
            writeLines(filename, lines, count);
        }

        return success;
    }

    double getBalanceLimitForCat(string cat) {
        if (cat == "Business") {
            return 100000000000.0;
        }

        if (cat == "Gold") {
            return 1000000000.0;
        }

        return 100000000.0;
    }

    void handleFraud(string uid, string cardNum) {
        appendLine(F_FREEZE_CARD, cardNum + ", " + uid);

        if (!isAccountSuspicious(uid)) {
            string name, cnic;
            getAccountDetails(uid, name, cnic);
            appendLine(F_SUSP_ACC, uid + ", " + name + ", " + cnic + ", Fraud detected");
        }
    }

public:
    BankSystem() {
        mySrand();
    }

    void initializeSystem() {
        string files[] = { F_USER,         F_PENDING,  F_COMP,
                          F_PENDING_COMP, F_EMP,      F_ADMIN,
                          F_CARDS,        F_TRANS,    F_FREEZE_ACC,
                          F_FREEZE_CARD,  F_SUSP_ACC, F_LOAN };

        for (int i = 0; i < 12; i++) {
            ifstream ifile(files[i]);

            if (!ifile.good()) {
                ofstream ofile(files[i]);
                ofile.close();

                if (files[i] == F_ADMIN) {
                    appendLine(F_ADMIN, "admin, 123");
                }
            }
            ifile.close();
        }
    }

    //   User Client

    void registerUser() {
        printTitle("USER REGISTRATION");
        showCategoryLimits();

        string name, addr, cnic, user, pass;
        double limit;
        int catChoice;
        string selectedCat;
        double maxLimitForCat;

        while (true) {
            cout << "Select Category (1. Bronze, 2. Gold, 3. Business): ";
            cin >> catChoice;
            if (catChoice == 3) {
                cout << "[ERROR] Business accounts are for Company Clients "
                    "only.\n";
            }
            else if (catChoice == 1) {
                selectedCat = "Bronze";
                maxLimitForCat = 100000.0;
                break;
            }
            else if (catChoice == 2) {
                selectedCat = "Gold";
                maxLimitForCat = 500000.0;
                break;
            }
            else {
                cout << "Invalid choice. Try again.\n";
            }
        }

        cout << "\nEnter Details:\n";
        cout << "Full Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Address: ";
        getline(cin, addr);
        while (true) {
            cout << "CNIC (13 digits): ";
            cin >> cnic;
            if (cnic.length() == 13 && isNumeric(cnic)) {
                break;
            }
            cout << "Invalid CNIC!\n";
        }
        do {
            cout << "Username: ";
            cin >> user;
            if (isUsernameTaken(user)) {
                cout << "Taken.\n";
            }
        } while (isUsernameTaken(user));
        cout << "Password: ";
        cin >> pass;

        while (true) {
            cout << "Daily Withdraw Limit (Max " << (long long)maxLimitForCat
                << "): ";
            cin >> limit;
            if (limit > 0 && limit <= maxLimitForCat) {
                break;
            }
            cout << "Invalid Limit.\n";
        }

        string id = genId();
        while (!isIdUnique(id)) {
            id = genId();
        }

        string line = id + ", " + name + ", " + addr + ", " + cnic + ", " +
            user + ", " + pass + ", 0, " + selectedCat;
        appendLine(F_PENDING, line);
        cout << "\n[SUCCESS] Application Submitted. Wait for Admin approval.\n";
    }

    void userMenu(string uid, string name, string catStr, string rawLine) {
        int choice;
        do {
            printTitle("USER DASHBOARD: " + name + " (" + catStr + ")");
            cout << "1. Deposit\n2. Withdraw\n3. Show Balance\n4. Transaction "
                "History\n5. Transfer Funds\n0. Logout\n";
            cout << "Select: ";
            cin >> choice;

            if (choice == 1) {
                double amt;
                cout << "Enter Amount (PKR): ";
                cin >> amt;
                if (amt > 0) {
                    if (modifyBalance(F_USER, uid, amt, true)) {
                        logTransaction(uid, amt, "Deposit");
                        cout << "Deposit Successful.\n";
                    }
                }
            }
            else if (choice == 2) {
                if (isAccountSuspicious(uid)) {
                    cout << "\n[!] ACCOUNT SUSPICIOUS. WITHDRAWAL BLOCKED.\n";
                    continue;
                }

                string cards[MAX_LINES], cParts[5];
                int cardCount = readLines(F_CARDS, cards, MAX_LINES);
                string myCards[5][2];
                int myCount = 0;
                string cleanUid = cleanString(uid);

                for (int i = 0; i < cardCount; i++) {
                    int pc = split(cards[i], ',', cParts, 5);
                    if (pc >= 3 && cParts[0] == cleanUid) {
                        if (!isCardFrozen(cParts[1])) {
                            myCards[myCount][0] = cParts[1];
                            myCards[myCount][1] = cParts[2];
                            myCount++;
                        }
                    }
                }

                if (myCount == 0) {
                    char c;
                    cout << "No active cards found. Request new card? (y/n): ";
                    cin >> c;
                    if (c == 'y' || c == 'Y') {
                        string newCard = genCard();
                        string newPin;
                        cout << "Generated Card: " << newCard << "\nSet PIN: ";
                        cin >> newPin;
                        appendLine(F_CARDS,
                            uid + ", " + newCard + ", " + newPin);
                        cout << "Card Created. You can now withdraw.\n";
                    }
                    continue;
                }

                int cIdx = 0;
                if (myCount > 1) {
                    cout << "Select Card:\n";
                    for (int i = 0; i < myCount; i++) {
                        cout << i + 1 << ". " << myCards[i][0] << endl;
                    }
                    cin >> cIdx;
                    cIdx--;
                }

                string pin;
                bool fraud = true;
                for (int i = 0; i < 3; i++) {
                    cout << "Enter PIN (" << 3 - i << " attempts): ";
                    cin >> pin;
                    if (cleanString(pin) == myCards[cIdx][1]) {
                        fraud = false;
                        break;
                    }
                }

                if (fraud) {
                    cout << "\n[!] FRAUD DETECTED! Card Frozen & Account "
                        "Suspicious.\n";
                    handleFraud(uid, myCards[cIdx][0]);
                }
                else {
                    double amt;
                    cout << "Enter Amount (PKR): ";
                    cin >> amt;
                    Account* temp =
                        new UserAccount(uid, "", "", "", "", "", 0, catStr);
                    if (amt > temp->getDailyLimit()) {
                        cout << "Exceeds Daily Limit of "
                            << (long long)temp->getDailyLimit() << " PKR\n";
                    }
                    else if (modifyBalance(F_USER, uid, amt, false)) {
                        logTransaction(uid, amt, "Withdraw");
                        cout << "Withdraw Successful.\n";
                    }
                    delete temp;
                }
            }
            else if (choice == 3) {
                string u[MAX_LINES], p[MAX_TOKENS];
                int c = readLines(F_USER, u, MAX_LINES);
                for (int i = 0; i < c; i++) {
                    split(u[i], ',', p, MAX_TOKENS);
                    if (p[0] == uid) {
                        cout << "\n*** Current Balance: "
                            << (long long)strToDouble(p[6]) << " PKR ***\n";
                    }
                }
            }
            else if (choice == 4) {
                string t[200], p[MAX_TOKENS];
                int c = readLines(F_TRANS, t, 200);
                printTitle("TRANSACTION HISTORY");
                cout << "DATE       | ID    | AMOUNT (PKR)    | TYPE" << endl;
                cout << "\n"
                    "\n"
                    << endl;
                bool found = false;
                for (int i = 0; i < c; i++) {
                    int k = split(t[i], ',', p, MAX_TOKENS);
                    if (p[1] == uid) {
                        cout << p[0] << " | " << p[1] << " | " << p[2]
                            << " PKR | " << p[3];
                        if (k > 4) {
                            cout << " (To: " << p[4] << ")";
                        }
                        cout << endl;
                        found = true;
                    }
                    else if (k > 4 && p[4] == uid) {
                        cout << p[0] << " | " << p[4] << " | " << p[2]
                            << " PKR | Received (From: " << p[1] << ")"
                            << endl;
                        found = true;
                    }
                }
                if (!found) {
                    cout << "(None)\n";
                }
            }
            else if (choice == 5) {
                int tType;
                cout << "1. User Client\n2. Company Client\nSelect: ";
                cin >> tType;
                string tId = "";
                string lines[MAX_LINES], parts[MAX_TOKENS];
                int count;

                if (tType == 1) {
                    string n, c;
                    cout << "Recipient Name: ";
                    cin.ignore();
                    getline(cin, n);
                    cout << "Recipient CNIC: ";
                    cin >> c;
                    count = readLines(F_USER, lines, MAX_LINES);
                    string cleanC = cleanString(c);
                    string cleanN = cleanString(n);
                    for (int i = 0; i < count; i++) {
                        int pc = split(lines[i], ',', parts, MAX_TOKENS);
                        if (pc > 3 && cleanString(parts[1]) == cleanN &&
                            cleanString(parts[3]) == cleanC) {
                            tId = parts[0];
                        }
                    }
                }
                else {
                    string n, tax;
                    cout << "Company Name: ";
                    cin.ignore();
                    getline(cin, n);
                    cout << "Tax Num: ";
                    cin >> tax;
                    count = readLines(F_COMP, lines, MAX_LINES);
                    string cleanT = cleanString(tax);
                    string cleanN = cleanString(n);
                    for (int i = 0; i < count; i++) {
                        int pc = split(lines[i], ',', parts, MAX_TOKENS);
                        if (pc > 3 && cleanString(parts[1]) == cleanN &&
                            cleanString(parts[3]) == cleanT) {
                            tId = parts[0];
                        }
                    }
                }

                if (tId != "") {
                    double amt;
                    cout << "Enter Amount (PKR): ";
                    cin >> amt;
                    if (modifyBalance(F_USER, uid, amt, false)) {
                        modifyBalance((tType == 1 ? F_USER : F_COMP), tId, amt,
                            true);
                        logTransaction(uid, amt, "Transfer", tId);
                        cout << "Transfer Successful.\n";
                    }
                }
                else {
                    cout << "Recipient Not Found.\n";
                }
            }
        } while (choice != 0);
    }

    void closeUserAccount(string uid) {
        char c;
        cout << "Are you sure? (y/n): ";
        cin >> c;
        if (c == 'y') {
            string all[MAX_LINES], keep[MAX_LINES], parts[MAX_TOKENS];
            int count = readLines(F_USER, all, MAX_LINES);
            int kCount = 0;
            for (int i = 0; i < count; i++) {
                split(all[i], ',', parts, MAX_TOKENS);
                if (parts[0] != uid) {
                    keep[kCount++] = all[i];
                }
            }
            writeLines(F_USER, keep, kCount);
            cout << "Account Closed.\n";
        }
    }

    //   Company client features

    void registerCompany() {
        printTitle("COMPANY REGISTRATION");
        showCategoryLimits();

        string name, addr, tax, user, pass;
        double limit;
        int catChoice;
        string selectedCat;
        double maxLimitForCat;

        while (true) {
            cout << "Select Category (1. Bronze, 2. Gold, 3. Business): ";
            cin >> catChoice;
            if (catChoice == 1) {
                selectedCat = "Bronze";
                maxLimitForCat = 100000.0;
                break;
            }
            else if (catChoice == 2) {
                selectedCat = "Gold";
                maxLimitForCat = 500000.0;
                break;
            }
            else if (catChoice == 3) {
                selectedCat = "Business";
                maxLimitForCat = 20000000.0;
                break;
            }
            else {
                cout << "Invalid choice.\n";
            }
        }

        cout << "\nEnter Details:\n";
        cout << "Comp Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Address: ";
        getline(cin, addr);
        while (true) {
            cout << "Tax (5 digits): ";
            cin >> tax;
            if (tax.length() == 5 && isNumeric(tax)) {
                break;
            }
            cout << "Invalid Tax.\n";
        }
        do {
            cout << "Username: ";
            cin >> user;
            if (isUsernameTaken(user)) {
                cout << "Taken.\n";
            }
        } while (isUsernameTaken(user));
        cout << "Password: ";
        cin >> pass;
        while (true) {
            cout << "Daily Limit (Max " << (long long)maxLimitForCat << "): ";
            cin >> limit;
            if (limit > 0 && limit <= maxLimitForCat) {
                break;
            }
            cout << "Invalid Limit.\n";
        }

        string id = genId();
        while (!isIdUnique(id)) {
            id = genId();
        }

        string line = id + ", " + name + ", " + addr + ", " + tax + ", " +
            user + ", " + pass + ", 0, " + selectedCat;
        appendLine(F_PENDING_COMP, line);
        cout << "\n[SUCCESS] Application Submitted. Wait for Admin approval.\n";
    }

    void companyMenu(string cid, string name, string catStr) {
        int choice;
        do {
            printTitle("COMPANY DASHBOARD: " + name);
            cout << "1. Deposit\n2. Withdraw\n3. Show Balance\n4. History\n5. "
                "Transfer\n6. Loan Request\n0. Logout\n";
            cout << "Select: ";
            cin >> choice;

            if (choice == 1) {
                double amt;
                cout << "Amount (PKR): ";
                cin >> amt;
                if (modifyBalance(F_COMP, cid, amt, true)) {
                    logTransaction(cid, amt, "Deposit");
                    cout << "Success.\n";
                }
            }
            else if (choice == 2) {
                if (isAccountSuspicious(cid)) {
                    cout << "Declined: Account Suspicious.\n";
                    continue;
                }

                // Company card check
                string cards[MAX_LINES], cParts[5];
                int cardCount = readLines(F_CARDS, cards, MAX_LINES);
                string myCards[5][2];
                int myCount = 0;
                string cleanUid = cleanString(cid);

                for (int i = 0; i < cardCount; i++) {
                    int pc = split(cards[i], ',', cParts, 5);
                    if (pc >= 3 && cParts[0] == cleanUid) {
                        if (!isCardFrozen(cParts[1])) {
                            myCards[myCount][0] = cParts[1];
                            myCards[myCount][1] = cParts[2];
                            myCount++;
                        }
                    }
                }

                if (myCount == 0) {
                    char c;
                    cout << "No active cards found. Request new card? (y/n): ";
                    cin >> c;
                    if (c == 'y' || c == 'Y') {
                        string newCard = genCard();
                        string newPin;
                        cout << "Generated Card: " << newCard << "\nSet PIN: ";
                        cin >> newPin;
                        appendLine(F_CARDS,
                            cid + ", " + newCard + ", " + newPin);
                        cout << "Card Created. You can now withdraw.\n";
                    }
                    continue;
                }

                int cIdx = 0;
                if (myCount > 1) {
                    cout << "Select Card:\n";
                    for (int i = 0; i < myCount; i++) {
                        cout << i + 1 << ". " << myCards[i][0] << endl;
                    }
                    cin >> cIdx;
                    cIdx--;
                }

                string pin;
                bool fraud = true;
                for (int i = 0; i < 3; i++) {
                    cout << "Enter PIN (" << 3 - i << " attempts): ";
                    cin >> pin;
                    if (cleanString(pin) == myCards[cIdx][1]) {
                        fraud = false;
                        break;
                    }
                }

                if (fraud) {
                    cout << "\n[!] FRAUD DETECTED! Card Frozen & Account "
                        "Suspicious.\n";
                    handleFraud(cid, myCards[cIdx][0]);
                }
                else {
                    double amt;
                    cout << "Amount (PKR): ";
                    cin >> amt;
                    if (modifyBalance(F_COMP, cid, amt, false)) {
                        logTransaction(cid, amt, "Withdraw");
                        cout << "Success.\n";
                    }
                }
            }
            else if (choice == 3) {
                string c[MAX_LINES], p[MAX_TOKENS];
                int cnt = readLines(F_COMP, c, MAX_LINES);
                for (int i = 0; i < cnt; i++) {
                    split(c[i], ',', p, MAX_TOKENS);
                    if (p[0] == cid) {
                        cout << "\n*** Bal: " << (long long)strToDouble(p[6])
                            << " PKR ***\n";
                    }
                }
            }
            else if (choice == 4) {
                // Company history
                string t[200], p[MAX_TOKENS];
                int c = readLines(F_TRANS, t, 200);
                printTitle("TRANSACTION HISTORY");
                cout << "DATE       | ID    | AMOUNT (PKR)    | TYPE" << endl;
                cout << "\n"
                    "\n"
                    << endl;
                bool found = false;
                for (int i = 0; i < c; i++) {
                    int k = split(t[i], ',', p, MAX_TOKENS);
                    if (p[1] == cid) {
                        cout << p[0] << " | " << p[1] << " | " << p[2]
                            << " PKR | " << p[3];
                        if (k > 4) {
                            cout << " (To: " << p[4] << ")";
                        }
                        cout << endl;
                        found = true;
                    }
                    else if (k > 4 && p[4] == cid) {
                        cout << p[0] << " | " << p[4] << " | " << p[2]
                            << " PKR | Received (From: " << p[1] << ")"
                            << endl;
                        found = true;
                    }
                }
                if (!found) {
                    cout << "(None)\n";
                }
            }
            else if (choice == 5) {
                // Company Transfer
                int tType;
                cout << "1. User Client\n2. Company Client\nSelect: ";
                cin >> tType;
                string tId = "";
                string lines[MAX_LINES], parts[MAX_TOKENS];
                int count;

                if (tType == 1) {
                    string n, c;
                    cout << "Recipient Name: ";
                    cin.ignore();
                    getline(cin, n);
                    cout << "Recipient CNIC: ";
                    cin >> c;
                    count = readLines(F_USER, lines, MAX_LINES);
                    string cleanC = cleanString(c);
                    string cleanN = cleanString(n);
                    for (int i = 0; i < count; i++) {
                        int pc = split(lines[i], ',', parts, MAX_TOKENS);
                        if (pc > 3 && cleanString(parts[1]) == cleanN &&
                            cleanString(parts[3]) == cleanC) {
                            tId = parts[0];
                        }
                    }
                }
                else {
                    string n, tax;
                    cout << "Company Name: ";
                    cin.ignore();
                    getline(cin, n);
                    cout << "Tax Num: ";
                    cin >> tax;
                    count = readLines(F_COMP, lines, MAX_LINES);
                    string cleanT = cleanString(tax);
                    string cleanN = cleanString(n);
                    for (int i = 0; i < count; i++) {
                        int pc = split(lines[i], ',', parts, MAX_TOKENS);
                        if (pc > 3 && cleanString(parts[1]) == cleanN &&
                            cleanString(parts[3]) == cleanT) {
                            tId = parts[0];
                        }
                    }
                }

                if (tId != "") {
                    double amt;
                    cout << "Enter Amount (PKR): ";
                    cin >> amt;
                    if (modifyBalance(F_COMP, cid, amt, false)) {
                        modifyBalance((tType == 1 ? F_USER : F_COMP), tId, amt,
                            true);
                        logTransaction(cid, amt, "Transfer", tId);
                        cout << "Transfer Successful.\n";
                    }
                }
                else {
                    cout << "Recipient Not Found.\n";
                }
            }
            else if (choice == 6) {
                double amt;
                cout << "Loan Amt (PKR): ";
                cin >> amt;
                string loan =
                    cid + ", " + name + ", " + to_string((long long)amt);
                appendLine(F_LOAN, loan);
                cout << "Loan Requested.\n";
            }
        } while (choice != 0);
    }

    //    Admin Features

    void adminMenu() {
        string u, p;
        printTitle("ADMIN LOGIN");
        cout << "Admin User: ";
        cin >> u;
        cout << "Admin Pass: ";
        cin >> p;
        string ads[5], parts[5];
        int c = readLines(F_ADMIN, ads, 5);
        bool auth = false;
        for (int i = 0; i < c; i++) {
            split(ads[i], ',', parts, 5);
            if (parts[0] == cleanString(u) && parts[1] == cleanString(p)) {
                auth = true;
            }
        }
        if (!auth) {
            cout << "Invalid Admin.\n";
            return;
        }

        int choice;
        do {
            printTitle("ADMIN PANEL");
            cout << "1. View Accounts\n2. New Acc Requests\n3. Loan "
                "Requests\n4. Transaction History\n";
            cout << "5. Freeze Account\n6. Unfreeze Account/Card\n7. Remove "
                "Account/Card\n0. Logout\n";
            cout << "Select: ";
            cin >> choice;

            if (choice == 1) {
                string lines[MAX_LINES], p[MAX_TOKENS];
                cout << "\n--- USERS ---\n";
                cout << "ID  | NAME           | CNIC           | BALANCE       "
                    "| TYPE\n"
                    << endl;
                int cnt = readLines(F_USER, lines, MAX_LINES);
                if (cnt == 0) {
                    cout << "(None)\n";
                }
                for (int i = 0; i < cnt; i++) {
                    split(lines[i], ',', p, MAX_TOKENS);
                    cout << p[0] << " | " << p[1] << " | " << p[3] << " | "
                        << p[6] << " PKR | " << p[7] << endl;
                }
                cout << "\n--- COMPANIES ---\n";
                cout << "ID  | COMPANY        | TAX ID         | BALANCE       "
                    "| TYPE\n"
                    << endl;
                cnt = readLines(F_COMP, lines, MAX_LINES);
                if (cnt == 0) {
                    cout << "(None)\n";
                }
                for (int i = 0; i < cnt; i++) {
                    split(lines[i], ',', p, MAX_TOKENS);
                    cout << p[0] << " | " << p[1] << " | " << p[3] << " | "
                        << p[6] << " PKR | " << p[7] << endl;
                }
            }
            else if (choice == 2) {
                int rType;
                cout << "1. User Requests\n2. Company Requests\nSelect: ";
                cin >> rType;

                string pend[MAX_LINES], p[MAX_TOKENS];
                int cnt;
                string fileName;

                if (rType == 1) {
                    fileName = F_PENDING;
                    printTitle("NEW USER REQUESTS");
                }
                else {
                    fileName = F_PENDING_COMP;
                    printTitle("NEW COMPANY REQUESTS");
                }

                cnt = readLines(fileName, pend, MAX_LINES);
                if (cnt == 0) {
                    cout << "(None)\n";
                }
                else {
                    cout << "NO. | ID  | NAME           | CNIC/TAX       | TYPE"
                        << endl;
                    cout << "\n"
                        "\n"
                        << endl;
                    for (int i = 0; i < cnt; i++) {
                        split(pend[i], ',', p, MAX_TOKENS);
                        cout << i + 1 << "   | " << p[0] << " | " << p[1]
                            << " | " << p[3] << " | " << p[7] << endl;
                    }
                    int idx;
                    cout << "\nSelect Request (0 back): ";
                    cin >> idx;
                    if (idx > 0 && idx <= cnt) {
                        int act;
                        cout << "1. Accept\n2. Reject\nSelect: ";
                        cin >> act;
                        if (act == 1) {
                            string targetFile =
                                (rType == 1) ? F_USER : F_COMP;
                            appendLine(targetFile, pend[idx - 1]);

                            string parts[MAX_TOKENS];
                            split(pend[idx - 1], ',', parts, MAX_TOKENS);
                            string card = genCard();
                            string pin;
                            cout << "Generated Card: " << card
                                << "\nEnter PIN: ";
                            cin >> pin;
                            appendLine(F_CARDS, parts[0] + ", " + card + ", " +
                                pin);
                            cout << "Approved.\n";
                        }
                        string newP[MAX_LINES];
                        int newCount = 0;
                        for (int i = 0; i < cnt; i++) {
                            if (i != idx - 1) {
                                newP[newCount++] = pend[i];
                            }
                        }
                        writeLines(fileName, newP, newCount);
                    }
                }
            }
            else if (choice == 3) {
                // Loan requests
                string loans[MAX_LINES], p[MAX_TOKENS];
                int cnt = readLines(F_LOAN, loans, MAX_LINES);
                printTitle("LOAN REQUESTS");
                if (cnt == 0) {
                    cout << "(None)\n";
                }
                else {
                    cout << "NO. | COMP ID | NAME   | AMOUNT (PKR)" << endl;
                    cout << "\n"
                        << endl;
                    for (int i = 0; i < cnt; i++) {
                        split(loans[i], ',', p, MAX_TOKENS);
                        cout << i + 1 << "   | " << p[0] << "     | " << p[1]
                            << " | " << p[2] << endl;
                    }
                    int idx;
                    cout << "\nSelect Loan (0 back): ";
                    cin >> idx;
                    if (idx > 0 && idx <= cnt) {
                        int act;
                        cout << "1. Approve\n2. Reject\nSelect: ";
                        cin >> act;
                        if (act == 1) {
                            split(loans[idx - 1], ',', p, MAX_TOKENS);
                            modifyBalance(F_COMP, p[0], strToDouble(p[2]),
                                true);
                            cout << "Loan Approved & Deposited.\n";
                        }
                        string newL[MAX_LINES];
                        int nc = 0;
                        for (int i = 0; i < cnt; i++) {
                            if (i != idx - 1) {
                                newL[nc++] = loans[i];
                            }
                        }
                        writeLines(F_LOAN, newL, nc);
                    }
                }
            }
            else if (choice == 4) {
                // Admin transaction histroy
                string lines[MAX_LINES], p[MAX_TOKENS];
                string tempIDs[MAX_LINES];
                int displayCount = 0;

                cout << "\n--- SELECT ACCOUNT TO VIEW HISTORY ---\n";
                cout << "NO. | ID  | NAME           | TYPE" << endl;
                cout << "\n" << endl;

                int cnt = readLines(F_USER, lines, MAX_LINES);
                for (int i = 0; i < cnt; i++) {
                    split(lines[i], ',', p, MAX_TOKENS);
                    cout << ++displayCount << "   | " << p[0] << " | " << p[1]
                        << " | User" << endl;
                    tempIDs[displayCount - 1] = p[0];
                }
                cnt = readLines(F_COMP, lines, MAX_LINES);
                for (int i = 0; i < cnt; i++) {
                    split(lines[i], ',', p, MAX_TOKENS);
                    cout << ++displayCount << "   | " << p[0] << " | " << p[1]
                        << " | Comp" << endl;
                    tempIDs[displayCount - 1] = p[0];
                }

                int sel;
                cout << "\nSelect Number (0 back): ";
                cin >> sel;
                if (sel > 0 && sel <= displayCount) {
                    string tid = tempIDs[sel - 1];
                    string t[200], tp[MAX_TOKENS];
                    int c = readLines(F_TRANS, t, 200);
                    printTitle("HISTORY FOR ID: " + tid);
                    cout << "DATE       | AMOUNT (PKR)    | TYPE" << endl;
                    cout << "\n" << endl;
                    bool found = false;
                    for (int i = 0; i < c; i++) {
                        int k = split(t[i], ',', tp, MAX_TOKENS);
                        if (cleanString(tp[1]) == cleanString(tid)) {
                            cout << tp[0] << " | " << tp[2] << " PKR | "
                                << tp[3];
                            if (k > 4) {
                                cout << " (To: " << tp[4] << ")";
                            }
                            cout << endl;
                            found = true;
                        }
                        else if (k > 4 &&
                            cleanString(tp[4]) == cleanString(tid)) {
                            cout << tp[0] << " | " << tp[2]
                                << " PKR | Received (From: " << tp[1] << ")"
                                << endl;
                            found = true;
                        }
                    }
                    if (!found) {
                        cout << "(None)\n";
                    }
                }
            }
            else if (choice == 5) {
                // Freeze Account
                string susp[MAX_LINES], parts[5];
                int scnt = readLines(F_SUSP_ACC, susp, MAX_LINES);
                printTitle("SUSPICIOUS ACCOUNTS (Available to Freeze)");
                if (scnt == 0) {
                    cout << "(None)\n";
                }
                else {
                    cout << "NO. | ID  | NAME           | CNIC/TAX       | "
                        "REASON"
                        << endl;
                    cout << "\n"
                        "\n"
                        << endl;
                    for (int i = 0; i < scnt; i++) {
                        split(susp[i], ',', parts, 5);
                        cout << i + 1 << "   | " << parts[0] << " | "
                            << parts[1] << " | " << parts[2] << " | "
                            << parts[3] << endl;
                    }

                    int sel;
                    cout << "\nSelect ID to Freeze (0 back): ";
                    cin >> sel;
                    if (sel > 0 && sel <= scnt) {
                        split(susp[sel - 1], ',', parts, 5);
                        string uid = parts[0];
                        string name = parts[1];
                        string cnic = parts[2];

                        appendLine(F_FREEZE_ACC, uid + ", " + name + ", " +
                            cnic +
                            ", Frozen by Admin");
                        // Freeze all cards
                        string cards[MAX_LINES], cp[5];
                        int cc = readLines(F_CARDS, cards, MAX_LINES);
                        for (int i = 0; i < cc; i++) {
                            split(cards[i], ',', cp, 5);
                            if (cp[0] == uid) {
                                appendLine(F_FREEZE_CARD, cp[1] + ", " + uid);
                            }
                        }
                        // Remove from Suspicious
                        string ns[MAX_LINES];
                        int nsc = 0;
                        for (int i = 0; i < scnt; i++) {
                            if (i != sel - 1) {
                                ns[nsc++] = susp[i];
                            }
                        }
                        writeLines(F_SUSP_ACC, ns, nsc);
                        cout << "Account and associated cards frozen.\n";
                    }
                }
            }
            else if (choice == 6) {
                // Unfreeze 
                int type;
                cout << "1. Unfreeze Account\n2. Unfreeze Card\nSelect: ";
                cin >> type;
                if (type == 1) {
                    string fr[MAX_LINES], p[5];
                    int cnt = readLines(F_FREEZE_ACC, fr, MAX_LINES);
                    printTitle("FROZEN ACCOUNTS");
                    if (cnt == 0) {
                        cout << "(None)\n";
                    }
                    else {
                        cout << "NO. | ID  | NAME           | CNIC/TAX       | "
                            "REASON"
                            << endl;
                        cout << "\n"
                            "\n"
                            << endl;
                        for (int i = 0; i < cnt; i++) {
                            split(fr[i], ',', p, 5);
                            cout << i + 1 << "   | " << p[0] << " | " << p[1]
                                << " | " << p[2] << " | " << p[3] << endl;
                        }

                        int sel;
                        cout << "\nSelect to Unfreeze (0 back): ";
                        cin >> sel;
                        if (sel > 0 && sel <= cnt) {
                            split(fr[sel - 1], ',', p, 5);
                            string uid = p[0];
                            string nf[MAX_LINES];
                            int nfc = 0;
                            for (int i = 0; i < cnt; i++) {
                                if (i != sel - 1) {
                                    nf[nfc++] = fr[i];
                                }
                            }
                            writeLines(F_FREEZE_ACC, nf, nfc);

                            // Unfreeze cards
                            string fc[MAX_LINES], fcp[5], nfcards[MAX_LINES];
                            int fcc =
                                readLines(F_FREEZE_CARD, fc, MAX_LINES),
                                nfcc = 0;
                            for (int i = 0; i < fcc; i++) {
                                split(fc[i], ',', fcp, 5);
                                if (fcp[1] != uid) {
                                    nfcards[nfcc++] = fc[i];
                                }
                            }
                            writeLines(F_FREEZE_CARD, nfcards, nfcc);
                            cout << "Account and Cards Unfrozen.\n";
                        }
                    }
                }
                else if (type == 2) {
                    string fc[MAX_LINES], p[5];
                    int cnt = readLines(F_FREEZE_CARD, fc, MAX_LINES);
                    printTitle("FROZEN CARDS");
                    if (cnt == 0) {
                        cout << "(None)\n";
                    }
                    else {
                        cout << "NO. | CARD NUMBER      | USER ID" << endl;
                        cout << "\n"
                            << endl;
                        for (int i = 0; i < cnt; i++) {
                            split(fc[i], ',', p, 5);
                            cout << i + 1 << "   | " << p[0] << " | " << p[1]
                                << endl;
                        }
                        int sel;
                        cout << "\nSelect to Unfreeze (0 back): ";
                        cin >> sel;
                        if (sel > 0 && sel <= cnt) {
                            split(fc[sel - 1], ',', p, 5);
                            string uid = p[1];
                            if (isAccountFrozen(uid)) {
                                cout << "[ERROR] Cannot unfreeze card. The "
                                    "Account ("
                                    << uid << ") is frozen.\n";
                            }
                            else {
                                string nfc[MAX_LINES];
                                int nfcc = 0;
                                for (int i = 0; i < cnt; i++) {
                                    if (i != sel - 1) {
                                        nfc[nfcc++] = fc[i];
                                    }
                                }
                                writeLines(F_FREEZE_CARD, nfc, nfcc);
                                cout << "Card Unfrozen.\n";
                            }
                        }
                    }
                }
            }
            else if (choice == 7) {
                // Remove
                int type;
                cout << "1. Remove Account\n2. Remove Card\nSelect: ";
                cin >> type;
                if (type == 1) {
                    string s[MAX_LINES], f[MAX_LINES], combined[MAX_LINES * 2],
                        p[5];
                    int sc = readLines(F_SUSP_ACC, s, MAX_LINES);
                    int fc = readLines(F_FREEZE_ACC, f, MAX_LINES);
                    int total = 0;
                    string combinedIDs[MAX_LINES * 2];

                    printTitle(
                        "ACCOUNTS AVAILABLE FOR REMOVAL (Suspicious/Frozen)");
                    if (sc == 0 && fc == 0) {
                        cout << "(None)\n";
                    }
                    else {
                        cout << "NO. | ID  | NAME           | STATUS" << endl;
                        cout << "\n"
                            << endl;
                        for (int i = 0; i < sc; i++) {
                            split(s[i], ',', p, 5);
                            combined[total] = s[i];
                            combinedIDs[total] = p[0];
                            cout << total + 1 << "   | " << p[0] << " | "
                                << p[1] << " | Suspicious" << endl;
                            total++;
                        }
                        for (int i = 0; i < fc; i++) {
                            split(f[i], ',', p, 5);
                            combined[total] = f[i];
                            combinedIDs[total] = p[0];
                            cout << total + 1 << "   | " << p[0] << " | "
                                << p[1] << " | Frozen" << endl;
                            total++;
                        }

                        int sel;
                        cout << "\nSelect Account to Remove (0 back): ";
                        cin >> sel;
                        if (sel > 0 && sel <= total) {
                            string uid = combinedIDs[sel - 1];
                            string lines[MAX_LINES], p[15], newL[MAX_LINES];
                            int cnt, ncnt = 0;

                            cnt = readLines(F_USER, lines, MAX_LINES);
                            ncnt = 0;
                            for (int i = 0; i < cnt; i++) {
                                split(lines[i], ',', p, 15);
                                if (p[0] != uid) {
                                    newL[ncnt++] = lines[i];
                                }
                            }
                            writeLines(F_USER, newL, ncnt);

                            cnt = readLines(F_COMP, lines, MAX_LINES);
                            ncnt = 0;
                            for (int i = 0; i < cnt; i++) {
                                split(lines[i], ',', p, 15);
                                if (p[0] != uid) {
                                    newL[ncnt++] = lines[i];
                                }
                            }
                            writeLines(F_COMP, newL, ncnt);

                            cnt = readLines(F_CARDS, lines, MAX_LINES);
                            ncnt = 0;
                            for (int i = 0; i < cnt; i++) {
                                split(lines[i], ',', p, 15);
                                if (p[0] != uid) {
                                    newL[ncnt++] = lines[i];
                                }
                            }
                            writeLines(F_CARDS, newL, ncnt);

                            cnt = readLines(F_FREEZE_ACC, lines, MAX_LINES);
                            ncnt = 0;
                            for (int i = 0; i < cnt; i++) {
                                split(lines[i], ',', p, 15);
                                if (p[0] != uid) {
                                    newL[ncnt++] = lines[i];
                                }
                            }
                            writeLines(F_FREEZE_ACC, newL, ncnt);

                            cnt = readLines(F_SUSP_ACC, lines, MAX_LINES);
                            ncnt = 0;
                            for (int i = 0; i < cnt; i++) {
                                split(lines[i], ',', p, 15);
                                if (p[0] != uid) {
                                    newL[ncnt++] = lines[i];
                                }
                            }
                            writeLines(F_SUSP_ACC, newL, ncnt);

                            // Remove from freeze_cards as well
                            cnt = readLines(F_FREEZE_CARD, lines, MAX_LINES);
                            ncnt = 0;
                            for (int i = 0; i < cnt; i++) {
                                split(lines[i], ',', p, 5);
                                if (p[1] != uid) {
                                    newL[ncnt++] = lines[i];
                                }
                            }
                            writeLines(F_FREEZE_CARD, newL, ncnt);

                            cout << "Account Removed.\n";
                        }
                    }
                }
                else if (type == 2) {
                    string fc[MAX_LINES], p[5];
                    int cnt = readLines(F_FREEZE_CARD, fc, MAX_LINES);
                    printTitle("FROZEN CARDS (Available to Remove)");
                    if (cnt == 0) {
                        cout << "(None)\n";
                    }
                    else {
                        cout << "NO. | CARD NUMBER      | USER ID" << endl;
                        cout << "\n"
                            << endl;
                        for (int i = 0; i < cnt; i++) {
                            split(fc[i], ',', p, 5);
                            cout << i + 1 << "   | " << p[0] << " | " << p[1]
                                << endl;
                        }
                        int sel;
                        cout << "\nSelect Card to Remove (0 back): ";
                        cin >> sel;
                        if (sel > 0 && sel <= cnt) {
                            split(fc[sel - 1], ',', p, 5);
                            string cardNum = p[0];
                            string c[MAX_LINES], cp[5], nc[MAX_LINES];
                            int cc = readLines(F_CARDS, c, MAX_LINES), ncc = 0;
                            for (int i = 0; i < cc; i++) {
                                split(c[i], ',', cp, 5);
                                if (cp[1] != cardNum) {
                                    nc[ncc++] = c[i];
                                }
                            }
                            writeLines(F_CARDS, nc, ncc);

                            string nfc[MAX_LINES];
                            int nfcc = 0;
                            for (int i = 0; i < cnt; i++) {
                                if (i != sel - 1) {
                                    nfc[nfcc++] = fc[i];
                                }
                            }
                            writeLines(F_FREEZE_CARD, nfc, nfcc);
                            cout << "Card Removed.\n";
                        }
                    }
                }
            }
        } while (choice != 0);
    }

    //   Entry Handlers

    void loginUser() {
        string u, p;
        printTitle("USER LOGIN");
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        string us[MAX_LINES], parts[MAX_TOKENS];
        int uc = readLines(F_USER, us, MAX_LINES);
        for (int i = 0; i < uc; i++) {
            int pc = split(us[i], ',', parts, MAX_TOKENS);
            if (pc > 5 && parts[4] == cleanString(u) &&
                parts[5] == cleanString(p)) {
                if (isAccountFrozen(parts[0])) {
                    cout << "\n[!] ACCOUNT FROZEN. Contact Admin.\n";
                    return;
                }
                userMenu(parts[0], parts[1], parts[7], us[i]);
                return;
            }
        }
        cout << "Invalid Username or Password.\n";
    }

    void loginCompany() {
        string eid;
        printTitle("COMPANY LOGIN");
        cout << "Emp ID: ";
        cin >> eid;
        string cleanEid = cleanString(eid);
        string es[MAX_LINES], parts[MAX_TOKENS];
        int esc = readLines(F_EMP, es, MAX_LINES);
        string cid = "";
        for (int i = 0; i < esc; i++) {
            split(es[i], ',', parts, MAX_TOKENS);
            if (parts[0] == cleanString(eid)) {
                cid = parts[1];
            }
        }
        if (cid == "") {
            cout << "Not a Company Employee.\n";
            return;
        }

        string u, p;
        cout << "Company User: ";
        cin >> u;
        cout << "Company Pass: ";
        cin >> p;
        if (isAccountFrozen(cid)) {
            cout << "\n[!] COMPANY ACCOUNT FROZEN.\n";
            return;
        }

        string cs[MAX_LINES];
        int csc = readLines(F_COMP, cs, MAX_LINES);
        for (int i = 0; i < csc; i++) {
            split(cs[i], ',', parts, MAX_TOKENS);
            if (parts[0] == cid && parts[4] == cleanString(u) &&
                parts[5] == cleanString(p)) {
                companyMenu(parts[0], parts[1], parts[7]);
                return;
            }
        }
        cout << "Invalid Credentials.\n";
    }

    void mainMenu() {
        int c;
        do {
            printTitle("MAIN MENU");
            cout << "1. User Client\n2. Company Client\n3. Banking "
                "Employee\n0. Exit\n";
            cout << "Select Account Type: ";
            cin >> c;

            if (c == 1) {
                int c2;
                printTitle("USER CLIENT MENU");
                cout << "1. Create New Account\n2. Access Existing Account\n0. "
                    "Back\nSelect: ";
                cin >> c2;
                if (c2 == 1) {
                    registerUser();
                }
                else if (c2 == 2) {
                    loginUser();
                }
            }
            else if (c == 2) {
                int c2;
                printTitle("COMPANY CLIENT MENU");
                cout << "1. Create New Account\n2. Access Existing Account\n0. "
                    "Back\nSelect: ";
                cin >> c2;
                if (c2 == 1) {
                    registerCompany();
                }
                else if (c2 == 2) {
                    loginCompany();
                }
            }
            else if (c == 3) {
                adminMenu();
            }

        } while (c != 0);
        cout << "Exiting System. Goodbye!\n";
    }
};

int main() {
    BankSystem bs;
    bs.initializeSystem();
    bs.mainMenu();
    return 0;
}
