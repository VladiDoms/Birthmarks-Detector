#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <getopt.h>

void hsv_(const char*);
void canny_(const char*);
void houghTransform_(const char*);

IplImage* image = 0;
IplImage* roi = 0;
IplImage* for_circle = 0;
IplImage* bin = 0;
IplImage* sub = 0;
IplImage* dst = 0;
IplImage* dst2 = 0;

// для хранения каналов HSV
IplImage* hsv = 0;
IplImage* h_plane = 0;
IplImage* s_plane = 0;
IplImage* v_plane = 0;
// для хранения каналов HSV после преобразования
IplImage* h_range = 0;
IplImage* s_range = 0;
IplImage* v_range = 0;
// для хранения суммарной картинки
IplImage* hsv_and = 0;

int Hmin = 0;
int Hmax = 256;

int Smin = 0;
int Smax = 256;

int Vmin = 0;
int Vmax = 256;

int HSVmax = 256;

option long_opts[] =
{
  {"hsv",    no_argument, 0, 'h'},
  {"canny", no_argument, 0, 'c'},
  {"transform",    no_argument, 0, 't'},
  {0,         0,           0, 0},
};

int ind = 0;

struct point
{
  int x;
  int y;
};

void myTrackbarHmin(int pos) {
        Hmin = pos;
        cvInRangeS(h_plane, cvScalar(Hmin), cvScalar(Hmax), h_range);
}

void myTrackbarHmax(int pos) {
        Hmax = pos;
        cvInRangeS(h_plane, cvScalar(Hmin), cvScalar(Hmax), h_range);
}

void myTrackbarSmin(int pos) {
        Smin = pos;
        cvInRangeS(s_plane, cvScalar(Smin), cvScalar(Smax), s_range);
}

void myTrackbarSmax(int pos) {
        Smax = pos;
        cvInRangeS(s_plane, cvScalar(Smin), cvScalar(Smax), s_range);
}

void myTrackbarVmin(int pos) {
        Vmin = pos;
        cvInRangeS(v_plane, cvScalar(Vmin), cvScalar(Vmax), v_range);
}

void myTrackbarVmax(int pos) {
        Vmax = pos;
        cvInRangeS(v_plane, cvScalar(Vmin), cvScalar(Vmax), v_range);
}


int main(int argc, char * argv[])
{
  const char* filename = argv[1];
  printf("[i] image: %s\n", filename);
  int intarg;
  std::string * uniq_opts = new std::string[argc];
  intarg = getopt_long(argc, argv, "hct", long_opts, &ind);
  int i = 0;
  while (intarg != -1)
  {
  switch(intarg)
    {
      case  0  : uniq_opts[i] = long_opts[ind].name;                    i++;      break;
      case 'h' : uniq_opts[i] = "hsv";       hsv_(filename);            i++;      break;
      case 'c' : uniq_opts[i] = "canny";     canny_(filename);          i++;      break;
      case 't' : uniq_opts[i] = "transform"; houghTransform_(filename); i++;      break;
      case '?' : std::cout << "Unexpected option." << std::endl;                  break;
    }
    intarg = getopt_long(argc, argv, "hst", long_opts, &ind);
  }
  return 0;
}

void hsv_(const char* filename)
{
  std::cout << "BLACK :      H-0,   S-0,   V-0" << std::endl;
  std::cout << "RED :        H-0,   S-100, V-100" << std::endl;
  std::cout << "ORANGE :     H-39,  S-100, V-100" << std::endl;
  std::cout << "YELLOW :     H-60,  S-100, V-100" << std::endl;
  std::cout << "GREEN :      H-120, S-100, V-100" << std::endl;
  std::cout << "LIGHT BLUE : H-195, S-100, V-100" << std::endl;
  std::cout << "BLUE :       H-240, S-100, V-100" << std::endl;
  std::cout << "VIOLET :     H-273, S-100, V-100" << std::endl;
  std::cout << "BROWN :      H-30,  S-100, V-59" << std::endl;
  std::cout << "RED :        H-0,   S-0,   V-100" << std::endl;

  image = cvLoadImage(filename, 1);
  assert( image != 0 );
  hsv = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
  h_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  s_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  v_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  h_range = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  s_range = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  v_range = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  hsv_and = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
  //  конвертируем в HSV
  cvCvtColor( image, hsv, CV_BGR2HSV );
  // разбиваем на отельные каналы
  cvSplit( hsv, h_plane, s_plane, v_plane, 0 );

  //
  // определяем минимальное и максимальное значение
  // у каналов HSV
  double framemin=0;
  double framemax=0;

  cvMinMaxLoc(h_plane, &framemin, &framemax);
  printf("[H] %f x %f\n", framemin, framemax );
  Hmin = framemin;
  Hmax = framemax;
  cvMinMaxLoc(s_plane, &framemin, &framemax);
  printf("[S] %f x %f\n", framemin, framemax );
  Smin = framemin;
  Smax = framemax;
  cvMinMaxLoc(v_plane, &framemin, &framemax);
  printf("[V] %f x %f\n", framemin, framemax );
  Vmin = framemin;
  Vmax = framemax;

  // окна для отображения картинки
  cvNamedWindow("original",0);
  cvNamedWindow("H",0);
  cvNamedWindow("S",0);
  cvNamedWindow("V",0);
  cvNamedWindow("H range",0);
  cvNamedWindow("S range",0);
  cvNamedWindow("V range",0);
  cvNamedWindow("hsv and",0);

  cvCreateTrackbar("Hmin", "H range", &Hmin, HSVmax, myTrackbarHmin);
  cvCreateTrackbar("Hmax", "H range", &Hmax, HSVmax, myTrackbarHmax);
  cvCreateTrackbar("Smin", "S range", &Smin, HSVmax, myTrackbarSmin);
  cvCreateTrackbar("Smax", "S range", &Smax, HSVmax, myTrackbarSmax);
  cvCreateTrackbar("Vmin", "V range", &Vmin, HSVmax, myTrackbarVmin);
  cvCreateTrackbar("Vmax", "V range", &Vmax, HSVmax, myTrackbarVmax);

  //
  // разместим окна по рабочему столу
  //
  if(image->width <1920/4 && image->height<1080/2){
          cvMoveWindow("original", 0, 0);
          cvMoveWindow("H", image->width+10, 0);
          cvMoveWindow("S", (image->width+10)*2, 0);
          cvMoveWindow("V", (image->width+10)*3, 0);
          cvMoveWindow("hsv and", 0, image->height+30);
          cvMoveWindow("H range", image->width+10, image->height+30);
          cvMoveWindow("S range", (image->width+10)*2, image->height+30);
          cvMoveWindow("V range", (image->width+10)*3, image->height+30);
  }

  while(true){

          // показываем картинку
          cvShowImage("original",image);

          cvShowImage( "H", h_plane );
          cvShowImage( "S", s_plane );
          cvShowImage( "V", v_plane );

          cvShowImage( "H range", h_range );
          cvShowImage( "S range", s_range );
          cvShowImage( "V range", v_range );

          // складываем
          cvAnd(h_range, s_range, hsv_and);
          cvAnd(hsv_and, v_range, hsv_and);

          cvShowImage( "hsv and", hsv_and );

          char c = cvWaitKey(33);
          if (c == 27) { // если нажата ESC - выходим
                  break;
          }
  }
//Scalar ideal_brown = new Scalar (15,255,150);


  printf("\n[i] Results:\n" );
  printf("[H] %d x %d\n", Hmin, Hmax );
  printf("[S] %d x %d\n", Smin, Smax );
  printf("[V] %d x %d\n", Vmin, Vmax );

  // освобождаем ресурсы
  cvReleaseImage(&image);
  cvReleaseImage(&hsv);
  cvReleaseImage(&h_plane);
  cvReleaseImage(&s_plane);
  cvReleaseImage(&v_plane);
  cvReleaseImage(&h_range);
  cvReleaseImage(&s_range);
  cvReleaseImage(&v_range);
  cvReleaseImage(&hsv_and);
  // удаляем окна
  cvDestroyAllWindows();
}

