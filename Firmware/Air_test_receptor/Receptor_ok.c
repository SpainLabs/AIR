#include <16F876A.h>
#FUSES NOWDT, XT, PUT, NOPROTECT, NODEBUG, NOBROWNOUT, NOLVP, NOCPD, NOWRT
#use delay(clock=4000000)
#use rs232(baud=9600, xmit=pin_C6, rcv=pin_C7)

#include "lib_rf2gh4_10.h"
//#include<stdio.h>
#include "string.h"

#byte porta=0x05           // Dirección del puerto A.
//int8 valor;
int8 Dato1 = 0;
float Bat_lvl = 0.00; 
char opcion;
int8 buffer[8];
int8 Buff[24]; //Buffer para el frame.
int8 ret2;
float temperatura = 0.0;
int8 count = 0;
int8 b = 0;

// Funciones.

void procesar(){
    //int fc;
    int i;
    
     for (i=0;i<8;i++){
         printf("%c", RF_DATA[i]);        
    }
    //printf(":");
    // Al tercer paquete, cambio de linea.
    b++;
    if (b==3){
    printf(" \r\n ");
    b=0;
    }
    /*
    printf(" \r\n ");
    printf("variableH: %U", RF_DATA[6]);
    printf(" \r\n ");
    printf("variableL: %U ", RF_DATA[7]);
    printf(" \r\n ");
    */
}


#int_ext                   // Interrupción del módulo RF.
void int_RB0()
{
    int8 ret1;   
    ret1 = RF_RECEIVE();
     
   if ( (ret1 == 0) || (ret1 == 1) )
   {
      do
      {  
          buffer[0] = RF_DATA[0];
          buffer[1] = RF_DATA[1];
          buffer[2] = RF_DATA[2];
          buffer[3] = RF_DATA[3];
          buffer[4] = RF_DATA[4];
          buffer[5] = RF_DATA[5];
          buffer[6] = RF_DATA[6];
          buffer[7] = RF_DATA[7];
          ret1 = RF_RECEIVE(); 
          procesar();  //Procesamos cada paquete que se recibe.
          
      } 
      while ( (ret1 == 0) || (ret1 == 1) );{
               ret1 = RF_RECEIVE(); 
          
      }
      
   }
   
}

void mote1(){

   printf("\r\n" );
   printf("Estado Mote 1 \r\n" );
   printf("Conectando..." );
   //Envio un comando para solicitar informacion.
   RF_DATA[0] = 100;
   RF_DATA[1] = 100;
   RF_DATA[2] = 100;
   RF_DATA[3] = 100;
   RF_DIR=0x01;           // Dirección del receptor.
   ret2=RF_SEND();        // Enviar datos.
   delay_ms(3000);        // Tiempo de espera para recibir la respuesta.
   Dato1 = buffer[0];
   /*
   Bat_lvl = (Dato1/256.0)*10.0;
   temperatura = buffer[3];
   temperatura = (temperatura*(5.0/256.0));
   temperatura = temperatura - 0.5;
   temperatura = temperatura/0.01;
   */
   printf("OK \r\n" );
   //printf("Nivel Bateria: %2.2f V \r\n", Bat_lvl);
   printf("Dato recibido_[0]: %U ",buffer[0]);
   printf("  Dato recibido_[1]: %D ",buffer[1]);
   printf("  Dato recibido_[2]: %D \r\n",buffer[2]);
   printf("Dato recibido_[3]: %D \r\n ",buffer[3]);
   printf(" \r\n ");
  // printf("Temperatura_[3]: %2.2f \r\n ",temperatura);
}



void modo_2(){
   printf("---- Pulsado 2---- \r\n");
   printf("---- Modo grabar datos---- \r\n");
   while(1)
   {
   RF_DATA[0] = 100;
   RF_DATA[1] = 100;
   RF_DATA[2] = 100;
   RF_DATA[3] = 100;
   RF_DIR=0x01;           // Dirección del receptor.
   ret2=RF_SEND();        // Enviar datos.
   delay_ms(3000);
   Dato1 = buffer[0];
   Bat_lvl = (Dato1/256.0)*10.0;
   temperatura = buffer[3];
   temperatura = (temperatura*(5.0/256.0));
   temperatura = temperatura - 0.5;
   temperatura = temperatura/0.01;
   // Devuelvo el valor por cada linea.
   printf("%2.2f \r\n ",temperatura);
   delay_ms(5000);
   }
}

