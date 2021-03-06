/*Bu kod veri gönderen ve gönderinin alındığını onaylayan bilginin alınması ve bu bilginin
 C portuna yansıtılması şeklindedir.
 Buradaki konfigürasyonlar MIKROC'un sağlamış olduğu CAN kütüphanesinin parametre, bayrak ve
 fonksiyonları kullanılarak yazılmıştır.
 Kütüphaneinin örnek kodundan faydalanılmıştır.
 */

unsigned char Can_Init_Flags, Can_Send_Flags, Can_Rcv_Flags; // can bayrakarı
unsigned char Rx_Data_Len;                                   // Alınan veri
char RxTx_Data[8];                                           // rx/tx data buffer
char Msg_Rcvd;                                               // reception flag
const long ID_1st = 12111, ID_2nd = 3;                       // node IDs rastgele seçilmiştir.
long Rx_ID;

void main() {

  PORTC = 0;                                                // PORTC  temizlendi.
  TRISC = 0;                                                // PORTC çıkış olarak ayarlandı.

  Can_Init_Flags = 0;                                       //
  Can_Send_Flags = 0;                                       // clear flags
  Can_Rcv_Flags  = 0;                                       //

  Can_Send_Flags = _CAN_TX_PRIORITY_0 &                     // can kütüphanesinde bulunan
                   _CAN_TX_XTD_FRAME &                      // CANWrite fonksiyonu kullanımı için bayraklar set edildi
                   _CAN_TX_NO_RTR_FRAME;

  Can_Init_Flags = _CAN_CONFIG_SAMPLE_THRICE &              //  can kütüphanesinde bulunan can kütüphanesinde bulunan
                   _CAN_CONFIG_PHSEG2_PRG_ON &              //  CANInit fonksiyonu parametreleri ayarlandı.
                   _CAN_CONFIG_XTD_MSG &
                   _CAN_CONFIG_DBL_BUFFER_ON &
                   _CAN_CONFIG_VALID_XTD_MSG;

  CANInitialize(1,3,3,3,1,Can_Init_Flags);                  // Initialize CAN module
  CANSetOperationMode(_CAN_MODE_CONFIG,0xFF);               // CONFIGURATION modu olarak ayarla.
  CANSetBaudrate(1, 1, 3, 3, 1, Can_Init_Flags);            //Baudrate 200 kbs olarak ayarlandı.
  //filtre ayarları
  CANSetMask(_CAN_MASK_B1,-1,_CAN_CONFIG_XTD_MSG);          // mask1 filtresinin tüm bitleri 1 olarak ayarlandı.
  CANSetMask(_CAN_MASK_B2,-1,_CAN_CONFIG_XTD_MSG);          // mask2 filtresinin tüm bitleri 1 olarak ayarlandı.
  CANSetFilter(_CAN_FILTER_B2_F4,ID_2nd,_CAN_CONFIG_XTD_MSG);// filtre ID'si ayarlanndı.

  CANSetOperationMode(_CAN_MODE_NORMAL,0xFF);               // Normal mod olarak ayarlandı.

  RxTx_Data[0] = 9;                                         // gönderilecekolan veri 9 olarak atandı.

  CANWrite(ID_1st, RxTx_Data, 1, Can_Send_Flags);           // başlanıç mesajını gönder.

  while(1) {                                                               // sonsuz döngü
    Msg_Rcvd = CANRead(&Rx_ID , RxTx_Data , &Rx_Data_Len, &Can_Rcv_Flags); // mesaj alındı.
    if ((Rx_ID == ID_2nd) && Msg_Rcvd) {                                   // mesaj geldiğinde id kontrol et
      PORTC = RxTx_Data[0];                                                // id uyuyorsa bilgiyi portc'ye yansıt.
      RxTx_Data[0]++ ;                                                     // alınan datayı 1 artır ve yeniden gönder.
      Delay_ms(10);
      CANWrite(ID_1st, RxTx_Data, 1, Can_Send_Flags);                      // yeniden gönder.
    }
  }
}