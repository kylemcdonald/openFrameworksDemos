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

#include "ofxQTKitVideoGrabber.h"
#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>
#import <QuickTime/QuickTime.h>

static inline void argb_to_rgb(unsigned char* src, unsigned char* dst, int numPix)
{
	for(int i = 0; i < numPix; i++){
		memcpy(dst, src+1, 3);
		src+=4;
		dst+=3;
	}	
}

@interface QTKitVideoGrabber : QTCaptureVideoPreviewOutput
{
    QTCaptureSession *session;
	QTCaptureDeviceInput *videoDeviceInput;
    QTCaptureDeviceInput *audioDeviceInput;
    QTCaptureMovieFileOutput *captureMovieFileOutput;
    QTCaptureDevice* selectedVideoDevice;
    QTCaptureDevice* selectedAudioDevice;
	NSInteger videoDeviceID, audioDeviceID;

	NSInteger width, height;
	
	CVImageBufferRef cvFrame;
	ofTexture* texture;
	ofPixels* pixels;	
    
	BOOL isRunning;
    BOOL isRecording;
	BOOL isRecordReady;
	BOOL hasNewFrame;
	BOOL isFrameNew;
	BOOL useTexture;
    BOOL useAudio;
	BOOL verbose;
}

@property(nonatomic, readonly) NSInteger height;
@property(nonatomic, readonly) NSInteger width;
@property(readwrite) NSInteger videoDeviceID;
@property(readwrite) NSInteger audioDeviceID;
@property(retain) QTCaptureSession* session;
@property(nonatomic, retain) QTCaptureDeviceInput* videoDeviceInput;
@property(nonatomic, retain) QTCaptureDeviceInput* audioDeviceInput;
@property(nonatomic, retain) QTCaptureMovieFileOutput *captureMovieFileOutput;
@property(nonatomic, readonly) BOOL isRunning;
@property(readonly) ofPixels* pixels;
@property(readonly) ofTexture* texture;
@property(readonly) BOOL isFrameNew;
@property(readonly) BOOL isRecording;
@property(readonly) BOOL isRecordReady;
@property(nonatomic, readwrite) BOOL verbose;
@property(nonatomic, readwrite) BOOL useTexture;
@property(nonatomic, readwrite) BOOL useAudio;

+ (void) listVideoDevices;
+ (void) listDevices;
+ (void) listAudioDevices;

- (id) initWithWidth:(NSInteger)_width 
              height:(NSInteger)_height
		 videoDevice:(NSInteger)_videoDeviceID
		 audioDevice:(NSInteger)_audioDeviceID
        usingTexture:(BOOL)_useTexture
          usingAudio:(BOOL)_useAudio;

- (void) outputVideoFrame:(CVImageBufferRef)videoFrame 
		 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
		   fromConnection:(QTCaptureConnection *)connection;

- (bool) setSelectedVideoDevice:(QTCaptureDevice *)selectedVideoDevice;
- (bool) setSelectedAudioDevice:(QTCaptureDevice *)selectedAudioDevice;

- (void) setDeviceID:(NSInteger)_deviceID;
- (void) setVideoDeviceID:(NSInteger)_videoDeviceID;
- (void) setAudioDeviceID:(NSInteger)_audioDeviceID;

- (void) initRecording:(NSString*)_selectedVideoCodec audioCodec:(NSString*)_selectedAudioCodec;
+ (void) listVideoCodecs;
+ (void) listAudioCodecs;

- (void) setVideoCodec:(NSString*)_selectedVideoCodec;
- (void) setAudioCodec:(NSString*)_selectedAudioCodec;

- (void) startRecording:(NSString*)filePath;
- (void) stopRecording;

+ (void) enumerateArray:(NSArray*)someArray;
+ (int) getIndexofStringInArray:(NSArray*)someArray stringToFind:(NSString*)someStringDescription;

- (id) initWithoutPreview:(NSInteger)_videoDeviceID 
			  audiodevice:(NSInteger)_audioDeviceID 
			   usingAudio:(BOOL)_useAudio;

- (void) videoSettings;
//- (void) audioSettings;
- (void) startSession;
- (void) update;
- (void) stop;

@end


