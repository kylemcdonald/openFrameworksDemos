/*
 * ofxQTKitVideoGrabber.cpp
 *
 * Copyright 2010 (c) James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxQTKitVideoGrabber works exactly the same as the standard ofMovieGrabber
 * but uses the QTKit Objective-C Libraries to drive the video display.
 * These libraries are naturally GPU enabled, multi-threaded, as well
 * as supporting more Quicktime capture codecs such as HDV.
 *
 * You will need to add the QTKit.framework and CoreVide.framework
 * to the openFrameworks Xcode project
 *
 * Requires Mac OS 10.5 or greater
 */

#ifndef _OFX_QTKITVIDEOGRABBER
#define _OFX_QTKITVIDEOGRABBER

#include "ofMain.h"

//using this #ifdef lets this .h file be included in cpp files without throwing errors
//but when included in .m files it works correctly.  another useful trick for mixing oF/ObjC
#ifdef __OBJC__
@class QTKitVideoGrabber;
#endif

class ofxQTKitVideoGrabber : public ofBaseVideoGrabber
{
  public:
	ofxQTKitVideoGrabber();
	~ofxQTKitVideoGrabber();

    bool			initGrabber(int w, int h);
	bool			initGrabber(int w, int h, bool bUseAudio = false);
	void			grabFrame();
	bool			isFrameNew();
	void			update();
	void 			setUseTexture(bool bUse);
	
	void 			listVideoDevices();
    void            listDevices();
    void            listAudioDevices();
    
	void			close();
	unsigned char	*getPixels();
	ofPixelsRef		getPixelsRef();
	
	ofTexture &		getTextureReference();
	void 			setVerbose(bool bTalkToMe);
    void            setDeviceID(int deviceID);
    void            setDeviceID(string deviceIDString);
    void			setVideoDeviceID(int videoDeviceID);
	void			setVideoDeviceID(string videoDeviceIDString);
    int				getVideoDeviceID();
    void			setAudioDeviceID(int audioDeviceID);
	void			setAudioDeviceID(string audioDeviceIDString);
	int				getAudioDeviceID();
    
	void			setDesiredFrameRate(int framerate){ ofLog(OF_LOG_WARNING, "ofxQTKitVideoGrabber -- Cannot specify framerate.");  };
	void			videoSettings();
//    void            audioSettings();

    void			initRecording();
	void			initGrabber(bool bUseAudio = true);	// used to init with no preview/textures etc
	void			listVideoCodecs();
    void			listAudioCodecs();
	void			setVideoCodec(string videoCodecIDString);
    void			setAudioCodec(string audioCodecIDString);

	void			startRecording(string filePath);
	void			stopRecording();
	bool			isRecording();
        
	void 			draw(float x, float y, float w, float h);
	void 			draw(float x, float y);
	
	float 			getHeight();
	float 			getWidth();
    
    void setPixelFormat(ofPixelFormat pixelFormat);
	ofPixelFormat getPixelFormat();

  protected:

	bool    confirmInit();
    int		videoDeviceID;
    int     audioDeviceID;
    string  videoCodecIDString;
    string  audioCodecIDString;
    bool    useAudio;
	bool    isInited;
	bool    bUseTexture;
    ofPixelFormat pixelFormat;
    
	#ifdef __OBJC__
	QTKitVideoGrabber* grabber; //only obj-c needs to know the type of this protected var
	#else
	void* grabber;
	#endif
};

#endif