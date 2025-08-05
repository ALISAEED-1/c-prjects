#include <iostream>
#include <string>
#include <fstream>  
#include <cstring>   

using namespace std;

class totalAmount {
protected:
    int accountNum;
    int amount;
    int day, month, year;
    string type;

public:
    totalAmount() {
        amount = day = month = year = 0;
        type = "";
    }

    totalAmount(int a, int d, int m, int y, string t, int acc) {
        amount = a;
        day = d;
        month = m;
        year = y;
        type = t;
        accountNum = acc;
    }

    virtual void display() {
        cout << amount << " | " << day << "/" << month << "/" << year << endl;
    }

    string getType() const { return type; }
    int getAmount() const { return amount; }
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    int getAccountNum() const { return accountNum; }

    virtual ~totalAmount() {}
};

class Income : public totalAmount {
    string source;
public:
    Income(int a, int d, int m, int y, string s, int acc)
        : totalAmount(a, d, m, y, "Income", acc), source(s) {}

    void display() override {
        cout << "[Income] " << amount << " | " << day << "/" << month << "/" << year
             << " | Source: " << source << endl;
    }
    void save(ofstream& fout) {
    fout.write((char*)&amount, sizeof(int));
    fout.write((char*)&day, sizeof(int));
    fout.write((char*)&month, sizeof(int));
    fout.write((char*)&year, sizeof(int));
    fout.write((char*)&accountNum, sizeof(int));
    
    int len = source.length();
    fout.write((char*)&len, sizeof(int));
    fout.write(source.c_str(), len);
}


    string getSource() const { return source; }
};

class Expense : public totalAmount {
    string category;
public:
    Expense(int a, int d, int m, int y, string c, int acc)
        : totalAmount(a, d, m, y, "Expense", acc), category(c) {}

    void display() override {
        cout << "[Expense] " << amount << " | " << day << "/" << month << "/" << year
             << " | Category: " << category << endl;
    }

	void save(ofstream& fout) {
    fout.write((char*)&amount, sizeof(int));
    fout.write((char*)&day, sizeof(int));
    fout.write((char*)&month, sizeof(int));
    fout.write((char*)&year, sizeof(int));
    fout.write((char*)&accountNum, sizeof(int));
    
    int len = category.length();
    fout.write((char*)&len, sizeof(int));
    fout.write(category.c_str(), len);
}

	
    string getCategory() const { return category; }
};

class User {
private:
    int accountNum;
    int pin;
    char name[50];

public:
    User() {
        accountNum = 0;
        pin = 0;
        strcpy(name, "");
    }

    void setData(int a, int p, const char* n) {
        accountNum = a;
        pin = p;
        strcpy(name, n);
    }

    bool login(int a, int p) {
        return accountNum == a && pin == p;
    }

    void display() {
        cout << "Account Number: " << accountNum << ", Name: " << name << endl;
    }

    void updateDetails(int Newacc, int newPin, const char* newName) {
        accountNum = Newacc;
        pin = newPin;
        strcpy(name, newName);
    }

    int getAccountNum() const { return accountNum; }
    int getPin() const{ return pin; }
    const char* getName() const{ return name; }
};


