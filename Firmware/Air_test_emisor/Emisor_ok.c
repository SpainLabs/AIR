/* ----------------------------------------------
CODIGO PARA TESTEAR LA COMUNICACION Y LA PLACA SL AIR V1 REV A.
EL CODIGO CORRESPONDERIA AL HW EMISOR. REALIZA LA MEDICION DEL ADC EN EL
CANAL 1 (PIN_A1) Y LO ENVIA.
 -----------------------------------------------
 */
 
#include <16F876A.h>
#device ADC = 10
#FUSES NOWDT, XT, PUT, NOPROTECT, NODEBUG, NOBROWNOUT, NOLVP, NOCPD, NOWRT
#use delay(clock=4000000)


#include "lib_rf2gh4_10.h"
#include "string.h"

#byte porta=0x05
#byte portb=0x06
int8 dato = 12;
int contador = 0;
int8 buffer[8];  // Buffer de 8 bytes.
char buff[24];   //Buffer para el frame.
int8 ret2;
int temperatura = 0;
float temperatura1 = 0.0;

//-----------------------------------------------------
// Identificador (ID unico para identificar el mote).
int8 ident = 90;
//-----------------------------------------------------


void Procesar()
{
   //Comando 100;100.
   //Respuesta a la solicitud del estado del mote.
   //Envio del numero identificativo y 3 valores.
   if ( (buffer[0]==100) && (buffer[1]==100)){
      //Envio info.
      contador= ++contador;
      RF_DATA[0] = ident;
      RF_DATA[1] = 13;
      RF_DATA[2] = 4;
      RF_DATA[3] = contador;
      RF_DIR=0x08;           // Dirección del receptor.
      ret2=RF_SEND();        // Enviar datos.
      delay_ms(50);
      output_high(PIN_B5);
      delay_ms(1000);
      output_low(PIN_B5);
      delay_ms(1000);
      
   }
}  
// Funcion envio del frame en 3 bloques de 8 bits.
void Send_frame(){
   int i;
   int fc;
   for (fc=0;fc<24;fc++){
       for (i=0;i<8;i++){
          RF_DATA[i] = Buff[fc];
          fc++;         
       }
       fc--; //Restamos 1 para no saltarnos un byte en el siguiente bucle, pues
             //se lo suma antes de ejecutar las instrucciones.
       RF_DIR=0x08;           // Dirección del receptor.
       ret2=RF_SEND();        // Enviar datos.
       //Led de estado.
       output_high(PIN_C0);
       delay_ms(300);         // Espero un tiempo para que se envie el dato.
       output_low(PIN_C0);
   }
}

#int_ext
void int_RB0()
{
   int8 ret1;
   
   ret1 = RF_RECEIVE();
   if ( (ret1 == 0) || (ret1 == 1) )
   {
      do
      { 
        //Asigno los bits de entrada a un buffer secundario.
        buffer[0] = RF_DATA[0];
        buffer[1] = RF_DATA[1];
        buffer[2] = RF_DATA[2];
        buffer[3] = RF_DATA[3];
        buffer[4] = RF_DATA[4]; 
        buffer[5] = RF_DATA[5]; 
        buffer[6] = RF_DATA[6]; 
        buffer[7] = RF_DATA[7]; 
        ret1 = RF_RECEIVE(); 
        Procesar(); //Proceso la informacion recibida.
      }  while ( (ret1 == 0) || (ret1 == 1) );
   }  
}

void main()
{  
   //int8 variable8L, variable8H;
   //int variable16;
   char s[7];  //Variable string para el dato de la temperatura.
   char frase[14] = "Temperatura: ";
   //Variables para el frame de pruebas.
   int g;
   int gg;
   //int h;
   RF_INT_EN();              // Habilitar interrupción RB0/INT.
   RF_CONFIG_SPI();          // Configurar módulo SPI del PIC.
   RF_CONFIG(0x40,0x01);     // Configurar módulo RF canal y dirección.
   RF_ON();                  // Activar el módulo RF.
   
   delay_ms(5);
   
   set_tris_a(0b111111);     // Todo el puerto A como entradas.
   output_low(PIN_B5);
   output_low(PIN_B4);
   output_low(PIN_B2);
   output_low(PIN_C0);
   
   // Configurar ADC.
   setup_adc_ports(all_analog);//Definimos las entradas analógicas.
   delay_us (20);
   setup_adc (ADC_CLOCK_INTERNAL);//Configuramos el modo del convertidor.
   delay_ms (20);
   /*
   //------------------------------
   // Creo un frame para pruebas.
   // Las variables g,h estan declaradas al principio del main.
   //------------------------------
    
     for (g=0;g<24;g++)
         {
         for (h=0;h<8;h++)
            {
            Buff[g] = h;
            g++;
            } 
          g--;  
         }
   //------------------------------
   
   
   Buff[0] = 2;
   Buff[1] = 4;
   Buff[2] = 3;
   Buff[3] = 5;
   Buff[4] = 0;
   Buff[5] = 0;
      
   for (g=9;g<24;g++)
         {
         for (h=0;h<8;h++)
            {
            Buff[g] = h;
            g++;
            } 
          g--;  
         }
   */
   while(true)
   {  
      int l; //Indice para parpadeo led estado.
      int t;
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
        temperatura1=0.0;
        //Varias medidas para sacar una media de la temperatura.
        for (t=0;t<1;t++){
         temperatura1 = temperatura1 + read_adc();
         delay_us(30);        
        }
        temperatura1 = (temperatura1/1); 
        
        //temperatura1=read_adc();
       // delay_us (20);
       // setup_adc(adc_off);
       // SETUP_ADC_PORTS(no_ANALOGs);
       delay_ms(1);
       //Led de estado.
       for (l=0;l<2;l++){
        output_low(PIN_C0);
        delay_ms(200);         
        output_high(PIN_C0);
        delay_ms(200);
        output_low(PIN_C0);}
       
     //Calculo de la temperatura a partir del valor ADC.MCP9700      
     temperatura1 = (temperatura1*(5.0/1024.0));
     temperatura1 = temperatura1 - 0.5;
     temperatura1 = temperatura1/0.01;
     //Led estado.
    for (l=0;l<3;l++){
        output_low(PIN_C0);
        delay_ms(200);         
        output_high(PIN_C0);
        delay_ms(200);
        output_low(PIN_C0);}
        delay_ms(2000);
     //Conversion float to string.
     sprintf(s,"%2.2f",temperatura1);   
     // Cargo informacion al frame para ser enviada.
     for (g=0;g<13;g++)
         {         
            Buff[g] = frase[g];           
         }
         g++;
      for (gg=0;gg<7;gg++)
         {         
            Buff[g] = s[gg];
            g++;
         }
      
      //variable8L=variable16;
      //variable8H=variable16>>8;
      
      //Buff[6] = variable8H;
      //Buff[7] = variable8L;
      
      
      Send_frame(); //Envio el frame por RF.
      
      output_high(PIN_B4);
      delay_ms(1000);
      output_low(PIN_B4);
      delay_ms(5000);
      
   }
}