@implementation QTKitVideoGrabber
@synthesize width, height;
@synthesize videoDeviceID;
@synthesize audioDeviceID;
@synthesize session;
@synthesize videoDeviceInput;
@synthesize audioDeviceInput;
@synthesize captureMovieFileOutput;
@synthesize pixels;
@synthesize texture;
@synthesize isFrameNew;
@synthesize isRecording;
@synthesize isRecordReady;
@synthesize verbose;
@synthesize useTexture;
@synthesize useAudio;

+ (void) enumerateArray:(NSArray*)someArray
{
	NSInteger count = 0;
	for (id object in someArray) 
	{
		NSLog(@"%d - %@", count, [object description]);
		count++;
	}
	NSLog(@"\n");
}

+ (int) getIndexofStringInArray:(NSArray*)someArray stringToFind:(NSString*)someStringDescription
{
	NSInteger count = 0;
	NSInteger index = -1;
	
	for (id object in someArray) 
	{
		if ([[object description] isEqualToString:someStringDescription]) {
			index = count;
			break;
		} else count++;
	}
	
	return index;
}

+ (void) listVideoDevices
{
//	NSLog(@"ofxQTKitVideoGrabber devices %@", 
//		  [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
//		   arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]]);
    
    NSLog(@"ofxQTKitVideoGrabber listing video devices");
	[self enumerateArray:[[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
						  arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]]];
}

+ (void) listDevices
{
    [self listVideoDevices];
}

+ (void) listAudioDevices
{
    //	NSLog(@"ofxQTKitVideoGrabber devices %@", 
    //		  [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
    //		   arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]]);
    
    NSLog(@"ofxQTKitVideoGrabber listing video devices");
	[self enumerateArray:[[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeSound]
						  arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]]];
}


- (id) initWithWidth:(NSInteger)_width 
              height:(NSInteger)_height
		 videoDevice:(NSInteger)_videoDeviceID
		 audioDevice:(NSInteger)_audioDeviceID
        usingTexture:(BOOL)_useTexture
          usingAudio:(BOOL)_useAudio
{
	if(self = [super init]){
		//configure self
		width  = _width;
		height = _height;
        		
		//instance variables
		cvFrame     = NULL;
		hasNewFrame = false;
		texture     = NULL;
		self.useTexture = _useTexture;
        self.useAudio   = _useAudio;
        isRecordReady = NO;
		isRecording   = NO;

		[self setPixelBufferAttributes: [NSDictionary dictionaryWithObjectsAndKeys:
                                         [NSNumber numberWithInt: kCVPixelFormatType_32ARGB], kCVPixelBufferPixelFormatTypeKey,
										 [NSNumber numberWithInt:width], kCVPixelBufferWidthKey, 
										 [NSNumber numberWithInt:height], kCVPixelBufferHeightKey, 
										 [NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey,
										 nil]];
		
		//pixels = (unsigned char*)calloc(sizeof(char), _width*_height*3);
		pixels = new ofPixels();
		pixels->allocate(_width, _height, OF_IMAGE_COLOR);
        
		//init the session
		self.session = [[[QTCaptureSession alloc] init] autorelease];
		
		NSError* error;
		bool success = [self.session addOutput:self error:&error];
        
//        [[[session outputs] objectAtIndex:0] setPixelBufferAttributes: [NSDictionary dictionaryWithObjectsAndKeys:
//                                                                        [NSNumber numberWithInt:480], kCVPixelBufferHeightKey,
//                                                                        [NSNumber numberWithInt:640], kCVPixelBufferWidthKey, nil]];
        
		if( !success ){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error creating capture session");
			return nil;
		}
		
		videoDeviceID = -1;		
		[self setVideoDeviceID:_videoDeviceID];
		
        // if we're using audio add an audio device										[added by gameover]
		if (self.useAudio) {
			audioDeviceID = -1;
			[self setAudioDeviceID:_audioDeviceID];
		}
        
		[self startSession];
	}
	return self;
}


- (id) initWithoutPreview:(NSInteger)_videoDeviceID 
              audiodevice:(NSInteger)_audioDeviceID
               usingAudio:(BOOL)_useAudio
{
	if(self = [super init]){
		//configure self
		width = 0;
		height = 0;
		
		//instance variables
		cvFrame = NULL;
		hasNewFrame = NO;
		texture = NULL;
		self.useTexture = NO;
		self.useAudio = _useAudio;
		isRecordReady = NO;
		isRecording = NO;
		
		//init the session
		self.session = [[[QTCaptureSession alloc] init] autorelease];
		
		NSError* error;
		bool success = [self.session addOutput:self error:&error];
		if( !success ){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error creating capture session");
			return nil;
		}
		
		videoDeviceID = -1;		
		[self setVideoDeviceID:_videoDeviceID];
		
		// if we're using audio add an audio device										[added by gameover]
		if (self.useAudio) {
			audioDeviceID = -1;
			[self setAudioDeviceID:_audioDeviceID];
		}
		
		// give us some info about the 'native format' of our device/s					[added by gameover]
		NSEnumerator *videoConnectionEnumerator = [[videoDeviceInput connections] objectEnumerator];
		QTCaptureConnection *videoConnection;
		
		while ((videoConnection = [videoConnectionEnumerator nextObject])) {
			NSLog(@"Video Input Format: %@\n", [[videoConnection formatDescription] localizedFormatSummary]);
		}
		
		NSEnumerator *audioConnectionEnumerator = [[audioDeviceInput connections] objectEnumerator];
		QTCaptureConnection *audioConnection;
		while ((audioConnection = [audioConnectionEnumerator nextObject])) {
			NSLog(@"Audio Input Format: %@\n", [[audioConnection formatDescription] localizedFormatSummary]);
		}   
		
		[self startSession];
	}
	return self;
}

- (void) startSession
{
	//start the session
	NSLog(@"starting video session");
	[session startRunning];
}

- (void) setDeviceID:(NSInteger)_deviceID
{
    [self setVideoDeviceID:_deviceID];
}

- (void) setVideoDeviceID:(NSInteger)_videoDeviceID
{	
	if(videoDeviceID != _videoDeviceID){
		
		//get video device
		NSArray* videoDevices = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
								 arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]];
		
		ofLog(OF_LOG_VERBOSE, "ofxQTKitVideoGrabber -- Video Device List:  %s", [[videoDevices description] cString]);
		
		// Try to open the new device
		if(_videoDeviceID >= videoDevices.count){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error video device ID out of range");
			return;
		}		
		selectedVideoDevice = [videoDevices objectAtIndex:_videoDeviceID];
		if([self setSelectedVideoDevice:selectedVideoDevice]){
			videoDeviceID = _videoDeviceID;
		}
	}
}