class AccountManager {
private:
    totalAmount** totalAmountList;  // renamed variable for clarity
    User* users;
    int size;
    int totalAmountCount;
    struct SimpleUserData {
    int accountNum;
    int pin;
    char name[50];
};

	
bool isLeap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isValidDate(int d, int m, int y) {
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;

    int daysInMonth[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
    if (m == 2 && isLeap(y)) {
        return d >= 1 && d <= 29;
    }

    return d >= 1 && d <= daysInMonth[m];
}



public:
    AccountManager() {
        totalAmountList = nullptr;
        users = nullptr;
        size = 0;
        totalAmountCount = 0;
    }

    ~AccountManager() {
        for (int i = 0; i < totalAmountCount; i++) {
            delete totalAmountList[i];
        }
        delete[] totalAmountList;
        delete[] users;
    }

	void loadTransactionsFromFile() {
    ifstream fin("fulldata.dat", ios::binary);
    if (!fin) return;

    while (true) {
        char type[10];
        fin.read(type, sizeof(type));
        type[9] = '\0';  // null-terminate just in case
		

        if (fin.eof()) break;

        if (strcmp(type, "Income") == 0) {
            int amt, d, m, y, acc, len;
            char buffer[100];

            fin.read((char*)&amt, sizeof(int));
            fin.read((char*)&d, sizeof(int));
            fin.read((char*)&m, sizeof(int));
            fin.read((char*)&y, sizeof(int));
            fin.read((char*)&acc, sizeof(int));
            fin.read((char*)&len, sizeof(int));
            fin.read(buffer, len);
            buffer[len] = '\0';

            Income* income = new Income(amt, d, m, y, buffer, acc);

            totalAmount** temp = new totalAmount*[totalAmountCount + 1];
            for (int i = 0; i < totalAmountCount; i++)
                temp[i] = totalAmountList[i];
            temp[totalAmountCount++] = income;

            delete[] totalAmountList;
            totalAmountList = temp;
        } 
        
        else if (strcmp(type, "Expense") == 0) {
            int amt, d, m, y, acc, len;
            char buffer[100];

            fin.read((char*)&amt, sizeof(int));
            fin.read((char*)&d, sizeof(int));
            fin.read((char*)&m, sizeof(int));
            fin.read((char*)&y, sizeof(int));
            fin.read((char*)&acc, sizeof(int));
            fin.read((char*)&len, sizeof(int));
            fin.read(buffer, len);
            buffer[len] = '\0';

            Expense* expense = new Expense(amt, d, m, y, buffer, acc);

            totalAmount** temp = new totalAmount*[totalAmountCount + 1];
            for (int i = 0; i < totalAmountCount; i++)
                temp[i] = totalAmountList[i];
            temp[totalAmountCount++] = expense;

            delete[] totalAmountList;
            totalAmountList = temp;
        }
    }

    fin.close();
}



    void adduser() {
        User* temp = new User[size + 1];
    for (int i = 0; i < size; i++) {
        temp[i] = users[i];
    }

    int accnum, pn;
    char nam[50];
    bool isDuplicate;

    do {
        isDuplicate = false;

        // Account Number input (5-digit)
        cout << "Enter 5-digit account number: ";
        cin >> accnum;
        if (accnum < 10000 || accnum > 99999) {
            cout << "ERROR: Account number must be exactly 5 digits.\n";
            isDuplicate = true;
            continue;
        }

        // Check if account number already exists
        for (int i = 0; i < size; i++) {
            if (users[i].getAccountNum() == accnum) {
                cout << "ERROR: This account number already exists.\n";
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate) continue;

        // PIN input loop (4-digit)
        bool validPin = false;
        while (!validPin) {
            cout << "Enter a 4-digit PIN: ";
            cin >> pn;
            if (pn < 1000 || pn > 9999) {
                cout << "ERROR: PIN must be exactly 4 digits.\n";
                continue;
            }

            bool pinExists = false;
            for (int i = 0; i < size; i++) {
                if (users[i].getPin() == pn) {
                    cout << "ERROR: This PIN already exists.\n";
                    pinExists = true;
                    break;
                }
            }

            if (!pinExists) validPin = true;
        }

        cin.ignore();
        cout << "Enter your name: ";
        cin.getline(nam, 50);

        bool nameHasChar = false;
        for (int i = 0; nam[i] != '\0'; i++) {
            if (nam[i] != ' ' && nam[i] != '\t' && nam[i] != '\n') {
                nameHasChar = true;
                break;
            }
        }

        if (!nameHasChar) {
            cout << "ERROR: Name cannot be empty or just spaces.\n";
            isDuplicate = true;
        }

    } while (isDuplicate);

    temp[size].setData(accnum, pn, nam);
    delete[] users;
    users = temp;
    size++;
    cout << "Account created successfully!" << endl;
    saveUserToFile(users[size - 1]);
    cout << "----------------------------------\n";
    }
    void saveUserToFile(const User& user) {
    ofstream fout("userdata.dat", ios::binary | ios::app);
    if (!fout) {
        cout << "Error: Unable to open file for writing.\n";
        return;
    }

    SimpleUserData temp;
    temp.accountNum = user.getAccountNum();
    temp.pin = user.getPin();
    strcpy(temp.name, user.getName());

    fout.write((char*)&temp, sizeof(SimpleUserData));
    fout.close();
}


    int num_of_users() {
        return size;
    }

    User* getUser(int index) {
        if (index >= 0 && index < size) {
            return &users[index];
        }
        return nullptr;
    }
    
    void loadUsersFromFile() {
    ifstream fin("userdata.dat", ios::binary);
    if (!fin) return;

    SimpleUserData temp;
    while (fin.read((char*)&temp, sizeof(SimpleUserData))) {
        User* newUsers = new User[size + 1];
        for (int i = 0; i < size; i++) newUsers[i] = users[i];
        newUsers[size].setData(temp.accountNum, temp.pin, temp.name);
        delete[] users;
        users = newUsers;
        size++;
    }
    fin.close();
}


    void addIncome(int currentIndex) {
        int amt, d, m, y;
    char src[100];

    cout << "Enter amount: ";
    cin >> amt;
    if (amt <= 0) { cout << "Amount must be positive.\n"; return; }

    do {
        cout << "Enter day (1-31): ";
        cin >> d;
        cout << "Enter month (1-12): ";
        cin >> m;
        cout << "Enter year (e.g. 2024): ";
        cin >> y;

        if (!isValidDate(d, m, y)) {
            cout << "Invalid date. Please re-enter.\n";
        } else break;
    } while (true);

    cin.ignore();
    cout << "Enter source: ";
    cin.getline(src, 100);
    if (src[0] == '\0') {
        cout << "Source cannot be empty.\n";
        return;
    }

    totalAmount** temp = new totalAmount*[totalAmountCount + 1];
    for (int i = 0; i < totalAmountCount; i++) temp[i] = totalAmountList[i];
    temp[totalAmountCount] = new Income(amt, d, m, y, src, users[currentIndex].getAccountNum());
    delete[] totalAmountList;
    totalAmountList = temp;
    totalAmountCount++;

    cout << "Income added successfully.\n";
    cout << "----------------------------------\n";
    }

    void viewIncome(int currentAccNum) {
    int count = 0;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Income" &&
            totalAmountList[i]->getAccountNum() == currentAccNum) {
            cout << ++count << ". ";
            totalAmountList[i]->display();
        }
    }
    if (count == 0) {
        cout << "No income records found.\n";
    }
    cout << "----------------------------------\n";
}


void summaryBySource(const string& targetSource, int currentAcc) {
    int total = 0, count = 0;
    cout << "Income from source: " << targetSource << endl;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Income" &&
            totalAmountList[i]->getAccountNum() == currentAcc) {
            Income* income = (Income*)totalAmountList[i];
            if (income->getSource() == targetSource) {
                income->display();
                total += income->getAmount();
                count++;
            }
        }
    }
    if (count == 0) {
        cout << "No records for this source.\n";
    }
    cout << "Total from " << targetSource << ": " << total << endl;
    cout << "----------------------------------\n";
}


    int getTotalIncome(int accNum) const {
    int total = 0;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Income" &&
            totalAmountList[i]->getAccountNum() == accNum) {
            total += totalAmountList[i]->getAmount();
        }
    }
    return total;
}



    void addExpense(int currentIndex) {
        int amt, d, m, y;
    char cat[100];

    cout << "Enter amount for expense: ";
    cin >> amt;
    if (amt <= 0) {
        cout << "Amount must be positive.\n";
        return;
    }
	int currentAcc = users[currentIndex].getAccountNum();
    int incomeSoFar = getTotalIncome(currentAcc);
    int expenseSoFar = getTotalExpense(currentAcc);
    if (expenseSoFar + amt > incomeSoFar) {
        cout << "Cannot add expense. Total expense would exceed income.\n";
        return;
    }

    do {
        cout << "Enter day (1-31): ";
        cin >> d;
        cout << "Enter month (1-12): ";
        cin >> m;
        cout << "Enter year (e.g. 2024): ";
        cin >> y;

        if (!isValidDate(d, m, y)) {
            cout << "Invalid date. Please re-enter.\n";
        } else break;
    } while (true);

    cin.ignore();
    cout << "Enter category: ";
    cin.getline(cat, 100);
    if (cat[0] == '\0') {
        cout << "Category cannot be empty.\n";
        return;
    }

    totalAmount** temp = new totalAmount*[totalAmountCount + 1];
    for (int i = 0; i < totalAmountCount; i++) temp[i] = totalAmountList[i];
    temp[totalAmountCount] = new Expense(amt, d, m, y, cat, users[currentIndex].getAccountNum());
    delete[] totalAmountList;
    totalAmountList = temp;
    totalAmountCount++;

    cout << "Expense added successfully.\n";
    cout << "----------------------------------\n";
    }

   void viewExpense(int currentAcc) {
    int count = 0;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Expense" &&
            totalAmountList[i]->getAccountNum() == currentAcc) {
            cout << ++count << ". ";
            totalAmountList[i]->display();
        }
    }
    if (count == 0) {
        cout << "No expense records found.\n";
    }
    cout << "----------------------------------\n";
}


    void summaryByCategory(const string& targetCategory, int currentAcc) {
    int total = 0, count = 0;
    cout << "Expenses in category: " << targetCategory << endl;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Expense" &&
            totalAmountList[i]->getAccountNum() == currentAcc) {
            Expense* expense = (Expense*)totalAmountList[i];
            if (expense->getCategory() == targetCategory) {
                expense->display();
                total += expense->getAmount();
                count++;
            }
        }
    }
    if (count == 0) {
        cout << "No records for this category.\n";
    }
    cout << "Total in " << targetCategory << ": " << total << endl;
    cout << "----------------------------------\n";
}



    

    

   int getTotalExpense(int accNum) const {
    int total = 0;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Expense" &&
            totalAmountList[i]->getAccountNum() == accNum) {
            total += totalAmountList[i]->getAmount();
        }
    }
    return total;
}


    void viewBalance(int accnum) const {
        int income = getTotalIncome(accnum);
        int expense = getTotalExpense(accnum);
        int balance = income - expense;

        cout << "\n========= Balance Summary =========\n";
        cout << "Total Income : " << income << endl;
        cout << "Total Expense: " << expense << endl;
        cout << "Net Balance  : " << balance << endl;
        cout << "======================================\n";
    }
    
    void viewMonthlySummary(int targetMonth, int targetYear, int currentAcc) const {
    int incomeTotal = 0, expenseTotal = 0;

    cout << "\n========== Income for " << targetMonth << "/" << targetYear << " ==========\n";
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Income" &&
            totalAmountList[i]->getAccountNum() == currentAcc &&
            totalAmountList[i]->getMonth() == targetMonth &&
            totalAmountList[i]->getYear() == targetYear) {
            totalAmountList[i]->display();
            incomeTotal += totalAmountList[i]->getAmount();
        }
    }
    cout << "Total Income: " << incomeTotal << "\n";

    cout << "\n========== Expenses for " << targetMonth << "/" << targetYear << " ==========\n";
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Expense" &&
            totalAmountList[i]->getAccountNum() == currentAcc &&
            totalAmountList[i]->getMonth() == targetMonth &&
            totalAmountList[i]->getYear() == targetYear) {
            totalAmountList[i]->display();
            expenseTotal += totalAmountList[i]->getAmount();
        }
    }

    cout << "Total Expenses: " << expenseTotal << "\n";
    cout << "\n========== Net Summary ==========\n";
    cout << "Net Balance: " << (incomeTotal - expenseTotal) << "\n";
    cout << "----------------------------------\n";
}


