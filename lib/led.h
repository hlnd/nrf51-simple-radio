#if defined(BOARD_PCA10001)
#define LED_START 18
#define LED0 18
#define LED1 19
#define LED_STOP 20

#elif defined(BOARD_PCA10000)
#define LED_START 21
#define LED0 21
#define LED1 22
#define LED2 23
#define LED_STOP 24
#else 
#error "Board not defined."

#endif

