/*
  Le conversor no pino P1.7 e indica valor em quatro displays
  de 7 segmentos em catodo comum.

  Configuração do display
      B
   -------
  A|     | C
   |  G  |
   -------
  F|     | D
   |  E  |
   ------     . (ponto)
*/
int valor = 0;
float valor_f1  = 0, valor_f2 = 0;

void ini_display()  
{
  //digits
  P1DIR |= BIT0 + BIT1 + BIT2 + BIT3;
  //        b      F 
  P1DIR |= BIT4 + BIT5;
  //        E      A      D      C      G    (ponto)
  P2DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5;
}
void set_display(int v)
{
  //        b      F 
  P1OUT &= ~(BIT4 + BIT5);
  //        E      A      D      C      G
  P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
  switch(v)
  {
    case 0:
      //        b      F 
      P1OUT |= BIT4 + BIT5;
      //        E      A      D      C      G
      P2OUT |= BIT0 + BIT1 + BIT2 + BIT3;
      break;
    case 1:
      //        D      C  
      P2OUT |= BIT2 + BIT3;
      break;
    case 2:
      //        b      F 
      P1OUT |= BIT4 + BIT5;
      //        E       C      G
      P2OUT |= BIT0  + BIT3 + BIT4;
      break;
    case 3:
      //        b  
      P1OUT |= BIT4;
      //        E       D      C      G
      P2OUT |= BIT0 + BIT2 + BIT3 + BIT4;
      break;
    case 4:
      //        A      D      C      G
      P2OUT |= BIT1 + BIT2 + BIT3 + BIT4;
      break;
    case 5:
      //        b    
      P1OUT |= BIT4;
      //        E      A      D      G
      P2OUT |= BIT0 + BIT1 + BIT2 + BIT4;
      break;
    case 6:
      //        b      F 
      P1OUT |= BIT4 + BIT5;
      //        E      A      D       G
      P2OUT |= BIT0 + BIT1 + BIT2  + BIT4;
      break;
    case 7:
      //        b   
      P1OUT |= BIT4 ;
      //         D      C 
      P2OUT |= BIT2 + BIT3 ;
      break;
    case 8:
      //        b      F 
      P1OUT |= BIT4 + BIT5;
      //        E      A      D      C      G
      P2OUT |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4;
      break;
    case 9:
      //        b     
      P1OUT |= BIT4;
      //        E      A      D      C      G
      P2OUT |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4;
      break;
    default:
      break; 
  }
}

void set_digit(int k)
{
  P1OUT |= BIT0 + BIT1 + BIT2 + BIT3;
  switch(k)
  {
    case 0:
      P1OUT ^= BIT0;
      break;
    case 1:
      P1OUT ^= BIT1;
      break;
    case 2:
      P1OUT ^= BIT2;
      break;
    case 3:
      P1OUT ^= BIT3;
      break;
    default:
      break;
      
  }
}

void set_point(bool point)
{
  if (point)  P2OUT |= BIT5;
  else  P2OUT &= ~BIT5;
}

void sweep_display(int v, int ponto)
{
  static int digit;
  int d;
  switch (digit)
  {
    case 0:
      d = v;
      break;
    case 1:
      d = v / 10;
      break;
    case 2:
      d = v / 100;
      break;
    case 3:
      d = v / 1000;
      break;
  }

  set_point(false);
  set_digit(0xff);
  
  set_display(d % 10);
  set_digit(digit);
  
  if (ponto == digit)
    set_point(true);
    
  if (++digit > 3) digit = 0;
}


// the setup routine runs once when you press reset:
void setup() {    

  //configura registradores
  WDTCTL = WDTPW +WDTHOLD; 
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  //ativa referencia interna para conversor
  analogReference(INTERNAL2V5);
  
  // initialize the digital pin as an output.
  P1DIR |= BIT6;  
  ini_display();

  //configura interrupção para CCR0
  TACTL = TASSEL_2 + MC_1;
  TACCR0=1000; // Set TACCR0 = 1000, 1ms em 1MHz
  CCTL0 = CCIE;
}

// the loop routine runs over and over again forever:
void loop() {

}

// The address function that follows this vector statement is placed in the specified location Interrupt Vector table 
__attribute__ ((interrupt(TIMER0_A0_VECTOR))) 
void Timer_A0 (void)
{
  static int msCount=0;// Count milliseconds to allow a 1 second pulse

  //varredura do display
  sweep_display(valor,3);
  
  msCount++;
  if (msCount >= 100)
  {
    P1OUT ^= BIT6;  //led de indicacao
    msCount = 0;
    valor = analogRead(A7);
    valor_f1 = (float)valor*2500/1023;
    valor = (int)( valor_f1*0.0004 + valor_f2*0.9996);
    valor_f2 = valor_f1;
  }
}
