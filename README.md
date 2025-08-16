# ATMBooth_Atmega32
A microcontroller-based ATM booth with secure PIN authentication, balance inquiry, cash withdrawal, and exit options. Features include shutdown after three failed attempts, robust error handling, and a user-friendly interface to ensure efficiency, reliability, and ease of use.

Tech Stack & Components

  - Microcontroller: Atmega32

  - Language: Embedded C

Peripherals:

  - LCD Display (16x2)

  - Keypad input

  - Motors (simulate cash dispense & ATM lock)

  - LEDs (status indication)

Tools:

  - AVR-GCC / CodeVisionAVR

  - Proteus (for simulation)

Flowchart

<img width="659" height="491" alt="image" src="https://github.com/user-attachments/assets/d946ec21-b33d-4cbc-999b-3751d25dbd91" />

Usage

  - Power up the ATM system.

  - Insert a “card” (simulated with keypad input).

  - Enter PIN:

  - 1234 → User 1 (balance: 10,000)

  - 2024 → User 2 (balance: 4,300)

  - Select operation:

  - 1 → Check balance

  - 2 → Withdraw cash

  - 3 → Exit

  - If more than 3 wrong PIN attempts → ATM locks.

  - Session automatically times out after 30s of inactivity.

Example Workflow

  - User enters PIN 1234

  - Chooses Balance → LCD shows 10000

  - Chooses Withdraw and enters 1500 → New balance 8500, motor simulates cash dispense

Security Features

  - 3 wrong PIN attempts → ATM locks (motor + LED alert)

  - Session timeout after 30 seconds

  - Withdrawal validation (only multiples of 500)
