#include "can_handler.h"

bool Can_Handler::req_more;
//int Can_Handler::fuel_tmp_buf[100];
int Can_Handler::fuel_tmp_total;
//int Can_Handler::fuel_tmp_last;
int Can_Handler::fuel_tmp_num;
//int Can_Handler::speed_tmp_buf[100];
int Can_Handler::speed_tmp_total;
int Can_Handler::speed_tmp_num;
int Can_Handler::wheel_angle;
mps_general_params_t Can_Handler::current_params;
mps_etacs_params_t Can_Handler::current_etacs_params;
mps_autoac_params_t Can_Handler::current_autoac_params;
mps_odom_params_t Can_Handler::current_odom_params;
bool Can_Handler::is_bus_active;
bool Can_Handler::is_did_active;
uint32_t Can_Handler::did_active_cnt;

//#define USE_MULTIK

Can_Handler::Can_Handler()
{
    current_params.at_drive = 0xff;
    current_params.speed = 0;
    current_params.raw_fuel = 0;
    current_params.trip_cnt_incr = 0;
    //fuel_tmp_buf[100] = {0};
    fuel_tmp_total = 0;
    //fuel_tmp_last = 0;
    fuel_tmp_num = 0;
    //speed_tmp_buf[100] = {0};
    speed_tmp_total = 0;
    speed_tmp_num = 0;
    wheel_angle = 0;
    Serial.println("Can_handler_construct");
    is_bus_active = false;
    is_did_active = false;
    did_active_cnt = 0;
}

void Can_Handler::set_mask_filt()
{
    //CAN0.setRXFilter(0, ID_AT_ANS, 0xFFF , false);
    //CAN.init_Filt(1, 0, ID_DID_ANS);
    //CAN0.setRXFilter(1, ID_CLIM_ANS, 0xFFF , false);
    //CAN0.setRXFilter(2, ID_MPS_DID_TEMP, 0xFFF , false);
    //CAN0.setRXFilter(3, ID_MPS_SPEED, 0xFFF , false);
    //CAN0.setRXFilter(4, ID_MPS_AT, 0xFFF , false);
    //CAN0.setRXFilter(5, ID_MPS_RPM, 0xFFF , false);

    //CAN0.setRXFilter(0, ID_MPS_SPEED, 0xFFF , false);
    //CAN0.setRXFilter(1, ID_MPS_AT, 0xFFF , false);

}

void Can_Handler::sendPid(unsigned long can_id, unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x21, __pid, 0, 0, 0, 0, 0};
    //Serial.print("SEND PID: 0x");
    //Serial.println(__pid, HEX);

    CAN_FRAME txFrame;
    txFrame.id = can_id;
    txFrame.length = 8;
    memcpy(&txFrame.data, tmp, 8); 

    CAN0.sendFrame(txFrame);
}



void Can_Handler::CanHandlerInit() {
  // initialize can
  Serial.println("CAN init start");
  CAN0.debuggingMode = true;
  CAN0.setCANPins(CAN_PIN_RX,CAN_PIN_TX);
  Serial.println("setCANPins");
  if (!CAN0.begin(500000)) {
        Serial.println("CAN0.begin false");
      ESP.restart();
  }
  CAN0.watchFor(); 
  Serial.println("CAN init ok!");

  //set_mask_filt();
  //Serial.println("CAN filter init ok!");

  CAN0.attachCANInterrupt(0, HandleRxEvent);
  Serial.println("CAN Interrupt init ok!");
  pid_iterator = 0;
  //CAN0.attachCANInterrupt([this](CAN_FRAME* frame){
  //                                                  this->HandleRxEvent(frame);} );

  
}

void Can_Handler::CanHandlerDeInit() 
{

  
    //set_mask_filt();
    //Serial.println("CAN filter init ok!");
  
    CAN0.removeCallback();
    CAN0.disable();
    Serial.println("CAN DEinit ok!");
    //pid_iterator = 0;
    //CAN0.attachCANInterrupt([this](CAN_FRAME* frame){
    //                                                  this->HandleRxEvent(frame);} );
  
}

