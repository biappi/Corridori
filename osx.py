from AppKit import *
from Quartz import *

from corridori import TimeRunners, STILL, LEFT, RIGHT, TOP, BOTTOM, NO_RUNNING, RUNNING

from PyObjCTools import AppHelper

def create_image_and_data(width, height):
    data = NSMutableData.alloc().initWithLength_(width * height  * 5)

    image = CGImageCreate(
        width,
        height,
        8,
        32,
        width * 4,
        CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB),
        kCGImageAlphaFirst,
        CGDataProviderCreateWithCFData(data),
        None,
        False,
        kCGRenderingIntentDefault
    )

    return image, data

class ImageView(NSView):
    def drawRect_(self, rect):
        if not self.tr:
            return

        c = NSGraphicsContext.currentContext().graphicsPort()
        CGContextScaleCTM(c, 3, 3)

        def blit(image, pos, flip=False, bottom_left=False):
            size = image.size()
            if bottom_left: pos = pos[0], pos[1] - size[1]

            pos = pos[0], 200 - pos[1] - size[1]

            CGContextTranslateCTM(c, pos[0], pos[1])
            if flip:
                #CGContextTranslateCTM(c, size[1], 0)
                size = size[0] * -1, size[1]
                CGContextScaleCTM(c, -1, 1)

            CGContextDrawImage(
                c,
                NSRect(NSPoint(0, 0), NSSize(*size)),
                image.cgimage,
            )

            if flip:
                CGContextScaleCTM(c, -1, 1)
                #CGContextTranslateCTM(c, -size[1], 0)

            CGContextTranslateCTM(c, -pos[0], -pos[1])

        self.tr.blit(blit)

        CGContextScaleCTM(c, -3, -3)

    def acceptsFirstResponder(self):
        return True

    def keyDown_(self, ev):
        code = ev.keyCode()

        if code == 49: # spacebar
            import sys
            sys.exit()

        if code == 123:
    		self.directions.add(LEFT)

        if code == 124:
    		self.directions.add(RIGHT)

        if code == 126:
            self.directions.add(TOP)
    
        if code == 125:
            self.directions.add(BOTTOM)

    def keyUp_(self, ev):
        code = ev.keyCode()

        if code == 123:
    		self.directions.remove(LEFT)

        if code == 124:
    		self.directions.remove(RIGHT)

        if code == 126:
            self.directions.remove(TOP)
    
        if code == 125:
            self.directions.remove(BOTTOM)

class ViewController(NSViewController):
    def loadView(self):
        iv = ImageView.alloc().initWithFrame_(NSRect(NSPoint(0, 0), NSSize(320 * 3, 200 * 3)))
        iv.tr = self.tr
        iv.directions = set()
        self.setView_(iv)

        s = objc.selector(self.tick,signature='v@:')
        self.timer = NSTimer.scheduledTimerWithTimeInterval_target_selector_userInfo_repeats_(1/70.0, self,s,None,True)

    def tick(self):
        X = RIGHT if RIGHT in self.view().directions else LEFT   if LEFT   in self.view().directions else STILL
        Y = TOP   if TOP   in self.view().directions else BOTTOM if BOTTOM in self.view().directions else STILL

        self.tr.step((X, Y, NO_RUNNING))

        self.view().setNeedsDisplay_(True)


class Renderer:
    def __init__(self, size):
        self.width, self.height = size
        self.cgimage, self.data = create_image_and_data(self.width, self.height)

    def set_at(self, pos, color):
        x, y = pos

        r, g, b = color

        pix = (y * self.width + x) * 4
        self.data.mutableBytes()[pix    ] = chr(0xff)
        self.data.mutableBytes()[pix + 1] = chr(r)
        self.data.mutableBytes()[pix + 2] = chr(g)
        self.data.mutableBytes()[pix + 3] = chr(b)

    def image(self):
        return self

    def size(self):
        return self.width, self.height

if __name__ == "__main__":
    tr = TimeRunners(Renderer)

    app = NSApplication.sharedApplication()
    vc  = ViewController.alloc().init()
    vc.tr = tr

    win = NSWindow.windowWithContentViewController_(vc)
    win.makeKeyAndOrderFront_(None)
    win.setInitialFirstResponder_(vc.view())

    AppHelper.runEventLoop()