int ofxQTKitVideoGrabber::getVideoDeviceID()
{
	if(confirmInit()){
		return grabber.videoDeviceID;
	}
	return -1;
}

int ofxQTKitVideoGrabber::getAudioDeviceID()
{
	if(confirmInit()){
		return grabber.audioDeviceID;
	}
	return -1;
}

// [added by gameover]
- (void) setAudioDeviceID:(NSInteger)_audioDeviceID
{	
	if(audioDeviceID != _audioDeviceID){
		
		//get audio device
		NSArray* audioDevices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeSound];
		
		ofLog(OF_LOG_VERBOSE, "ofxQTKitVideoGrabber -- Audio Device List:  %s", [[audioDevices description] cString]);
		
		// Try to open the new device
		if(_audioDeviceID >= audioDevices.count){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error audio device ID out of range");
			return;
		}		
		selectedAudioDevice = [audioDevices objectAtIndex:_audioDeviceID];
        
		if([self setSelectedAudioDevice:selectedAudioDevice]){
			audioDeviceID = _audioDeviceID;
		}
	}
}

- (bool) setSelectedVideoDevice:(QTCaptureDevice *)_selectedVideoDevice
{
	BOOL success = YES;	
	if (self.videoDeviceInput) {
		// Remove the old device input from the session and close the device
		[self.session removeInput:videoDeviceInput];
		[[self.videoDeviceInput device] close];
		[videoDeviceInput release];
		videoDeviceInput = nil;
	}
	
	if (_selectedVideoDevice) {
		NSError *error = nil;
		
		// Try to open the new device
		success = [selectedVideoDevice open:&error];
		if(success){
			// Create a device input for the device and add it to the session
			self.videoDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:_selectedVideoDevice];
			
			success = [self.session addInput:self.videoDeviceInput error:&error];
			if(verbose)
                ofLog(OF_LOG_VERBOSE, "ofxQTKitVideoGrabber -- Attached camera %s", [[selectedVideoDevice description] cString]);
		}
	}
	
	if(!success) ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error adding device to session");	
    
	return success;
}

