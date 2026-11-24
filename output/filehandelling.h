#pragma once
#include <string>
class PatientList;
class AppointmentScheduler;

class FileHandling {
public:
    static bool savePatients(const PatientList& patients,
                             const std::string& filename = "patients.txt");
    static bool loadPatients(PatientList& patients,
                             const std::string& filename = "patients.txt");
    static bool saveAppointments(const AppointmentScheduler& appointments,
                                 const std::string& filename = "appointments.txt");
    static bool loadAppointments(AppointmentScheduler& appointments,
                                 const std::string& filename = "appointments.txt");
};
