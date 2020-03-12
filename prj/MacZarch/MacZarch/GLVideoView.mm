#import "GLVideoView.h"

#include <memory>
#include <OpenGL/gl.h>
#include <kxm/Zarch/Zarch.h>
#include <kxm/Zarch/ControlsState.h>
#include <kxm/Zarch/Video/OpenGL/RenderTarget.h>

using namespace std;
using namespace kxm::Zarch;

@interface GLVideoView () {
    shared_ptr<Zarch>         _zarch;
    shared_ptr<ControlsState> _controlsState;
    CGSize                    _size;
}
@end

@implementation GLVideoView

- (void)awakeFromNib {
    [super awakeFromNib];
    
    NSOpenGLPixelFormatAttribute attributes[] = { NSOpenGLPFADoubleBuffer,
                                                  NSOpenGLPFADepthSize, 32,
                                                  0
                                                };
    self.pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: attributes];
    [self clearGLContext];
    
    [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(surfaceNeedsUpdate:)
                                          name: NSViewGlobalFrameDidChangeNotification object:self];
}

- (std::shared_ptr<kxm::Zarch::Video::RenderTargetInterface>)getRenderTarget {
    return make_shared<Video::OpenGL::RenderTarget>();
}

- (void)setZarch: (const std::shared_ptr<kxm::Zarch::Zarch> &)aZarch
        controlsState: (const std::shared_ptr<kxm::Zarch::ControlsState> &)aControlsState {
    _zarch         = aZarch;
    _controlsState = aControlsState;
    _size.width  = 0.0f;
    _size.height = 0.0f;
}

- (void)triggerFrame {
    self.needsDisplay = YES;
}

- (void)drawRect: (NSRect)bounds {
    if (_zarch.get()) {
        [self.openGLContext makeCurrentContext];
    
        CGSize newSize = bounds.size;
        if ((newSize.width != _size.width) || (newSize.height != _size.height)) {
            _size.width  = newSize.width;
            _size.height = newSize.height;
            _zarch->SetViewPort((int)(_size.width +.5f), (int)(_size.height + .5f));
        }
        
        _zarch->PrepareFrame(*_controlsState);
        _zarch->RenderFrame();
        
        [self.openGLContext flushBuffer];
    }
}

- (void) surfaceNeedsUpdate: (NSNotification *)notification {
   NSLog(@"surfaceNeedsUpdate");
   [self update];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

/*
- (void)touchesBegan: (NSSet *)touches withEvent: (UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint position = [touch locationInView: self];
        if (position.x < self.frame.size.width / 2.0f) {
            if (!leftTouch) {
                leftTouch = touch;
                controlsState->trigger = true;
            }
        }
        else {
            if (!rightTouch) {
                rightTouch = touch;
                controlsState->thruster = true;
            }
        }
    } 
}

- (void)touchesMoved: (NSSet *)touches withEvent: (UIEvent *)event {
    // Nop.
}

- (void)touchesEnded: (NSSet *)touches withEvent: (UIEvent *)event {
    for (UITouch *touch in touches) {
        if (touch == leftTouch) {
            leftTouch = nil;
            controlsState->trigger = false;
        }
        else if (touch == rightTouch) {
            rightTouch = nil;
            controlsState->thruster = false;
        }
    }
}

- (void)touchesCancelled: (NSSet *)touches withEvent: (UIEvent *)event {
    [self touchesEnded: touches withEvent: event];
}
*/

@end