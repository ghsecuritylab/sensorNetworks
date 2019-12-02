#include "mbed.h"
#include "MBed_Adafruit_GPS.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define UTC_DIFF 1

extern long int numberOfMeasures;
extern long int time_interval;
extern int mode;

extern Serial pc; //9600 baudios - used to print some values


float valueSM=0.0;
float valueLight = 0.0;
uint16_t CRGB_values[4];
float temp = 0.0, hum = 0.0;
float accel_data[3];
char colour[6] = "     ";
int counter =  0;
extern Adafruit_GPS myGPS;

float soilmoisMin, soilmoisMax;
float lightMin,lightMax;
float tempMin, tempMax;
float humMin, humMax;

float soilmoisSum;
float lightSum;
float tempSum;
float humSum;

float accel_max_values[3];
float accel_min_values[3];

int most_dominant_rgb[3];

void printAll(){
    // print sensor readings
  pc.printf("Colour sensor data: Clear (%d); Red (%d); Green (%d); Blue (%d) -- Dominant colour: %s\r\n", CRGB_values[0], CRGB_values[1], CRGB_values[2], CRGB_values[3], colour);
  pc.printf("Accel data: X = %f g; Y = %f g; Z = %f g\n\r", accel_data[0], accel_data[1], accel_data[2]);
  
  pc.printf("Temp/hum sensor data: Temp: %.1f C\tHum: %.1f %%\r\n", temp, hum);
	
  		int local_time = myGPS.hour + UTC_DIFF;
		if(local_time ==24){
			local_time = 0;
			//myGPS.day += 1;
		}
    
  if (myGPS.fix){
    pc.printf("GPS: #Sats: %d; Lat: %5.2f%c; Lon: %5.2f%c; Altitude: %5.2f; ", myGPS.satellites, myGPS.latitude/100, myGPS.lat, myGPS.longitude/100, myGPS.lon, myGPS.altitude);
    pc.printf("GPS_time (local_time): %d:%d:%d.%u\r\n",local_time, myGPS.minute, myGPS.seconds, myGPS.milliseconds);
  }else
    pc.printf("GPS_time: %d:%d:%d; GPS not fixed\r\n", local_time, myGPS.minute, myGPS.seconds);

		pc.printf("Light: %.1f %%\n\r", valueLight);
		pc.printf("Value soil mois: %.1f %%\n\r", valueSM);
    pc.printf("------------------------------------------------------------\r\n");

}
int saveData(){
  counter %= numberOfMeasures;
	if(counter == 0){
    memset(accel_max_values, 0, sizeof(accel_max_values));
    memset(most_dominant_rgb, 0, sizeof(most_dominant_rgb));
    accel_min_values[0] = 1000;
    accel_min_values[1] = 1000;
    accel_min_values[2] = 1000;

    soilmoisMin = 1000;
    soilmoisMax = 0;
    lightMin = 1000;
    lightMax = 0;
    tempMin = 1000;
    tempMax = 0;
    humMin = 1000;
    humMax = 0;
    soilmoisSum = 0;
    lightSum = 0;
    tempSum = 0;
    humSum = 0;
  }
  accel_max_values[0] = MAX(accel_max_values[0], accel_data[0]);
  accel_max_values[1] = MAX(accel_max_values[1], accel_data[1]);
  accel_max_values[2] = MAX(accel_max_values[2], accel_data[2]);

  accel_min_values[0] = MIN(accel_min_values[0], accel_data[0]);
  accel_min_values[1] = MIN(accel_min_values[1], accel_data[1]);
  accel_min_values[2] = MIN(accel_min_values[2], accel_data[2]);
  
  if(!strcmp(colour, "RED  ")) most_dominant_rgb[0] += 1;
  else if(!strcmp(colour, "GREEN")) most_dominant_rgb[1] += 1;
  else if(!strcmp(colour, "BLUE ")) most_dominant_rgb[2] += 1;
  
  soilmoisMax = MAX(soilmoisMax, valueSM);
  soilmoisMin = MIN(soilmoisMin, valueSM);

  lightMax = MAX(lightMax, valueLight);
  lightMin = MIN(lightMin, valueLight);
  
  tempMax = MAX(tempMax, temp);
  tempMin = MIN(tempMin, temp);
  
  humMax = MAX(humMax, hum);
  humMin = MIN(humMin, hum);
  
  soilmoisSum += valueSM;
  lightSum += valueLight;
  tempSum += temp;
  humSum += hum;
    
  counter++;
  return counter;
}
void printMean_Max_Min(){
  pc.printf("************************************************************\r\n");
  pc.printf("                 %d values have been read\r\n", numberOfMeasures);
  pc.printf("\r\n");

  pc.printf("Max values acc: X = %f g; Y = %f g; Z = %f g\r\n", accel_max_values[0], accel_max_values[1], accel_max_values[2]);
  pc.printf("Min values acc: X = %f g; Y = %f g; Z = %f g\r\n", accel_min_values[0], accel_min_values[1], accel_min_values[2]);
  
  if(most_dominant_rgb[0] >= most_dominant_rgb[1] && most_dominant_rgb[0] > most_dominant_rgb[2])
    pc.printf("Most dominant colour in last hour: Red\r\n");
  else if(most_dominant_rgb[1] >= most_dominant_rgb[0] && most_dominant_rgb[1] > most_dominant_rgb[2])
    pc.printf("Most dominant colour in last hour: Green\r\n");
  else if(most_dominant_rgb[2] >= most_dominant_rgb[0] && most_dominant_rgb[2] > most_dominant_rgb[1])
    pc.printf("Most dominant colour in last hour: Blue\r\n");
  
  pc.printf("Soilmois max//min: %.1f %% // %.1f %%\r\n", soilmoisMax, soilmoisMin);
  pc.printf("Temp max//min: %.1f C // %.1f C\r\n", tempMax, tempMin);
  pc.printf("Hum max//min: %.1f %% // %.1f %%\r\n", humMax, humMin);
  pc.printf("Light max//min: %.1f %% // %.1f %%\r\n", lightMax, lightMin);

  pc.printf("Soilmois mean: %.1f %%\r\n", soilmoisSum/numberOfMeasures);
  pc.printf("Temp mean: %.1f C\r\n", tempSum/numberOfMeasures);
  pc.printf("Hum mean: %.1f %%\r\n", humSum/numberOfMeasures);
  pc.printf("Light mean: %.1f %%\r\n", lightSum/numberOfMeasures);
  pc.printf("------------------------------------------------------------\r\n");

}

 void advancedMode(){
	 pc.printf("\r\n************INFO: ADVANCED MODE INICIALITED************\r\n");
	 char terminal_1[10];
   
   pc.printf("Please enter the number of measures (>2): ");
   int i=0;
      while(true){
      char c = pc.getc();
      if(c == 13){
       terminal_1[i] =0;
        break;
      }
      if (c<48 || c>57){ //13 = retorno de carro
      pc.printf("\r\nERROR. Only positives numbers.\r\n");
      pc.printf("Enter again: ");
      }else{
       terminal_1[i] = c;
        i++;
        if(i==10) break;
     }
      
   
  }
   
  numberOfMeasures = atoi(terminal_1);
  pc.printf("\r\nNumber of measures: %ld", numberOfMeasures);

  char terminal_2[10];
   
  pc.printf("\r\nPlease enter the time interval between reads in seconds (>1s): ");
  i=0;
    while(true){
    char c = pc.getc();
    if(c == 13){
     terminal_2[i] =0;
      break;
    }
    if (c<48 || c>57){
    pc.printf("\r\nERROR. Only positives numbers.\r\n");
    pc.printf("Enter again: ");
    }else{
     terminal_2[i] = c;
      i++;
      if(i==10) break;
   }
 }
   
  time_interval = atoi(terminal_2);
  pc.printf("\r\nTime interval: %ld\r\n", time_interval);
  if(numberOfMeasures>2 && time_interval>1){
    pc.printf("Restart board on mode 0\r\n");
    pc.printf("------------------------------------------------------------\r\n");

    mode = 0;
  }else
    pc.printf("Please put valid values (#measures > 2 & T.interval > 1)\r\n");
}
