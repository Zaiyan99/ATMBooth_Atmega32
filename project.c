#include <mega32.h>
#include <alcd.h>
#include <delay.h>
#include <stdlib.h>

// Global variables
int id_pin = 1234;
int id_pin1 = 2024;
int id_bal1 = 4300;
int id_bal = 10000;
int user = 0;
int count = 0;
int PIN = 0;
int amount = 0;
char id[16];
int count_timer = 0;
int page = 0;
int wrong_pin = 0;

// Function prototypes
void atm_home();
void account_home();
void balance(int bal);
void last_page();
int withdrawal(int balance);
void motor1();
void motor2();
void led();

// Timer1 ISR - session timeout
interrupt [TIM1_COMPA] void timer1_compa_isr(void) {
    count_timer++; // Count increments every second
    TCNT1H = 0;
    TCNT1L = 0;

    if (count_timer == 30) {
        // If no response for more than 30s
        count_timer = 0;
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("Your session");
        lcd_gotoxy(0, 1);
        lcd_putsf("timed out.");
        page = 10;
        PIN = 0;
        count = 0;
        delay_ms(1300);
        lcd_clear();
    }
}

// Home page
void atm_home() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putsf("Welcome");
    lcd_gotoxy(0, 1);
    lcd_putsf("Enter Card");
}

// Account menu
void account_home() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putsf("1:BALANCE");
    lcd_gotoxy(0, 1);
    lcd_putsf("2:WITHDRWL 3:EXT");
    delay_ms(1200);
}

// Balance display
void balance(int bal) {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putsf("Account Balance");
    itoa(bal, id);      // Convert balance to string
    lcd_gotoxy(0, 1);
    lcd_puts(id);       // Display balance
}

// Motor for cash dispense
void motor1() {
    PORTB.2 = 1;
    PORTB.0 = 1;
    PORTB.1 = 0;
    delay_ms(1500);
    PORTB.2 = 0;
}

// Motor for ATM lock
void motor2() {
    PORTB.3 = 1;
    PORTB.4 = 1;
    PORTB.5 = 0;
    delay_ms(2000);
    PORTB.3 = 0;
}

// LED alert
void led() {
    PORTD = 0xF0;
    delay_ms(5000);
    PORTD = 0x00;
    PORTB.4 = 0;
}

// Withdrawal logic
int withdrawal(int balance) {
    int new_balance = 0;
    amount = PIN;
    PIN = 0;

    if (amount > balance) {
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("INSUFFICIENT");
        lcd_gotoxy(0, 1);
        lcd_putsf("BALANCE!");
        delay_ms(2000);
        page = 10;
        count = 0;
        PIN = 0;
    }
    else if (amount % 500 != 0) { // Must be multiple of 500
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("Invalid Amount");
        lcd_gotoxy(0, 1);
        lcd_putsf("Enter N x 500");
        delay_ms(2000);
        page = 10;
        count = 0;
        PIN = 0;
    }
    else if (amount > 0) {
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("New Balance:");
        new_balance = balance - amount;
        itoa(new_balance, id);
        lcd_gotoxy(0, 1);
        lcd_puts(id);
        page = 10;
        motor1();
        delay_ms(1200);
        return new_balance;
    }

    return balance;
}

// Exit page
void last_page() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putsf("THANK YOU!");
}

// Main function
void main(void) {
    int i;

    // Port configuration
    DDRD = 0xFF;
    DDRC = 0xF0;
    DDRB = 0xFF;
    PORTC = 0xFF;  // Activate pull-up resistors
    DDRA = 0xF0;

    // LCD init
    lcd_init(16);

    // Timer init
    TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10) | (1 << WGM12); // Prescaler 1024
    OCR1AH = 0x3D; // Compare value for 1s at 8 MHz
    OCR1AL = 0x08;
    #asm("sei");   // Enable global interrupts

    while (1) {
        // === PAGE 0: Home ===
        if (page == 0) {
            atm_home();
            PORTC = 0b10111111;
            if (PINC.3 == 0) {
                page++;
                PIN = 0;
                count = 0;
            }
            delay_ms(300);
        }

        // === PAGE 1: Enter PIN ===
        if (page == 1) {
            TIMSK |= (1 << OCIE1A);
            count_timer = 0;

            while (page == 1) {
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts("Enter PIN:");

                for (i = 0; i < count; i++)
                    lcd_putchar('*'); // Show asterisks

                if (count > 4) { // Limit 4 digits
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_puts("Limit Exceeded");
                    delay_ms(1000);
                    page = 10;
                }

                // Keypad scanning (3 blocks)
                // ... (your keypad logic here, already in original code)

                // Enter pressed → move to next page
                // ...
            }
        }

        // === PIN Validation ===
        if (PIN == id_pin) user = 1;
        if (PIN == id_pin1) user = 2;

        if ((PIN == id_pin && page == 2) || (PIN == id_pin1 && page == 2)) {
            // Correct PIN → continue
        }
        else if ((PIN != id_pin && page == 2) || (PIN != id_pin1 && page == 2)) {
            wrong_pin++;
            if (wrong_pin == 3) {
                count_timer = 0;
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_putsf("TOO MANY FAILS");
                lcd_gotoxy(0, 1);
                lcd_putsf("ATM LOCKED!");
                motor2();
                led();
                wrong_pin = 0;
                TIMSK |= (0 << OCIE1A);
                page = 10;
            }
            else {
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_putsf("WRONG PIN!");
                lcd_gotoxy(0, 1);
                lcd_putsf("TRY AGAIN");
                delay_ms(1200);
                page = 1;
                PIN = 0;
                count = 0;
            }
        }

        // === PAGE 3: Account Menu ===
        if (page == 3) {
            account_home();
            // ... (menu handling like balance, withdrawal, exit)
        }

        // === PAGE 4: Balance or Withdrawal ===
        // ... (your balance + withdrawal logic here, from original)

        // === PAGE 10: Exit ===
        if (page == 10) {
            last_page();
            delay_ms(3000);
            page = 0;
            count = 0;
            PIN = 0;
            TIMSK |= (0 << OCIE1A);
            count_timer = 0;
        }
    }
}