// Test de frame. Impresion por pantalla.
void modo_4(){
      //float f=561.234567;
     // int in=134;
     // char s2[20];
     // char s[20];
     // int longitud;
      
      int j;
      int g;
      printf("---- Pulsado 4---- \r\n");    
     // Generamos un frame de numeros.
     for (g=0;g<24;g++)
         {
         int h;
         for (h=0;h<8;h++)
            {
            Buff[g] = h;
            g++;
            } 
          g--;  
         }
      printf("Frame Buffer: ");   
    for (j=0;j<24;j++){
         printf("%D", Buff[j]);     
    
    }
    /*
    printf(" \r\n ");
    
    
    sprintf(s,"%3.4f",f);
    printf("%s",s);
    printf(" \r\n ");
    
    printf("en int: %U", in); 
    printf(" \r\n ");
    printf("en int - string: %s", in);
    printf(" \r\n ");
    sprintf(s2,"%U",in);
    printf("%s",s2);
    printf(" \r\n ");
    

   longitud = strlen(s);
   printf( "La cadena \"%s\" tiene %i caracteres.\n", s, longitud );
   */
}

void Modo_escucha(){
   printf("(6)- Modo Escucha \r\n" );
   RF_DATA[3] = 100;
   RF_DIR=0x01;           // Dirección del receptor.
   ret2=RF_SEND();        // Enviar datos.
   delay_ms(200);
   while(1){
      output_high(PIN_C0);
      delay_ms(1000);
      output_low(PIN_C0);
      delay_ms(1000);
   }
  }

void modo_continuo(){
// Modo continuo, sin salida. Solicita al mote cada X segundos su estado.
   
   while(1){
       printf("---- Pulsado 5 - Modo Continuo v0.1 ---- \r\n");
       printf("  Contador: %D \r\n",count);
       count++;
       Mote1();
       delay_ms(5000);
   }
}

void main()                  //Programa principal
{
   set_tris_a(0b000000);     // Todo el puerto A como salida.
   porta=0;                  // Inicialmente lo ponemos a cero.
  // output_high(PIN_B2);      // ON/OFF Consumo modulo de comunicacion.   
   RF_INT_EN();              // Habilitar interrupción RB0/INT.
   RF_CONFIG_SPI();          // Configurar módulos SPI del PIC.
   RF_CONFIG(0x40,0x08);     // Configurar módulo RF (canal y dirección).
   RF_ON();                  // Activar el módulo RF.
   
   
   printf("Receptor listo:");
   
   while(1) // Bucle infinito.
       {
       
       printf("\r\n" );
       printf("\r\n" );
       printf("Firm SL-Air Mote_Master Beta 0.02\r\n");
       printf("--- Menu ---\r\n");
       printf("Canal;Direccion Master...: 0x40;0x08 \r\n");
       printf("(1)- Estado Mote 1 \r\n" );
       printf("(3)- Modo grabar datos \r\n" );
       printf("(4)- Test Buffer Frame \r\n" );
       printf("(5)- Modo Continuo v0.1 \r\n" );
       printf("(6)- Modo Escucha \r\n" );
       
      //RF_OFF();
       opcion = getc();
         switch(opcion){
         case ' ': printf("---- Pulsado espacio ---- \r\n");
         break;
         case '0': modo_continuo();
         break;       
         case '1': Mote1();
         break;
         case '2': modo_2();         
         break;
         case '3': printf("---- Pulsado 3---- \r\n");
         break;
         case '4': modo_4();
         break;
         case '5': modo_continuo();
         break;
         case '6': Modo_escucha();
         break;
       delay_ms(1000);
           
       }
}
}