- (void) setVideoCodec:(NSString*)_selectedVideoCodec
{
	// set codec on connection for type Video
	NSArray *outputConnections = [captureMovieFileOutput connections];
	QTCaptureConnection *connection;

	for (connection in outputConnections)
	{
		if ([[connection mediaType] isEqualToString:QTMediaTypeVideo])
        {
			[captureMovieFileOutput setCompressionOptions:[QTCompressionOptions compressionOptionsWithIdentifier:_selectedVideoCodec] forConnection:connection];
        }
	}
}

// [added by gameover]
- (bool) setSelectedAudioDevice:(QTCaptureDevice *)_selectedAudioDevice
{
	BOOL success = YES;	
	if (self.audioDeviceInput) {
		// Remove the old device input from the session and close the device
		[self.session removeInput:audioDeviceInput];
		[[self.audioDeviceInput device] close];
		[audioDeviceInput release];
		audioDeviceInput = nil;
	}
	
	if (_selectedAudioDevice) {
		NSError *error = nil;
		
		// Try to open the new device
		success = [_selectedAudioDevice open:&error];
		if(success){
			// Create a device input for the device and add it to the session
			self.audioDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:_selectedAudioDevice];
			
			success = [self.session addInput:self.audioDeviceInput error:&error];
			ofLog(OF_LOG_VERBOSE, "ofxQTKitVideoGrabber -- Attached audio %s", [[_selectedAudioDevice description] cString]);
		}
	}
	
	if(!success) ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error adding audio device to session");	
	
	return success;
}

- (void) setAudioCodec:(NSString*)_selectedAudioCodec
{
	// set codec on connection for type Sound
	NSArray *outputConnections = [captureMovieFileOutput connections];
	QTCaptureConnection *connection;
	for (connection in outputConnections)
	{
		if ([[connection mediaType] isEqualToString:QTMediaTypeSound])
			[captureMovieFileOutput setCompressionOptions:[QTCompressionOptions compressionOptionsWithIdentifier:_selectedAudioCodec] forConnection:connection];
	}
}

- (void) initRecording:(NSString*)_selectedVideoCodec audioCodec:(NSString*)_selectedAudioCodec
{
	BOOL success = YES;	
	NSError *error = nil;
	
	// Create the movie file output and add it to the session		[added by gameover]
	captureMovieFileOutput = [[QTCaptureMovieFileOutput alloc] init];
	success = [self.session addOutput:captureMovieFileOutput error:&error];
	if (!success) {
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error adding capture output to session");
	} else {
		
        [captureMovieFileOutput setMinimumVideoFrameInterval:1.0/24];
		[self setVideoCodec:_selectedVideoCodec];
        [self setAudioCodec:_selectedAudioCodec];
		
		isRecordReady = YES;
	}
}

+ (void) listVideoCodecs
{
	NSLog(@"ofxQTKitVideoGrabber listing video compression options"); 
	[self enumerateArray:[QTCompressionOptions compressionOptionsIdentifiersForMediaType:QTMediaTypeVideo]];
}

+ (void) listAudioCodecs
{
	NSLog(@"ofxQTKitVideoGrabber listing audio compression options"); 
	[self enumerateArray:[QTCompressionOptions compressionOptionsIdentifiersForMediaType:QTMediaTypeSound]];
}

- (void) startRecording:(NSString*)filePath
{
	if (isRecordReady) {
		
		BOOL success = YES;
		NSError *error = nil;
		
		if (isRecording) [self stopRecording]; // make sure last movie has stopped
		
		// set url for recording
		[captureMovieFileOutput recordToOutputFileURL:[NSURL fileURLWithPath:filePath]];
		
		ofLog(OF_LOG_VERBOSE, "Started recording movie to: %s", [filePath cString]);
		isRecording = YES;
		
	} else {
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Not set up to record - call initRecording first");
	}
}