void deleteIncomeEntry(int entryNumber, int currentAcc) {
    int count = 0, deleteIndex = -1;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Income" &&
            totalAmountList[i]->getAccountNum() == currentAcc) {
            count++;
            if (count == entryNumber) {
                deleteIndex = i;
                break;
            }
        }
    }
    if (deleteIndex == -1) {
        cout << "Invalid entry number.\n";
        return;
    }

    totalAmount** temp = new totalAmount*[totalAmountCount - 1];
    for (int i = 0, j = 0; i < totalAmountCount; i++) {
        if (i != deleteIndex) temp[j++] = totalAmountList[i];
        else delete totalAmountList[i];
    }
    delete[] totalAmountList;
    totalAmountList = temp;
    totalAmountCount--;

    cout << "Income entry deleted.\n";

    // Remove all expenses if total expense > total income
    int income = getTotalIncome(currentAcc);
    int expense = getTotalExpense(currentAcc);
    if (expense > income) {
        totalAmount** filtered = new totalAmount*[totalAmountCount];
        int newCount = 0;
        for (int i = 0; i < totalAmountCount; i++) {
            if (totalAmountList[i]->getType() == "Income") {
                filtered[newCount++] = totalAmountList[i];
            } else {
                delete totalAmountList[i];
            }
        }
        delete[] totalAmountList;
        totalAmountList = filtered;
        totalAmountCount = newCount;
        cout << "Expenses reset due to insufficient income.\n";
    }

    cout << "----------------------------------\n";
}




