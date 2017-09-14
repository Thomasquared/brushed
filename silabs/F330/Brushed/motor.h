//Setup 2 ports which we are using for H bridge. 
#define FORWARD_LOW  LOW_C
#define FORWARD_HIGH HIGH_A
#define BACKWARD_LOW  LOW_A
#define BACKWARD_HIGH HIGH_C
#define SPARE_HIGH HIGH_B
#define SPARE_LOW LOW_B

//Motor Functions
void setForwardLow() {
    if (LOW_ACTIVE_LOW) {
        FORWARD_LOW = 0;
    } else {
        FORWARD_LOW = 1;
    }
}

void clrForwardLow() {
    if (LOW_ACTIVE_LOW) {
        FORWARD_LOW = 1;
    } else {
        FORWARD_LOW = 0;
    }
}

void setForwardHigh() {
    if (HIGH_ACTIVE_LOW) {
        FORWARD_HIGH = 0;
    } else {
        FORWARD_HIGH = 1;
    }
}

void clrForwardHigh() {
    if (HIGH_ACTIVE_LOW) {
        FORWARD_HIGH = 1;
    } else {
        FORWARD_HIGH = 0;
    }
}

void setBackwardLow() {
    if (LOW_ACTIVE_LOW) {
        BACKWARD_LOW = 0;
    } else {
        BACKWARD_LOW = 1;
    }
}

void clrBackwardLow() {
    if (LOW_ACTIVE_LOW) {
        BACKWARD_LOW = 1;
    } else {
        BACKWARD_LOW = 0;
    }
}

void setBackwardHigh() {
    if (HIGH_ACTIVE_LOW) {
        BACKWARD_HIGH = 0;
    } else {
        BACKWARD_HIGH = 1;
    }
}

void clrBackwardHigh() {
    if (HIGH_ACTIVE_LOW) {
        BACKWARD_HIGH = 1;
    } else {
        BACKWARD_HIGH = 0;
    }
}


void setSpareLow() {
    if (LOW_ACTIVE_LOW) {
        SPARE_LOW = 0;
    } else {
        SPARE_LOW = 1;
    }
}

void clrSpareLow() {
    if (LOW_ACTIVE_LOW) {
        SPARE_LOW = 1;
    } else {
        SPARE_LOW = 0;
    }
}

void setSpareHigh() {
    if (HIGH_ACTIVE_LOW) {
        SPARE_HIGH = 0;
    } else {
        SPARE_HIGH = 1;
    }
}

void clrSpareHigh() {
    if (HIGH_ACTIVE_LOW) {
        SPARE_HIGH = 1;
    } else {
        SPARE_HIGH = 0;
    }
}
