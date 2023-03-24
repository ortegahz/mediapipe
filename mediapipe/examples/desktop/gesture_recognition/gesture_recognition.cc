#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"

#include "mediapipe/tasks/cc/vision/gesture_recognizer/gesture_recognizer.h"

#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

#define SHMSIZE (1920 * 1080 * 3)

typedef struct
{
  float x1;
  float y1;
  float x2;
  float y2;
  int index;
  float score;
  char lable[15];
  char hands_r_l[64];
}rrect_t;


union semun
{
  int val;               
  struct semid_ds *buf; 
  unsigned short *array;
  struct seminfo *__buf;
};

int init_semnum_value(int semid, int semnum, int value)
{
    union semun sem;
    sem.val = value;
    if (semctl(semid, semnum, SETVAL, sem) < 0)
    {
      printf("semctl error\n");
    }
    return 0;
}

int sem_init(void)
{
    key_t key;
    int semid;
    if ((key = ftok("/", 'w')) == -1)
    {
      printf("ftok error\n");
    }

    if ((semid = semget(key, 2, IPC_CREAT | IPC_EXCL | 0664)) == -1)
    {
        if(errno == EEXIST){
            semid = semget(key, 2, IPC_CREAT | 0664);
        }else{
            printf("create sem error\n");
        }
    }
    else
    {
        init_semnum_value(semid, 0, 0);
        init_semnum_value(semid, 1, 1);
    }
    return semid;
}

// P操作代表申请资源
int P(int semid, int semnum)
{
    struct sembuf buf;
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0; 

    if (semop(semid, &buf, 1) < 0)
    {
      printf("P error\n");
    }
    return 0; 
}
// V操作代表释放资源
int V(int semid, int semnum)
{
    struct sembuf buf;
    buf.sem_num = semnum;
    buf.sem_op = 1;  
    buf.sem_flg = 0;

    if (semop(semid, &buf, 1) < 0)
    {
      printf("V error\n");
    }
    return 0; 
}

int sem_del(int semid)
{
    // 1.删除信号灯集合
    if (semctl(semid, 0, IPC_RMID, NULL) == -1)
    {
      printf("delete sem error\n");
    }
    return 0;
}




























