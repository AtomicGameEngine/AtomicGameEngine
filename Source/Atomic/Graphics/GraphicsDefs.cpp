//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "Precompiled.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Math/StringHash.h"
#include "../Math/Vector3.h"

#include "../DebugNew.h"

namespace Atomic
{

extern ATOMIC_API const StringHash VSP_AMBIENTSTARTCOLOR("AmbientStartColor");
extern ATOMIC_API const StringHash VSP_AMBIENTENDCOLOR("AmbientEndColor");
extern ATOMIC_API const StringHash VSP_BILLBOARDROT("BillboardRot");
extern ATOMIC_API const StringHash VSP_CAMERAPOS("CameraPos");
extern ATOMIC_API const StringHash VSP_CAMERAROT("CameraRot");
extern ATOMIC_API const StringHash VSP_CLIPPLANE("ClipPlane");
extern ATOMIC_API const StringHash VSP_NEARCLIP("NearClip");
extern ATOMIC_API const StringHash VSP_FARCLIP("FarClip");
extern ATOMIC_API const StringHash VSP_DEPTHMODE("DepthMode");
extern ATOMIC_API const StringHash VSP_DELTATIME("DeltaTime");
extern ATOMIC_API const StringHash VSP_ELAPSEDTIME("ElapsedTime");
extern ATOMIC_API const StringHash VSP_FRUSTUMSIZE("FrustumSize");
extern ATOMIC_API const StringHash VSP_GBUFFEROFFSETS("GBufferOffsets");
extern ATOMIC_API const StringHash VSP_LIGHTDIR("LightDir");
extern ATOMIC_API const StringHash VSP_LIGHTPOS("LightPos");
extern ATOMIC_API const StringHash VSP_MODEL("Model");
extern ATOMIC_API const StringHash VSP_VIEWPROJ("ViewProj");
extern ATOMIC_API const StringHash VSP_UOFFSET("UOffset");
extern ATOMIC_API const StringHash VSP_VOFFSET("VOffset");
extern ATOMIC_API const StringHash VSP_LMOFFSET("LMOffset");
extern ATOMIC_API const StringHash VSP_ZONE("Zone");
extern ATOMIC_API const StringHash VSP_LIGHTMATRICES("LightMatrices");
extern ATOMIC_API const StringHash VSP_SKINMATRICES("SkinMatrices");
extern ATOMIC_API const StringHash VSP_VERTEXLIGHTS("VertexLights");
extern ATOMIC_API const StringHash PSP_AMBIENTCOLOR("AmbientColor");
extern ATOMIC_API const StringHash PSP_CAMERAPOS("CameraPosPS");
extern ATOMIC_API const StringHash PSP_DELTATIME("DeltaTimePS");
extern ATOMIC_API const StringHash PSP_DEPTHRECONSTRUCT("DepthReconstruct");
extern ATOMIC_API const StringHash PSP_ELAPSEDTIME("ElapsedTimePS");
extern ATOMIC_API const StringHash PSP_FOGCOLOR("FogColor");
extern ATOMIC_API const StringHash PSP_FOGPARAMS("FogParams");
extern ATOMIC_API const StringHash PSP_GBUFFERINVSIZE("GBufferInvSize");
extern ATOMIC_API const StringHash PSP_LIGHTCOLOR("LightColor");
extern ATOMIC_API const StringHash PSP_LIGHTDIR("LightDirPS");
extern ATOMIC_API const StringHash PSP_LIGHTPOS("LightPosPS");
extern ATOMIC_API const StringHash PSP_MATDIFFCOLOR("MatDiffColor");
extern ATOMIC_API const StringHash PSP_MATEMISSIVECOLOR("MatEmissiveColor");
extern ATOMIC_API const StringHash PSP_MATENVMAPECOLOR("MatEnvMapColor");
extern ATOMIC_API const StringHash PSP_MATSPECCOLOR("MatSpecColor");
extern ATOMIC_API const StringHash PSP_NEARCLIP("NearClipPS");
extern ATOMIC_API const StringHash PSP_FARCLIP("FarClipPS");
extern ATOMIC_API const StringHash PSP_SHADOWCUBEADJUST("ShadowCubeAdjust");
extern ATOMIC_API const StringHash PSP_SHADOWDEPTHFADE("ShadowDepthFade");
extern ATOMIC_API const StringHash PSP_SHADOWINTENSITY("ShadowIntensity");
extern ATOMIC_API const StringHash PSP_SHADOWMAPINVSIZE("ShadowMapInvSize");
extern ATOMIC_API const StringHash PSP_SHADOWSPLITS("ShadowSplits");
extern ATOMIC_API const StringHash PSP_LIGHTMATRICES("LightMatricesPS");

extern ATOMIC_API const Vector3 DOT_SCALE(1 / 3.0f, 1 / 3.0f, 1 / 3.0f);

}
