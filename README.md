# ARM Cortex M4 handheld console implementation

This gaming console implementation uses an 8x8 LED matrix as a display and a rotary encoder as a controller. It features three classic games: snake, tetris and brickbreaker.

Special Kinetis k64f capabilities such as direct memory access (DMA) were exploited for seamlessly updating the display without putting strain on the processor.

The system features accelerometer support, and is UART and I2C ready: it can be connected to a wireless enabled board for wireless connectivity. 
