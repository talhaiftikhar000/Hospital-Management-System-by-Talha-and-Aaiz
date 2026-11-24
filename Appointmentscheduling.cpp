#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cctype>
#include <sstream>
#include <limits>

using namespace std;

string toStr(int n) {
    ostringstream oss;
    oss << n;
    return oss.str();
}

string toLower(string s) {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = static_cast<char>(tolower(static_cast<unsigned char>(s[i])));
    return s;
}

struct Patient {
    int id, age;
    string name, gender, contact, disease;
    Patient(int i = 0, string n = "", int a = 0, string g = "", string c = "", string d = "")
        : id(i), age(a), name(n), gender(g), contact(c), disease(d) {}
    void display() const {
        cout << "  [" << setw(4) << id << "] " << left << setw(18) << name
             << " | Age: " << setw(3) << age
             << " | " << setw(6) << gender
             << " | " << setw(13) << contact
             << " | " << disease << "\n";
    }
};

struct PatientNode {
    Patient data;
    PatientNode* next;
    PatientNode(Patient p) : data(p), next(NULL) {}
};

class PatientList {
public:
    PatientNode* head;
    PatientList() : head(NULL) {}

    PatientNode* searchById(int id) {
        for (PatientNode* t = head; t != NULL; t = t->next)
            if (t->data.id == id) return t;
        return NULL;
    }

    bool registerPatient(Patient p) {
        if (searchById(p.id) != NULL) return false;
        PatientNode* n = new PatientNode(p);
        n->next = head;
        head = n;
        sortById();
        return true;
    }

    PatientList searchByName(const string& name) {
        PatientList res;
        for (PatientNode* t = head; t != NULL; t = t->next)
            if (toLower(t->data.name) == toLower(name)) res.registerPatient(t->data);
        return res;
    }

    void sortById() {
        PatientNode* sorted = NULL;
        while (head != NULL) {
            PatientNode* next = head->next;
            if (!sorted || head->data.id < sorted->data.id) {
                head->next = sorted; sorted = head;
            } else {
                PatientNode* c = sorted;
                while (c->next && c->next->data.id < head->data.id) c = c->next;
                head->next = c->next; c->next = head;
            }
            head = next;
        }
        head = sorted;
    }

    void sortByName() {
        PatientNode* sorted = NULL;
        while (head != NULL) {
            PatientNode* next = head->next;
            if (!sorted || toLower(head->data.name) < toLower(sorted->data.name)) {
                head->next = sorted; sorted = head;
            } else {
                PatientNode* c = sorted;
                while (c->next && toLower(c->next->data.name) < toLower(head->data.name)) c = c->next;
                head->next = c->next; c->next = head;
            }
            head = next;
        }
        head = sorted;
    }

    bool updatePatient(int id, string name, int age, string gender, string contact, string disease) {
        PatientNode* n = searchById(id);
        if (!n) return false;
        n->data = Patient(id, name, age, gender, contact, disease);
        return true;
    }

    bool deletePatient(int id) {
        PatientNode *curr = head, *prev = NULL;
        while (curr && curr->data.id != id) { prev = curr; curr = curr->next; }
        if (!curr) return false;
        if (!prev) head = curr->next; else prev->next = curr->next;
        delete curr;
        return true;
    }

    void display() const {
        if (!head) { cout << "  No patient records found.\n"; return; }
        for (PatientNode* t = head; t != NULL; t = t->next) t->data.display();
    }
};

struct Appointment {
    int appID, patID;
    string patName, docName, date, time, status;
    Appointment* next;
    Appointment(int a, int p, string pn, string dn, string dt, string tm, string st = "Scheduled")
        : appID(a), patID(p), patName(pn), docName(dn), date(dt), time(tm), status(st), next(NULL) {}
};

class AppointmentScheduler {
    Appointment *front, *rear, *histHead, *histTail;
    int pendingCount, historyCount, nextID;

    string pad2(int n) const { return (n < 10 ? "0" : "") + toStr(n); }

    void freeList(Appointment* h) {
        while (h) { Appointment* t = h; h = h->next; delete t; }
    }