void Can_Handler::CanHandlerTwaiRestart()
{
    // Serial.println("CAN0.twai_initiate_recovery_v2");
    // twai_initiate_recovery_v2(CAN0.bus_handle);
    //return;
    //------------------------

    //-----------------------------------------
    int res = 0;
    Serial.println("CAN0.removeCallback");
    //CAN0.
    CAN0.removeCallback();
    //CAN0.detachCANInterrupt(0);
    
    Serial.println("CAN0.disable");
    CAN0.disable();

    Serial.println("CAN0.disable ok");

    //return;
    // if(!CAN0.set_baudrate(500000))
    // {
    //     Serial.println("CAN0.set_baudrate ok");
    // }
    // else{
    //     Serial.println("CAN0.set_baudrate false"); 
    //     return;
    // }
    delay(500);
    

    //CAN0.debuggingMode = true;
    // Serial.println("CAN0.setCANPins");
    // CAN0.setCANPins(CAN_PIN_RX,CAN_PIN_TX);
    // Serial.println("CAN0.begin");


    // if (!CAN0.begin(500000)) {
    //      Serial.println("CAN0.begin false");
    //   ESP.restart();
    // }
    CAN0.enable();

    //twai_clear_receive_queue_v2(CAN0.bus_handle);
    //twai_clear_transmit_queue_v2(CAN0.bus_handle);

    CAN0.watchFor(); 
    Serial.println("CAN0.enable");

    Serial.println("CAN0.attachCANInterrupt");
    CAN0.attachCANInterrupt(0, HandleRxEvent);   
    //twai_clear_receive_queue_v2(CAN0.bus_handle);     
} 

void Can_Handler::taskCanSend()
{    
    //Serial.println("CAN0.attachCANInterrupt");
    // if(!is_did_active)
    // //if(false)
    // {
    //     sendPid(ID_CLIM_REQ, PID_CLIMAT_TEMP);
    //     Serial.println("CAN0.taskCanSend");
    // }
    // else
    // {
        //Serial.println("CAN0.taskCanSend");
        switch (pid_iterator)
        {
        case 1:
            sendPid(ID_CLIM_REQ, PID_CLIMAT_TEMP);
            break;

        case 2:
            sendPid(ID_DID_REQ, PID_ODO_INFO);
            break;   

        case 3:
            sendPid(ID_DID_REQ, PID_ECU_VOLTAGE);
            break;

        case 4:
            #ifndef USE_MULTIK 
            sendPid(ID_AT_REQ, PID_AT_INFO);
            #endif
            break; 
        default:
            pid_iterator = 0; 
            break;
        }
        pid_iterator++;  
    // }

} 