std::unique_ptr<mediapipe::tasks::vision::gesture_recognizer::GestureRecognizer> gesture_rcognizer;
bool flag = false;
char *addr = NULL;
float temp_x = 0;
float temp_y = 0;
char name[10] = {0};
int count = 1;
static int kkk = 0;
static char bufff[64] = {0};
namespace mediapipe
{
  namespace tasks
  {
    namespace vision
    {
      namespace gesture_recognizer
      {
        absl::Status Run()
        {
          
          if (!flag)
          {
            LOG(INFO) << "begin ... ";
            auto gesture_rcognizer_options = std::make_unique<GestureRecognizerOptions>();
            gesture_rcognizer_options->base_options.model_asset_path = "./gesture_recognizer.task";
            ASSIGN_OR_RETURN(gesture_rcognizer, GestureRecognizer::Create(std::move(gesture_rcognizer_options)));
            flag = true;
          }
          int w = *(int *)(addr);
          int h = *(int *)(addr + 4);
          printf("w = %d  h = %d\n", w, h);
          cv::Mat image_mat = cv::Mat(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
          //cv::Mat image_mat = cv::imread("vlcsnap-2023-02-13-15h11m59s736.png");
          memcpy(image_mat.data, addr + 8, w * h * 3);

          cv::Mat image_rgb;
          cv::cvtColor(image_mat, image_rgb, CV_BGR2RGB);
          // cv::imwrite("./out.jpg", image_mat);
          mediapipe::ImageFrame image_frame(mediapipe::ImageFormat::SRGB, w, h,image_rgb.step, image_rgb.data, [image_rgb](uint8[]) {});
          mediapipe::Image image(std::make_shared<mediapipe::ImageFrame>(std::move(image_frame)));
          absl::StatusOr<GestureRecognizerResult> gesture_recognizer_result;
          ASSIGN_OR_RETURN(gesture_recognizer_result, gesture_rcognizer->Recognize(image));
          int handsize = gesture_recognizer_result->hand_landmarks.size();
          printf("hand size = %d -------------->>>>>>>>>>>> %d \n", handsize, gesture_recognizer_result->hand_landmarks.size());
          memcpy(addr, &handsize, sizeof(handsize));
          for (int i = 0; i < handsize; i++)
          {
            rrect_t rect_temp;
            rect_temp.x1 = (float)1920;
            rect_temp.y1 = (float)1080;
            rect_temp.x2 = (float)0;
            rect_temp.y2 = (float)0;

            for (int j = 0; j < 21; j++)
            {
              temp_x = gesture_recognizer_result->hand_landmarks.at(i).landmark(j).x() * w;
              if (temp_x > rect_temp.x2)
              {
                rect_temp.x2 = temp_x;
              }
              if (temp_x < rect_temp.x1)
              {
                rect_temp.x1 = temp_x;
              }
              temp_y = gesture_recognizer_result->hand_landmarks.at(i).landmark(j).y() * h;
              if (temp_y > rect_temp.y2)
              {
                rect_temp.y2 = temp_y;
              }
              if (temp_y < rect_temp.y1)
              {
                rect_temp.y1 = temp_y;
              }
            }
            //cv::rectangle(image_mat, cv::Point(rect_temp.x1, rect_temp.y1), cv::Point(rect_temp.x2, rect_temp.y2), cv::Scalar(0, 0, 255));
            // sprintf(bufff, "%d.bmp", kkk);
            // cv::imwrite(bufff, image_mat);
            // kkk++;
            // cv::imwrite(bufff, image_mat);
            // kkk++;
            rect_temp.index = gesture_recognizer_result->gestures.at(i).classification(0).index();
            rect_temp.score = gesture_recognizer_result->gestures.at(i).classification(0).score();
            std::string tempp = gesture_recognizer_result->gestures.at(i).classification(0).label();
            memcpy(rect_temp.lable, tempp.c_str(), tempp.length());
            rect_temp.lable[14] = '\0';
            tempp = gesture_recognizer_result->handedness.at(i).classification(0).label();
            memcpy(rect_temp.hands_r_l, tempp.c_str(), tempp.length());
            memcpy(addr+4+sizeof(rect_temp)*i, &rect_temp, sizeof(rect_temp));
            rect_temp.hands_r_l[63] = '\0';
            //rectangle(image_mat, cv::Point((int)rect_temp.x1, (int)rect_temp.y1), cv::Point((int)rect_temp.x2, (int)rect_temp.y2),cv::Scalar(255, 0, 0, 255), 3);
          }
          if (handsize != 0)
          {
            sprintf(name, "./%dp.jpg", count);
            count++;
            //cv::imwrite(name, image_mat);
          }

          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->hand_landmarks.at(0).landmark(0).x();
          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->hand_landmarks.at(0).landmark(0).y();
          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->hand_landmarks.at(0).landmark(0).z();
          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification_size();
          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).label();
          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).index();
          // LOG(INFO) << "gesture_recognizer_result --> " << gesture_recognizer_result->gestures.at(0).classification(0).score();
          return absl::OkStatus();
        }
      } // namespace gesture_recognizer
    }   // namespace vision
  }     // namespace tasks
} // namespace mediapipe








int main(int argc, char **argv)
{
  key_t key = -1;
  int shmid = -1;
  int semid = -1;
  if ((semid = sem_init()) == -1)
  {
    printf("sem create and init error\n");
  }
  init_semnum_value(semid, 0, 0);
  init_semnum_value(semid, 1, 1);
  // 1.获取key值
  key = ftok("/", 'a');
  if (key == -1)
  {
    printf("ftok get key error\n");
  }
  // 2.创建共享内存
  shmid = shmget(key, SHMSIZE, IPC_CREAT | 0664);
  if (shmid == -1)
  {
    printf("shmget create share mem error\n");
  }
  addr = (char *)shmat(shmid, NULL, 0);
  if (addr == (void *)-1)
  {
    printf("shmat mem error\n");
  }
  google::InitGoogleLogging(argv[0]);
  while(1)
  {
    printf("debug ---cjh 2222222222222222222222222\n");
    P(semid, 0);
    printf("debug ---cjh 44444444444444444444444444\n");
    CHECK(mediapipe::tasks::vision::gesture_recognizer::Run().ok());
    LOG(INFO) << "end ... ";
    V(semid, 1);
  }
  return 0;
}
