void glitchTick() {
  if (!glitchFlag) {
    if (glitchTimer.tick()) {
      glitchFlag = true;
      indiState = 0;
      glitchCounter = 0;
      glitchMax = random(3, 9);  //2, 6 количество мигов
      glitchIndic = random(0, 6);
      glitchTimer.setTime(random(1, 6) * 30);  // 60
    }
  } else if (glitchFlag && glitchTimer.tick()) {
    //indiDimm[glitchIndic] = indiState * indiMaxBright; //  яркость мигания
    indiDimm[glitchIndic] = indiState * 24; //  яркость мигания
    indiState = !indiState;
    glitchTimer.setTime(random(1, 6) * 30);  // 60 длительность мигов
    glitchCounter++;
    if (glitchCounter > glitchMax) {
      glitchTimer.setTime(random(GLITCH_MIN * 1000L, GLITCH_MAX * 1000L));
      glitchFlag = false;
      indiDimm[glitchIndic] = indiMaxBright;
    }
  }
}