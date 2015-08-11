package atomic;

@:native("Atomic")
extern enum FrustumPlane {
    PLANE_NEAR;
    PLANE_LEFT;
    PLANE_RIGHT;
    PLANE_UP;
    PLANE_DOWN;
    PLANE_FAR;
}
@:native("Atomic")
extern enum Intersection {
    OUTSIDE;
    INTERSECTS;
    INSIDE;
}
@:native("Atomic")
extern enum InterpolationMode {
    BEZIER_CURVE;
}
@:native("Atomic")
extern enum VariantType {
    VAR_NONE;
    VAR_INT;
    VAR_BOOL;
    VAR_FLOAT;
    VAR_VECTOR2;
    VAR_VECTOR3;
    VAR_VECTOR4;
    VAR_QUATERNION;
    VAR_COLOR;
    VAR_STRING;
    VAR_BUFFER;
    VAR_VOIDPTR;
    VAR_RESOURCEREF;
    VAR_RESOURCEREFLIST;
    VAR_VARIANTVECTOR;
    VAR_VARIANTMAP;
    VAR_INTRECT;
    VAR_INTVECTOR2;
    VAR_PTR;
    VAR_MATRIX3;
    VAR_MATRIX3X4;
    VAR_MATRIX4;
    VAR_DOUBLE;
    MAX_VAR_TYPES;
}
@:native("Atomic")
extern enum WrapMode {
    WM_LOOP;
    WM_ONCE;
    WM_CLAMP;
}
@:native("Atomic")
extern enum CreateMode {
    REPLICATED;
    LOCAL;
}
@:native("Atomic")
extern enum TransformSpace {
    TS_LOCAL;
    TS_PARENT;
    TS_WORLD;
}
@:native("Atomic")
extern enum LoadMode {
    LOAD_RESOURCES_ONLY;
    LOAD_SCENE;
    LOAD_SCENE_AND_RESOURCES;
}
@:native("Atomic")
extern enum InterpMethod {
    IM_LINEAR;
    IM_SPLINE;
}
@:native("Atomic")
extern enum UpdateGeometryType {
    UPDATE_NONE;
    UPDATE_MAIN_THREAD;
    UPDATE_WORKER_THREAD;
}
@:native("Atomic")
extern enum PrimitiveType {
    TRIANGLE_LIST;
    LINE_LIST;
    POINT_LIST;
    TRIANGLE_STRIP;
    LINE_STRIP;
    TRIANGLE_FAN;
}
@:native("Atomic")
extern enum GeometryType {
    GEOM_STATIC;
    GEOM_SKINNED;
    GEOM_INSTANCED;
    GEOM_BILLBOARD;
    GEOM_STATIC_NOINSTANCING;
    MAX_GEOMETRYTYPES;
}
@:native("Atomic")
extern enum BlendMode {
    BLEND_REPLACE;
    BLEND_ADD;
    BLEND_MULTIPLY;
    BLEND_ALPHA;
    BLEND_ADDALPHA;
    BLEND_PREMULALPHA;
    BLEND_INVDESTALPHA;
    BLEND_SUBTRACT;
    BLEND_SUBTRACTALPHA;
    MAX_BLENDMODES;
}
@:native("Atomic")
extern enum CompareMode {
    CMP_ALWAYS;
    CMP_EQUAL;
    CMP_NOTEQUAL;
    CMP_LESS;
    CMP_LESSEQUAL;
    CMP_GREATER;
    CMP_GREATEREQUAL;
    MAX_COMPAREMODES;
}
@:native("Atomic")
extern enum CullMode {
    CULL_NONE;
    CULL_CCW;
    CULL_CW;
    MAX_CULLMODES;
}
@:native("Atomic")
extern enum FillMode {
    FILL_SOLID;
    FILL_WIREFRAME;
    FILL_POINT;
}
@:native("Atomic")
extern enum StencilOp {
    OP_KEEP;
    OP_ZERO;
    OP_REF;
    OP_INCR;
    OP_DECR;
}
@:native("Atomic")
extern enum LockState {
    LOCK_NONE;
    LOCK_HARDWARE;
    LOCK_SHADOW;
    LOCK_SCRATCH;
}
@:native("Atomic")
extern enum VertexElement {
    ELEMENT_POSITION;
    ELEMENT_NORMAL;
    ELEMENT_COLOR;
    ELEMENT_TEXCOORD1;
    ELEMENT_TEXCOORD2;
    ELEMENT_CUBETEXCOORD1;
    ELEMENT_CUBETEXCOORD2;
    ELEMENT_TANGENT;
    ELEMENT_BLENDWEIGHTS;
    ELEMENT_BLENDINDICES;
    ELEMENT_INSTANCEMATRIX1;
    ELEMENT_INSTANCEMATRIX2;
    ELEMENT_INSTANCEMATRIX3;
    MAX_VERTEX_ELEMENTS;
}
@:native("Atomic")
extern enum TextureFilterMode {
    FILTER_NEAREST;
    FILTER_BILINEAR;
    FILTER_TRILINEAR;
    FILTER_ANISOTROPIC;
    FILTER_DEFAULT;
    MAX_FILTERMODES;
}
@:native("Atomic")
extern enum TextureAddressMode {
    ADDRESS_WRAP;
    ADDRESS_MIRROR;
    ADDRESS_CLAMP;
    ADDRESS_BORDER;
    MAX_ADDRESSMODES;
}
@:native("Atomic")
extern enum TextureCoordinate {
    COORD_U;
    COORD_V;
    COORD_W;
    MAX_COORDS;
}
@:native("Atomic")
extern enum TextureUsage {
    TEXTURE_STATIC;
    TEXTURE_DYNAMIC;
    TEXTURE_RENDERTARGET;
    TEXTURE_DEPTHSTENCIL;
}
@:native("Atomic")
extern enum CubeMapFace {
    FACE_POSITIVE_X;
    FACE_NEGATIVE_X;
    FACE_POSITIVE_Y;
    FACE_NEGATIVE_Y;
    FACE_POSITIVE_Z;
    FACE_NEGATIVE_Z;
    MAX_CUBEMAP_FACES;
}
@:native("Atomic")
extern enum CubeMapLayout {
    CML_HORIZONTAL;
    CML_HORIZONTALNVIDIA;
    CML_HORIZONTALCROSS;
    CML_VERTICALCROSS;
    CML_BLENDER;
}
@:native("Atomic")
extern enum RenderSurfaceUpdateMode {
    SURFACE_MANUALUPDATE;
    SURFACE_UPDATEVISIBLE;
    SURFACE_UPDATEALWAYS;
}
@:native("Atomic")
extern enum ShaderType {
    VS;
    PS;
}
@:native("Atomic")
extern enum ShaderParameterGroup {
    SP_FRAME;
    SP_CAMERA;
    SP_ZONE;
    SP_LIGHT;
    SP_MATERIAL;
    SP_OBJECT;
    SP_CUSTOM;
    MAX_SHADER_PARAMETER_GROUPS;
}
@:native("Atomic")
extern enum TextureUnit {
    TU_DIFFUSE;
    TU_ALBEDOBUFFER;
    TU_NORMAL;
    TU_NORMALBUFFER;
    TU_SPECULAR;
    TU_EMISSIVE;
    TU_ENVIRONMENT;
    TU_VOLUMEMAP;
    TU_CUSTOM1;
    TU_CUSTOM2;
    TU_LIGHTRAMP;
    TU_LIGHTSHAPE;
    TU_SHADOWMAP;
    TU_FACESELECT;
    TU_INDIRECTION;
    TU_DEPTHBUFFER;
    TU_LIGHTBUFFER;
    TU_ZONE;
    MAX_MATERIAL_TEXTURE_UNITS;
    MAX_TEXTURE_UNITS;
}
@:native("Atomic")
extern enum FaceCameraMode {
    FC_NONE;
    FC_ROTATE_XYZ;
    FC_ROTATE_Y;
    FC_LOOKAT_XYZ;
    FC_LOOKAT_Y;
}
@:native("Atomic")
extern enum LightType {
    LIGHT_DIRECTIONAL;
    LIGHT_SPOT;
    LIGHT_POINT;
}
@:native("Atomic")
extern enum RayQueryLevel {
    RAY_AABB;
    RAY_OBB;
    RAY_TRIANGLE;
    RAY_TRIANGLE_UV;
}
@:native("Atomic")
extern enum LightVSVariation {
    LVS_DIR;
    LVS_SPOT;
    LVS_POINT;
    LVS_SHADOW;
    LVS_SPOTSHADOW;
    LVS_POINTSHADOW;
    MAX_LIGHT_VS_VARIATIONS;
}
@:native("Atomic")
extern enum VertexLightVSVariation {
    VLVS_NOLIGHTS;
    VLVS_1LIGHT;
    VLVS_2LIGHTS;
    VLVS_3LIGHTS;
    VLVS_4LIGHTS;
    MAX_VERTEXLIGHT_VS_VARIATIONS;
}
@:native("Atomic")
extern enum LightPSVariation {
    LPS_NONE;
    LPS_SPOT;
    LPS_POINT;
    LPS_POINTMASK;
    LPS_SPEC;
    LPS_SPOTSPEC;
    LPS_POINTSPEC;
    LPS_POINTMASKSPEC;
    LPS_SHADOW;
    LPS_SPOTSHADOW;
    LPS_POINTSHADOW;
    LPS_POINTMASKSHADOW;
    LPS_SHADOWSPEC;
    LPS_SPOTSHADOWSPEC;
    LPS_POINTSHADOWSPEC;
    LPS_POINTMASKSHADOWSPEC;
    MAX_LIGHT_PS_VARIATIONS;
}
@:native("Atomic")
extern enum DeferredLightVSVariation {
    DLVS_NONE;
    DLVS_DIR;
    DLVS_ORTHO;
    DLVS_ORTHODIR;
    MAX_DEFERRED_LIGHT_VS_VARIATIONS;
}
@:native("Atomic")
extern enum DeferredLightPSVariation {
    DLPS_NONE;
    DLPS_SPOT;
    DLPS_POINT;
    DLPS_POINTMASK;
    DLPS_SPEC;
    DLPS_SPOTSPEC;
    DLPS_POINTSPEC;
    DLPS_POINTMASKSPEC;
    DLPS_SHADOW;
    DLPS_SPOTSHADOW;
    DLPS_POINTSHADOW;
    DLPS_POINTMASKSHADOW;
    DLPS_SHADOWSPEC;
    DLPS_SPOTSHADOWSPEC;
    DLPS_POINTSHADOWSPEC;
    DLPS_POINTMASKSHADOWSPEC;
    DLPS_ORTHO;
    DLPS_ORTHOSPOT;
    DLPS_ORTHOPOINT;
    DLPS_ORTHOPOINTMASK;
    DLPS_ORTHOSPEC;
    DLPS_ORTHOSPOTSPEC;
    DLPS_ORTHOPOINTSPEC;
    DLPS_ORTHOPOINTMASKSPEC;
    DLPS_ORTHOSHADOW;
    DLPS_ORTHOSPOTSHADOW;
    DLPS_ORTHOPOINTSHADOW;
    DLPS_ORTHOPOINTMASKSHADOW;
    DLPS_ORTHOSHADOWSPEC;
    DLPS_ORTHOSPOTSHADOWSPEC;
    DLPS_ORTHOPOINTSHADOWSPEC;
    DLPS_ORTHOPOINTMASKSHADOWSPEC;
    MAX_DEFERRED_LIGHT_PS_VARIATIONS;
}
@:native("Atomic")
extern enum RenderCommandType {
    CMD_NONE;
    CMD_CLEAR;
    CMD_SCENEPASS;
    CMD_QUAD;
    CMD_FORWARDLIGHTS;
    CMD_LIGHTVOLUMES;
    CMD_RENDERUI;
}
@:native("Atomic")
extern enum RenderCommandSortMode {
    SORT_FRONTTOBACK;
    SORT_BACKTOFRONT;
}
@:native("Atomic")
extern enum RenderTargetSizeMode {
    SIZE_ABSOLUTE;
    SIZE_VIEWPORTDIVISOR;
    SIZE_VIEWPORTMULTIPLIER;
}
@:native("Atomic")
extern enum PassLightingMode {
    LIGHTING_UNLIT;
    LIGHTING_PERVERTEX;
    LIGHTING_PERPIXEL;
}
@:native("Atomic")
extern enum EmitterType {
    EMITTER_SPHERE;
    EMITTER_BOX;
}
@:native("Atomic")
extern enum LoopMode2D {
    LM_DEFAULT;
    LM_FORCE_LOOPED;
    LM_FORCE_CLAMPED;
}
@:native("Atomic")
extern enum LightType2D {
    LIGHT2D_DIRECTIONAL;
    LIGHT2D_POINT;
}
@:native("Atomic")
extern enum EmitterType2D {
    EMITTER_TYPE_GRAVITY;
    EMITTER_TYPE_RADIAL;
}
@:native("Atomic")
extern enum BodyType2D {
    BT_STATIC;
    BT_DYNAMIC;
    BT_KINEMATIC;
}
@:native("Atomic")
extern enum Orientation2D {
    O_ORTHOGONAL;
    O_ISOMETRIC;
    O_STAGGERED;
}
@:native("Atomic")
extern enum TileMapLayerType2D {
    LT_TILE_LAYER;
    LT_OBJECT_GROUP;
    LT_IMAGE_LAYER;
    LT_INVALID;
}
@:native("Atomic")
extern enum TileMapObjectType2D {
    OT_RECTANGLE;
    OT_ELLIPSE;
    OT_POLYGON;
    OT_POLYLINE;
    OT_TILE;
    OT_INVALID;
}
@:native("Atomic")
extern enum ShapeType {
    SHAPE_BOX;
    SHAPE_SPHERE;
    SHAPE_STATICPLANE;
    SHAPE_CYLINDER;
    SHAPE_CAPSULE;
    SHAPE_CONE;
    SHAPE_TRIANGLEMESH;
    SHAPE_CONVEXHULL;
    SHAPE_TERRAIN;
}
@:native("Atomic")
extern enum ConstraintType {
    CONSTRAINT_POINT;
    CONSTRAINT_HINGE;
    CONSTRAINT_SLIDER;
    CONSTRAINT_CONETWIST;
}
@:native("Atomic")
extern enum CollisionEventMode {
    COLLISION_NEVER;
    COLLISION_ACTIVE;
    COLLISION_ALWAYS;
}
@:native("Atomic")
extern enum CrowdTargetState {
    CROWD_AGENT_TARGET_NONE;
    CROWD_AGENT_TARGET_FAILED;
    CROWD_AGENT_TARGET_VALID;
    CROWD_AGENT_TARGET_REQUESTING;
    CROWD_AGENT_TARGET_WAITINGFORQUEUE;
    CROWD_AGENT_TARGET_WAITINGFORPATH;
    CROWD_AGENT_TARGET_VELOCITY;
}
@:native("Atomic")
extern enum CrowdAgentState {
    CROWD_AGENT_INVALID;
    CROWD_AGENT_READY;
    CROWD_AGENT_TRAVERSINGLINK;
}
@:native("Atomic")
extern enum NavigationQuality {
    NAVIGATIONQUALITY_LOW;
    NAVIGATIONQUALITY_MEDIUM;
    NAVIGATIONQUALITY_HIGH;
}
@:native("Atomic")
extern enum NavigationPushiness {
    PUSHINESS_LOW;
    PUSHINESS_MEDIUM;
    PUSHINESS_HIGH;
}
@:native("Atomic")
extern enum NavmeshPartitionType {
    NAVMESH_PARTITION_WATERSHED;
    NAVMESH_PARTITION_MONOTONE;
}
@:native("Atomic")
extern enum MouseMode {
    MM_ABSOLUTE;
    MM_RELATIVE;
    MM_WRAP;
}
@:native("Atomic")
extern enum TEXT_ALIGN {
    TEXT_ALIGN_LEFT;
    TEXT_ALIGN_RIGHT;
    TEXT_ALIGN_CENTER;
}
@:native("Atomic")
extern enum UI_EDIT_TYPE {
    UI_EDIT_TYPE_TEXT;
    UI_EDIT_TYPE_SEARCH;
    UI_EDIT_TYPE_PASSWORD;
    UI_EDIT_TYPE_EMAIL;
    UI_EDIT_TYPE_PHONE;
    UI_EDIT_TYPE_URL;
    UI_EDIT_TYPE_NUMBER;
}
@:native("Atomic")
extern enum UI_AXIS {
    UI_AXIS_X;
    UI_AXIS_Y;
}
@:native("Atomic")
extern enum UI_LAYOUT_SIZE {
    UI_LAYOUT_SIZE_GRAVITY;
    UI_LAYOUT_SIZE_PREFERRED;
    UI_LAYOUT_SIZE_AVAILABLE;
}
@:native("Atomic")
extern enum UI_LAYOUT_DISTRIBUTION {
    UI_LAYOUT_DISTRIBUTION_PREFERRED;
    UI_LAYOUT_DISTRIBUTION_AVAILABLE;
    UI_LAYOUT_DISTRIBUTION_GRAVITY;
}
@:native("Atomic")
extern enum UI_LAYOUT_POSITION {
    UI_LAYOUT_POSITION_CENTER;
    UI_LAYOUT_POSITION_LEFT_TOP;
    UI_LAYOUT_POSITION_RIGHT_BOTTOM;
    UI_LAYOUT_POSITION_GRAVITY;
}
@:native("Atomic")
extern enum UI_LAYOUT_DISTRIBUTION_POSITION {
    UI_LAYOUT_DISTRIBUTION_POSITION_CENTER;
    UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;
    UI_LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM;
}
@:native("Atomic")
extern enum UI_MESSAGEWINDOW_SETTINGS {
    UI_MESSAGEWINDOW_SETTINGS_OK;
    UI_MESSAGEWINDOW_SETTINGS_OK_CANCEL;
    UI_MESSAGEWINDOW_SETTINGS_YES_NO;
}
@:native("Atomic")
extern enum UI_SIZE_DEP {
    UI_SIZE_DEP_NONE;
    UI_SIZE_DEP_WIDTH_DEPEND_ON_HEIGHT;
    UI_SIZE_DEP_HEIGHT_DEPEND_ON_WIDTH;
    UI_SIZE_DEP_BOTH;
}
@:native("Atomic")
extern enum UI_SCROLL_MODE {
    UI_SCROLL_MODE_X_Y;
    UI_SCROLL_MODE_Y;
    UI_SCROLL_MODE_Y_AUTO;
    UI_SCROLL_MODE_X_AUTO_Y_AUTO;
    UI_SCROLL_MODE_OFF;
}
@:native("Atomic")
extern enum UI_TEXT_ALIGN {
    UI_TEXT_ALIGN_LEFT;
    UI_TEXT_ALIGN_RIGHT;
    UI_TEXT_ALIGN_CENTER;
}
@:native("Atomic")
extern enum UI_WIDGET_VISIBILITY {
    UI_WIDGET_VISIBILITY_VISIBLE;
    UI_WIDGET_VISIBILITY_INVISIBLE;
    UI_WIDGET_VISIBILITY_GONE;
}
@:native("Atomic")
extern enum UI_GRAVITY {
    UI_GRAVITY_NONE;
    UI_GRAVITY_LEFT;
    UI_GRAVITY_RIGHT;
    UI_GRAVITY_TOP;
    UI_GRAVITY_BOTTOM;
    UI_GRAVITY_LEFT_RIGHT;
    UI_GRAVITY_TOP_BOTTOM;
    UI_GRAVITY_ALL;
    UI_GRAVITY_DEFAULT;
}
@:native("Atomic")
extern enum UI_EVENT_TYPE {
    UI_EVENT_TYPE_CLICK;
    UI_EVENT_TYPE_LONG_CLICK;
    UI_EVENT_TYPE_POINTER_DOWN;
    UI_EVENT_TYPE_POINTER_UP;
    UI_EVENT_TYPE_POINTER_MOVE;
    UI_EVENT_TYPE_RIGHT_POINTER_DOWN;
    UI_EVENT_TYPE_RIGHT_POINTER_UP;
    UI_EVENT_TYPE_WHEEL;
    UI_EVENT_TYPE_CHANGED;
    UI_EVENT_TYPE_KEY_DOWN;
    UI_EVENT_TYPE_KEY_UP;
    UI_EVENT_TYPE_SHORTCUT;
    UI_EVENT_TYPE_CONTEXT_MENU;
    UI_EVENT_TYPE_FILE_DROP;
    UI_EVENT_TYPE_TAB_CHANGED;
    UI_EVENT_TYPE_CUSTOM;
}
@:native("Atomic")
extern enum UI_WIDGET_Z_REL {
    UI_WIDGET_Z_REL_BEFORE;
    UI_WIDGET_Z_REL_AFTER;
}
@:native("Atomic")
extern enum UI_WINDOW_SETTINGS {
    UI_WINDOW_SETTINGS_NONE;
    UI_WINDOW_SETTINGS_TITLEBAR;
    UI_WINDOW_SETTINGS_RESIZABLE;
    UI_WINDOW_SETTINGS_CLOSE_BUTTON;
    UI_WINDOW_SETTINGS_CAN_ACTIVATE;
    UI_WINDOW_SETTINGS_DEFAULT;
}
@:native("Atomic")
extern enum CompressedFormat {
    CF_NONE;
    CF_RGBA;
    CF_DXT1;
    CF_DXT3;
    CF_DXT5;
    CF_ETC1;
    CF_PVRTC_RGB_2BPP;
    CF_PVRTC_RGBA_2BPP;
    CF_PVRTC_RGB_4BPP;
    CF_PVRTC_RGBA_4BPP;
}
@:native("Atomic")
extern enum JSONValueType {
    JSON_ANY;
    JSON_OBJECT;
    JSON_ARRAY;
}
@:native("Atomic")
extern enum PListValueType {
    PLVT_NONE;
    PLVT_INT;
    PLVT_BOOL;
    PLVT_FLOAT;
    PLVT_STRING;
    PLVT_VALUEMAP;
    PLVT_VALUEVECTOR;
}
@:native("Atomic")
extern enum AsyncLoadState {
    ASYNC_DONE;
    ASYNC_QUEUED;
    ASYNC_LOADING;
    ASYNC_SUCCESS;
    ASYNC_FAIL;
}
@:native("Atomic")
extern enum ResourceRequest {
    RESOURCE_CHECKEXISTS;
    RESOURCE_GETFILE;
}
@:native("Atomic")
extern enum ObserverPositionSendMode {
    OPSM_NONE;
    OPSM_POSITION;
    OPSM_POSITION_ROTATION;
}
@:native("Atomic")
extern enum HttpRequestState {
    HTTP_INITIALIZING;
    HTTP_ERROR;
    HTTP_OPEN;
    HTTP_CLOSED;
}
@:native("Atomic")
extern enum FileMode {
    FILE_READ;
    FILE_WRITE;
    FILE_READWRITE;
}
//Atomic Haxe Definitions

