//Original source code found at : http://developer.android.com/reference/android/app/NativeActivity.html

#ifndef _OS_ANDROID_
#error "Not supported platform!!"
#endif

#include <stdio.h>
#include <android/sensor.h>

#define MAX_LOOP_COUNT 50
#define SAMP_PER_SEC 5
#define LOOPER_ID_USER 3

void PrintSensorInformation(const ASensor* sensor)
{
	/*
	* Returns this sensor's name (non localized)
	*/
	const char* name = ASensor_getName(sensor);

	/*
	* Returns this sensor's vendor's name (non localized)
	*/
	const char* vendor = ASensor_getVendor(sensor);

	/*
	* Return this sensor's type
	*/
	int type = ASensor_getType(sensor);

	/*
	* Returns this sensors's resolution
	*/
	float resolution = ASensor_getResolution(sensor);

	/*
	* Returns the minimum delay allowed between events in microseconds.
	* A value of zero means that this sensor doesn't report events at a
	* constant rate, but rather only when a new data is available.
	*/
	int minimumDelay = ASensor_getMinDelay(sensor);

	printf("name=%s\nvendor=%s\ntype=%d\nresolution=%f\nminimumDelay=%d\n",
		name, vendor, type, resolution, minimumDelay);
}

int main()
{
	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	// Prepare to monitor accelerometer
	sensorManager = ASensorManager_getInstance();

	accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);

	PrintSensorInformation(accelerometerSensor);

	ALooper *looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

	sensorEventQueue = ASensorManager_createEventQueue(sensorManager,
		looper, LOOPER_ID_USER, NULL, NULL);

	int err;
	if ((err = ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor)) < 0)
		printf("ASensorEventQueue_enableSensor() failed. err=%d.\n", err);
	if ((err = ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L / SAMP_PER_SEC) * 1000)) < 0)
		printf("ASensorEventQueue_setEventRate failed. err=%d\n", err);

	// Read all pending events.
	int ident;
	int events;
	int loopCount = 0;

	// we will block forever waiting for events.
	while (1)
	{
		ident = ALooper_pollAll(100, NULL, &events, NULL);

		// If a sensor has data, process it now.
		if (ident == LOOPER_ID_USER)
		{
			if (accelerometerSensor != NULL)
			{
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0)
				{
					printf("acc : x=%.2f y=%.2f z=%.2f\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);

					loopCount++;
				}
			}
		}

		if (loopCount > MAX_LOOP_COUNT)
			break;
	}

	return 0;
}