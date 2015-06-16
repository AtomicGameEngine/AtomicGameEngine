/*
#include <stdio.h>

#include <ThirdParty/SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Graphics/Graphics.h>

#include <ToolCore/Assets/Asset.h>

#include "AEDragAndDrop.h"

static WeakPtr<AtomicEditor::AEDragAndDrop> dragAndDrop_;

@interface NSWindow (NSWindowWithDragAndDrop)

    -(NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender;
    -(NSDragOperation)draggingUpdated:(id)sender;
    -(BOOL)prepareForDragOperation:(id)sender;
    -(BOOL)performDragOperation:(id <NSDraggingInfo>)sender;
    -(void)concludeDragOperation:(id <NSDraggingInfo>)sender;
@end

@implementation NSWindow (NSWindowWithDragAndDrop)

-(NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pboard = [sender draggingPasteboard];
    NSString *type = [pboard availableTypeFromArray:[NSArray arrayWithObject:NSFilenamesPboardType]];
    if (type)
    {
        if ([type isEqualToString:NSFilenamesPboardType])
        {
            NSArray *filenames = [pboard propertyListForType:NSFilenamesPboardType];
            int i = (int) [filenames count];

            if (i)
                dragAndDrop_->BeginDrag();

            while (i-- > 0)
            {
                NSString *filename = [filenames objectAtIndex:i];
                dragAndDrop_->AddDragFilename([filename UTF8String]);
            }

        }
    }

    return NSDragOperationCopy;
}

-(NSDragOperation)draggingUpdated:(id <NSDraggingInfo>)sender
{
    using namespace Atomic::MouseMove;

    Graphics* graphics = dragAndDrop_->GetSubsystem<Graphics>();

    NSPoint dragPoint = [sender draggingLocation];

    VariantMap eventData;
    eventData[P_X] = (int) dragPoint.x;
    eventData[P_Y] = graphics->GetHeight() - (int) dragPoint.y;
    eventData[P_DX] = 0;
    eventData[P_DY] = 0;
    eventData[P_BUTTONS] = 0;
    eventData[P_QUALIFIERS] = 0;

    dragAndDrop_->SendEvent(E_MOUSEMOVE, eventData);

    return NSDragOperationCopy;
}

-(BOOL)prepareForDragOperation:(id)sender
{
    return YES;
}

-(BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    return YES;
}

-(void)concludeDragOperation:(id <NSDraggingInfo>)sender
{
    dragAndDrop_->ConcludeDrag();
}

@end

namespace AtomicEditor
{

static NSWindow* GetNSWindow()
{
    SDL_Window* window = (SDL_Window*) dragAndDrop_->GetSubsystem<Graphics>()->GetSDLWindow();

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if(SDL_GetWindowWMInfo(window, &info)) {

        return info.info.cocoa.window;
    }

    return NULL;
}

void StartDragAndDrop()
{
    Object* o = dragAndDrop_->GetDragObject();

    if (!o)
        return;

    ToolCore::Asset* asset = o->GetType() == ToolCore::Asset::GetTypeStatic() ? (ToolCore::Asset*) o : NULL;

    if (asset)
    {
        NSWindow* window = GetNSWindow();
        if (window)
        {

            NSImage *dragImage = nil;
            NSPoint dragPosition;
            NSString* filePath = [NSString stringWithUTF8String:asset->GetPath().CString()];

            // Write data to the pasteboard

            NSArray *fileList = [NSArray arrayWithObjects:filePath, nil];
            NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
            [pboard declareTypes:[NSArray arrayWithObject:NSFilenamesPboardType]
                    owner:nil];
            [pboard setPropertyList:fileList forType:NSFilenamesPboardType];

            // blocks
            [window dragImage:dragImage
                    at:dragPosition
                    offset:NSZeroSize
                    event:nil
                    pasteboard:pboard
                    source:window
                    slideBack:YES];

        }
    }
}

void InitDragAndDrop(AEDragAndDrop *dragAndDrop)
{
    dragAndDrop_ = dragAndDrop;

    NSWindow* window = GetNSWindow();
    if (window)
    {
        [window registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    }

}

}

*/

