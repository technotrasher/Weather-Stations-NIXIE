///*
void burnIndicators() {
  for (uint8_t k = 0; k < BURN_LOOPS; k++) {
    for (uint8_t i = 0; i < 6; i++) {
      for (uint8_t digit = 0; digit < 10; digit++) {
        indiDigits[i] = digit;
        //if (burnTimer.isReady()) {
        indiDimm[i] = 20;
        //Serial.println(" test burn");
        _delay_ms(BURN_TIME);
        //}
        indiDimm[i] = indiMaxBright;
      }
    }
  }
}

//*/

/*
  void burnIndicators() {
  for (uint8_t k = 0; k < BURN_LOOPS; k++) {
    for (uint8_t digit = 0; digit < 10; digit++) {
      for (uint8_t i = 0; i < 6; i++) {
        indiDimm[i] = 20;
        indiDigits[i]--;
        if (indiDigits[i] < 0) indiDigits[i] = 9;
        //Serial.println(" test burn");
        delay(BURN_TIME);
        indiDimm[i] = indiMaxBright;
      }
    }
  }
  }
*/