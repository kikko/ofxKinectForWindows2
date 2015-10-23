#pragma once

#include "Joint.h"

#include <Kinect.h>
#include "Kinect.Face.h"

namespace ofxKinectForWindows2 {
	namespace Data {
		class Body {
		public:
			int bodyId;
			int trackingId;
			bool tracked;
      DetectionResult faceProperties[FaceProperty::FaceProperty_Count];
      string getFacePropertiesDebug() const;
			HandState leftHandState;
			HandState rightHandState;
			std::map<JointType, Joint> joints;
			std::map<Activity, DetectionResult> activity;
      ofQuaternion faceOrientation;
      ofRectangle faceBoundingBox;
			void drawWorld();
			void clear();

			static const std::vector<pair<JointType, JointType> > & getBonesAtlas();

		protected:
			static void initBonesAtlas();
			static vector<pair<JointType, JointType> > * bonesAtlas;
		};
	}
}