- (void) stopRecording
{
	if (isRecordReady) {
		
		// set url to nil to stop recording
		[captureMovieFileOutput recordToOutputFileURL:nil];
		
		ofLog(OF_LOG_VERBOSE, "Stopped recording movie");
		
		isRecording = NO;
		
	} else {
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Not set up to record - call initRecording first");
	}
}

//Frame from the camera
//this tends to be fired on a different thread, so keep the work really minimal
- (void) outputVideoFrame:(CVImageBufferRef)videoFrame 
		 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
		   fromConnection:(QTCaptureConnection *)connection
{
	CVImageBufferRef toRelease;	
	@synchronized(self){
		toRelease = cvFrame;
		CVBufferRetain(videoFrame);
		cvFrame = videoFrame;
		hasNewFrame = YES;
		if(toRelease != NULL){
			CVBufferRelease(toRelease);
		}
	}	
}

- (void) setUseTexture:(BOOL)_useTexture
{
	if(_useTexture && texture == NULL){
		texture = new ofTexture();
		texture->allocate(self.width, self.height, GL_RGB);
	}
	useTexture = _useTexture;
}

- (void) update
{
	@synchronized(self){
		if(hasNewFrame){
			CVPixelBufferLockBaseAddress(cvFrame, 0);
			unsigned char* src = (unsigned char*)CVPixelBufferGetBaseAddress(cvFrame);
			
			//I wish this weren't necessary, but
			//in my tests the only performant & reliabile
			//pixel format for QTCapture is k32ARGBPixelFormat, 
			//to my knowledge there is only RGBA format
			//available to gl textures
			
			//convert pixels from ARGB to RGB			
			argb_to_rgb(src, pixels->getPixels(), width*height);
			if(self.useTexture){
				texture->loadData(pixels->getPixels(), width, height, GL_RGB);
			}
			CVPixelBufferUnlockBaseAddress(cvFrame, 0);
			hasNewFrame = NO;
			isFrameNew = YES;
		}
		else{
			isFrameNew = NO;
		}
	}	
}

/**
 * JG:
 * This is experimental and doesn't quite work yet --
 *
 * Bring up the oldschool video setting dialog.
 * It just gets a pointer to the underlying SequenceGrabber
 * component from within QTKit.
 * this doesn't seem to work for all cameras, for example my macbook iSight the pointer is null
 * but it does work with the Macam driver for the PS3Eye which is the important one at the moment
 */
- (void) videoSettings
{
	NSDictionary* attr = [self.videoDeviceInput.device deviceAttributes];
	if (attr == NULL) {
		ofLog(OF_LOG_WARNING, "ofxQTKitVideoGrabber -- Warning: Video Settings not available for this camera");
		return;
	}
	
	NSValue* sgnum = [attr objectForKey:QTCaptureDeviceLegacySequenceGrabberAttribute];
	if (sgnum == NULL) {
		ofLog(OF_LOG_WARNING, "ofxQTKitVideoGrabber -- Warning: Video Settings not available for this camera");
		return;
	}
	
	
	OSErr err;
	SeqGrabComponent sg = (SeqGrabComponent)[sgnum pointerValue];
	SGChannel chan;
	OSType type;
	
	err = SGPause (sg, true);
	if(err){
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Could not pause for video settings");
	}
	
	static SGModalFilterUPP gSeqGrabberModalFilterUPP = NewSGModalFilterUPP(SeqGrabberModalFilterUPP);
	err = SGGetIndChannel(sg, 1, &chan, &type );
	if (err == noErr){
		ComponentResult result = SGSettingsDialog(sg, chan, 0, NULL, 0, gSeqGrabberModalFilterUPP, 0 );
		if(result != noErr){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Error in Sequence Grabber Dialog");
		}
	}
	else{
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Could not init channel");
	}
	
	SGPause(sg, false);
	
}

- (void) stop
{
	if(self.isRunning) {
		[self.session stopRunning];
	}	
	
	self.session = nil;
	
	if(texture != NULL) {
		delete texture;
	}
}

- (BOOL) isRunning
{
	return self.session && self.session.isRunning;
}

@end

//C++ Wrapper class:
ofxQTKitVideoGrabber::ofxQTKitVideoGrabber(){
	videoDeviceID = 0;
	grabber = NULL;
	isInited = false;
	bUseTexture = true;
}

