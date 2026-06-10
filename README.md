# Smart Parking Management System

A console-based Smart Parking Management System for Kigali City, implemented in C++ using in-memory data structures only.

## Default Parking Rates

| Vehicle Type | Rate (RWF/hour) |
|--------------|-----------------|
| Motorcycle   | 500             |
| Car          | 1,000           |
| Truck        | 1,500           |

**Billing rule:** Fees are calculated only at vehicle exit. Partial hours are charged as full hours (e.g. 15 minutes = 1 hour, 1 hour 20 minutes = 2 hours).

## How to Compile

```bash
g++ -std=c++17 -o SmartParking Ndahiro_SmartParkingSystem.cpp
```

On Windows with MinGW or g++:

```bash
g++ -std=c++17 -o SmartParking.exe Ndahiro_SmartParkingSystem.cpp
```

## How to Run

```bash
./SmartParking
```

Windows:

```bash
SmartParking.exe
```

## Menu Options

| Option | Description |
|--------|-------------|
| 1 | Configure a new parking slot (ID, vehicle type, zone) |
| 2 | Register vehicle entry and auto-allocate a matching slot |
| 3 | Register vehicle exit, calculate fee, release slot, save history |
| 4 | Update hourly parking price for Motorcycle, Car, or Truck |
| 5 | View all available (unoccupied) slots |
| 6 | View all configured slots |
| 7 | View currently parked vehicles |
| 8 | View completed parking transaction history |
| 9 | View daily revenue and all-time revenue |
| 10 | View current active parking rates |
| 11 | Load demo/test parking slots |
| 0 | Exit the system |

## Suggested Test Flow

1. Choose **11** to load demo slots.
2. Choose **2** to register entry:
   - Plate: `RAB123A`
   - Type: `Car`
3. Choose **7** to confirm the vehicle is parked.
4. Choose **5** to see remaining available slots.
5. Choose **3** to register exit for `RAB123A` and view the receipt.
6. Choose **4** to update Car rate to `1200`.
7. Park another car, exit it, and confirm the new rate applies only to new exits.
8. Choose **8** and **9** to review history and revenue.

## Data Structure Choices

- `vector<ParkingSlot>` — stores all slots; supports traversal for reports and slot allocation.
- `unordered_map<string, Vehicle>` — O(1) lookup of active vehicles by plate number.
- `vector<ParkingTransaction>` — append-only history of completed transactions.
- `vector<unique_ptr<TariffPolicy>>` — polymorphic tariff policies for fee calculation.

## OOP Design

- **Encapsulation:** Private fields with public accessors in all entity classes.
- **Abstraction:** `Displayable` and `TariffPolicy` abstract base classes.
- **Inheritance:** `ParkingSlot`/`ParkingTransaction` extend `Displayable`; tariff classes extend `TariffPolicy`.
- **Polymorphism:** Virtual `display()` and `calculateFee()` methods.
