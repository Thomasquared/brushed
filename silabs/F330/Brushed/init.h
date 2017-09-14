
void SYSCLK_Init(void) {
    OSCICN = 0x83; // Set internal oscillator to run
    // at its maximum frequency
    CLKSEL = 0x00; // Select the internal osc. as
    // the SYSCLK source
	PCA0MD = 0x00; //Disable watchdog
}

void Timer0_Init(void) {
    TH0 = 0x00; // Reinit Timer0 High register
    TL0 = 0x00; // Reinit Timer0 Low register
    TMOD = 0x09; // Timer0 in 16-bit mode with INT0 enable

    CKCON = 0x01; // Timer0 uses a 1:4 prescaler

    //ET0 = 1;       // Timer0 interrupt enabled

    TCON |= 0x11; // Timer0 ON with INT0 edge detection
}

void PORT_Init(void) {
    //So according to datasheet.
    //1 Set Analogue inputs
    P0MDIN = 0xFF; //Set pins 4,5 as 
    P1MDIN = 0xFF; //Set no analogue inputs

    P0 = 0x00; //Turn off all outputs
    P1 = 0x00; //Turn off all outputs

    P0MDOUT |=  PORT0_DIR; //Set digital outputs
    P1MDOUT |=  PORT1_DIR; //Set digital outputs
	
	P1	|= 0x00;
    P0 = INT0_SKIP; 
    P0SKIP = INT0_SKIP;
    IT01CF = INT0_PIN; //Set interrupt active high & on pin 0.4
    EX0 = 1;

    XBR0 = 0x00; // no digital peripherals selected
    XBR1 = 0x40; // Enable crossbar and no pullupts
}

void PCA_Init(void) {
    //Setup Comparator.
    PCA0MD &= ~0x40; // disable watchdog timer

    // enable)
	PCA0CN = 0x00;        
	PCA0MD = 0x00;
    PCA0MD |= 0x01; //Enable  overflow interrupt
    PCA0MD |= 0x08; //Use System Clock

    PCA0CPM0 = 0x49;

    PCA0L = 0x00; // Reset PCA Counter Value to 0x0000
    PCA0H = 0x00;

    PCA0CPL0 = PCA_TIMEOUT & 0x00FF; // Set up first match
    PCA0CPH0 = (PCA_TIMEOUT & 0xFF00) >> 8;

    EIE1 |= 0x10; // Enable PCA interrupts

    CR = 1; // Start PCA

    PCA0CPL2 = 0xFF;
    PCA0CPH2 = 0x00;

    //PCA0MD |= 0x40; // enable watchdog timer
}

void Timer1_Init(void) {
    TH1 = 0x00; // Reinit Timer0 High register
    TL1 = 0x00; // Reinit Timer0 Low register
    CKCON = 0x01; // Timer0 uses a 1:4 prescaler
    ET1 = 1; // Timer0 interrupt enabled
    TCON |= 0x40; // Timer0 ON with INT0 edge detection
    EA = 1;
}