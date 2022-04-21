#include "camera.h"



void Camera::startCamera()
{
	lDeviceL = NULL, lDeviceR = NULL;
	lStreamL = NULL, lStreamR = NULL;

	PV_SAMPLE_INIT();


	PvString lConnectionID0, lConnectionID1;
	if (SelectDevice(&lConnectionID0, &lConnectionID1))
	{
		lDeviceL = ConnectToDevice(lConnectionID0);
		lDeviceR = ConnectToDevice(lConnectionID1);
		if (lDeviceL != NULL&&lDeviceR != NULL)
		{
			lStreamL = OpenStream(lConnectionID0);
			lStreamR = OpenStream(lConnectionID1);
			if (lStreamL != NULL&&lStreamR != NULL)
			{
				lPipelineL = NULL;
				lPipelineR = NULL;

				ConfigureStream(lDeviceL, lStreamL);
				ConfigureStream(lDeviceR, lStreamR);
				lPipelineL = CreatePipeline(lDeviceL, lStreamL);
				lPipelineR = CreatePipeline(lDeviceR, lStreamR);
				if (lPipelineL&&lPipelineR)
					AcquireImages(lDeviceL, lDeviceR, lStreamL, lStreamR, lPipelineL, lPipelineR);
			}
		}
	}
}

void Camera::stopCamera()
{
	//if (!flagStartCamera || flagEndCamera)
	//{
	//	//QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("还没打开相机，无法关闭相机"));
	//	return;
	//}
	//flagEndCamera = false;


	if (lPipelineL&&lPipelineR)
		StopAcquireImages(lDeviceL, lDeviceR, lStreamL, lStreamR, lPipelineL, lPipelineR);;

	delete lPipelineL;
	delete lPipelineR;

	// Close the stream
	cout << "Closing stream" << endl;
	lStreamL->Close();
	lStreamR->Close();
	PvStream::Free(lStreamL);
	PvStream::Free(lStreamR);


	// Disconnect the device
	cout << "Disconnecting device" << endl;
	lDeviceL->Disconnect();
	lDeviceR->Disconnect();
	PvDevice::Free(lDeviceL);
	PvDevice::Free(lDeviceR);



	PV_SAMPLE_TERMINATE();
}

bool Camera::SelectDevice(PvString *aConnectionID0, PvString *aConnectionID1, PvDeviceInfoType *aType0, PvDeviceInfoType *aType1)
{
	PvResult lResult;
	const PvDeviceInfo *lSelectedDI0 = NULL;
	const PvDeviceInfo *lSelectedDI1 = NULL;
	PvSystem lSystem;

	if (gStop)
	{
		return false;
	}
	lSystem.Find();

	// Detect, select device.
	vector<const PvDeviceInfo *> lDIVector;
	for (uint32_t i = 0; i < lSystem.GetInterfaceCount(); i++)
	{
		const PvInterface *lInterface = dynamic_cast<const PvInterface *>(lSystem.GetInterface(i));
		if (lInterface != NULL)
		{
			for (uint32_t j = 0; j < lInterface->GetDeviceCount(); j++)
			{
				const PvDeviceInfo *lDI = dynamic_cast<const PvDeviceInfo *>(lInterface->GetDeviceInfo(j));
				if (lDI != NULL)
					lDIVector.push_back(lDI);
			}
		}
	}

	if (lDIVector.size() == 0)
		return false;


	// Read device selection, optional new IP address.
	PV_DISABLE_SIGNAL_HANDLER();
	PV_ENABLE_SIGNAL_HANDLER();

	//分配左右相机
	lSelectedDI0 = lDIVector[1];
	lSelectedDI1 = lDIVector[0];

	// If the IP Address valid?
	if (lSelectedDI0->IsConfigurationValid() && lSelectedDI1->IsConfigurationValid())
	{
		*aConnectionID0 = lSelectedDI0->GetConnectionID();
		*aConnectionID1 = lSelectedDI1->GetConnectionID();
		if (aType0 != NULL || aType1 != NULL)
		{
			*aType0 = lSelectedDI0->GetType();
			*aType1 = lSelectedDI1->GetType();
		}

		return true;
	}
	return false;
}

PvDevice *Camera::ConnectToDevice(const PvString &aConnectionID)
{
	PvDevice *lDevice;
	PvResult lResult;

	// Connect to the GigE Vision or USB3 Vision device
	lDevice = PvDevice::CreateAndConnect(aConnectionID, &lResult);
	if (lDevice == NULL)
	{
		//QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Unable to connect to device."));
	}

	return lDevice;
}