void deleteExpenseEntry(int entryNumber, int currentAcc) {
    int count = 0, deleteIndex = -1;
    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getType() == "Expense" &&
            totalAmountList[i]->getAccountNum() == currentAcc) {
            count++;
            if (count == entryNumber) {
                deleteIndex = i;
                break;
            }
        }
    }
    if (deleteIndex == -1) {
        cout << "Invalid entry number.\n";
        return;
    }

    totalAmount** temp = new totalAmount*[totalAmountCount - 1];
    for (int i = 0, j = 0; i < totalAmountCount; i++) {
        if (i != deleteIndex) temp[j++] = totalAmountList[i];
        else delete totalAmountList[i];
    }
    delete[] totalAmountList;
    totalAmountList = temp;
    totalAmountCount--;

    cout << "Expense entry deleted.\n";
}


    void deleteUser(int index) {
        if (index < 0 || index >= size) {
            cout << "Invalid user index.\n";
            return;
        }

        int userAcc = users[index].getAccountNum();
        int newCount = 0;

        for (int i = 0; i < totalAmountCount; i++) {
            if (totalAmountList[i]->getAccountNum() == userAcc) {
                delete totalAmountList[i];
            } else {
                totalAmountList[newCount++] = totalAmountList[i];
            }
        }

        totalAmountCount = newCount;

        for (int i = index; i < size - 1; i++) {
            users[i] = users[i + 1];
        }

        User* tempUsers = new User[size - 1];
        for (int i = 0; i < size - 1; i++) {
            tempUsers[i] = users[i];
        }

        delete[] users;
        users = tempUsers;
        size--;

        cout << "User and their transactions deleted successfully.\n";
        rewriteUserFile();
        rewriteTransactionFile();

    }
    
    void rewriteUserFile() {
    ofstream fout("userdata.dat", ios::binary | ios::trunc);
    if (!fout) {
        cout << "Error: Unable to open file for rewriting.\n";
        return;
    }

    for (int i = 0; i < size; i++) {
        SimpleUserData temp;
        temp.accountNum = users[i].getAccountNum();
        temp.pin = users[i].getPin();
        strcpy(temp.name, users[i].getName());

        fout.write((char*)&temp, sizeof(SimpleUserData));
    }

    fout.close();
}
void saveUpdatedUsersToFile() {
    ofstream fout("userdata.dat", ios::binary | ios::trunc);
    if (!fout) {
        cout << "Error: Could not open userdata.dat for updating.\n";
        return;
    }

    for (int i = 0; i < size; i++) {
        SimpleUserData temp;
        temp.accountNum = users[i].getAccountNum();
        temp.pin = users[i].getPin();
        strcpy(temp.name, users[i].getName());

        fout.write((char*)&temp, sizeof(SimpleUserData));
    }

    fout.close();
}