void Can_Handler::reset_did_active()
{
    if(did_active_cnt>0)        
        is_did_active = true;
    else is_did_active = false;
    did_active_cnt=0;
}
void Can_Handler::HandleRxEvent(CAN_FRAME* rxFrame) 
{
    //Serial.println("HandleRxEvent");
    is_bus_active = 1;
    uint8_t data[8] = {0};
    memcpy(data, &rxFrame->data, rxFrame->length);
    
    //esp_task_wdt_reset();
    //Serial.println("HandleRxEvent");
     //return;
    
    //uint16_t pid = rxFrame->data.uint8[2];
#ifdef DEBUF_MSG_OUT
    Serial.println("\r\n---------------------------------------------------");
    for (int i = 0; i < rxFrame->length; i++) { // print the data
        Serial.print("0x");
        Serial.print(rxFrame->data.uint8[i], HEX);
        Serial.print("\t");
    }
    Serial.println();
#endif  
    //Serial.println(rxFrame->id);
    switch(rxFrame->id)
    {
    case  ID_AT_ANS: 
    #ifndef USE_MULTIK           
      if(rxFrame->data.uint8[0]==0x10 && rxFrame->data.uint8[2]==0x61 && rxFrame->data.uint8[3]==0x02)  // при использовании мультитроникса не обязательно, просто слушаем, что отвечают ему
      {                                                                   // проверить, что мультик не переводит коробку в режим диагностики, возможно, что если его не будет на связи, коробка нам не ответит
          //memset(stmp,0,8);
          //stmp[0] = 0x30;
          //CAN.sendMsgBuf(ID_AT_REQ, 0, 8, stmp);
          
          

          unsigned char tmp[8] = {0};
          tmp[0] = 0x30;
          CAN_FRAME txFrame;
          txFrame.id = ID_AT_REQ;
          txFrame.length = 8;
          memcpy(&txFrame.data, tmp, 8); 
      
          CAN0.sendFrame(txFrame);
          req_more = true;
          //Serial.println("req_more true");

      }
      else if(req_more == true)
    #endif
      if((uint8_t)rxFrame->data.uint8[0] == 0x21 )//&& (uint8_t)rxFrame->data.uint8[1]==0x00 )
      {

          current_params.t_akpp = (rxFrame->data.uint8[2]-40); 
          req_more = false;
          /*Serial.print("req_more");
          Serial.println(req_more);
          Serial.println(current_params.t_akpp);
          Serial.println(rxFrame->id);
          for (int i = 0; i < rxFrame->length; i++) { // print the data
            Serial.print("0x");
            Serial.print(rxFrame->data.uint8[i], HEX);
            Serial.print("\t");
            }
            Serial.println();       */ 
      }        
      break; 

    case  ID_CLIM_ANS: 
        if(rxFrame->data.uint8[0]==0x07 && rxFrame->data.uint8[1]==0x61 && rxFrame->data.uint8[2]==0x10)
        {
            current_params.t_ext = rxFrame->data.uint8[5]*0.3-29;
            current_params.t_int = rxFrame->data.uint8[3]*0.3-29;    
            current_params.t_engine2 = rxFrame->data.uint8[6]*0.6-23;
            current_params.t_airflow = rxFrame->data.uint8[7]*0.3-29;
        }
        break; 

    case ID_MPS_SPEED:
        current_params.speed = ((uint32_t)rxFrame->data.uint8[0] * 256 + (uint32_t)rxFrame->data.uint8[1]) / 128;
        if(speed_tmp_num>100)
        {
          speed_tmp_num = 0;
          speed_tmp_total = 0;
          Serial.println("overflow speed cnt");
        }
        speed_tmp_total  += ((uint32_t)rxFrame->data.uint8[0] * 256 + (uint32_t)rxFrame->data.uint8[1]) / 128;
        speed_tmp_num++;
        //Serial.print("current_params.speed ");
        //Serial.println(current_params.speed);
        

        double trip;
        trip = (double)((uint32_t)rxFrame->data.uint8[2] * 256 + (uint32_t)rxFrame->data.uint8[3]); 
        if(trip>0)
        {
            trip = ((double)trip)/TRIP_COEFF;            // попытка рассчета исходя из измеренного коэффициента
            trip = trip+current_params.trip_cnt_incr*(0xffff/TRIP_COEFF);
        

            if(trip < current_params.trip_over_cnt) 
            {
                current_params.trip_cnt_incr ++;
                trip = trip+(0xffff/TRIP_COEFF);
                Serial.println(current_params.trip_cnt_incr);
            }
            current_params.trip_over_cnt = trip;
        }

        break;

    case ID_MPS_AT:
    // AT-коробка в наличии!
      //bcomp.at_present = 1;
    // Отображение передачи:
        current_params.at_drive = ((uint8_t)rxFrame->data.uint8[2] >> 4) & 0x0F;
        current_params.at_drive_current = (uint8_t)rxFrame->data.uint8[2] & 0x0F;
        break;

    case ID_MPS_STEER:
    // Положение руля:
    // правое       - 09 FE 10 00 10 00 00 3C
    // центральное  - 10 02 10 00 D0 00 00 69
    // левое        - 15 F9 10 00 E0 00 00 79
        wheel_angle = (int32_t)rxFrame->data.uint8[0]*256 + rxFrame->data.uint8[1];
        wheel_angle = 0x1001 - wheel_angle;    // 1001 - параметр зависит от калибровки датчика рулевого колеса
        break;

    case ID_MPS_RPM:
      // Событие приходит каждые 20мс.
      // Проверка флага каждые 40мс
        current_params.rpm = (uint32_t)rxFrame->data.uint8[1] * 256 + rxFrame->data.uint8[2];
        break;

    case ID_MPS_TORQ:

        current_params.torque = ((uint32_t)rxFrame->data.uint8[0] * 256 + rxFrame->data.uint8[1])/4 - 500;
        is_did_active = true;
        did_active_cnt++;
        break;

    case ID_MPS_DID_TEMP:
        current_params.t_engine = (int32_t)rxFrame->data.uint8[0] - 40;

        /*fuel_tmp_total -= fuel_tmp_buf[fuel_tmp_num];
        fuel_tmp_buf[fuel_tmp_num] = (int32_t)rxFrame->data.uint8[5]*256 + rxFrame->data.uint8[6];
        fuel_tmp_total += (int32_t)rxFrame->data.uint8[5]*256 + rxFrame->data.uint8[6];
        fuel_tmp_num++;
        if(fuel_tmp_num >= FUEL_READ_CNT)fuel_tmp_num=0;
        current_params.raw_fuel = fuel_tmp_total/FUEL_READ_CNT;*/
        if(fuel_tmp_num>100)
        {
            fuel_tmp_num = 0;
            fuel_tmp_total = 0;
            Serial.println("overflow speed cnt");
        }
        fuel_tmp_total += (int32_t)rxFrame->data.uint8[5]*256 + rxFrame->data.uint8[6];
        fuel_tmp_num++;

        //Serial.println(fuel_tmp_num);
        //Serial.println(fuel_tmp_total);
        
        break;
    case  ID_DID_ANS: 
        if(rxFrame->data.uint8[0]==0x06 && rxFrame->data.uint8[1]==0x61 && rxFrame->data.uint8[2]==PID_ODO_INFO)
        {
            current_params.odometer = ((int32_t)rxFrame->data.uint8[4] * 256 + (int32_t)rxFrame->data.uint8[5]) * 256 + (int32_t)rxFrame->data.uint8[6];
            //current_params.odometer = current_params.odometer;
            //Serial.print("ODO");
            //Serial.println(current_params.odometer);
        }
        if(rxFrame->data.uint8[0]==0x06 
            &&  rxFrame->data.uint8[1]==0x61 
                && rxFrame->data.uint8[2]==PID_ECU_VOLTAGE)
        {
            //current_params.v_ecu = (float)((rxFrame->data.uint8[3]*256)+rxFrame->data.uint8[4])/1000.0f; 
            
            current_params.v_ecu = (float)rxFrame->data.uint8[3]*18.68/255.0f;
            //Serial.println(current_params.v_ecu );
        }
        break;
      
    case ID_MPS_ETACS:     
        current_etacs_params.driver_door = rxFrame->data.uint8[2] & 0x02;  
        if(!current_etacs_params.driver_door)current_etacs_params.other_door = rxFrame->data.uint8[2] & 0x01; 
        //current_etacs_params.other_door = rxFrame->data.uint8[2] & 0x01;      
        current_etacs_params.left_turn = (rxFrame->data.uint8[1] & 0x02);
        current_etacs_params.right_turn = (rxFrame->data.uint8[1] & 0x01);
        current_etacs_params.head_lamp_lo = (rxFrame->data.uint8[1] & 0x20);
        current_etacs_params.position_lamp = (rxFrame->data.uint8[0] & 0x04);
        current_etacs_params.head_lamp_hi = (rxFrame->data.uint8[1] & 0x04);
        //Serial.println(rxFrame->data.uint8[1]);
        break;
    
    case ID_MPS_AUTOAC:
        current_autoac_params.ac_on = (rxFrame->data.uint8[3] & 0x08);
        current_autoac_params.flow_down = (rxFrame->data.uint8[5] & 0x04);
        current_autoac_params.flow_up = (rxFrame->data.uint8[5] & 0x02);
        current_autoac_params.flow_wind = (rxFrame->data.uint8[5] & 0x08);
        current_autoac_params.inside_air = (rxFrame->data.uint8[3] & 0x02);
        current_autoac_params.mirror_heat = (rxFrame->data.uint8[4] & 0x20);
        current_autoac_params.fan_speed = (rxFrame->data.uint8[5] & 0xf0)>>4;
        //current_autoac_params.setted_temp = (rxFrame->data.uint8[0] & 0x3e)>>1;
        if(rxFrame->data.uint8[0] <= 50) current_autoac_params.setted_temp = 0;
        else current_autoac_params.setted_temp = (rxFrame->data.uint8[0]-63);
        break;

    default:
        break;  
    }
  //Serial.println("end parce data");


}