PvStream *Camera::OpenStream(const PvString &aConnectionID)
{
	PvStream *lStream;
	PvResult lResult;

	// Open stream to the GigE Vision or USB3 Vision device
	lStream = PvStream::CreateAndOpen(aConnectionID, &lResult);
	if (lStream == NULL)
	{
		//QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Unable to stream from device."));
	}

	return lStream;
}

void Camera::ConfigureStream(PvDevice *aDevice, PvStream *aStream)
{
	// If this is a GigE Vision device, configure GigE Vision specific streaming parameters
	PvDeviceGEV* lDeviceGEV = dynamic_cast<PvDeviceGEV *>(aDevice);
	if (lDeviceGEV != NULL)
	{
		PvStreamGEV *lStreamGEV = static_cast<PvStreamGEV *>(aStream);

		// Negotiate packet size
		lDeviceGEV->NegotiatePacketSize();

		// Configure device streaming destination
		lDeviceGEV->SetStreamDestination(lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort());
	}
}

PvPipeline *Camera::CreatePipeline(PvDevice *aDevice, PvStream *aStream)
{
	// Create the PvPipeline object
	PvPipeline* lPipeline = new PvPipeline(aStream);

	if (lPipeline != NULL)
	{
		// Reading payload size from device
		uint32_t lSize = aDevice->GetPayloadSize();

		// Set the Buffer count and the Buffer size
		lPipeline->SetBufferCount(BUFFER_COUNT);
		lPipeline->SetBufferSize(lSize);
	}

	return lPipeline;
}

void Camera::AcquireImages(PvDevice *aDevice, PvDevice *bDevice, PvStream *aStream, PvStream *bStream, PvPipeline *aPipeline, PvPipeline *bPipeline)
{
	// Get device parameters need to control streaming
	PvGenParameterArray *alDeviceParams = aDevice->GetParameters();
	PvGenParameterArray *blDeviceParams = bDevice->GetParameters();

	// Map the GenICam AcquisitionStart and AcquisitionStop commands
	PvGenCommand *alStart = dynamic_cast<PvGenCommand *>(alDeviceParams->Get("AcquisitionStart"));
	PvGenCommand *blStart = dynamic_cast<PvGenCommand *>(blDeviceParams->Get("AcquisitionStart"));

	// Note: the pipeline must be initialized before we start acquisition
	// std::cout << "Starting pipeline" << endl;
	aPipeline->Start();
	bPipeline->Start();

	// Get stream parameters
	PvGenParameterArray *alStreamParams = aStream->GetParameters();
	PvGenParameterArray *blStreamParams = bStream->GetParameters();

	// Map a few GenICam stream stats counters
	PvGenFloat *alFrameRate = dynamic_cast<PvGenFloat *>(alStreamParams->Get("AcquisitionRate"));
	PvGenFloat *alBandwidth = dynamic_cast<PvGenFloat *>(alStreamParams->Get("Bandwidth"));
	PvGenFloat *blFrameRate = dynamic_cast<PvGenFloat *>(blStreamParams->Get("AcquisitionRate"));
	PvGenFloat *blBandwidth = dynamic_cast<PvGenFloat *>(blStreamParams->Get("Bandwidth"));

	// Enable streaming and send the AcquisitionStart command
	// cout << "Enabling streaming and sending AcquisitionStart command." << endl;
	aDevice->StreamEnable();
	bDevice->StreamEnable();
	alStart->Execute();
	blStart->Execute();
}

void Camera::StopAcquireImages(PvDevice *aDevice, PvDevice *bDevice, PvStream *aStream, PvStream *bStream, PvPipeline *aPipeline, PvPipeline *bPipeline)
{
	// Get device parameters need to control streaming
	PvGenParameterArray *alDeviceParams = aDevice->GetParameters();
	PvGenParameterArray *blDeviceParams = bDevice->GetParameters();

	// Map the GenICam AcquisitionStart and AcquisitionStop commands
	PvGenCommand *alStop = dynamic_cast<PvGenCommand *>(alDeviceParams->Get("AcquisitionStop"));
	PvGenCommand *blStop = dynamic_cast<PvGenCommand *>(blDeviceParams->Get("AcquisitionStop"));


	// Tell the device to stop sending images.
	// cout << "Sending AcquisitionStop command to the device" << endl;
	alStop->Execute();
	blStop->Execute();

	// Disable streaming on the device
	// cout << "Disable streaming on the controller." << endl;
	aDevice->StreamDisable();
	bDevice->StreamDisable();

	// Stop the pipeline
	// cout << "Stop pipeline" << endl;
	aPipeline->Stop();
	bPipeline->Stop();
}