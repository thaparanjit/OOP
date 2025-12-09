#include <iostream>   // For input/output
#include <fstream>    // For file handling
#include <vector>     // For dynamic arrays
#include <limits>     // For numeric limits (to clear input buffer)
#include <chrono>     // For timing (loading animation)
#include <thread>     // For sleep/delay
#include <iomanip>    // For formatting output
#include <map>        // For mapping food orders and quantities

using namespace std;

// --- Terminal Colors for console output ---
#define GREEN  "\033[1;32m"   // Green text
#define RED    "\033[1;31m"   // Red text
#define RESET  "\033[0m"      // Reset color

// --- Loading Animation ---
void loadingAnimation(string task = "Processing") {
    cout << task;
    for (int i = 0; i < 5; ++i) {      // Show 5 dots one by one
        cout << ".";
        cout.flush();                  // Flush output immediately
        this_thread::sleep_for(chrono::milliseconds(1000)); // Wait 4 sec
    }
    cout << "\n";
}

// --- Welcome Banner ---
void showBanner() {
    cout << R"(    
 __        __   _                                        
 \ \      / /__| | ___ ___  _ __ ___   ___     
  \ \ /\ / / _ \ |/ __/ _ \| '_ ` _ \ / _ \   
   \ V  V /  __/ | (_| (_) | | | | | |  __/    
    \_/\_/ \___|_|\___\___/|_| |_| |_|\___|  
  
)" << endl;
}

// -------- Customer Class --------
class Customer {
public:
    string name, phone, email, idProof;

    void inputDetails() {       // Get customer info from user
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear input buffer
        cout << "\nEnter Customer Name: ";
        getline(cin, name);
        cout << "Enter Phone Number: ";
        getline(cin, phone);
        cout << "Enter Email: ";
        getline(cin, email);
        cout << "Enter ID Proof: ";
        getline(cin, idProof);
    }

    void displayDetails() const {    // Show customer info
        cout << "\n--- Customer Details ---\n";
        cout << "Name     : " << name << endl;
        cout << "Phone    : " << phone << endl;
        cout << "Email    : " << email << endl;
        cout << "ID Proof : " << idProof << endl;
    }

    void saveToFile(int roomNum) const {  // Save info to file
        ofstream outFile("customers.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "Room: " << roomNum << "		" << name << "		" << phone << "		" << email << "		" << idProof << "\n";
            outFile.close();
        } else {
            cout << RED << "Error: Could not save customer details.\n" << RESET;
        }
    }
};

// -------- Room Class --------
class Room : public Customer {
public:
    int roomNumber;
    float price;
    bool isAvailable = true;
    vector<string> foodOrdered;     // List of food items ordered

    void setDetails(int num, float p) {   // Initialize room number & price
        roomNumber = num;
        price = p;
        isAvailable = true;
    }

    void display() const {    // Show room basic info (number, price, availability)
        cout << "Room " << roomNumber << " | Price: $" << price << " | " << (isAvailable ? "Available" : "Booked") << "\n";
    }

    void bookRoom() {   // Book room if available, take customer info, save to file
        if (isAvailable) {
            inputDetails();
            loadingAnimation("Booking room");
            saveToFile(roomNumber);
            isAvailable = false;
            cout << GREEN << "Room " << roomNumber << " Booked successfully!\n" << RESET;
        } else {
            cout << RED << "Room already booked.\n" << RESET;
        }
    }

    void showFullInfo() const {   // Show detailed info including food orders
        cout << "\n--- Room " << roomNumber << " Info ---\n";
        displayDetails();
        cout << "Room Price : $" << price << endl;
        cout << "Status     : " << (isAvailable ? "Available" : "Booked") << endl;
        if (!foodOrdered.empty()) {
            cout << "\n--- Food Ordered ---\n";
            map<string, int> count;
            for (size_t i = 0; i < foodOrdered.size(); ++i)
                count[foodOrdered[i]]++;  // Count quantity per food

            for (map<string, int>::iterator it = count.begin(); it != count.end(); ++it)
                cout << "- " << it->first << " x " << it->second << endl;
        } else {
            cout << "\nNo food ordered.\n";
        }
    }

    float getFoodCost() const {  // Calculate total food cost
        float cost = 0;
        for (size_t i = 0; i < foodOrdered.size(); ++i) {
            if (foodOrdered[i] == "Burger") cost += 5;
            else if (foodOrdered[i] == "Pizza") cost += 8;
            else if (foodOrdered[i] == "Water") cost += 2;
            else if (foodOrdered[i] == "Fries") cost += 3.5f;
            else if (foodOrdered[i] == "Coke") cost += 2.5f;
            else if (foodOrdered[i] == "Ice Cream") cost += 4.0f;
        }
        return cost;
    }