ofxQTKitVideoGrabber::~ofxQTKitVideoGrabber(){
	if(isInited){
		close();		
	}
}

// used to init with no texture or preview etc ie., recording only [added by gameover]
void ofxQTKitVideoGrabber::initGrabber(bool bUseAudio)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    grabber = [[QTKitVideoGrabber alloc] initWithoutPreview:videoDeviceID audiodevice:audioDeviceID usingAudio:bUseAudio];

	isInited = (grabber != nil);
	[pool release];	
}

bool ofxQTKitVideoGrabber::initGrabber(int w, int h){
    return initGrabber(w, h, true);
}
 
bool ofxQTKitVideoGrabber::initGrabber(int w, int h, bool bUseAudio){
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
    grabber = [[QTKitVideoGrabber alloc] initWithWidth:w
                                                height:h 
                                           videoDevice:videoDeviceID
                                           audioDevice:audioDeviceID 
                                          usingTexture:bUseTexture 
                                            usingAudio:bUseAudio];
	
	isInited = (grabber != nil);
	
	[pool release];	
	
	return isInited;
}

// [added by gameover]
void ofxQTKitVideoGrabber::listVideoCodecs() 
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[QTKitVideoGrabber listVideoCodecs];
	[pool release];	
}

// [added by gameover]
void ofxQTKitVideoGrabber::listAudioCodecs() 
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[QTKitVideoGrabber listAudioCodecs];
	[pool release];	
}

void ofxQTKitVideoGrabber::setDeviceID(string _videoDeviceIDString)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	// set array filled with devices
	NSArray* deviceArray = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
							arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]];
	
	// convert device string to NSString
	NSString* deviceIDString = [NSString stringWithUTF8String: _videoDeviceIDString.c_str()];
	
	// find the index of the device name in the array of devices
	videoDeviceID = (NSInteger)[QTKitVideoGrabber getIndexofStringInArray:deviceArray
															 stringToFind:deviceIDString];
	
	if(isInited)[grabber setVideoDeviceID:videoDeviceID];
	[pool release];	
    
}

void ofxQTKitVideoGrabber::setDeviceID(int _videoDeviceID)
{
	
	videoDeviceID = _videoDeviceID;
	
	if(isInited){
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];		// was leaking before 
		[grabber setVideoDeviceID:videoDeviceID];						// (re)sets device correctly now [added by gameover]
		[pool release];	
	}		
}

// [added by gameover]
void ofxQTKitVideoGrabber::setVideoDeviceID(int _videoDeviceID)
{
	setDeviceID(_videoDeviceID);		
}

// [added by gameover]
void ofxQTKitVideoGrabber::setVideoDeviceID(string _videoDeviceIDString)
{
    setDeviceID(_videoDeviceIDString);
}


// [added by gameover]
void ofxQTKitVideoGrabber::setVideoCodec(string _videoCodec) 
{
	videoCodecIDString = _videoCodec;
	
	if(isInited){
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];	
		NSString * NSvideoCodec = [NSString stringWithUTF8String: videoCodecIDString.c_str()];
		[grabber setVideoCodec:NSvideoCodec];
		[pool release];
	}
	
}

// [added by gameover]
void ofxQTKitVideoGrabber::setAudioCodec(string _audioCodec) 
{	
	audioCodecIDString = _audioCodec;
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];	
	NSString * NSaudioCodec = [NSString stringWithUTF8String: audioCodecIDString.c_str()];
	[grabber setVideoCodec:NSaudioCodec];
	[pool release];
}

// [added by gameover]
void ofxQTKitVideoGrabber::initRecording()
{
	if(confirmInit()){
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSString * NSvideoCodec = [NSString stringWithUTF8String: videoCodecIDString.c_str()];
		NSString * NSaudioCodec = [NSString stringWithUTF8String: audioCodecIDString.c_str()];
		[grabber initRecording:NSvideoCodec audioCodec:NSaudioCodec];
		[pool release];	
	}
    
}

// [added by gameover]
void ofxQTKitVideoGrabber::setAudioDeviceID(int _audioDeviceID)
{
	
	audioDeviceID = _audioDeviceID;
	
	if(isInited){
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];		// was leaking before 
		[grabber setAudioDeviceID:audioDeviceID];						// (re)sets device correctly now [added by gameover]
		[pool release];	
	}		
}