extern class Atomic {
	
	public static var engine: Engine;
    public static var graphics: Graphics;
    public static var renderer: Renderer;
    public static var cache: ResourceCache;
    public static var input: Input;
    public static var fileSystem: FileSystem;
    public static var network: Network;

    public static var QUICKSORT_THRESHOLD: Int;
    public static var CONVERSION_BUFFER_LENGTH: Int;
    public static var MATRIX_CONVERSION_BUFFER_LENGTH: Int;


    public static var NUM_FRUSTUM_PLANES: Int;
    public static var NUM_FRUSTUM_VERTICES: Int;
    public static var M_PI: Int;
    public static var M_HALF_PI: Int;
    public static var M_MIN_INT: Int;
    public static var M_MAX_INT: Int;
    public static var M_MIN_UNSIGNED: Int;
    public static var M_MAX_UNSIGNED: Int;
    public static var M_EPSILON: Int;
    public static var M_LARGE_EPSILON: Int;
    public static var M_MIN_NEARCLIP: Int;
    public static var M_MAX_FOV: Int;
    public static var M_LARGE_VALUE: Int;
    public static var M_INFINITY: Int;
    public static var M_DEGTORAD: Int;
    public static var M_DEGTORAD_2: Int;
    public static var M_RADTODEG: Int;


    public static var AM_EDIT: Int;
    public static var AM_FILE: Int;
    public static var AM_NET: Int;
    public static var AM_DEFAULT: Int;
    public static var AM_LATESTDATA: Int;
    public static var AM_NOEDIT: Int;
    public static var AM_NODEID: Int;
    public static var AM_COMPONENTID: Int;
    public static var AM_NODEIDVECTOR: Int;


    public static var USE_UPDATE: Int;
    public static var USE_POSTUPDATE: Int;
    public static var USE_FIXEDUPDATE: Int;
    public static var USE_FIXEDPOSTUPDATE: Int;
    public static var MAX_NETWORK_ATTRIBUTES: Int;
    public static var FIRST_REPLICATED_ID: Int;
    public static var LAST_REPLICATED_ID: Int;
    public static var FIRST_LOCAL_ID: Int;
    public static var LAST_LOCAL_ID: Int;
    public static var SMOOTH_NONE: Int;
    public static var SMOOTH_POSITION: Int;
    public static var SMOOTH_ROTATION: Int;


    public static var DEFAULT_NEARCLIP: Int;
    public static var DEFAULT_FARCLIP: Int;
    public static var DEFAULT_CAMERA_FOV: Int;
    public static var DEFAULT_ORTHOSIZE: Int;
    public static var VO_NONE: Int;
    public static var VO_LOW_MATERIAL_QUALITY: Int;
    public static var VO_DISABLE_SHADOWS: Int;
    public static var VO_DISABLE_OCCLUSION: Int;
    public static var DRAWABLE_GEOMETRY: Int;
    public static var DRAWABLE_LIGHT: Int;
    public static var DRAWABLE_ZONE: Int;
    public static var DRAWABLE_GEOMETRY2D: Int;
    public static var DRAWABLE_ANY: Int;
    public static var DEFAULT_VIEWMASK: Int;
    public static var DEFAULT_LIGHTMASK: Int;
    public static var DEFAULT_SHADOWMASK: Int;
    public static var DEFAULT_ZONEMASK: Int;
    public static var MAX_VERTEX_LIGHTS: Int;
    public static var ANIMATION_LOD_BASESCALE: Int;
    public static var QUALITY_LOW: Int;
    public static var QUALITY_MEDIUM: Int;
    public static var QUALITY_HIGH: Int;
    public static var QUALITY_MAX: Int;
    public static var SHADOWQUALITY_LOW_16BIT: Int;
    public static var SHADOWQUALITY_LOW_24BIT: Int;
    public static var SHADOWQUALITY_HIGH_16BIT: Int;
    public static var SHADOWQUALITY_HIGH_24BIT: Int;
    public static var CLEAR_COLOR: Int;
    public static var CLEAR_DEPTH: Int;
    public static var CLEAR_STENCIL: Int;
    public static var MASK_NONE: Int;
    public static var MASK_POSITION: Int;
    public static var MASK_NORMAL: Int;
    public static var MASK_COLOR: Int;
    public static var MASK_TEXCOORD1: Int;
    public static var MASK_TEXCOORD2: Int;
    public static var MASK_CUBETEXCOORD1: Int;
    public static var MASK_CUBETEXCOORD2: Int;
    public static var MASK_TANGENT: Int;
    public static var MASK_BLENDWEIGHTS: Int;
    public static var MASK_BLENDINDICES: Int;
    public static var MASK_INSTANCEMATRIX1: Int;
    public static var MASK_INSTANCEMATRIX2: Int;
    public static var MASK_INSTANCEMATRIX3: Int;
    public static var MASK_DEFAULT: Int;
    public static var NO_ELEMENT: Int;
    public static var MAX_RENDERTARGETS: Int;
    public static var MAX_VERTEX_STREAMS: Int;
    public static var MAX_CONSTANT_REGISTERS: Int;
    public static var BITS_PER_COMPONENT: Int;
    public static var SHADOW_MIN_QUANTIZE: Int;
    public static var SHADOW_MIN_VIEW: Int;
    public static var MAX_LIGHT_SPLITS: Int;
    public static var MAX_CASCADE_SPLITS: Int;
    public static var OCCLUSION_MIN_SIZE: Int;
    public static var OCCLUSION_DEFAULT_MAX_TRIANGLES: Int;
    public static var OCCLUSION_RELATIVE_BIAS: Int;
    public static var OCCLUSION_FIXED_BIAS: Int;
    public static var OCCLUSION_X_SCALE: Int;
    public static var OCCLUSION_Z_SCALE: Int;
    public static var NUM_OCTANTS: Int;
    public static var ROOT_INDEX: Int;
    public static var SHADOW_MIN_PIXELS: Int;
    public static var INSTANCING_BUFFER_DEFAULT_SIZE: Int;
    public static var MAX_VIEWPORT_TEXTURES: Int;
    public static var MAX_TEXTURE_QUALITY_LEVELS: Int;


    public static var CHANNEL_POSITION: Int;
    public static var CHANNEL_ROTATION: Int;
    public static var CHANNEL_SCALE: Int;
    public static var MAX_BILLBOARDS: Int;
    public static var DEFAULT_NUM_PARTICLES: Int;
    public static var BONECOLLISION_NONE: Int;
    public static var BONECOLLISION_SPHERE: Int;
    public static var BONECOLLISION_BOX: Int;


    public static var PIXEL_SIZE: Int;


    public static var STREAM_BUFFER_LENGTH: Int;


    public static var DEFAULT_MAX_NETWORK_ANGULAR_VELOCITY: Int;


    public static var MOUSEB_LEFT: Int;
    public static var MOUSEB_MIDDLE: Int;
    public static var MOUSEB_RIGHT: Int;
    public static var MOUSEB_X1: Int;
    public static var MOUSEB_X2: Int;
    public static var QUAL_SHIFT: Int;
    public static var QUAL_CTRL: Int;
    public static var QUAL_ALT: Int;
    public static var QUAL_ANY: Int;
    public static var KEY_A: Int;
    public static var KEY_B: Int;
    public static var KEY_C: Int;
    public static var KEY_D: Int;
    public static var KEY_E: Int;
    public static var KEY_F: Int;
    public static var KEY_G: Int;
    public static var KEY_H: Int;
    public static var KEY_I: Int;
    public static var KEY_J: Int;
    public static var KEY_K: Int;
    public static var KEY_L: Int;
    public static var KEY_M: Int;
    public static var KEY_N: Int;
    public static var KEY_O: Int;
    public static var KEY_P: Int;
    public static var KEY_Q: Int;
    public static var KEY_R: Int;
    public static var KEY_S: Int;
    public static var KEY_T: Int;
    public static var KEY_U: Int;
    public static var KEY_V: Int;
    public static var KEY_W: Int;
    public static var KEY_X: Int;
    public static var KEY_Y: Int;
    public static var KEY_Z: Int;
    public static var KEY_0: Int;
    public static var KEY_1: Int;
    public static var KEY_2: Int;
    public static var KEY_3: Int;
    public static var KEY_4: Int;
    public static var KEY_5: Int;
    public static var KEY_6: Int;
    public static var KEY_7: Int;
    public static var KEY_8: Int;
    public static var KEY_9: Int;
    public static var KEY_BACKSPACE: Int;
    public static var KEY_TAB: Int;
    public static var KEY_RETURN: Int;
    public static var KEY_RETURN2: Int;
    public static var KEY_KP_ENTER: Int;
    public static var KEY_SHIFT: Int;
    public static var KEY_CTRL: Int;
    public static var KEY_ALT: Int;
    public static var KEY_GUI: Int;
    public static var KEY_PAUSE: Int;
    public static var KEY_CAPSLOCK: Int;
    public static var KEY_ESC: Int;
    public static var KEY_SPACE: Int;
    public static var KEY_PAGEUP: Int;
    public static var KEY_PAGEDOWN: Int;
    public static var KEY_END: Int;
    public static var KEY_HOME: Int;
    public static var KEY_LEFT: Int;
    public static var KEY_UP: Int;
    public static var KEY_RIGHT: Int;
    public static var KEY_DOWN: Int;
    public static var KEY_SELECT: Int;
    public static var KEY_PRINTSCREEN: Int;
    public static var KEY_INSERT: Int;
    public static var KEY_DELETE: Int;
    public static var KEY_LGUI: Int;
    public static var KEY_RGUI: Int;
    public static var KEY_APPLICATION: Int;
    public static var KEY_KP_0: Int;
    public static var KEY_KP_1: Int;
    public static var KEY_KP_2: Int;
    public static var KEY_KP_3: Int;
    public static var KEY_KP_4: Int;
    public static var KEY_KP_5: Int;
    public static var KEY_KP_6: Int;
    public static var KEY_KP_7: Int;
    public static var KEY_KP_8: Int;
    public static var KEY_KP_9: Int;
    public static var KEY_KP_MULTIPLY: Int;
    public static var KEY_KP_PLUS: Int;
    public static var KEY_KP_MINUS: Int;
    public static var KEY_KP_PERIOD: Int;
    public static var KEY_KP_DIVIDE: Int;
    public static var KEY_F1: Int;
    public static var KEY_F2: Int;
    public static var KEY_F3: Int;
    public static var KEY_F4: Int;
    public static var KEY_F5: Int;
    public static var KEY_F6: Int;
    public static var KEY_F7: Int;
    public static var KEY_F8: Int;
    public static var KEY_F9: Int;
    public static var KEY_F10: Int;
    public static var KEY_F11: Int;
    public static var KEY_F12: Int;
    public static var KEY_F13: Int;
    public static var KEY_F14: Int;
    public static var KEY_F15: Int;
    public static var KEY_F16: Int;
    public static var KEY_F17: Int;
    public static var KEY_F18: Int;
    public static var KEY_F19: Int;
    public static var KEY_F20: Int;
    public static var KEY_F21: Int;
    public static var KEY_F22: Int;
    public static var KEY_F23: Int;
    public static var KEY_F24: Int;
    public static var KEY_NUMLOCKCLEAR: Int;
    public static var KEY_SCROLLLOCK: Int;
    public static var KEY_LSHIFT: Int;
    public static var KEY_RSHIFT: Int;
    public static var KEY_LCTRL: Int;
    public static var KEY_RCTRL: Int;
    public static var KEY_LALT: Int;
    public static var KEY_RALT: Int;
    public static var SCANCODE_UNKNOWN: Int;
    public static var SCANCODE_CTRL: Int;
    public static var SCANCODE_SHIFT: Int;
    public static var SCANCODE_ALT: Int;
    public static var SCANCODE_GUI: Int;
    public static var SCANCODE_A: Int;
    public static var SCANCODE_B: Int;
    public static var SCANCODE_C: Int;
    public static var SCANCODE_D: Int;
    public static var SCANCODE_E: Int;
    public static var SCANCODE_F: Int;
    public static var SCANCODE_G: Int;
    public static var SCANCODE_H: Int;
    public static var SCANCODE_I: Int;
    public static var SCANCODE_J: Int;
    public static var SCANCODE_K: Int;
    public static var SCANCODE_L: Int;
    public static var SCANCODE_M: Int;
    public static var SCANCODE_N: Int;
    public static var SCANCODE_O: Int;
    public static var SCANCODE_P: Int;
    public static var SCANCODE_Q: Int;
    public static var SCANCODE_R: Int;
    public static var SCANCODE_S: Int;
    public static var SCANCODE_T: Int;
    public static var SCANCODE_U: Int;
    public static var SCANCODE_V: Int;
    public static var SCANCODE_W: Int;
    public static var SCANCODE_X: Int;
    public static var SCANCODE_Y: Int;
    public static var SCANCODE_Z: Int;
    public static var SCANCODE_1: Int;
    public static var SCANCODE_2: Int;
    public static var SCANCODE_3: Int;
    public static var SCANCODE_4: Int;
    public static var SCANCODE_5: Int;
    public static var SCANCODE_6: Int;
    public static var SCANCODE_7: Int;
    public static var SCANCODE_8: Int;
    public static var SCANCODE_9: Int;
    public static var SCANCODE_0: Int;
    public static var SCANCODE_RETURN: Int;
    public static var SCANCODE_ESCAPE: Int;
    public static var SCANCODE_BACKSPACE: Int;
    public static var SCANCODE_TAB: Int;
    public static var SCANCODE_SPACE: Int;
    public static var SCANCODE_MINUS: Int;
    public static var SCANCODE_EQUALS: Int;
    public static var SCANCODE_LEFTBRACKET: Int;
    public static var SCANCODE_RIGHTBRACKET: Int;
    public static var SCANCODE_BACKSLASH: Int;
    public static var SCANCODE_NONUSHASH: Int;
    public static var SCANCODE_SEMICOLON: Int;
    public static var SCANCODE_APOSTROPHE: Int;
    public static var SCANCODE_GRAVE: Int;
    public static var SCANCODE_COMMA: Int;
    public static var SCANCODE_PERIOD: Int;
    public static var SCANCODE_SLASH: Int;
    public static var SCANCODE_CAPSLOCK: Int;
    public static var SCANCODE_F1: Int;
    public static var SCANCODE_F2: Int;
    public static var SCANCODE_F3: Int;
    public static var SCANCODE_F4: Int;
    public static var SCANCODE_F5: Int;
    public static var SCANCODE_F6: Int;
    public static var SCANCODE_F7: Int;
    public static var SCANCODE_F8: Int;
    public static var SCANCODE_F9: Int;
    public static var SCANCODE_F10: Int;
    public static var SCANCODE_F11: Int;
    public static var SCANCODE_F12: Int;
    public static var SCANCODE_PRINTSCREEN: Int;
    public static var SCANCODE_SCROLLLOCK: Int;
    public static var SCANCODE_PAUSE: Int;
    public static var SCANCODE_INSERT: Int;
    public static var SCANCODE_HOME: Int;
    public static var SCANCODE_PAGEUP: Int;
    public static var SCANCODE_DELETE: Int;
    public static var SCANCODE_END: Int;
    public static var SCANCODE_PAGEDOWN: Int;
    public static var SCANCODE_RIGHT: Int;
    public static var SCANCODE_LEFT: Int;
    public static var SCANCODE_DOWN: Int;
    public static var SCANCODE_UP: Int;
    public static var SCANCODE_NUMLOCKCLEAR: Int;
    public static var SCANCODE_KP_DIVIDE: Int;
    public static var SCANCODE_KP_MULTIPLY: Int;
    public static var SCANCODE_KP_MINUS: Int;
    public static var SCANCODE_KP_PLUS: Int;
    public static var SCANCODE_KP_ENTER: Int;
    public static var SCANCODE_KP_1: Int;
    public static var SCANCODE_KP_2: Int;
    public static var SCANCODE_KP_3: Int;
    public static var SCANCODE_KP_4: Int;
    public static var SCANCODE_KP_5: Int;
    public static var SCANCODE_KP_6: Int;
    public static var SCANCODE_KP_7: Int;
    public static var SCANCODE_KP_8: Int;
    public static var SCANCODE_KP_9: Int;
    public static var SCANCODE_KP_0: Int;
    public static var SCANCODE_KP_PERIOD: Int;
    public static var SCANCODE_NONUSBACKSLASH: Int;
    public static var SCANCODE_APPLICATION: Int;
    public static var SCANCODE_POWER: Int;
    public static var SCANCODE_KP_EQUALS: Int;
    public static var SCANCODE_F13: Int;
    public static var SCANCODE_F14: Int;
    public static var SCANCODE_F15: Int;
    public static var SCANCODE_F16: Int;
    public static var SCANCODE_F17: Int;
    public static var SCANCODE_F18: Int;
    public static var SCANCODE_F19: Int;
    public static var SCANCODE_F20: Int;
    public static var SCANCODE_F21: Int;
    public static var SCANCODE_F22: Int;
    public static var SCANCODE_F23: Int;
    public static var SCANCODE_F24: Int;
    public static var SCANCODE_EXECUTE: Int;
    public static var SCANCODE_HELP: Int;
    public static var SCANCODE_MENU: Int;
    public static var SCANCODE_SELECT: Int;
    public static var SCANCODE_STOP: Int;
    public static var SCANCODE_AGAIN: Int;
    public static var SCANCODE_UNDO: Int;
    public static var SCANCODE_CUT: Int;
    public static var SCANCODE_COPY: Int;
    public static var SCANCODE_PASTE: Int;
    public static var SCANCODE_FIND: Int;
    public static var SCANCODE_MUTE: Int;
    public static var SCANCODE_VOLUMEUP: Int;
    public static var SCANCODE_VOLUMEDOWN: Int;
    public static var SCANCODE_KP_COMMA: Int;
    public static var SCANCODE_KP_EQUALSAS400: Int;
    public static var SCANCODE_INTERNATIONAL1: Int;
    public static var SCANCODE_INTERNATIONAL2: Int;
    public static var SCANCODE_INTERNATIONAL3: Int;
    public static var SCANCODE_INTERNATIONAL4: Int;
    public static var SCANCODE_INTERNATIONAL5: Int;
    public static var SCANCODE_INTERNATIONAL6: Int;
    public static var SCANCODE_INTERNATIONAL7: Int;
    public static var SCANCODE_INTERNATIONAL8: Int;
    public static var SCANCODE_INTERNATIONAL9: Int;
    public static var SCANCODE_LANG1: Int;
    public static var SCANCODE_LANG2: Int;
    public static var SCANCODE_LANG3: Int;
    public static var SCANCODE_LANG4: Int;
    public static var SCANCODE_LANG5: Int;
    public static var SCANCODE_LANG6: Int;
    public static var SCANCODE_LANG7: Int;
    public static var SCANCODE_LANG8: Int;
    public static var SCANCODE_LANG9: Int;
    public static var SCANCODE_ALTERASE: Int;
    public static var SCANCODE_SYSREQ: Int;
    public static var SCANCODE_CANCEL: Int;
    public static var SCANCODE_CLEAR: Int;
    public static var SCANCODE_PRIOR: Int;
    public static var SCANCODE_RETURN2: Int;
    public static var SCANCODE_SEPARATOR: Int;
    public static var SCANCODE_OUT: Int;
    public static var SCANCODE_OPER: Int;
    public static var SCANCODE_CLEARAGAIN: Int;
    public static var SCANCODE_CRSEL: Int;
    public static var SCANCODE_EXSEL: Int;
    public static var SCANCODE_KP_00: Int;
    public static var SCANCODE_KP_000: Int;
    public static var SCANCODE_THOUSANDSSEPARATOR: Int;
    public static var SCANCODE_DECIMALSEPARATOR: Int;
    public static var SCANCODE_CURRENCYUNIT: Int;
    public static var SCANCODE_CURRENCYSUBUNIT: Int;
    public static var SCANCODE_KP_LEFTPAREN: Int;
    public static var SCANCODE_KP_RIGHTPAREN: Int;
    public static var SCANCODE_KP_LEFTBRACE: Int;
    public static var SCANCODE_KP_RIGHTBRACE: Int;
    public static var SCANCODE_KP_TAB: Int;
    public static var SCANCODE_KP_BACKSPACE: Int;
    public static var SCANCODE_KP_A: Int;
    public static var SCANCODE_KP_B: Int;
    public static var SCANCODE_KP_C: Int;
    public static var SCANCODE_KP_D: Int;
    public static var SCANCODE_KP_E: Int;
    public static var SCANCODE_KP_F: Int;
    public static var SCANCODE_KP_XOR: Int;
    public static var SCANCODE_KP_POWER: Int;
    public static var SCANCODE_KP_PERCENT: Int;
    public static var SCANCODE_KP_LESS: Int;
    public static var SCANCODE_KP_GREATER: Int;
    public static var SCANCODE_KP_AMPERSAND: Int;
    public static var SCANCODE_KP_DBLAMPERSAND: Int;
    public static var SCANCODE_KP_VERTICALBAR: Int;
    public static var SCANCODE_KP_DBLVERTICALBAR: Int;
    public static var SCANCODE_KP_COLON: Int;
    public static var SCANCODE_KP_HASH: Int;
    public static var SCANCODE_KP_SPACE: Int;
    public static var SCANCODE_KP_AT: Int;
    public static var SCANCODE_KP_EXCLAM: Int;
    public static var SCANCODE_KP_MEMSTORE: Int;
    public static var SCANCODE_KP_MEMRECALL: Int;
    public static var SCANCODE_KP_MEMCLEAR: Int;
    public static var SCANCODE_KP_MEMADD: Int;
    public static var SCANCODE_KP_MEMSUBTRACT: Int;
    public static var SCANCODE_KP_MEMMULTIPLY: Int;
    public static var SCANCODE_KP_MEMDIVIDE: Int;
    public static var SCANCODE_KP_PLUSMINUS: Int;
    public static var SCANCODE_KP_CLEAR: Int;
    public static var SCANCODE_KP_CLEARENTRY: Int;
    public static var SCANCODE_KP_BINARY: Int;
    public static var SCANCODE_KP_OCTAL: Int;
    public static var SCANCODE_KP_DECIMAL: Int;
    public static var SCANCODE_KP_HEXADECIMAL: Int;
    public static var SCANCODE_LCTRL: Int;
    public static var SCANCODE_LSHIFT: Int;
    public static var SCANCODE_LALT: Int;
    public static var SCANCODE_LGUI: Int;
    public static var SCANCODE_RCTRL: Int;
    public static var SCANCODE_RSHIFT: Int;
    public static var SCANCODE_RALT: Int;
    public static var SCANCODE_RGUI: Int;
    public static var SCANCODE_MODE: Int;
    public static var SCANCODE_AUDIONEXT: Int;
    public static var SCANCODE_AUDIOPREV: Int;
    public static var SCANCODE_AUDIOSTOP: Int;
    public static var SCANCODE_AUDIOPLAY: Int;
    public static var SCANCODE_AUDIOMUTE: Int;
    public static var SCANCODE_MEDIASELECT: Int;
    public static var SCANCODE_WWW: Int;
    public static var SCANCODE_MAIL: Int;
    public static var SCANCODE_CALCULATOR: Int;
    public static var SCANCODE_COMPUTER: Int;
    public static var SCANCODE_AC_SEARCH: Int;
    public static var SCANCODE_AC_HOME: Int;
    public static var SCANCODE_AC_BACK: Int;
    public static var SCANCODE_AC_FORWARD: Int;
    public static var SCANCODE_AC_STOP: Int;
    public static var SCANCODE_AC_REFRESH: Int;
    public static var SCANCODE_AC_BOOKMARKS: Int;
    public static var SCANCODE_BRIGHTNESSDOWN: Int;
    public static var SCANCODE_BRIGHTNESSUP: Int;
    public static var SCANCODE_DISPLAYSWITCH: Int;
    public static var SCANCODE_KBDILLUMTOGGLE: Int;
    public static var SCANCODE_KBDILLUMDOWN: Int;
    public static var SCANCODE_KBDILLUMUP: Int;
    public static var SCANCODE_EJECT: Int;
    public static var SCANCODE_SLEEP: Int;
    public static var SCANCODE_APP1: Int;
    public static var SCANCODE_APP2: Int;
    public static var HAT_CENTER: Int;
    public static var HAT_UP: Int;
    public static var HAT_RIGHT: Int;
    public static var HAT_DOWN: Int;
    public static var HAT_LEFT: Int;
    public static var CONTROLLER_BUTTON_A: Int;
    public static var CONTROLLER_BUTTON_B: Int;
    public static var CONTROLLER_BUTTON_X: Int;
    public static var CONTROLLER_BUTTON_Y: Int;
    public static var CONTROLLER_BUTTON_BACK: Int;
    public static var CONTROLLER_BUTTON_GUIDE: Int;
    public static var CONTROLLER_BUTTON_START: Int;
    public static var CONTROLLER_BUTTON_LEFTSTICK: Int;
    public static var CONTROLLER_BUTTON_RIGHTSTICK: Int;
    public static var CONTROLLER_BUTTON_LEFTSHOULDER: Int;
    public static var CONTROLLER_BUTTON_RIGHTSHOULDER: Int;
    public static var CONTROLLER_BUTTON_DPAD_UP: Int;
    public static var CONTROLLER_BUTTON_DPAD_DOWN: Int;
    public static var CONTROLLER_BUTTON_DPAD_LEFT: Int;
    public static var CONTROLLER_BUTTON_DPAD_RIGHT: Int;
    public static var CONTROLLER_AXIS_LEFTX: Int;
    public static var CONTROLLER_AXIS_LEFTY: Int;
    public static var CONTROLLER_AXIS_RIGHTX: Int;
    public static var CONTROLLER_AXIS_RIGHTY: Int;
    public static var CONTROLLER_AXIS_TRIGGERLEFT: Int;
    public static var CONTROLLER_AXIS_TRIGGERRIGHT: Int;


    public static var UI_VERTEX_SIZE: Int;


    public static var COLOR_LUT_SIZE: Int;
    public static var PRIORITY_LAST: Int;


    public static var MSG_IDENTITY: Int;
    public static var MSG_CONTROLS: Int;
    public static var MSG_SCENELOADED: Int;
    public static var MSG_REQUESTPACKAGE: Int;
    public static var MSG_PACKAGEDATA: Int;
    public static var MSG_LOADSCENE: Int;
    public static var MSG_SCENECHECKSUMERROR: Int;
    public static var MSG_CREATENODE: Int;
    public static var MSG_NODEDELTAUPDATE: Int;
    public static var MSG_NODELATESTDATA: Int;
    public static var MSG_REMOVENODE: Int;
    public static var MSG_CREATECOMPONENT: Int;
    public static var MSG_COMPONENTDELTAUPDATE: Int;
    public static var MSG_COMPONENTLATESTDATA: Int;
    public static var MSG_REMOVECOMPONENT: Int;
    public static var MSG_REMOTEEVENT: Int;
    public static var MSG_REMOTENODEEVENT: Int;
    public static var MSG_PACKAGEINFO: Int;
    public static var CONTROLS_CONTENT_ID: Int;
    public static var PACKAGE_FRAGMENT_SIZE: Int;


    public static var SCAN_FILES: Int;
    public static var SCAN_DIRS: Int;
    public static var SCAN_HIDDEN: Int;
    public static var LOG_RAW: Int;
    public static var LOG_DEBUG: Int;
    public static var LOG_INFO: Int;
    public static var LOG_WARNING: Int;
    public static var LOG_ERROR: Int;
    public static var LOG_NONE: Int;


}

//----------------------------------------------------
// MODULE: Container
//----------------------------------------------------


@:native("Atomic.RefCounted")
extern class RefCounted {

      // Construct. Allocate the reference count structure and set an initial self weak reference.
    function new();

      // Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
    function addRef(): Void;
      // Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
    function releaseRef(): Void;
      // Return reference count.
    function refs(): Int;
      // Return weak reference count.
    function weakRefs(): Int;
    function isObject(): Bool;

}



//----------------------------------------------------
// MODULE: Math
//----------------------------------------------------


typedef BoundingBox = Array<Float>;
typedef Color = Array<Float>;
typedef Quaternion = Array<Float>;
typedef Rect = Array<Float>;
typedef IntRect = Array<Float>;
typedef Vector2 = Array<Float>;
typedef IntVector2 = Array<Float>;
typedef Vector3 = Array<Float>;
typedef Vector4 = Array<Float>;


//----------------------------------------------------
// MODULE: Core
//----------------------------------------------------


@:native("Atomic.Context")
extern class Context extends RefCounted {

    var eventSender: AObject;
    var editorContext: Bool;

      // Construct.
    function new();

      // Register a subsystem.
    function registerSubsystem(subsystem: AObject): Void;
      // Remove a subsystem.
    function removeSubsystem(objectType: String): Void;
      // Copy base class attributes to derived class.
    function copyBaseAttributes(baseType: String, derivedType: String): Void;
      // Return subsystem by type.
    function getSubsystem(type: String): AObject;
      // Return active event sender. Null outside event handling.
    function getEventSender(): AObject;
      // Return object type name from hash, or empty if unknown.
    function getTypeName(objectType: String): String;
      // Get whether an Editor Context
    function getEditorContext(): Bool;
      // Get whether an Editor Context
    function setEditorContext(editor: Bool): Void;

}

@:native("Atomic.AObject")
extern class AObject extends RefCounted {

    var type: String;
    var baseType: String;
    var typeName: String;
    var context: Context;
    var eventSender: AObject;
    var category: String;
    var typeNameStatic: String;

      // Construct.
    function new();

      // Return type hash.
    function getType(): String;
      // Return base class type hash.
    function getBaseType(): String;
      // Return type name.
    function getTypeName(): String;
      // Unsubscribe from a specific sender's events.
    function unsubscribeFromEvents(sender: AObject): Void;
      // Unsubscribe from all events.
    function unsubscribeFromAllEvents(): Void;
      // Return execution context.
    function getContext(): Context;
      // Return subsystem by type.
    function getSubsystem(type: String): AObject;
      // Return active event sender. Null outside event handling.
    function getEventSender(): AObject;
      // Return whether has subscribed to any event.
    function hasEventHandlers(): Bool;
      // Return object category. Categories are (optionally) registered along with the object factory. Return an empty string if the object category is not registered.
    function getCategory(): String;
    @:overload(function(): Bool{})
    override function isObject(): Bool;
    function getTypeNameStatic(): String;

}



//----------------------------------------------------
// MODULE: Scene
//----------------------------------------------------


@:native("Atomic.Animatable")
extern class Animatable extends Serializable {

    var animationEnabled: Bool;
    var objectAnimation: ObjectAnimation;

      // Construct.
    function new();

      // Set animation enabled.
    function setAnimationEnabled(enable: Bool): Void;
      // Set object animation.
    function setObjectAnimation(objectAnimation: ObjectAnimation): Void;
      // Set attribute animation.
    function setAttributeAnimation(name: String, attributeAnimation: ValueAnimation, ?wrapMode: WrapMode, ?speed: Float): Void;
      // Set attribute animation wrap mode.
    function setAttributeAnimationWrapMode(name: String, wrapMode: WrapMode): Void;
      // Set attribute animation speed.
    function setAttributeAnimationSpeed(name: String, speed: Float): Void;
      // Return animation enabled.
    function getAnimationEnabled(): Bool;
      // Return object animation.
    function getObjectAnimation(): ObjectAnimation;
      // Return attribute animation.
    function getAttributeAnimation(name: String): ValueAnimation;
      // Return attribute animation wrap mode.
    function getAttributeAnimationWrapMode(name: String): WrapMode;
      // Return attribute animation speed.
    function getAttributeAnimationSpeed(name: String): Float;

}

@:native("Atomic.Component")
extern class Component extends Animatable {

    var enabled: Bool;
    var id: Int;
    var node: Node;
    var scene: Scene;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    function onSetEnabled(): Void;
      // Mark for attribute check on the next network update.
    @:overload(function(): Void{})
    override function markNetworkUpdate(): Void;
      // Visualize the component as debug geometry.
    function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set enabled/disabled state.
    function setEnabled(enable: Bool): Void;
      // Remove from the scene node. If no other shared pointer references exist, causes immediate deletion.
    function remove(): Void;
      // Return ID.
    function getID(): Int;
      // Return scene node.
    function getNode(): Node;
      // Return the scene the node belongs to.
    function getScene(): Scene;
      // Return whether is enabled.
    function isEnabled(): Bool;
      // Return whether is effectively enabled (node is also enabled.)
    function isEnabledEffective(): Bool;
      // Return component in the same scene node by type. If there are several, returns the first.
    function getComponent(type: String): Component;
      // Prepare network update by comparing attributes and marking replication states dirty as necessary.
    function prepareNetworkUpdate(): Void;

}

@:native("Atomic.Node")
extern class Node extends Animatable {

    var name: String;
    var position: Vector3;
    var position2D: Vector2;
    var rotation: Quaternion;
    var rotation2D: Float;
    var direction: Vector3;
    var scale: Vector3;
    var scale2D: Vector2;
    var worldPosition: Vector3;
    var worldRotation: Quaternion;
    var worldRotation2D: Float;
    var worldDirection: Vector3;
    var enabled: Bool;
    var deepEnabled: Bool;
    var enabledRecursive: Bool;
    var parent: Node;
    var id: Int;
    var nameHash: String;
    var scene: Scene;
    var up: Vector3;
    var right: Vector3;
    var worldPosition2D: Vector2;
    var worldUp: Vector3;
    var worldRight: Vector3;
    var worldScale: Vector3;
    var worldScale2D: Vector2;
    var numComponents: Int;
    var numNetworkComponents: Int;
    var netPositionAttr: Vector3;
    var numPersistentChildren: Int;
    var numPersistentComponents: Int;
    var positionSilent: Vector3;
    var rotationSilent: Quaternion;
    var scaleSilent: Vector3;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately recursively to child nodes and components.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Return whether should save default-valued attributes into XML. Always save node transforms for readability, even if identity.
    @:overload(function(): Bool{})
    override function saveDefaultAttributes(): Bool;
      // Mark for attribute check on the next network update.
    @:overload(function(): Void{})
    override function markNetworkUpdate(): Void;
      // Set name of the scene node. Names are not required to be unique.
    function setName(name: String): Void;
      // Set position in parent space. If the scene node is on the root level (is child of the scene itself), this is same as world space.
    function setPosition(position: Vector3): Void;
      // Set position in parent space (for Atomic2D).
    function setPosition2D(position: Vector2): Void;
      // Set rotation in parent space.
    function setRotation(rotation: Quaternion): Void;
      // Set rotation in parent space (for Atomic2D).
    function setRotation2D(rotation: Float): Void;
      // Set forward direction in parent space. Positive Z axis equals identity rotation.
    function setDirection(direction: Vector3): Void;
      // Set scale in parent space.
    function setScale(scale: Vector3): Void;
      // Set scale in parent space (for Atomic2D).
    function setScale2D(scale: Vector2): Void;
      // Set position in world space.
    function setWorldPosition(position: Vector3): Void;
      // Set rotation in world space.
    function setWorldRotation(rotation: Quaternion): Void;
      // Set rotation in world space (for Atomic2D).
    function setWorldRotation2D(rotation: Float): Void;
      // Set forward direction in world space.
    function setWorldDirection(direction: Vector3): Void;
      // Move the scene node in the chosen transform space.
    function translate(delta: Vector3, ?space: TransformSpace): Void;
      // Move the scene node in the chosen transform space (for Atomic2D).
    function translate2D(delta: Vector2, ?space: TransformSpace): Void;
      // Rotate the scene node in the chosen transform space.
    function rotate(delta: Quaternion, ?space: TransformSpace): Void;
      // Rotate the scene node in the chosen transform space (for Atomic2D).
    function rotate2D(delta: Float, ?space: TransformSpace): Void;
      // Rotate around a point in the chosen transform space.
    function rotateAround(point: Vector3, delta: Quaternion, ?space: TransformSpace): Void;
      // Rotate around a point in the chosen transform space (for Atomic2D).
    function rotateAround2D(point: Vector2, delta: Float, ?space: TransformSpace): Void;
      // Rotate around the X axis.
    function pitch(angle: Float, ?space: TransformSpace): Void;
      // Rotate around the Y axis.
    function yaw(angle: Float, ?space: TransformSpace): Void;
      // Rotate around the Z axis.
    function roll(angle: Float, ?space: TransformSpace): Void;
      // Look at a target position in the chosen transform space. Note that the up vector is always specified in world space. Return true if successful, or false if resulted in an illegal rotation, in which case the current rotation remains.
    function lookAt(target: Vector3, ?up: Vector3, ?space: TransformSpace): Bool;
      // Set enabled/disabled state without recursion. Components in a disabled node become effectively disabled regardless of their own enable/disable state.
    function setEnabled(enable: Bool): Void;
      // Set enabled state on self and child nodes. Nodes' own enabled state is remembered (IsEnabledSelf) and can be restored.
    function setDeepEnabled(enable: Bool): Void;
      // Reset enabled state to the node's remembered state prior to calling SetDeepEnabled.
    function resetDeepEnabled(): Void;
      // Set enabled state on self and child nodes. Unlike SetDeepEnabled this does not remember the nodes' own enabled state, but overwrites it.
    function setEnabledRecursive(enable: Bool): Void;
      // Mark node and child nodes to need world transform recalculation. Notify listener components.
    function markDirty(): Void;
      // Create a child scene node (with specified ID if provided).
    function createChild(?name: String, ?mode: CreateMode, ?id: Int): Node;
      // Add a child scene node at a specific index. If index is not explicitly specified or is greater than current children size, append the new child at the end.
    function addChild(node: Node, ?index: Int): Void;
      // Remove a child scene node.
    function removeChild(node: Node): Void;
      // Remove all child scene nodes.
    function removeAllChildren(): Void;
      // Remove child scene nodes that match criteria.
    function removeChildren(removeReplicated: Bool, removeLocal: Bool, recursive: Bool): Void;
      // Create a component to this node (with specified ID if provided).
    function createComponent(type: String, ?mode: CreateMode, ?id: Int): Component;
      // Create a component to this node if it does not exist already.
    function getOrCreateComponent(type: String, ?mode: CreateMode, ?id: Int): Component;
      // Remove all components from this node.
    function removeAllComponents(): Void;
      // Remove components that match criteria.
    function removeComponents(removeReplicated: Bool, removeLocal: Bool): Void;
      // Clone scene node, components and child nodes. Return the clone.
    function clone(?mode: CreateMode): Node;
      // Remove from the parent node. If no other shared pointer references exist, causes immediate deletion.
    function remove(): Void;
      // Set parent scene node. Retains the world transform.
    function setParent(parent: Node): Void;
      // Add listener component that is notified of node being dirtied. Can either be in the same node or another.
    function addListener(component: Component): Void;
      // Remove listener component.
    function removeListener(component: Component): Void;
      // Return ID.
    function getID(): Int;
      // Return name.
    function getName(): String;
      // Return name hash.
    function getNameHash(): String;
      // Return parent scene node.
    function getParent(): Node;
      // Return scene.
    function getScene(): Scene;
      // Return whether is enabled. Disables nodes effectively disable all their components.
    function isEnabled(): Bool;
      // Returns the node's last own enabled state. May be different than the value returned by IsEnabled when SetDeepEnabled has been used.
    function isEnabledSelf(): Bool;
      // Return position in parent space.
    function getPosition(): Vector3;
      // Return position in parent space (for Atomic2D).
    function getPosition2D(): Vector2;
      // Return rotation in parent space.
    function getRotation(): Quaternion;
      // Return rotation in parent space (for Atomic2D).
    function getRotation2D(): Float;
      // Return forward direction in parent space. Positive Z axis equals identity rotation.
    function getDirection(): Vector3;
      // Return up direction in parent space. Positive Y axis equals identity rotation.
    function getUp(): Vector3;
      // Return right direction in parent space. Positive X axis equals identity rotation.
    function getRight(): Vector3;
      // Return scale in parent space.
    function getScale(): Vector3;
      // Return scale in parent space (for Atomic2D).
    function getScale2D(): Vector2;
      // Return position in world space.
    function getWorldPosition(): Vector3;
      // Return position in world space (for Atomic2D).
    function getWorldPosition2D(): Vector2;
      // Return rotation in world space.
    function getWorldRotation(): Quaternion;
      // Return rotation in world space (for Atomic2D).
    function getWorldRotation2D(): Float;
      // Return direction in world space.
    function getWorldDirection(): Vector3;
      // Return node's up vector in world space.
    function getWorldUp(): Vector3;
      // Return node's right vector in world space.
    function getWorldRight(): Vector3;
      // Return scale in world space.
    function getWorldScale(): Vector3;
      // Return scale in world space (for Atomic2D).
    function getWorldScale2D(): Vector2;
      // Convert a local space position or rotation to world space (for Atomic2D).
    function localToWorld2D(vector: Vector2): Vector2;
      // Convert a world space position or rotation to local space (for Atomic2D).
    function worldToLocal2D(vector: Vector2): Vector2;
      // Return whether transform has changed and world transform needs recalculation.
    function isDirty(): Bool;
      // Return number of child scene nodes.
    function getNumChildren(?recursive: Bool): Int;
      // Return child scene node by name.
    function getChild(name: String, ?recursive: Bool): Node;
      // Return number of components.
    function getNumComponents(): Int;
      // Return number of non-local components.
    function getNumNetworkComponents(): Int;
      // Return component by type. If there are several, returns the first.
    function getComponent(type: String): Component;
      // Return whether has a specific component.
    function hasComponent(type: String): Bool;
      // Set ID. Called by Scene.
    function setID(id: Int): Void;
      // Set scene. Called by Scene.
    function setScene(scene: Scene): Void;
      // Reset scene, ID and owner. Called by Scene.
    function resetScene(): Void;
      // Set network position attribute.
    function setNetPositionAttr(value: Vector3): Void;
      // Return network position attribute.
    function getNetPositionAttr(): Vector3;
      // Prepare network update by comparing attributes and marking replication states dirty as necessary.
    function prepareNetworkUpdate(): Void;
      // Mark node dirty in scene replication states.
    function markReplicationDirty(): Void;
      // Add a pre-created component.
    function addComponent(component: Component, id: Int, mode: CreateMode): Void;
      // Calculate number of non-temporary child nodes.
    function getNumPersistentChildren(): Int;
      // Calculate number of non-temporary components.
    function getNumPersistentComponents(): Int;
      // Set position in parent space silently without marking the node & child nodes dirty. Used by animation code.
    function setPositionSilent(position: Vector3): Void;
      // Set position in parent space silently without marking the node & child nodes dirty. Used by animation code.
    function setRotationSilent(rotation: Quaternion): Void;
      // Set scale in parent space silently without marking the node & child nodes dirty. Used by animation code.
    function setScaleSilent(scale: Vector3): Void;
      // Set local transform silently without marking the node & child nodes dirty. Used by animation code.
    function setTransformSilent(position: Vector3, rotation: Quaternion, scale: Vector3): Void;

}

@:native("Atomic.ObjectAnimation")
extern class ObjectAnimation extends Resource {

      // Construct.
    function new();

      // Add attribute animation, attribute name can in following format: "attribute" or "#0/#1/attribute" or ""#0/#1/@component#1/attribute.
    function addAttributeAnimation(name: String, attributeAnimation: ValueAnimation, ?wrapMode: WrapMode, ?speed: Float): Void;
      // Return attribute animation by name.
    function getAttributeAnimation(name: String): ValueAnimation;
      // Return attribute animation wrap mode by name.
    function getAttributeAnimationWrapMode(name: String): WrapMode;
      // Return attribute animation speed by name.
    function getAttributeAnimationSpeed(name: String): Float;
      // Return attribute animation info by name.
    function getAttributeAnimationInfo(name: String): ValueAnimationInfo;

}

@:native("Atomic.PrefabComponent")
extern class PrefabComponent extends Component {

    var prefabGUID: String;

      // Construct.
    function new();

    function setPrefabGUID(guid: String): Void;
    function getPrefabGUID(): String;
    function savePrefab(): Bool;
    function undoPrefab(): Void;
    function breakPrefab(): Void;

}

@:native("Atomic.Scene")
extern class Scene extends Node {

    var updateEnabled: Bool;
    var timeScale: Float;
    var elapsedTime: Float;
    var smoothingConstant: Float;
    var snapThreshold: Float;
    var asyncLoadingMs: Int;
    var asyncProgress: Float;
    var asyncLoadMode: LoadMode;
    var fileName: String;
    var checksum: Int;
    var varNamesAttr: String;

      // Construct.
    function new();

      // Load from a binary file asynchronously. Return true if started successfully. The LOAD_RESOURCES_ONLY mode can also be used to preload resources from object prefab files.
    function loadAsync(file: File, ?mode: LoadMode): Bool;
      // Load from an XML file asynchronously. Return true if started successfully. The LOAD_RESOURCES_ONLY mode can also be used to preload resources from object prefab files.
    function loadAsyncXML(file: File, ?mode: LoadMode): Bool;
      // Stop asynchronous loading.
    function stopAsyncLoading(): Void;
      // Clear scene completely of either replicated, local or all nodes and components.
    function clear(?clearReplicated: Bool, ?clearLocal: Bool): Void;
      // Enable or disable scene update.
    function setUpdateEnabled(enable: Bool): Void;
      // Set update time scale. 1.0 = real time (default.)
    function setTimeScale(scale: Float): Void;
      // Set elapsed time in seconds. This can be used to prevent inaccuracy in the timer if the scene runs for a long time.
    function setElapsedTime(time: Float): Void;
      // Set network client motion smoothing constant.
    function setSmoothingConstant(constant: Float): Void;
      // Set network client motion smoothing snap threshold.
    function setSnapThreshold(threshold: Float): Void;
      // Set maximum milliseconds per frame to spend on async scene loading.
    function setAsyncLoadingMs(ms: Int): Void;
      // Clear required package files.
    function clearRequiredPackageFiles(): Void;
      // Register a node user variable hash reverse mapping (for editing.)
    function registerVar(name: String): Void;
      // Unregister a node user variable hash reverse mapping.
    function unregisterVar(name: String): Void;
      // Clear all registered node user variable hash reverse mappings.
    function unregisterAllVars(): Void;
      // Return node from the whole scene by ID, or null if not found.
    function getNode(id: Int): Node;
      // Return whether updates are enabled.
    function isUpdateEnabled(): Bool;
      // Return whether an asynchronous loading operation is in progress.
    function isAsyncLoading(): Bool;
      // Return asynchronous loading progress between 0.0 and 1.0, or 1.0 if not in progress.
    function getAsyncProgress(): Float;
      // Return the load mode of the current asynchronous loading operation.
    function getAsyncLoadMode(): LoadMode;
      // Return source file name.
    function getFileName(): String;
      // Return source file checksum.
    function getChecksum(): Int;
      // Return update time scale.
    function getTimeScale(): Float;
      // Return elapsed time in seconds.
    function getElapsedTime(): Float;
      // Return motion smoothing constant.
    function getSmoothingConstant(): Float;
      // Return motion smoothing snap threshold.
    function getSnapThreshold(): Float;
      // Return maximum milliseconds per frame to spend on async loading.
    function getAsyncLoadingMs(): Int;
      // Return a node user variable name, or empty if not registered.
    function getVarName(hash: String): String;
      // Update scene. Called by HandleUpdate.
    function update(timeStep: Float): Void;
      // Begin a threaded update. During threaded update components can choose to delay dirty processing.
    function beginThreadedUpdate(): Void;
      // End a threaded update. Notify components that marked themselves for delayed dirty processing.
    function endThreadedUpdate(): Void;
      // Add a component to the delayed dirty notify queue. Is thread-safe.
    function delayedMarkedDirty(component: Component): Void;
      // Return threaded update flag.
    function isThreadedUpdate(): Bool;
      // Get free node ID, either non-local or local.
    function getFreeNodeID(mode: CreateMode): Int;
      // Get free component ID, either non-local or local.
    function getFreeComponentID(mode: CreateMode): Int;
      // Node added. Assign scene pointer and add to ID map.
    function nodeAdded(node: Node): Void;
      // Node removed. Remove from ID map.
    function nodeRemoved(node: Node): Void;
      // Component added. Add to ID map.
    function componentAdded(component: Component): Void;
      // Component removed. Remove from ID map.
    function componentRemoved(component: Component): Void;
      // Set node user variable reverse mappings.
    function setVarNamesAttr(value: String): Void;
      // Return node user variable reverse mappings.
    function getVarNamesAttr(): String;
      // Prepare network update by comparing attributes and marking replication states dirty as necessary.
    @:overload(function(): Void{})
    override function prepareNetworkUpdate(): Void;

}

