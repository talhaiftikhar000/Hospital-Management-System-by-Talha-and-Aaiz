FILE HANDLING INTEGRATION WITH YOUR EXACT CODE

Your uploaded code defines:
- Patient: id, name, age, gender, contact, disease
- PatientList: linked list with public head
- AppointmentScheduler: private pending queue and appointment history
- Appointment: appointmentID, patientID, patientName, doctorName, date, time, status

Files:
1. HospitalData.h contains the classes from the pasted code. Only the
   addition of `friend class FileHandling;` to AppointmentScheduler makes its
   private queue and history accessible to the persistence helper.
2. filehandelling.h declares the four save/load functions.
3. filehandelling.cpp implements the functions.
4. test_filehandling.cpp runs a demonstration (NOT part of your final main app).

Compile/test (Mac terminal, from this folder):
    g++ -std=c++11 test_filehandling.cpp filehandelling.cpp -o test_files
    ./test_files

Integration into your own project:
- Do not compile duplicate definitions of Patient/PatientList/AppointmentScheduler.
  Move the definitions into your shared HospitalData.h, or adapt the include
  to your existing header if your group already uses one.
- Include #include "HospitalData.h" and #include "filehandelling.h" in main.cpp.
- At startup (after creating PatientList and AppointmentScheduler):
    FileHandling::loadPatients(patients);
    FileHandling::loadAppointments(appointments);
- After a successful patient register/update/delete:
    FileHandling::savePatients(patients);
- After a successful appointment booking/complete/cancel:
    FileHandling::saveAppointments(appointments);
- Save both again on exit.
- Text files will be created in the current working directory.
- Saved appointment IDs are used to set the next ID correctly after restart.
- Keep all six patient fields and all seven appointment fields in sync.
- Do not include test_filehandling.cpp in the final application build because
  your app must have exactly one main() function.
- Do not use the | separator or newline inside saved text fields.