    void moveToHistory(Appointment* n, const string& st) {
        n->status = st; n->next = NULL;
        if (!histHead) histHead = histTail = n;
        else { histTail->next = n; histTail = n; }
        historyCount++;
    }

    void printTable(Appointment* h, const string& title, int total) const {
        if (!h) { cout << "  No records in " << title << ".\n"; return; }
        cout << "\n  " << title << " (Total: " << total << ")\n"
             << "  " << string(76, '-') << "\n"
             << "  " << left << setw(8) << "AppID" << setw(8) << "PatID" << setw(16) << "Patient"
             << setw(15) << "Doctor" << setw(12) << "Date" << setw(7) << "Time" << "Status\n"
             << "  " << string(76, '-') << "\n";
        for (Appointment* c = h; c; c = c->next)
            cout << "  " << left << setw(8) << c->appID << setw(8) << c->patID << setw(16) << c->patName
                 << setw(15) << c->docName << setw(12) << c->date << setw(7) << c->time << c->status << "\n";
    }

public:
    AppointmentScheduler() : front(NULL), rear(NULL), histHead(NULL), histTail(NULL),
                             pendingCount(0), historyCount(0), nextID(1001) {}
    ~AppointmentScheduler() { freeList(front); freeList(histHead); }

    bool validDateTime(int d, int m, int y, int hr, int min) const {
        if (y < 2024 || y > 2100 || m < 1 || m > 12 || hr < 0 || hr > 23 || min < 0 || min > 59) return false;
        int dim[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) dim[1] = 29;
        if (d < 1 || d > dim[m - 1]) return false;

        time_t now = time(NULL);
        tm* t = localtime(&now);
        if (!t) return false;
        int cy = t->tm_year + 1900, cm = t->tm_mon + 1;
        if (y != cy) return y > cy;
        if (m != cm) return m > cm;
        if (d != t->tm_mday) return d > t->tm_mday;
        if (hr != t->tm_hour) return hr > t->tm_hour;
        return min >= t->tm_min;
    }

    bool bookAppointment(int patID, const string& doc, int d, int m, int y, int hr, int min, PatientList& pl) {
        PatientNode* p = pl.searchById(patID);
        if (!p || !validDateTime(d, m, y, hr, min)) return false;

        string dt = toStr(y) + "-" + pad2(m) + "-" + pad2(d), tmStr = pad2(hr) + ":" + pad2(min);
        for (Appointment* c = front; c; c = c->next)
            if (c->docName == doc && c->date == dt && c->time == tmStr) return false;

        Appointment* node = new Appointment(nextID++, patID, p->data.name, doc, dt, tmStr);
        if (!rear) front = rear = node; else { rear->next = node; rear = node; }
        pendingCount++;
        return true;
    }

    bool completeNext() {
        if (!front) return false;
        Appointment* c = front;
        front = front->next;
        if (!front) rear = NULL;
        pendingCount--;
        moveToHistory(c, "Completed");
        return true;
    }

    bool cancelAppointment(int appID) {
        Appointment *c = front, *prev = NULL;
        while (c && c->appID != appID) { prev = c; c = c->next; }
        if (!c) return false;
        if (!prev) front = c->next; else prev->next = c->next;
        if (c == rear) rear = prev;
        pendingCount--;
        moveToHistory(c, "Cancelled");
        return true;
    }

    bool hasPendingAppointment(int patID) const {
        for (Appointment* t = front; t; t = t->next)
            if (t->patID == patID) return true;
        return false;
    }

    void viewPending() const { printTable(front, "PENDING QUEUE", pendingCount); }
    void viewHistory() const { printTable(histHead, "APPOINTMENT HISTORY", historyCount); }
};

int readInt(const string& label) {
    int x;
    while (true) {
        cout << "  " << label;
        if (cin >> x) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid number. Try again.\n";
    }
}

string readLine(const string& label) {
    string s;
    cout << "  " << label;
    getline(cin, s);
    return s;
}

void printBanner() {
    cout << "\n  +==============================================+\n"
         << "  |      HOSPITAL & APPOINTMENT MANAGEMENT       |\n"
         << "  +==============================================+\n";
}

