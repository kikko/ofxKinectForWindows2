#pragma once

#include "Base.h"
#include "../Utils.h"
#include "ofMain.h"
#include <Kinect.VisualGestureBuilder.h>

namespace ofxKinectForWindows2 {

	namespace Source {

		class Gesture {
		public:
			Gesture() :
				gesture(NULL),
				confidence(0),
				detected(false),
				firstFrameDetected(false),
				progress(0){}

			// set during init
			string name;
			GestureType type;
			IGesture* gesture;

			// discrete
			float confidence;
			BOOLEAN detected;
			BOOLEAN firstFrameDetected;

			// continuous
			float progress;
		};

		// -------
		class VGB {
		public:

			VGB::VGB();
			VGB::~VGB();

			string getTypeName() const;

			void init(IKinectSensor *, string gbd, UINT64 bodyId = 0);
			void update();

			string getDebugString();

			UINT64 getBodyId(){ return bodyId; }
			void setBodyId(UINT64 bodyId);

			const vector<shared_ptr<Gesture>> & getGestures() {
				return gesturesVec;
			}

			IVisualGestureBuilderFrameReader * getReader() {
				return reader;
			};
			IVisualGestureBuilderFrameSource * getSource() {
				return source;
			};
			IVisualGestureBuilderDatabase * getDatabase() {
				return vgbd;
			};

		protected:
			UINT64 bodyId;

			vector<shared_ptr<Gesture>> gesturesVec;

			BOOLEAN bTrackingIdValid;

			UINT numGestures;
			IGesture** gestures;
			IVisualGestureBuilderDatabase * vgbd;
			IVisualGestureBuilderFrameSource * source;
			IVisualGestureBuilderFrameReader * reader;
		};
	}
}