    void generateBill() {  // Create and print bill, save to file, reset room
        if (isAvailable) {
            cout << RED << "Room not booked yet. No bill available.\n" << RESET;
            return;
        }

        loadingAnimation("Generating bill");
        float foodCost = getFoodCost();
        float total = price + foodCost;

        cout << "\n------ BILL ------\n";
        cout << "Room Number : " << roomNumber << endl;
        cout << "Room Cost   : $" << fixed << setprecision(2) << price << endl;
        cout << "Food Cost   : $" << fixed << setprecision(2) << foodCost << endl;
        cout << "---------------------\n";
        cout << "Total Amount: $" << fixed << setprecision(2) << total << endl;
        cout << "----------------------\n";
        cout << GREEN << "Payment Status: Paid\n" << RESET;

        // Save bill to text file
        ofstream billFile("bill_room_" + to_string(roomNumber) + ".txt");
        if (billFile.is_open()) {
            billFile << "------ BILL ------\n";
            billFile << "Room Number : " << roomNumber << endl;
            billFile << "Customer    : " << name << endl;
            billFile << "Room Cost   : $" << price << endl;
            billFile << "Food Cost   : $" << foodCost << endl;
            billFile << "------------------\n";
            billFile << "Total Amount: $" << total << endl;
            billFile << "------------------\n";
            billFile << "Payment Status: Paid\n";
            billFile.close();
        }

        // Log checkout info
        ofstream history("checkout_history.txt", ios::app);
        if (history.is_open()) {
            history << "Room " << roomNumber << " | Customer: " << name << " | Total: $" << total << "\n";
            history.close();
        }

        // Reset room state for next customer
        isAvailable = true;
        foodOrdered.clear();
        name = phone = email = idProof = "";
    }
};

// -------- Food Class --------
class Food {
public:
    struct MenuItem {
        string name;
        float price;
    };

    vector<MenuItem> menu;

    Food() {   // Initialize menu with items and prices
        menu.push_back({"Burger", 5.0f});
        menu.push_back({"Pizza", 8.0f});
        menu.push_back({"Water", 2.0f});
        menu.push_back({"Fries", 3.5f});
        menu.push_back({"Coke", 2.5f});
        menu.push_back({"Ice Cream", 4.0f});
    }

    void showMenu() {   // Display food menu
        cout << "\n========== FOOD MENU ==========\n";
        cout << left << setw(5) << "No." << setw(20) << "Item" << setw(10) << "Price\n";
        cout << "-------------------------------\n";
        for (size_t i = 0; i < menu.size(); ++i) {
            cout << left << setw(5) << (i + 1)
                 << setw(20) << menu[i].name
                 << "$" << fixed << setprecision(2) << menu[i].price << "\n";
        }
        cout << "0. Finish Ordering\n";
        cout << "===============================\n";
    }

    void orderFood(Room &room) {   // Take food orders and add to room
        int choice, quantity;
        map<string, int> orderMap;

        do {
            showMenu();
            cout << "Enter item number to order (0 to finish): ";
            cin >> choice;

            if (choice > 0 && choice <= (int)menu.size()) {
                cout << "Enter quantity: ";
                cin >> quantity;
                if (quantity <= 0) {
                    cout << RED << "Quantity must be positive.\n" << RESET;
                    continue;
                }
                orderMap[menu[choice - 1].name] += quantity;  // Add quantity
                cout << GREEN << "Added " << quantity << " x " << menu[choice - 1].name << RESET << "\n";
            } else if (choice != 0) {
                cout << RED << "Invalid choice!\n" << RESET;
            }
        } while (choice != 0);

        // Append ordered items to room's food list
        for (map<string, int>::iterator it = orderMap.begin(); it != orderMap.end(); ++it) {
            string item = it->first;
            int qty = it->second;
            for (int i = 0; i < qty; ++i)
                room.foodOrdered.push_back(item);
        }

        if (!orderMap.empty()) {   // Print order summary
            cout << "\n------ Order Summary ------\n";
            float subtotal = 0.0f;
            for (map<string, int>::iterator it = orderMap.begin(); it != orderMap.end(); ++it) {
                string item = it->first;
                int qty = it->second;

                float unitPrice = 0;
                for (size_t i = 0; i < menu.size(); ++i) {
                    if (menu[i].name == item) {
                        unitPrice = menu[i].price;
                        break;
                    }
                }
                cout << qty << " x " << item << " = $" << fixed << setprecision(2) << qty * unitPrice << "\n";
                subtotal += qty * unitPrice;
            }
            cout << "----------------------------\n";
            cout << "Subtotal: $" << fixed << setprecision(2) << subtotal << "\n";
        } else {
            cout << "No items ordered.\n";
        }
    }
};