void printMenu() {
    cout << "\n  --- PATIENTS ---\n"
         << "   1. Register Patient        2. View All Patients\n"
         << "   3. Search by ID / Name     4. Update Patient\n"
         << "   5. Delete Patient          6. Sort (ID / Name)\n"
         << "  --- APPOINTMENTS ---\n"
         << "   7. Book Appointment        8. View Pending Queue\n"
         << "   9. Complete Next (FIFO)   10. Cancel Appointment\n"
         << "  11. View History            0. Exit\n";
}

int main() {
    PatientList patients;
    AppointmentScheduler scheduler;
    printBanner();

    while (true) {
        printMenu();
        int choice = readInt("Select option: ");
        cout << "\n";

        if (choice == 0) {
            cout << "  Goodbye. Press Enter to close...";
            cin.get();
            break;
        } else if (choice == 1) {
            int id = readInt("Patient ID : ");
            string name = readLine("Full Name  : ");
            int age = readInt("Age        : ");
            string gender = readLine("Gender     : ");
            string contact = readLine("Contact    : ");
            string disease = readLine("Disease    : ");
            if (patients.registerPatient(Patient(id, name, age, gender, contact, disease)))
                cout << "  [+] Patient registered successfully.\n";
            else
                cout << "  [!] Duplicate ID. Registration failed.\n";
        } else if (choice == 2) {
            patients.display();
        } else if (choice == 3) {
            int mode = readInt("Search by (1 = ID, 2 = Name): ");
            if (mode == 1) {
                PatientNode* n = patients.searchById(readInt("Enter ID: "));
                if (n) n->data.display(); else cout << "  [!] Patient not found.\n";
            } else if (mode == 2) {
                PatientList matches = patients.searchByName(readLine("Enter Name: "));
                matches.display();
            }
        } else if (choice == 4) {
            int id = readInt("Patient ID to update: ");
            string name = readLine("New Name   : ");
            int age = readInt("New Age    : ");
            string gender = readLine("New Gender : ");
            string contact = readLine("New Contact: ");
            string disease = readLine("New Disease: ");
            cout << (patients.updatePatient(id, name, age, gender, contact, disease)
                        ? "  [+] Record updated.\n" : "  [!] Patient ID not found.\n");
        } else if (choice == 5) {
            int id = readInt("Patient ID to delete: ");
            if (scheduler.hasPendingAppointment(id)) {
                cout << "  [!] Cannot delete: patient has a pending appointment.\n";
            } else {
                cout << (patients.deletePatient(id) ? "  [+] Patient deleted.\n" : "  [!] Patient ID not found.\n");
            }
        } else if (choice == 6) {
            int s = readInt("Sort by (1 = ID, 2 = Name): ");
            if (s == 1) { patients.sortById(); cout << "  [+] Sorted by ID.\n"; }
            else if (s == 2) { patients.sortByName(); cout << "  [+] Sorted by Name.\n"; }
        } else if (choice == 7) {
            int pid = readInt("Patient ID : ");
            string doc = readLine("Doctor Name: ");
            int d = readInt("Day (1-31) : ");
            int m = readInt("Month(1-12): ");
            int y = readInt("Year       : ");
            int hr = readInt("Hour (0-23): ");
            int min = readInt("Min (0-59) : ");
            cout << (scheduler.bookAppointment(pid, doc, d, m, y, hr, min, patients)
                        ? "  [+] Appointment booked.\n"
                        : "  [!] Booking failed (check patient ID, date/time, or doctor clash).\n");
        } else if (choice == 8) {
            scheduler.viewPending();
        } else if (choice == 9) {
            cout << (scheduler.completeNext() ? "  [+] Next appointment marked Completed.\n" : "  [!] Queue is empty.\n");
        } else if (choice == 10) {
            int aid = readInt("Appointment ID to cancel: ");
            cout << (scheduler.cancelAppointment(aid) ? "  [+] Appointment Cancelled.\n" : "  [!] Appointment ID not found.\n");
        } else if (choice == 11) {
            scheduler.viewHistory();
        } else {
            cout << "  [!] Invalid option.\n";
        }
    }
    return 0;
}
