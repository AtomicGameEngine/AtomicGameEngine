
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Technique.h>

#include "WebClient.h"
#include "WebRenderHandler.h"

namespace Atomic
{

WebRenderHandler::WebRenderHandler(Context* context) : Object (context)
{
}

WebRenderHandler::~WebRenderHandler()
{

}

void WebRenderHandler::SetWebClient(WebClient* webClient)
{
    webClient_ = webClient;
}

WebClient* WebRenderHandler::GetWebClient() const
{
    return webClient_;
}



}
