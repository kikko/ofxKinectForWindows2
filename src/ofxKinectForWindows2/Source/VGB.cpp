#include "VGB.h"
#include "ofMain.h"

#define CHECK_OPEN if(!this->reader) { OFXKINECTFORWINDOWS2_ERROR << "Failed : Reader is not open"; }

std::string convertWideToNarrow(const wchar_t *s, char dfault = '?',
	const std::locale& loc = std::locale())
{
	std::ostringstream stm;
	while (*s != L'\0') {
		stm << std::use_facet< std::ctype<wchar_t> >(loc).narrow(*s++, dfault);
	}
	return stm.str();
}


namespace ofxKinectForWindows2 {


	namespace Source {

		VGB::VGB() :
			gestures(NULL),
			vgbd(NULL),
			source(NULL),
			reader(NULL),
			bodyId(0){}

		VGB::~VGB(){
			SafeRelease(this->reader);
			SafeRelease(this->source);
			delete gestures;
		}

		//----------
		string VGB::getTypeName() const {
			return "VGBFrame";
		}

		void VGB::setBodyId(UINT64 bodyId) {
			try {
				if (FAILED(source->put_TrackingId(bodyId))){
					throw Exception("Failed to put tracking Id");
				}
				this->bodyId = bodyId;
				ofLogVerbose("VGB") << "new id set";
			}
			catch (std::exception & e){
				throw e;
			}
		}

		//----------
		void VGB::init(IKinectSensor * sensor, string gbd, UINT64 bodyId) {
			this->reader = NULL;
			this->bodyId = bodyId;
			try {

				if (FAILED(CreateVisualGestureBuilderFrameSource(sensor, bodyId, &source))) {
					throw Exception("Failed to initialise VGBFrame source");
				}

				if (FAILED(source->OpenReader(&reader))) {
					throw Exception("Failed to initialise VGBFrame reader");
				}

				string path = ofToDataPath(gbd, true);
				wstring wpath = wstring(path.begin(), path.end());
				if (FAILED(CreateVisualGestureBuilderDatabaseInstanceFromFile(wpath.c_str(), &vgbd))){
					throw Exception("Failed to load VGB database");
				}

				if (FAILED(vgbd->get_AvailableGesturesCount(&numGestures))){
					throw Exception("Failed to retrieve gestures count");
				}
				
				gestures = new IGesture*[numGestures];
				if (FAILED(vgbd->get_AvailableGestures(numGestures, gestures))){
					throw Exception("Failed to retrieve gestures");
				}
				
				for (int i = 0; i < numGestures; i++){

					GestureType gtype;
					HRESULT hr = gestures[i]->get_GestureType(&gtype);
					if (hr < 0){
						throw Exception("failed to retrieve gesture type");
					}

					wchar_t gname[512];
					hr = gestures[i]->get_Name(_countof(gname), gname);
					if (hr < 0){
						throw Exception("failed to retrieve gesture name");
					}
					string name = convertWideToNarrow(gname);

					string typeS = (gtype == GestureType_Discrete ? +"discrete" : "continuous");
					ofLogVerbose("VGB") << "adding " + typeS + " gesture " + name;

					shared_ptr<Gesture> gst = shared_ptr<Gesture>(new Gesture);
					gst->gesture = gestures[i];
					gst->name = name;
					gst->type = gtype;
					gesturesVec.push_back(gst);

					source->AddGesture(gestures[i]);
				}
				
			}
			catch (std::exception & e) {
				SafeRelease(this->reader);
				SafeRelease(this->source);
				throw (e);
			}
		}


		//----------
		void VGB::update() {
			CHECK_OPEN

			if (bodyId == 0) return;

			IVisualGestureBuilderFrame * frame = NULL;
			IDiscreteGestureResult * dgResult = NULL;
			IContinuousGestureResult * cgResult = NULL;
			try {
				
				if (FAILED(reader->CalculateAndAcquireLatestFrame(&frame))) {
					return;
				}

				BOOLEAN bTrackIdOk = false;
				if (FAILED(frame->get_IsTrackingIdValid(&bTrackIdOk))) {
					throw Exception("failed to retrieve tracking id validity");
				}

				if (!bTrackIdOk) {
					throw Exception("tracking id invalid");
				}

				for (int i = 0; i < gesturesVec.size(); i++)
				{
					auto & g = gesturesVec[i];
					switch (g->type)
					{
						case GestureType_Discrete:{
							if (FAILED(frame->get_DiscreteGestureResult(g->gesture, &dgResult))){
								throw Exception("Failed to refresh discrete gesture");
							}
							if (dgResult != NULL) {
								if (FAILED(dgResult->get_Detected(&g->detected)) ||
									FAILED(dgResult->get_Confidence(&g->confidence)) ||
									FAILED(dgResult->get_FirstFrameDetected(&g->firstFrameDetected))){
									throw Exception("Failed to update discrete gesture result");
								}
							}
							break;
						}
						case GestureType_Continuous:{
							if (FAILED(frame->get_ContinuousGestureResult(g->gesture, &cgResult))){
								throw Exception("Failed to refresh continuous gesture");
							}
							if (cgResult != NULL) {
								if (FAILED(cgResult->get_Progress(&g->progress))){
									throw Exception("Failed to update continuous gesture result");
								}
							}
							break;
						}
					}
				}
			}
			catch (std::exception & e) {
				OFXKINECTFORWINDOWS2_ERROR << e.what();
			}
			SafeRelease(dgResult);
			SafeRelease(frame);
		}


		//----------
		string VGB::getDebugString(){
			string buff;
			for (auto & g : gesturesVec) {
				if (g->type == GestureType_Discrete){
					buff += g->name + " (Disc.) : " + (g->detected ? "TRUE" : "FALSE") + " (" + ofToString(g->confidence, 3) + ")\n";
				}
				else {
					buff += g->name + " (Cont.) : " + ofToString(g->progress, 3) + ")\n";
				}
			}
			return buff;
		}
	}
}