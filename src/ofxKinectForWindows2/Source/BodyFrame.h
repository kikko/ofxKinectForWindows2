#pragma once

#include "Base.h"
#include "../Utils.h"
#include "ofMain.h"
#include "FaceFrame.h"

namespace ofxKinectForWindows2 {

	enum ProjectionCoordinates {
		DepthCamera,
		ColorCamera
	};

	// -------
	class Joint
	{
	public:
		Joint(){}
		Joint(const _Joint& j, const _JointOrientation& jo) {
			set(j, jo);
		}

		void set(const _Joint& j, const _JointOrientation& jo) {
			joint = j;
			jOrient = jo;
			orientation.set(jo.Orientation.x, jo.Orientation.y, jo.Orientation.z, jo.Orientation.w);
			position.set(j.Position.X, j.Position.Y, j.Position.Z);
			type = j.JointType;
			trackingState = j.TrackingState;
		}

		JointType getType() const {
			return type;
		}

		ofVec3f getPosition() const {
			return position;
		}

		ofVec2f getProjected(ICoordinateMapper * coordinateMapper, ProjectionCoordinates proj = ColorCamera) const {
			switch (proj) {
				case ColorCamera: {
					ColorSpacePoint projected = { 0 };
					coordinateMapper->MapCameraPointToColorSpace(joint.Position, &projected);
					return ofVec2f(projected.X, projected.Y);
				}
				case DepthCamera: {
					DepthSpacePoint projected = { 0 };
					coordinateMapper->MapCameraPointToDepthSpace(joint.Position, &projected);
					return ofVec2f(projected.X, projected.Y);
				}
			}
		}

		ofQuaternion getOrientation() const {
			return orientation;
		}

		TrackingState getTrackingState() const {
			return trackingState;
		}

	protected:
		ofVec3f position;
		ofQuaternion orientation;
		JointType type;
		TrackingState trackingState;

		_Joint joint;
		_JointOrientation jOrient;
	};


	// -------
	class Body {
	public:

		Body::Body();

		void clear();

		void setTrackHands(bool val);
		bool isTrackingHands() const;

		void setTrackFaceProperties(bool val);
		bool isTrackingFaceProperties() const;

		UINT64 trackingId;

		bool tracked;
		map<JointType, Joint> joints;

		HandState leftHandState;
		HandState rightHandState;

		map<FaceProperty, DetectionResult> faceProperties;

	private:
		bool bTrackHands;
		bool bTrackFaceProperties;
	};


	namespace Source {


		// -------
		class BodyFrame : public Base {
		public:
			BodyFrame::~BodyFrame();

			string getTypeName() const override;
			void init(IKinectSensor *) override;
			void update();

			void drawProjected(int x, int y, int width, int height, ProjectionCoordinates proj = ColorCamera);

			IBodyFrameReader * getReader();
			vector<Body> & getBodies();
			const vector< pair<JointType, JointType> > & getBonesDef() const;

			Body * getBodyPtrById(UINT64 trackingId);

			const Vector4 getFloorClipPlane() {
				return floorClipPlane;
			}

			shared_ptr<FaceFrame> faceFrame;

			HandState getLeftHandState(const Body & body);
			HandState getRightHandState(const Body & body);

			ofMatrix4x4 getFloorTransform();

		protected:
			IKinectSensor* sensor;

			void drawProjectedBone(map<JointType, Joint> & pJoints, map<JointType, ofVec2f> & pJointPoints, JointType joint0, JointType joint1);
			void drawProjectedHand(HandState handState, ofVec2f & handPos);

			void initBonesDefinition();

			IBodyFrameReader * reader;
			ICoordinateMapper * coordinateMapper;

			IBody* ppBodies[BODY_COUNT];

			Vector4 floorClipPlane;

			vector<Body> bodies;
			vector< pair<JointType, JointType> > bonesDef;
		};
	}
}