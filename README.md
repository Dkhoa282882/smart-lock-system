# Smart-access-control-system
This system uses ESP32 microcontroller . The system provides authentication methods such as entering password through a 4x4 keypad or fingerprint recognition through a fingerprint sensor. Additionally , The system is connected to a local network, allowing remote monitoring and management within the local network.
* **Multiple Authentication Methods:** Grant access via a 4-to-6 digit password or by scanning a registered fingerprint.
* **State Machine Architecture:** Employs a state machine to manage system modes , enabling non-blocking multitasking across keypad inputs, fingerprint scanning, and web requests.
* **Non - volatile Memory:** This system have a feature to change the password so the password will be saved into ESP32's EEPROM to prevent data loss during power loss.