@:native("Atomic.Serializable")
extern class Serializable extends AObject {

    var temporary: Bool;
    var numAttributes: Int;
    var numNetworkAttributes: Int;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    function applyAttributes(): Void;
      // Return whether should save default-valued attributes into XML. Default false.
    function saveDefaultAttributes(): Bool;
      // Mark for attribute check on the next network update.
    function markNetworkUpdate(): Void;
      // Reset all editable attributes to their default values.
    function resetToDefault(): Void;
      // Remove instance's default values if they are set previously.
    function removeInstanceDefault(): Void;
      // Set temporary flag. Temporary objects will not be saved.
    function setTemporary(enable: Bool): Void;
      // Enable interception of an attribute from network updates. Intercepted attributes are sent as events instead of applying directly. This can be used to implement client side prediction.
    function setInterceptNetworkUpdate(attributeName: String, enable: Bool): Void;
      // Allocate network attribute state.
    function allocateNetworkState(): Void;
      // Return number of attributes.
    function getNumAttributes(): Int;
      // Return number of network replication attributes.
    function getNumNetworkAttributes(): Int;
      // Return whether is temporary.
    function isTemporary(): Bool;
      // Return whether an attribute's network updates are being intercepted.
    function getInterceptNetworkUpdate(attributeName: String): Bool;

}

@:native("Atomic.SmoothedTransform")
extern class SmoothedTransform extends Component {

    var targetPosition: Vector3;
    var targetRotation: Quaternion;
    var targetWorldPosition: Vector3;
    var targetWorldRotation: Quaternion;

      // Construct.
    function new();

      // Update smoothing.
    function update(constant: Float, squaredSnapThreshold: Float): Void;
      // Set target position in parent space.
    function setTargetPosition(position: Vector3): Void;
      // Set target rotation in parent space.
    function setTargetRotation(rotation: Quaternion): Void;
      // Set target position in world space.
    function setTargetWorldPosition(position: Vector3): Void;
      // Set target rotation in world space.
    function setTargetWorldRotation(rotation: Quaternion): Void;
      // Return target position in parent space.
    function getTargetPosition(): Vector3;
      // Return target rotation in parent space.
    function getTargetRotation(): Quaternion;
      // Return target position in world space.
    function getTargetWorldPosition(): Vector3;
      // Return target rotation in world space.
    function getTargetWorldRotation(): Quaternion;
      // Return whether smoothing is in progress.
    function isInProgress(): Bool;

}

@:native("Atomic.SplinePath")
extern class SplinePath extends Component {

    var interpolationMode: InterpolationMode;
    var speed: Float;
    var position: Vector3;
    var controlledNode: Node;
    var controlledIdAttr: Int;

      // Construct an Empty SplinePath.
    function new();

      // Apply Attributes to the SplinePath.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Draw the Debug Geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Add a Node to the SplinePath as a Control Point.
    function addControlPoint(point: Node, ?index: Int): Void;
      // Remove a Node Control Point from the SplinePath.
    function removeControlPoint(point: Node): Void;
      // Clear the Control Points from the SplinePath.
    function clearControlPoints(): Void;
      // Set the Interpolation Mode.
    function setInterpolationMode(interpolationMode: InterpolationMode): Void;
      // Set the movement Speed.
    function setSpeed(speed: Float): Void;
      // Set the controlled Node's position on the SplinePath.
    function setPosition(factor: Float): Void;
      // Set the Node to be moved along the SplinePath.
    function setControlledNode(controlled: Node): Void;
      // Get the Interpolation Mode.
    function getInterpolationMode(): InterpolationMode;
      // Get the movement Speed.
    function getSpeed(): Float;
      // Get the parent Node's last position on the spline.
    function getPosition(): Vector3;
      // Get the controlled Node.
    function getControlledNode(): Node;
      // Get a point on the SplinePath from 0.f to 1.f where 0 is the start and 1 is the end.
    function getPoint(factor: Float): Vector3;
      // Move the controlled Node to the next position along the SplinePath based off the Speed value.
    function move(timeStep: Float): Void;
      // Reset movement along the path.
    function reset(): Void;
      // Returns whether the movement along the SplinePath is complete.
    function isFinished(): Bool;
      // Set Controlled Node ID attribute.
    function setControlledIdAttr(value: Int): Void;
      // Get Controlled Node ID attribute.
    function getControlledIdAttr(): Int;

}

@:native("Atomic.ValueAnimation")
extern class ValueAnimation extends Resource {

    var interpolationMethod: InterpMethod;
    var splineTension: Float;
    var valueType: VariantType;
    var beginTime: Float;
    var endTime: Float;

      // Construct.
    function new();

      // Set interpolation method.
    function setInterpolationMethod(method: InterpMethod): Void;
      // Set spline tension, should be between 0.0f and 1.0f, but this is not a must.
    function setSplineTension(tension: Float): Void;
      // Set value type.
    function setValueType(valueType: VariantType): Void;
      // Return animation is valid.
    function isValid(): Bool;
      // Return interpolation method.
    function getInterpolationMethod(): InterpMethod;
      // Return spline tension.
    function getSplineTension(): Float;
      // Return value type.
    function getValueType(): VariantType;
      // Return begin time.
    function getBeginTime(): Float;
      // Return end time.
    function getEndTime(): Float;
      // Has event frames.
    function hasEventFrames(): Bool;

}

@:native("Atomic.ValueAnimationInfo")
extern class ValueAnimationInfo extends RefCounted {

    var wrapMode: WrapMode;
    var speed: Float;
    var target: AObject;
    var animation: ValueAnimation;

      // Construct without target object.
    function new(animation: ValueAnimation, wrapMode: WrapMode, speed: Float);

      // Update. Return true when the animation is finished. No-op when the target object is not defined.
    function update(timeStep: Float): Bool;
      // Set wrap mode.
    function setWrapMode(wrapMode: WrapMode): Void;
      // Set speed.
    function setSpeed(speed: Float): Void;
      // Return target object.
    function getTarget(): AObject;
      // Return animation.
    function getAnimation(): ValueAnimation;
      // Return wrap mode.
    function getWrapMode(): WrapMode;
      // Return speed.
    function getSpeed(): Float;

}



//----------------------------------------------------
// MODULE: Graphics
//----------------------------------------------------


@:native("Atomic.Camera")
extern class Camera extends Component {

    var nearClip: Float;
    var farClip: Float;
    var fov: Float;
    var orthoSize: Float;
    var aspectRatio: Float;
    var fillMode: FillMode;
    var zoom: Float;
    var lodBias: Float;
    var viewMask: Int;
    var viewOverrideFlags: Int;
    var orthographic: Bool;
    var autoAspectRatio: Bool;
    var projectionOffset: Vector2;
    var useReflection: Bool;
    var useClipping: Bool;
    var flipVertical: Bool;
    var halfViewSize: Float;
    var reverseCulling: Bool;
    var aspectRatioInternal: Float;
    var orthoSizeAttr: Float;
    var reflectionPlaneAttr: Vector4;
    var clipPlaneAttr: Vector4;

      // Construct.
    function new();

      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set near clip distance.
    function setNearClip(nearClip: Float): Void;
      // Set far clip distance.
    function setFarClip(farClip: Float): Void;
      // Set vertical field of view in degrees.
    function setFov(fov: Float): Void;
      // Set orthographic mode view uniform size.
    function setOrthoSize(orthoSize: Float): Void;
      // Set aspect ratio manually. Disables the auto aspect ratio -mode.
    function setAspectRatio(aspectRatio: Float): Void;
      // Set polygon fill mode to use when rendering a scene.
    function setFillMode(mode: FillMode): Void;
      // Set zoom.
    function setZoom(zoom: Float): Void;
      // Set LOD bias.
    function setLodBias(bias: Float): Void;
      // Set view mask. Will be and'ed with object's view mask to see if the object should be rendered.
    function setViewMask(mask: Int): Void;
      // Set view override flags.
    function setViewOverrideFlags(flags: Int): Void;
      // Set orthographic mode enabled/disabled.
    function setOrthographic(enable: Bool): Void;
      // Set automatic aspect ratio based on viewport dimensions. Enabled by default.
    function setAutoAspectRatio(enable: Bool): Void;
      // Set projection offset. It needs to be calculated as (offset in pixels) / (viewport dimensions.)
    function setProjectionOffset(offset: Vector2): Void;
      // Set reflection mode.
    function setUseReflection(enable: Bool): Void;
      // Set whether to use a custom clip plane.
    function setUseClipping(enable: Bool): Void;
      // Set vertical flipping mode. Called internally by View to resolve OpenGL / Direct3D9 rendertarget sampling differences.
    function setFlipVertical(enable: Bool): Void;
      // Return far clip distance.
    function getFarClip(): Float;
      // Return near clip distance.
    function getNearClip(): Float;
      // Return vertical field of view in degrees.
    function getFov(): Float;
      // Return orthographic mode size.
    function getOrthoSize(): Float;
      // Return aspect ratio.
    function getAspectRatio(): Float;
      // Return zoom.
    function getZoom(): Float;
      // Return LOD bias.
    function getLodBias(): Float;
      // Return view mask.
    function getViewMask(): Int;
      // Return view override flags.
    function getViewOverrideFlags(): Int;
      // Return fill mode.
    function getFillMode(): FillMode;
      // Return orthographic flag.
    function isOrthographic(): Bool;
      // Return auto aspect ratio flag.
    function getAutoAspectRatio(): Bool;
      // Return frustum near and far sizes.
    function getFrustumSize(near: Vector3, far: Vector3): Void;
      // Return half view size.
    function getHalfViewSize(): Float;
    function worldToScreenPoint(worldPos: Vector3): Vector2;
    function screenToWorldPoint(screenPos: Vector3): Vector3;
      // Return projection offset.
    function getProjectionOffset(): Vector2;
      // Return whether is using reflection.
    function getUseReflection(): Bool;
      // Return whether is using a custom clipping plane.
    function getUseClipping(): Bool;
      // Return vertical flipping mode.
    function getFlipVertical(): Bool;
      // Return whether to reverse culling; affected by vertical flipping and reflection.
    function getReverseCulling(): Bool;
      // Return distance to position. In orthographic mode uses only Z coordinate.
    function getDistance(worldPos: Vector3): Float;
      // Return squared distance to position. In orthographic mode uses only Z coordinate.
    function getDistanceSquared(worldPos: Vector3): Float;
      // Return a scene node's LOD scaled distance.
    function getLodDistance(distance: Float, scale: Float, bias: Float): Float;
      // Return a world rotation for facing a camera on certain axes based on the existing world rotation.
    function getFaceCameraRotation(position: Vector3, rotation: Quaternion, mode: FaceCameraMode): Quaternion;
      // Return if projection parameters are valid for rendering and raycasting.
    function isProjectionValid(): Bool;
      // Set aspect ratio without disabling the "auto aspect ratio" mode. Called internally by View.
    function setAspectRatioInternal(aspectRatio: Float): Void;
      // Set orthographic size attribute without forcing the aspect ratio.
    function setOrthoSizeAttr(orthoSize: Float): Void;
      // Set reflection plane attribute.
    function setReflectionPlaneAttr(value: Vector4): Void;
      // Return reflection plane attribute.
    function getReflectionPlaneAttr(): Vector4;
      // Set clipping plane attribute.
    function setClipPlaneAttr(value: Vector4): Void;
      // Return clipping plane attribute.
    function getClipPlaneAttr(): Vector4;

}

@:native("Atomic.DebugRenderer")
extern class DebugRenderer extends Component {

    var view: Camera;

      // Construct.
    function new();

      // Set the camera viewpoint. Call before rendering, or before adding geometry if you want to use culling.
    function setView(camera: Camera): Void;
      // Add a scene node represented as its coordinate axes.
    function addNode(node: Node, ?scale: Float, ?depthTest: Bool): Void;
      // Add a bounding box.
    function addBoundingBox(box: BoundingBox, color: Color, ?depthTest: Bool): Void;
      // Add a cylinder
    function addCylinder(position: Vector3, radius: Float, height: Float, color: Color, ?depthTest: Bool): Void;
      // Update vertex buffer and render all debug lines. The viewport and rendertarget should be set before.
    function render(): Void;
      // Check whether a bounding box is inside the view frustum.
    function isInside(box: BoundingBox): Bool;
      // Return whether has something to render.
    function hasContent(): Bool;

}

@:native("Atomic.Drawable")
extern class Drawable extends Component {

    var updateGeometryType: UpdateGeometryType;
    var numOccluderTriangles: Int;
    var drawDistance: Float;
    var shadowDistance: Float;
    var lodBias: Float;
    var viewMask: Int;
    var lightMask: Int;
    var shadowMask: Int;
    var zoneMask: Int;
    var maxLights: Int;
    var castShadows: Bool;
    var occluder: Bool;
    var occludee: Bool;
    var boundingBox: BoundingBox;
    var worldBoundingBox: BoundingBox;
    var drawableFlags: Dynamic;
    var sortValue: Float;
    var basePass: Int;
    var zone: Zone;
    var distance: Float;
    var lodDistance: Float;
    var firstLight: Light;
    var minZ: Float;
    var maxZ: Float;

      // Construct.
    function new(drawableFlags: Dynamic);

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
    function getUpdateGeometryType(): UpdateGeometryType;
      // Return number of occlusion geometry triangles.
    function getNumOccluderTriangles(): Int;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set draw distance.
    function setDrawDistance(distance: Float): Void;
      // Set shadow draw distance.
    function setShadowDistance(distance: Float): Void;
      // Set LOD bias.
    function setLodBias(bias: Float): Void;
      // Set view mask. Is and'ed with camera's view mask to see if the object should be rendered.
    function setViewMask(mask: Int): Void;
      // Set light mask. Is and'ed with light's and zone's light mask to see if the object should be lit.
    function setLightMask(mask: Int): Void;
      // Set shadow mask. Is and'ed with light's light mask and zone's shadow mask to see if the object should be rendered to a shadow map.
    function setShadowMask(mask: Int): Void;
      // Set zone mask. Is and'ed with zone's zone mask to see if the object should belong to the zone.
    function setZoneMask(mask: Int): Void;
      // Set maximum number of per-pixel lights. Default 0 is unlimited.
    function setMaxLights(num: Int): Void;
      // Set shadowcaster flag.
    function setCastShadows(enable: Bool): Void;
      // Set occlusion flag.
    function setOccluder(enable: Bool): Void;
      // Set occludee flag.
    function setOccludee(enable: Bool): Void;
      // Mark for update and octree reinsertion. Update is automatically queued when the drawable's scene node moves or changes scale.
    function markForUpdate(): Void;
      // Return local space bounding box. May not be applicable or properly updated on all drawables.
    function getBoundingBox(): BoundingBox;
      // Return world-space bounding box.
    function getWorldBoundingBox(): BoundingBox;
      // Return drawable flags.
    function getDrawableFlags(): Dynamic;
      // Return draw distance.
    function getDrawDistance(): Float;
      // Return shadow draw distance.
    function getShadowDistance(): Float;
      // Return LOD bias.
    function getLodBias(): Float;
      // Return view mask.
    function getViewMask(): Int;
      // Return light mask.
    function getLightMask(): Int;
      // Return shadow mask.
    function getShadowMask(): Int;
      // Return zone mask.
    function getZoneMask(): Int;
      // Return maximum number of per-pixel lights.
    function getMaxLights(): Int;
      // Return shadowcaster flag.
    function getCastShadows(): Bool;
      // Return occluder flag.
    function isOccluder(): Bool;
      // Return occludee flag.
    function isOccludee(): Bool;
      // Set new zone. Zone assignment may optionally be temporary, meaning it needs to be re-evaluated on the next frame.
    function setZone(zone: Zone, ?temporary: Bool): Void;
      // Set sorting value.
    function setSortValue(value: Float): Void;
      // Set view-space depth bounds.
    function setMinMaxZ(minZ: Float, maxZ: Float): Void;
      // Mark in view without specifying a camera. Used for shadow casters.
    function markInView(frameNumber: Int): Void;
      // Sort and limit per-pixel lights to maximum allowed. Convert extra lights into vertex lights.
    function limitLights(): Void;
      // Sort and limit per-vertex lights to maximum allowed.
    function limitVertexLights(removeConvertedLights: Bool): Void;
      // Set base pass flag for a batch.
    function setBasePass(batchIndex: Int): Void;
      // Return current zone.
    function getZone(): Zone;
      // Return whether current zone is inconclusive or dirty due to the drawable moving.
    function isZoneDirty(): Bool;
      // Return distance from camera.
    function getDistance(): Float;
      // Return LOD scaled distance from camera.
    function getLodDistance(): Float;
      // Return sorting value.
    function getSortValue(): Float;
      // Return whether has a base pass.
    function hasBasePass(batchIndex: Int): Bool;
      // Return the first added per-pixel light.
    function getFirstLight(): Light;
      // Return the minimum view-space depth.
    function getMinZ(): Float;
      // Return the maximum view-space depth.
    function getMaxZ(): Float;
    function addLight(light: Light): Void;
    function addVertexLight(light: Light): Void;

}

@:native("Atomic.Light")
extern class Light extends Drawable {

    var lightType: LightType;
    var perVertex: Bool;
    var color: Color;
    var specularIntensity: Float;
    var brightness: Float;
    var range: Float;
    var fov: Float;
    var aspectRatio: Float;
    var fadeDistance: Float;
    var shadowFadeDistance: Float;
    var shadowIntensity: Float;
    var shadowResolution: Float;
    var shadowNearFarRatio: Float;
    var rampTexture: Texture;
    var shapeTexture: Texture;
    var effectiveColor: Color;
    var effectiveSpecularIntensity: Float;
    var numShadowSplits: Int;

      // Construct.
    function new();

      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set light type.
    function setLightType(type: LightType): Void;
      // Set vertex lighting mode.
    function setPerVertex(enable: Bool): Void;
      // Set color.
    function setColor(color: Color): Void;
      // Set specular intensity. Zero disables specular calculations.
    function setSpecularIntensity(intensity: Float): Void;
      // Set light brightness multiplier. Both the color and specular intensity are multiplied with this to get final values for rendering.
    function setBrightness(brightness: Float): Void;
      // Set range.
    function setRange(range: Float): Void;
      // Set spotlight field of view.
    function setFov(fov: Float): Void;
      // Set spotlight aspect ratio.
    function setAspectRatio(aspectRatio: Float): Void;
      // Set fade out start distance.
    function setFadeDistance(distance: Float): Void;
      // Set shadow fade out start distance. Only has effect if shadow distance is also non-zero.
    function setShadowFadeDistance(distance: Float): Void;
      // Set shadow intensity between 0.0 - 1.0. 0.0 (the default) gives fully dark shadows.
    function setShadowIntensity(intensity: Float): Void;
      // Set shadow resolution between 0.25 - 1.0. Determines the shadow map to use.
    function setShadowResolution(resolution: Float): Void;
      // Set shadow camera near/far clip distance ratio.
    function setShadowNearFarRatio(nearFarRatio: Float): Void;
      // Set range attenuation texture.
    function setRampTexture(texture: Texture): Void;
      // Set spotlight attenuation texture.
    function setShapeTexture(texture: Texture): Void;
      // Return light type.
    function getLightType(): LightType;
      // Return vertex lighting mode.
    function getPerVertex(): Bool;
      // Return color.
    function getColor(): Color;
      // Return specular intensity.
    function getSpecularIntensity(): Float;
      // Return brightness multiplier.
    function getBrightness(): Float;
      // Return effective color, multiplied by brightness. Do not multiply the alpha so that can compare against the default black color to detect a light with no effect.
    function getEffectiveColor(): Color;
      // Return effective specular intensity, multiplied by absolute value of brightness.
    function getEffectiveSpecularIntensity(): Float;
      // Return range.
    function getRange(): Float;
      // Return spotlight field of view.
    function getFov(): Float;
      // Return spotlight aspect ratio.
    function getAspectRatio(): Float;
      // Return fade start distance.
    function getFadeDistance(): Float;
      // Return shadow fade start distance.
    function getShadowFadeDistance(): Float;
      // Return shadow intensity.
    function getShadowIntensity(): Float;
      // Return shadow resolution.
    function getShadowResolution(): Float;
      // Return shadow camera near/far clip distance ratio.
    function getShadowNearFarRatio(): Float;
      // Return range attenuation texture.
    function getRampTexture(): Texture;
      // Return spotlight attenuation texture.
    function getShapeTexture(): Texture;
      // Return number of shadow map cascade splits for a directional light, considering also graphics API limitations.
    function getNumShadowSplits(): Int;
      // Return whether light has negative (darkening) color.
    function isNegative(): Bool;
      // Return a divisor value based on intensity for calculating the sort value.
    function getIntensityDivisor(?attenuation: Float): Float;

}

@:native("Atomic.Material")
extern class Material extends Resource {

    var numTechniques: Int;
    var cullMode: CullMode;
    var shadowCullMode: CullMode;
    var fillMode: FillMode;
    var scene: Scene;
    var auxViewFrameNumber: Int;
    var occlusion: Bool;
    var specular: Bool;
    var shaderParameterHash: Int;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Set number of techniques.
    function setNumTechniques(num: Int): Void;
      // Set technique.
    function setTechnique(index: Int, tech: Technique, ?qualityLevel: Int, ?lodDistance: Float): Void;
    function setShaderParameterAnimation(name: String, animation: ValueAnimation, ?wrapMode: WrapMode, ?speed: Float): Void;
      // Set shader parameter animation wrap mode.
    function setShaderParameterAnimationWrapMode(name: String, wrapMode: WrapMode): Void;
      // Set shader parameter animation speed.
    function setShaderParameterAnimationSpeed(name: String, speed: Float): Void;
      // Set texture.
    function setTexture(unit: TextureUnit, texture: Texture): Void;
      // Set culling mode.
    function setCullMode(mode: CullMode): Void;
      // Set culling mode for shadows.
    function setShadowCullMode(mode: CullMode): Void;
      // Set polygon fill mode. Interacts with the camera's fill mode setting so that the "least filled" mode will be used.
    function setFillMode(mode: FillMode): Void;
      // Associate the material with a scene to ensure that shader parameter animation happens in sync with scene update, respecting the scene time scale. If no scene is set, the global update events will be used.
    function setScene(scene: Scene): Void;
      // Remove shader parameter.
    function removeShaderParameter(name: String): Void;
      // Reset all shader pointers.
    function releaseShaders(): Void;
      // Clone the material.
    function clone(?cloneName: String): Material;
      // Ensure that material techniques are listed in correct order.
    function sortTechniques(): Void;
      // Mark material for auxiliary view rendering.
    function markForAuxView(frameNumber: Int): Void;
      // Return number of techniques.
    function getNumTechniques(): Int;
      // Return technique by index.
    function getTechnique(index: Int): Technique;
      // Return pass by technique index and pass name.
    function getPass(index: Int, passName: String): Pass;
      // Return texture by unit.
    function getTexture(unit: TextureUnit): Texture;
      // Return shader parameter animation.
    function getShaderParameterAnimation(name: String): ValueAnimation;
      // Return shader parameter animation wrap mode.
    function getShaderParameterAnimationWrapMode(name: String): WrapMode;
      // Return shader parameter animation speed.
    function getShaderParameterAnimationSpeed(name: String): Float;
      // Return normal culling mode.
    function getCullMode(): CullMode;
      // Return culling mode for shadows.
    function getShadowCullMode(): CullMode;
      // Return polygon fill mode.
    function getFillMode(): FillMode;
      // Return last auxiliary view rendered frame number.
    function getAuxViewFrameNumber(): Int;
      // Return whether should render occlusion.
    function getOcclusion(): Bool;
      // Return whether should render specular.
    function getSpecular(): Bool;
      // Return the scene associated with the material for shader parameter animation updates.
    function getScene(): Scene;
      // Return shader parameter hash value. Used as an optimization to avoid setting shader parameters unnecessarily.
    function getShaderParameterHash(): Int;
      // Return name for texture unit.
    function getTextureUnitName(unit: TextureUnit): String;

}

@:native("Atomic.Octree")
extern class Octree extends Component {

    var numLevels: Int;

      // Construct.
    function new();

      // Set size and maximum subdivision levels. If octree is not empty, drawable objects will be temporarily moved to the root.
    function setSize(box: BoundingBox, numLevels: Int): Void;
      // Add a drawable manually.
    function addManualDrawable(drawable: Drawable): Void;
      // Remove a manually added drawable.
    function removeManualDrawable(drawable: Drawable): Void;
      // Return subdivision levels.
    function getNumLevels(): Int;
      // Mark drawable object as requiring an update and a reinsertion.
    function queueUpdate(drawable: Drawable): Void;
      // Cancel drawable object's update.
    function cancelUpdate(drawable: Drawable): Void;

}

@:native("Atomic.Renderer")
extern class Renderer extends AObject {

    var numViewports: Int;
    var hDRRendering: Bool;
    var specularLighting: Bool;
    var textureAnisotropy: Int;
    var textureFilterMode: TextureFilterMode;
    var textureQuality: Int;
    var materialQuality: Int;
    var drawShadows: Bool;
    var shadowMapSize: Int;
    var shadowQuality: Int;
    var reuseShadowMaps: Bool;
    var maxShadowMaps: Int;
    var dynamicInstancing: Bool;
    var minInstances: Int;
    var maxSortedInstances: Int;
    var maxOccluderTriangles: Int;
    var occlusionBufferSize: Int;
    var occluderSizeThreshold: Float;
    var mobileShadowBiasMul: Float;
    var mobileShadowBiasAdd: Float;
    var defaultRenderPath: RenderPath;
    var numViews: Int;
    var numPrimitives: Int;
    var numBatches: Int;
    var defaultZone: Zone;
    var defaultMaterial: Material;
    var defaultLightRamp: Texture2D;
    var defaultLightSpot: Texture2D;
    var faceSelectCubeMap: TextureCube;
    var indirectionCubeMap: TextureCube;
    var shadowCamera: Camera;

      // Construct.
    function new();

      // Set number of backbuffer viewports to render.
    function setNumViewports(num: Int): Void;
      // Set a backbuffer viewport.
    function setViewport(index: Int, viewport: Viewport): Void;
      // Set HDR rendering on/off.
    function setHDRRendering(enable: Bool): Void;
      // Set specular lighting on/off.
    function setSpecularLighting(enable: Bool): Void;
      // Set texture anisotropy.
    function setTextureAnisotropy(level: Int): Void;
      // Set texture filtering.
    function setTextureFilterMode(mode: TextureFilterMode): Void;
      // Set texture quality level. See the QUALITY constants in GraphicsDefs.h.
    function setTextureQuality(quality: Int): Void;
      // Set material quality level. See the QUALITY constants in GraphicsDefs.h.
    function setMaterialQuality(quality: Int): Void;
      // Set shadows on/off.
    function setDrawShadows(enable: Bool): Void;
      // Set shadow map resolution.
    function setShadowMapSize(size: Int): Void;
      // Set shadow quality mode. See the SHADOWQUALITY constants in GraphicsDefs.h.
    function setShadowQuality(quality: Int): Void;
      // Set reuse of shadow maps. Default is true. If disabled, also transparent geometry can be shadowed.
    function setReuseShadowMaps(enable: Bool): Void;
      // Set maximum number of shadow maps created for one resolution. Only has effect if reuse of shadow maps is disabled.
    function setMaxShadowMaps(shadowMaps: Int): Void;
      // Set dynamic instancing on/off.
    function setDynamicInstancing(enable: Bool): Void;
      // Set minimum number of instances required in a batch group to render as instanced.
    function setMinInstances(instances: Int): Void;
      // Set maximum number of sorted instances per batch group. If exceeded, instances are rendered unsorted.
    function setMaxSortedInstances(instances: Int): Void;
      // Set maximum number of occluder trianges.
    function setMaxOccluderTriangles(triangles: Int): Void;
      // Set occluder buffer width.
    function setOcclusionBufferSize(size: Int): Void;
      // Set required screen size (1.0 = full screen) for occluders.
    function setOccluderSizeThreshold(screenSize: Float): Void;
      // Set shadow depth bias multiplier for mobile platforms (OpenGL ES.) No effect on desktops. Default 2.
    function setMobileShadowBiasMul(mul: Float): Void;
      // Set shadow depth bias addition for mobile platforms (OpenGL ES.)  No effect on desktops. Default 0.0001.
    function setMobileShadowBiasAdd(add: Float): Void;
      // Force reload of shaders.
    function reloadShaders(): Void;
      // Return number of backbuffer viewports.
    function getNumViewports(): Int;
      // Return backbuffer viewport by index.
    function getViewport(index: Int): Viewport;
      // Return default renderpath.
    function getDefaultRenderPath(): RenderPath;
      // Return whether HDR rendering is enabled.
    function getHDRRendering(): Bool;
      // Return whether specular lighting is enabled.
    function getSpecularLighting(): Bool;
      // Return whether drawing shadows is enabled.
    function getDrawShadows(): Bool;
      // Return texture anisotropy.
    function getTextureAnisotropy(): Int;
      // Return texture filtering.
    function getTextureFilterMode(): TextureFilterMode;
      // Return texture quality level.
    function getTextureQuality(): Int;
      // Return material quality level.
    function getMaterialQuality(): Int;
      // Return shadow map resolution.
    function getShadowMapSize(): Int;
      // Return shadow quality.
    function getShadowQuality(): Int;
      // Return whether shadow maps are reused.
    function getReuseShadowMaps(): Bool;
      // Return maximum number of shadow maps per resolution.
    function getMaxShadowMaps(): Int;
      // Return whether dynamic instancing is in use.
    function getDynamicInstancing(): Bool;
      // Return minimum number of instances required in a batch group to render as instanced.
    function getMinInstances(): Int;
      // Return maximum number of sorted instances per batch group.
    function getMaxSortedInstances(): Int;
      // Return maximum number of occluder triangles.
    function getMaxOccluderTriangles(): Int;
      // Return occlusion buffer width.
    function getOcclusionBufferSize(): Int;
      // Return occluder screen size threshold.
    function getOccluderSizeThreshold(): Float;
      // Return shadow depth bias multiplier for mobile platforms.
    function getMobileShadowBiasMul(): Float;
      // Return shadow depth bias addition for mobile platforms.
    function getMobileShadowBiasAdd(): Float;
      // Return number of views rendered.
    function getNumViews(): Int;
      // Return number of primitives rendered.
    function getNumPrimitives(): Int;
      // Return number of batches rendered.
    function getNumBatches(): Int;
      // Return number of geometries rendered.
    function getNumGeometries(?allViews: Bool): Int;
      // Return number of lights rendered.
    function getNumLights(?allViews: Bool): Int;
      // Return number of shadow maps rendered.
    function getNumShadowMaps(?allViews: Bool): Int;
      // Return number of occluders rendered.
    function getNumOccluders(?allViews: Bool): Int;
      // Return the default zone.
    function getDefaultZone(): Zone;
      // Return the default material.
    function getDefaultMaterial(): Material;
      // Return the default range attenuation texture.
    function getDefaultLightRamp(): Texture2D;
      // Return the default spotlight attenuation texture.
    function getDefaultLightSpot(): Texture2D;
      // Return the shadowed pointlight face selection cube map.
    function getFaceSelectCubeMap(): TextureCube;
      // Return the shadowed pointlight indirection cube map.
    function getIndirectionCubeMap(): TextureCube;
      // Update for rendering. Called by HandleRenderUpdate().
    function update(timeStep: Float): Void;
      // Render. Called by Engine.
    function render(): Void;
      // Add debug geometry to the debug renderer.
    function drawDebugGeometry(depthTest: Bool): Void;
      // Queue a render surface's viewports for rendering. Called by the surface, or by View.
    function queueRenderSurface(renderTarget: RenderSurface): Void;
      // Queue a viewport for rendering. Null surface means backbuffer.
    function queueViewport(renderTarget: RenderSurface, viewport: Viewport): Void;
      // Allocate a shadow map. If shadow map reuse is disabled, a different map is returned each time.
    function getShadowMap(light: Light, camera: Camera, viewWidth: Int, viewHeight: Int): Texture2D;
      // Allocate a rendertarget or depth-stencil texture for deferred rendering or postprocessing. Should only be called during actual rendering, not before.
    function getScreenBuffer(width: Int, height: Int, format: Int, cubemap: Bool, filtered: Bool, srgb: Bool, ?persistentKey: Int): Texture;
      // Allocate a depth-stencil surface that does not need to be readable. Should only be called during actual rendering, not before.
    function getDepthStencil(width: Int, height: Int): RenderSurface;
      // Allocate a temporary shadow camera and a scene node for it. Is thread-safe.
    function getShadowCamera(): Camera;
      // Set cull mode while taking possible projection flipping into account.
    function setCullMode(mode: CullMode, camera: Camera): Void;
      // Ensure sufficient size of the instancing vertex buffer. Return true if successful.
    function resizeInstancingBuffer(numInstances: Int): Bool;
      // Save the screen buffer allocation status. Called by View.
    function saveScreenBufferAllocations(): Void;
      // Restore the screen buffer allocation status. Called by View.
    function restoreScreenBufferAllocations(): Void;
      // Optimize a light by scissor rectangle.
    function optimizeLightByScissor(light: Light, camera: Camera): Void;
      // Optimize a light by marking it to the stencil buffer and setting a stencil test.
    function optimizeLightByStencil(light: Light, camera: Camera): Void;
      // Return a scissor rectangle for a light.
    function getLightScissor(light: Light, camera: Camera): Rect;

}

@:native("Atomic.RenderPath")
extern class RenderPath extends RefCounted {

    var numRenderTargets: Int;
    var numCommands: Int;

      // Construct.
    function new();

      // Clone the rendering path.
    function clone(): RenderPath;
      // Clear existing data and load from an XML file. Return true if successful.
    function load(file: XMLFile): Bool;
      // Append data from an XML file. Return true if successful.
    function append(file: XMLFile): Bool;
      // Enable/disable commands and rendertargets by tag.
    function setEnabled(tag: String, active: Bool): Void;
      // Toggle enabled state of commands and rendertargets by tag.
    function toggleEnabled(tag: String): Void;
      // Remove rendertargets by tag name.
    function removeRenderTargets(tag: String): Void;
      // Remove a command by index.
    function removeCommand(index: Int): Void;
      // Remove commands by tag name.
    function removeCommands(tag: String): Void;
      // Return number of rendertargets.
    function getNumRenderTargets(): Int;
      // Return number of commands.
    function getNumCommands(): Int;

}

@:native("Atomic.Shader")
extern class Shader extends Resource {

    var timeStamp: Int;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Return a variation with defines.
    function getVariation(type: ShaderType, defines: String): ShaderVariation;
      // Return either vertex or pixel shader source code.
    function getSourceCode(type: ShaderType): String;
      // Return the latest timestamp of the shader code and its includes.
    function getTimeStamp(): Int;

}