// [added by gameover]
void ofxQTKitVideoGrabber::setAudioDeviceID(string _audioDeviceIDString)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	// set array filled with devices
	NSArray* deviceArray = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeSound];
	
	// convert device string to NSString
	NSString* deviceIDString = [NSString stringWithUTF8String: _audioDeviceIDString.c_str()];
	
	// find the index of the device name in the array of devices
	audioDeviceID = (NSInteger)[QTKitVideoGrabber getIndexofStringInArray:deviceArray
															 stringToFind:deviceIDString];
	
	if(isInited) [grabber setAudioDeviceID:audioDeviceID];
	[pool release];	
}

void ofxQTKitVideoGrabber::startRecording(string filePath) 
{
	NSString * NSfilePath = [NSString stringWithUTF8String: filePath.c_str()];
	[grabber startRecording:NSfilePath];
}

void ofxQTKitVideoGrabber::stopRecording() 
{
	[grabber stopRecording];
}

void ofxQTKitVideoGrabber::update(){ 
	grabFrame(); 
}

void ofxQTKitVideoGrabber::grabFrame(){
	if(confirmInit()){
		[grabber update];
	}
}

bool ofxQTKitVideoGrabber::isFrameNew(){
	return isInited && [grabber isFrameNew];
}

bool ofxQTKitVideoGrabber::isRecording()
{
	return isInited && [grabber isRecording];
}

void ofxQTKitVideoGrabber::listVideoDevices(){
	[QTKitVideoGrabber listVideoDevices];	
}

void ofxQTKitVideoGrabber::listAudioDevices() {
    [QTKitVideoGrabber listAudioDevices];
}

void ofxQTKitVideoGrabber::listDevices(){
	[QTKitVideoGrabber listDevices];	
}

void ofxQTKitVideoGrabber::close(){	
	[grabber stop];
	[grabber release];
	isInited = false;	
}

unsigned char* ofxQTKitVideoGrabber::getPixels(){
	if(confirmInit()){
		return [grabber pixels]->getPixels();
	}
	return NULL;
}

ofPixelsRef ofxQTKitVideoGrabber::getPixelsRef(){
	if(confirmInit()){
		return *[grabber pixels];
	}
}

void ofxQTKitVideoGrabber::setUseTexture(bool _bUseTexture){
	if(_bUseTexture != bUseTexture){
		if(isInited){
			grabber.useTexture = _bUseTexture;
		}
		bUseTexture = _bUseTexture;
	}
}

ofTexture&	ofxQTKitVideoGrabber::getTextureReference(){
	if(!bUseTexture){
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Requesting texture while use texture is false");
	}
	if(confirmInit() && bUseTexture){
		return *[grabber texture];
	}
}

void ofxQTKitVideoGrabber::setVerbose(bool bTalkToMe){
	if(confirmInit()){
		grabber.verbose = bTalkToMe;
	}
}

void ofxQTKitVideoGrabber::videoSettings(){
	if(confirmInit()){
		NSLog(@"loading video settings");
		[grabber videoSettings];
	}
}

void ofxQTKitVideoGrabber::draw(float x, float y, float w, float h){
	if(confirmInit()){
		[grabber texture]->draw(x, y, w, h);
	}
}

void ofxQTKitVideoGrabber::draw(float x, float y){
	if(confirmInit()){
		[grabber texture]->draw(x, y);
	}
}

float ofxQTKitVideoGrabber::getHeight(){
	if(confirmInit()){
		return float(grabber.height);
	}
	return 0;
}

float ofxQTKitVideoGrabber::getWidth(){
	if(confirmInit()){
		return float(grabber.width);
	}
	return 0;
	
}

bool ofxQTKitVideoGrabber::confirmInit(){
	if(!isInited){
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- ERROR -- Calling method on non intialized video grabber");
	}
	return isInited;
}

void ofxQTKitVideoGrabber::setPixelFormat(ofPixelFormat _pixelFormat) {
    pixelFormat = _pixelFormat;
}
ofPixelFormat ofxQTKitVideoGrabber::getPixelFormat() {
    return pixelFormat;
}

