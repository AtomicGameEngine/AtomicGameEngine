
#include <stdio.h>

#include <ThirdParty/SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <Atomic/Graphics/Graphics.h>

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
    dragAndDrop_->BeginDrag();
    return NSDragOperationGeneric;
}

-(NSDragOperation)draggingUpdated:(id)sender
{
    return NSDragOperationGeneric;
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
    NSPasteboard *pboard = [sender draggingPasteboard];
    NSString *type = [pboard availableTypeFromArray:[NSArray arrayWithObject:NSFilenamesPboardType]];
    if (type)
    {
        if ([type isEqualToString:NSFilenamesPboardType])
        {
            NSArray *filenames = [pboard propertyListForType:NSFilenamesPboardType];
            unsigned i = [filenames count];
            NSMutableArray *goodFiles = [NSMutableArray array];
            NSFileManager *fm = [NSFileManager defaultManager];
            BOOL isDir;
            while (i-- > 0)
            {
                NSString *filename = [filenames objectAtIndex:i];
                dragAndDrop_->AddDragFilename([filename UTF8String]);
            }

            dragAndDrop_->ConcludeDrag();
        }
    }
}

@end

namespace AtomicEditor
{

void InitDragAndDrop(AEDragAndDrop *dragAndDrop)
{
    dragAndDrop_ = dragAndDrop;

    SDL_Window* window = (SDL_Window*) dragAndDrop->GetSubsystem<Graphics>()->GetSDLWindow();

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if(SDL_GetWindowWMInfo(window, &info)) {

        NSWindow* window = info.info.cocoa.window;
        [window registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    }

}

}

