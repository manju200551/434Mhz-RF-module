/*---PURE C CODE WITHOUT LIBRARY---*/ 
 
/*---EEPROM 4 ZONE LOGIC WITH LED FEEDBACK---*/ 
 
 
 
 
#define F_CPU 16000000UL 
 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <stdint.h> 
 
/* ---------------- Pin definitions ---------------- */ 
#define LED_DDR     DDRB 
#define LED_PORT    PORTB 
#define LED_BIT     PB5 
 
#define BTN_DDR     DDRD 
#define BTN_PORT    PORTD 
#define BTN_PINREG  PIND 
 
#define RF_DDR      DDRD 
#define RF_PORT     PORTD 
#define RF_BIT      PD2 
 
#define ZONE1_BTN   PD3 
#define ZONE2_BTN   PD4 
#define ZONE3_BTN   PD5 
#define ZONE4_BTN   PD6 
#define SAVE_BTN    PD7 
 
/* ---------------- RF capture config ---------------- */ 
#define MAX_TIMINGS         100 
#define FRAME_GAP_US        5000UL 
#define NOISE_MIN_US        120UL 
#define REQUIRED_MATCHES    5 
#define REPEAT_LOCK_MS      800UL 
#define BTN_LOCK_MS         250UL 
#define DECODE_TOL_PERCENT  40U 
 
/* ---------------- EEPROM ---------------- */ 
uint32_t EEMEM ee_zone1; 
uint32_t EEMEM ee_zone2; 
uint32_t EEMEM ee_zone3; 
uint32_t EEMEM ee_zone4; 
 
/* ---------------- Globals ---------------- */ 
volatile uint16_t timings[MAX_TIMINGS]; 
volatile uint8_t timingCount = 0; 
volatile uint8_t frameReady = 0; 
volatile uint32_t timer0MicrosBase = 0; 
 
uint32_t learnedCode[4] = {0, 0, 0, 0}; 
 
uint8_t learningMode = 0; 
uint8_t selectedZone = 0; 
uint32_t tempCode = 0; 
uint8_t matchCount = 0; 
uint8_t pairReady = 0; 
uint8_t ledState = 0; 
 
uint32_t lastHandledCode = 0; 
uint32_t lastHandledTime = 0; 
uint32_t lastButtonTime = 0; 
 
/* ---------------- Timer0 overflow for micros ---------------- */ 
ISR(TIMER0_OVF_vect) 
{ 
    timer0MicrosBase += 1024UL; 
} 
 
/* ---------------- Time helpers ---------------- */ 
static uint32_t micros_now(void) 
{ 
    uint32_t base; 
    uint8_t t; 
    uint8_t s = SREG; 
 
    cli(); 
    base = timer0MicrosBase; 
    t = TCNT0; 
    if ((TIFR0 & (1 << TOV0)) && t < 255) { 
        base += 1024UL; 
    } 
    SREG = s; 
 
    return base + ((uint32_t)t * 4UL); 
} 
 
static uint32_t millis_now(void) 
{ 
    return micros_now() / 1000UL; 
} 
 
/* ---------------- INT0 capture ---------------- */ 
ISR(INT0_vect) 
{ 
    static uint32_t lastEdge = 0; 
    uint32_t now = micros_now(); 
    uint32_t width = now - lastEdge; 
    lastEdge = now; 
 
    if (width < NOISE_MIN_US) return; 
 
    if (width > FRAME_GAP_US) { 
        if (timingCount >= 20) frameReady = 1; 
        return; 
    } 
 
    if (timingCount < MAX_TIMINGS) { 
        timings[timingCount++] = (uint16_t)width; 
    } 
} 
 
/* ---------------- UART ---------------- */ 
static void uart_init(void) 
{ 
    UBRR0H = 0; 
    UBRR0L = 103; 
    UCSR0B = (1 << TXEN0); 
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
} 
 
static void uart_tx(char c) 
{ 
    while (!(UCSR0A & (1 << UDRE0))); 
    UDR0 = c; 
} 
 
static void uart_str(const char *s) 
{ 
    while (*s) uart_tx(*s++); 
} 
 
static void uart_num(uint32_t n) 
{ 
    char buf[11]; 
    uint8_t i = 0; 
 
    if (n == 0) { 
        uart_tx('0'); 
        return; 
    } 
 
    while (n > 0) { 
        buf[i++] = '0' + (n % 10); 
        n /= 10; 
    } 
    while (i > 0) uart_tx(buf[--i]); 
} 
 
static void uart_nl(void) 
{ 
    uart_str("\r\n"); 
} 
 
