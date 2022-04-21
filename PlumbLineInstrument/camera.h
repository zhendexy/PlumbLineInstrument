#pragma once

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
//PV_INIT_SIGNAL_HANDLER();
#define BUFFER_COUNT ( 16 )


class Camera
{

public:
	Camera()
	{

	}

	//读取相机数据需要的函数
	bool SelectDevice(PvString *aConnectionID0, PvString *aConnectionID1, PvDeviceInfoType *aType0 = NULL, PvDeviceInfoType *aType1 = NULL);
	PvDevice *ConnectToDevice(const PvString &aConnectionID);
	PvStream *OpenStream(const PvString &aConnectionID);
	void ConfigureStream(PvDevice *aDevice, PvStream *aStream);
	PvPipeline* CreatePipeline(PvDevice *aDevice, PvStream *aStream);
	void AcquireImages(PvDevice *aDevice, PvDevice *bDevice, PvStream *aStream, PvStream *bStream, PvPipeline *aPipeline, PvPipeline *bPipeline);
	void StopAcquireImages(PvDevice *aDevice, PvDevice *bDevice, PvStream *aStream, PvStream *bStream, PvPipeline *aPipeline, PvPipeline *bPipeline);
	void startCamera();
	void stopCamera();


public:
	PvDevice *lDeviceL = NULL, *lDeviceR = NULL;
	PvStream *lStreamL = NULL, *lStreamR = NULL;
	PvPipeline *lPipelineL = NULL, *lPipelineR = NULL;
	PvBuffer *blBuffer = NULL, *alBuffer = NULL;

};