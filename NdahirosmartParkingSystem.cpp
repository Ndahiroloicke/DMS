#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// ---------------------------------------------------------------------------
// UI helpers
// ---------------------------------------------------------------------------

void printLine(char ch = '=', int width = 60) {
    cout << string(width, ch) << "\n";
}

void printTitle(const string& title) {
    cout << "\n";
    printLine();
    cout << "  " << title << "\n";
    printLine();
}

void printMessage(const string& message, bool success = true) {
    cout << (success ? "[OK] " : "[!!] ") << message << "\n";
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string formatDateTime(time_t timestamp) {
    if (timestamp <= 0) {
        return "N/A";
    }

    tm* local = localtime(&timestamp);
    if (!local) {
        return "N/A";
    }

    ostringstream output;
    output << put_time(local, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

bool isSameDay(time_t first, time_t second) {
    tm dayOne {};
    tm dayTwo {};

#ifdef _WIN32
    if (localtime_s(&dayOne, &first) != 0 || localtime_s(&dayTwo, &second) != 0) {
        return false;
    }
#else
    if (!localtime_r(&first, &dayOne) || !localtime_r(&second, &dayTwo)) {
        return false;
    }
#endif

    return dayOne.tm_year == dayTwo.tm_year &&
           dayOne.tm_yday == dayTwo.tm_yday;
}

// ---------------------------------------------------------------------------
// Vehicle type utilities
// ---------------------------------------------------------------------------

enum class VehicleType { Motorcycle, Car, Truck };

string vehicleTypeToString(VehicleType type) {
    switch (type) {
        case VehicleType::Motorcycle:
            return "Motorcycle";
        case VehicleType::Car:
            return "Car";
        case VehicleType::Truck:
            return "Truck";
    }
    return "Unknown";
}

bool parseVehicleType(const string& input, VehicleType& type) {
    string normalized = input;
    transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);

    if (normalized == "motorcycle" || normalized == "m") {
        type = VehicleType::Motorcycle;
        return true;
    }
    if (normalized == "car" || normalized == "c") {
        type = VehicleType::Car;
        return true;
    }
    if (normalized == "truck" || normalized == "t") {
        type = VehicleType::Truck;
        return true;
    }

    return false;
}

bool isValidIdentifier(const string& value) {
    if (value.empty()) {
        return false;
    }

    for (char ch : value) {
        if (!isalnum(static_cast<unsigned char>(ch)) && ch != '-' && ch != '_') {
            return false;
        }
    }

    return true;
}

bool isValidFilename(const string& filename) {
    if (filename.empty()) {
        return false;
    }

    const string invalidChars = "\\/:*?\"<>|";
    for (char ch : filename) {
        if (invalidChars.find(ch) != string::npos) {
            return false;
        }
    }

    return true;
}

void writeFileLine(ofstream& file, char separator, int width) {
    file << string(width, separator) << "\n";
}

// ---------------------------------------------------------------------------
// Abstraction + Polymorphism: displayable entities
// ---------------------------------------------------------------------------

class Displayable {
public:
    virtual void display() const = 0;
    virtual ~Displayable() = default;
};

// ---------------------------------------------------------------------------
// Abstraction + Polymorphism: tariff policies
// ---------------------------------------------------------------------------

class TariffPolicy {
protected:
    double hourlyRate;

public:
    explicit TariffPolicy(double rate) : hourlyRate(rate) {}

    virtual ~TariffPolicy() = default;

    virtual VehicleType getVehicleType() const = 0;
    virtual string getTypeName() const = 0;

    double getHourlyRate() const {
        return hourlyRate;
    }

    void setHourlyRate(double rate) {
        hourlyRate = rate;
    }

    virtual double calculateFee(int billedHours) const {
        return billedHours * hourlyRate;
    }
};

class MotorcycleTariff : public TariffPolicy {
public:
    explicit MotorcycleTariff(double rate = 500.0) : TariffPolicy(rate) {}

    VehicleType getVehicleType() const override {
        return VehicleType::Motorcycle;
    }

    string getTypeName() const override {
        return "Motorcycle";
    }
};

class CarTariff : public TariffPolicy {
public:
    explicit CarTariff(double rate = 1000.0) : TariffPolicy(rate) {}

    VehicleType getVehicleType() const override {
        return VehicleType::Car;
    }

    string getTypeName() const override {
        return "Car";
    }
};

class TruckTariff : public TariffPolicy {
public:
    explicit TruckTariff(double rate = 1500.0) : TariffPolicy(rate) {}

    VehicleType getVehicleType() const override {
        return VehicleType::Truck;
    }

    string getTypeName() const override {
        return "Truck";
    }
};

// ---------------------------------------------------------------------------
// Task 1: Parking Slot Configuration
// ---------------------------------------------------------------------------

class ParkingSlot : public Displayable {
private:
    string slotId;
    VehicleType supportedType;
    string zone;
    bool occupied;

public:
    ParkingSlot() : supportedType(VehicleType::Car), occupied(false) {}

    ParkingSlot(const string& id, VehicleType type, const string& z)
        : slotId(id), supportedType(type), zone(z), occupied(false) {}

    string getSlotId() const {
        return slotId;
    }

    VehicleType getSupportedType() const {
        return supportedType;
    }

    string getZone() const {
        return zone;
    }

    bool isOccupied() const {
        return occupied;
    }

    void setOccupied(bool status) {
        occupied = status;
    }

    void display() const override {
        cout << left
             << setw(10) << slotId
             << setw(14) << vehicleTypeToString(supportedType)
             << setw(12) << zone
             << setw(12) << (occupied ? "Occupied" : "Available")
             << "\n";
    }
};

// ---------------------------------------------------------------------------
// Task 2: Vehicle Entry Management
// ---------------------------------------------------------------------------

class Vehicle {
private:
    string plateNumber;
    VehicleType vehicleType;
    time_t entryTime;
    string allocatedSlot;

public:
    Vehicle() : vehicleType(VehicleType::Car), entryTime(0) {}

    Vehicle(const string& plate,
            VehicleType type,
            time_t entry,
            const string& slot)
        : plateNumber(plate),
          vehicleType(type),
          entryTime(entry),
          allocatedSlot(slot) {}

    string getPlateNumber() const {
        return plateNumber;
    }

    VehicleType getVehicleType() const {
        return vehicleType;
    }

    time_t getEntryTime() const {
        return entryTime;
    }

    string getAllocatedSlot() const {
        return allocatedSlot;
    }
};

// ---------------------------------------------------------------------------
// Task 4: Parking transaction history
// ---------------------------------------------------------------------------

class ParkingTransaction : public Displayable {
private:
    string plateNumber;
    VehicleType vehicleType;
    string slotId;
    time_t entryTime;
    time_t exitTime;
    int hoursParked;
    double fee;

public:
    ParkingTransaction(const string& plate,
                       VehicleType type,
                       const string& slot,
                       time_t entry,
                       time_t exitT,
                       int hours,
                       double amount)
        : plateNumber(plate),
          vehicleType(type),
          slotId(slot),
          entryTime(entry),
          exitTime(exitT),
          hoursParked(hours),
          fee(amount) {}

    double getFee() const {
        return fee;
    }

    time_t getExitTime() const {
        return exitTime;
    }

    time_t getEntryTime() const {
        return entryTime;
    }

    string getPlateNumber() const {
        return plateNumber;
    }

    VehicleType getVehicleType() const {
        return vehicleType;
    }

    string getSlotId() const {
        return slotId;
    }

    int getHoursParked() const {
        return hoursParked;
    }

    void writeTo(ostream& out) const {
        out << left
            << setw(12) << plateNumber
            << setw(14) << vehicleTypeToString(vehicleType)
            << setw(10) << slotId
            << setw(8) << hoursParked
            << setw(12) << fixed << setprecision(0) << fee
            << setw(20) << formatDateTime(entryTime)
            << setw(20) << formatDateTime(exitTime)
            << "\n";
    }

    void writeCsvTo(ostream& out) const {
        out << plateNumber << ","
            << vehicleTypeToString(vehicleType) << ","
            << slotId << ","
            << hoursParked << ","
            << fixed << setprecision(0) << fee << ","
            << formatDateTime(entryTime) << ","
            << formatDateTime(exitTime) << "\n";
    }

    void display() const override {
        cout << left
             << setw(12) << plateNumber
             << setw(14) << vehicleTypeToString(vehicleType)
             << setw(10) << slotId
             << setw(8) << hoursParked
             << setw(12) << fixed << setprecision(0) << fee
             << setw(20) << formatDateTime(exitTime)
             << "\n";
    }
};

// ---------------------------------------------------------------------------
// Tariff manager (polymorphic access to active rates)
// ---------------------------------------------------------------------------

class TariffManager {
private:
    vector<unique_ptr<TariffPolicy>> policies;

public:
    TariffManager() {
        policies.push_back(make_unique<MotorcycleTariff>(500.0));
        policies.push_back(make_unique<CarTariff>(1000.0));
        policies.push_back(make_unique<TruckTariff>(1500.0));
    }

    TariffPolicy* getPolicy(VehicleType type) {
        for (auto& policy : policies) {
            if (policy->getVehicleType() == type) {
                return policy.get();
            }
        }
        return nullptr;
    }

    const TariffPolicy* getPolicy(VehicleType type) const {
        for (const auto& policy : policies) {
            if (policy->getVehicleType() == type) {
                return policy.get();
            }
        }
        return nullptr;
    }

    bool updateRate(VehicleType type, double newRate) {
        TariffPolicy* policy = getPolicy(type);
        if (!policy) {
            return false;
        }

        policy->setHourlyRate(newRate);
        return true;
    }

    void showRates() const {
        cout << left
             << setw(14) << "Vehicle Type"
             << setw(18) << "Rate (RWF/hour)"
             << "\n";
        printLine('-', 32);

        for (const auto& policy : policies) {
            cout << left
                 << setw(14) << policy->getTypeName()
                 << setw(18) << fixed << setprecision(0) << policy->getHourlyRate()
                 << "\n";
        }
    }
};

// ---------------------------------------------------------------------------
// Core system manager
// ---------------------------------------------------------------------------

class ParkingManager {
private:
    vector<ParkingSlot> slots;
    unordered_map<string, Vehicle> activeVehicles;
    vector<ParkingTransaction> history;
    TariffManager tariffManager;

    bool slotExists(const string& slotId) const {
        for (const auto& slot : slots) {
            if (slot.getSlotId() == slotId) {
                return true;
            }
        }
        return false;
    }

    int findAvailableSlot(VehicleType type) const {
        for (int i = 0; i < static_cast<int>(slots.size()); ++i) {
            if (!slots[i].isOccupied() && slots[i].getSupportedType() == type) {
                return i;
            }
        }
        return -1;
    }

    bool releaseSlot(const string& slotId) {
        for (auto& slot : slots) {
            if (slot.getSlotId() == slotId) {
                slot.setOccupied(false);
                return true;
            }
        }
        return false;
    }

    int calculateBilledHours(time_t entryTime, time_t exitTime) const {
        double seconds = difftime(exitTime, entryTime);
        if (seconds < 0) {
            seconds = 0;
        }
        return max(1, static_cast<int>(ceil(seconds / 3600.0)));
    }

    void printSlotTableHeader() const {
        cout << left
             << setw(10) << "Slot ID"
             << setw(14) << "Vehicle Type"
             << setw(12) << "Zone"
             << setw(12) << "Status"
             << "\n";
        printLine('-', 48);
    }

    void printParkedVehicleHeader() const {
        cout << left
             << setw(12) << "Plate"
             << setw(14) << "Vehicle Type"
             << setw(10) << "Slot"
             << setw(20) << "Entry Time"
             << "\n";
        printLine('-', 56);
    }

    void printHistoryHeader() const {
        cout << left
             << setw(12) << "Plate"
             << setw(14) << "Vehicle Type"
             << setw(10) << "Slot"
             << setw(8) << "Hours"
             << setw(12) << "Fee (RWF)"
             << setw(20) << "Exit Time"
             << "\n";
        printLine('-', 76);
    }

    void writeHistoryHeader(ostream& out) const {
        out << left
            << setw(12) << "Plate"
            << setw(14) << "Vehicle Type"
            << setw(10) << "Slot"
            << setw(8) << "Hours"
            << setw(12) << "Fee (RWF)"
            << setw(20) << "Entry Time"
            << setw(20) << "Exit Time"
            << "\n";
        writeFileLine(out, '-', 96);
    }

    struct RevenueSummary {
        time_t reportTime;
        int dailyTransactions;
        double dailyTotal;
        double allTimeTotal;
    };

    RevenueSummary buildRevenueSummary() const {
        RevenueSummary summary {};
        summary.reportTime = time(nullptr);
        summary.dailyTransactions = 0;
        summary.dailyTotal = 0.0;
        summary.allTimeTotal = 0.0;

        for (const auto& transaction : history) {
            summary.allTimeTotal += transaction.getFee();

            if (isSameDay(transaction.getExitTime(), summary.reportTime)) {
                summary.dailyTotal += transaction.getFee();
                summary.dailyTransactions++;
            }
        }

        return summary;
    }

public:
    void loadDemoData() {
        if (!slots.empty()) {
            printMessage("Demo data already loaded or slots exist.", false);
            return;
        }

        slots.push_back(ParkingSlot("M01", VehicleType::Motorcycle, "Zone-A"));
        slots.push_back(ParkingSlot("M02", VehicleType::Motorcycle, "Zone-A"));
        slots.push_back(ParkingSlot("C01", VehicleType::Car, "Zone-B"));
        slots.push_back(ParkingSlot("C02", VehicleType::Car, "Zone-B"));
        slots.push_back(ParkingSlot("T01", VehicleType::Truck, "Zone-C"));

        printMessage("Demo parking slots loaded successfully.");
    }

    void addParkingSlot() {
        printTitle("CONFIGURE PARKING SLOT");

        string id;
        cout << "Slot ID (unique, alphanumeric): ";
        cin >> id;

        if (!isValidIdentifier(id)) {
            printMessage("Invalid Slot ID. Use letters, numbers, '-' or '_'.", false);
            return;
        }

        if (slotExists(id)) {
            printMessage("Slot ID already exists. Each slot must be unique.", false);
            return;
        }

        string typeInput;
        VehicleType type;
        cout << "Supported Vehicle Type (Motorcycle/Car/Truck): ";
        cin >> typeInput;

        if (!parseVehicleType(typeInput, type)) {
            printMessage("Invalid vehicle type. Choose Motorcycle, Car, or Truck.", false);
            return;
        }

        string zone;
        cout << "Zone (location): ";
        cin >> zone;

        if (zone.empty() || !isValidIdentifier(zone)) {
            printMessage("Invalid zone. Use letters, numbers, '-' or '_'.", false);
            return;
        }

        slots.push_back(ParkingSlot(id, type, zone));
        printMessage("Parking slot configured successfully.");
    }

    void registerVehicleEntry() {
        printTitle("REGISTER VEHICLE ENTRY");

        if (slots.empty()) {
            printMessage("No parking slots configured. Add slots first (Menu 1).", false);
            return;
        }

        string plate;
        cout << "Vehicle Plate Number (unique): ";
        cin >> plate;

        if (!isValidIdentifier(plate)) {
            printMessage("Invalid plate number format.", false);
            return;
        }

        if (activeVehicles.find(plate) != activeVehicles.end()) {
            printMessage("Vehicle is already parked. A vehicle cannot park twice.", false);
            return;
        }

        string typeInput;
        VehicleType type;
        cout << "Vehicle Type (Motorcycle/Car/Truck): ";
        cin >> typeInput;

        if (!parseVehicleType(typeInput, type)) {
            printMessage("Invalid vehicle type. Choose Motorcycle, Car, or Truck.", false);
            return;
        }

        int slotIndex = findAvailableSlot(type);
        if (slotIndex == -1) {
            printMessage("No suitable parking slot available for this vehicle type.", false);
            return;
        }

        time_t entryTime = time(nullptr);
        slots[slotIndex].setOccupied(true);

        activeVehicles[plate] = Vehicle(
            plate,
            type,
            entryTime,
            slots[slotIndex].getSlotId()
        );

        printMessage("Vehicle entry registered successfully.");
        cout << left
             << setw(18) << "Allocated Slot"
             << ": " << slots[slotIndex].getSlotId() << "\n";
        cout << left
             << setw(18) << "Zone"
             << ": " << slots[slotIndex].getZone() << "\n";
        cout << left
             << setw(18) << "Entry Time"
             << ": " << formatDateTime(entryTime) << "\n";
    }

    void registerVehicleExit() {
        printTitle("REGISTER VEHICLE EXIT");

        string plate;
        cout << "Vehicle Plate Number: ";
        cin >> plate;

        auto iterator = activeVehicles.find(plate);
        if (iterator == activeVehicles.end()) {
            printMessage("Vehicle not found among currently parked vehicles.", false);
            return;
        }

        Vehicle vehicle = iterator->second;
        time_t exitTime = time(nullptr);
        int billedHours = calculateBilledHours(vehicle.getEntryTime(), exitTime);

        const TariffPolicy* policy = tariffManager.getPolicy(vehicle.getVehicleType());
        if (!policy) {
            printMessage("Tariff policy not found for this vehicle type.", false);
            return;
        }

        double fee = policy->calculateFee(billedHours);

        if (!releaseSlot(vehicle.getAllocatedSlot())) {
            printMessage("Warning: allocated slot could not be released.", false);
        }

        history.push_back(
            ParkingTransaction(
                vehicle.getPlateNumber(),
                vehicle.getVehicleType(),
                vehicle.getAllocatedSlot(),
                vehicle.getEntryTime(),
                exitTime,
                billedHours,
                fee
            )
        );

        activeVehicles.erase(iterator);

        printTitle("PARKING RECEIPT");
        cout << left << setw(18) << "Plate Number" << ": " << vehicle.getPlateNumber() << "\n";
        cout << left << setw(18) << "Vehicle Type" << ": " << vehicleTypeToString(vehicle.getVehicleType()) << "\n";
        cout << left << setw(18) << "Slot ID" << ": " << vehicle.getAllocatedSlot() << "\n";
        cout << left << setw(18) << "Entry Time" << ": " << formatDateTime(vehicle.getEntryTime()) << "\n";
        cout << left << setw(18) << "Exit Time" << ": " << formatDateTime(exitTime) << "\n";
        cout << left << setw(18) << "Hours Billed" << ": " << billedHours << " hour(s)\n";
        cout << left << setw(18) << "Rate Applied" << ": " << fixed << setprecision(0)
             << policy->getHourlyRate() << " RWF/hour\n";
        printLine('-', 40);
        cout << left << setw(18) << "TOTAL FEE" << ": " << fixed << setprecision(0) << fee << " RWF\n";
        printMessage("Payment recorded. Slot released and history updated.");
    }

    void updatePrices() {
        printTitle("UPDATE PARKING PRICES");

        cout << "Select vehicle type to update:\n";
        cout << "  1. Motorcycle\n";
        cout << "  2. Car\n";
        cout << "  3. Truck\n";
        cout << "Choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printMessage("Invalid input. Enter a number.", false);
            return;
        }

        VehicleType type;
        switch (choice) {
            case 1:
                type = VehicleType::Motorcycle;
                break;
            case 2:
                type = VehicleType::Car;
                break;
            case 3:
                type = VehicleType::Truck;
                break;
            default:
                printMessage("Invalid choice. Select 1, 2, or 3.", false);
                return;
        }

        double newRate;
        cout << "Enter new hourly rate (RWF): ";
        if (!(cin >> newRate) || newRate <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printMessage("Invalid price. Enter a positive number.", false);
            return;
        }

        if (tariffManager.updateRate(type, newRate)) {
            printMessage("Price updated. Future exits will use the new rate.");
            printMessage("Completed transactions in history remain unchanged.", true);
        } else {
            printMessage("Failed to update price.", false);
        }
    }

    void showAvailableSlots() const {
        printTitle("AVAILABLE PARKING SLOTS");
        printSlotTableHeader();

        bool found = false;
        for (const auto& slot : slots) {
            if (!slot.isOccupied()) {
                slot.display();
                found = true;
            }
        }

        if (!found) {
            printMessage("No available slots at the moment.", false);
        }
    }

    void showAllSlots() const {
        printTitle("ALL PARKING SLOTS");
        printSlotTableHeader();

        if (slots.empty()) {
            printMessage("No parking slots configured yet.", false);
            return;
        }

        for (const auto& slot : slots) {
            slot.display();
        }
    }

    void showParkedVehicles() const {
        printTitle("CURRENTLY PARKED VEHICLES");
        printParkedVehicleHeader();

        if (activeVehicles.empty()) {
            printMessage("No vehicles are currently parked.", false);
            return;
        }

        for (const auto& entry : activeVehicles) {
            const Vehicle& vehicle = entry.second;
            cout << left
                 << setw(12) << vehicle.getPlateNumber()
                 << setw(14) << vehicleTypeToString(vehicle.getVehicleType())
                 << setw(10) << vehicle.getAllocatedSlot()
                 << setw(20) << formatDateTime(vehicle.getEntryTime())
                 << "\n";
        }
    }

    void showHistory() const {
        printTitle("PARKING TRANSACTION HISTORY");
        printHistoryHeader();

        if (history.empty()) {
            printMessage("No completed parking transactions yet.", false);
            return;
        }

        for (const auto& transaction : history) {
            transaction.display();
        }
    }

    void showRevenue() const {
        printTitle("DAILY REVENUE REPORT");

        RevenueSummary summary = buildRevenueSummary();

        cout << left << setw(24) << "Report Date" << ": " << formatDateTime(summary.reportTime) << "\n";
        cout << left << setw(24) << "Transactions Today" << ": " << summary.dailyTransactions << "\n";
        printLine('-', 40);
        cout << left << setw(24) << "Daily Revenue (RWF)" << ": "
             << fixed << setprecision(0) << summary.dailyTotal << "\n";
        cout << left << setw(24) << "All-Time Revenue (RWF)" << ": "
             << fixed << setprecision(0) << summary.allTimeTotal << "\n";
    }

    bool saveHistoryToFile(const string& filename) const {
        ofstream file(filename, ios::out | ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        file << "SMART PARKING MANAGEMENT SYSTEM\n";
        file << "PARKING HISTORY REPORT\n";
        file << "Generated: " << formatDateTime(time(nullptr)) << "\n";
        writeFileLine(file, '=', 96);
        file << "Total Records: " << history.size() << "\n\n";

        if (history.empty()) {
            file << "No completed parking transactions found.\n";
            return file.good();
        }

        writeHistoryHeader(file);
        for (const auto& transaction : history) {
            transaction.writeTo(file);
        }

        file << "\n";
        writeFileLine(file, '=', 96);
        file << "CSV FORMAT\n";
        file << "Plate,Vehicle Type,Slot,Hours,Fee (RWF),Entry Time,Exit Time\n";
        for (const auto& transaction : history) {
            transaction.writeCsvTo(file);
        }

        return file.good();
    }

    bool saveRevenueToFile(const string& filename) const {
        ofstream file(filename, ios::out | ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        RevenueSummary summary = buildRevenueSummary();

        file << "SMART PARKING MANAGEMENT SYSTEM\n";
        file << "DAILY REVENUE REPORT\n";
        file << "Generated: " << formatDateTime(summary.reportTime) << "\n";
        writeFileLine(file, '=', 60);
        file << left << setw(28) << "Report Date" << ": " << formatDateTime(summary.reportTime) << "\n";
        file << left << setw(28) << "Transactions Today" << ": " << summary.dailyTransactions << "\n";
        writeFileLine(file, '-', 60);
        file << left << setw(28) << "Daily Revenue (RWF)" << ": "
             << fixed << setprecision(0) << summary.dailyTotal << "\n";
        file << left << setw(28) << "All-Time Revenue (RWF)" << ": "
             << fixed << setprecision(0) << summary.allTimeTotal << "\n";
        file << "\n";
        writeFileLine(file, '=', 60);
        file << "CSV FORMAT\n";
        file << "Metric,Value\n";
        file << "Report Date," << formatDateTime(summary.reportTime) << "\n";
        file << "Transactions Today," << summary.dailyTransactions << "\n";
        file << "Daily Revenue (RWF)," << fixed << setprecision(0) << summary.dailyTotal << "\n";
        file << "All-Time Revenue (RWF)," << fixed << setprecision(0) << summary.allTimeTotal << "\n";

        if (!history.empty()) {
            file << "\nTODAY'S TRANSACTIONS\n";
            file << "Plate,Vehicle Type,Slot,Hours,Fee (RWF),Entry Time,Exit Time\n";

            for (const auto& transaction : history) {
                if (isSameDay(transaction.getExitTime(), summary.reportTime)) {
                    transaction.writeCsvTo(file);
                }
            }
        }

        return file.good();
    }

    void exportReports() {
        printTitle("EXPORT REPORTS TO FILE");

        cout << "Select report to save:\n";
        cout << "  1. Parking History\n";
        cout << "  2. Daily Revenue\n";
        cout << "  3. Both Reports\n";
        cout << "Choice: ";

        int reportChoice;
        if (!(cin >> reportChoice) || reportChoice < 1 || reportChoice > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printMessage("Invalid report choice. Select 1, 2, or 3.", false);
            return;
        }

        string filename;
        cout << "Enter filename (example: parking_report.txt): ";
        cin >> filename;

        if (!isValidFilename(filename)) {
            printMessage("Invalid filename. Avoid empty names and special characters.", false);
            return;
        }

        bool historySaved = true;
        bool revenueSaved = true;

        if (reportChoice == 1 || reportChoice == 3) {
            string historyFile = filename;
            if (reportChoice == 3 && filename.find('.') == string::npos) {
                historyFile += "_history.txt";
            } else if (reportChoice == 3) {
                const auto dotPos = filename.find_last_of('.');
                historyFile = filename.substr(0, dotPos) + "_history" + filename.substr(dotPos);
            }

            historySaved = saveHistoryToFile(historyFile);
            if (historySaved) {
                printMessage("Parking history saved to: " + historyFile);
            } else {
                printMessage("Failed to save parking history.", false);
            }
        }

        if (reportChoice == 2 || reportChoice == 3) {
            string revenueFile = filename;
            if (reportChoice == 3 && filename.find('.') == string::npos) {
                revenueFile += "_revenue.txt";
            } else if (reportChoice == 3) {
                const auto dotPos = filename.find_last_of('.');
                revenueFile = filename.substr(0, dotPos) + "_revenue" + filename.substr(dotPos);
            }

            revenueSaved = saveRevenueToFile(revenueFile);
            if (revenueSaved) {
                printMessage("Daily revenue saved to: " + revenueFile);
            } else {
                printMessage("Failed to save daily revenue.", false);
            }
        }
    }

    void showCurrentRates() const {
        printTitle("CURRENT ACTIVE PARKING RATES");
        tariffManager.showRates();
    }
};

void showMainMenu() {
    cout << "\n";
    printLine();
    cout << "       SMART PARKING MANAGEMENT SYSTEM\n";
    cout << "                  KIGALI CITY\n";
    printLine();
    cout << "  CONFIGURATION & OPERATIONS\n";
    cout << "   1.  Configure Parking Slot\n";
    cout << "   2.  Register Vehicle Entry\n";
    cout << "   3.  Register Vehicle Exit\n";
    cout << "   4.  Update Parking Prices\n";
    cout << "\n";
    cout << "  REPORTS\n";
    cout << "   5.  View Available Slots\n";
    cout << "   6.  View All Slots\n";
    cout << "   7.  View Parked Vehicles\n";
    cout << "   8.  View Parking History\n";
    cout << "   9.  View Daily Revenue\n";
    cout << "  10.  View Current Rates\n";
    cout << "\n";
    cout << "  UTILITIES\n";
    cout << "  11.  Load Demo/Test Data\n";
    cout << "  12.  Export Reports to File\n";
    cout << "   0.  Exit System\n";
    printLine('-');
    cout << "  Enter your choice: ";
}

int main() {
    ParkingManager system;
    int choice = -1;

    cout << fixed << setprecision(0);

    do {
        showMainMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printMessage("Invalid input. Please enter a number.", false);
            continue;
        }

        switch (choice) {
            case 1:
                system.addParkingSlot();
                pauseScreen();
                break;
            case 2:
                system.registerVehicleEntry();
                pauseScreen();
                break;
            case 3:
                system.registerVehicleExit();
                pauseScreen();
                break;
            case 4:
                system.updatePrices();
                pauseScreen();
                break;
            case 5:
                system.showAvailableSlots();
                pauseScreen();
                break;
            case 6:
                system.showAllSlots();
                pauseScreen();
                break;
            case 7:
                system.showParkedVehicles();
                pauseScreen();
                break;
            case 8:
                system.showHistory();
                pauseScreen();
                break;
            case 9:
                system.showRevenue();
                pauseScreen();
                break;
            case 10:
                system.showCurrentRates();
                pauseScreen();
                break;
            case 11:
                system.loadDemoData();
                pauseScreen();
                break;
            case 12:
                system.exportReports();
                pauseScreen();
                break;
            case 0:
                printTitle("SYSTEM SHUTDOWN");
                printMessage("Thank you for using Smart Parking Management System.");
                break;
            default:
                printMessage("Invalid menu choice. Select a number from 0 to 12.", false);
                pauseScreen();
                break;
        }
    } while (choice != 0);

    return 0;
}