void saveOnlyTransactionsOnLogout(int currentIndex) {
    saveUpdatedUsersToFile();

    ofstream fout("fulldata.dat", ios::binary | ios::trunc);

    if (!fout) {
        cout << "Error: Could not open transaction file.\n";
        return;
    }

    int currentAcc = users[currentIndex].getAccountNum();

    for (int i = 0; i < totalAmountCount; i++) {
        if (totalAmountList[i]->getAccountNum() == currentAcc) {
            string type = totalAmountList[i]->getType();
            char typeField[10] = {0};
			strncpy(typeField, type.c_str(), sizeof(typeField) - 1);  // copy safely
			fout.write(typeField, sizeof(typeField));  // write first 10 chars as type

            if (type == "Income") {
                ((Income*)totalAmountList[i])->save(fout);

            } else if (type == "Expense") {
                ((Expense*)totalAmountList[i])->save(fout);

            }
        }
    }

    fout.close();
}

void rewriteTransactionFile() {
    ofstream fout("fulldata.dat", ios::binary | ios::trunc);
    if (!fout) {
        cout << "Error: Unable to rewrite transaction file.\n";
        return;
    }

    for (int i = 0; i < totalAmountCount; i++) {
        string type = totalAmountList[i]->getType();
        char typeField[10] = {0};
        strncpy(typeField, type.c_str(), sizeof(typeField) - 1);
        fout.write(typeField, sizeof(typeField));

        if (type == "Income")
            ((Income*)totalAmountList[i])->save(fout);
        else if (type == "Expense")
            ((Expense*)totalAmountList[i])->save(fout);
    }

    fout.close();
}


};