// -------- Admin Class --------
class Admin {
    string username = "admin";   // Default admin username
    string password = "1234";    // Default password

public:
    bool login() {   // Simple admin login check
        string u, p;
        cout << "Enter Admin Username: ";
        cin >> u;
        cout << "Enter Password: ";
        cin >> p;
        loadingAnimation("Authenticating");
        return u == username && p == password;
    }

    void adminMenu(vector<Room>& rooms) {  // Admin actions menu
        int choice;
        do {
            cout << "\n---- Admin Menu ----\n";
            cout << "1. View Room Availability\n";
            cout << "2. View Room Details by Number\n";
            cout << "3. Logout\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                for (size_t i = 0; i < rooms.size(); ++i) rooms[i].display();
            } else if (choice == 2) {
                int rnum;
                cout << "Enter room number: ";
                cin >> rnum;
                bool found = false;
                for (size_t i = 0; i < rooms.size(); ++i) {
                    if (rooms[i].roomNumber == rnum) {
                        rooms[i].showFullInfo();
                        found = true;
                        break;
                    }
                }
                if (!found) cout << RED << "Room not found.\n" << RESET;
            } else if (choice != 3) {
                cout << RED << "Invalid choice.\n" << RESET;
            }
        } while (choice != 3);
    }
};

// -------- Main Function --------
int main() {
    showBanner();

    vector<Room> rooms(5);  // Create 5 rooms
    for (int i = 0; i < 5; ++i)
        rooms[i].setDetails(101 + i, 1000 + i * 200);   // Room 101..105 prices 1000,1200...

    Food food;
    Admin admin;
    int choice;

    do {
        cout << "\n---- Royal Palace Menu ----\n";
        cout << "1. View Available Rooms\n";
        cout << "2. Book a Room\n";
        cout << "3. Order Food\n";
        cout << "4. Admin Login\n";
        cout << "5. Generate Bill & Checkout\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                for (size_t i = 0; i < rooms.size(); ++i) rooms[i].display();  // Show rooms
                break;

            case 2: {  // Book room
                int rnum;
                cout << "Enter room number to book: ";
                cin >> rnum;
                bool found = false;
                for (size_t i = 0; i < rooms.size(); ++i) {
                    if (rooms[i].roomNumber == rnum) {
                        rooms[i].bookRoom();
                        found = true;
                        break;
                    }
                }
                if (!found) cout << RED << "Room not found.\n" << RESET;
                break;
            }

            case 3: {  // Order food for booked room
                int rnum;
                cout << "Enter your room number: ";
                cin >> rnum;
                bool found = false;
                for (size_t i = 0; i < rooms.size(); ++i) {
                    if (rooms[i].roomNumber == rnum && !rooms[i].isAvailable) {
                        food.orderFood(rooms[i]);
                        found = true;
                        break;
                    }
                }
                if (!found) cout << RED << "Room not booked or invalid.\n" << RESET;
                break;
            }

            case 4:  // Admin login and menu
                if (admin.login()) {
                    cout << GREEN << "Login successful!\n" << RESET;
                    admin.adminMenu(rooms);
                } else {
                    cout << RED << "Login failed.\n" << RESET;
                }
                break;

            case 5: {  // Generate bill and checkout
                int rnum;
                cout << "Enter your room number to generate bill and checkout: ";
                cin >> rnum;
                bool found = false;
                for (size_t i = 0; i < rooms.size(); ++i) {
                    if (rooms[i].roomNumber == rnum && !rooms[i].isAvailable) {
                        rooms[i].generateBill();
                        found = true;
                        break;
                    }
                }
                if (!found) cout << RED << "Room not booked or invalid.\n" << RESET;
                break;
            }

            case 6:
                cout << "Thank you for staying with Royal Palace ??!\n";
                break;

            default:
                cout << RED << "Invalid option.\n" << RESET;
        }
    } while (choice != 6);

    return 0;
}