void canny_(const char* filename)
{
  image = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
  assert( image != 0 );
  cvNamedWindow( "original", 1 );
  cvShowImage( "original", image );

  // получим бинарное изображение
  bin = cvCreateImage( cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
  sub = cvCreateImage( cvSize(bin->width, bin->height), IPL_DEPTH_8U, 1);
  cvCanny(image, bin, 100, 170);

  cvNamedWindow( "bin", 1 );
  cvShowImage( "bin", bin);

  //cvScale(src, dst);
  cvSub(image, bin, sub);
  cvNamedWindow( "sub", 1 );
  cvShowImage( "sub", sub);

  // ждём нажатия клавиши
  cvWaitKey(0);
  cvReleaseImage(&image);
  cvReleaseImage(&bin);
  cvReleaseImage(&sub);
  // удаляем окна
  cvDestroyAllWindows();
}

void houghTransform_(const char* filename)
{
  IplImage* image = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
  IplImage* src = cvLoadImage(filename);
  assert( image != 0 );
  assert( src != 0 );
  // хранилище памяти для кругов
  CvMemStorage* storage = cvCreateMemStorage(0);
  // сглаживаем изображение
  cvSmooth(image, image, CV_GAUSSIAN, 5, 5 );
  // поиск кругов
  CvSeq* results = cvHoughCircles(
          image,
          storage,
          CV_HOUGH_GRADIENT,
          2.5,
          image->width/5,
          100,
          100,
          10,
          image->width/5
          );
  // пробегаемся по кругам и рисуем их на оригинальном изображении
  for( int i = 0; i < results->total; i++ ) {
          float* p = (float*) cvGetSeqElem( results, i );
          CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
          cvCircle( src, pt, cvRound( p[2] ), CV_RGB(0xff,0,0) );
  }
  // показываем
  cvNamedWindow( "cvHoughCircles", 1 );
  cvShowImage( "cvHoughCircles", src);

  int Xc = 0;
  int Yc = 0;
  int counter = 0; // счётчик числа белых пикселей
  point center;

  // пробегаемся по пикселям изображения
  for(int y=0; y<image->height; y++)
  {
          uchar* ptr = (uchar*) (image->imageData + y * image->widthStep);
          for(int x=0; x<image->width; x++)
          {
                  if( ptr[x]>0 )
                  {
                          Xc += x;
                          Yc += y;
                          counter++;
                  }
          }
  }

  //if(counter!=0)
 // {
          center.x = float(Xc)/counter;
          center.y = float(Yc)/counter;
 // }
 std::cout << "center.x = " <<float(Xc)/counter << std::endl;
 std::cout << "center.y = " <<float(Yc)/counter << std::endl;

 IplImage* cropped = cvCreateImage( cvSize(100, 100), src->depth, src->nChannels );
 cvSetImageROI(src, cvRect(center.x-50, center.y-50, 100, 100));
 cvCopy( src, cropped );
  cvShowImage("ROI", src);
  cvSaveImage("ROI.jpg", src, 0);
  // ждём нажатия клавиши
  cvWaitKey(0);
  cvReleaseMemStorage(&storage);
  cvReleaseImage(& image);
  cvReleaseImage(&src);
  cvDestroyAllWindows();
}