/* ---------------- Helpers ---------------- */ 
static uint8_t btn_pressed(uint8_t bit) 
{ 
    return ((BTN_PINREG & (1 << bit)) == 0); 
} 
 
static void led_write(uint8_t on) 
{ 
    if (on) LED_PORT |= (1 << LED_BIT); 
    else    LED_PORT &= ~(1 << LED_BIT); 
} 
 
static void toggle_led(void) 
{ 
    ledState ^= 1; 
    led_write(ledState); 
} 
 
static uint8_t approx(uint16_t value, uint16_t target, uint8_t tolPercent) 
{ 
    uint32_t low = target - ((uint32_t)target * tolPercent / 100UL); 
    uint32_t high = target + ((uint32_t)target * tolPercent / 100UL); 
    return ((uint32_t)value >= low && (uint32_t)value <= high); 
} 
 
/* ---------------- EEPROM ---------------- */ 
static void load_codes(void) 
{ 
    learnedCode[0] = eeprom_read_dword(&ee_zone1); 
    learnedCode[1] = eeprom_read_dword(&ee_zone2); 
    learnedCode[2] = eeprom_read_dword(&ee_zone3); 
    learnedCode[3] = eeprom_read_dword(&ee_zone4); 
} 
 
static void save_zone(uint8_t zone) 
{ 
    switch (zone) { 
        case 1: eeprom_update_dword(&ee_zone1, learnedCode[0]); break; 
        case 2: eeprom_update_dword(&ee_zone2, learnedCode[1]); break; 
        case 3: eeprom_update_dword(&ee_zone3, learnedCode[2]); break; 
        case 4: eeprom_update_dword(&ee_zone4, learnedCode[3]); break; 
        default: break; 
    } 
} 
 
/* ---------------- RF decode ---------------- */ 
static uint8_t decode_protocol1(uint16_t *buf, uint8_t count, uint32_t 
*outCode) 
{ 
    uint8_t i; 
    uint8_t bits = 0; 
    uint16_t baseT = 0xFFFF; 
 
    if (count < 20) return 0; 
 
    for (i = 0; i < count; i++) { 
        if (buf[i] >= 100 && buf[i] <= 800 && buf[i] < baseT) { 
            baseT = buf[i]; 
        } 
    } 
 
    if (baseT == 0xFFFF) return 0; 
 
    *outCode = 0; 
 
    for (i = 0; (i + 1) < count && bits < 24; i += 2) { 
        uint16_t t1 = buf[i]; 
        uint16_t t2 = buf[i + 1]; 
 
        if (approx(t1, baseT, DECODE_TOL_PERCENT) && 
            approx(t2, baseT * 3, DECODE_TOL_PERCENT)) { 
            *outCode <<= 1; 
            bits++; 
        } 
        else if (approx(t1, baseT * 3, DECODE_TOL_PERCENT) && 
                 approx(t2, baseT, DECODE_TOL_PERCENT)) { 
            *outCode <<= 1; 
            *outCode |= 1UL; 
            bits++; 
        } 
    } 
 
    return (bits == 24); 
} 
 
/* ---------------- Application logic ---------------- */ 
static void start_learning(uint8_t zone) 
{ 
    selectedZone = zone; 
    learningMode = 1; 
    tempCode = 0; 
    matchCount = 0; 
    pairReady = 0; 
 
    led_write(1); 
    for (uint8_t i = 0; i < 150; i++) { 
        for (volatile uint16_t j = 0; j < 4000; j++) { 
            __asm__ __volatile__("nop"); 
        } 
    } 
    led_write(0); 
 
    uart_str("ZONE "); 
    uart_num(zone); 
    uart_str(" SELECTED"); 
    uart_nl(); 
    uart_str("PRESS REMOTE BUTTON 5 TIMES"); 
    uart_nl(); 
} 
 
static void save_current_zone(void) 
{ 
    if (selectedZone >= 1 && selectedZone <= 4) { 
        save_zone(selectedZone); 
 
        uart_str("ZONE "); 
        uart_num(selectedZone); 
        uart_str(" SAVE SUCCESS"); 
        uart_nl(); 
 
        for (uint8_t i = 0; i < 3; i++) { 
            led_write(1); 
            for (volatile uint16_t j = 0; j < 3000; j++) __asm__ 
__volatile__("nop"); 
            led_write(0); 
            for (volatile uint16_t j = 0; j < 3000; j++) __asm__ 
__volatile__("nop"); 
        } 
    } 
 
    learningMode = 0; 
    selectedZone = 0; 
    tempCode = 0; 
    matchCount = 0; 
    pairReady = 0; 
} 
 