@:native("Atomic.ShaderPrecache")
extern class ShaderPrecache extends AObject {

      // Construct and begin collecting shader combinations. Load existing combinations from XML if the file exists.
    function new(fileName: String);

      // Collect a shader combination. Called by Graphics when shaders have been set.
    function storeShaders(vs: ShaderVariation, ps: ShaderVariation): Void;

}

@:native("Atomic.Pass")
extern class Pass extends RefCounted {

    var blendMode: BlendMode;
    var depthTestMode: CompareMode;
    var lightingMode: PassLightingMode;
    var depthWrite: Bool;
    var alphaMask: Bool;
    var isDesktop: Bool;
    var vertexShader: String;
    var pixelShader: String;
    var vertexShaderDefines: String;
    var pixelShaderDefines: String;
    var name: String;
    var index: Int;
    var shadersLoadedFrameNumber: Int;

      // Construct.
    function new(passName: String);

      // Set blend mode.
    function setBlendMode(mode: BlendMode): Void;
      // Set depth compare mode.
    function setDepthTestMode(mode: CompareMode): Void;
      // Set pass lighting mode, affects what shader variations will be attempted to be loaded.
    function setLightingMode(mode: PassLightingMode): Void;
      // Set depth write on/off.
    function setDepthWrite(enable: Bool): Void;
      // Set alpha masking hint. Completely opaque draw calls will be performed before alpha masked.
    function setAlphaMask(enable: Bool): Void;
      // Set whether requires desktop level hardware.
    function setIsDesktop(enable: Bool): Void;
      // Set vertex shader name.
    function setVertexShader(name: String): Void;
      // Set pixel shader name.
    function setPixelShader(name: String): Void;
      // Set vertex shader defines.
    function setVertexShaderDefines(defines: String): Void;
      // Set pixel shader defines.
    function setPixelShaderDefines(defines: String): Void;
      // Reset shader pointers.
    function releaseShaders(): Void;
      // Mark shaders loaded this frame.
    function markShadersLoaded(frameNumber: Int): Void;
      // Return pass name.
    function getName(): String;
      // Return pass index. This is used for optimal render-time pass queries that avoid map lookups.
    function getIndex(): Int;
      // Return blend mode.
    function getBlendMode(): BlendMode;
      // Return depth compare mode.
    function getDepthTestMode(): CompareMode;
      // Return pass lighting mode.
    function getLightingMode(): PassLightingMode;
      // Return last shaders loaded frame number.
    function getShadersLoadedFrameNumber(): Int;
      // Return depth write mode.
    function getDepthWrite(): Bool;
      // Return alpha masking hint.
    function getAlphaMask(): Bool;
      // Return vertex shader name.
    function getVertexShader(): String;
      // Return pixel shader name.
    function getPixelShader(): String;
      // Return vertex shader defines.
    function getVertexShaderDefines(): String;
      // Return pixel shader defines.
    function getPixelShaderDefines(): String;

}

@:native("Atomic.Technique")
extern class Technique extends Resource {

    var isDesktop: Bool;
    var numPasses: Int;

      // Construct.
    function new();

      // Set whether requires desktop level hardware.
    function setIsDesktop(enable: Bool): Void;
      // Create a new pass.
    function createPass(passName: String): Pass;
      // Remove a pass.
    function removePass(passName: String): Void;
      // Reset shader pointers in all passes.
    function releaseShaders(): Void;
      // Return whether technique is supported by the current hardware.
    function isSupported(): Bool;
      // Return number of passes.
    function getNumPasses(): Int;
      // Return a pass type index by name. Allocate new if not used yet.
    function getPassIndex(passName: String): Int;

}

@:native("Atomic.View")
extern class View extends AObject {

    var graphics: Graphics;
    var renderer: Renderer;
    var scene: Scene;
    var octree: Octree;
    var camera: Camera;
    var renderTarget: RenderSurface;
    var drawDebug: Bool;

      // Construct.
    function new();

      // Define with rendertarget and viewport. Return true if successful.
    function define(renderTarget: RenderSurface, viewport: Viewport): Bool;
      // Render batches.
    function render(): Void;
      // Return graphics subsystem.
    function getGraphics(): Graphics;
      // Return renderer subsystem.
    function getRenderer(): Renderer;
      // Return scene.
    function getScene(): Scene;
      // Return octree.
    function getOctree(): Octree;
      // Return camera.
    function getCamera(): Camera;
      // Return the rendertarget. 0 if using the backbuffer.
    function getRenderTarget(): RenderSurface;
      // Return whether should draw debug geometry.
    function getDrawDebug(): Bool;
      // Set global (per-frame) shader parameters. Called by Batch and internally by View.
    function setGlobalShaderParameters(): Void;
      // Set camera-specific shader parameters. Called by Batch and internally by View.
    function setCameraShaderParameters(camera: Camera, setProjectionMatrix: Bool): Void;
      // Set G-buffer offset and inverse size shader parameters. Called by Batch and internally by View.
    function setGBufferShaderParameters(texSize: IntVector2, viewRect: IntRect): Void;

}

@:native("Atomic.Viewport")
extern class Viewport extends AObject {

    var scene: Scene;
    var camera: Camera;
    var rect: IntRect;
    var renderPath: RenderPath;
    var drawDebug: Bool;
    var view: View;
    var width: Int;
    var height: Int;

      // Construct with a full rectangle.
    function new(scene: Scene, camera: Camera, ?renderPath: RenderPath);

      // Set scene.
    function setScene(scene: Scene): Void;
      // Set camera.
    function setCamera(camera: Camera): Void;
      // Set rectangle.
    function setRect(rect: IntRect): Void;
      // Set rendering path from an XML file.
    function setRenderPath(file: XMLFile): Void;
      // Set whether to render debug geometry. Default true.
    function setDrawDebug(enable: Bool): Void;
      // Return scene.
    function getScene(): Scene;
      // Return camera.
    function getCamera(): Camera;
      // Return the internal rendering structure. May be null if the viewport has not been rendered yet.
    function getView(): View;
      // Return rectangle.
    function getRect(): IntRect;
      // Return the viewport width
    function getWidth(): Int;
      // Return the viewport height
    function getHeight(): Int;
      // Return rendering path.
    function getRenderPath(): RenderPath;
      // Return whether to draw debug geometry.
    function getDrawDebug(): Bool;
    function worldToScreenPoint(worldPos: Vector3): IntVector2;
    function screenToWorldPoint(x: Int, y: Int, depth: Float): Vector3;
      // Allocate the view structure. Called by Renderer.
    function allocateView(): Void;

}

@:native("Atomic.Zone")
extern class Zone extends Drawable {

    var ambientColor: Color;
    var fogColor: Color;
    var fogStart: Float;
    var fogEnd: Float;
    var fogHeight: Float;
    var fogHeightScale: Float;
    var priority: Int;
    var heightFog: Bool;
    var overide: Bool;
    var ambientGradient: Bool;
    var zoneTexture: Texture;
    var ambientStartColor: Color;
    var ambientEndColor: Color;

      // Construct.
    function new();

      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set local-space bounding box. Will be used as an oriented bounding box to test whether objects or the camera are inside.
    function setBoundingBox(box: BoundingBox): Void;
      // Set ambient color
    function setAmbientColor(color: Color): Void;
      // Set fog color.
    function setFogColor(color: Color): Void;
      // Set fog start distance.
    function setFogStart(start: Float): Void;
      // Set fog end distance.
    function setFogEnd(end: Float): Void;
      // Set fog height distance relative to the scene node's world position. Effective only in height fog mode.
    function setFogHeight(height: Float): Void;
      // Set fog height scale. Effective only in height fog mode.
    function setFogHeightScale(scale: Float): Void;
      // Set zone priority. If an object or camera is inside several zones, the one with highest priority is used.
    function setPriority(priority: Int): Void;
      // Set height fog mode.
    function setHeightFog(enable: Bool): Void;
      // Set override mode. If camera is inside an override zone, that zone will be used for all rendered objects instead of their own zone.
    function setOverride(enable: Bool): Void;
      // Set ambient gradient mode. In gradient mode ambient color is interpolated from neighbor zones.
    function setAmbientGradient(enable: Bool): Void;
      // Set zone texture. This will be bound to the zone texture unit when rendering objects inside the zone. Note that the default shaders do not use it.
    function setZoneTexture(texture: Texture): Void;
      // Return zone's own ambient color, disregarding gradient mode.
    function getAmbientColor(): Color;
      // Return ambient start color. Not safe to call from worker threads due to possible octree query.
    function getAmbientStartColor(): Color;
      // Return ambient end color. Not safe to call from worker threads due to possible octree query.
    function getAmbientEndColor(): Color;
      // Return fog color.
    function getFogColor(): Color;
      // Return fog start distance.
    function getFogStart(): Float;
      // Return fog end distance.
    function getFogEnd(): Float;
      // Return fog height distance relative to the scene node's world position.
    function getFogHeight(): Float;
      // Return fog height scale.
    function getFogHeightScale(): Float;
      // Return zone priority.
    function getPriority(): Int;
      // Return whether height fog mode is enabled.
    function getHeightFog(): Bool;
      // Return whether override mode is enabled.
    function getOverride(): Bool;
      // Return whether ambient gradient mode is enabled.
    function getAmbientGradient(): Bool;
      // Return zone texture.
    function getZoneTexture(): Texture;
      // Check whether a point is inside.
    function isInside(point: Vector3): Bool;

}

@:native("Atomic.Graphics")
extern class Graphics extends AObject {

    var windowTitle: String;
    var windowIcon: Image;
    var srgb: Bool;
    var flushGPU: Bool;
    var orientations: String;
    var defaultTextureFilterMode: TextureFilterMode;
    var textureAnisotropy: Int;
    var viewport: IntRect;
    var blendMode: BlendMode;
    var colorWrite: Bool;
    var cullMode: CullMode;
    var depthTest: CompareMode;
    var depthWrite: Bool;
    var fillMode: FillMode;
    var apiName: String;
    var windowPosition: IntVector2;
    var width: Int;
    var height: Int;
    var multiSample: Int;
    var fullscreen: Bool;
    var resizable: Bool;
    var borderless: Bool;
    var vSync: Bool;
    var tripleBuffer: Bool;
    var numPrimitives: Int;
    var numBatches: Int;
    var dummyColorFormat: Int;
    var shadowMapFormat: Int;
    var hiresShadowMapFormat: Int;
    var instancingSupport: Bool;
    var lightPrepassSupport: Bool;
    var deferredSupport: Bool;
    var hardwareShadowSupport: Bool;
    var readableDepthSupport: Bool;
    var sRGBSupport: Bool;
    var sRGBWriteSupport: Bool;
    var desktopResolution: IntVector2;
    var vertexShader: ShaderVariation;
    var pixelShader: ShaderVariation;
    var depthStencil: RenderSurface;
    var depthConstantBias: Float;
    var depthSlopeScaledBias: Float;
    var stencilTest: Bool;
    var scissorTest: Bool;
    var scissorRect: IntRect;
    var stencilTestMode: CompareMode;
    var stencilPass: StencilOp;
    var stencilFail: StencilOp;
    var stencilZFail: StencilOp;
    var stencilRef: Int;
    var stencilCompareMask: Int;
    var stencilWriteMask: Int;
    var useClipPlane: Bool;
    var renderTargetDimensions: IntVector2;
    var alphaFormat: Int;
    var luminanceFormat: Int;
    var luminanceAlphaFormat: Int;
    var rGBFormat: Int;
    var rGBAFormat: Int;
    var rGBA16Format: Int;
    var rGBAFloat16Format: Int;
    var rGBAFloat32Format: Int;
    var rG16Format: Int;
    var rGFloat16Format: Int;
    var rGFloat32Format: Int;
    var float16Format: Int;
    var float32Format: Int;
    var linearDepthFormat: Int;
    var depthStencilFormat: Int;
    var readableDepthFormat: Int;
    var pixelUVOffset: Vector2;
    var maxBones: Int;

      // Construct.
    function new();

      // Set window title.
    function setWindowTitle(windowTitle: String): Void;
      // Set window icon.
    function setWindowIcon(windowIcon: Image): Void;
      // Set window size.
    function setWindowSize(width: Int, height: Int): Void;
      // Center window.
    function centerWindow(): Void;
      // Bring the window to front with focus
    function raiseWindow(): Void;
      // Set whether the main window uses sRGB conversion on write.
    function setSRGB(enable: Bool): Void;
      // Set whether to flush the GPU command buffer to prevent multiple frames being queued and uneven frame timesteps. Default off, may decrease performance if enabled.
    function setFlushGPU(enable: Bool): Void;
      // Set allowed screen orientations as a space-separated list of "LandscapeLeft", "LandscapeRight", "Portrait" and "PortraitUpsideDown". Affects currently only iOS platform.
    function setOrientations(orientations: String): Void;
      // Toggle between full screen and windowed mode. Return true if successful.
    function toggleFullscreen(): Bool;
      // Close the window.
    function close(): Void;
      // Take a screenshot. Return true if successful.
    function takeScreenShot(destImage: Image): Bool;
      // Begin frame rendering. Return true if device available and can render.
    function beginFrame(): Bool;
      // End frame rendering and swap buffers.
    function endFrame(): Void;
      // Clear any or all of rendertarget, depth buffer and stencil buffer.
    function clear(flags: Int, ?color: Color, ?depth: Float, ?stencil: Int): Void;
      // Resolve multisampled backbuffer to a texture rendertarget. The texture's size should match the viewport size.
    function resolveToTexture(destination: Texture2D, viewport: IntRect): Bool;
      // Draw indexed, instanced geometry. An instancing vertex buffer must be set.
    function drawInstanced(type: PrimitiveType, indexStart: Int, indexCount: Int, minVertex: Int, vertexCount: Int, instanceCount: Int): Void;
      // Set shaders.
    function setShaders(vs: ShaderVariation, ps: ShaderVariation): Void;
      // Check whether a shader parameter exists on the currently set shaders.
    function hasShaderParameter(param: String): Bool;
      // Check whether the current pixel shader uses a texture unit.
    function hasTextureUnit(unit: TextureUnit): Bool;
      // Clear remembered shader parameter source group.
    function clearParameterSource(group: ShaderParameterGroup): Void;
      // Clear remembered shader parameter sources.
    function clearParameterSources(): Void;
      // Clear remembered transform shader parameter sources.
    function clearTransformSources(): Void;
      // Set texture.
    function setTexture(index: Int, texture: Texture): Void;
      // Set default texture filtering mode.
    function setDefaultTextureFilterMode(mode: TextureFilterMode): Void;
      // Set texture anisotropy.
    function setTextureAnisotropy(level: Int): Void;
      // Reset all rendertargets, depth-stencil surface and viewport.
    function resetRenderTargets(): Void;
      // Reset specific rendertarget.
    function resetRenderTarget(index: Int): Void;
      // Reset depth-stencil surface.
    function resetDepthStencil(): Void;
      // Set viewport.
    function setViewport(rect: IntRect): Void;
      // Set blending mode.
    function setBlendMode(mode: BlendMode): Void;
      // Set color write on/off.
    function setColorWrite(enable: Bool): Void;
      // Set hardware culling mode.
    function setCullMode(mode: CullMode): Void;
      // Set depth bias.
    function setDepthBias(constantBias: Float, slopeScaledBias: Float): Void;
      // Set depth compare.
    function setDepthTest(mode: CompareMode): Void;
      // Set depth write on/off.
    function setDepthWrite(enable: Bool): Void;
      // Set polygon fill mode.
    function setFillMode(mode: FillMode): Void;
      // Set stencil test.
    function setStencilTest(enable: Bool, ?mode: CompareMode, ?pass: StencilOp, ?fail: StencilOp, ?zFail: StencilOp, ?stencilRef: Int, ?compareMask: Int, ?writeMask: Int): Void;
      // Begin dumping shader variation names to an XML file for precaching.
    function beginDumpShaders(fileName: String): Void;
      // End dumping shader variations names.
    function endDumpShaders(): Void;
      // Return whether rendering initialized.
    function isInitialized(): Bool;
      // Return window title.
    function getWindowTitle(): String;
      // Return graphics API name.
    function getApiName(): String;
      // Return window position.
    function getWindowPosition(): IntVector2;
      // Return window width.
    function getWidth(): Int;
      // Return window height.
    function getHeight(): Int;
      // Return multisample mode (1 = no multisampling.)
    function getMultiSample(): Int;
      // Return whether window is fullscreen.
    function getFullscreen(): Bool;
      // Return whether window is resizable.
    function getResizable(): Bool;
      // Return whether window is borderless.
    function getBorderless(): Bool;
      // Return whether vertical sync is on.
    function getVSync(): Bool;
      // Return whether triple buffering is enabled.
    function getTripleBuffer(): Bool;
      // Return whether the main window is using sRGB conversion on write.
    function getSRGB(): Bool;
      // Return whether the GPU command buffer is flushed each frame.
    function getFlushGPU(): Bool;
      // Return allowed screen orientations.
    function getOrientations(): String;
      // Return whether Direct3D device is lost, and can not yet render. This happens during fullscreen resolution switching.
    function isDeviceLost(): Bool;
      // Return number of primitives drawn this frame.
    function getNumPrimitives(): Int;
      // Return number of batches drawn this frame.
    function getNumBatches(): Int;
      // Return dummy color texture format for shadow maps. Is "NULL" (consume no video memory) if supported.
    function getDummyColorFormat(): Int;
      // Return shadow map depth texture format, or 0 if not supported.
    function getShadowMapFormat(): Int;
      // Return 24-bit shadow map depth texture format, or 0 if not supported.
    function getHiresShadowMapFormat(): Int;
      // Return whether hardware instancing is supported..
    function getInstancingSupport(): Bool;
      // Return whether light pre-pass rendering is supported.
    function getLightPrepassSupport(): Bool;
      // Return whether deferred rendering is supported.
    function getDeferredSupport(): Bool;
      // Return whether shadow map depth compare is done in hardware.
    function getHardwareShadowSupport(): Bool;
      // Return whether a readable hardware depth format is available.
    function getReadableDepthSupport(): Bool;
      // Return whether sRGB conversion on texture sampling is supported.
    function getSRGBSupport(): Bool;
      // Return whether sRGB conversion on rendertarget writing is supported.
    function getSRGBWriteSupport(): Bool;
      // Return the desktop resolution.
    function getDesktopResolution(): IntVector2;
      // Return a shader variation by name and defines.
    function getShader(type: ShaderType, name: String, ?defines: String): ShaderVariation;
      // Return current vertex shader.
    function getVertexShader(): ShaderVariation;
      // Return current pixel shader.
    function getPixelShader(): ShaderVariation;
      // Return texture unit index by name.
    function getTextureUnit(name: String): TextureUnit;
      // Return texture unit name by index.
    function getTextureUnitName(unit: TextureUnit): String;
      // Return current texture by texture unit index.
    function getTexture(index: Int): Texture;
      // Return default texture filtering mode.
    function getDefaultTextureFilterMode(): TextureFilterMode;
      // Return current rendertarget by index.
    function getRenderTarget(index: Int): RenderSurface;
      // Return current depth-stencil surface.
    function getDepthStencil(): RenderSurface;
      // Return the viewport coordinates.
    function getViewport(): IntRect;
      // Return texture anisotropy.
    function getTextureAnisotropy(): Int;
      // Return blending mode.
    function getBlendMode(): BlendMode;
      // Return whether color write is enabled.
    function getColorWrite(): Bool;
      // Return hardware culling mode.
    function getCullMode(): CullMode;
      // Return depth constant bias.
    function getDepthConstantBias(): Float;
      // Return depth slope scaled bias.
    function getDepthSlopeScaledBias(): Float;
      // Return depth compare mode.
    function getDepthTest(): CompareMode;
      // Return whether depth write is enabled.
    function getDepthWrite(): Bool;
      // Return polygon fill mode.
    function getFillMode(): FillMode;
      // Return whether stencil test is enabled.
    function getStencilTest(): Bool;
      // Return whether scissor test is enabled.
    function getScissorTest(): Bool;
      // Return scissor rectangle coordinates.
    function getScissorRect(): IntRect;
      // Return stencil compare mode.
    function getStencilTestMode(): CompareMode;
      // Return stencil operation to do if stencil test passes.
    function getStencilPass(): StencilOp;
      // Return stencil operation to do if stencil test fails.
    function getStencilFail(): StencilOp;
      // Return stencil operation to do if depth compare fails.
    function getStencilZFail(): StencilOp;
      // Return stencil reference value.
    function getStencilRef(): Int;
      // Return stencil compare bitmask.
    function getStencilCompareMask(): Int;
      // Return stencil write bitmask.
    function getStencilWriteMask(): Int;
      // Return whether a custom clipping plane is in use.
    function getUseClipPlane(): Bool;
      // Return rendertarget width and height.
    function getRenderTargetDimensions(): IntVector2;
      // Window was resized through user interaction. Called by Input subsystem.
    function windowResized(): Void;
      // Window was moved through user interaction. Called by Input subsystem.
    function windowMoved(): Void;
      // Maximize the Window.
    function maximize(): Void;
      // Minimize the Window.
    function minimize(): Void;
      // Clean up too large scratch buffers.
    function cleanupScratchBuffers(): Void;
      // Clean up shader programs when a shader variation is released or destroyed.
    function cleanupShaderPrograms(variation: ShaderVariation): Void;
      // Return the API-specific alpha texture format.
    function getAlphaFormat(): Int;
      // Return the API-specific luminance texture format.
    function getLuminanceFormat(): Int;
      // Return the API-specific luminance alpha texture format.
    function getLuminanceAlphaFormat(): Int;
      // Return the API-specific RGB texture format.
    function getRGBFormat(): Int;
      // Return the API-specific RGBA texture format.
    function getRGBAFormat(): Int;
      // Return the API-specific RGBA 16-bit texture format.
    function getRGBA16Format(): Int;
      // Return the API-specific RGBA 16-bit float texture format.
    function getRGBAFloat16Format(): Int;
      // Return the API-specific RGBA 32-bit float texture format.
    function getRGBAFloat32Format(): Int;
      // Return the API-specific RG 16-bit texture format.
    function getRG16Format(): Int;
      // Return the API-specific RG 16-bit float texture format.
    function getRGFloat16Format(): Int;
      // Return the API-specific RG 32-bit float texture format.
    function getRGFloat32Format(): Int;
      // Return the API-specific single channel 16-bit float texture format.
    function getFloat16Format(): Int;
      // Return the API-specific single channel 32-bit float texture format.
    function getFloat32Format(): Int;
      // Return the API-specific linear depth texture format.
    function getLinearDepthFormat(): Int;
      // Return the API-specific hardware depth-stencil texture format.
    function getDepthStencilFormat(): Int;
      // Return the API-specific readable hardware depth format, or 0 if not supported.
    function getReadableDepthFormat(): Int;
      // Return UV offset required for pixel perfect rendering.
    function getPixelUVOffset(): Vector2;
      // Return maximum number of supported bones for skinning.
    function getMaxBones(): Int;

}

@:native("Atomic.RenderSurface")
extern class RenderSurface extends RefCounted {

    var numViewports: Int;
    var updateMode: RenderSurfaceUpdateMode;
    var linkedRenderTarget: RenderSurface;
    var linkedDepthStencil: RenderSurface;
    var parentTexture: Texture;
    var width: Int;
    var height: Int;
    var usage: TextureUsage;

      // Construct with parent texture.
    function new(parentTexture: Texture);

      // Set number of viewports.
    function setNumViewports(num: Int): Void;
      // Set viewport.
    function setViewport(index: Int, viewport: Viewport): Void;
      // Set viewport update mode. Default is to update when visible.
    function setUpdateMode(mode: RenderSurfaceUpdateMode): Void;
      // Set linked color rendertarget.
    function setLinkedRenderTarget(renderTarget: RenderSurface): Void;
      // Set linked depth-stencil surface.
    function setLinkedDepthStencil(depthStencil: RenderSurface): Void;
      // Queue manual update of the viewport(s).
    function queueUpdate(): Void;
      // Release surface.
    function release(): Void;
      // Return parent texture.
    function getParentTexture(): Texture;
      // Return width.
    function getWidth(): Int;
      // Return height.
    function getHeight(): Int;
      // Return usage.
    function getUsage(): TextureUsage;
      // Return number of viewports.
    function getNumViewports(): Int;
      // Return viewport by index.
    function getViewport(index: Int): Viewport;
      // Return viewport update mode.
    function getUpdateMode(): RenderSurfaceUpdateMode;
      // Return linked color rendertarget.
    function getLinkedRenderTarget(): RenderSurface;
      // Return linked depth-stencil surface.
    function getLinkedDepthStencil(): RenderSurface;
      // Clear update flag. Called by Renderer.
    function wasUpdated(): Void;

}

@:native("Atomic.ShaderVariation")
extern class ShaderVariation extends RefCounted {

    var name: String;
    var defines: String;
    var owner: Shader;
    var shaderType: ShaderType;
    var fullName: String;
    var compilerOutput: String;

      // Construct.
    function new(owner: Shader, type: ShaderType);

      // Release the shader.
    function release(): Void;
      // Compile the shader. Return true if successful.
    function create(): Bool;
      // Set name.
    function setName(name: String): Void;
      // Set defines.
    function setDefines(defines: String): Void;
      // Return the owner resource.
    function getOwner(): Shader;
      // Return shader type.
    function getShaderType(): ShaderType;
      // Return shader name.
    function getName(): String;
      // Return defines.
    function getDefines(): String;
      // Return full shader name.
    function getFullName(): String;
      // Return compile error/warning string.
    function getCompilerOutput(): String;
      // Return whether uses a parameter.
    function hasParameter(param: String): Bool;
      // Return whether uses a texture unit (only for pixel shaders.)
    function hasTextureUnit(unit: TextureUnit): Bool;

}

@:native("Atomic.Texture")
extern class Texture extends Resource {

    var numLevels: Int;
    var filterMode: TextureFilterMode;
    var shadowCompare: Bool;
    var borderColor: Color;
    var srgb: Bool;
    var backupTexture: Texture;
    var format: Int;
    var levels: Int;
    var width: Int;
    var height: Int;
    var depth: Int;
    var usage: TextureUsage;
    var components: Int;
    var parameters: XMLFile;

      // Construct.
    function new();

      // Set number of requested mip levels. Needs to be called before setting size.
    function setNumLevels(levels: Int): Void;
      // Set filtering mode.
    function setFilterMode(filter: TextureFilterMode): Void;
      // Set addressing mode by texture coordinate.
    function setAddressMode(coord: TextureCoordinate, address: TextureAddressMode): Void;
      // Set shadow compare mode. No-op on D3D9.
    function setShadowCompare(enable: Bool): Void;
      // Set border color for border addressing mode.
    function setBorderColor(color: Color): Void;
      // Set sRGB sampling and writing mode.
    function setSRGB(enable: Bool): Void;
      // Set backup texture to use when rendering to this texture.
    function setBackupTexture(texture: Texture): Void;
      // Set mip levels to skip on a quality setting when loading. Ensures higher quality levels do not skip more.
    function setMipsToSkip(quality: Int, mips: Int): Void;
      // Return texture format.
    function getFormat(): Int;
      // Return whether the texture format is compressed.
    function isCompressed(): Bool;
      // Return number of mip levels.
    function getLevels(): Int;
      // Return width.
    function getWidth(): Int;
      // Return height.
    function getHeight(): Int;
      // Return height.
    function getDepth(): Int;
      // Return filtering mode.
    function getFilterMode(): TextureFilterMode;
      // Return addressing mode by texture coordinate.
    function getAddressMode(coord: TextureCoordinate): TextureAddressMode;
      // Return whether shadow compare is enabled. Always false on D3D9.
    function getShadowCompare(): Bool;
      // Return border color.
    function getBorderColor(): Color;
      // Return whether is using sRGB sampling and writing.
    function getSRGB(): Bool;
      // Return backup texture.
    function getBackupTexture(): Texture;
      // Return mip levels to skip on a quality setting when loading.
    function getMipsToSkip(quality: Int): Int;
      // Return mip level width, or 0 if level does not exist.
    function getLevelWidth(level: Int): Int;
      // Return mip level width, or 0 if level does not exist.
    function getLevelHeight(level: Int): Int;
      // Return mip level depth, or 0 if level does not exist.
    function getLevelDepth(level: Int): Int;
      // Return texture usage type.
    function getUsage(): TextureUsage;
      // Return data size in bytes for a pixel or block row.
    function getRowDataSize(width: Int): Int;
      // Return number of image components required to receive pixel data from GetData(), or 0 for compressed images.
    function getComponents(): Int;
      // Set additional parameters from an XML file.
    function setParameters(xml: XMLFile): Void;

}

@:native("Atomic.Texture2D")
extern class Texture2D extends Texture {

    var renderSurface: RenderSurface;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Release default pool resources.
    function onDeviceLost(): Void;
      // Recreate default pool resources.
    function onDeviceReset(): Void;
      // Release texture.
    function release(): Void;
      // Set size, format and usage. Zero size will follow application window size. Return true if successful.
    function setSize(width: Int, height: Int, format: Int, ?usage: TextureUsage): Bool;
      // Return render surface.
    function getRenderSurface(): RenderSurface;

}

@:native("Atomic.Texture3D")
extern class Texture3D extends Texture {

    var renderSurface: RenderSurface;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Release default pool resources.
    function onDeviceLost(): Void;
      // Recreate default pool resources.
    function onDeviceReset(): Void;
      // Release texture.
    function release(): Void;
      // Set size, format and usage. Zero size will follow application window size. Return true if successful.
    function setSize(width: Int, height: Int, depth: Int, format: Int, ?usage: TextureUsage): Bool;
      // Return render surface.
    function getRenderSurface(): RenderSurface;

}

@:native("Atomic.TextureCube")
extern class TextureCube extends Texture {

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Release default pool resources.
    function onDeviceLost(): Void;
      // ReCreate default pool resources.
    function onDeviceReset(): Void;
      // Release texture.
    function release(): Void;
      // Set size, format and usage. Return true if successful.
    function setSize(size: Int, format: Int, ?usage: TextureUsage): Bool;
      // Return render surface for one face.
    function getRenderSurface(face: CubeMapFace): RenderSurface;

}



//----------------------------------------------------
// MODULE: Atomic3D
//----------------------------------------------------


@:native("Atomic.AnimatedModel")
extern class AnimatedModel extends StaticModel {

    var animationLodBias: Float;
    var updateInvisible: Bool;
    var numAnimationStates: Int;
    var numMorphs: Int;
    var boneCreationEnabled: Bool;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
    @:overload(function(): UpdateGeometryType{})
    override function getUpdateGeometryType(): UpdateGeometryType;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set model.
    @:overload(function(model: Model, ?createBones: Bool): Void{})
    override function setModel(model: Model): Void;
      // Add an animation.
    function addAnimationState(animation: Animation): AnimationState;
      // Remove all animations.
    function removeAllAnimationStates(): Void;
      // Set animation LOD bias.
    function setAnimationLodBias(bias: Float): Void;
      // Set whether to update animation and the bounding box when not visible. Recommended to enable for physically controlled models like ragdolls.
    function setUpdateInvisible(enable: Bool): Void;
      // Reset all vertex morphs to zero.
    function resetMorphWeights(): Void;
      // Return number of animation states.
    function getNumAnimationStates(): Int;
      // Return animation LOD bias.
    function getAnimationLodBias(): Float;
      // Return whether to update animation when not visible.
    function getUpdateInvisible(): Bool;
      // Return number of vertex morphs.
    function getNumMorphs(): Int;
      // Return whether is the master (first) animated model.
    function isMaster(): Bool;
      // Globally enable/disable bone creation, useful for when in the editor
    function setBoneCreationEnabled(enabled: Bool): Void;

}

@:native("Atomic.Animation")
extern class Animation extends Resource {

    var animationName: String;
    var length: Float;
    var numTriggers: Int;
    var animationNameHash: String;
    var numTracks: Int;

      // Construct.
    function new();

      // Set animation name.
    function setAnimationName(name: String): Void;
      // Set animation length.
    function setLength(length: Float): Void;
      // Remove a trigger point by index.
    function removeTrigger(index: Int): Void;
      // Remove all trigger points.
    function removeAllTriggers(): Void;
      // Resize trigger point vector.
    function setNumTriggers(num: Int): Void;
      // Return animation name.
    function getAnimationName(): String;
      // Return animation name hash.
    function getAnimationNameHash(): String;
      // Return animation length.
    function getLength(): Float;
      // Return number of animation tracks.
    function getNumTracks(): Int;
      // Return number of animation trigger points.
    function getNumTriggers(): Int;

}

@:native("Atomic.AnimationController")
extern class AnimationController extends Component {

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Update the animations. Is called from HandleScenePostUpdate().
    function update(timeStep: Float): Void;
      // Play an animation and set full target weight. Name must be the full resource name. Return true on success.
    function play(name: String, layer: Dynamic, looped: Bool, ?fadeInTime: Float): Bool;
      // Play an animation, set full target weight and fade out all other animations on the same layer. Name must be the full resource name. Return true on success.
    function playExclusive(name: String, layer: Dynamic, looped: Bool, ?fadeTime: Float): Bool;
      // Stop an animation. Zero fadetime is instant. Return true on success.
    function stop(name: String, ?fadeOutTime: Float): Bool;
      // Stop all animations on a specific layer. Zero fadetime is instant.
    function stopLayer(layer: Dynamic, ?fadeOutTime: Float): Void;
      // Stop all animations. Zero fadetime is instant.
    function stopAll(?fadeTime: Float): Void;
      // Fade animation to target weight. Return true on success.
    function fade(name: String, targetWeight: Float, fadeTime: Float): Bool;
      // Fade other animations on the same layer to target weight. Return true on success.
    function fadeOthers(name: String, targetWeight: Float, fadeTime: Float): Bool;
      // Set animation blending layer priority. Return true on success.
    function setLayer(name: String, layer: Dynamic): Bool;
      // Set animation start bone. Return true on success.
    function setStartBone(name: String, startBoneName: String): Bool;
      // Set animation time position. Return true on success.
    function setTime(name: String, time: Float): Bool;
      // Set animation weight. Return true on success.
    function setWeight(name: String, weight: Float): Bool;
      // Set animation looping. Return true on success.
    function setLooped(name: String, enable: Bool): Bool;
      // Set animation speed. Return true on success.
    function setSpeed(name: String, speed: Float): Bool;
      // Set animation autofade at end (non-looped animations only.) Zero time disables. Return true on success.
    function setAutoFade(name: String, fadeOutTime: Float): Bool;
      // Return whether an animation is active. Note that non-looping animations that are being clamped at the end also return true.
    function isPlaying(name: String): Bool;
      // Return whether an animation is fading in.
    function isFadingIn(name: String): Bool;
      // Return whether an animation is fading out.
    function isFadingOut(name: String): Bool;
      // Return whether an animation is at its end. Will return false if the animation is not active at all.
    function isAtEnd(name: String): Bool;
      // Return animation blending layer.
    function getLayer(name: String): Dynamic;
      // Return animation start bone name, or empty string if no such animation.
    function getStartBoneName(name: String): String;
      // Return animation time position.
    function getTime(name: String): Float;
      // Return animation weight.
    function getWeight(name: String): Float;
      // Return animation looping.
    function isLooped(name: String): Bool;
      // Return animation length.
    function getLength(name: String): Float;
      // Return animation speed.
    function getSpeed(name: String): Float;
      // Return animation fade target weight.
    function getFadeTarget(name: String): Float;
      // Return animation fade time.
    function getFadeTime(name: String): Float;
      // Return animation autofade time.
    function getAutoFade(name: String): Float;
    function addAnimationResource(animation: Animation): Void;
    function removeAnimationResource(animation: Animation): Void;
    function clearAnimationResources(): Void;

}