void Can_Handler::calculate()
{
    if(speed_tmp_num > 0)
    {
      current_params.avrg_speed = (double)speed_tmp_total/speed_tmp_num;
      speed_tmp_total = 0;
      speed_tmp_num = 0;
    }
    else
    {
      current_params.avrg_speed = speed_tmp_total;
    }
    if(fuel_tmp_num>0)
    {
      current_params.raw_fuel = fuel_tmp_total/fuel_tmp_num;
      fuel_tmp_total = 0;
      fuel_tmp_num = 0;
    }

    current_params.fuel_in_H = ((double)current_params.raw_fuel * 3.6)/1000.0f;
    current_params.fuel_in_H = current_params.fuel_in_H/0.83f;
    current_params.fuel_in_100 = (current_params.fuel_in_H * 100)/current_params.avrg_speed;
    current_params.total_fuel += current_params.raw_fuel/(100000.0f*0.83f);
    current_params.raw_fuel = 0; 

    double d_dist = (double)current_params.avrg_speed / (3600.0f);
    current_params.trip += d_dist*1.015f;  
    current_odom_params.trip_a += d_dist*1.015f; 
    current_odom_params.trip_b += d_dist*1.015f; 
    current_odom_params.trip_curr = current_params.trip;
    current_odom_params.odometer = current_params.odometer;
}