static void process_learning(uint32_t code) 
{ 
    uart_str("RAW CODE = "); 
    uart_num(code); 
    uart_nl(); 
 
    if (tempCode == 0) { 
        tempCode = code; 
        matchCount = 1; 
    } 
    else if (tempCode == code) { 
        if (matchCount < 255) matchCount++; 
 
        uart_str("MATCH COUNT = "); 
        uart_num(matchCount); 
        uart_nl(); 
 
        if (matchCount >= REQUIRED_MATCHES) { 
            learnedCode[selectedZone - 1] = code; 
            pairReady = 1; 
            uart_str("PAIR READY FOR ZONE "); 
            uart_num(selectedZone); 
            uart_nl(); 
            uart_str("PRESS SAVE BUTTON"); 
            uart_nl(); 
            led_write(1); 
        } 
    } 
    else { 
        tempCode = code; 
        matchCount = 1; 
    } 
} 
 
static void process_normal(uint32_t code) 
{ 
    uint32_t now = millis_now(); 
 
    if (code == lastHandledCode && (now - lastHandledTime) < REPEAT_LOCK_MS) { 
        return; 
    } 
 
    lastHandledCode = code; 
    lastHandledTime = now; 
 
    for (uint8_t i = 0; i < 4; i++) { 
        if (learnedCode[i] != 0 && code == learnedCode[i]) { 
            uart_str("ZONE "); 
            uart_num(i + 1); 
            uart_str(" TOGGLED"); 
            uart_nl(); 
            toggle_led(); 
            break; 
        } 
    } 
} 
 
static void handle_rf(void) 
{ 
    if (!frameReady) return; 
 
    uint16_t localBuf[MAX_TIMINGS]; 
    uint8_t localCount; 
    uint32_t code = 0; 
 
    cli(); 
    localCount = timingCount; 
    if (localCount > MAX_TIMINGS) localCount = MAX_TIMINGS; 
    for (uint8_t i = 0; i < localCount; i++) { 
        localBuf[i] = timings[i]; 
    } 
    frameReady = 0; 
    timingCount = 0; 
    sei(); 
 
    if (decode_protocol1(localBuf, localCount, &code)) { 
        if (learningMode) process_learning(code); 
        else process_normal(code); 
    } 
} 
 
static void check_buttons(void) 
{ 
    uint32_t now = millis_now(); 
    if ((now - lastButtonTime) < BTN_LOCK_MS) return; 
 
    if (btn_pressed(ZONE1_BTN)) { 
        start_learning(1); 
        lastButtonTime = now; 
    } 
    else if (btn_pressed(ZONE2_BTN)) { 
        start_learning(2); 
        lastButtonTime = now; 
    } 
    else if (btn_pressed(ZONE3_BTN)) { 
        start_learning(3); 
        lastButtonTime = now; 
    } 
    else if (btn_pressed(ZONE4_BTN)) { 
        start_learning(4); 
        lastButtonTime = now; 
    } 
    else if (learningMode && pairReady && btn_pressed(SAVE_BTN)) { 
        save_current_zone(); 
        lastButtonTime = now; 
    } 
} 
 
/* ---------------- Init ---------------- */ 
static void io_init(void) 
{ 
    LED_DDR |= (1 << LED_BIT); 
    led_write(0); 
 
    BTN_DDR &= ~((1 << ZONE1_BTN) | (1 << ZONE2_BTN) | (1 << ZONE3_BTN) | (1 
<< ZONE4_BTN) | (1 << SAVE_BTN)); 
    BTN_PORT |= ((1 << ZONE1_BTN) | (1 << ZONE2_BTN) | (1 << ZONE3_BTN) | (1 
<< ZONE4_BTN) | (1 << SAVE_BTN)); 
 
    RF_DDR &= ~(1 << RF_BIT); 
    RF_PORT &= ~(1 << RF_BIT); 
} 
 
static void timer0_init(void) 
{ 
    TCCR0A = 0x00; 
    TCCR0B = (1 << CS01) | (1 << CS00); 
    TIMSK0 = (1 << TOIE0); 
} 
 
static void int0_init(void) 
{ 
    EICRA = (1 << ISC00); 
    EIMSK = (1 << INT0); 
} 
 
int main(void) 
{ 
io_init(); 
uart_init(); 
timer0_init(); 
int0_init(); 
load_codes(); 
sei(); 
uart_str("SYSTEM READY"); 
uart_nl(); 
while (1) { 
check_buttons(); 
handle_rf(); 
} 
} 