@:native("Atomic.AnimationState")
extern class AnimationState extends RefCounted {

    var looped: Bool;
    var weight: Float;
    var time: Float;
    var layer: Dynamic;
    var animation: Animation;
    var model: AnimatedModel;
    var node: Node;
    var length: Float;

      // Construct with animated model and animation pointers.
    function new(model: AnimatedModel, animation: Animation);

      // Set looping enabled/disabled.
    function setLooped(looped: Bool): Void;
      // Set blending weight.
    function setWeight(weight: Float): Void;
      // Set time position. Does not fire animation triggers.
    function setTime(time: Float): Void;
      // Modify blending weight.
    function addWeight(delta: Float): Void;
      // Modify time position. %Animation triggers will be fired.
    function addTime(delta: Float): Void;
      // Set blending layer.
    function setLayer(layer: Dynamic): Void;
      // Return animation.
    function getAnimation(): Animation;
      // Return animated model this state belongs to (model mode.)
    function getModel(): AnimatedModel;
      // Return root scene node this state controls (node hierarchy mode.)
    function getNode(): Node;
      // Return whether weight is nonzero.
    function isEnabled(): Bool;
      // Return whether looped.
    function isLooped(): Bool;
      // Return blending weight.
    function getWeight(): Float;
      // Return time position.
    function getTime(): Float;
      // Return animation length.
    function getLength(): Float;
      // Return blending layer.
    function getLayer(): Dynamic;
      // Apply the animation at the current time position.
    function apply(): Void;

}

@:native("Atomic.BillboardSet")
extern class BillboardSet extends Drawable {

    var material: Material;
    var numBillboards: Int;
    var relative: Bool;
    var scaled: Bool;
    var sorted: Bool;
    var faceCameraMode: FaceCameraMode;
    var animationLodBias: Float;

      // Construct.
    function new();

      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
    @:overload(function(): UpdateGeometryType{})
    override function getUpdateGeometryType(): UpdateGeometryType;
      // Set material.
    function setMaterial(material: Material): Void;
      // Set number of billboards.
    function setNumBillboards(num: Int): Void;
      // Set whether billboards are relative to the scene node. Default true.
    function setRelative(enable: Bool): Void;
      // Set whether scene node scale affects billboards' size. Default true.
    function setScaled(enable: Bool): Void;
      // Set whether billboards are sorted by distance. Default false.
    function setSorted(enable: Bool): Void;
      // Set how the billboards should rotate in relation to the camera. Default is to follow camera rotation on all axes (FC_ROTATE_XYZ.)
    function setFaceCameraMode(mode: FaceCameraMode): Void;
      // Set animation LOD bias.
    function setAnimationLodBias(bias: Float): Void;
      // Mark for bounding box and vertex buffer update. Call after modifying the billboards.
    function commit(): Void;
      // Return material.
    function getMaterial(): Material;
      // Return number of billboards.
    function getNumBillboards(): Int;
      // Return whether billboards are relative to the scene node.
    function isRelative(): Bool;
      // Return whether scene node scale affects billboards' size.
    function isScaled(): Bool;
      // Return whether billboards are sorted.
    function isSorted(): Bool;
      // Return how the billboards rotate in relation to the camera.
    function getFaceCameraMode(): FaceCameraMode;
      // Return animation LOD bias.
    function getAnimationLodBias(): Float;

}

@:native("Atomic.CustomGeometry")
extern class CustomGeometry extends Drawable {

    var numGeometries: Int;
    var dynamik: Bool;

      // Construct.
    function new();

      // Return number of occlusion geometry triangles.
    @:overload(function(): Int{})
    override function getNumOccluderTriangles(): Int;
      // Clear all geometries.
    function clear(): Void;
      // Set number of geometries.
    function setNumGeometries(num: Int): Void;
      // Set vertex buffer dynamic mode. A dynamic buffer should be faster to update frequently. Effective at the next Commit() call.
    function setDynamic(enable: Bool): Void;
      // Begin defining a geometry. Clears existing vertices in that index.
    function beginGeometry(index: Int, type: PrimitiveType): Void;
      // Define a vertex position. This begins a new vertex.
    function defineVertex(position: Vector3): Void;
      // Define a vertex normal.
    function defineNormal(normal: Vector3): Void;
      // Define a vertex color.
    function defineColor(color: Color): Void;
      // Define a vertex UV coordinate.
    function defineTexCoord(texCoord: Vector2): Void;
      // Define a vertex tangent.
    function defineTangent(tangent: Vector4): Void;
      // Set the primitive type, number of vertices and elements in a geometry, after which the vertices can be edited with GetVertex(). An alternative to BeginGeometry() / DefineVertex().
    function defineGeometry(index: Int, type: PrimitiveType, numVertices: Int, hasNormals: Bool, hasColors: Bool, hasTexCoords: Bool, hasTangents: Bool): Void;
      // Update vertex buffer and calculate the bounding box. Call after finishing defining geometry.
    function commit(): Void;
      // Return number of geometries.
    function getNumGeometries(): Int;
      // Return number of vertices in a geometry.
    function getNumVertices(index: Int): Int;
      // Return whether vertex buffer dynamic mode is enabled.
    function isDynamic(): Bool;
      // Return material by geometry index.
    function getMaterial(?index: Int): Material;

}

@:native("Atomic.DecalSet")
extern class DecalSet extends Drawable {

    var material: Material;
    var maxVertices: Int;
    var maxIndices: Int;
    var numDecals: Int;
    var numVertices: Int;
    var numIndices: Int;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
    @:overload(function(): UpdateGeometryType{})
    override function getUpdateGeometryType(): UpdateGeometryType;
      // Set material. The material should use a small negative depth bias to avoid Z-fighting.
    function setMaterial(material: Material): Void;
      // Set maximum number of decal vertices.
    function setMaxVertices(num: Int): Void;
      // Set maximum number of decal vertex indices.
    function setMaxIndices(num: Int): Void;
      // Add a decal at world coordinates, using a target drawable's geometry for reference. If the decal needs to move with the target, the decal component should be created to the target's node. Return true if successful.
    function addDecal(target: Drawable, worldPosition: Vector3, worldRotation: Quaternion, size: Float, aspectRatio: Float, depth: Float, topLeftUV: Vector2, bottomRightUV: Vector2, ?timeToLive: Float, ?normalCutoff: Float, ?subGeometry: Int): Bool;
      // Remove n oldest decals.
    function removeDecals(num: Int): Void;
      // Remove all decals.
    function removeAllDecals(): Void;
      // Return material.
    function getMaterial(): Material;
      // Return number of decals.
    function getNumDecals(): Int;
      // Retur number of vertices in the decals.
    function getNumVertices(): Int;
      // Retur number of vertex indices in the decals.
    function getNumIndices(): Int;
      // Return maximum number of decal vertices.
    function getMaxVertices(): Int;
      // Return maximum number of decal vertex indices.
    function getMaxIndices(): Int;

}

@:native("Atomic.Model")
extern class Model extends Resource {

    var boundingBox: BoundingBox;
    var numGeometries: Int;
    var numMorphs: Int;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Set local-space bounding box.
    function setBoundingBox(box: BoundingBox): Void;
      // Set number of geometries.
    function setNumGeometries(num: Int): Void;
      // Set number of LOD levels in a geometry.
    function setNumGeometryLodLevels(index: Int, num: Int): Bool;
      // Set geometry center.
    function setGeometryCenter(index: Int, center: Vector3): Bool;
      // Clone the model. The geometry data is deep-copied and can be modified in the clone without affecting the original.
    function clone(?cloneName: String): Model;
      // Return bounding box.
    function getBoundingBox(): BoundingBox;
      // Return number of geometries.
    function getNumGeometries(): Int;
      // Return number of LOD levels in geometry.
    function getNumGeometryLodLevels(index: Int): Int;
      // Return geometry center by index.
    function getGeometryCenter(index: Int): Vector3;
      // Return number of vertex morphs.
    function getNumMorphs(): Int;
      // Return vertex buffer morph range start.
    function getMorphRangeStart(bufferIndex: Int): Int;
      // Return vertex buffer morph range vertex count.
    function getMorphRangeCount(bufferIndex: Int): Int;

}

@:native("Atomic.ParticleEffect")
extern class ParticleEffect extends Resource {

    var material: Material;
    var numParticles: Int;
    var updateInvisible: Bool;
    var relative: Bool;
    var scaled: Bool;
    var sorted: Bool;
    var animationLodBias: Float;
    var emitterType: EmitterType;
    var emitterSize: Vector3;
    var minDirection: Vector3;
    var maxDirection: Vector3;
    var constantForce: Vector3;
    var dampingForce: Float;
    var activeTime: Float;
    var inactiveTime: Float;
    var minEmissionRate: Float;
    var maxEmissionRate: Float;
    var minParticleSize: Vector2;
    var maxParticleSize: Vector2;
    var minTimeToLive: Float;
    var maxTimeToLive: Float;
    var minVelocity: Float;
    var maxVelocity: Float;
    var minRotation: Float;
    var maxRotation: Float;
    var minRotationSpeed: Float;
    var maxRotationSpeed: Float;
    var sizeAdd: Float;
    var sizeMul: Float;
    var numColorFrames: Int;
    var numTextureFrames: Int;
    var randomDirection: Vector3;
    var randomSize: Vector2;
    var randomVelocity: Float;
    var randomTimeToLive: Float;
    var randomRotationSpeed: Float;
    var randomRotation: Float;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Set material.
    function setMaterial(material: Material): Void;
      // Set maximum number of particles.
    function setNumParticles(num: Int): Void;
      // Set whether to update when particles are not visible.
    function setUpdateInvisible(enable: Bool): Void;
      // Set whether billboards are relative to the scene node. Default true.
    function setRelative(enable: Bool): Void;
      // Set scaled.
    function setScaled(enable: Bool): Void;
      // Set sorted.
    function setSorted(enable: Bool): Void;
      // Set animation LOD bias.
    function setAnimationLodBias(lodBias: Float): Void;
      // Set emitter type.
    function setEmitterType(type: EmitterType): Void;
      // Set emitter size.
    function setEmitterSize(size: Vector3): Void;
      // Set negative direction limit.
    function setMinDirection(direction: Vector3): Void;
      // Set positive direction limit.
    function setMaxDirection(direction: Vector3): Void;
      // Set constant force acting on particles.
    function setConstantForce(force: Vector3): Void;
      // Set particle velocity damping force.
    function setDampingForce(force: Float): Void;
      // Set emission active period length (0 = infinite.)
    function setActiveTime(time: Float): Void;
      // Set emission inactive period length (0 = infinite.)
    function setInactiveTime(time: Float): Void;
      // Set minimum emission rate.
    function setMinEmissionRate(rate: Float): Void;
      // Set maximum emission rate.
    function setMaxEmissionRate(rate: Float): Void;
      // Set particle minimum size.
    function setMinParticleSize(size: Vector2): Void;
      // Set particle maximum size.
    function setMaxParticleSize(size: Vector2): Void;
      // Set particle minimum time to live.
    function setMinTimeToLive(time: Float): Void;
      // Set particle maximum time to live.
    function setMaxTimeToLive(time: Float): Void;
      // Set particle minimum velocity.
    function setMinVelocity(velocity: Float): Void;
      // Set particle maximum velocity.
    function setMaxVelocity(velocity: Float): Void;
      // Set particle minimum rotation.
    function setMinRotation(rotation: Float): Void;
      // Set particle maximum rotation.
    function setMaxRotation(rotation: Float): Void;
      // Set particle minimum rotation speed.
    function setMinRotationSpeed(speed: Float): Void;
      // Set particle maximum rotation speed.
    function setMaxRotationSpeed(speed: Float): Void;
      // Set particle size additive modifier.
    function setSizeAdd(sizeAdd: Float): Void;
      // Set particle size multiplicative modifier.
    function setSizeMul(sizeMul: Float): Void;
      // Add a color frame sorted in the correct position based on time.
    function addColorTime(color: Color, time: Float): Void;
      // Remove color frame at index
    function removeColorFrame(index: Int): Void;
      // Set number of color frames.
    function setNumColorFrames(number: Int): Void;
      // Sort the list of color frames based on time.
    function sortColorFrames(): Void;
      // Add a texture frame sorted in the correct position based on time.
    function addTextureTime(uv: Rect, time: Float): Void;
      // Remove texture frame at index
    function removeTextureFrame(index: Int): Void;
      // Set number of texture frames.
    function setNumTextureFrames(number: Int): Void;
      // Sort the list of texture frames based on time.
    function sortTextureFrames(): Void;
      // Return material.
    function getMaterial(): Material;
      // Return maximum number of particles.
    function getNumParticles(): Int;
      // Return whether to update when particles are not visible.
    function getUpdateInvisible(): Bool;
      // Return whether billboards are relative to the scene node.
    function isRelative(): Bool;
      // Return whether scene node scale affects billboards' size.
    function isScaled(): Bool;
      // Return whether billboards are sorted.
    function isSorted(): Bool;
      // Return animation Lod bias.
    function getAnimationLodBias(): Float;
      // Return emitter type.
    function getEmitterType(): EmitterType;
      // Return emitter size.
    function getEmitterSize(): Vector3;
      // Return negative direction limit.
    function getMinDirection(): Vector3;
      // Return positive direction limit.
    function getMaxDirection(): Vector3;
      // Return constant force acting on particles.
    function getConstantForce(): Vector3;
      // Return particle velocity damping force.
    function getDampingForce(): Float;
      // Return emission active period length (0 = infinite.)
    function getActiveTime(): Float;
      // Return emission inactive period length (0 = infinite.)
    function getInactiveTime(): Float;
      // Return minimum emission rate.
    function getMinEmissionRate(): Float;
      // Return maximum emission rate.
    function getMaxEmissionRate(): Float;
      // Return particle minimum size.
    function getMinParticleSize(): Vector2;
      // Return particle maximum size.
    function getMaxParticleSize(): Vector2;
      // Return particle minimum time to live.
    function getMinTimeToLive(): Float;
      // Return particle maximum time to live.
    function getMaxTimeToLive(): Float;
      // Return particle minimum velocity.
    function getMinVelocity(): Float;
      // Return particle maximum velocity.
    function getMaxVelocity(): Float;
      // Return particle minimum rotation.
    function getMinRotation(): Float;
      // Return particle maximum rotation.
    function getMaxRotation(): Float;
      // Return particle minimum rotation speed.
    function getMinRotationSpeed(): Float;
      // Return particle maximum rotation speed.
    function getMaxRotationSpeed(): Float;
      // Return particle size additive modifier.
    function getSizeAdd(): Float;
      // Return particle size multiplicative modifier.
    function getSizeMul(): Float;
      // Return number of color animation frames.
    function getNumColorFrames(): Int;
      // Return number of texture animation frames.
    function getNumTextureFrames(): Int;
      // Return random direction.
    function getRandomDirection(): Vector3;
      // Return random size.
    function getRandomSize(): Vector2;
      // Return random velocity.
    function getRandomVelocity(): Float;
      // Return random timetolive.
    function getRandomTimeToLive(): Float;
      // Return random rotationspeed.
    function getRandomRotationSpeed(): Float;
      // Return random rotation.
    function getRandomRotation(): Float;

}

@:native("Atomic.ParticleEmitter")
extern class ParticleEmitter extends BillboardSet {

    var effect: ParticleEffect;
    var numParticles: Int;
    var emitting: Bool;
    var serializeParticles: Bool;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set particle effect.
    function setEffect(effect: ParticleEffect): Void;
      // Set maximum number of particles.
    function setNumParticles(num: Int): Void;
      // Set whether should be emitting. If the state was changed, also resets the emission period timer.
    function setEmitting(enable: Bool): Void;
      // Set whether particles should be serialized. Default true, set false to reduce scene file size.
    function setSerializeParticles(enable: Bool): Void;
      // Reset the emission period timer.
    function resetEmissionTimer(): Void;
      // Remove all current particles.
    function removeAllParticles(): Void;
      // Reset the particle emitter completely. Removes current particles, sets emitting state on, and resets the emission timer.
    function reset(): Void;
      // Apply not continuously updated values such as the material, the number of particles and sorting mode from the particle effect. Call this if you change the effect programmatically.
    function applyEffect(): Void;
      // Return particle effect.
    function getEffect(): ParticleEffect;
      // Return maximum number of particles.
    function getNumParticles(): Int;
      // Return whether is currently emitting.
    function isEmitting(): Bool;
      // Return whether particles are to be serialized.
    function getSerializeParticles(): Bool;

}

@:native("Atomic.Skybox")
extern class Skybox extends StaticModel {

      // Construct.
    function new();


}

@:native("Atomic.StaticModel")
extern class StaticModel extends Drawable {

    var model: Model;
    var material: Material;
    var occlusionLodLevel: Int;
    var numGeometries: Int;

      // Construct.
    function new();

      // Return number of occlusion geometry triangles.
    @:overload(function(): Int{})
    override function getNumOccluderTriangles(): Int;
      // Set model.
    function setModel(model: Model): Void;
      // Set material on all geometries.
    function setMaterial(material: Material): Void;
      // Set occlusion LOD level. By default (M_MAX_UNSIGNED) same as visible.
    function setOcclusionLodLevel(level: Int): Void;
      // Apply default materials from a material list file. If filename is empty (default), the model's resource name with extension .txt will be used.
    function applyMaterialList(?fileName: String): Void;
      // Return model.
    function getModel(): Model;
      // Return number of geometries.
    function getNumGeometries(): Int;
      // Return material by geometry index.
    function getMaterial(?index: Int): Material;
      // Return occlusion LOD level.
    function getOcclusionLodLevel(): Int;
      // Determines if the given world space point is within the model geometry.
    function isInside(point: Vector3): Bool;
      // Determines if the given local space point is within the model geometry.
    function isInsideLocal(point: Vector3): Bool;

}

@:native("Atomic.StaticModelGroup")
extern class StaticModelGroup extends StaticModel {

    var numInstanceNodes: Int;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Return number of occlusion geometry triangles.
    @:overload(function(): Int{})
    override function getNumOccluderTriangles(): Int;
      // Add an instance scene node. It does not need any drawable components of its own.
    function addInstanceNode(node: Node): Void;
      // Remove an instance scene node.
    function removeInstanceNode(node: Node): Void;
      // Remove all instance scene nodes.
    function removeAllInstanceNodes(): Void;
      // Return number of instance nodes.
    function getNumInstanceNodes(): Int;
      // Return instance node by index.
    function getInstanceNode(index: Int): Node;

}

@:native("Atomic.Terrain")
extern class Terrain extends Component {

    var patchSize: Int;
    var spacing: Vector3;
    var smoothing: Bool;
    var material: Material;
    var drawDistance: Float;
    var shadowDistance: Float;
    var lodBias: Float;
    var viewMask: Int;
    var lightMask: Int;
    var shadowMask: Int;
    var zoneMask: Int;
    var maxLights: Int;
    var castShadows: Bool;
    var occluder: Bool;
    var occludee: Bool;
    var numVertices: IntVector2;
    var numPatches: IntVector2;
    var heightMap: Image;
    var patchSizeAttr: Int;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set patch quads per side. Must be a power of two.
    function setPatchSize(size: Int): Void;
      // Set vertex (XZ) and height (Y) spacing.
    function setSpacing(spacing: Vector3): Void;
      // Set smoothing of heightmap.
    function setSmoothing(enable: Bool): Void;
      // Set heightmap image. Dimensions should be a power of two + 1. Uses 8-bit grayscale, or optionally red as MSB and green as LSB for 16-bit accuracy. Return true if successful.
    function setHeightMap(image: Image): Bool;
      // Set material.
    function setMaterial(material: Material): Void;
      // Set draw distance for patches.
    function setDrawDistance(distance: Float): Void;
      // Set shadow draw distance for patches.
    function setShadowDistance(distance: Float): Void;
      // Set LOD bias for patches. Affects which terrain LOD to display.
    function setLodBias(bias: Float): Void;
      // Set view mask for patches. Is and'ed with camera's view mask to see if the object should be rendered.
    function setViewMask(mask: Int): Void;
      // Set light mask for patches. Is and'ed with light's and zone's light mask to see if the object should be lit.
    function setLightMask(mask: Int): Void;
      // Set shadow mask for patches. Is and'ed with light's light mask and zone's shadow mask to see if the object should be rendered to a shadow map.
    function setShadowMask(mask: Int): Void;
      // Set zone mask for patches. Is and'ed with zone's zone mask to see if the object should belong to the zone.
    function setZoneMask(mask: Int): Void;
      // Set maximum number of per-pixel lights for patches. Default 0 is unlimited.
    function setMaxLights(num: Int): Void;
      // Set shadowcaster flag for patches.
    function setCastShadows(enable: Bool): Void;
      // Set occlusion flag for patches. Occlusion uses the coarsest LOD and may potentially be too aggressive, so use with caution.
    function setOccluder(enable: Bool): Void;
      // Set occludee flag for patches.
    function setOccludee(enable: Bool): Void;
      // Apply changes from the heightmap image.
    function applyHeightMap(): Void;
      // Return patch quads per side.
    function getPatchSize(): Int;
      // Return vertex and height spacing.
    function getSpacing(): Vector3;
      // Return heightmap size in vertices.
    function getNumVertices(): IntVector2;
      // Return heightmap size in patches.
    function getNumPatches(): IntVector2;
      // Return whether smoothing is in use.
    function getSmoothing(): Bool;
      // Return heightmap image.
    function getHeightMap(): Image;
      // Return material.
    function getMaterial(): Material;
      // Return height at world coordinates.
    function getHeight(worldPosition: Vector3): Float;
      // Return normal at world coordinates.
    function getNormal(worldPosition: Vector3): Vector3;
      // Convert world position to heightmap pixel position. Note that the internal height data representation is reversed vertically, but in the heightmap image north is at the top.
    function worldToHeightMap(worldPosition: Vector3): IntVector2;
      // Return draw distance.
    function getDrawDistance(): Float;
      // Return shadow draw distance.
    function getShadowDistance(): Float;
      // Return LOD bias.
    function getLodBias(): Float;
      // Return view mask.
    function getViewMask(): Int;
      // Return light mask.
    function getLightMask(): Int;
      // Return shadow mask.
    function getShadowMask(): Int;
      // Return zone mask.
    function getZoneMask(): Int;
      // Return maximum number of per-pixel lights.
    function getMaxLights(): Int;
      // Return visible flag.
    function isVisible(): Bool;
      // Return shadowcaster flag.
    function getCastShadows(): Bool;
      // Return occluder flag.
    function isOccluder(): Bool;
      // Return occludee flag.
    function isOccludee(): Bool;
      // Regenerate patch geometry.
    function createPatchGeometry(patch: TerrainPatch): Void;
      // Update patch based on LOD and neighbor LOD.
    function updatePatchLod(patch: TerrainPatch): Void;
      // Set patch size attribute.
    function setPatchSizeAttr(value: Int): Void;

}

@:native("Atomic.TerrainPatch")
extern class TerrainPatch extends Drawable {

    var owner: Terrain;
    var material: Material;
    var coordinates: IntVector2;
    var occlusionOffset: Float;
    var northPatch: TerrainPatch;
    var southPatch: TerrainPatch;
    var westPatch: TerrainPatch;
    var eastPatch: TerrainPatch;
    var lodLevel: Int;

      // Construct.
    function new();

      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
    @:overload(function(): UpdateGeometryType{})
    override function getUpdateGeometryType(): UpdateGeometryType;
      // Return number of occlusion geometry triangles.
    @:overload(function(): Int{})
    override function getNumOccluderTriangles(): Int;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set owner terrain.
    function setOwner(terrain: Terrain): Void;
      // Set neighbor patches.
    function setNeighbors(north: TerrainPatch, south: TerrainPatch, west: TerrainPatch, east: TerrainPatch): Void;
      // Set material.
    function setMaterial(material: Material): Void;
      // Set local-space bounding box.
    function setBoundingBox(box: BoundingBox): Void;
      // Set patch coordinates.
    function setCoordinates(coordinates: IntVector2): Void;
      // Set vertical offset for occlusion geometry. Should be negative.
    function setOcclusionOffset(offset: Float): Void;
      // Reset to LOD level 0.
    function resetLod(): Void;
      // Return owner terrain.
    function getOwner(): Terrain;
      // Return north neighbor patch.
    function getNorthPatch(): TerrainPatch;
      // Return south neighbor patch.
    function getSouthPatch(): TerrainPatch;
      // Return west neighbor patch.
    function getWestPatch(): TerrainPatch;
      // Return east neighbor patch.
    function getEastPatch(): TerrainPatch;
      // Return patch coordinates.
    function getCoordinates(): IntVector2;
      // Return current LOD level.
    function getLodLevel(): Int;
      // Return vertical offset for occlusion geometry..
    function getOcclusionOffset(): Float;

}



//----------------------------------------------------
// MODULE: Atomic2D
//----------------------------------------------------


@:native("Atomic.AnimatedSprite2D")
extern class AnimatedSprite2D extends StaticSprite2D {

    var speed: Float;
    var animationSet: AnimationSet2D;
    var loopMode: LoopMode2D;
    var animation: String;
    var rootNode: Node;
    var animationAttr: String;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set speed.
    function setSpeed(speed: Float): Void;
      // Set animation by name and loop mode.
    function setAnimation(name: String, ?loopMode: LoopMode2D): Void;
      // Set animation set.
    function setAnimationSet(animationSet: AnimationSet2D): Void;
      // Set loop mode.
    function setLoopMode(loopMode: LoopMode2D): Void;
      // Return speed.
    function getSpeed(): Float;
      // Return animation name.
    function getAnimation(): String;
      // Return animation.
    function getAnimationSet(): AnimationSet2D;
      // Return loop mode.
    function getLoopMode(): LoopMode2D;
      // Return root node.
    function getRootNode(): Node;
      // Set animation by name.
    function setAnimationAttr(name: String): Void;

}

@:native("Atomic.Animation2D")
extern class Animation2D extends RefCounted {

    var name: String;
    var length: Float;
    var looped: Bool;
    var animationSet: AnimationSet2D;
    var numTracks: Int;

      // Construct.
    function new(animationSet: AnimationSet2D);

      // Set name.
    function setName(name: String): Void;
      // Set length.
    function setLength(length: Float): Void;
      // Set looped.
    function setLooped(looped: Bool): Void;
      // Return animation set.
    function getAnimationSet(): AnimationSet2D;
      // Return name.
    function getName(): String;
      // Return length.
    function getLength(): Float;
      // Return looped.
    function isLooped(): Bool;
      // Return number of animation tracks.
    function getNumTracks(): Int;

}

@:native("Atomic.AnimationSet2D")
extern class AnimationSet2D extends Resource {

    var numAnimations: Int;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Get number of animations.
    function getNumAnimations(): Int;

}

@:native("Atomic.CollisionBox2D")
extern class CollisionBox2D extends CollisionShape2D {

    var size: Vector2;
    var angle: Float;
    var center: Vector2;

      // Construct.
    function new();

      // Set size.
    function setSize(size: Vector2): Void;
      // Set angle.
    function setAngle(angle: Float): Void;
      // Return size.
    function getSize(): Vector2;
      // Return center.
    function getCenter(): Vector2;
      // Return angle.
    function getAngle(): Float;

}

@:native("Atomic.CollisionChain2D")
extern class CollisionChain2D extends CollisionShape2D {

    var loop: Bool;
    var vertexCount: Int;

      // Construct.
    function new();

      // Set loop.
    function setLoop(loop: Bool): Void;
      // Set vertex count.
    function setVertexCount(count: Int): Void;
      // Set vertex.
    function setVertex(index: Int, vertex: Vector2): Void;
      // Return loop.
    function getLoop(): Bool;
      // Return vertex count.
    function getVertexCount(): Int;
      // Return vertex.
    function getVertex(index: Int): Vector2;

}

@:native("Atomic.CollisionCircle2D")
extern class CollisionCircle2D extends CollisionShape2D {

    var radius: Float;
    var center: Vector2;

      // Construct.
    function new();

      // Set radius.
    function setRadius(radius: Float): Void;
      // Return radius.
    function getRadius(): Float;
      // Return center.
    function getCenter(): Vector2;

}

@:native("Atomic.CollisionEdge2D")
extern class CollisionEdge2D extends CollisionShape2D {

    var vertex1: Vector2;
    var vertex2: Vector2;

      // Construct.
    function new();

      // Set vertex 1.
    function setVertex1(vertex: Vector2): Void;
      // Set vertex 2.
    function setVertex2(vertex: Vector2): Void;
      // Set vertices.
    function setVertices(vertex1: Vector2, vertex2: Vector2): Void;
      // Return vertex 1.
    function getVertex1(): Vector2;
      // Return vertex 2.
    function getVertex2(): Vector2;

}

@:native("Atomic.CollisionPolygon2D")
extern class CollisionPolygon2D extends CollisionShape2D {

    var vertexCount: Int;

      // Construct.
    function new();

      // Set vertex count.
    function setVertexCount(count: Int): Void;
      // Set vertex.
    function setVertex(index: Int, vertex: Vector2): Void;
      // Return vertex count.
    function getVertexCount(): Int;
      // Return vertex.
    function getVertex(index: Int): Vector2;

}

@:native("Atomic.CollisionShape2D")
extern class CollisionShape2D extends Component {

    var trigger: Bool;
    var categoryBits: Int;
    var maskBits: Int;
    var groupIndex: Int;
    var density: Float;
    var friction: Float;
    var restitution: Float;
    var mass: Float;
    var inertia: Float;
    var massCenter: Vector2;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set trigger.
    function setTrigger(trigger: Bool): Void;
      // Set filter category bits.
    function setCategoryBits(categoryBits: Int): Void;
      // Set filter mask bits.
    function setMaskBits(maskBits: Int): Void;
      // Set filter group index.
    function setGroupIndex(groupIndex: Int): Void;
      // Set density.
    function setDensity(density: Float): Void;
      // Set friction.
    function setFriction(friction: Float): Void;
      // Set restitution .
    function setRestitution(restitution: Float): Void;
      // Create fixture.
    function createFixture(): Void;
      // Release fixture.
    function releaseFixture(): Void;
      // Return trigger.
    function isTrigger(): Bool;
      // Return filter category bits.
    function getCategoryBits(): Int;
      // Return filter mask bits.
    function getMaskBits(): Int;
      // Return filter group index.
    function getGroupIndex(): Int;
      // Return density.
    function getDensity(): Float;
      // Return friction.
    function getFriction(): Float;
      // Return restitution.
    function getRestitution(): Float;
      // Return mass.
    function getMass(): Float;
      // Return inertia.
    function getInertia(): Float;
      // Return mass center.
    function getMassCenter(): Vector2;

}

@:native("Atomic.Constraint2D")
extern class Constraint2D extends Component {

    var otherBody: RigidBody2D;
    var collideConnected: Bool;
    var attachedConstraint: Constraint2D;
    var ownerBody: RigidBody2D;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Create Joint.
    function createJoint(): Void;
      // Release Joint.
    function releaseJoint(): Void;
      // Set other rigid body.
    function setOtherBody(body: RigidBody2D): Void;
      // Set collide connected.
    function setCollideConnected(collideConnected: Bool): Void;
      // Set attached constriant (for gear).
    function setAttachedConstraint(constraint: Constraint2D): Void;
      // Return owner body.
    function getOwnerBody(): RigidBody2D;
      // Return other body.
    function getOtherBody(): RigidBody2D;
      // Return collide connected.
    function getCollideConnected(): Bool;
      // Return attached constraint (for gear).
    function getAttachedConstraint(): Constraint2D;

}

@:native("Atomic.ConstraintDistance2D")
extern class ConstraintDistance2D extends Constraint2D {

    var ownerBodyAnchor: Vector2;
    var otherBodyAnchor: Vector2;
    var frequencyHz: Float;
    var dampingRatio: Float;

      // Construct.
    function new();

      // Set owner body anchor.
    function setOwnerBodyAnchor(anchor: Vector2): Void;
      // Set other body anchor.
    function setOtherBodyAnchor(anchor: Vector2): Void;
      // Set frequency Hz.
    function setFrequencyHz(frequencyHz: Float): Void;
      // Set damping ratio.
    function setDampingRatio(dampingRatio: Float): Void;
      // Return owner body anchor.
    function getOwnerBodyAnchor(): Vector2;
      // Return other body anchor.
    function getOtherBodyAnchor(): Vector2;
      // Return frequency Hz.
    function getFrequencyHz(): Float;
      // Return damping ratio.
    function getDampingRatio(): Float;

}

@:native("Atomic.ConstraintFriction2D")
extern class ConstraintFriction2D extends Constraint2D {

    var anchor: Vector2;
    var maxForce: Float;
    var maxTorque: Float;

      // Construct.
    function new();

      // Set anchor.
    function setAnchor(anchor: Vector2): Void;
      // Set max force.
    function setMaxForce(maxForce: Float): Void;
      // Set max torque.
    function setMaxTorque(maxTorque: Float): Void;
      // Return anchor.
    function getAnchor(): Vector2;
      // Set max force.
    function getMaxForce(): Float;
      // Set max torque.
    function getMaxTorque(): Float;

}

@:native("Atomic.ConstraintGear2D")
extern class ConstraintGear2D extends Constraint2D {

    var ownerConstraint: Constraint2D;
    var otherConstraint: Constraint2D;
    var ratio: Float;

      // Construct.
    function new();

      // Set owner constraint.
    function setOwnerConstraint(constraint: Constraint2D): Void;
      // Set other constraint.
    function setOtherConstraint(constraint: Constraint2D): Void;
      // Set ratio.
    function setRatio(ratio: Float): Void;
      // Return owner constraint.
    function getOwnerConstraint(): Constraint2D;
      // Return other constraint.
    function getOtherConstraint(): Constraint2D;
      // Return ratio.
    function getRatio(): Float;

}

@:native("Atomic.ConstraintMotor2D")
extern class ConstraintMotor2D extends Constraint2D {

    var linearOffset: Vector2;
    var angularOffset: Float;
    var maxForce: Float;
    var maxTorque: Float;
    var correctionFactor: Float;

      // Construct.
    function new();

      // Set linear offset.
    function setLinearOffset(linearOffset: Vector2): Void;
      // Set angular offset.
    function setAngularOffset(angularOffset: Float): Void;
      // Set max force.
    function setMaxForce(maxForce: Float): Void;
      // Set max torque.
    function setMaxTorque(maxTorque: Float): Void;
      // Set correction factor.
    function setCorrectionFactor(correctionFactor: Float): Void;
      // Return linear offset.
    function getLinearOffset(): Vector2;
      // Return angular offset.
    function getAngularOffset(): Float;
      // Return max force.
    function getMaxForce(): Float;
      // Return max torque.
    function getMaxTorque(): Float;
      // Return correction factor.
    function getCorrectionFactor(): Float;

}

@:native("Atomic.ConstraintMouse2D")
extern class ConstraintMouse2D extends Constraint2D {

    var target: Vector2;
    var maxForce: Float;
    var frequencyHz: Float;
    var dampingRatio: Float;

      // Construct.
    function new();

      // Set target.
    function setTarget(target: Vector2): Void;
      // Set max force.
    function setMaxForce(maxForce: Float): Void;
      // Set frequency Hz.
    function setFrequencyHz(frequencyHz: Float): Void;
      // Set damping ratio.
    function setDampingRatio(dampingRatio: Float): Void;
      // Return target.
    function getTarget(): Vector2;
      // Return max force.
    function getMaxForce(): Float;
      // Return frequency Hz.
    function getFrequencyHz(): Float;
      // Return damping ratio.
    function getDampingRatio(): Float;

}

