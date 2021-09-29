/*Bu kod veri gönderen ve gönderinin alýndýðýný onaylayan bilginin alýnmasý ve bu bilginin
 C portuna yansýtýlmasý þeklindedir.
 Buradaki konfigürasyonlar MIKROC'un saðlamýþ olduðu CAN kütüphanesinin parametre, bayrak ve
 fonksiyonlarý kullanýlarak yazýlmýþtýr.
 Kütüphaneinin örnek kodundan faydalanýlmýþtýr.
 */

unsigned char Can_Init_Flags, Can_Send_Flags, Can_Rcv_Flags; // can bayrakarý
unsigned char Rx_Data_Len;                                   // Alýnan veri
char RxTx_Data[8];                                           // rx/tx data buffer
char Msg_Rcvd;                                               // reception flag
const long ID_1st = 12111, ID_2nd = 3;                       // node IDs rastgele seçilmiþtir.
long Rx_ID;

void main() {

  PORTC = 0;                                                // PORTC  temizlendi.
  TRISC = 0;                                                // PORTC çýkýþ olarak ayarlandý.

  Can_Init_Flags = 0;                                       //
  Can_Send_Flags = 0;                                       // clear flags
  Can_Rcv_Flags  = 0;                                       //

  Can_Send_Flags = _CAN_TX_PRIORITY_0 &                     // can kütüphanesinde bulunan
                   _CAN_TX_XTD_FRAME &                      // CANWrite fonksiyonu kullanýmý için bayraklar set edildi
                   _CAN_TX_NO_RTR_FRAME;

  Can_Init_Flags = _CAN_CONFIG_SAMPLE_THRICE &              //  can kütüphanesinde bulunan can kütüphanesinde bulunan
                   _CAN_CONFIG_PHSEG2_PRG_ON &              //  CANInit fonksiyonu parametreleri ayarlandý.
                   _CAN_CONFIG_XTD_MSG &
                   _CAN_CONFIG_DBL_BUFFER_ON &
                   _CAN_CONFIG_VALID_XTD_MSG;

  CANInitialize(1,3,3,3,1,Can_Init_Flags);                  // Initialize CAN module
  CANSetOperationMode(_CAN_MODE_CONFIG,0xFF);               // CONFIGURATION modu olarak ayarla.
  CANSetBaudrate(1, 1, 3, 3, 1, Can_Init_Flags);            //Baudrate 200 kbs olarak ayarlandý.
  //filtre ayarlarý
  CANSetMask(_CAN_MASK_B1,-1,_CAN_CONFIG_XTD_MSG);          // mask1 filtresinin tüm bitleri 1 olarak ayarlandý.
  CANSetMask(_CAN_MASK_B2,-1,_CAN_CONFIG_XTD_MSG);          // mask2 filtresinin tüm bitleri 1 olarak ayarlandý.
  CANSetFilter(_CAN_FILTER_B2_F4,ID_2nd,_CAN_CONFIG_XTD_MSG);// filtre ID'si ayarlanndý.

  CANSetOperationMode(_CAN_MODE_NORMAL,0xFF);               // Normal mod olarak ayarlandý.

  RxTx_Data[0] = 9;                                         // gönderilecekolan veri 9 olarak atandý.

  CANWrite(ID_1st, RxTx_Data, 1, Can_Send_Flags);           // baþlanýç mesajýný gönder.

  while(1) {                                                               // sonsuz döngü
    Msg_Rcvd = CANRead(&Rx_ID , RxTx_Data , &Rx_Data_Len, &Can_Rcv_Flags); // mesaj alýndý.
    if ((Rx_ID == ID_2nd) && Msg_Rcvd) {                                   // mesaj geldiðinde id kontrol et
      PORTC = RxTx_Data[0];                                                // id uyuyorsa bilgiyi portc'ye yansýt.
      RxTx_Data[0]++ ;                                                     // alýnan datayý 1 artýr ve yeniden gönder.
      Delay_ms(10);
      CANWrite(ID_1st, RxTx_Data, 1, Can_Send_Flags);                      // yeniden gönder.
    }
  }
}