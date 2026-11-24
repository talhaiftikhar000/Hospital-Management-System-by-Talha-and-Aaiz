#include "filehandelling.h"
#include "HospitalData.h"
#include <fstream> //created to open the ofstream and ifstream to read and write the data to the file
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

// For the simple | format, fields must not contain the | character.
static bool safeField(const string& s) {
    return s.find('|') == string::npos &&
           s.find('\n') == string::npos &&
           s.find('\r') == string::npos;
}
static vector<string> split(const string& line) {
    vector<string> fields;
    istringstream stream(line);
    string field;
    while (getline(stream, field, '|')) fields.push_back(field);
    return fields;
}
static bool readNumber(const string& s, int& value) {
    try {
        size_t used = 0;
        value = stoi(s, &used);
        return used == s.size();
    } catch (...) {
        return false;
    }
}
bool FileHandling::savePatients(const PatientList& patients, const string& filename) {
    // Validate before opening: otherwise an invalid field could destroy old records.
    for (PatientNode* node = patients.head; node; node = node->next) {
        const Patient& p = node->data;
        if (!safeField(p.name) || !safeField(p.gender) ||
            !safeField(p.contact) || !safeField(p.disease)) {
            cerr << "Cannot save patient with a | or newline in a field.\n";
            return false;
        }
    }

    ofstream file(filename.c_str()); // rewrite entire current list
    if (!file) { cerr << "Unable to open " << filename << " for writing.\n"; return false; }
    for (PatientNode* node = patients.head; node; node = node->next) {
        const Patient& p = node->data;
        file << p.id << '|' << p.name << '|' << p.age << '|'
             << p.gender << '|' << p.contact << '|' << p.disease << '\n';
    }
    file.close();
    return !file.fail();
}

bool FileHandling::loadPatients(PatientList& patients, const string& filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "No saved patient file. Starting with existing/empty records.\n";
        return true; // a normal first run
    }

    string line;
    int loaded = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> f = split(line);
        int id = 0, age = 0;
        if (f.size() != 6 || !readNumber(f[0], id) ||
            !readNumber(f[2], age) || id <= 0 || age < 0) {
            cerr << "Skipping invalid patient row: " << line << '\n';
            continue;
        }
        if (patients.registerPatient(Patient(id, f[1], age, f[3], f[4], f[5]))) {
            ++loaded;
        }
    }
    cout << "Restored " << loaded << " patients.\n";
    return !file.bad();
}

bool FileHandling::saveAppointments(const AppointmentScheduler& scheduler,
                                     const string& filename) {
    for (int list = 0; list < 2; ++list) {
        Appointment* node = list == 0 ? scheduler.front : scheduler.historyHead;
        for (; node; node = node->next) {
            if (!safeField(node->patientName) || !safeField(node->doctorName) ||
                !safeField(node->date) || !safeField(node->time) ||
                !safeField(node->status)) {
                cerr << "Cannot save appointment with | or newline in a field.\n";
                return false;
            }
        }
    }

    ofstream file(filename.c_str());
    if (!file) { cerr << "Unable to open " << filename << " for writing.\n"; return false; }
    // Scheduled appointments first: preserve queue order (FIFO) after restart.
    for (int list = 0; list < 2; ++list) {
        Appointment* node = list == 0 ? scheduler.front : scheduler.historyHead;
        for (; node; node = node->next) {
            file << node->appointmentID << '|' << node->patientID << '|'
                 << node->patientName << '|' << node->doctorName << '|'
                 << node->date << '|' << node->time << '|'
                 << node->status << '\n';
        }
    }
    file.close();
    return !file.fail();
}

bool FileHandling::loadAppointments(AppointmentScheduler& scheduler,
                                     const string& filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "No saved appointments file. Starting with empty queue.\n";
        return true;
    }

    // Call once at program startup, before new bookings are made.
    scheduler.freeList(scheduler.front);
    scheduler.freeList(scheduler.historyHead);
    scheduler.front = scheduler.rear = nullptr;
    scheduler.historyHead = scheduler.historyTail = nullptr;
    scheduler.pendingCount = scheduler.historyCount = 0;
    scheduler.nextAppointmentID = 1001;

    string line;
    int loaded = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> f = split(line);
        int appID = 0, patientID = 0;
        if (f.size() != 7 || !readNumber(f[0], appID) ||
            !readNumber(f[1], patientID) || appID <= 0 || patientID <= 0 ||
            (f[6] != STATUS_SCHEDULED && f[6] != STATUS_COMPLETED &&
             f[6] != STATUS_CANCELLED)) {
            cerr << "Skipping invalid appointment row: " << line << '\n';
            continue;
        }
        Appointment* node = new Appointment(appID, patientID, f[2], f[3],
                                             f[4], f[5], f[6]);
        if (f[6] == STATUS_SCHEDULED) {
            scheduler.enqueue(node);
        } else {
            scheduler.addToHistory(node, f[6]);
        }
        if (appID < numeric_limits<int>::max())
            scheduler.nextAppointmentID = max(scheduler.nextAppointmentID, appID + 1);
        ++loaded;
    }
    cout << "Restored " << loaded << " appointments.\n";
    return !file.bad();
}