@:native("Atomic.ConstraintPrismatic2D")
extern class ConstraintPrismatic2D extends Constraint2D {

    var anchor: Vector2;
    var axis: Vector2;
    var enableLimit: Bool;
    var lowerTranslation: Float;
    var upperTranslation: Float;
    var enableMotor: Bool;
    var maxMotorForce: Float;
    var motorSpeed: Float;

      // Construct.
    function new();

      // Set anchor.
    function setAnchor(anchor: Vector2): Void;
      // Set axis.
    function setAxis(axis: Vector2): Void;
      // Set enable limit.
    function setEnableLimit(enableLimit: Bool): Void;
      // Set lower translation.
    function setLowerTranslation(lowerTranslation: Float): Void;
      // Set upper translation.
    function setUpperTranslation(upperTranslation: Float): Void;
      // Set enable motor.
    function setEnableMotor(enableMotor: Bool): Void;
      // Set maxmotor force.
    function setMaxMotorForce(maxMotorForce: Float): Void;
      // Set motor speed.
    function setMotorSpeed(motorSpeed: Float): Void;
      // Return anchor.
    function getAnchor(): Vector2;
      // Return axis.
    function getAxis(): Vector2;
      // Return enable limit.
    function getEnableLimit(): Bool;
      // Return lower translation.
    function getLowerTranslation(): Float;
      // Return upper translation.
    function getUpperTranslation(): Float;
      // Return enable motor.
    function getEnableMotor(): Bool;
      // Return maxmotor force.
    function getMaxMotorForce(): Float;
      // Return motor speed.
    function getMotorSpeed(): Float;

}

@:native("Atomic.ConstraintPulley2D")
extern class ConstraintPulley2D extends Constraint2D {

    var ownerBodyGroundAnchor: Vector2;
    var otherBodyGroundAnchor: Vector2;
    var ownerBodyAnchor: Vector2;
    var otherBodyAnchor: Vector2;
    var ratio: Float;

      // Construct.
    function new();

      // Set other body ground anchor point.
    function setOwnerBodyGroundAnchor(groundAnchor: Vector2): Void;
      // Set other body ground anchor point.
    function setOtherBodyGroundAnchor(groundAnchor: Vector2): Void;
      // Set owner body anchor point.
    function setOwnerBodyAnchor(anchor: Vector2): Void;
      // Set other body anchor point.
    function setOtherBodyAnchor(anchor: Vector2): Void;
      // Set ratio.
    function setRatio(ratio: Float): Void;
      // Return owner body ground anchor.
    function getOwnerBodyGroundAnchor(): Vector2;
      // return other body ground anchor.
    function getOtherBodyGroundAnchor(): Vector2;
      // Return owner body anchor.
    function getOwnerBodyAnchor(): Vector2;
      // Return other body anchor.
    function getOtherBodyAnchor(): Vector2;
      // Return ratio.
    function getRatio(): Float;

}

@:native("Atomic.ConstraintRevolute2D")
extern class ConstraintRevolute2D extends Constraint2D {

    var anchor: Vector2;
    var enableLimit: Bool;
    var lowerAngle: Float;
    var upperAngle: Float;
    var enableMotor: Bool;
    var motorSpeed: Float;
    var maxMotorTorque: Float;

      // Construct.
    function new();

      // Set anchor.
    function setAnchor(anchor: Vector2): Void;
      // Set enable limit.
    function setEnableLimit(enableLimit: Bool): Void;
      // Set lower angle.
    function setLowerAngle(lowerAngle: Float): Void;
      // Set upper angle.
    function setUpperAngle(upperAngle: Float): Void;
      // Set enable motor.
    function setEnableMotor(enableMotor: Bool): Void;
      // Set motor speed.
    function setMotorSpeed(motorSpeed: Float): Void;
      // Set max motor torque.
    function setMaxMotorTorque(maxMotorTorque: Float): Void;
      // Return anchor.
    function getAnchor(): Vector2;
      // Return enable limit.
    function getEnableLimit(): Bool;
      // Return lower angle.
    function getLowerAngle(): Float;
      // Return upper angle.
    function getUpperAngle(): Float;
      // Return enable motor.
    function getEnableMotor(): Bool;
      // Return motor speed.
    function getMotorSpeed(): Float;
      // Return max motor torque.
    function getMaxMotorTorque(): Float;

}

@:native("Atomic.ConstraintRope2D")
extern class ConstraintRope2D extends Constraint2D {

    var ownerBodyAnchor: Vector2;
    var otherBodyAnchor: Vector2;
    var maxLength: Float;

      // Construct.
    function new();

      // Set owner body anchor.
    function setOwnerBodyAnchor(anchor: Vector2): Void;
      // Set other body anchor.
    function setOtherBodyAnchor(anchor: Vector2): Void;
      // Set max length.
    function setMaxLength(maxLength: Float): Void;
      // Return owner body anchor.
    function getOwnerBodyAnchor(): Vector2;
      // Return other body anchor.
    function getOtherBodyAnchor(): Vector2;
      // Return max length.
    function getMaxLength(): Float;

}

@:native("Atomic.ConstraintWeld2D")
extern class ConstraintWeld2D extends Constraint2D {

    var anchor: Vector2;
    var frequencyHz: Float;
    var dampingRatio: Float;

      // Construct.
    function new();

      // Set anchor.
    function setAnchor(anchor: Vector2): Void;
      // Set frequency Hz.
    function setFrequencyHz(frequencyHz: Float): Void;
      // Set damping ratio.
    function setDampingRatio(dampingRatio: Float): Void;
      // Return anchor.
    function getAnchor(): Vector2;
      // Return frequency Hz.
    function getFrequencyHz(): Float;
      // Return damping ratio.
    function getDampingRatio(): Float;

}

@:native("Atomic.ConstraintWheel2D")
extern class ConstraintWheel2D extends Constraint2D {

    var anchor: Vector2;
    var axis: Vector2;
    var enableMotor: Bool;
    var maxMotorTorque: Float;
    var motorSpeed: Float;
    var frequencyHz: Float;
    var dampingRatio: Float;

      // Construct.
    function new();

      // Set anchor.
    function setAnchor(anchor: Vector2): Void;
      // Set axis.
    function setAxis(axis: Vector2): Void;
      // Set enable motor.
    function setEnableMotor(enableMotor: Bool): Void;
      // Set max motor torque.
    function setMaxMotorTorque(maxMotorTorque: Float): Void;
      // Set motor speed.
    function setMotorSpeed(motorSpeed: Float): Void;
      // Set frequency Hz.
    function setFrequencyHz(frequencyHz: Float): Void;
      // Set damping ratio.
    function setDampingRatio(dampingRatio: Float): Void;
      // Return anchor.
    function getAnchor(): Vector2;
      // Return axis.
    function getAxis(): Vector2;
      // Return enable motor.
    function getEnableMotor(): Bool;
      // Return maxMotor torque.
    function getMaxMotorTorque(): Float;
      // Return motor speed.
    function getMotorSpeed(): Float;
      // Return frequency Hz.
    function getFrequencyHz(): Float;
      // Return damping ratio.
    function getDampingRatio(): Float;

}

@:native("Atomic.Drawable2D")
extern class Drawable2D extends Drawable {

    var layer: Int;
    var orderInLayer: Int;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set layer.
    function setLayer(layer: Int): Void;
      // Set order in layer.
    function setOrderInLayer(orderInLayer: Int): Void;
      // Return layer.
    function getLayer(): Int;
      // Return order in layer.
    function getOrderInLayer(): Int;

}

@:native("Atomic.Light2D")
extern class Light2D extends Component {

    var lightGroupID: Int;
    var color: Color;
    var numRays: Int;
    var lightType: LightType2D;
    var castShadows: Bool;
    var softShadows: Bool;
    var softShadowLength: Float;
    var backtrace: Bool;

      // Construct.
    function new();

    function setLightGroupID(id: Int): Void;
    function getLightGroupID(): Int;
    function getColor(): Color;
    function setColor(color: Color): Void;
    function updateVertices(): Void;
    function setNumRays(numRays: Int): Void;
    function getNumRays(): Int;
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
    function getLightType(): LightType2D;
    function getCastShadows(): Bool;
    function setCastShadows(castShadows: Bool): Void;
    function getSoftShadows(): Bool;
    function setSoftShadows(softShadows: Bool): Void;
    function getSoftShadowLength(): Float;
    function setSoftShadowLength(softShadowLength: Float): Void;
    function getBacktrace(): Bool;
    function setBacktrace(backtrace: Bool): Void;

}

@:native("Atomic.DirectionalLight2D")
extern class DirectionalLight2D extends Light2D {

    var direction: Float;

      // Construct.
    function new();

    @:overload(function(): Void{})
    override function updateVertices(): Void;
    function getDirection(): Float;
    function setDirection(direction: Float): Void;

}

@:native("Atomic.PositionalLight2D")
extern class PositionalLight2D extends Light2D {

      // Construct.
    function new();

    @:overload(function(): Void{})
    override function updateVertices(): Void;

}

@:native("Atomic.PointLight2D")
extern class PointLight2D extends PositionalLight2D {

    var radius: Float;

      // Construct.
    function new();

    @:overload(function(): Void{})
    override function updateVertices(): Void;
    function setRadius(radius: Float): Void;
    function getRadius(): Float;

}

@:native("Atomic.Light2DGroup")
extern class Light2DGroup extends Drawable2D {

    var physicsWorld: PhysicsWorld2D;
    var ambientColor: Color;
    var lightGroupID: Int;
    var frustumBox: BoundingBox;

      // Construct.
    function new();

    function getPhysicsWorld(): PhysicsWorld2D;
    function addLight2D(light: Light2D): Void;
    function removeLight2D(light: Light2D): Void;
    function setDirty(): Void;
    function setAmbientColor(color: Color): Void;
    function getAmbientColor(): Color;
    function setLightGroupID(id: Int): Void;
    function getLightGroupID(): Int;
    function getFrustumBox(): BoundingBox;

}

@:native("Atomic.ParticleEffect2D")
extern class ParticleEffect2D extends Resource {

    var sprite: Sprite2D;
    var sourcePositionVariance: Vector2;
    var speed: Float;
    var speedVariance: Float;
    var particleLifeSpan: Float;
    var particleLifespanVariance: Float;
    var angle: Float;
    var angleVariance: Float;
    var gravity: Vector2;
    var radialAcceleration: Float;
    var tangentialAcceleration: Float;
    var radialAccelVariance: Float;
    var tangentialAccelVariance: Float;
    var startColor: Color;
    var startColorVariance: Color;
    var finishColor: Color;
    var finishColorVariance: Color;
    var maxParticles: Int;
    var startParticleSize: Float;
    var startParticleSizeVariance: Float;
    var finishParticleSize: Float;
    var finishParticleSizeVariance: Float;
    var duration: Float;
    var emitterType: EmitterType2D;
    var maxRadius: Float;
    var maxRadiusVariance: Float;
    var minRadius: Float;
    var minRadiusVariance: Float;
    var rotatePerSecond: Float;
    var rotatePerSecondVariance: Float;
    var blendMode: BlendMode;
    var rotationStart: Float;
    var rotationStartVariance: Float;
    var rotationEnd: Float;
    var rotationEndVariance: Float;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Set sprite.
    function setSprite(sprite: Sprite2D): Void;
      // Set source position variance.
    function setSourcePositionVariance(sourcePositionVariance: Vector2): Void;
      // Set speed.
    function setSpeed(speed: Float): Void;
      // Set speed variance.
    function setSpeedVariance(speedVariance: Float): Void;
      // Set particle lifespan.
    function setParticleLifeSpan(particleLifeSpan: Float): Void;
      // Set particle lifespan variance.
    function setParticleLifespanVariance(particleLifespanVariance: Float): Void;
      // Set angle.
    function setAngle(angle: Float): Void;
      // Set angle variance.
    function setAngleVariance(angleVariance: Float): Void;
      // Set gravity.
    function setGravity(gravity: Vector2): Void;
      // Set radial acceleration.
    function setRadialAcceleration(radialAcceleration: Float): Void;
      // Set tangential acceleration.
    function setTangentialAcceleration(tangentialAcceleration: Float): Void;
      // Set radial acceleration variance.
    function setRadialAccelVariance(radialAccelVariance: Float): Void;
      // Set tangential acceleration variance.
    function setTangentialAccelVariance(tangentialAccelVariance: Float): Void;
      // Set start color.
    function setStartColor(startColor: Color): Void;
      // Set start color variance.
    function setStartColorVariance(startColorVariance: Color): Void;
      // Set finish color.
    function setFinishColor(finishColor: Color): Void;
      // Set finish color variance.
    function setFinishColorVariance(finishColorVariance: Color): Void;
      // Set max particles.
    function setMaxParticles(maxParticles: Int): Void;
      // Set start particle size.
    function setStartParticleSize(startParticleSize: Float): Void;
      // Set start particle size variance.
    function setStartParticleSizeVariance(startParticleSizeVariance: Float): Void;
      // Set finish particle size.
    function setFinishParticleSize(finishParticleSize: Float): Void;
      // Set finish particle size variance.
    function setFinishParticleSizeVariance(FinishParticleSizeVariance: Float): Void;
      // Set duration.
    function setDuration(duration: Float): Void;
      // Set emitter type.
    function setEmitterType(emitterType: EmitterType2D): Void;
      // Set max radius.
    function setMaxRadius(maxRadius: Float): Void;
      // Set max radius variance.
    function setMaxRadiusVariance(maxRadiusVariance: Float): Void;
      // Set min radius.
    function setMinRadius(minRadius: Float): Void;
      // Set min radius variance.
    function setMinRadiusVariance(minRadiusVariance: Float): Void;
      // Set rotate per second.
    function setRotatePerSecond(rotatePerSecond: Float): Void;
      // Set rotate per second variance.
    function setRotatePerSecondVariance(rotatePerSecondVariance: Float): Void;
      // Set blend mode.
    function setBlendMode(blendMode: BlendMode): Void;
      // Set rotation start.
    function setRotationStart(rotationStart: Float): Void;
      // Set rotation start variance.
    function setRotationStartVariance(rotationStartVariance: Float): Void;
      // Set rotation end.
    function setRotationEnd(rotationEnd: Float): Void;
      // Set rotation end variance.
    function setRotationEndVariance(rotationEndVariance: Float): Void;
      // Return sprite.
    function getSprite(): Sprite2D;
      // Return source position variance.
    function getSourcePositionVariance(): Vector2;
      // Return speed.
    function getSpeed(): Float;
      // Return speed variance.
    function getSpeedVariance(): Float;
      // Return particle lifespan.
    function getParticleLifeSpan(): Float;
      // Return particle lifespan variance.
    function getParticleLifespanVariance(): Float;
      // Return angle.
    function getAngle(): Float;
      // Return angle variance.
    function getAngleVariance(): Float;
      // Return gravity.
    function getGravity(): Vector2;
      // Return radial acceleration.
    function getRadialAcceleration(): Float;
      // Return tangential acceleration.
    function getTangentialAcceleration(): Float;
      // Return radial acceleration variance.
    function getRadialAccelVariance(): Float;
      // Return tangential acceleration variance.
    function getTangentialAccelVariance(): Float;
      // Return start color.
    function getStartColor(): Color;
      // Return start color variance.
    function getStartColorVariance(): Color;
      // Return finish color.
    function getFinishColor(): Color;
      // Return finish color variance.
    function getFinishColorVariance(): Color;
      // Return max particles.
    function getMaxParticles(): Int;
      // Return start particle size.
    function getStartParticleSize(): Float;
      // Return start particle size variance.
    function getStartParticleSizeVariance(): Float;
      // Return finish particle size.
    function getFinishParticleSize(): Float;
      // Return finish particle size variance.
    function getFinishParticleSizeVariance(): Float;
      // Return duration.
    function getDuration(): Float;
      // Return emitter type.
    function getEmitterType(): EmitterType2D;
      // Return max radius.
    function getMaxRadius(): Float;
      // Return max radius variance.
    function getMaxRadiusVariance(): Float;
      // Return min radius.
    function getMinRadius(): Float;
      // Return min radius variance.
    function getMinRadiusVariance(): Float;
      // Return rotate per second.
    function getRotatePerSecond(): Float;
      // Return rotate per second variance.
    function getRotatePerSecondVariance(): Float;
      // Return blend mode.
    function getBlendMode(): BlendMode;
      // Return rotation start.
    function getRotationStart(): Float;
      // Return rotation start variance.
    function getRotationStartVariance(): Float;
      // Return rotation end.
    function getRotationEnd(): Float;
      // Return rotation end variance.
    function getRotationEndVariance(): Float;

}

@:native("Atomic.ParticleEmitter2D")
extern class ParticleEmitter2D extends Drawable2D {

    var effect: ParticleEffect2D;
    var sprite: Sprite2D;
    var blendMode: BlendMode;
    var maxParticles: Int;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set particle effect.
    function setEffect(effect: ParticleEffect2D): Void;
      // Set sprite.
    function setSprite(sprite: Sprite2D): Void;
      // Set blend mode.
    function setBlendMode(blendMode: BlendMode): Void;
      // Set max particles.
    function setMaxParticles(maxParticles: Int): Void;
      // Return particle effect.
    function getEffect(): ParticleEffect2D;
      // Return sprite.
    function getSprite(): Sprite2D;
      // Return blend mode.
    function getBlendMode(): BlendMode;
      // Return max particles.
    function getMaxParticles(): Int;

}

