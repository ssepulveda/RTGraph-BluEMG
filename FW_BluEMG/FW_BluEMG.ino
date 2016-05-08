#include <FlexiTimer2.h>

#define LED_STATUS 13
#define BT_CMD_DELAY 10

#define CAL_SIG 9
#define SAMPFREQ 200                      // ADC sampling rate 256
#define TIMER2VAL (1024/(SAMPFREQ))       // Set 256Hz sampli/ng frequency
#define SAMPLE2VOLT 0.004882813

#include <SPI.h> // Included for SFE_LSM9DS0 library
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW

/**
 * Globals
 */ 
volatile unsigned char counter = 0;	  //Additional divider used to generate CAL_SIG
volatile unsigned int ADC_Value = 0;	  //ADC current value
volatile unsigned int delayer = 0;	  //ADC current value
volatile unsigned int dof_status = 0;

volatile float acc[3];
volatile float gyro[3];
volatile float mag[3];


LSM9DS0 dof(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);

void setup()
{
  /**
   * Configure LSM9DS0
   */
  dof_status = dof.begin(dof.G_SCALE_2000DPS, dof.A_SCALE_4G, dof.M_SCALE_4GS,
                         dof.G_ODR_380_BW_100, dof.A_ODR_400, dof.M_ODR_100
                         );
  noInterrupts();  // Disable all interrupts before initialization
  
  /**
   * Pin setup
   */
  pinMode(CAL_SIG, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW);
  Serial.begin(115200);
  
  /**
   * Start BT module (Pre-configured)
   */
  bt_configure();
  bt_start();
   
  /**
   * Configure timer 2 to sample rate
   */
  FlexiTimer2::set(TIMER2VAL, Timer2_Overflow_ISR);
  FlexiTimer2::start();
  Serial.println("\n\n");
  interrupts();  // Enable all interrupts after initialization has been completed
  
  
}

void loop()
{
  dof.readAccel();
  acc[0] = dof.calcGyro(dof.ax);
  acc[1] = dof.calcGyro(dof.ay);
  acc[2] = dof.calcGyro(dof.az);
  
  /*
  dof.readGyro();
  gyro[0] = dof.calcGyro(dof.gx);
  gyro[1] = dof.calcGyro(dof.gy);
  gyro[2] = dof.calcGyro(dof.gz);
  
  dof.readMag();
  mag[0] = dof.calcGyro(dof.mx);
  mag[1] = dof.calcGyro(dof.my);
  mag[2] = dof.calcGyro(dof.mz);
  */
}

/**
 * Timer ISR
 */
 void Timer2_Overflow_ISR()
{
    /**
     * Print ADC / EMG
     */
    Serial.print(analogRead(0) * SAMPLE2VOLT);
    
    int i;
    for (i=0; i<3; i++) {
      Serial.print(",");
      Serial.print(acc[i]);
    }
    
    /*
    for (i=0; i<3; i++) {
      Serial.print(",");
      Serial.print(gyro[i]);
    }
    
    for (i=0; i<3; i++) {
      Serial.print(",");
      Serial.print(mag[i]);
    }
    */
    
    Serial.println();
    
    // Generate the CAL_SIGnal
    counter++;		// increment the devider counter
    if(counter == 12){	// 250/12/2 = 10.4Hz ->Toggle frequency
      counter = 0;
      toggle_GAL_SIG();	// Generate CAL signal with frequ ~10Hz
    }
}

/**
 * Shield CAL
 */
 void toggle_GAL_SIG(void)
 {
   if(digitalRead(CAL_SIG) == HIGH){ digitalWrite(CAL_SIG, LOW); }
   else{ digitalWrite(CAL_SIG, HIGH); }
 }

/**
 * BT Functions
 */

void bt_configure(void)
{
  Serial.print("+++");
  delay(BT_CMD_DELAY);
}

void bt_send_cmd(char *cmd)
{
  Serial.print("AT");
  Serial.print(cmd);
  Serial.print("\r");
  delay(BT_CMD_DELAY);
}

void bt_set_echo(void)
{
  bt_send_cmd("E");
}

void bt_get_info(void)
{
  bt_send_cmd("I");
}

void bt_start(void)
{
  bt_send_cmd("+BTSRV=1");
}
