# Smart Parking System — Test Guide

Use this file to manually test the system after compiling and running it.

---

## Quick Start

1. Compile:
   ```bash
   g++ -std=c++17 -o SmartParking.exe Ndahiro_SmartParkingSystem.cpp
   ```
2. Run:
   ```bash
   SmartParking.exe
   ```
3. Start with **Menu 11** to load demo slots, then follow the tests below.

---

## Test 1: Load Demo Data

| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `11` | Success message: demo slots loaded |
| 2 | Menu `6` | Shows 5 slots: M01, M02, C01, C02, T01 |
| 3 | Menu `5` | All 5 slots show as Available |

**Pass if:** demo slots appear and all are available.

---

## Test 2: Configure Parking Slot (Task 1)

### Valid slot
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `1` | Slot configuration screen opens |
| 2 | Slot ID: `C03` | Accepted |
| 3 | Type: `Car` | Accepted |
| 4 | Zone: `Zone-B` | Success message shown |
| 5 | Menu `6` | `C03` appears in slot list |

### Duplicate slot ID (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `1` | Slot configuration screen opens |
| 2 | Slot ID: `C01` | Error: slot already exists |

### Invalid vehicle type (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `1` | Slot configuration screen opens |
| 2 | Slot ID: `X01` | Accepted |
| 3 | Type: `Bus` | Error: invalid vehicle type |

**Pass if:** valid slots are added, duplicates and invalid types are rejected.

---

## Test 3: Vehicle Entry (Task 2)

### Valid car entry
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `2` | Entry screen opens |
| 2 | Plate: `RAB123A` | Accepted |
| 3 | Type: `Car` | Success + slot allocated (e.g. C01) |
| 4 | Menu `7` | `RAB123A` appears as parked |
| 5 | Menu `5` | C01 no longer in available slots |

### Duplicate entry (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `2` | Entry screen opens |
| 2 | Plate: `RAB123A` | Error: vehicle already parked |
| 3 | Type: `Car` | — |

### No matching slot (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Park cars until both C01 and C02 are occupied | Both car slots full |
| 2 | Menu `2` | Entry screen opens |
| 3 | Plate: `RAB999Z` | — |
| 4 | Type: `Car` | Error: no suitable slot available |

### Entry with no slots (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Restart program (fresh run) | Empty system |
| 2 | Menu `2` | Entry screen opens |
| 3 | Plate: `RAB111A` | — |
| 4 | Type: `Car` | Error: no parking slots configured |

**Pass if:** entry works, duplicate parking is blocked, and full/no-slot cases fail gracefully.

---

## Test 4: Vehicle Exit and Fee (Tasks 3 & 4)

### Basic exit and receipt
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Ensure `RAB123A` is parked | Vehicle in parked list |
| 2 | Menu `3` | Exit screen opens |
| 3 | Plate: `RAB123A` | Receipt shown with hours and fee |
| 4 | Menu `7` | `RAB123A` no longer parked |
| 5 | Menu `5` | Released slot is available again |
| 6 | Menu `8` | Transaction saved in history |

### Exit unknown vehicle (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `3` | Exit screen opens |
| 2 | Plate: `UNKNOWN1` | Error: vehicle not found |

### Fee check (minimum 1 hour)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `2`, plate `MOTO01`, type `Motorcycle` | Parked successfully |
| 2 | Menu `3`, plate `MOTO01` | Exit immediately |
| 3 | Check receipt | Billed at least 1 hour, fee = 500 RWF |

**Pass if:** exit releases slot, shows receipt, updates history, and unknown plates are rejected.

---

## Test 5: Update Parking Prices (Task 3)

| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `10` | Shows default rates: 500 / 1000 / 1500 |
| 2 | Menu `4`, choice `2` (Car), price `1200` | Success message |
| 3 | Menu `10` | Car rate now 1200 |
| 4 | Menu `2`, plate `CARNEW1`, type `Car` | Park car |
| 5 | Menu `3`, plate `CARNEW1` | Fee uses 1200 RWF/hour |
| 6 | Menu `8` | Old transactions still show old fees |

### Invalid price (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `4`, choice `1`, price `-100` | Error: invalid price |

**Pass if:** new exits use updated rate, but old history fees stay unchanged.

---

## Test 6: Export Reports to File

| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Complete at least one vehicle exit first | History has records |
| 2 | Menu `12`, choice `1` | Export history screen opens |
| 3 | Filename: `history_report.txt` | Success message shown |
| 4 | Open `history_report.txt` | File contains history table + CSV rows |
| 5 | Menu `12`, choice `2` | Export revenue screen opens |
| 6 | Filename: `revenue_report.txt` | Success message shown |
| 7 | Open `revenue_report.txt` | File contains daily revenue summary + CSV |
| 8 | Menu `12`, choice `3`, filename `full_report.txt` | Two files created: `_history` and `_revenue` |

### Invalid filename (should fail)
| Step | Input | Expected Result |
|------|-------|-----------------|
| 1 | Menu `12`, choice `1` | Export screen opens |
| 2 | Filename: `bad\name.txt` | Error: invalid filename |

**Pass if:** reports are saved correctly and files can be opened outside the program.

---

## Test 7: Reports

| Step | Menu | What to Check |
|------|------|---------------|
| 1 | `5` | Shows only available slots |
| 2 | `6` | Shows all slots with status |
| 3 | `7` | Shows only currently parked vehicles |
| 4 | `8` | Shows all completed transactions |
| 5 | `9` | Shows daily revenue and all-time revenue |
| 6 | `10` | Shows current active rates |

**Pass if:** all six reports display correctly.

---

## Test 8: Full End-to-End Scenario

Run this full flow in order:

```
11          → Load demo data
6           → View all slots
2 RAB100C Car     → Park car
2 RAB200M Motorcycle → Park motorcycle
2 RAB300T Truck   → Park truck
7           → See 3 parked vehicles
5           → Available slots reduced
3 RAB100C   → Exit car, note fee (1000 RWF for 1 hour)
3 RAB200M   → Exit motorcycle, note fee (500 RWF for 1 hour)
3 RAB300T   → Exit truck, note fee (1500 RWF for 1 hour)
8           → History shows 3 records
9           → Revenue = 3000 RWF (if all exited same day)
10          → Rates still correct
0           → Exit system
```

**Pass if:** the full flow completes without errors.

---

## Test 8: Input Validation

| Test | Input | Expected Result |
|------|-------|-----------------|
| Invalid menu choice | `99` | Error: invalid menu choice |
| Invalid menu input | `abc` | Error: invalid input |
| Invalid slot ID | ` `@` or empty | Error: invalid format |
| Invalid plate | special characters | Error: invalid format |

**Pass if:** system handles bad input without crashing.

---

## Final Checklist

- [ ] Slots can be configured with unique IDs
- [ ] Vehicles can enter and get allocated slots
- [ ] Duplicate parking is blocked
- [ ] Full parking shows a clear error
- [ ] Exit calculates fee and releases slot
- [ ] History stores completed transactions
- [ ] Price updates affect new exits only
- [ ] All reports work
- [ ] Invalid inputs are handled safely
- [ ] Program exits cleanly with menu `0`

---

## Notes

- Fees are only calculated at **exit**, not at entry.
- Partial hours are billed as full hours (15 min = 1 hour).
- If you test exit immediately, expect **1 hour minimum** charge.
- For billing over multiple hours, wait before exiting or test logic separately.
