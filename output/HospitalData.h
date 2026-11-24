 #ifndef HOSPITAL_DATA_H
#define HOSPITAL_DATA_H
// Patient and Appointment classes from your shared code.
// The only functional integration change is the friend declaration.
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>       // time, localtime, strftime
#include <limits>      // numeric_limits
#include <cctype>      // tolower

using namespace std;

const string STATUS_SCHEDULED = "Scheduled";
const string STATUS_COMPLETED = "Completed";
const string STATUS_CANCELLED = "Cancelled";
class Patient {
public:
    int id;
    string name;
    int age;
    string gender;
    string contact;
    string disease;
    Patient()
    {
        id = 0;
        age = 0;
    }
    Patient(int id, string name, int age, string gender, string contact, string disease)
    {
        this->id = id;
        this->name = name;
        this->age = age;
        this->gender = gender;
        this->contact = contact;
        this->disease = disease;
    }
    void display()
    {
        cout << "ID=" << id << "|Name=" << name << "|Age=" << age << "|Gender="
            << gender << "|Contact=" << contact << "|Disease=" << disease << endl;
    }
};
class PatientNode
{
public:
    Patient data;
    PatientNode* next;
    PatientNode(Patient p)
    {
        data = p;
        next = nullptr;
    }
    string tolowercase(string s)
    {
        for (string::size_type i = 0; i < s.length(); i++)
        {
            s[i] = static_cast<char>(tolower(static_cast<unsigned char>(s[i])));
        }
        return s;
    }
};
class PatientList
{
public:
    PatientNode* head;
    PatientList()
    {
        head = nullptr;
    }
    string toLower(string s) {
        for (string::size_type i = 0; i < s.length(); i++) {
            s[i] = static_cast<char>(tolower(static_cast<unsigned char>(s[i])));
        }
        return s;
    }
    PatientNode* searchbyid(int id)
    {
        if (head == nullptr)
        {
            return nullptr;
        }
        PatientNode* temp = head;
        while (temp != nullptr)
        {
            if (temp->data.id == id)
            {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }
    bool registerPatient(Patient p)
    {
        if (searchbyid(p.id) != nullptr)
        {
            cout << "Duplicate id";
            return false;
        }
        PatientNode* newNode = new PatientNode(p);
        if (head == nullptr)
        {
            head = newNode;
            return true;
        }
        else
        {
            PatientNode* temp = head;
            while (temp->next != nullptr)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        sortbyId();
        return true;
    }
    PatientList searchByName(string name) {
        PatientList result;
        PatientNode* temp = head;
        while (temp != nullptr) {
            if (toLower(temp->data.name) == toLower(name)) {
                result.registerPatient(temp->data);
            }
            temp = temp->next;
        }
        return result;
    }
    void sortbyId()
    {
        PatientNode* sorted = nullptr;
        PatientNode* temp = head;
        while (temp != nullptr)
        {
            PatientNode* nextnode = temp->next;
            if (sorted == nullptr || temp->data.id < sorted->data.id)
            {
                temp->next = sorted;
                sorted = temp;
            }
            else
            {
                PatientNode* temp2 = sorted;
                while (temp2->next != nullptr && temp2->next->data.id < temp->data.id)
                {
                    temp2 = temp2->next;
                }
                temp->next = temp2->next;
                temp2->next = temp;
            }
            temp = nextnode;

        }
        head = sorted;
    }
    void sortbyName()
    {
        PatientNode* sorted = nullptr;
        PatientNode* temp = head;
        while (temp != nullptr)
        {
            PatientNode* nextnode = temp->next;
            if (sorted == nullptr || toLower(temp->data.name) < toLower(sorted->data.name))
            {
                temp->next = sorted;
                sorted = temp;
            }
            else
            {
                PatientNode* temp2 = sorted;
                while (temp2->next != nullptr && toLower(temp2->next->data.name) < toLower(temp->data.name))
                {
                    temp2 = temp2->next;
                }
                temp->next = temp2->next;
                temp2->next = temp;
            }
            temp = nextnode;

        }
        head = sorted;
    }
    bool deletePatient(int id)
    {
        if (head == nullptr)
        {
            cout << "Empty list";
            return false;
        }
        if (head->data.id == id)
        {
            PatientNode* temp = head;
            head = temp->next;
            delete temp;
            return true;
        }
        PatientNode* temp = head;
        while (temp->next != nullptr && temp->next->data.id != id)
        {
            temp = temp->next;
        }
        if (temp->next == nullptr)
        {
            cout << "Id does not exist";
            return false;
        }
        PatientNode* temp2 = temp->next;
        temp->next = temp2->next;
        delete temp2;
        return true;
    }
    bool updatePatient(int id, string name, int age, string gender, string contact, string disease) {
        PatientNode* node = searchbyid(id);
        if (node == NULL) {
            return false;
        }
        node->data.name = name;
        node->data.age = age;
        node->data.gender = gender;
        node->data.contact = contact;
        node->data.disease = disease;
        return true;
    }
    void display()
    {
        if (head == nullptr)
        {
            cout << "No records found" << endl;
            return;
        }
        PatientNode* temp = head;
        while (temp != nullptr) {
            temp->data.display();
            temp = temp->next;
        }
    }
};


struct Appointment {
    int appointmentID, patientID;
    string patientName, doctorName, date, time, status;
    Appointment* next;

    Appointment(int appID = 0, int patID = 0, string patName = "",string docName = "", string dt = "", string tm = "",
                string st = STATUS_SCHEDULED) 
{
    appointmentID = appID;
    patientID = patID;
    patientName = patName;
    doctorName = docName;
    date = dt;
    time = tm;
    status = st;
    next = nullptr;
}
};


class AppointmentScheduler {
    friend class FileHandling;  // Permit saving/loading queue and history
private:
    Appointment *front, *rear;                 // pending queue
    Appointment *historyHead, *historyTail;    // completed / cancelled
    int pendingCount, historyCount, nextAppointmentID;

    bool allDigits(const string& s) const 
    {
    for (char c : s)
        {if (c < '0' || c > '9') return false;}
    return true;
    }

    // turns 5 into "05" so the stored strings stay in YYYY-MM-DD and HH:MM format
    string twoDigits(int n) const 
    {
        return (n < 10 ? "0" : "") + to_string(n);
    }

    void freeList(Appointment* head) 
    {
        while (head != nullptr)
        {
            Appointment* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addToHistory(Appointment* node, const string& status) 
    {
        node->status = status;
        node->next = nullptr;
        if (historyHead == nullptr) 
        {
            historyHead = historyTail = node;
        }
        else 
        {
            historyTail->next = node;
            historyTail = node;
        }
        historyCount++;
    }

    void printTable(Appointment* head, const string& title,const string& totalLabel, int total) const 
        {
        cout << "\n===== " << title << " =====\n";
        cout << left << setw(10) << "App ID" << setw(12) << "Patient ID"
             << setw(18) << "Patient Name" << setw(16) << "Doctor"
             << setw(12) << "Date" << setw(8) << "Time"
             << setw(12) << "Status" << "\n";
        cout << string(88, '-') << "\n";
        for (Appointment* curr = head; curr != nullptr; curr = curr->next)
        {
            cout << left << setw(10) << curr->appointmentID
                 << setw(12) << curr->patientID
                 << setw(18) << curr->patientName
                 << setw(16) << curr->doctorName
                 << setw(12) << curr->date
                 << setw(8)  << curr->time
                 << setw(12) << curr->status << "\n";
        }
        cout << totalLabel << total << "\n";
    }

public:
  AppointmentScheduler() 
  {
    front = nullptr;
    rear = nullptr;
    pendingCount = 0;
    historyHead = nullptr;
    historyTail = nullptr;
    historyCount = 0;
    nextAppointmentID = 1001;
  }

    ~AppointmentScheduler() 
    {
        freeList(front);
        freeList(historyHead);
    }

    // 1. Verify registered patient (Linear Search)
    bool verifyRegisteredPatient(int patientID, PatientList& patients)
    {

        if (patients.searchbyid(patientID) != nullptr)
        {
            return true;
        }
        return false;

    }

  bool validateDate(int day, int month, int year) const
   {
        if (year < 2024 || year > 2100 || month < 1 || month > 12) return false;

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
            daysInMonth[1] = 29;                    // leap year

        return day >= 1 && day <= daysInMonth[month - 1];
    }

    bool validateTime(int hour, int minute) const 
    {
        return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
    }

    bool isInPast(int day, int month, int year, int hour, int minute) const
    {
        time_t now = time(nullptr);
        tm* t = localtime(&now);

        int curYear   = t->tm_year + 1900;   // tm_year counts from 1900
        int curMonth  = t->tm_mon + 1;       // tm_mon is 0-11
        int curDay    = t->tm_mday;
        int curHour   = t->tm_hour;
        int curMinute = t->tm_min;

        // compare from the biggest unit to the smallest
        if (year != curYear)     
        return year < curYear;
        if (month != curMonth)   
        return month < curMonth;
        if (day != curDay)      
        return day < curDay;
        if (hour != curHour)   
        return hour < curHour;
        return minute < curMinute;
    }

    // 3. Enqueue (FIFO)
    void enqueue(Appointment* newNode) {
        newNode->next = nullptr;
        if (rear == nullptr) front = rear = newNode;
        else { rear->next = newNode; rear = newNode; }
        pendingCount++;
    }

   // 4. Book appointment
   bool bookAppointment(int patientID, const string& doctorName,
                         int day, int month, int year, int hour, int minute,
                         PatientList& patients)
    {
        PatientNode* found = patients.searchbyid(patientID);
        if (!verifyRegisteredPatient(patientID, patients))
        {
            cout << "[Error] Patient ID " << patientID << " is not registered.\n";
            return false;
        }
        string patientName = found->data.name;

        if (!validateDate(day, month, year)) {
            cout << "[Error] Invalid date.\n";
            return false;
        }
        if (!validateDate(day, month, year)) {
            cout << "[Error] Invalid date.\n";
            return false;
        }
        if (!validateTime(hour, minute)) {
            cout << "[Error] Invalid time (hour 0-23, minute 0-59).\n";
            return false;
        }
        if (isInPast(day, month, year, hour, minute)) {
            cout << "[Error] Appointment date/time is in the past.\n";
            return false;
        }

        // build the strings used for storing and displaying
        string date = to_string(year) + "-" + twoDigits(month) + "-" + twoDigits(day);
        string apptTime = twoDigits(hour) + ":" + twoDigits(minute);

        // check if the doctor already has an appointment at this date and time
        for (Appointment* curr = front; curr != nullptr; curr = curr->next) {
            if (curr->doctorName == doctorName && curr->date == date && curr->time == apptTime) {
                cout << "[Error] " << doctorName << " already has an appointment on "
                     << date << " at " << apptTime << ".\n";
                return false;
            }
        }

        int appID = nextAppointmentID++;
        enqueue(new Appointment(appID, patientID, patientName, doctorName, date, apptTime));

        cout << "[Success] Appointment #" << appID << " booked for " << patientName
             << " with " << doctorName << " on " << date << " at " << apptTime
             << " [" << STATUS_SCHEDULED << "].\n";
        return true;
    }

    // 5. View pending queue
    void viewPendingQueue() const 
    {
        if (front == nullptr)
        {
             cout << "\nNo pending appointments in the queue.\n"; return; 
        }
        printTable(front, "PENDING APPOINTMENT QUEUE (FIFO)", "Total Pending: ", pendingCount);
    }

    // 6. Complete next (dequeue)
    bool completeNext() {
        if (front == nullptr) 
        {
            cout << "[Info] Queue is empty. No appointment to complete.\n";
            return false;
        }
        Appointment* completed = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
            pendingCount--;

        addToHistory(completed, STATUS_COMPLETED);
        cout << "[Success] Appointment #" << completed->appointmentID<< " marked as [" << STATUS_COMPLETED << "].\n";
        return true;
    }

    // 7. Cancel by appointment ID
    bool cancelAppointment(int appointmentID) 
    {
        Appointment *curr = front, *prev = nullptr;

        while (curr != nullptr && curr->appointmentID != appointmentID) 
        {
            prev = curr;
            curr = curr->next;
        }

        if (curr == nullptr)
        {
            cout << "[Error] Pending Appointment #" << appointmentID << " not found.\n";
            return false;
        }

        if (prev == nullptr) front = curr->next;
        else prev->next = curr->next;
        if (curr == rear) rear = prev;
        pendingCount--;

        addToHistory(curr, STATUS_CANCELLED);
        cout << "[Success] Appointment #" << appointmentID<< " marked as [" << STATUS_CANCELLED << "].\n";
        return true;
    }

    // 8. View history
    void viewAppointmentHistory() const 
    {
        if (historyHead == nullptr) 
        { 
            cout << "\nNo completed or cancelled appointments yet.\n"; return; 
        }
        printTable(historyHead, "APPOINTMENT HISTORY (COMPLETED / CANCELLED)", "Total History Records: ", historyCount);
    }
    // 9. to block patient deletion if appointment on that id exists
    bool pendingappointmentexists(int patientID) const
    {
        Appointment* temp = front;
        while (temp != nullptr)
        {
            
            if (temp->patientID == patientID)
            {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

};



#endif // HOSPITAL_DATA_H
