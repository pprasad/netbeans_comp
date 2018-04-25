#include <stdio.h>
#include <iostream>
#include <fstream>
#define WM_CAP_START  0x0400
#define WM_CAP_DRIVER_CONNECT  (WM_CAP_START + 10)
#define WM_CAP_DRIVER_DISCONNECT  (WM_CAP_START + 11)
#define WM_CAP_EDIT_COPY (WM_CAP_START + 30)
#define WM_CAP_GRAB_FRAME (WM_CAP_START + 60)
#define WM_CAP_SET_SCALE (WM_CAP_START + 53)
#define WM_CAP_SET_PREVIEWRATE (WM_CAP_START + 52)
#define WM_CAP_SET_PREVIEW (WM_CAP_START + 50)
#define WM_CAP_DLG_VIDEOSOURCE  (WM_CAP_START + 42)
#define WM_CAP_STOP (WM_CAP_START+ 68)
#define WM_CAP_FILE_SET_CAPTURE_FILE	(WM_CAP_START+ 20)
#define WM_CAP_FILE_SAVEDIB		(WM_CAP_START+  25)
#define WM_CAP_SEQUENCE 		(WM_CAP_START + 62)
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <winbase.h>
#include <signal.h>
#include <assert.h>
#include <vfw.h>
#include <vector>
#include "webcame.h"
using namespace std;
/*WINAPI typedef*/
#define VFWAPI WINAPI
typedef BOOL (VFWAPI *DEVICES)(WORD,LPTSTR,int,LPTSTR,int);
typedef HWND (VFWAPI *CAMVI)(LPCWSTR,DWORD,int,int,int,int,HWND,int);
LPCSTR path="avicap32.dll";
HINSTANCE hInstance=NULL;
/*Variable Declarations*/
vector<string> deviceList;
//JNIEnv *cur_env;
int frameNum=0;
JNIEXPORT jobject JNICALL Java_WebCam_getDevices(JNIEnv *env, jobject jObj){
	     char deviceName[100];
         char version[100];
         WORD  wDriverIndex=0;
	     jclass arrClass = env->FindClass("java/util/ArrayList");
	     jmethodID initArr=env->GetMethodID(arrClass,"<init>","()V");
	     jmethodID arrSize = env->GetMethodID (arrClass, "size", "()I");
	     jobject arr = env->NewObject(arrClass, initArr);
	     jmethodID arrAdd = env->GetMethodID(arrClass, "add", "(Ljava/lang/Object;)Z");
	     DEVICES device;
	     hInstance=LoadLibrary(path);
	     if(hInstance!=NULL){
		 	 /*Creating GetDriverDescription Instance*/
		 	 device=(DEVICES)GetProcAddress(hInstance,"capGetDriverDescriptionA");
		 	 while(device(wDriverIndex,deviceName,sizeof(deviceName),version,sizeof(version))){
		         deviceList.push_back(deviceName);
		         env->CallBooleanMethod(arr,arrAdd,env->NewStringUTF(deviceName));
		         wDriverIndex+=1;
	       	}
		 }    	
		 return arr;     
  }
JNIEXPORT jint JNICALL Java_WebCam_connect(JNIEnv *env, jobject jObj, jstring paramString,jobject comp){
    //cur_env = env;
	int flag=0;
	int dev_id=0;
	jboolean result;
	jobject jhandle;
	CAMVI camvi=NULL;
    HWND camhwnd=NULL;
	DWORD wSize;
	char szAppName [] = TEXT("Motion Tracker");
	const char *devId=env->GetStringUTFChars(paramString,0);
	//registerWndProc();
	camvi=(CAMVI)GetProcAddress(hInstance,"capCreateCaptureWindowA");
	if(camvi!=NULL){   //GetDesktopWindow()
		camhwnd=camvi((LPCWSTR) "Web Cam Caputer App",WS_CHILD | WS_VISIBLE,0,0,200,200,HWND_MESSAGE,0);
		if(camhwnd!=NULL){
			 for(int i=0;i<deviceList.size();i++){
		    	 if(deviceList[i]==devId){
		    	 	  flag=1;
		    	 	  dev_id=i;
		    	      break;	
				  }
			}
			if(flag){
			  	capDriverConnect(camhwnd,dev_id);
			    /*Frame Rate */ 
			     CAPTUREPARMS captureParms;
			     float FramesPerSec=30;
			     capCaptureGetSetup(camhwnd, &captureParms, sizeof(CAPTUREPARMS));
                 captureParms.dwRequestMicroSecPerFrame = (DWORD) (1.0e6 / FramesPerSec);
                 capCaptureSetSetup(camhwnd, &captureParms, sizeof (CAPTUREPARMS));
                 // OPTIONAL STEP: Setup resolution
                 BITMAPINFO psVideoFormat;
                capGetVideoFormat(camhwnd, &psVideoFormat,sizeof(psVideoFormat));
                psVideoFormat.bmiHeader.biWidth = 640;
                psVideoFormat.bmiHeader.biHeight = 480;
                capSetVideoFormat(camhwnd, &psVideoFormat,sizeof(psVideoFormat));
                capDriverDisconnect(camhwnd);
                capDriverConnect (camhwnd,dev_id);
                //STEP 3: Setup the preview window size 
                CAPSTATUS CapStatus;
                capGetStatus(camhwnd, &CapStatus, sizeof (CAPSTATUS)); 
                //capSetCallbackOnFrame(camhwnd,capVideoStreamCallback);
                ::SetWindowPos(camhwnd, NULL, 0, 0, CapStatus.uiImageWidth, CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE); 
                //STEP 4: Previewing video
                capPreview(camhwnd, TRUE);       // start preview 
                capPreviewRate(camhwnd,33);     // rate, in milliseconds, the frame rate here is just for preview window, not for capturing
                capPreviewScale(camhwnd,TRUE);
            }
	  }
	}
   return (jlong)camhwnd;
}  

JNIEXPORT void JNICALL Java_WebCam_clipBoard(JNIEnv *env, jobject Jobj,jint hwnd){
	capGrabFrameNoStop((HWND)hwnd);
	capEditCopy((HWND)hwnd);
}
JNIEXPORT jbyteArray JNICALL Java_WebCam_getImage(JNIEnv *env, jobject Jobj,jint hwnd){
    std::string filename = "frame.bmp";
    /*C:\\Users\\umprasad\\Pictures\\Saved Pictures\\*/
	capGrabFrame((HWND)hwnd);
    capFileSaveDIB((HWND)hwnd,filename.data());
    ifstream fl(filename.data());
    fl.seekg(0, ios::end );
    size_t len = fl.tellg();
    char *ret = new char[len];
    fl.seekg(0, ios::beg);
    fl.read(ret, len);
    fl.close();
    jbyteArray result = env->NewByteArray( len);
    env->SetByteArrayRegion( result, 0, len, (const jbyte*)ret );
    delete[] ret;
    return result;
}
JNIEXPORT void JNICALL Java_WebCam_disConnect(JNIEnv *, jobject Jobj,jint hwnd){
	//capCaptureStop((HWND)hwnd);
	capDriverDisconnect((HWND)hwnd);
}
