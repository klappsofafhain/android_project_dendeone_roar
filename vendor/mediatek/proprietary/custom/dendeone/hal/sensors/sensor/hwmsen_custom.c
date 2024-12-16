/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <hardware/sensors.h>
#include <linux/hwmsensor.h>
#include "hwmsen_custom.h"

struct sensor_t sSensorList[MAX_NUM_SENSORS] = 
{
	/*MT6516 the spec follows ADXL345*/
	{  
		.name       = "MT6516 3-axis Accelerometer",
		.vendor     = "The Android Open Source Project",
		.version    = 1,
		.handle     = ID_ACCELEROMETER,
		.type       = SENSOR_TYPE_ACCELEROMETER,
		.maxRange   = 39.22f,
		.resolution = 4.0f/1024.0f,
		.power      = 20.0f/990.0f,
		.reserved   = {}
	},	
	
	{
         .name                   = "LTR559 Proximity Sensor",
         .vendor                 = "LITE-ON TECHNOLOGY CORP.",
         .version                = 1,
         .handle                 = ID_PROXIMITY,
         .type                   = SENSOR_TYPE_PROXIMITY,
         .maxRange               = 5.00f,
		.resolution              = 5.0f,
         .power                  = 3.0f, /* 150 uA */
         .reserved   = {}
                
      },
      
  {
                .name                   = "LTR559 Ambient Light Sensor",
                .vendor                 = "LITE-ON TECHNOLOGY CORP.",
                .version                = 1,
                .handle                 = ID_LIGHT,
                .type                   = SENSOR_TYPE_LIGHT,
                .maxRange               = 32767.0f,
                .resolution             = 1.0f,
                .power                  = 0.1f, /* 100 uA */
                .reserved   = {}
                
        },
        
  
	{ 
		.name       = "AKM09911 3-axis Magnetic Field sensor",
		.vendor     = "Asahi Kasei Microdevices Corporation",
		.version    = 1,
		.handle     = ID_MAGNETIC,
		.type       = SENSOR_TYPE_MAGNETIC_FIELD,
		.maxRange   = 2000.0f,
		.resolution = 0.0625,
		.power      = 6.8,
		.reserved   = {}
},

{ 

         .name       = "BMI160 gyroscope",
		.vendor     = "Bosch",
		.version    = 1,
		.handle     = ID_GYROSCOPE,
		.type       = SENSOR_TYPE_GYROSCOPE,
		.maxRange   = 34.906586,
		.resolution = 0.0010681152,
		.power      = 0.9,
		.reserved   = {}

},

{ 
		.name       = "BMI160 Orientation sensor",
		.vendor     = "Bosch",
		.version    = 1,
		.handle     = ID_ORIENTATION,
		.type       = SENSOR_TYPE_ORIENTATION,
		.maxRange   = 360.0f,
		.resolution = 1.0f,
		.power      = 0,
		.reserved   = {}
},


};

