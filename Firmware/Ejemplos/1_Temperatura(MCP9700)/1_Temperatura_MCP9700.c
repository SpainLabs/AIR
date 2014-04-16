/* ----------------------------------------------
* Proyecto de la Comunidad SpainLabs - SL Air Mote
* Web Oficial: www.spainlabs.com
* Foro de desarrollo: http://www.spainlabs.com/foro/viewforum.php?f=44
*
* Titulo: 01 Ejemplo sensor de Temperatura MCP9700
*
* Descripcion: Ejemplo de medida del sensor de temperatura MCP9700 de Microchip.
* El mote realiza la medida de la temperatura y envia al master la informacion
* usando el frame.
*
* Hardware: Air v1.0 rev A
* Web version Hardware: http://www.spainlabs.com/foro/viewtopic.php?f=44&t=1337
* Dispositivo: Air v1.0 rev A configurado como mote.
*
* Version: 002
* Fecha: 15/0/14
* Autor: Grafisoft
 -----------------------------------------------
 */
 
#include <16F876A.h>
#device ADC = 10     // ADC configurado a 10 bits.
#FUSES NOWDT, XT, PUT, NOPROTECT, NODEBUG, NOBROWNOUT, NOLVP, NOCPD, NOWRT
#use delay(clock=4000000)

// Libreria modulo comunicacion NRF24L01+
#include "lib_rf2gh4_10.h"

#include "string.h"

#byte porta=0x05
#byte portb=0x06

int8 buffer[8];  // Buffer de 8 bytes.
char buff[32];   //Buffer para el frame.
int8 ret2;
int l; //Indice para parpadeo led estado.

float temperatura = 0.0;

//-----------------------------------------------------
// Identificador (Nombre del dispositivo).
// Nota: Vector de 8 bytes. 7 para caracteres, 1 para vector NULL.
char ident[9] = "Mote_01 "; 
//-----------------------------------------------------


// Funcion envio del frame en 4 bloques de 8 bits.
void Send_frame(){
   int i;
   int fc;
   for (fc=0;fc<32;fc++){
       for (i=0;i<8;i++){
          RF_DATA[i] = Buff[fc];
          fc++;         
       }
       fc--; //Restamos 1 para no saltarnos un byte en el siguiente bucle, pues
             //se lo suma antes de ejecutar las instrucciones.
       RF_DIR=0x08;           // Dirección del receptor.
       ret2=RF_SEND();        // Enviar datos.
       
       //Led de estado.
       for (l=0;l<3;l++){
        output_low(PIN_C0);
        delay_ms(200);         
        output_high(PIN_C0);
        delay_ms(200);
        output_low(PIN_C0);
        }
       
   }
}

// Interrupcion. Lee el paquete recibido.
#int_ext
void int_RB0()
{
   int8 ret1;
   
   ret1 = RF_RECEIVE();
   if ( (ret1 == 0) || (ret1 == 1) )
   {
      do
      { 
        //Asigno los 8 bytes de entrada a un buffer secundario.
        buffer[0] = RF_DATA[0];
        buffer[1] = RF_DATA[1];
        buffer[2] = RF_DATA[2];
        buffer[3] = RF_DATA[3];
        buffer[4] = RF_DATA[4]; 
        buffer[5] = RF_DATA[5]; 
        buffer[6] = RF_DATA[6]; 
        buffer[7] = RF_DATA[7]; 
        ret1 = RF_RECEIVE(); 
        
      }  while ( (ret1 == 0) || (ret1 == 1) );
   }  
}

void main()
{  
   
   char s[7];  //Variable string para el dato de la temperatura.
   char frase[14] = "Temperatura: ";
   //Variables para el frame de pruebas.
   int g;
   int gg;
  
   RF_INT_EN();              // Habilitar interrupción RB0/INT.
   RF_CONFIG_SPI();          // Configurar módulo SPI del PIC.
   RF_CONFIG(0x40,0x01);     // Configurar módulo RF canal y dirección.
   RF_ON();                  // Activar el módulo RF.
   
   delay_ms(5);
   
   set_tris_a(0b111111);     // Todo el puerto A como entradas.
   
   output_low(PIN_C0);       //Led de estado apagado.
   
   // Configurar ADC.
   setup_adc_ports(all_analog);//Definimos las entradas analógicas.
   delay_us (20);
   setup_adc (ADC_CLOCK_INTERNAL);//Configuramos el modo del convertidor.
   delay_ms (20);
   
   while(true)
   {  
      
      int t; //Indice para varias medidas de temperatura.
      
      //Led de estado.
       for (l=0;l<2;l++){
        output_low(PIN_C0);
        delay_ms(200);         
        output_high(PIN_C0);
        delay_ms(200);
        output_low(PIN_C0);}
        delay_ms(2000);
      
      //LECTURA DE SENSORES.      
        set_adc_channel(1); //Selección del canal analógico.
        delay_us (20);
        temperatura=0.0;
        
        //Varias medidas para sacar una media de la temperatura.
        for (t=0;t<5;t++){
         temperatura += read_adc();
         delay_ms(10);        
        }
        temperatura = (temperatura/5);       
        delay_ms(1);
        
       
       
     //Calculo de la temperatura dada por el sensor MCP9700. 
     //ADC configurado a 10 bits. Vcc = 5v.
     temperatura = (temperatura*(5.0/1024.0));
     temperatura = temperatura - 0.5;
     temperatura = temperatura/0.01;
     
     //Conversion float to string.
     sprintf(s,"%2.2f",temperatura);
     
     // Cargo informacion al frame para ser enviada.
     gg = 0;
     for (g=0;g<9;g++){
          Buff[gg] = ident[g];
          gg++;
     }
     
     for (g=0;g<13;g++)
         {         
            Buff[gg] = frase[g];
            gg++;
         }
         
      for (g=0;g<5;g++)
         {         
            Buff[gg] = s[g];
            gg++;
         }
         
      for (g=0;g<5;g++)
         {            
            Buff[gg] = " ";
            gg++;
         }         
      
      Send_frame(); //Envio el frame por RF.
      
      //Tiempo de espera.
      Delay_ms(10000);
         
   }
}
