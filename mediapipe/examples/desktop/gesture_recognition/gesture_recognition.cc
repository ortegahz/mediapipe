#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"

#include "mediapipe/tasks/cc/vision/gesture_recognizer/gesture_recognizer.h"

#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

namespace mediapipe {
namespace tasks {
namespace vision {
namespace gesture_recognizer {

absl::Status Run() {
  LOG(INFO) << "begin ... ";
  auto gesture_rcognizer_options = std::make_unique<GestureRecognizerOptions>();
  gesture_rcognizer_options->base_options.model_asset_path = "./gesture_recognizer.task";
  // auto gesture_rcognizer = GestureRecognizer::Create(std::move(gesture_rcognizer_options));
  // LOG(INFO) << "gesture_rcognizer.ok() --> " << gesture_rcognizer.ok();

  std::unique_ptr<GestureRecognizer> gesture_rcognizer;
  ASSIGN_OR_RETURN(gesture_rcognizer, GestureRecognizer::Create(std::move(gesture_rcognizer_options)));

  cv::Mat image_mat_bgr, image_mat_rgb;
  image_mat_bgr = cv::imread("vlcsnap-2023-02-24-11h30m17s065_rs.bmp");
  cv::cvtColor(image_mat_bgr, image_mat_rgb, cv::COLOR_BGR2RGB);
  mediapipe::ImageFrame image_frame(
      mediapipe::ImageFormat::SRGB, 960, 540,
      image_mat_rgb.step, image_mat_rgb.data, [image_mat_rgb](uint8[]) {});
  mediapipe::Image image(std::make_shared<mediapipe::ImageFrame>(std::move(image_frame)));

  // std::optional<core::ImageProcessingOptions> image_processing_options;
  // auto image_processing_options = std::make_optional<core::ImageProcessingOptions>();
  // core::ImageProcessingOptions image_processing_options;
  absl::StatusOr<GestureRecognizerResult> gesture_recognizer_result;
  ASSIGN_OR_RETURN(gesture_recognizer_result, gesture_rcognizer->Recognize(image));
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->hand_landmarks.at(0).landmark(0).x();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->hand_landmarks.at(0).landmark(0).y();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->hand_landmarks.at(0).landmark(0).z();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification_size();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).label();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).index();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).score();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).has_index();
  LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->handedness.at(0).classification(0).label();
  // const mediapipe::Classification & c = h.classification(0);
  // std::cerr << "label:" << c.label() << "\tindex:" << c.index() << "\n";
  return absl::OkStatus();
}
}  // namespace gesture_recognizer
}  // namespace vision
}  // namespace tasks
}  // namespace mediapipe

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  CHECK(mediapipe::tasks::vision::gesture_recognizer::Run().ok());
  LOG(INFO) << "end ... ";
  return 0;
}
