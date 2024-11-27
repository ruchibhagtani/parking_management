#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

// Slot Class
class Slot {
public:
    string type;
    string ticketId;
    string entryTime;
    string vehicleType;
    string registration;

    // Constructor
    Slot(string type = "") : type(type), ticketId(""), entryTime(""), vehicleType(""), registration("") {}
};

// ParkingLot Class
class ParkingLot {
private:
    const int MAX_FLOORS = 10;
    const int SLOTS_PER_FLOOR = 10;
    string parkingLotId;
    vector<vector<Slot>> slots;
    int availableCarSlots;
    int availableTruckSlots;
    int availableBikeSlots;

    // Helper function to get current time as a string
    string getCurrentTime() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
           << setw(2) << setfill('0') << ltm->tm_mday << " "
           << setw(2) << setfill('0') << ltm->tm_hour << ":"
           << setw(2) << setfill('0') << ltm->tm_min << ":"
           << setw(2) << setfill('0') << ltm->tm_sec;
        return ss.str();
    }

    // Write logs to a file
    void writeToLogFile(const string &message) {
        ofstream outFile("parking_log.txt", ios::app);
        if (outFile.is_open()) {
            outFile << message << endl;
            outFile.close();
        }
    }

    // Update available slots based on vehicle type
    void updateAvailableSlots(const string &type, int change) {
        if (type == "car") availableCarSlots += change;
        else if (type == "truck") availableTruckSlots += change;
        else if (type == "bike") availableBikeSlots += change;
    }

public:
    // Constructor
    ParkingLot(string id, int floors, int carSlots, int truckSlots, int bikeSlots)
        : parkingLotId(id),
          availableCarSlots(floors * carSlots),
          availableTruckSlots(floors * truckSlots),
          availableBikeSlots(floors * bikeSlots) {
        slots.resize(floors, vector<Slot>(SLOTS_PER_FLOOR));

        for (int i = 0; i < floors; i++) {
            for (int j = 0; j < SLOTS_PER_FLOOR; j++) {
                if (j < carSlots)
                    slots[i][j] = Slot("car");
                else if (j < carSlots + truckSlots)
                    slots[i][j] = Slot("truck");
                else
                    slots[i][j] = Slot("bike");
            }
        }
    }

    // Park a vehicle
    string parkVehicle(const string &type, const string &registration, int preferredFloor, int preferredSlot) {
        if (preferredFloor <= 0 || preferredFloor > slots.size() || preferredSlot <= 0 || preferredSlot > SLOTS_PER_FLOOR)
            return "Invalid floor or slot.";

        Slot &slot = slots[preferredFloor - 1][preferredSlot - 1];

        if (!slot.vehicleType.empty())
            return "Slot is occupied.";

        if ((type == "car" && availableCarSlots <= 0) ||
            (type == "truck" && availableTruckSlots <= 0) ||
            (type == "bike" && availableBikeSlots <= 0))
            return "No available slots.";

        slot.vehicleType = type;
        slot.registration = registration;
        slot.ticketId = parkingLotId + "_" + to_string(preferredFloor) + "_" + to_string(preferredSlot);
        slot.entryTime = getCurrentTime();

        updateAvailableSlots(type, -1);

        writeToLogFile("[" + getCurrentTime() + "] Vehicle parked - Type: " + type + ", Registration: " + registration +
                       ", Floor: " + to_string(preferredFloor) + ", Slot: " + to_string(preferredSlot));
        return slot.ticketId;
    }

    // Unpark a vehicle by ticket ID
    bool unparkVehicle(const string &ticketId) {
        for (auto &floor : slots) {
            for (auto &slot : floor) {
                if (slot.ticketId == ticketId) {
                    string type = slot.vehicleType;
                    writeToLogFile("[" + getCurrentTime() + "] Vehicle unparked - Ticket ID: " + ticketId);
                    slot = Slot(type);
                    updateAvailableSlots(type, 1);
                    return true;
                }
            }
        }
        return false;
    }

    // Display availability
    void displayAvailability() {
        cout << "+-----------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+" << endl;
        cout << "| Floor     | Slot 1  | Slot 2  | Slot 3  | Slot 4  | Slot 5  | Slot 6  | Slot 7  | Slot 8  | Slot 9  | Slot 10 |" << endl;
        cout << "+-----------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+" << endl;

        for (size_t i = 0; i < slots.size(); i++) {
            cout << "| Floor " << setw(3) << i + 1 << " |";
            for (size_t j = 0; j < slots[i].size(); j++) {
                cout << " " << (slots[i][j].vehicleType.empty() ? "[?????]" : "[" + slots[i][j].vehicleType + "]") << " |";
            }
            cout << endl;
        }
        cout << "+-----------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+" << endl;
    }
};

int main() {
    int floors, carSlots, truckSlots, bikeSlots;

    cout << "Enter the number of floors: ";
    cin >> floors;

    ParkingLot parkingLot("PL1", floors, carSlots, truckSlots, bikeSlots);

    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Park Vehicle\n";
        cout << "2. Unpark Vehicle\n";
        cout << "3. Display Availability\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string type, registration;
            int floor, slot;
            cout << "Enter vehicle type (car/truck/bike): ";
            cin >> type;
            cout << "Enter registration number: ";
            cin >> registration;
            cout << "Enter preferred floor: ";
            cin >> floor;
            cout << "Enter preferred slot: ";
            cin >> slot;
            string ticket = parkingLot.parkVehicle(type, registration, floor, slot);
            cout << "Parking Result: " << ticket << endl;

        } else if (choice == 2) {
            string ticketId;
            cout << "Enter Ticket ID to unpark: ";
            cin >> ticketId;
            if (parkingLot.unparkVehicle(ticketId)) {
                cout << "Vehicle unparked successfully.\n";
            } else {
                cout << "Invalid Ticket ID.\n";
            }

        } else if (choice == 3) {
            parkingLot.displayAvailability();
        }

    } while (choice != 4);

    cout << "Exiting... Goodbye!\n";
    return 0;
}
