//Motor Pin functions
inline void setALow(){
    if (LOW_ACTIVE_LOW){
        LOW_A_PORT &= ~(1 << LOW_A); 
    }
    else {
        LOW_A_PORT |= (1 << LOW_A);     
    }
}

inline void clrALow(){
    if (LOW_ACTIVE_LOW){
        LOW_A_PORT |= (1 << LOW_A); 
    }
    else {
        LOW_A_PORT &= ~(1 << LOW_A);
    }
}

inline void setAHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_A_PORT &= ~(1 << HIGH_A);
    }
    else {
        HIGH_A_PORT |= (1 << HIGH_A);
    }
}

inline void clrAHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_A_PORT |= (1 << HIGH_A);
    }
    else {
        HIGH_A_PORT &= ~(1 << HIGH_A);
    }
}

inline void setBLow(){
    if (LOW_ACTIVE_LOW){
        LOW_B_PORT &= ~(1 << LOW_B); 
    }
    else {
        LOW_B_PORT |= (1 << LOW_B);     
    }
}

inline void clrBLow(){
    if (LOW_ACTIVE_LOW){
        LOW_B_PORT |= (1 << LOW_B); 
    }
    else {
        LOW_B_PORT &= ~(1 << LOW_B);
    }
}

inline void setBHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_B_PORT &= ~(1 << HIGH_B);
    }
    else {
        HIGH_B_PORT |= (1 << HIGH_B);
    }
}

inline void clrBHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_B_PORT |= (1 << HIGH_B);
    }
    else {
        HIGH_B_PORT &= ~(1 << HIGH_B);
    }
}

inline void setCLow(){
    if (LOW_ACTIVE_LOW){
        LOW_C_PORT &= ~(1 << LOW_C); 
    }
    else {
        LOW_C_PORT |= (1 << LOW_C);     
    }
}

inline void clrCLow(){
    if (LOW_ACTIVE_LOW){
        LOW_C_PORT |= (1 << LOW_C); 
    }
    else {
        LOW_C_PORT &= ~(1 << LOW_C);
    }
}

inline void setCHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_C_PORT &= ~(1 << HIGH_C);
    }
    else {
        HIGH_C_PORT |= (1 << HIGH_C);
    }
}

inline void clrCHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_C_PORT |= (1 << HIGH_C);
    }
    else {
        HIGH_C_PORT &= ~(1 << HIGH_C);
    }
}