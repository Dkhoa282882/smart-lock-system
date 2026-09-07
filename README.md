# Access-control-system
This project uses an ESP32 microcontroller to implement a smart access-control system. The system provides multiple authentication and management functions, including password authentication through a 3x4 keypad and fingerprint recognition through a fingerprint sensor. 
* **Multiple Authentication Methods:** Grant access via a 4-to-6 digit password or by scanning a enrolled fingerprint.
* **FreeRTOS Architecture:** Uses FreeRTOS to divide the system into multiple independent tasks, including keypad input, system logic, fingerprint scanning, and door control. Tasks communicate and synchronize through mechanisms such as Queues, Task Notifications, and Mutexes, enabling responsive and non-blocking multitasking.
*  **User Guidance and Display:** Uses a 16x2 LCD to display system status, menus, and instructions, guiding users through authentication and administrative operations.
* **Non - volatile Storage:** Stores the system password in the ESP32's Flash memory using EEPROM emulation, allowing the password to be retained after system is powered off or restarted.
* **Administrative Configuration:** Allows the administrator to change the master password, enroll new fingerprints with automatically assigned IDs, and delete existing fingerprint templates on the device.
<p align="center">
  <img width="50%" alt="hethongvantay" src="https://github.com/user-attachments/assets/a685f27a-fdeb-41fb-9afb-9627b5b27902" />
  <img width="40%" alt="menu" src="https://github.com/user-attachments/assets/166e666b-7061-4423-963f-2d8e3f24d3c3" />
</p>
