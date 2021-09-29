/*Bu kod veri g�nderen ve g�nderinin al�nd���n� onaylayan bilginin al�nmas� ve bu bilginin
 C portuna yans�t�lmas� �eklindedir.
 Buradaki konfig�rasyonlar MIKROC'un sa�lam�� oldu�u CAN k�t�phanesinin parametre, bayrak ve
 fonksiyonlar� kullan�larak yaz�lm��t�r.
 K�t�phaneinin �rnek kodundan faydalan�lm��t�r.
 */

unsigned char Can_Init_Flags, Can_Send_Flags, Can_Rcv_Flags; // can bayrakar�
unsigned char Rx_Data_Len;                                   // Al�nan veri
char RxTx_Data[8];                                           // rx/tx data buffer
char Msg_Rcvd;                                               // reception flag
const long ID_1st = 12111, ID_2nd = 3;                       // node IDs rastgele se�ilmi�tir.
long Rx_ID;

void main() {

  PORTC = 0;                                                // PORTC  temizlendi.
  TRISC = 0;                                                // PORTC ��k�� olarak ayarland�.

  Can_Init_Flags = 0;                                       //
  Can_Send_Flags = 0;                                       // clear flags
  Can_Rcv_Flags  = 0;                                       //

  Can_Send_Flags = _CAN_TX_PRIORITY_0 &                     // can k�t�phanesinde bulunan
                   _CAN_TX_XTD_FRAME &                      // CANWrite fonksiyonu kullan�m� i�in bayraklar set edildi
                   _CAN_TX_NO_RTR_FRAME;

  Can_Init_Flags = _CAN_CONFIG_SAMPLE_THRICE &              //  can k�t�phanesinde bulunan can k�t�phanesinde bulunan
                   _CAN_CONFIG_PHSEG2_PRG_ON &              //  CANInit fonksiyonu parametreleri ayarland�.
                   _CAN_CONFIG_XTD_MSG &
                   _CAN_CONFIG_DBL_BUFFER_ON &
                   _CAN_CONFIG_VALID_XTD_MSG;

  CANInitialize(1,3,3,3,1,Can_Init_Flags);                  // Initialize CAN module
  CANSetOperationMode(_CAN_MODE_CONFIG,0xFF);               // CONFIGURATION modu olarak ayarla.
  CANSetBaudrate(1, 1, 3, 3, 1, Can_Init_Flags);            //Baudrate 200 kbs olarak ayarland�.
  //filtre ayarlar�
  CANSetMask(_CAN_MASK_B1,-1,_CAN_CONFIG_XTD_MSG);          // mask1 filtresinin t�m bitleri 1 olarak ayarland�.
  CANSetMask(_CAN_MASK_B2,-1,_CAN_CONFIG_XTD_MSG);          // mask2 filtresinin t�m bitleri 1 olarak ayarland�.
  CANSetFilter(_CAN_FILTER_B2_F4,ID_2nd,_CAN_CONFIG_XTD_MSG);// filtre ID'si ayarlannd�.

  CANSetOperationMode(_CAN_MODE_NORMAL,0xFF);               // Normal mod olarak ayarland�.

  RxTx_Data[0] = 9;                                         // g�nderilecekolan veri 9 olarak atand�.

  CANWrite(ID_1st, RxTx_Data, 1, Can_Send_Flags);           // ba�lan�� mesaj�n� g�nder.

  while(1) {                                                               // sonsuz d�ng�
    Msg_Rcvd = CANRead(&Rx_ID , RxTx_Data , &Rx_Data_Len, &Can_Rcv_Flags); // mesaj al�nd�.
    if ((Rx_ID == ID_2nd) && Msg_Rcvd) {                                   // mesaj geldi�inde id kontrol et
      PORTC = RxTx_Data[0];                                                // id uyuyorsa bilgiyi portc'ye yans�t.
      RxTx_Data[0]++ ;                                                     // al�nan datay� 1 art�r ve yeniden g�nder.
      Delay_ms(10);
      CANWrite(ID_1st, RxTx_Data, 1, Can_Send_Flags);                      // yeniden g�nder.
    }
  }
}