int main() {
    int choice;
    AccountManager a1;
    a1.loadUsersFromFile();
    a1.loadTransactionsFromFile();



    do {
        cout << "\n========= Personal Finance Tracker =========" << endl;
        cout << "1. Create Account" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            a1.adduser();
            break;

        case 2: {
            int accnum, pin;
            cout << "Enter account number: ";
            cin >> accnum;
            cout << "Enter PIN: ";
            cin >> pin;

            bool found = false;
            int currentIndex = -1;
            for (int i = 0; i < a1.num_of_users(); i++) {
                User* u = a1.getUser(i);
                if (u && u->login(accnum, pin)) {
                    cout << "Login successful!\n";
                    u->display();
                    cout << "-----------------------------\n";
                    found = true;
                    currentIndex = i;
                    break;
                }
            }

            if (!found) {
                cout << "Invalid account number or PIN." << endl;
            }

            if (found) {
                int ch;
                do {
                    cout << "\n=====================================\n";
                    cout << "         Finance Dashboard         \n";
                    cout << "=====================================\n";
                    cout << "1. Manage Income\n";
                    cout << "2. Manage Expense\n";
                    cout << "3. View Balance\n";
                    cout << "4. Delete Entries\n";
                    cout << "5. Update Personal Info\n";
                    cout << "6. Logout\n";
                    cout << "=====================================\n";

                    cout << "Enter your choice: ";
                    cin >> ch;

                    switch (ch) {
                    case 1: {
                        int ich;
                        do {
                            cout << "\n---------- Manage Income ----------\n";
                            cout << "1. Add Income\n";
                            cout << "2. View Income History\n";
                            cout << "3. View Income by Source\n";
                            cout << "4. Back to Main Menu\n";
                            cout << "----------------------------------\n";

                            cout << "Enter your choice: ";
                            cin >> ich;

                            switch (ich) {
                            case 1:
                                a1.addIncome(currentIndex);
                                break;
                            case 2:
                                a1.viewIncome(a1.getUser(currentIndex)->getAccountNum());



                                break;
                            case 3: {
                                cin.ignore();
                                string src;
                                cout << "Enter source to view summary: ";
                                getline(cin, src);
                                a1.summaryBySource(src, a1.getUser(currentIndex)->getAccountNum());

                                break;
                            }
                            case 4:
                                cout << "Returning to main menu...\n";
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                            }

                        } while (ich != 4);
                        break;
                    }

                    case 2: {
                        int ech;
                        do {
                            cout << "\n---------- Manage Expense ----------\n";
                            cout << "1. Add Expense\n";
                            cout << "2. View Expense History\n";
                            cout << "3. View Expense by Category\n";
                            cout << "4. Back to Main Menu\n";
                            cout << "----------------------------------\n";

                            cout << "Enter your choice: ";
                            cin >> ech;

                            switch (ech) {
                            case 1:
                                a1.addExpense(currentIndex);
                                break;
                            case 2:
                                a1.viewExpense(a1.getUser(currentIndex)->getAccountNum());

                                break;
                            case 3: {
                                string cat;
                                cout << "Enter your category: ";
                                cin.ignore();
                                getline(cin, cat);
                                a1.summaryByCategory(cat, a1.getUser(currentIndex)->getAccountNum());


                                break;
                            }
                            case 4:
                                cout << "Returning to main menu...\n";
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                            }

                        } while (ech != 4);
                        break;
                    }

                    case 3: {
                        int bch;
                        do {
                            cout << "\n---------- View Balance ----------\n";
                            cout << "1. View Net Balance\n";
                            cout << "2. View Monthly Summary\n";
                            cout << "3. Back to Main Menu\n";
                            cout << "----------------------------------\n";

                            cout << "Enter your choice: ";
                            cin >> bch;

                            switch (bch) {
                            case 1:
                                a1.viewBalance(a1.getUser(currentIndex)->getAccountNum());

                                break;
                            case 2: {
                                int mon, year;
                                cout << "Enter month (1-12): ";
                                cin >> mon;
                                cout << "Enter year: ";
                                cin >> year;
                                a1.viewMonthlySummary(mon, year, a1.getUser(currentIndex)->getAccountNum());

                                break;
                            }
                            case 3:
                                cout << "Returning to main menu...\n";
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                            }

                        } while (bch != 3);
                        break;
                    }

                    case 4: {
                        int dch;
                        do {
                            cout << "\n---------- Deletion Block ----------\n";
                            cout << "1. Delete Account\n";
                            cout << "2. Delete Income Entry\n";
                            cout << "3. Delete Expense Entry\n";
                            cout << "4. Back to Main Menu\n";
                            cout << "----------------------------------\n";

                            cout << "Enter your choice: ";
                            cin >> dch;

                            switch (dch) {
                            case 1: {
                                cout << "Are you sure you want to delete your account? (y/n): ";
                                char confirm;
                                cin >> confirm;
                                if (confirm == 'y' || confirm == 'Y') {
                                    a1.deleteUser(currentIndex);
                                    ch = 6; // force logout
                                    dch = 4;
                                } else {
                                    cout << "Account deletion cancelled.\n";
                                }
                                break;
                            }

                            case 2:
                                a1.viewIncome(a1.getUser(currentIndex)->getAccountNum());
								int delIncome;
								cout<<"Enter the income entry number you want to delete: ";
								cin >> delIncome;
								a1.deleteIncomeEntry(delIncome, a1.getUser(currentIndex)->getAccountNum());
								break;

                            case 3:
                                a1.viewExpense(a1.getUser(currentIndex)->getAccountNum());
								int delExp;
								cout<<"Enter the expense entry number you want to delete: ";
								cin >> delExp;
								a1.deleteExpenseEntry(delExp, a1.getUser(currentIndex)->getAccountNum());
                                break;

                            case 4:
                                cout << "Returning to main menu...\n";
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                            }

                        } while (dch != 4);
                        break;
                    }

                    case 5: {
                        int opt;
                        do {
                            cout << "\n---------- Update Personal Info ----------\n";
                            cout << "1. Change Name\n";
                            cout << "2. Change PIN\n";
                            cout << "3. Change Account Number\n";
                            cout << "4. Back to Finance Menu\n";
                            cout << "----------------------------------\n";

                            cout << "Enter your choice: ";
                            cin >> opt;

                            switch (opt) {
                            case 1: {
                                char newName[50];
                                cin.ignore();
                                cout << "Enter new name: ";
                                cin.getline(newName, 50);

                                if (newName[0] == '\0') {
                                    cout << "Name cannot be empty.\n";
                                    break;
                                }

                                a1.getUser(currentIndex)->setData(
                                    a1.getUser(currentIndex)->getAccountNum(),
                                    a1.getUser(currentIndex)->getPin(),
                                    newName
                                );
                                cout << "Name updated successfully.\n";
                                a1.saveUpdatedUsersToFile();

                                break;
                            }

                            case 2: {
                                int newPin;
                                bool duplicate;
                                do {
                                    duplicate = false;
                                    cout << "Enter new 4-digit PIN: ";
                                    cin >> newPin;

                                    if (newPin < 1000 || newPin > 9999) {
                                        cout << "PIN must be a 4-digit number.\n";
                                        duplicate = true;
                                        continue;
                                    }

                                    for (int i = 0; i < a1.num_of_users(); i++) {
                                        if (i != currentIndex && a1.getUser(i)->getPin() == newPin) {
                                            duplicate = true;
                                            cout << "This PIN is already taken. Try again.\n";
                                            break;
                                        }
                                    }
                                } while (duplicate);

                                a1.getUser(currentIndex)->setData(
                                    a1.getUser(currentIndex)->getAccountNum(),
                                    newPin,
                                    a1.getUser(currentIndex)->getName()
                                );
                                cout << "PIN updated successfully.\n";
                                a1.saveUpdatedUsersToFile();

                                break;
                            }

                            case 3: {
                                int newAccNum;
                                bool duplicate;
                                do {
                                    duplicate = false;
                                    cout << "Enter new account number: ";
                                    cin >> newAccNum;

                                    if (newAccNum <= 0) {
                                        cout << "Account number must be positive.\n";
                                        duplicate = true;
                                        continue;
                                    }

                                    for (int i = 0; i < a1.num_of_users(); i++) {
                                        if (i != currentIndex && a1.getUser(i)->getAccountNum() == newAccNum) {
                                            duplicate = true;
                                            cout << "This account number is already taken. Try again.\n";
                                            break;
                                        }
                                    }
                                } while (duplicate);

                                a1.getUser(currentIndex)->setData(
                                    newAccNum,
                                    a1.getUser(currentIndex)->getPin(),
                                    a1.getUser(currentIndex)->getName()
                                );
                                cout << "Account number updated successfully.\n";
                                a1.saveUpdatedUsersToFile();

								break;
                            }

                            case 4:
                                cout << "Returning to finance menu...\n";
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                            }
                        } while (opt != 4);
                        break;
                    }

                    case 6:
                        cout << "Logging out...\n";
                        a1.saveOnlyTransactionsOnLogout(currentIndex);

                        break;

                    default:
                        cout << "Invalid choice. Try again.\n";
                    }

                } while (ch != 6);
            }
            break;
        }

        case 3:
            cout << "Exiting program..." << endl;
            break;

        default:
            cout << "Invalid choice. Try again." << endl;
        }

    } while (choice != 3);

    return 0;
}