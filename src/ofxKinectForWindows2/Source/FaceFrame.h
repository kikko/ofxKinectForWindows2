#pragma once

#include "Base.h"
#include "../Utils.h"
#include "ofMain.h"
#include <Kinect.Face.h>

namespace ofxKinectForWindows2 {

	namespace Source {

		// -------
		class FaceFrame /*: public Base*/ {
		public:

			FaceFrame::~FaceFrame();

			string getTypeName() const;
			void init(IKinectSensor *, UINT64 bodyId);
			void update();

			UINT64 getBodyId(){ return bodyId; }
			IFaceFrameReader * getReader();

			map<FaceProperty, DetectionResult> getProperties() {
				return properties;
			}

			string getDebugString();

		protected:
			UINT64 bodyId;

			map<FaceProperty, DetectionResult> properties;

			IFaceFrameReader * reader;
			DetectionResult detectionResults[FaceProperty_Count];
		};
	}
}