#include "FaceFrame.h"
#include "ofMain.h"

#define CHECK_OPEN if(!this->reader) { OFXKINECTFORWINDOWS2_ERROR << "Failed : Reader is not open"; }

namespace ofxKinectForWindows2 {


	namespace Source {

		FaceFrame::~FaceFrame(){
			SafeRelease(this->reader);
		}

		//----------
		string FaceFrame::getTypeName() const {
			return "FaceFrame";
		}

		//----------
		void FaceFrame::init(IKinectSensor * sensor, UINT64 bodyId) {
			this->reader = NULL;
			this->bodyId = bodyId;
			try {

				const DWORD features = 
					/*FaceFrameFeatures_BoundingBoxInInfraredSpace
					| FaceFrameFeatures_PointsInInfraredSpace
					| FaceFrameFeatures_BoundingBoxInColorSpace
					| FaceFrameFeatures_PointsInColorSpace
					| FaceFrameFeatures_RotationOrientation
					| */FaceFrameFeatures_Happy
					| FaceFrameFeatures_RightEyeClosed
					| FaceFrameFeatures_LeftEyeClosed
					| FaceFrameFeatures_MouthOpen
					| FaceFrameFeatures_MouthMoved
					| FaceFrameFeatures_LookingAway
					| FaceFrameFeatures_Glasses
					| FaceFrameFeatures_FaceEngagement;

				IFaceFrameSource* source = NULL;

				if (FAILED(CreateFaceFrameSource(sensor, bodyId, features, &source))) {
					throw Exception("Failed to initialise FaceFrame source");
				}

				if (FAILED(source->OpenReader(&reader))) {
					throw Exception("Failed to initialise FaceFrame reader");
				}

				SafeRelease(source);
			}
			catch (std::exception & e) {
				SafeRelease(this->reader);
				throw (e);
			}
		}


		//----------
		void FaceFrame::update() {
			CHECK_OPEN

			IFaceFrame * frame = NULL;
			IFaceFrameResult * frameResult = NULL;
			try {
				
				if (FAILED(this->reader->AcquireLatestFrame(&frame))) {
					return; // we often throw here when no new frame is available
				}

				if (FAILED(frame->get_FaceFrameResult(&frameResult))){
					throw Exception("Failed to refresh face data");
				}

				if (frameResult != NULL) {
					if (FAILED(frameResult->GetFaceProperties(FaceProperty_Count, detectionResults))){
						throw Exception("Failed to get face properties");
					}

					for (int i = 0; i < FaceProperty_Count; i++) {
						properties[(FaceProperty)i] = detectionResults[i];
					}
				}
			}
			catch (std::exception & e) {
				OFXKINECTFORWINDOWS2_ERROR << e.what();
			}
			SafeRelease(frameResult);
			SafeRelease(frame);
		}


		//----------
		string FaceFrame::getDebugString(){
			string buff;
			string resultStr[] = { "unknown", "no", "maybe", "yes" };
			string props[] = {
				"Happy",
				"Engaged",
				"WearingGlasses",
				"LeftEyeClosed",
				"RightEyeClosed",
				"MouthOpen",
				"MouthMoved",
				"LookingAway" };
			for (int i = 0; i < FaceProperty_Count; i++)
			{
				string prop = resultStr[detectionResults[i]];
				buff += props[i] + ": " + prop + "\n";
			}
			return buff;
		}
	}
}