@:native("Atomic.PhysicsWorld2D")
extern class PhysicsWorld2D extends Component {

    var drawShape: Bool;
    var drawJoint: Bool;
    var drawAabb: Bool;
    var drawPair: Bool;
    var drawCenterOfMass: Bool;
    var allowSleeping: Bool;
    var warmStarting: Bool;
    var continuousPhysics: Bool;
    var subStepping: Bool;
    var gravity: Vector2;
    var autoClearForces: Bool;
    var velocityIterations: Int;
    var positionIterations: Int;
    var applyingTransforms: Bool;

      // Construct.
    function new();

      // Step the simulation forward.
    function update(timeStep: Float): Void;
      // Add debug geometry to the debug renderer.
    @:overload(function(): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set draw shape.
    function setDrawShape(drawShape: Bool): Void;
      // Set draw joint.
    function setDrawJoint(drawJoint: Bool): Void;
      // Set draw aabb.
    function setDrawAabb(drawAabb: Bool): Void;
      // Set draw pair.
    function setDrawPair(drawPair: Bool): Void;
      // Set draw center of mass.
    function setDrawCenterOfMass(drawCenterOfMass: Bool): Void;
      // Set allow sleeping.
    function setAllowSleeping(enable: Bool): Void;
      // Set warm starting.
    function setWarmStarting(enable: Bool): Void;
      // Set continuous physics.
    function setContinuousPhysics(enable: Bool): Void;
      // Set sub stepping.
    function setSubStepping(enable: Bool): Void;
      // Set gravity.
    function setGravity(gravity: Vector2): Void;
      // Set auto clear forces.
    function setAutoClearForces(enable: Bool): Void;
      // Set velocity iterations.
    function setVelocityIterations(velocityIterations: Int): Void;
      // Set position iterations.
    function setPositionIterations(positionIterations: Int): Void;
      // Add rigid body.
    function addRigidBody(rigidBody: RigidBody2D): Void;
      // Remove rigid body.
    function removeRigidBody(rigidBody: RigidBody2D): Void;
      // Return draw shape.
    function getDrawShape(): Bool;
      // Return draw joint.
    function getDrawJoint(): Bool;
      // Return draw aabb.
    function getDrawAabb(): Bool;
      // Return draw pair.
    function getDrawPair(): Bool;
      // Return draw center of mass.
    function getDrawCenterOfMass(): Bool;
      // Return allow sleeping.
    function getAllowSleeping(): Bool;
      // Return warm starting.
    function getWarmStarting(): Bool;
      // Return continuous physics.
    function getContinuousPhysics(): Bool;
      // Return sub stepping.
    function getSubStepping(): Bool;
      // Return auto clear forces.
    function getAutoClearForces(): Bool;
      // Return gravity.
    function getGravity(): Vector2;
      // Return velocity iterations.
    function getVelocityIterations(): Int;
      // Return position iterations.
    function getPositionIterations(): Int;
      // Set node dirtying to be disregarded.
    function setApplyingTransforms(enable: Bool): Void;
      // Return whether node dirtying should be disregarded.
    function isApplyingTransforms(): Bool;

}

@:native("Atomic.RigidBody2D")
extern class RigidBody2D extends Component {

    var bodyType: BodyType2D;
    var mass: Float;
    var inertia: Float;
    var massCenter: Vector2;
    var useFixtureMass: Bool;
    var linearDamping: Float;
    var angularDamping: Float;
    var allowSleep: Bool;
    var fixedRotation: Bool;
    var bullet: Bool;
    var gravityScale: Float;
    var awake: Bool;
    var linearVelocity: Vector2;
    var angularVelocity: Float;
    var castShadows: Bool;

      // Construct.
    function new();

      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set body type.
    function setBodyType(bodyType: BodyType2D): Void;
      // Set Mass.
    function setMass(mass: Float): Void;
      // Set inertia.
    function setInertia(inertia: Float): Void;
      // Set mass center.
    function setMassCenter(center: Vector2): Void;
      // Use fixture mass (default is true).
    function setUseFixtureMass(useFixtureMass: Bool): Void;
      // Set linear damping.
    function setLinearDamping(linearDamping: Float): Void;
      // Set angular damping.
    function setAngularDamping(angularDamping: Float): Void;
      // Set allow sleep.
    function setAllowSleep(allowSleep: Bool): Void;
      // Set fixed rotation.
    function setFixedRotation(fixedRotation: Bool): Void;
      // Set bullet.
    function setBullet(bullet: Bool): Void;
      // Set gravity scale.
    function setGravityScale(gravityScale: Float): Void;
      // Set awake.
    function setAwake(awake: Bool): Void;
      // Set linear velocity.
    function setLinearVelocity(linearVelocity: Vector2): Void;
      // Set angular velocity.
    function setAngularVelocity(angularVelocity: Float): Void;
      // Apply force.
    function applyForce(force: Vector2, point: Vector2, wake: Bool): Void;
      // Apply force to center.
    function applyForceToCenter(force: Vector2, wake: Bool): Void;
      // Apply Torque.
    function applyTorque(torque: Float, wake: Bool): Void;
      // Apply linear impulse.
    function applyLinearImpulse(impulse: Vector2, point: Vector2, wake: Bool): Void;
      // Apply angular impulse.
    function applyAngularImpulse(impulse: Float, wake: Bool): Void;
      // Create body.
    function createBody(): Void;
      // Release body.
    function releaseBody(): Void;
      // Apply world transform.
    function applyWorldTransform(): Void;
      // Add collision shape.
    function addCollisionShape2D(collisionShape: CollisionShape2D): Void;
      // Remove collision shape.
    function removeCollisionShape2D(collisionShape: CollisionShape2D): Void;
      // Add constraint.
    function addConstraint2D(constraint: Constraint2D): Void;
      // Remove constraint.
    function removeConstraint2D(constraint: Constraint2D): Void;
      // Return body type.
    function getBodyType(): BodyType2D;
      // Return Mass.
    function getMass(): Float;
      // Return inertia.
    function getInertia(): Float;
      // Return mass center.
    function getMassCenter(): Vector2;
      // Return use fixture mass.
    function getUseFixtureMass(): Bool;
      // Return linear damping.
    function getLinearDamping(): Float;
      // Return angular damping.
    function getAngularDamping(): Float;
      // Return allow sleep.
    function isAllowSleep(): Bool;
      // Return fixed rotation.
    function isFixedRotation(): Bool;
      // Return bullet.
    function isBullet(): Bool;
      // Return gravity scale.
    function getGravityScale(): Float;
      // Return awake.
    function isAwake(): Bool;
      // Return linear velocity.
    function getLinearVelocity(): Vector2;
      // Return angular velocity.
    function getAngularVelocity(): Float;
    function getCastShadows(): Bool;
    function setCastShadows(castShadows: Bool): Void;

}

@:native("Atomic.Sprite2D")
extern class Sprite2D extends Resource {

    var texture: Texture2D;
    var rectangle: IntRect;
    var hotSpot: Vector2;
    var offset: IntVector2;
    var spriteSheet: SpriteSheet2D;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Set texture.
    function setTexture(texture: Texture2D): Void;
      // Set rectangle.
    function setRectangle(rectangle: IntRect): Void;
      // Set hot spot.
    function setHotSpot(hotSpot: Vector2): Void;
      // Set offset.
    function setOffset(offset: IntVector2): Void;
      // Set sprite sheet.
    function setSpriteSheet(spriteSheet: SpriteSheet2D): Void;
      // Return texture.
    function getTexture(): Texture2D;
      // Return rectangle.
    function getRectangle(): IntRect;
      // Return hot spot.
    function getHotSpot(): Vector2;
      // Return offset.
    function getOffset(): IntVector2;
      // Return sprite sheet.
    function getSpriteSheet(): SpriteSheet2D;
      // Return texture rectangle.
    function getTextureRectangle(rect: Rect, ?flipX: Bool, ?flipY: Bool): Bool;

}

@:native("Atomic.SpriteSheet2D")
extern class SpriteSheet2D extends Resource {

    var texture: Texture2D;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Return texture.
    function getTexture(): Texture2D;
      // Return sprite.
    function getSprite(name: String): Sprite2D;
      // Define sprite.
    function defineSprite(name: String, rectangle: IntRect, ?hotSpot: Vector2, ?offset: IntVector2): Void;

}

@:native("Atomic.StaticSprite2D")
extern class StaticSprite2D extends Drawable2D {

    var sprite: Sprite2D;
    var blendMode: BlendMode;
    var flipX: Bool;
    var flipY: Bool;
    var color: Color;
    var alpha: Float;
    var useHotSpot: Bool;
    var hotSpot: Vector2;
    var customMaterial: Material;

      // Construct.
    function new();

      // Set sprite.
    function setSprite(sprite: Sprite2D): Void;
      // Set blend mode.
    function setBlendMode(blendMode: BlendMode): Void;
      // Set flip.
    function setFlip(flipX: Bool, flipY: Bool): Void;
      // Set flip X.
    function setFlipX(flipX: Bool): Void;
      // Set flip Y.
    function setFlipY(flipY: Bool): Void;
      // Set color.
    function setColor(color: Color): Void;
      // Set alpha.
    function setAlpha(alpha: Float): Void;
      // Set use hot spot.
    function setUseHotSpot(useHotSpot: Bool): Void;
      // Set hot spot.
    function setHotSpot(hotspot: Vector2): Void;
      // Set custom material.
    function setCustomMaterial(customMaterial: Material): Void;
      // Return sprite.
    function getSprite(): Sprite2D;
      // Return blend mode.
    function getBlendMode(): BlendMode;
      // Return flip X.
    function getFlipX(): Bool;
      // Return flip Y.
    function getFlipY(): Bool;
      // Return color.
    function getColor(): Color;
      // Return alpha.
    function getAlpha(): Float;
      // Return use hot spot.
    function getUseHotSpot(): Bool;
      // Return hot spot.
    function getHotSpot(): Vector2;
      // Return custom material.
    function getCustomMaterial(): Material;

}

@:native("Atomic.TileMap2D")
extern class TileMap2D extends Component {

    var tmxFile: TmxFile2D;
    var numLayers: Int;

      // Construct.
    function new();

      // Set tmx file.
    function setTmxFile(tmxFile: TmxFile2D): Void;
      // Return tmx file.
    function getTmxFile(): TmxFile2D;
      // Return number of layers.
    function getNumLayers(): Int;
      // Return tile map layer at index.
    function getLayer(index: Int): TileMapLayer2D;
    function getLayerByName(name: String): TileMapLayer2D;
      // Convert tile index to position.
    function tileIndexToPosition(x: Int, y: Int): Vector2;

}

@:native("Atomic.PropertySet2D")
extern class PropertySet2D extends RefCounted {

    function new();

      // Return has property.
    function hasProperty(name: String): Bool;
      // Return property value.
    function getProperty(name: String): String;

}

@:native("Atomic.Tile2D")
extern class Tile2D extends RefCounted {

    var gid: Int;
    var sprite: Sprite2D;
    var objectGroup: TmxObjectGroup2D;

      // Construct.
    function new();

      // Return gid.
    function getGid(): Int;
      // Return sprite.
    function getSprite(): Sprite2D;
      // Return Object Group.
    function getObjectGroup(): TmxObjectGroup2D;
      // Return has property.
    function hasProperty(name: String): Bool;
      // Return property.
    function getProperty(name: String): String;

}

@:native("Atomic.TileMapObject2D")
extern class TileMapObject2D extends RefCounted {

    var objectType: TileMapObjectType2D;
    var name: String;
    var type: String;
    var position: Vector2;
    var size: Vector2;
    var numPoints: Int;
    var tileGid: Int;
    var tileSprite: Sprite2D;

    function new();

      // Return type.
    function getObjectType(): TileMapObjectType2D;
      // Return name.
    function getName(): String;
      // Return type.
    function getType(): String;
      // Return position.
    function getPosition(): Vector2;
      // Return size (for rectangle and ellipse).
    function getSize(): Vector2;
      // Return number of points (use for script).
    function getNumPoints(): Int;
      // Return point at index (use for script).
    function getPoint(index: Int): Vector2;
      // Return tile Gid.
    function getTileGid(): Int;
      // Return tile sprite.
    function getTileSprite(): Sprite2D;
      // Return has property.
    function hasProperty(name: String): Bool;
      // Return property value.
    function getProperty(name: String): String;
    function validCollisionShape(): Bool;
    function createCollisionShape(node: Node): CollisionShape2D;

}

@:native("Atomic.TileMapLayer2D")
extern class TileMapLayer2D extends Component {

    var drawOrder: Int;
    var visible: Bool;
    var tileMap: TileMap2D;
    var tmxLayer: TmxLayer2D;
    var layerType: TileMapLayerType2D;
    var width: Int;
    var height: Int;
    var numObjects: Int;
    var imageNode: Node;
    var name: String;

      // Construct.
    function new();

      // Add debug geometry to the debug renderer.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Initialize with tile map and tmx layer.
    function initialize(tileMap: TileMap2D, tmxLayer: TmxLayer2D): Void;
      // Set draw order
    function setDrawOrder(drawOrder: Int): Void;
      // Set visible.
    function setVisible(visible: Bool): Void;
      // Return tile map.
    function getTileMap(): TileMap2D;
      // Return tmx layer.
    function getTmxLayer(): TmxLayer2D;
      // Return draw order.
    function getDrawOrder(): Int;
      // Return visible.
    function isVisible(): Bool;
      // Return has property
    function hasProperty(name: String): Bool;
      // Return property.
    function getProperty(name: String): String;
      // Return layer type.
    function getLayerType(): TileMapLayerType2D;
      // Return width (for tile layer only).
    function getWidth(): Int;
      // Return height (for tile layer only).
    function getHeight(): Int;
      // Return tile node (for tile layer only).
    function getTileNode(x: Int, y: Int): Node;
      // Return tile (for tile layer only).
    function getTile(x: Int, y: Int): Tile2D;
      // Return number of tile map objects (for object group only).
    function getNumObjects(): Int;
      // Return tile map object (for object group only).
    function getObject(index: Int): TileMapObject2D;
      // Return object node (for object group only).
    function getObjectNode(index: Int): Node;
      // Return image node (for image layer only).
    function getImageNode(): Node;
    function getName(): String;

}

@:native("Atomic.TmxLayer2D")
extern class TmxLayer2D extends RefCounted {

    var tmxFile: TmxFile2D;
    var type: TileMapLayerType2D;
    var name: String;
    var width: Int;
    var height: Int;

    function new(tmxFile: TmxFile2D, type: TileMapLayerType2D);

      // Return tmx file.
    function getTmxFile(): TmxFile2D;
      // Return type.
    function getType(): TileMapLayerType2D;
      // Return name.
    function getName(): String;
      // Return width.
    function getWidth(): Int;
      // Return height.
    function getHeight(): Int;
      // Return is visible.
    function isVisible(): Bool;
      // Return has property (use for script).
    function hasProperty(name: String): Bool;
      // Return property value (use for script).
    function getProperty(name: String): String;

}

@:native("Atomic.TmxTileLayer2D")
extern class TmxTileLayer2D extends TmxLayer2D {

    function new(tmxFile: TmxFile2D);

      // Return tile.
    function getTile(x: Int, y: Int): Tile2D;

}

@:native("Atomic.TmxObjectGroup2D")
extern class TmxObjectGroup2D extends TmxLayer2D {

    var numObjects: Int;

    function new(tmxFile: TmxFile2D);

      // Return number of objects.
    function getNumObjects(): Int;
      // Return tile map object at index.
    function getObject(index: Int): TileMapObject2D;

}

@:native("Atomic.TmxImageLayer2D")
extern class TmxImageLayer2D extends TmxLayer2D {

    var position: Vector2;
    var source: String;
    var sprite: Sprite2D;

    function new(tmxFile: TmxFile2D);

      // Return position.
    function getPosition(): Vector2;
      // Return source.
    function getSource(): String;
      // Return sprite.
    function getSprite(): Sprite2D;

}

@:native("Atomic.TmxFile2D")
extern class TmxFile2D extends Resource {

    var numLayers: Int;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    @:overload(function(): Bool{})
    override function endLoad(): Bool;
      // Return tile sprite by gid, if not exist return 0.
    function getTileSprite(gid: Int): Sprite2D;
      // Return tile property set by gid, if not exist return 0.
    function getTilePropertySet(gid: Int): PropertySet2D;
      // Return tile object group by gid, if not exist return 0.
    function getTileObjectGroup(gid: Int): TmxObjectGroup2D;
      // Return number of layers.
    function getNumLayers(): Int;
      // Return layer at index.
    function getLayer(index: Int): TmxLayer2D;

}



//----------------------------------------------------
// MODULE: Audio
//----------------------------------------------------


@:native("Atomic.Sound")
extern class Sound extends Resource {

    var size: Int;
    var looped: Bool;
    var length: Float;
    var dataSize: Int;
    var sampleSize: Int;
    var frequency: Float;
    var intFrequency: Int;

      // Construct.
    function new();

      // Set sound size in bytes. Also resets the sound to be uncompressed and one-shot.
    function setSize(dataSize: Int): Void;
      // Set uncompressed sound data format.
    function setFormat(frequency: Int, sixteenBit: Bool, stereo: Bool): Void;
      // Set loop on/off. If loop is enabled, sets the full sound as loop range.
    function setLooped(enable: Bool): Void;
      // Define loop.
    function setLoop(repeatOffset: Int, endOffset: Int): Void;
      // Return length in seconds.
    function getLength(): Float;
      // Return total sound data size.
    function getDataSize(): Int;
      // Return sample size.
    function getSampleSize(): Int;
      // Return default frequency as a float.
    function getFrequency(): Float;
      // Return default frequency as an integer.
    function getIntFrequency(): Int;
      // Return whether is looped.
    function isLooped(): Bool;
      // Return whether data is sixteen bit.
    function isSixteenBit(): Bool;
      // Return whether data is stereo.
    function isStereo(): Bool;
      // Return whether is compressed.
    function isCompressed(): Bool;
      // Fix interpolation by copying data from loop start to loop end (looped), or adding silence (oneshot.) Called internally, does not normally need to be called, unless the sound data is modified manually on the fly.
    function fixInterpolation(): Void;

}

@:native("Atomic.SoundSource")
extern class SoundSource extends Component {

    var soundType: String;
    var frequency: Float;
    var gain: Float;
    var attenuation: Float;
    var panning: Float;
    var autoRemove: Bool;
    var sound: Sound;
    var timePosition: Float;
    var positionAttr: Int;
    var playingAttr: Bool;

      // Construct.
    function new();

      // Play a sound.
    function play(sound: Sound): Void;
      // Stop playback.
    function stop(): Void;
      // Set sound type, determines the master gain group.
    function setSoundType(type: String): Void;
      // Set frequency.
    function setFrequency(frequency: Float): Void;
      // Set gain. 0.0 is silence, 1.0 is full volume.
    function setGain(gain: Float): Void;
      // Set attenuation. 1.0 is unaltered. Used for distance attenuated playback.
    function setAttenuation(attenuation: Float): Void;
      // Set stereo panning. -1.0 is full left and 1.0 is full right.
    function setPanning(panning: Float): Void;
      // Set whether sound source will be automatically removed from the scene node when playback stops.
    function setAutoRemove(enable: Bool): Void;
      // Return sound.
    function setSound(sound: Sound): Void;
      // Return sound.
    function getSound(): Sound;
      // Return sound type, determines the master gain group.
    function getSoundType(): String;
      // Return playback time position.
    function getTimePosition(): Float;
      // Return frequency.
    function getFrequency(): Float;
      // Return gain.
    function getGain(): Float;
      // Return attenuation.
    function getAttenuation(): Float;
      // Return stereo panning.
    function getPanning(): Float;
      // Return autoremove mode.
    function getAutoRemove(): Bool;
      // Return whether is playing.
    function isPlaying(): Bool;
      // Update the sound source. Perform subclass specific operations. Called by Audio.
    function update(timeStep: Float): Void;
      // Update the effective master gain. Called internally and by Audio when the master gain changes.
    function updateMasterGain(): Void;
      // Set sound position attribute.
    function setPositionAttr(value: Int): Void;
      // Set sound playing attribute
    function setPlayingAttr(value: Bool): Void;
      // Return sound position attribute.
    function getPositionAttr(): Int;

}



//----------------------------------------------------
// MODULE: Physics
//----------------------------------------------------


@:native("Atomic.CollisionShape")
extern class CollisionShape extends Component {

    var terrain: Int;
    var shapeType: ShapeType;
    var size: Vector3;
    var position: Vector3;
    var rotation: Quaternion;
    var margin: Float;
    var model: Model;
    var lodLevel: Int;
    var physicsWorld: PhysicsWorld;
    var worldBoundingBox: BoundingBox;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set as a box.
    function setBox(size: Vector3, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a sphere.
    function setSphere(diameter: Float, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a static plane.
    function setStaticPlane(?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a cylinder.
    function setCylinder(diameter: Float, height: Float, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a capsule.
    function setCapsule(diameter: Float, height: Float, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a cone.
    function setCone(diameter: Float, height: Float, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a triangle mesh from Model. If you update a model's geometry and want to reapply the shape, call physicsWorld->RemoveCachedGeometry(model) first.
    function setTriangleMesh(model: Model, ?lodLevel: Int, ?scale: Vector3, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a triangle mesh from CustomGeometry.
    function setCustomTriangleMesh(custom: CustomGeometry, ?scale: Vector3, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a convex hull from Model.
    function setConvexHull(model: Model, ?lodLevel: Int, ?scale: Vector3, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a convex hull from CustomGeometry.
    function setCustomConvexHull(custom: CustomGeometry, ?scale: Vector3, ?position: Vector3, ?rotation: Quaternion): Void;
      // Set as a terrain. Only works if the same scene node contains a Terrain component.
    function setTerrain(?lodLevel: Int): Void;
      // Set shape type.
    function setShapeType(type: ShapeType): Void;
      // Set shape size.
    function setSize(size: Vector3): Void;
      // Set offset position.
    function setPosition(position: Vector3): Void;
      // Set offset rotation.
    function setRotation(rotation: Quaternion): Void;
      // Set offset transform.
    function setTransform(position: Vector3, rotation: Quaternion): Void;
      // Set collision margin.
    function setMargin(margin: Float): Void;
      // Set triangle mesh / convex hull model.
    function setModel(model: Model): Void;
      // Set model LOD level.
    function setLodLevel(lodLevel: Int): Void;
      // Return physics world.
    function getPhysicsWorld(): PhysicsWorld;
      // Return shape type.
    function getShapeType(): ShapeType;
      // Return shape size.
    function getSize(): Vector3;
      // Return offset position.
    function getPosition(): Vector3;
      // Return offset rotation.
    function getRotation(): Quaternion;
      // Return collision margin.
    function getMargin(): Float;
      // Return triangle mesh / convex hull model.
    function getModel(): Model;
      // Return model LOD level.
    function getLodLevel(): Int;
      // Return world-space bounding box.
    function getWorldBoundingBox(): BoundingBox;
      // Update the new collision shape to the RigidBody.
    function notifyRigidBody(?updateMass: Bool): Void;
      // Release the collision shape.
    function releaseShape(): Void;

}

@:native("Atomic.Constraint")
extern class Constraint extends Component {

    var constraintType: ConstraintType;
    var otherBody: RigidBody;
    var position: Vector3;
    var rotation: Quaternion;
    var axis: Vector3;
    var otherPosition: Vector3;
    var otherRotation: Quaternion;
    var otherAxis: Vector3;
    var worldPosition: Vector3;
    var highLimit: Vector2;
    var lowLimit: Vector2;
    var erp: Float;
    var cfm: Float;
    var disableCollision: Bool;
    var physicsWorld: PhysicsWorld;
    var ownBody: RigidBody;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set constraint type and recreate the constraint.
    function setConstraintType(type: ConstraintType): Void;
      // Set other body to connect to. Set to null to connect to the static world.
    function setOtherBody(body: RigidBody): Void;
      // Set constraint position relative to own body.
    function setPosition(position: Vector3): Void;
      // Set constraint rotation relative to own body.
    function setRotation(rotation: Quaternion): Void;
      // Set constraint rotation relative to own body by specifying the axis.
    function setAxis(axis: Vector3): Void;
      // Set constraint position relative to the other body. If connected to the static world, is a world space position.
    function setOtherPosition(position: Vector3): Void;
      // Set constraint rotation relative to the other body. If connected to the static world, is a world space rotation.
    function setOtherRotation(rotation: Quaternion): Void;
      // Set constraint rotation relative to the other body by specifying the axis.
    function setOtherAxis(axis: Vector3): Void;
      // Set constraint world space position. Resets both own and other body relative position, ie. zeroes the constraint error.
    function setWorldPosition(position: Vector3): Void;
      // Set high limit. Interpretation is constraint type specific.
    function setHighLimit(limit: Vector2): Void;
      // Set low limit. Interpretation is constraint type specific.
    function setLowLimit(limit: Vector2): Void;
      // Set constraint error reduction parameter. Zero = leave to default.
    function setERP(erp: Float): Void;
      // Set constraint force mixing parameter. Zero = leave to default.
    function setCFM(cfm: Float): Void;
      // Set whether to disable collisions between connected bodies.
    function setDisableCollision(disable: Bool): Void;
      // Return physics world.
    function getPhysicsWorld(): PhysicsWorld;
      // Return constraint type.
    function getConstraintType(): ConstraintType;
      // Return rigid body in own scene node.
    function getOwnBody(): RigidBody;
      // Return the other rigid body. May be null if connected to the static world.
    function getOtherBody(): RigidBody;
      // Return constraint position relative to own body.
    function getPosition(): Vector3;
      // Return constraint rotation relative to own body.
    function getRotation(): Quaternion;
      // Return constraint position relative to other body.
    function getOtherPosition(): Vector3;
      // Return constraint rotation relative to other body.
    function getOtherRotation(): Quaternion;
      // Return constraint world position, calculated from own body.
    function getWorldPosition(): Vector3;
      // Return high limit.
    function getHighLimit(): Vector2;
      // Return low limit.
    function getLowLimit(): Vector2;
      // Return constraint error reduction parameter.
    function getERP(): Float;
      // Return constraint force mixing parameter.
    function getCFM(): Float;
      // Return whether collisions between connected bodies are disabled.
    function getDisableCollision(): Bool;
      // Release the constraint.
    function releaseConstraint(): Void;
      // Apply constraint frames.
    function applyFrames(): Void;

}

@:native("Atomic.PhysicsWorld")
extern class PhysicsWorld extends Component {

    var fps: Int;
    var gravity: Vector3;
    var maxSubSteps: Int;
    var numIterations: Int;
    var interpolation: Bool;
    var internalEdge: Bool;
    var splitImpulse: Bool;
    var maxNetworkAngularVelocity: Float;
    var debugRenderer: DebugRenderer;
    var debugDepthTest: Bool;
    var applyingTransforms: Bool;

      // Construct.
    function new();

      // Set debug draw flags.
    function setDebugMode(debugMode: Int): Void;
      // Return debug draw flags.
    function getDebugMode(): Int;
      // Step the simulation forward.
    function update(timeStep: Float): Void;
      // Refresh collisions only without updating dynamics.
    function updateCollisions(): Void;
      // Set simulation substeps per second.
    function setFps(fps: Int): Void;
      // Set gravity.
    function setGravity(gravity: Vector3): Void;
      // Set maximum number of physics substeps per frame. 0 (default) is unlimited. Positive values cap the amount. Use a negative value to enable an adaptive timestep. This may cause inconsistent physics behavior.
    function setMaxSubSteps(num: Int): Void;
      // Set number of constraint solver iterations.
    function setNumIterations(num: Int): Void;
      // Set whether to interpolate between simulation steps.
    function setInterpolation(enable: Bool): Void;
      // Set whether to use Bullet's internal edge utility for trimesh collisions. Disabled by default.
    function setInternalEdge(enable: Bool): Void;
      // Set split impulse collision mode. This is more accurate, but slower. Disabled by default.
    function setSplitImpulse(enable: Bool): Void;
      // Set maximum angular velocity for network replication.
    function setMaxNetworkAngularVelocity(velocity: Float): Void;
      // Invalidate cached collision geometry for a model.
    function removeCachedGeometry(model: Model): Void;
      // Return gravity.
    function getGravity(): Vector3;
      // Return maximum number of physics substeps per frame.
    function getMaxSubSteps(): Int;
      // Return number of constraint solver iterations.
    function getNumIterations(): Int;
      // Return whether interpolation between simulation steps is enabled.
    function getInterpolation(): Bool;
      // Return whether Bullet's internal edge utility for trimesh collisions is enabled.
    function getInternalEdge(): Bool;
      // Return whether split impulse collision mode is enabled.
    function getSplitImpulse(): Bool;
      // Return simulation steps per second.
    function getFps(): Int;
      // Return maximum angular velocity for network replication.
    function getMaxNetworkAngularVelocity(): Float;
      // Add a rigid body to keep track of. Called by RigidBody.
    function addRigidBody(body: RigidBody): Void;
      // Remove a rigid body. Called by RigidBody.
    function removeRigidBody(body: RigidBody): Void;
      // Add a collision shape to keep track of. Called by CollisionShape.
    function addCollisionShape(shape: CollisionShape): Void;
      // Remove a collision shape. Called by CollisionShape.
    function removeCollisionShape(shape: CollisionShape): Void;
      // Add a constraint to keep track of. Called by Constraint.
    function addConstraint(joint: Constraint): Void;
      // Remove a constraint. Called by Constraint.
    function removeConstraint(joint: Constraint): Void;
      // Set debug renderer to use. Called both by PhysicsWorld itself and physics components.
    function setDebugRenderer(debug: DebugRenderer): Void;
      // Set debug geometry depth test mode. Called both by PhysicsWorld itself and physics components.
    function setDebugDepthTest(enable: Bool): Void;
      // Clean up the geometry cache.
    function cleanupGeometryCache(): Void;
      // Set node dirtying to be disregarded.
    function setApplyingTransforms(enable: Bool): Void;
      // Return whether node dirtying should be disregarded.
    function isApplyingTransforms(): Bool;

}

@:native("Atomic.RigidBody")
extern class RigidBody extends Component {

    var mass: Float;
    var position: Vector3;
    var rotation: Quaternion;
    var linearVelocity: Vector3;
    var linearFactor: Vector3;
    var linearRestThreshold: Float;
    var linearDamping: Float;
    var angularVelocity: Vector3;
    var angularFactor: Vector3;
    var angularRestThreshold: Float;
    var angularDamping: Float;
    var friction: Float;
    var anisotropicFriction: Vector3;
    var rollingFriction: Float;
    var restitution: Float;
    var contactProcessingThreshold: Float;
    var ccdRadius: Float;
    var ccdMotionThreshold: Float;
    var useGravity: Bool;
    var gravityOverride: Vector3;
    var kinematic: Bool;
    var trigger: Bool;
    var collisionLayer: Int;
    var collisionMask: Int;
    var collisionEventMode: CollisionEventMode;
    var physicsWorld: PhysicsWorld;
    var centerOfMass: Vector3;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Handle enabled/disabled state change.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set mass. Zero mass makes the body static.
    function setMass(mass: Float): Void;
      // Set rigid body position in world space.
    function setPosition(position: Vector3): Void;
      // Set rigid body rotation in world space.
    function setRotation(rotation: Quaternion): Void;
      // Set rigid body position and rotation in world space as an atomic operation.
    function setTransform(position: Vector3, rotation: Quaternion): Void;
      // Set linear velocity.
    function setLinearVelocity(velocity: Vector3): Void;
      // Set linear degrees of freedom. Use 1 to enable an axis or 0 to disable. Default is all axes enabled (1, 1, 1).
    function setLinearFactor(factor: Vector3): Void;
      // Set linear velocity deactivation threshold.
    function setLinearRestThreshold(threshold: Float): Void;
      // Set linear velocity damping factor.
    function setLinearDamping(damping: Float): Void;
      // Set angular velocity.
    function setAngularVelocity(angularVelocity: Vector3): Void;
      // Set angular degrees of freedom. Use 1 to enable an axis or 0 to disable. Default is all axes enabled (1, 1, 1).
    function setAngularFactor(factor: Vector3): Void;
      // Set angular velocity deactivation threshold.
    function setAngularRestThreshold(threshold: Float): Void;
      // Set angular velocity damping factor.
    function setAngularDamping(factor: Float): Void;
      // Set friction coefficient.
    function setFriction(friction: Float): Void;
      // Set anisotropic friction.
    function setAnisotropicFriction(friction: Vector3): Void;
      // Set rolling friction coefficient.
    function setRollingFriction(friction: Float): Void;
      // Set restitution coefficient.
    function setRestitution(restitution: Float): Void;
      // Set contact processing threshold.
    function setContactProcessingThreshold(threshold: Float): Void;
      // Set continuous collision detection swept sphere radius.
    function setCcdRadius(radius: Float): Void;
      // Set continuous collision detection motion-per-simulation-step threshold. 0 disables, which is the default.
    function setCcdMotionThreshold(threshold: Float): Void;
      // Set whether gravity is applied to rigid body.
    function setUseGravity(enable: Bool): Void;
      // Set gravity override. If zero, uses physics world's gravity.
    function setGravityOverride(gravity: Vector3): Void;
      // Set rigid body kinematic mode. In kinematic mode forces are not applied to the rigid body.
    function setKinematic(enable: Bool): Void;
      // Set rigid body trigger mode. In trigger mode collisions are reported but do not apply forces.
    function setTrigger(enable: Bool): Void;
      // Set collision layer.
    function setCollisionLayer(layer: Int): Void;
      // Set collision mask.
    function setCollisionMask(mask: Int): Void;
      // Set collision group and mask.
    function setCollisionLayerAndMask(layer: Int, mask: Int): Void;
      // Set collision event signaling mode. Default is to signal when rigid bodies are active.
    function setCollisionEventMode(mode: CollisionEventMode): Void;
      // Apply torque.
    function applyTorque(torque: Vector3): Void;
      // Apply impulse to center of mass.
    function applyImpulse(impulse: Vector3): Void;
      // Apply torque impulse.
    function applyTorqueImpulse(torque: Vector3): Void;
      // Reset accumulated forces.
    function resetForces(): Void;
      // Activate rigid body if it was resting.
    function activate(): Void;
      // Readd rigid body to the physics world to clean up internal state like stale contacts.
    function reAddBodyToWorld(): Void;
      // Disable mass update. Call this to optimize performance when adding or editing multiple collision shapes in the same node.
    function disableMassUpdate(): Void;
      // Re-enable mass update and recalculate the mass/inertia by calling UpdateMass(). Call when collision shape changes are finished.
    function enableMassUpdate(): Void;
      // Return physics world.
    function getPhysicsWorld(): PhysicsWorld;
      // Return mass.
    function getMass(): Float;
      // Return rigid body position in world space.
    function getPosition(): Vector3;
      // Return rigid body rotation in world space.
    function getRotation(): Quaternion;
      // Return linear velocity.
    function getLinearVelocity(): Vector3;
      // Return linear degrees of freedom.
    function getLinearFactor(): Vector3;
      // Return linear velocity at local point.
    function getVelocityAtPoint(position: Vector3): Vector3;
      // Return linear velocity deactivation threshold.
    function getLinearRestThreshold(): Float;
      // Return linear velocity damping factor.
    function getLinearDamping(): Float;
      // Return angular velocity.
    function getAngularVelocity(): Vector3;
      // Return angular degrees of freedom.
    function getAngularFactor(): Vector3;
      // Return angular velocity deactivation threshold.
    function getAngularRestThreshold(): Float;
      // Return angular velocity damping factor.
    function getAngularDamping(): Float;
      // Return friction coefficient.
    function getFriction(): Float;
      // Return anisotropic friction.
    function getAnisotropicFriction(): Vector3;
      // Return rolling friction coefficient.
    function getRollingFriction(): Float;
      // Return restitution coefficient.
    function getRestitution(): Float;
      // Return contact processing threshold.
    function getContactProcessingThreshold(): Float;
      // Return continuous collision detection swept sphere radius.
    function getCcdRadius(): Float;
      // Return continuous collision detection motion-per-simulation-step threshold.
    function getCcdMotionThreshold(): Float;
      // Return whether rigid body uses gravity.
    function getUseGravity(): Bool;
      // Return gravity override. If zero (default), uses the physics world's gravity.
    function getGravityOverride(): Vector3;
      // Return center of mass offset.
    function getCenterOfMass(): Vector3;
      // Return kinematic mode flag.
    function isKinematic(): Bool;
      // Return whether this RigidBody is acting as a trigger.
    function isTrigger(): Bool;
      // Return whether rigid body is active (not sleeping.)
    function isActive(): Bool;
      // Return collision layer.
    function getCollisionLayer(): Int;
      // Return collision mask.
    function getCollisionMask(): Int;
      // Return collision event signaling mode.
    function getCollisionEventMode(): CollisionEventMode;
      // Apply new world transform after a simulation step. Called internally.
    function applyWorldTransform(newWorldPosition: Vector3, newWorldRotation: Quaternion): Void;
      // Update mass and inertia to the Bullet rigid body.
    function updateMass(): Void;
      // Update gravity parameters to the Bullet rigid body.
    function updateGravity(): Void;
      // Add a constraint that refers to this rigid body.
    function addConstraint(constraint: Constraint): Void;
      // Remove a constraint that refers to this rigid body.
    function removeConstraint(constraint: Constraint): Void;
      // Remove the rigid body.
    function releaseBody(): Void;

}



//----------------------------------------------------
// MODULE: Navigation
//----------------------------------------------------


@:native("Atomic.Navigable")
extern class Navigable extends Component {

    var recursive: Bool;

      // Construct.
    function new();

      // Set whether geometry is automatically collected from child nodes. Default true.
    function setRecursive(enable: Bool): Void;
      // Return whether geometry is automatically collected from child nodes.
    function isRecursive(): Bool;

}

@:native("Atomic.NavigationMesh")
extern class NavigationMesh extends Component {

    var tileSize: Int;
    var cellSize: Float;
    var cellHeight: Float;
    var agentHeight: Float;
    var agentRadius: Float;
    var agentMaxClimb: Float;
    var agentMaxSlope: Float;
    var regionMinSize: Float;
    var regionMergeSize: Float;
    var edgeMaxLength: Float;
    var edgeMaxError: Float;
    var detailSampleDistance: Float;
    var detailSampleMaxError: Float;
    var padding: Vector3;
    var randomPoint: Vector3;
    var meshName: String;
    var boundingBox: BoundingBox;
    var worldBoundingBox: BoundingBox;
    var numTiles: IntVector2;
    var partitionType: NavmeshPartitionType;
    var drawOffMeshConnections: Bool;
    var drawNavAreas: Bool;

      // Construct.
    function new();

      // Set tile size.
    function setTileSize(size: Int): Void;
      // Set cell size.
    function setCellSize(size: Float): Void;
      // Set cell height.
    function setCellHeight(height: Float): Void;
      // Set navigation agent height.
    function setAgentHeight(height: Float): Void;
      // Set navigation agent radius.
    function setAgentRadius(radius: Float): Void;
      // Set navigation agent max vertical climb.
    function setAgentMaxClimb(maxClimb: Float): Void;
      // Set navigation agent max slope.
    function setAgentMaxSlope(maxSlope: Float): Void;
      // Set region minimum size.
    function setRegionMinSize(size: Float): Void;
      // Set region merge size.
    function setRegionMergeSize(size: Float): Void;
      // Set edge max length.
    function setEdgeMaxLength(length: Float): Void;
      // Set edge max error.
    function setEdgeMaxError(error: Float): Void;
      // Set detail sampling distance.
    function setDetailSampleDistance(distance: Float): Void;
      // Set detail sampling maximum error.
    function setDetailSampleMaxError(error: Float): Void;
      // Set padding of the navigation mesh bounding box. Having enough padding allows to add geometry on the extremities of the navigation mesh when doing partial rebuilds.
    function setPadding(padding: Vector3): Void;
      // Set the cost of an area.
    function setAreaCost(areaID: Int, cost: Float): Void;
      // Find the nearest point on the navigation mesh to a given point. Extens specifies how far out from the specified point to check along each axis.
    function findNearestPoint(point: Vector3, ?extents: Vector3): Vector3;
      // Try to move along the surface from one point to another.
    function moveAlongSurface(start: Vector3, end: Vector3, ?extents: Vector3, ?maxVisited: Int): Vector3;
      // Return a random point on the navigation mesh.
    function getRandomPoint(): Vector3;
      // Return a random point on the navigation mesh within a circle. The circle radius is only a guideline and in practice the returned point may be further away.
    function getRandomPointInCircle(center: Vector3, radius: Float, ?extents: Vector3): Vector3;
      // Return distance to wall from a point. Maximum search radius must be specified.
    function getDistanceToWall(point: Vector3, radius: Float, ?extents: Vector3): Float;
      // Perform a walkability raycast on the navigation mesh between start and end and return the point where a wall was hit, or the end point if no walls.
    function raycast(start: Vector3, end: Vector3, ?extents: Vector3): Vector3;
      // Return the given name of this navigation mesh.
    function getMeshName(): String;
      // Set the name of this navigation mesh.
    function setMeshName(newName: String): Void;
      // Return tile size.
    function getTileSize(): Int;
      // Return cell size.
    function getCellSize(): Float;
      // Return cell height.
    function getCellHeight(): Float;
      // Return navigation agent height.
    function getAgentHeight(): Float;
      // Return navigation agent radius.
    function getAgentRadius(): Float;
      // Return navigation agent max vertical climb.
    function getAgentMaxClimb(): Float;
      // Return navigation agent max slope.
    function getAgentMaxSlope(): Float;
      // Return region minimum size.
    function getRegionMinSize(): Float;
      // Return region merge size.
    function getRegionMergeSize(): Float;
      // Return edge max length.
    function getEdgeMaxLength(): Float;
      // Return edge max error.
    function getEdgeMaxError(): Float;
      // Return detail sampling distance.
    function getDetailSampleDistance(): Float;
      // Return detail sampling maximum error.
    function getDetailSampleMaxError(): Float;
      // Return navigation mesh bounding box padding.
    function getPadding(): Vector3;
      // Get the current cost of an area
    function getAreaCost(areaID: Int): Float;
      // Return whether has been initialized with valid navigation data.
    function isInitialized(): Bool;
      // Return local space bounding box of the navigation mesh.
    function getBoundingBox(): BoundingBox;
      // Return world space bounding box of the navigation mesh.
    function getWorldBoundingBox(): BoundingBox;
      // Return number of tiles.
    function getNumTiles(): IntVector2;
      // Set the partition type used for polygon generation.
    function setPartitionType(aType: NavmeshPartitionType): Void;
      // Return Partition Type.
    function getPartitionType(): NavmeshPartitionType;
      // Draw debug geometry for OffMeshConnection components.
    function setDrawOffMeshConnections(enable: Bool): Void;
      // Return whether to draw OffMeshConnection components.
    function getDrawOffMeshConnections(): Bool;
      // Draw debug geometry for NavArea components.
    function setDrawNavAreas(enable: Bool): Void;
      // Return whether to draw NavArea components.
    function getDrawNavAreas(): Bool;

}

@:native("Atomic.OffMeshConnection")
extern class OffMeshConnection extends Component {

    var endPoint: Node;
    var radius: Float;
    var bidirectional: Bool;
    var mask: Int;
    var areaID: Int;

      // Construct.
    function new();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    @:overload(function(): Void{})
    override function applyAttributes(): Void;
      // Visualize the component as debug geometry.
    @:overload(function(debug: DebugRenderer, depthTest: Bool): Void{})
    override function drawDebugGeometry(debug: DebugRenderer, depthTest: Bool): Void;
      // Set endpoint node.
    function setEndPoint(node: Node): Void;
      // Set radius.
    function setRadius(radius: Float): Void;
      // Set bidirectional flag. Default true.
    function setBidirectional(enabled: Bool): Void;
      // Set a user assigned mask
    function setMask(newMask: Int): Void;
      // Sets the assigned area Id for the connection
    function setAreaID(newAreaID: Int): Void;
      // Return endpoint node.
    function getEndPoint(): Node;
      // Return radius.
    function getRadius(): Float;
      // Return whether is bidirectional.
    function isBidirectional(): Bool;
      // Return the user assigned mask
    function getMask(): Int;
      // Return the user assigned area ID
    function getAreaID(): Int;

}



//----------------------------------------------------
// MODULE: Input
//----------------------------------------------------


@:native("Atomic.Input")
extern class Input extends AObject {

    var toggleFullscreen: Bool;
    var mouseGrabbed: Bool;
    var mouseMode: MouseMode;
    var screenKeyboardVisible: Bool;
    var touchEmulation: Bool;
    var qualifiers: Int;
    var mousePosition: IntVector2;
    var mouseMove: IntVector2;
    var mouseMoveX: Int;
    var mouseMoveY: Int;
    var mouseMoveWheel: Int;
    var numTouches: Int;
    var numJoysticks: Int;
    var screenKeyboardSupport: Bool;

      // Construct.
    function new();

      // Poll for window messages. Called by HandleBeginFrame().
    function update(): Void;
      // Set whether ALT-ENTER fullscreen toggle is enabled.
    function setToggleFullscreen(enable: Bool): Void;
      // Set whether the operating system mouse cursor is visible. When not visible (default), is kept centered to prevent leaving the window. Mouse visibility event can be suppressed-- this also recalls any unsuppressed SetMouseVisible which can be returned by ResetMouseVisible().
    function setMouseVisible(enable: Bool, ?suppressEvent: Bool): Void;
      // Reset last mouse visibility that was not suppressed in SetMouseVisible.
    function resetMouseVisible(): Void;
      // Set whether the mouse is currently being grabbed by an operation.
    function setMouseGrabbed(grab: Bool): Void;
    function setMouseMode(mode: MouseMode): Void;
      // Show or hide on-screen keyboard on platforms that support it. When shown, keypresses from it are delivered as key events.
    function setScreenKeyboardVisible(enable: Bool): Void;
      // Set touch emulation by mouse. Only available on desktop platforms. When enabled, actual mouse events are no longer sent and the mouse cursor is forced visible.
    function setTouchEmulation(enable: Bool): Void;
      // Begin recording a touch gesture. Return true if successful. The E_GESTURERECORDED event (which contains the ID for the new gesture) will be sent when recording finishes.
    function recordGesture(): Bool;
      // Remove an in-memory gesture by ID. Return true if was found.
    function removeGesture(gestureID: Int): Bool;
      // Remove all in-memory gestures.
    function removeAllGestures(): Void;
      // Return keycode from key name.
    function getKeyFromName(name: String): Int;
      // Return keycode from scancode.
    function getKeyFromScancode(scancode: Int): Int;
      // Return name of key from keycode.
    function getKeyName(key: Int): String;
      // Return scancode from keycode.
    function getScancodeFromKey(key: Int): Int;
      // Return scancode from key name.
    function getScancodeFromName(name: String): Int;
      // Return name of key from scancode.
    function getScancodeName(scancode: Int): String;
      // Check if a key is held down.
    function getKeyDown(key: Int): Bool;
      // Check if a key has been pressed on this frame.
    function getKeyPress(key: Int): Bool;
      // Check if a key is held down by scancode.
    function getScancodeDown(scancode: Int): Bool;
      // Check if a key has been pressed on this frame by scancode.
    function getScancodePress(scancode: Int): Bool;
      // Check if a mouse button is held down.
    function getMouseButtonDown(button: Int): Bool;
      // Check if a mouse button has been pressed on this frame.
    function getMouseButtonPress(button: Int): Bool;
      // Check if a qualifier key is held down.
    function getQualifierDown(qualifier: Int): Bool;
      // Check if a qualifier key has been pressed on this frame.
    function getQualifierPress(qualifier: Int): Bool;
      // Return the currently held down qualifiers.
    function getQualifiers(): Int;
      // Return mouse position within window. Should only be used with a visible mouse cursor.
    function getMousePosition(): IntVector2;
      // Return mouse movement since last frame.
    function getMouseMove(): IntVector2;
      // Return horizontal mouse movement since last frame.
    function getMouseMoveX(): Int;
      // Return vertical mouse movement since last frame.
    function getMouseMoveY(): Int;
      // Return mouse wheel movement since last frame.
    function getMouseMoveWheel(): Int;
      // Return number of active finger touches.
    function getNumTouches(): Int;
      // Return number of connected joysticks.
    function getNumJoysticks(): Int;
      // Return whether fullscreen toggle is enabled.
    function getToggleFullscreen(): Bool;
      // Return whether on-screen keyboard is supported.
    function getScreenKeyboardSupport(): Bool;
      // Return whether on-screen keyboard is being shown.
    function isScreenKeyboardVisible(): Bool;
      // Return whether touch emulation is enabled.
    function getTouchEmulation(): Bool;
      // Return whether the operating system mouse cursor is visible.
    function isMouseVisible(): Bool;
      // Return whether the mouse is currently being grabbed by an operation.
    function isMouseGrabbed(): Bool;
      // Return the mouse mode.
    function getMouseMode(): MouseMode;
      // Return whether application window has input focus.
    function hasFocus(): Bool;
      // Return whether application window is minimized.
    function isMinimized(): Bool;

}



//----------------------------------------------------
// MODULE: UI
//----------------------------------------------------


@:native("Atomic.UIButton")
extern class UIButton extends UIWidget {

    var squeezable: Bool;

    function new(?createWidget: Bool);

    function setSqueezable(value: Bool): Void;

}

@:native("Atomic.UICheckBox")
extern class UICheckBox extends UIWidget {

    function new(?createWidget: Bool);


}

@:native("Atomic.UIClickLabel")
extern class UIClickLabel extends UIWidget {

    function new(?createWidget: Bool);


}

@:native("Atomic.UIContainer")
extern class UIContainer extends UIWidget {

    function new(?createWidget: Bool);


}

@:native("Atomic.UIDimmer")
extern class UIDimmer extends UIWidget {

    function new(?createWidget: Bool);


}

@:native("Atomic.UIDragObject")
extern class UIDragObject extends AObject {

    var text: String;
    var icon: String;
    var object: AObject;
    var filenames: Array<String>;

      // Construct.
    function new(?object: AObject, ?text: String, ?icon: String);

    function getText(): String;
    function getIcon(): String;
    function getObject(): AObject;
    function getFilenames(): Array<String>;
    function setText(text: String): Void;
    function setIcon(icon: String): Void;
    function setObject(object: AObject): Void;
    function addFilename(filename: String): Void;

}

@:native("Atomic.UIEditField")
extern class UIEditField extends UIWidget {

    var textAlign: TEXT_ALIGN;
    var editType: UI_EDIT_TYPE;
    var readOnly: Bool;
    var wrapping: Bool;

    function new(?createWidget: Bool);

    function appendText(text: String): Void;
    function setTextAlign(align: TEXT_ALIGN): Void;
    function setEditType(type: UI_EDIT_TYPE): Void;
    function setReadOnly(readonly: Bool): Void;
    function scrollTo(x: Int, y: Int): Void;
    function setWrapping(wrap: Bool): Void;
    function getWrapping(): Bool;

}

@:native("Atomic.UIFontDescription")
extern class UIFontDescription extends AObject {

    var id: String;
    var size: Int;

    function new();

    function setId(id: String): Void;
    function setSize(size: Int): Void;

}

@:native("Atomic.UIImageWidget")
extern class UIImageWidget extends UIWidget {

    var image: String;

    function new(?createWidget: Bool);

    function setImage(imagePath: String): Void;

}

@:native("Atomic.UIInlineSelect")
extern class UIInlineSelect extends UIWidget {

    var editFieldLayoutParams: UILayoutParams;

    function new(?createWidget: Bool);

    function setLimits(minimum: Float, maximum: Float): Void;
    function setEditFieldLayoutParams(params: UILayoutParams): Void;

}

@:native("Atomic.UILayoutParams")
extern class UILayoutParams extends AObject {

    var width: Int;
    var height: Int;
    var minWidth: Int;
    var minHeight: Int;
    var maxWidth: Int;
    var maxHeight: Int;

    function new();

    function setWidth(width: Int): Void;
    function setHeight(height: Int): Void;
    function setMinWidth(width: Int): Void;
    function setMinHeight(height: Int): Void;
    function setMaxWidth(width: Int): Void;
    function setMaxHeight(height: Int): Void;

}

@:native("Atomic.UILayout")
extern class UILayout extends UIWidget {

    var spacing: Int;
    var axis: UI_AXIS;
    var layoutSize: UI_LAYOUT_SIZE;
    var layoutPosition: UI_LAYOUT_POSITION;
    var layoutDistribution: UI_LAYOUT_DISTRIBUTION;
    var layoutDistributionPosition: UI_LAYOUT_DISTRIBUTION_POSITION;

    function new(?axis: UI_AXIS, ?createWidget: Bool);

    function setSpacing(spacing: Int): Void;
    function setAxis(axis: UI_AXIS): Void;
    function setLayoutSize(size: UI_LAYOUT_SIZE): Void;
    function setLayoutPosition(position: UI_LAYOUT_POSITION): Void;
    function setLayoutDistribution(distribution: UI_LAYOUT_DISTRIBUTION): Void;
    function setLayoutDistributionPosition(distribution_pos: UI_LAYOUT_DISTRIBUTION_POSITION): Void;

}

@:native("Atomic.UIListView")
extern class UIListView extends UIWidget {

    var hoverItemID: String;
    var selectedItemID: String;
    var rootList: UISelectList;

      // Construct.
    function new(?createWidget: Bool);

    function addRootItem(text: String, icon: String, id: String): Int;
    function addChildItem(parentItemID: Int, text: String, icon: String, id: String): Int;
    function setItemText(id: String, text: String): Void;
    function setItemTextSkin(id: String, skin: String): Void;
    function setItemIcon(id: String, icon: String): Void;
    function deleteItemByID(id: String): Void;
    function setExpanded(itemID: Int, value: Bool): Void;
    function deleteAllItems(): Void;
    function selectItemByID(id: String): Void;
    function getHoverItemID(): String;
    function getSelectedItemID(): String;
    function getRootList(): UISelectList;

}

@:native("Atomic.UIMenuItem")
extern class UIMenuItem extends UISelectItem {

    function new(?str: String, ?id: String, ?shortcut: String, ?skinBg: String);


}

@:native("Atomic.UIMenuItemSource")
extern class UIMenuItemSource extends UISelectItemSource {

    function new();


}

@:native("Atomic.UIMenuWindow")
extern class UIMenuWindow extends UIWidget {

    function new(target: UIWidget, id: String);

    function show(source: UISelectItemSource, ?x: Int, ?y: Int): Void;
    function close(): Void;

}

@:native("Atomic.UIMessageWindow")
extern class UIMessageWindow extends UIWindow {

    function new(target: UIWidget, id: String, ?createWidget: Bool);

    function show(title: String, message: String, settings: UI_MESSAGEWINDOW_SETTINGS, dimmer: Bool, width: Int, height: Int): Void;

}

@:native("Atomic.UIPreferredSize")
extern class UIPreferredSize extends RefCounted {

    var minWidth: Int;
    var minHeight: Int;
    var maxWidth: Int;
    var maxHeight: Int;
    var prefWidth: Int;
    var prefHeight: Int;
    var sizeDep: UI_SIZE_DEP;

    function new(?w: Int, ?h: Int);

    function getMinWidth(): Int;
    function getMinHeight(): Int;
    function getMaxWidth(): Int;
    function getMaxHeight(): Int;
    function getPrefWidth(): Int;
    function getPrefHeight(): Int;
    function getSizeDep(): UI_SIZE_DEP;
    function setMinWidth(w: Int): Void;
    function setMinHeight(h: Int): Void;
    function setMaxWidth(w: Int): Void;
    function setMaxHeight(h: Int): Void;
    function setPrefWidth(w: Int): Void;
    function setPrefHeight(h: Int): Void;
    function setSizeDep(dep: UI_SIZE_DEP): Void;

}

@:native("Atomic.UISceneView")
extern class UISceneView extends UIWidget {

    var format: Int;
    var autoUpdate: Bool;
    var scene: Scene;
    var cameraNode: Node;
    var renderTexture: Texture2D;
    var depthTexture: Texture2D;
    var viewport: Viewport;
    var size: IntVector2;

    function new(?createWidget: Bool);

      // React to resize.
    function onResize(newSize: IntVector2): Void;
      // Define the scene and camera to use in rendering. When ownScene is true the View3D will take ownership of them with shared pointers.
    function setView(scene: Scene, camera: Camera): Void;
      // Set render texture pixel format. Default is RGB.
    function setFormat(format: Int): Void;
      // Set render target auto update mode. Default is true.
    function setAutoUpdate(enable: Bool): Void;
      // Queue manual update on the render texture.
    function queueUpdate(): Void;
      // Return render texture pixel format.
    function getFormat(): Int;
      // Return whether render target updates automatically.
    function getAutoUpdate(): Bool;
      // Return scene.
    function getScene(): Scene;
      // Return camera scene node.
    function getCameraNode(): Node;
      // Return render texture.
    function getRenderTexture(): Texture2D;
      // Return depth stencil texture.
    function getDepthTexture(): Texture2D;
      // Return viewport.
    function getViewport(): Viewport;
    function setResizeRequired(): Void;
    function getSize(): IntVector2;

}

@:native("Atomic.UIScrollContainer")
extern class UIScrollContainer extends UIWidget {

    var scrollMode: UI_SCROLL_MODE;
    var adaptToContentSize: Bool;
    var adaptContentSize: Bool;

    function new(?createWidget: Bool);

    function setScrollMode(mode: UI_SCROLL_MODE): Void;
    function getScrollMode(): UI_SCROLL_MODE;
      // Set to true if the preferred size of this container should adapt to the preferred size of the content. This is disabled by default.
    function setAdaptToContentSize(adapt: Bool): Void;
    function getAdaptToContentSize(): Bool;
      // Set to true if the content should adapt to the available size of this container when it's larger than the preferred size. */
    function setAdaptContentSize(adapt: Bool): Void;
    function getAdaptContentSize(): Bool;

}

@:native("Atomic.UISection")
extern class UISection extends UIWidget {

    function new(?createWidget: Bool);


}

@:native("Atomic.UISelectItem")
extern class UISelectItem extends AObject {

    var string: String;
    var id: String;
    var skinImage: String;
    var subSource: UISelectItemSource;

    function new(?str: String, ?id: String, ?skinImage: String);

    function setString(str: String): Void;
    function setID(id: String): Void;
    function setSkinImage(skinImage: String): Void;
    function setSubSource(subSource: UISelectItemSource): Void;

}

@:native("Atomic.UISelectItemSource")
extern class UISelectItemSource extends AObject {

    function new();

    function addItem(item: UISelectItem): Void;
    function clear(): Void;

}

@:native("Atomic.UISelectList")
extern class UISelectList extends UIWidget {

    var filter: String;
    var source: UISelectItemSource;
    var hoverItemID: String;
    var selectedItemID: String;

    function new(?createWidget: Bool);

    function setFilter(filter: String): Void;
    function setSource(source: UISelectItemSource): Void;
    function invalidateList(): Void;
    @:overload(function(value: Int): Void{})
    override function setValue(value: Float): Void;
    @:overload(function(): Int{})
    override function getValue(): Int;
    function getHoverItemID(): String;
    function getSelectedItemID(): String;

}

@:native("Atomic.UISeparator")
extern class UISeparator extends UIWidget {

    function new(?createWidget: Bool);


}

@:native("Atomic.UISkinImage")
extern class UISkinImage extends UIWidget {

    function new(bitmapID: String, ?createWidget: Bool);


}

@:native("Atomic.UITabContainer")
extern class UITabContainer extends UIWidget {

    var numPages: Int;
    var currentPage: Int;
    var currentPageWidget: UIWidget;

    function new(?createWidget: Bool);

    function getNumPages(): Int;
    function setCurrentPage(page: Int): Void;
    function getCurrentPageWidget(): UIWidget;

}

@:native("Atomic.UITextField")
extern class UITextField extends UIWidget {

    var textAlign: UI_TEXT_ALIGN;

    function new(?createWidget: Bool);

    function setTextAlign(align: UI_TEXT_ALIGN): Void;

}

@:native("Atomic.UITextureWidget")
extern class UITextureWidget extends UIWidget {

    var texture: Texture;

    function new(?createWidget: Bool);

    function setTexture(texture: Texture): Void;
    function getTexture(): Texture;

}

@:native("Atomic.UIView")
extern class UIView extends UIWidget {

    function new();


}

@:native("Atomic.UIWidget")
extern class UIWidget extends AObject {

    var id: String;
    var parent: UIWidget;
    var contentRoot: UIWidget;
    var rect: IntRect;
    var preferredSize: UIPreferredSize;
    var text: String;
    var skinBg: String;
    var layoutParams: UILayoutParams;
    var fontDescription: UIFontDescription;
    var gravity: UI_GRAVITY;
    var value: Float;
    var focus: Bool;
    var visibility: UI_WIDGET_VISIBILITY;
    var stateRaw: Int;
    var dragObject: UIDragObject;
    var firstChild: UIWidget;
    var next: UIWidget;
    var isFocusable: Bool;
    var view: UIView;

    function new(?createWidget: Bool);

    function load(filename: String): Bool;
    function getId(): String;
    function getParent(): UIWidget;
    function getContentRoot(): UIWidget;
    function getRect(): IntRect;
    function getPreferredSize(): UIPreferredSize;
    function getText(): String;
    function setRect(r: IntRect): Void;
    function setSize(width: Int, height: Int): Void;
    function setPosition(x: Int, y: Int): Void;
    function setText(text: String): Void;
    function setSkinBg(id: String): Void;
    function setLayoutParams(params: UILayoutParams): Void;
    function setFontDescription(fd: UIFontDescription): Void;
    function removeChild(child: UIWidget, ?cleanup: Bool): Void;
    function deleteAllChildren(): Void;
    function setId(id: String): Void;
    function center(): Void;
    function setGravity(gravity: UI_GRAVITY): Void;
    function setValue(value: Float): Void;
    function getValue(): Float;
    function setFocus(): Void;
    function getFocus(): Bool;
      // Set focus to first widget which accepts it
    function setFocusRecursive(): Void;
    function onFocusChanged(focused: Bool): Void;
    function setState(state: Int, on: Bool): Void;
    function getState(state: Int): Bool;
    function setVisibility(visibility: UI_WIDGET_VISIBILITY): Void;
    function getVisibility(): UI_WIDGET_VISIBILITY;
    function setStateRaw(state: Int): Void;
    function getStateRaw(): Int;
    function invalidate(): Void;
    function die(): Void;
    function setDragObject(object: UIDragObject): Void;
    function getDragObject(): UIDragObject;
    function getFirstChild(): UIWidget;
    function getNext(): UIWidget;
    function isAncestorOf(widget: UIWidget): Bool;
    function setIsFocusable(value: Bool): Void;
    function getWidget(id: String): UIWidget;
    function getView(): UIView;
    function addChild(child: UIWidget): Void;
      // This takes a relative Z and insert the child before or after the given reference widget.*/
    function addChildRelative(child: UIWidget, z: UI_WIDGET_Z_REL, reference: UIWidget): Void;

}

@:native("Atomic.UIWindow")
extern class UIWindow extends UIWidget {

    var settings: UI_WINDOW_SETTINGS;

    function new(?createWidget: Bool);

    function getSettings(): UI_WINDOW_SETTINGS;
    function setSettings(settings: UI_WINDOW_SETTINGS): Void;
    function resizeToFitContent(): Void;
    @:overload(function(child: UIWidget): Void{})
    override function addChild(child: UIWidget): Void;
    function close(): Void;

}



//----------------------------------------------------
// MODULE: Resource
//----------------------------------------------------


@:native("Atomic.Image")
extern class Image extends Resource {

    var width: Int;
    var height: Int;
    var depth: Int;
    var components: Int;
    var compressedFormat: CompressedFormat;
    var numCompressedLevels: Int;
    var nextLevel: Image;

      // Construct empty.
    function new();

      // Flip image horizontally. Return true if successful.
    function flipHorizontal(): Bool;
      // Flip image vertically. Return true if successful.
    function flipVertical(): Bool;
      // Resize image by bilinear resampling. Return true if successful.
    function resize(width: Int, height: Int): Bool;
      // Clear the image with a color.
    function clear(color: Color): Void;
      // Clear the image with an integer color. R component is in the 8 lowest bits.
    function clearInt(uintColor: Int): Void;
      // Save in BMP format. Return true if successful.
    function saveBMP(fileName: String): Bool;
      // Save in PNG format. Return true if successful.
    function savePNG(fileName: String): Bool;
      // Save in TGA format. Return true if successful.
    function saveTGA(fileName: String): Bool;
      // Save in JPG format with compression quality. Return true if successful.
    function saveJPG(fileName: String, quality: Int): Bool;
      // Return a bilinearly sampled 2D pixel color. X and Y have the range 0-1.
    function getPixelBilinear(x: Float, y: Float): Color;
      // Return a trilinearly sampled 3D pixel color. X, Y and Z have the range 0-1.
    function getPixelTrilinear(x: Float, y: Float, z: Float): Color;
      // Return width.
    function getWidth(): Int;
      // Return height.
    function getHeight(): Int;
      // Return depth.
    function getDepth(): Int;
      // Return number of color components.
    function getComponents(): Int;
      // Return whether is compressed.
    function isCompressed(): Bool;
      // Return compressed format.
    function getCompressedFormat(): CompressedFormat;
      // Return number of compressed mip levels.
    function getNumCompressedLevels(): Int;
      // Return next mip level by bilinear filtering.
    function getNextLevel(): Image;
      // Return image converted to 4-component (RGBA) to circumvent modern rendering API's not supporting e.g. the luminance-alpha format.
    function convertToRGBA(): Image;
      // Return subimage from the image by the defined rect or null if failed. 3D images are not supported. You must free the subimage yourself.
    function getSubimage(rect: IntRect): Image;
      // Precalculate the mip levels. Used by asynchronous texture loading.
    function precalculateLevels(): Void;

}

@:native("Atomic.JSONFile")
extern class JSONFile extends Resource {

      // Construct.
    function new();


}

@:native("Atomic.PListFile")
extern class PListFile extends Resource {

      // Construct.
    function new();


}

@:native("Atomic.Resource")
extern class Resource extends AObject {

    var name: String;
    var memoryUse: Int;
    var asyncLoadState: AsyncLoadState;
    var nameHash: String;
    var useTimer: Int;

      // Construct.
    function new();

      // Finish resource loading. Always called from the main thread. Return true if successful.
    function endLoad(): Bool;
      // Set name.
    function setName(name: String): Void;
      // Set memory use in bytes, possibly approximate.
    function setMemoryUse(size: Int): Void;
      // Reset last used timer.
    function resetUseTimer(): Void;
      // Set the asynchronous loading state. Called by ResourceCache. Resources in the middle of asynchronous loading are not normally returned to user.
    function setAsyncLoadState(newState: AsyncLoadState): Void;
      // Return name.
    function getName(): String;
      // Return name hash.
    function getNameHash(): String;
      // Return memory use in bytes, possibly approximate.
    function getMemoryUse(): Int;
      // Return time since last use in milliseconds. If referred to elsewhere than in the resource cache, returns always zero.
    function getUseTimer(): Int;
      // Return the asynchronous loading state.
    function getAsyncLoadState(): AsyncLoadState;

}

@:native("Atomic.ResourceCache")
extern class ResourceCache extends AObject {

    var autoReloadResources: Bool;
    var returnFailedResources: Bool;
    var searchPackagesFirst: Bool;
    var finishBackgroundResourcesMs: Int;
    var numBackgroundLoadResources: Int;
    var resourceDirs: Array<String>;
    var totalMemoryUse: Int;

      // Construct.
    function new();

      // Add a resource load directory. Optional priority parameter which will control search order.
    function addResourceDir(pathName: String, ?priority: Int): Bool;
      // Add a package file for loading resources from by name. Optional priority parameter which will control search order.
    function addPackageFile(fileName: String, ?priority: Int): Bool;
      // Add a manually created resource. Must be uniquely named.
    function addManualResource(resource: Resource): Bool;
      // Remove a resource load directory.
    function removeResourceDir(pathName: String): Void;
      // Remove a package file by name. Optionally release the resources loaded from it.
    function removePackageFile(fileName: String, ?releaseResources: Bool, ?forceRelease: Bool): Void;
      // Release a resource by name.
    function releaseResource(type: String, name: String, ?force: Bool): Void;
      // Release all resources. When called with the force flag false, releases all currently unused resources.
    function releaseAllResources(?force: Bool): Void;
      // Reload a resource. Return true on success. The resource will not be removed from the cache in case of failure.
    function reloadResource(resource: Resource): Bool;
      // Reload a resource based on filename. Causes also reload of dependent resources if necessary.
    function reloadResourceWithDependencies(fileName: String): Void;
      // Set memory budget for a specific resource type, default 0 is unlimited.
    function setMemoryBudget(type: String, budget: Int): Void;
      // Enable or disable automatic reloading of resources as files are modified. Default false.
    function setAutoReloadResources(enable: Bool): Void;
      // Enable or disable returning resources that failed to load. Default false. This may be useful in editing to not lose resource ref attributes.
    function setReturnFailedResources(enable: Bool): Void;
      // Define whether when getting resources should check package files or directories first. True for packages, false for directories.
    function setSearchPackagesFirst(value: Bool): Void;
      // Set how many milliseconds maximum per frame to spend on finishing background loaded resources.
    function setFinishBackgroundResourcesMs(ms: Int): Void;
      // Open and return a file from the resource load paths or from inside a package file. If not found, use a fallback search with absolute path. Return null if fails. Can be called from outside the main thread.
    function getFile(name: String, ?sendEventOnFailure: Bool): File;
      // Return a resource by type and name. Load if not loaded yet. Return null if not found or if fails, unless SetReturnFailedResources(true) has been called. Can be called only from the main thread.
    function getResource(type: String, name: String, ?sendEventOnFailure: Bool): Resource;
      // Load a resource without storing it in the resource cache. Return null if not found or if fails. Can be called from outside the main thread if the resource itself is safe to load completely (it does not possess for example GPU data.)
    function getTempResource(type: String, name: String, ?sendEventOnFailure: Bool): Resource;
      // Background load a resource. An event will be sent when complete. Return true if successfully stored to the load queue, false if eg. already exists. Can be called from outside the main thread.
    function backgroundLoadResource(type: String, name: String, ?sendEventOnFailure: Bool, ?caller: Resource): Bool;
      // Return number of pending background-loaded resources.
    function getNumBackgroundLoadResources(): Int;
      // Return an already loaded resource of specific type & name, or null if not found. Will not load if does not exist.
    function getExistingResource(type: String, name: String): Resource;
      // Return added resource load directories.
    function getResourceDirs(): Array<String>;
      // Return whether a file exists by name.
    function exists(name: String): Bool;
      // Return memory budget for a resource type.
    function getMemoryBudget(type: String): Int;
      // Return total memory use for a resource type.
    function getMemoryUse(type: String): Int;
      // Return total memory use for all resources.
    function getTotalMemoryUse(): Int;
      // Return full absolute file name of resource if possible.
    function getResourceFileName(name: String): String;
      // Return whether automatic resource reloading is enabled.
    function getAutoReloadResources(): Bool;
      // Return whether resources that failed to load are returned.
    function getReturnFailedResources(): Bool;
      // Return whether when getting resources should check package files or directories first.
    function getSearchPackagesFirst(): Bool;
      // Return how many milliseconds maximum to spend on finishing background loaded resources.
    function getFinishBackgroundResourcesMs(): Int;
      // Return either the path itself or its parent, based on which of them has recognized resource subdirectories.
    function getPreferredResourceDir(path: String): String;
      // Remove unsupported constructs from the resource name to prevent ambiguity, and normalize absolute filename to resource path relative if possible.
    function sanitateResourceName(name: String): String;
      // Remove unnecessary constructs from a resource directory name and ensure it to be an absolute path.
    function sanitateResourceDirName(name: String): String;
      // Store a dependency for a resource. If a dependency file changes, the resource will be reloaded.
    function storeResourceDependency(resource: Resource, dependency: String): Void;
      // Reset dependencies for a resource.
    function resetDependencies(resource: Resource): Void;

}

@:native("Atomic.XMLFile")
extern class XMLFile extends Resource {

      // Construct.
    function new();

      // Deserialize from a string. Return true if successful.
    function fromString(source: String): Bool;
      // Serialize the XML content to a string.
    function toString(?indentation: String): String;
      // Patch the XMLFile with another XMLFile. Based on RFC 5261.
    function patch(patchFile: XMLFile): Void;

}



//----------------------------------------------------
// MODULE: Network
//----------------------------------------------------


@:native("Atomic.HttpRequest")
extern class HttpRequest extends RefCounted {

    var url: String;
    var verb: String;
    var error: String;
    var state: HttpRequestState;
    var availableSize: Int;

      // Construct with parameters.
    function new(url: String, verb: String, headers: Array<String>, postData: String);

      // Process the connection in the worker thread until closed.
    function threadFunction(): Void;
      // Set position from the beginning of the stream. Not supported.
    function seek(position: Int): Int;
      // Return URL used in the request.
    function getURL(): String;
      // Return verb used in the request. Default GET if empty verb specified on construction.
    function getVerb(): String;
      // Return error. Only non-empty in the error state.
    function getError(): String;
      // Return connection state.
    function getState(): HttpRequestState;
      // Return amount of bytes in the read buffer.
    function getAvailableSize(): Int;
      // Return whether connection is in the open state.
    function isOpen(): Bool;

}

@:native("Atomic.Network")
extern class Network extends AObject {

    var updateFps: Int;
    var simulatedLatency: Int;
    var simulatedPacketLoss: Float;
    var packageCacheDir: String;

      // Construct.
    function new();

      // Disconnect the connection to the server. If wait time is non-zero, will block while waiting for disconnect to finish.
    function disconnect(?waitMSec: Int): Void;
      // Start a server on a port using UDP protocol. Return true if successful.
    function startServer(port: Dynamic): Bool;
      // Stop the server.
    function stopServer(): Void;
      // Set network update FPS.
    function setUpdateFps(fps: Int): Void;
      // Set simulated latency in milliseconds. This adds a fixed delay before sending each packet.
    function setSimulatedLatency(ms: Int): Void;
      // Set simulated packet loss probability between 0.0 - 1.0.
    function setSimulatedPacketLoss(probability: Float): Void;
      // Register a remote event as allowed to be received. There is also a fixed blacklist of events that can not be allowed in any case, such as ConsoleCommand.
    function registerRemoteEvent(eventType: String): Void;
      // Unregister a remote event as allowed to received.
    function unregisterRemoteEvent(eventType: String): Void;
      // Unregister all remote events.
    function unregisterAllRemoteEvents(): Void;
      // Set the package download cache directory.
    function setPackageCacheDir(path: String): Void;
      // Perform an HTTP request to the specified URL. Empty verb defaults to a GET request. Return a request object which can be used to read the response data.
    function makeHttpRequest(url: String, ?verb: String, ?headers: Array<String>, ?postData: String): HttpRequest;
      // Return network update FPS.
    function getUpdateFps(): Int;
      // Return simulated latency in milliseconds.
    function getSimulatedLatency(): Int;
      // Return simulated packet loss probability.
    function getSimulatedPacketLoss(): Float;
      // Return whether the server is running.
    function isServerRunning(): Bool;
      // Return whether a remote event is allowed to be received.
    function checkRemoteEvent(eventType: String): Bool;
      // Return the package download cache directory.
    function getPackageCacheDir(): String;
      // Process incoming messages from connections. Called by HandleBeginFrame.
    function update(timeStep: Float): Void;
      // Send outgoing messages after frame logic. Called by HandleRenderUpdate.
    function postUpdate(timeStep: Float): Void;

}

@:native("Atomic.NetworkPriority")
extern class NetworkPriority extends Component {

    var basePriority: Float;
    var distanceFactor: Float;
    var minPriority: Float;
    var alwaysUpdateOwner: Bool;

      // Construct.
    function new();

      // Set base priority. Default 100 (send updates at full frequency.)
    function setBasePriority(priority: Float): Void;
      // Set priority reduction distance factor. Default 0 (no effect.)
    function setDistanceFactor(factor: Float): Void;
      // Set minimum priority. Default 0 (no updates when far away enough.)
    function setMinPriority(priority: Float): Void;
      // Set whether updates to owner should be sent always at full rate. Default true.
    function setAlwaysUpdateOwner(enable: Bool): Void;
      // Return base priority.
    function getBasePriority(): Float;
      // Return priority reduction distance factor.
    function getDistanceFactor(): Float;
      // Return minimum priority.
    function getMinPriority(): Float;
      // Return whether updates to owner should be sent always at full rate.
    function getAlwaysUpdateOwner(): Bool;

}



//----------------------------------------------------
// MODULE: IO
//----------------------------------------------------


@:native("Atomic.File")
extern class File extends AObject {

    var name: String;
    var checksum: Int;
    var fullPath: String;
    var mode: FileMode;

      // Construct and open a filesystem file.
    function new(fileName: String, ?mode: FileMode);

      // Set position from the beginning of the file.
    function seek(position: Int): Int;
      // Return the file name.
    function getName(): String;
      // Return a checksum of the file contents using the SDBM hash algorithm.
    function getChecksum(): Int;
      // Open a filesystem file. Return true if successful.
    function open(fileName: String, ?mode: FileMode): Bool;
      // Close the file.
    function close(): Void;
      // Flush any buffered output to the file.
    function flush(): Void;
      // Change the file name. Used by the resource system.
    function setName(name: String): Void;
      // Set the fullpath to the file
    function setFullPath(path: String): Void;
      // Return the open mode.
    function getMode(): FileMode;
      // Return whether is open.
    function isOpen(): Bool;
      // Return whether the file originates from a package.
    function isPackaged(): Bool;
      // Return the fullpath to the file
    function getFullPath(): String;
      // Unlike FileSystem.Copy this copy works when the source file is in a package file
    function copy(srcFile: File): Bool;

}

@:native("Atomic.FileSystem")
extern class FileSystem extends AObject {

    var executeConsoleCommands: Bool;
    var currentDir: String;
    var programDir: String;
    var userDocumentsDir: String;
    var appBundleResourceFolder: String;

      // Construct.
    function new();

      // Set the current working directory.
    function setCurrentDir(pathName: String): Bool;
      // Create a directory.
    function createDir(pathName: String): Bool;
      // Set whether to execute engine console commands as OS-specific system command.
    function setExecuteConsoleCommands(enable: Bool): Void;
      // Run a program using the command interpreter, block until it exits and return the exit code. Will fail if any allowed paths are defined.
    function systemCommand(commandLine: String, ?redirectStdOutToLog: Bool): Int;
      // Run a specific program, block until it exits and return the exit code. Will fail if any allowed paths are defined.
    function systemRun(fileName: String, arguments: Array<String>): Int;
      // Run a program using the command interpreter asynchronously. Return a request ID or M_MAX_UNSIGNED if failed. The exit code will be posted together with the request ID in an AsyncExecFinished event. Will fail if any allowed paths are defined.
    function systemCommandAsync(commandLine: String): Int;
      // Run a specific program asynchronously. Return a request ID or M_MAX_UNSIGNED if failed. The exit code will be posted together with the request ID in an AsyncExecFinished event. Will fail if any allowed paths are defined.
    function systemRunAsync(fileName: String, arguments: Array<String>): Int;
      // Open a file in an external program, with mode such as "edit" optionally specified. Will fail if any allowed paths are defined.
    function systemOpen(fileName: String, ?mode: String): Bool;
      // Copy a file. Return true if successful.
    function copy(srcFileName: String, destFileName: String): Bool;
      // Rename a file. Return true if successful.
    function rename(srcFileName: String, destFileName: String): Bool;
      // Delete a file. Return true if successful.
    function delete(fileName: String): Bool;
      // Register a path as allowed to access. If no paths are registered, all are allowed. Registering allowed paths is considered securing the Atomic execution environment: running programs and opening files externally through the system will fail afterward.
    function registerPath(pathName: String): Void;
      // Set a file's last modified time as seconds since 1.1.1970. Return true on success.
    function setLastModifiedTime(fileName: String, newTime: Int): Bool;
      // Return the absolute current working directory.
    function getCurrentDir(): String;
      // Return whether is executing engine console commands as OS-specific system command.
    function getExecuteConsoleCommands(): Bool;
      // Return whether paths have been registered.
    function hasRegisteredPaths(): Bool;
      // Check if a path is allowed to be accessed. If no paths are registered, all are allowed.
    function checkAccess(pathName: String): Bool;
      // Returns the file's last modified time as seconds since 1.1.1970, or 0 if can not be accessed.
    function getLastModifiedTime(fileName: String): Int;
      // Check if a file exists.
    function fileExists(fileName: String): Bool;
      // Check if a directory exists.
    function dirExists(pathName: String): Bool;
      // Return the program's directory. If it does not contain the Atomic default CoreData and Data directories, and the current working directory does, return the working directory instead.
    function getProgramDir(): String;
      // Return the user documents directory.
    function getUserDocumentsDir(): String;
      // Return the application preferences directory.
    function getAppPreferencesDir(org: String, app: String): String;
    function getAppBundleResourceFolder(): String;
      // Remove a directory
    function removeDir(directoryIn: String, recursive: Bool): Bool;
      // Create directory and all necessary subdirectories below a given root
    function createDirs(root: String, subdirectory: String): Bool;
      // Copy a directory, directoryOut must not exist
    function copyDir(directoryIn: String, directoryOut: String): Bool;
      // Check if a file or directory exists at the specified path
    function exists(pathName: String): Bool;
    function createDirsRecursive(directoryIn: String, directoryOut: String): Bool;

}

@:native("Atomic.FileWatcher")
extern class FileWatcher extends AObject {

    var delay: Float;
    var path: String;

      // Construct.
    function new();

      // Directory watching loop.
    function threadFunction(): Void;
      // Start watching a directory. Return true if successful.
    function startWatching(pathName: String, watchSubDirs: Bool): Bool;
      // Stop watching the directory.
    function stopWatching(): Void;
      // Set the delay in seconds before file changes are notified. This (hopefully) avoids notifying when a file save is still in progress. Default 1 second.
    function setDelay(interval: Float): Void;
      // Add a file change into the changes queue.
    function addChange(fileName: String): Void;
      // Return the path being watched, or empty if not watching.
    function getPath(): String;
      // Return the delay in seconds for notifying file changes.
    function getDelay(): Float;

}

@:native("Atomic.Log")
extern class Log extends AObject {

    var level: Int;
    var timeStamp: Bool;
    var quiet: Bool;
    var lastMessage: String;

      // Construct.
    function new();

      // Open the log file.
    function open(fileName: String): Void;
      // Close the log file.
    function close(): Void;
      // Set logging level.
    function setLevel(level: Int): Void;
      // Set whether to timestamp log messages.
    function setTimeStamp(enable: Bool): Void;
      // Set quiet mode ie. only print error entries to standard error stream (which is normally redirected to console also). Output to log file is not affected by this mode.
    function setQuiet(quiet: Bool): Void;
      // Return logging level.
    function getLevel(): Int;
      // Return whether log messages are timestamped.
    function getTimeStamp(): Bool;
      // Return last log message.
    function getLastMessage(): String;
      // Return whether log is in quiet mode (only errors printed to standard error stream).
    function isQuiet(): Bool;
      // Write to the log. If logging level is higher than the level of the message, the message is ignored.
    function write(level: Int, message: String): Void;
      // Write raw output to the log.
    function writeRaw(message: String, ?error: Bool): Void;

}



//----------------------------------------------------
// MODULE: Engine
//----------------------------------------------------


@:native("Atomic.Engine")
extern class Engine extends AObject {

    var minFps: Int;
    var maxFps: Int;
    var maxInactiveFps: Int;
    var timeStepSmoothing: Int;
    var pauseMinimized: Bool;
    var autoExit: Bool;
    var nextTimeStep: Float;

      // Construct.
    function new();

      // Run one frame.
    function runFrame(): Void;
      // Set minimum frames per second. If FPS goes lower than this, time will appear to slow down.
    function setMinFps(fps: Int): Void;
      // Set maximum frames per second. The engine will sleep if FPS is higher than this.
    function setMaxFps(fps: Int): Void;
      // Set maximum frames per second when the application does not have input focus.
    function setMaxInactiveFps(fps: Int): Void;
      // Set how many frames to average for timestep smoothing. Default is 2. 1 disables smoothing.
    function setTimeStepSmoothing(frames: Int): Void;
      // Set whether to pause update events and audio when minimized.
    function setPauseMinimized(enable: Bool): Void;
      // Set whether to exit automatically on exit request (window close button.)
    function setAutoExit(enable: Bool): Void;
      // Override timestep of the next frame. Should be called in between RunFrame() calls.
    function setNextTimeStep(seconds: Float): Void;
      // Close the graphics window and set the exit flag. No-op on iOS, as an iOS application can not legally exit.
    function exit(): Void;
      // Dump profiling information to the log.
    function dumpProfiler(): Void;
      // Dump information of all resources to the log.
    function dumpResources(?dumpFileName: Bool): Void;
      // Dump information of all memory allocations to the log. Supported in MSVC debug mode only.
    function dumpMemory(): Void;
      // Get timestep of the next frame. Updated by ApplyFrameLimit().
    function getNextTimeStep(): Float;
      // Return the minimum frames per second.
    function getMinFps(): Int;
      // Return the maximum frames per second.
    function getMaxFps(): Int;
      // Return the maximum frames per second when the application does not have input focus.
    function getMaxInactiveFps(): Int;
      // Return how many frames to average for timestep smoothing.
    function getTimeStepSmoothing(): Int;
      // Return whether to pause update events and audio when minimized.
    function getPauseMinimized(): Bool;
      // Return whether to exit automatically on exit request.
    function getAutoExit(): Bool;
      // Return whether engine has been initialized.
    function isInitialized(): Bool;
      // Return whether exit has been requested.
    function isExiting(): Bool;
      // Return whether the engine has been created in headless mode.
    function isHeadless(): Bool;
      // Send frame update events.
    function update(): Void;
      // Render after frame update.
    function render(): Void;
      // Get the timestep for the next frame and sleep for frame limiting if necessary.
    function applyFrameLimit(): Void;

}



//----------------------------------------------------
// MODULE: Javascript
//----------------------------------------------------


@:native("Atomic.JSComponent")
extern class JSComponent extends Component {

    var componentFile: JSComponentFile;
    var updateEventMask: Dynamic;

      // Construct.
    function new();

    @:overload(function(): Void{})
    override function applyAttributes(): Void;
    function getComponentFile(): JSComponentFile;
      // Match script name
    function matchScriptName(path: String): Bool;
      // Handle enabled/disabled state change. Changes update event subscription.
    @:overload(function(): Void{})
    override function onSetEnabled(): Void;
      // Set what update events should be subscribed to. Use this for optimization: by default all are in use. Note that this is not an attribute and is not saved or network-serialized, therefore it should always be called eg. in the subclass constructor.
    function setUpdateEventMask(mask: Dynamic): Void;
      // Return what update events are subscribed to.
    function getUpdateEventMask(): Dynamic;
      // Return whether the DelayedStart() function has been called.
    function isDelayedStartCalled(): Bool;
    function setComponentFile(cfile: JSComponentFile): Void;
    function setDestroyed(): Void;
    function initInstance(?hasArgs: Bool, ?argIdx: Int): Void;

}

@:native("Atomic.JSComponentFile")
extern class JSComponentFile extends Resource {

    var scriptClass: Bool;

      // Construct.
    function new();

    function getScriptClass(): Bool;
    function createJSComponent(): JSComponent;
    function pushModule(): Bool;

}

@:native("Atomic.JSEventHelper")
extern class JSEventHelper extends AObject {

      // Construct.
    function new(object: AObject);


}

@:native("Atomic.ScriptObject")
extern class ScriptObject extends AObject {

      // Construct.
    function new();


}



//----------------------------------------------------
// MODULE: Environment
//----------------------------------------------------


@:native("Atomic.ProcSky")
extern class ProcSky extends Drawable {

    var dayTime: Float;
    var autoUpdate: Bool;
    var timeOfDay: Float;

    function new();

    function setDayTime(time: Float): Float;
    function getDayTime(): Float;
    function setAutoUpdate(autoUpdate: Bool): Void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
    @:overload(function(): UpdateGeometryType{})
    override function getUpdateGeometryType(): UpdateGeometryType;
    function getTimeOfDay(): Float;

}


