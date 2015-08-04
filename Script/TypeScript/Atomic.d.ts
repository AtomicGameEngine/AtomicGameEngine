//Atomic TypeScript Definitions


declare module Atomic {


   // enum FrustumPlane
   export type FrustumPlane = number;
   export var PLANE_NEAR: FrustumPlane;
   export var PLANE_LEFT: FrustumPlane;
   export var PLANE_RIGHT: FrustumPlane;
   export var PLANE_UP: FrustumPlane;
   export var PLANE_DOWN: FrustumPlane;
   export var PLANE_FAR: FrustumPlane;


   // enum Intersection
   export type Intersection = number;
   export var OUTSIDE: Intersection;
   export var INTERSECTS: Intersection;
   export var INSIDE: Intersection;


   // enum InterpolationMode
   export type InterpolationMode = number;
   export var BEZIER_CURVE: InterpolationMode;


   // enum VariantType
   export type VariantType = number;
   export var VAR_NONE: VariantType;
   export var VAR_INT: VariantType;
   export var VAR_BOOL: VariantType;
   export var VAR_FLOAT: VariantType;
   export var VAR_VECTOR2: VariantType;
   export var VAR_VECTOR3: VariantType;
   export var VAR_VECTOR4: VariantType;
   export var VAR_QUATERNION: VariantType;
   export var VAR_COLOR: VariantType;
   export var VAR_STRING: VariantType;
   export var VAR_BUFFER: VariantType;
   export var VAR_VOIDPTR: VariantType;
   export var VAR_RESOURCEREF: VariantType;
   export var VAR_RESOURCEREFLIST: VariantType;
   export var VAR_VARIANTVECTOR: VariantType;
   export var VAR_VARIANTMAP: VariantType;
   export var VAR_INTRECT: VariantType;
   export var VAR_INTVECTOR2: VariantType;
   export var VAR_PTR: VariantType;
   export var VAR_MATRIX3: VariantType;
   export var VAR_MATRIX3X4: VariantType;
   export var VAR_MATRIX4: VariantType;
   export var VAR_DOUBLE: VariantType;
   export var MAX_VAR_TYPES: VariantType;


   // enum WrapMode
   export type WrapMode = number;
   export var WM_LOOP: WrapMode;
   export var WM_ONCE: WrapMode;
   export var WM_CLAMP: WrapMode;


   // enum CreateMode
   export type CreateMode = number;
   export var REPLICATED: CreateMode;
   export var LOCAL: CreateMode;


   // enum TransformSpace
   export type TransformSpace = number;
   export var TS_LOCAL: TransformSpace;
   export var TS_PARENT: TransformSpace;
   export var TS_WORLD: TransformSpace;


   // enum LoadMode
   export type LoadMode = number;
   export var LOAD_RESOURCES_ONLY: LoadMode;
   export var LOAD_SCENE: LoadMode;
   export var LOAD_SCENE_AND_RESOURCES: LoadMode;


   // enum InterpMethod
   export type InterpMethod = number;
   export var IM_LINEAR: InterpMethod;
   export var IM_SPLINE: InterpMethod;


   // enum UpdateGeometryType
   export type UpdateGeometryType = number;
   export var UPDATE_NONE: UpdateGeometryType;
   export var UPDATE_MAIN_THREAD: UpdateGeometryType;
   export var UPDATE_WORKER_THREAD: UpdateGeometryType;


   // enum PrimitiveType
   export type PrimitiveType = number;
   export var TRIANGLE_LIST: PrimitiveType;
   export var LINE_LIST: PrimitiveType;
   export var POINT_LIST: PrimitiveType;
   export var TRIANGLE_STRIP: PrimitiveType;
   export var LINE_STRIP: PrimitiveType;
   export var TRIANGLE_FAN: PrimitiveType;


   // enum GeometryType
   export type GeometryType = number;
   export var GEOM_STATIC: GeometryType;
   export var GEOM_SKINNED: GeometryType;
   export var GEOM_INSTANCED: GeometryType;
   export var GEOM_BILLBOARD: GeometryType;
   export var GEOM_STATIC_NOINSTANCING: GeometryType;
   export var MAX_GEOMETRYTYPES: GeometryType;


   // enum BlendMode
   export type BlendMode = number;
   export var BLEND_REPLACE: BlendMode;
   export var BLEND_ADD: BlendMode;
   export var BLEND_MULTIPLY: BlendMode;
   export var BLEND_ALPHA: BlendMode;
   export var BLEND_ADDALPHA: BlendMode;
   export var BLEND_PREMULALPHA: BlendMode;
   export var BLEND_INVDESTALPHA: BlendMode;
   export var BLEND_SUBTRACT: BlendMode;
   export var BLEND_SUBTRACTALPHA: BlendMode;
   export var MAX_BLENDMODES: BlendMode;


   // enum CompareMode
   export type CompareMode = number;
   export var CMP_ALWAYS: CompareMode;
   export var CMP_EQUAL: CompareMode;
   export var CMP_NOTEQUAL: CompareMode;
   export var CMP_LESS: CompareMode;
   export var CMP_LESSEQUAL: CompareMode;
   export var CMP_GREATER: CompareMode;
   export var CMP_GREATEREQUAL: CompareMode;
   export var MAX_COMPAREMODES: CompareMode;


   // enum CullMode
   export type CullMode = number;
   export var CULL_NONE: CullMode;
   export var CULL_CCW: CullMode;
   export var CULL_CW: CullMode;
   export var MAX_CULLMODES: CullMode;


   // enum FillMode
   export type FillMode = number;
   export var FILL_SOLID: FillMode;
   export var FILL_WIREFRAME: FillMode;
   export var FILL_POINT: FillMode;


   // enum StencilOp
   export type StencilOp = number;
   export var OP_KEEP: StencilOp;
   export var OP_ZERO: StencilOp;
   export var OP_REF: StencilOp;
   export var OP_INCR: StencilOp;
   export var OP_DECR: StencilOp;


   // enum LockState
   export type LockState = number;
   export var LOCK_NONE: LockState;
   export var LOCK_HARDWARE: LockState;
   export var LOCK_SHADOW: LockState;
   export var LOCK_SCRATCH: LockState;


   // enum VertexElement
   export type VertexElement = number;
   export var ELEMENT_POSITION: VertexElement;
   export var ELEMENT_NORMAL: VertexElement;
   export var ELEMENT_COLOR: VertexElement;
   export var ELEMENT_TEXCOORD1: VertexElement;
   export var ELEMENT_TEXCOORD2: VertexElement;
   export var ELEMENT_CUBETEXCOORD1: VertexElement;
   export var ELEMENT_CUBETEXCOORD2: VertexElement;
   export var ELEMENT_TANGENT: VertexElement;
   export var ELEMENT_BLENDWEIGHTS: VertexElement;
   export var ELEMENT_BLENDINDICES: VertexElement;
   export var ELEMENT_INSTANCEMATRIX1: VertexElement;
   export var ELEMENT_INSTANCEMATRIX2: VertexElement;
   export var ELEMENT_INSTANCEMATRIX3: VertexElement;
   export var MAX_VERTEX_ELEMENTS: VertexElement;


   // enum TextureFilterMode
   export type TextureFilterMode = number;
   export var FILTER_NEAREST: TextureFilterMode;
   export var FILTER_BILINEAR: TextureFilterMode;
   export var FILTER_TRILINEAR: TextureFilterMode;
   export var FILTER_ANISOTROPIC: TextureFilterMode;
   export var FILTER_DEFAULT: TextureFilterMode;
   export var MAX_FILTERMODES: TextureFilterMode;


   // enum TextureAddressMode
   export type TextureAddressMode = number;
   export var ADDRESS_WRAP: TextureAddressMode;
   export var ADDRESS_MIRROR: TextureAddressMode;
   export var ADDRESS_CLAMP: TextureAddressMode;
   export var ADDRESS_BORDER: TextureAddressMode;
   export var MAX_ADDRESSMODES: TextureAddressMode;


   // enum TextureCoordinate
   export type TextureCoordinate = number;
   export var COORD_U: TextureCoordinate;
   export var COORD_V: TextureCoordinate;
   export var COORD_W: TextureCoordinate;
   export var MAX_COORDS: TextureCoordinate;


   // enum TextureUsage
   export type TextureUsage = number;
   export var TEXTURE_STATIC: TextureUsage;
   export var TEXTURE_DYNAMIC: TextureUsage;
   export var TEXTURE_RENDERTARGET: TextureUsage;
   export var TEXTURE_DEPTHSTENCIL: TextureUsage;


   // enum CubeMapFace
   export type CubeMapFace = number;
   export var FACE_POSITIVE_X: CubeMapFace;
   export var FACE_NEGATIVE_X: CubeMapFace;
   export var FACE_POSITIVE_Y: CubeMapFace;
   export var FACE_NEGATIVE_Y: CubeMapFace;
   export var FACE_POSITIVE_Z: CubeMapFace;
   export var FACE_NEGATIVE_Z: CubeMapFace;
   export var MAX_CUBEMAP_FACES: CubeMapFace;


   // enum CubeMapLayout
   export type CubeMapLayout = number;
   export var CML_HORIZONTAL: CubeMapLayout;
   export var CML_HORIZONTALNVIDIA: CubeMapLayout;
   export var CML_HORIZONTALCROSS: CubeMapLayout;
   export var CML_VERTICALCROSS: CubeMapLayout;
   export var CML_BLENDER: CubeMapLayout;


   // enum RenderSurfaceUpdateMode
   export type RenderSurfaceUpdateMode = number;
   export var SURFACE_MANUALUPDATE: RenderSurfaceUpdateMode;
   export var SURFACE_UPDATEVISIBLE: RenderSurfaceUpdateMode;
   export var SURFACE_UPDATEALWAYS: RenderSurfaceUpdateMode;


   // enum ShaderType
   export type ShaderType = number;
   export var VS: ShaderType;
   export var PS: ShaderType;


   // enum ShaderParameterGroup
   export type ShaderParameterGroup = number;
   export var SP_FRAME: ShaderParameterGroup;
   export var SP_CAMERA: ShaderParameterGroup;
   export var SP_ZONE: ShaderParameterGroup;
   export var SP_LIGHT: ShaderParameterGroup;
   export var SP_MATERIAL: ShaderParameterGroup;
   export var SP_OBJECT: ShaderParameterGroup;
   export var SP_CUSTOM: ShaderParameterGroup;
   export var MAX_SHADER_PARAMETER_GROUPS: ShaderParameterGroup;


   // enum TextureUnit
   export type TextureUnit = number;
   export var TU_DIFFUSE: TextureUnit;
   export var TU_ALBEDOBUFFER: TextureUnit;
   export var TU_NORMAL: TextureUnit;
   export var TU_NORMALBUFFER: TextureUnit;
   export var TU_SPECULAR: TextureUnit;
   export var TU_EMISSIVE: TextureUnit;
   export var TU_ENVIRONMENT: TextureUnit;
   export var TU_VOLUMEMAP: TextureUnit;
   export var TU_CUSTOM1: TextureUnit;
   export var TU_CUSTOM2: TextureUnit;
   export var TU_LIGHTRAMP: TextureUnit;
   export var TU_LIGHTSHAPE: TextureUnit;
   export var TU_SHADOWMAP: TextureUnit;
   export var TU_FACESELECT: TextureUnit;
   export var TU_INDIRECTION: TextureUnit;
   export var TU_DEPTHBUFFER: TextureUnit;
   export var TU_LIGHTBUFFER: TextureUnit;
   export var TU_ZONE: TextureUnit;
   export var MAX_MATERIAL_TEXTURE_UNITS: TextureUnit;
   export var MAX_TEXTURE_UNITS: TextureUnit;


   // enum FaceCameraMode
   export type FaceCameraMode = number;
   export var FC_NONE: FaceCameraMode;
   export var FC_ROTATE_XYZ: FaceCameraMode;
   export var FC_ROTATE_Y: FaceCameraMode;
   export var FC_LOOKAT_XYZ: FaceCameraMode;
   export var FC_LOOKAT_Y: FaceCameraMode;


   // enum LightType
   export type LightType = number;
   export var LIGHT_DIRECTIONAL: LightType;
   export var LIGHT_SPOT: LightType;
   export var LIGHT_POINT: LightType;


   // enum RayQueryLevel
   export type RayQueryLevel = number;
   export var RAY_AABB: RayQueryLevel;
   export var RAY_OBB: RayQueryLevel;
   export var RAY_TRIANGLE: RayQueryLevel;
   export var RAY_TRIANGLE_UV: RayQueryLevel;


   // enum LightVSVariation
   export type LightVSVariation = number;
   export var LVS_DIR: LightVSVariation;
   export var LVS_SPOT: LightVSVariation;
   export var LVS_POINT: LightVSVariation;
   export var LVS_SHADOW: LightVSVariation;
   export var LVS_SPOTSHADOW: LightVSVariation;
   export var LVS_POINTSHADOW: LightVSVariation;
   export var MAX_LIGHT_VS_VARIATIONS: LightVSVariation;


   // enum VertexLightVSVariation
   export type VertexLightVSVariation = number;
   export var VLVS_NOLIGHTS: VertexLightVSVariation;
   export var VLVS_1LIGHT: VertexLightVSVariation;
   export var VLVS_2LIGHTS: VertexLightVSVariation;
   export var VLVS_3LIGHTS: VertexLightVSVariation;
   export var VLVS_4LIGHTS: VertexLightVSVariation;
   export var MAX_VERTEXLIGHT_VS_VARIATIONS: VertexLightVSVariation;


   // enum LightPSVariation
   export type LightPSVariation = number;
   export var LPS_NONE: LightPSVariation;
   export var LPS_SPOT: LightPSVariation;
   export var LPS_POINT: LightPSVariation;
   export var LPS_POINTMASK: LightPSVariation;
   export var LPS_SPEC: LightPSVariation;
   export var LPS_SPOTSPEC: LightPSVariation;
   export var LPS_POINTSPEC: LightPSVariation;
   export var LPS_POINTMASKSPEC: LightPSVariation;
   export var LPS_SHADOW: LightPSVariation;
   export var LPS_SPOTSHADOW: LightPSVariation;
   export var LPS_POINTSHADOW: LightPSVariation;
   export var LPS_POINTMASKSHADOW: LightPSVariation;
   export var LPS_SHADOWSPEC: LightPSVariation;
   export var LPS_SPOTSHADOWSPEC: LightPSVariation;
   export var LPS_POINTSHADOWSPEC: LightPSVariation;
   export var LPS_POINTMASKSHADOWSPEC: LightPSVariation;
   export var MAX_LIGHT_PS_VARIATIONS: LightPSVariation;


   // enum DeferredLightVSVariation
   export type DeferredLightVSVariation = number;
   export var DLVS_NONE: DeferredLightVSVariation;
   export var DLVS_DIR: DeferredLightVSVariation;
   export var DLVS_ORTHO: DeferredLightVSVariation;
   export var DLVS_ORTHODIR: DeferredLightVSVariation;
   export var MAX_DEFERRED_LIGHT_VS_VARIATIONS: DeferredLightVSVariation;


   // enum DeferredLightPSVariation
   export type DeferredLightPSVariation = number;
   export var DLPS_NONE: DeferredLightPSVariation;
   export var DLPS_SPOT: DeferredLightPSVariation;
   export var DLPS_POINT: DeferredLightPSVariation;
   export var DLPS_POINTMASK: DeferredLightPSVariation;
   export var DLPS_SPEC: DeferredLightPSVariation;
   export var DLPS_SPOTSPEC: DeferredLightPSVariation;
   export var DLPS_POINTSPEC: DeferredLightPSVariation;
   export var DLPS_POINTMASKSPEC: DeferredLightPSVariation;
   export var DLPS_SHADOW: DeferredLightPSVariation;
   export var DLPS_SPOTSHADOW: DeferredLightPSVariation;
   export var DLPS_POINTSHADOW: DeferredLightPSVariation;
   export var DLPS_POINTMASKSHADOW: DeferredLightPSVariation;
   export var DLPS_SHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_SPOTSHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_POINTSHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_POINTMASKSHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHO: DeferredLightPSVariation;
   export var DLPS_ORTHOSPOT: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINT: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTMASK: DeferredLightPSVariation;
   export var DLPS_ORTHOSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOSPOTSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTMASKSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOSHADOW: DeferredLightPSVariation;
   export var DLPS_ORTHOSPOTSHADOW: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTSHADOW: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTMASKSHADOW: DeferredLightPSVariation;
   export var DLPS_ORTHOSHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOSPOTSHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTSHADOWSPEC: DeferredLightPSVariation;
   export var DLPS_ORTHOPOINTMASKSHADOWSPEC: DeferredLightPSVariation;
   export var MAX_DEFERRED_LIGHT_PS_VARIATIONS: DeferredLightPSVariation;


   // enum RenderCommandType
   export type RenderCommandType = number;
   export var CMD_NONE: RenderCommandType;
   export var CMD_CLEAR: RenderCommandType;
   export var CMD_SCENEPASS: RenderCommandType;
   export var CMD_QUAD: RenderCommandType;
   export var CMD_FORWARDLIGHTS: RenderCommandType;
   export var CMD_LIGHTVOLUMES: RenderCommandType;
   export var CMD_RENDERUI: RenderCommandType;


   // enum RenderCommandSortMode
   export type RenderCommandSortMode = number;
   export var SORT_FRONTTOBACK: RenderCommandSortMode;
   export var SORT_BACKTOFRONT: RenderCommandSortMode;


   // enum RenderTargetSizeMode
   export type RenderTargetSizeMode = number;
   export var SIZE_ABSOLUTE: RenderTargetSizeMode;
   export var SIZE_VIEWPORTDIVISOR: RenderTargetSizeMode;
   export var SIZE_VIEWPORTMULTIPLIER: RenderTargetSizeMode;


   // enum PassLightingMode
   export type PassLightingMode = number;
   export var LIGHTING_UNLIT: PassLightingMode;
   export var LIGHTING_PERVERTEX: PassLightingMode;
   export var LIGHTING_PERPIXEL: PassLightingMode;


   // enum EmitterType
   export type EmitterType = number;
   export var EMITTER_SPHERE: EmitterType;
   export var EMITTER_BOX: EmitterType;


   // enum LoopMode2D
   export type LoopMode2D = number;
   export var LM_DEFAULT: LoopMode2D;
   export var LM_FORCE_LOOPED: LoopMode2D;
   export var LM_FORCE_CLAMPED: LoopMode2D;


   // enum LightType2D
   export type LightType2D = number;
   export var LIGHT2D_DIRECTIONAL: LightType2D;
   export var LIGHT2D_POINT: LightType2D;


   // enum EmitterType2D
   export type EmitterType2D = number;
   export var EMITTER_TYPE_GRAVITY: EmitterType2D;
   export var EMITTER_TYPE_RADIAL: EmitterType2D;


   // enum BodyType2D
   export type BodyType2D = number;
   export var BT_STATIC: BodyType2D;
   export var BT_DYNAMIC: BodyType2D;
   export var BT_KINEMATIC: BodyType2D;


   // enum Orientation2D
   export type Orientation2D = number;
   export var O_ORTHOGONAL: Orientation2D;
   export var O_ISOMETRIC: Orientation2D;
   export var O_STAGGERED: Orientation2D;


   // enum TileMapLayerType2D
   export type TileMapLayerType2D = number;
   export var LT_TILE_LAYER: TileMapLayerType2D;
   export var LT_OBJECT_GROUP: TileMapLayerType2D;
   export var LT_IMAGE_LAYER: TileMapLayerType2D;
   export var LT_INVALID: TileMapLayerType2D;


   // enum TileMapObjectType2D
   export type TileMapObjectType2D = number;
   export var OT_RECTANGLE: TileMapObjectType2D;
   export var OT_ELLIPSE: TileMapObjectType2D;
   export var OT_POLYGON: TileMapObjectType2D;
   export var OT_POLYLINE: TileMapObjectType2D;
   export var OT_TILE: TileMapObjectType2D;
   export var OT_INVALID: TileMapObjectType2D;


   // enum ConstraintType
   export type ConstraintType = number;
   export var CONSTRAINT_POINT: ConstraintType;
   export var CONSTRAINT_HINGE: ConstraintType;
   export var CONSTRAINT_SLIDER: ConstraintType;
   export var CONSTRAINT_CONETWIST: ConstraintType;


   // enum CollisionEventMode
   export type CollisionEventMode = number;
   export var COLLISION_NEVER: CollisionEventMode;
   export var COLLISION_ACTIVE: CollisionEventMode;
   export var COLLISION_ALWAYS: CollisionEventMode;


   // enum ShapeType
   export type ShapeType = number;
   export var SHAPE_BOX: ShapeType;
   export var SHAPE_SPHERE: ShapeType;
   export var SHAPE_STATICPLANE: ShapeType;
   export var SHAPE_CYLINDER: ShapeType;
   export var SHAPE_CAPSULE: ShapeType;
   export var SHAPE_CONE: ShapeType;
   export var SHAPE_TRIANGLEMESH: ShapeType;
   export var SHAPE_CONVEXHULL: ShapeType;
   export var SHAPE_TERRAIN: ShapeType;


   // enum NavmeshPartitionType
   export type NavmeshPartitionType = number;
   export var NAVMESH_PARTITION_WATERSHED: NavmeshPartitionType;
   export var NAVMESH_PARTITION_MONOTONE: NavmeshPartitionType;


   // enum CrowdTargetState
   export type CrowdTargetState = number;
   export var CROWD_AGENT_TARGET_NONE: CrowdTargetState;
   export var CROWD_AGENT_TARGET_FAILED: CrowdTargetState;
   export var CROWD_AGENT_TARGET_VALID: CrowdTargetState;
   export var CROWD_AGENT_TARGET_REQUESTING: CrowdTargetState;
   export var CROWD_AGENT_TARGET_WAITINGFORQUEUE: CrowdTargetState;
   export var CROWD_AGENT_TARGET_WAITINGFORPATH: CrowdTargetState;
   export var CROWD_AGENT_TARGET_VELOCITY: CrowdTargetState;


   // enum CrowdAgentState
   export type CrowdAgentState = number;
   export var CROWD_AGENT_INVALID: CrowdAgentState;
   export var CROWD_AGENT_READY: CrowdAgentState;
   export var CROWD_AGENT_TRAVERSINGLINK: CrowdAgentState;


   // enum NavigationQuality
   export type NavigationQuality = number;
   export var NAVIGATIONQUALITY_LOW: NavigationQuality;
   export var NAVIGATIONQUALITY_MEDIUM: NavigationQuality;
   export var NAVIGATIONQUALITY_HIGH: NavigationQuality;


   // enum NavigationPushiness
   export type NavigationPushiness = number;
   export var PUSHINESS_LOW: NavigationPushiness;
   export var PUSHINESS_MEDIUM: NavigationPushiness;
   export var PUSHINESS_HIGH: NavigationPushiness;


   // enum MouseMode
   export type MouseMode = number;
   export var MM_ABSOLUTE: MouseMode;
   export var MM_RELATIVE: MouseMode;
   export var MM_WRAP: MouseMode;


   // enum TEXT_ALIGN
   export type TEXT_ALIGN = number;
   export var TEXT_ALIGN_LEFT: TEXT_ALIGN;
   export var TEXT_ALIGN_RIGHT: TEXT_ALIGN;
   export var TEXT_ALIGN_CENTER: TEXT_ALIGN;


   // enum UI_EDIT_TYPE
   export type UI_EDIT_TYPE = number;
   export var UI_EDIT_TYPE_TEXT: UI_EDIT_TYPE;
   export var UI_EDIT_TYPE_SEARCH: UI_EDIT_TYPE;
   export var UI_EDIT_TYPE_PASSWORD: UI_EDIT_TYPE;
   export var UI_EDIT_TYPE_EMAIL: UI_EDIT_TYPE;
   export var UI_EDIT_TYPE_PHONE: UI_EDIT_TYPE;
   export var UI_EDIT_TYPE_URL: UI_EDIT_TYPE;
   export var UI_EDIT_TYPE_NUMBER: UI_EDIT_TYPE;


   // enum UI_AXIS
   export type UI_AXIS = number;
   export var UI_AXIS_X: UI_AXIS;
   export var UI_AXIS_Y: UI_AXIS;


   // enum UI_LAYOUT_SIZE
   export type UI_LAYOUT_SIZE = number;
   export var UI_LAYOUT_SIZE_GRAVITY: UI_LAYOUT_SIZE;
   export var UI_LAYOUT_SIZE_PREFERRED: UI_LAYOUT_SIZE;
   export var UI_LAYOUT_SIZE_AVAILABLE: UI_LAYOUT_SIZE;


   // enum UI_LAYOUT_DISTRIBUTION
   export type UI_LAYOUT_DISTRIBUTION = number;
   export var UI_LAYOUT_DISTRIBUTION_PREFERRED: UI_LAYOUT_DISTRIBUTION;
   export var UI_LAYOUT_DISTRIBUTION_AVAILABLE: UI_LAYOUT_DISTRIBUTION;
   export var UI_LAYOUT_DISTRIBUTION_GRAVITY: UI_LAYOUT_DISTRIBUTION;


   // enum UI_LAYOUT_POSITION
   export type UI_LAYOUT_POSITION = number;
   export var UI_LAYOUT_POSITION_CENTER: UI_LAYOUT_POSITION;
   export var UI_LAYOUT_POSITION_LEFT_TOP: UI_LAYOUT_POSITION;
   export var UI_LAYOUT_POSITION_RIGHT_BOTTOM: UI_LAYOUT_POSITION;
   export var UI_LAYOUT_POSITION_GRAVITY: UI_LAYOUT_POSITION;


   // enum UI_LAYOUT_DISTRIBUTION_POSITION
   export type UI_LAYOUT_DISTRIBUTION_POSITION = number;
   export var UI_LAYOUT_DISTRIBUTION_POSITION_CENTER: UI_LAYOUT_DISTRIBUTION_POSITION;
   export var UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP: UI_LAYOUT_DISTRIBUTION_POSITION;
   export var UI_LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM: UI_LAYOUT_DISTRIBUTION_POSITION;


   // enum UI_MESSAGEWINDOW_SETTINGS
   export type UI_MESSAGEWINDOW_SETTINGS = number;
   export var UI_MESSAGEWINDOW_SETTINGS_OK: UI_MESSAGEWINDOW_SETTINGS;
   export var UI_MESSAGEWINDOW_SETTINGS_OK_CANCEL: UI_MESSAGEWINDOW_SETTINGS;
   export var UI_MESSAGEWINDOW_SETTINGS_YES_NO: UI_MESSAGEWINDOW_SETTINGS;


   // enum UI_SCROLL_MODE
   export type UI_SCROLL_MODE = number;
   export var UI_SCROLL_MODE_X_Y: UI_SCROLL_MODE;
   export var UI_SCROLL_MODE_Y: UI_SCROLL_MODE;
   export var UI_SCROLL_MODE_Y_AUTO: UI_SCROLL_MODE;
   export var UI_SCROLL_MODE_X_AUTO_Y_AUTO: UI_SCROLL_MODE;
   export var UI_SCROLL_MODE_OFF: UI_SCROLL_MODE;


   // enum UI_TEXT_ALIGN
   export type UI_TEXT_ALIGN = number;
   export var UI_TEXT_ALIGN_LEFT: UI_TEXT_ALIGN;
   export var UI_TEXT_ALIGN_RIGHT: UI_TEXT_ALIGN;
   export var UI_TEXT_ALIGN_CENTER: UI_TEXT_ALIGN;


   // enum UI_WIDGET_VISIBILITY
   export type UI_WIDGET_VISIBILITY = number;
   export var UI_WIDGET_VISIBILITY_VISIBLE: UI_WIDGET_VISIBILITY;
   export var UI_WIDGET_VISIBILITY_INVISIBLE: UI_WIDGET_VISIBILITY;
   export var UI_WIDGET_VISIBILITY_GONE: UI_WIDGET_VISIBILITY;


   // enum UI_GRAVITY
   export type UI_GRAVITY = number;
   export var UI_GRAVITY_NONE: UI_GRAVITY;
   export var UI_GRAVITY_LEFT: UI_GRAVITY;
   export var UI_GRAVITY_RIGHT: UI_GRAVITY;
   export var UI_GRAVITY_TOP: UI_GRAVITY;
   export var UI_GRAVITY_BOTTOM: UI_GRAVITY;
   export var UI_GRAVITY_LEFT_RIGHT: UI_GRAVITY;
   export var UI_GRAVITY_TOP_BOTTOM: UI_GRAVITY;
   export var UI_GRAVITY_ALL: UI_GRAVITY;
   export var UI_GRAVITY_DEFAULT: UI_GRAVITY;


   // enum UI_EVENT_TYPE
   export type UI_EVENT_TYPE = number;
   export var UI_EVENT_TYPE_CLICK: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_LONG_CLICK: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_POINTER_DOWN: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_POINTER_UP: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_POINTER_MOVE: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_RIGHT_POINTER_DOWN: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_RIGHT_POINTER_UP: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_WHEEL: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_CHANGED: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_KEY_DOWN: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_KEY_UP: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_SHORTCUT: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_CONTEXT_MENU: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_FILE_DROP: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_TAB_CHANGED: UI_EVENT_TYPE;
   export var UI_EVENT_TYPE_CUSTOM: UI_EVENT_TYPE;


   // enum UI_WIDGET_Z_REL
   export type UI_WIDGET_Z_REL = number;
   export var UI_WIDGET_Z_REL_BEFORE: UI_WIDGET_Z_REL;
   export var UI_WIDGET_Z_REL_AFTER: UI_WIDGET_Z_REL;


   // enum UI_WINDOW_SETTINGS
   export type UI_WINDOW_SETTINGS = number;
   export var UI_WINDOW_SETTINGS_NONE: UI_WINDOW_SETTINGS;
   export var UI_WINDOW_SETTINGS_TITLEBAR: UI_WINDOW_SETTINGS;
   export var UI_WINDOW_SETTINGS_RESIZABLE: UI_WINDOW_SETTINGS;
   export var UI_WINDOW_SETTINGS_CLOSE_BUTTON: UI_WINDOW_SETTINGS;
   export var UI_WINDOW_SETTINGS_CAN_ACTIVATE: UI_WINDOW_SETTINGS;
   export var UI_WINDOW_SETTINGS_DEFAULT: UI_WINDOW_SETTINGS;


   // enum UI_SIZE_DEP
   export type UI_SIZE_DEP = number;
   export var UI_SIZE_DEP_NONE: UI_SIZE_DEP;
   export var UI_SIZE_DEP_WIDTH_DEPEND_ON_HEIGHT: UI_SIZE_DEP;
   export var UI_SIZE_DEP_HEIGHT_DEPEND_ON_WIDTH: UI_SIZE_DEP;
   export var UI_SIZE_DEP_BOTH: UI_SIZE_DEP;


   // enum CompressedFormat
   export type CompressedFormat = number;
   export var CF_NONE: CompressedFormat;
   export var CF_RGBA: CompressedFormat;
   export var CF_DXT1: CompressedFormat;
   export var CF_DXT3: CompressedFormat;
   export var CF_DXT5: CompressedFormat;
   export var CF_ETC1: CompressedFormat;
   export var CF_PVRTC_RGB_2BPP: CompressedFormat;
   export var CF_PVRTC_RGBA_2BPP: CompressedFormat;
   export var CF_PVRTC_RGB_4BPP: CompressedFormat;
   export var CF_PVRTC_RGBA_4BPP: CompressedFormat;


   // enum JSONValueType
   export type JSONValueType = number;
   export var JSON_ANY: JSONValueType;
   export var JSON_OBJECT: JSONValueType;
   export var JSON_ARRAY: JSONValueType;


   // enum PListValueType
   export type PListValueType = number;
   export var PLVT_NONE: PListValueType;
   export var PLVT_INT: PListValueType;
   export var PLVT_BOOL: PListValueType;
   export var PLVT_FLOAT: PListValueType;
   export var PLVT_STRING: PListValueType;
   export var PLVT_VALUEMAP: PListValueType;
   export var PLVT_VALUEVECTOR: PListValueType;


   // enum AsyncLoadState
   export type AsyncLoadState = number;
   export var ASYNC_DONE: AsyncLoadState;
   export var ASYNC_QUEUED: AsyncLoadState;
   export var ASYNC_LOADING: AsyncLoadState;
   export var ASYNC_SUCCESS: AsyncLoadState;
   export var ASYNC_FAIL: AsyncLoadState;


   // enum ResourceRequest
   export type ResourceRequest = number;
   export var RESOURCE_CHECKEXISTS: ResourceRequest;
   export var RESOURCE_GETFILE: ResourceRequest;


   // enum ObserverPositionSendMode
   export type ObserverPositionSendMode = number;
   export var OPSM_NONE: ObserverPositionSendMode;
   export var OPSM_POSITION: ObserverPositionSendMode;
   export var OPSM_POSITION_ROTATION: ObserverPositionSendMode;


   // enum HttpRequestState
   export type HttpRequestState = number;
   export var HTTP_INITIALIZING: HttpRequestState;
   export var HTTP_ERROR: HttpRequestState;
   export var HTTP_OPEN: HttpRequestState;
   export var HTTP_CLOSED: HttpRequestState;


   // enum FileMode
   export type FileMode = number;
   export var FILE_READ: FileMode;
   export var FILE_WRITE: FileMode;
   export var FILE_READWRITE: FileMode;


   export var QUICKSORT_THRESHOLD: number;
   export var CONVERSION_BUFFER_LENGTH: number;
   export var MATRIX_CONVERSION_BUFFER_LENGTH: number;


   export var NUM_FRUSTUM_PLANES: number;
   export var NUM_FRUSTUM_VERTICES: number;
   export var M_PI: number;
   export var M_HALF_PI: number;
   export var M_MIN_INT: number;
   export var M_MAX_INT: number;
   export var M_MIN_UNSIGNED: number;
   export var M_MAX_UNSIGNED: number;
   export var M_EPSILON: number;
   export var M_LARGE_EPSILON: number;
   export var M_MIN_NEARCLIP: number;
   export var M_MAX_FOV: number;
   export var M_LARGE_VALUE: number;
   export var M_INFINITY: number;
   export var M_DEGTORAD: number;
   export var M_DEGTORAD_2: number;
   export var M_RADTODEG: number;


   export var AM_EDIT: number;
   export var AM_FILE: number;
   export var AM_NET: number;
   export var AM_DEFAULT: number;
   export var AM_LATESTDATA: number;
   export var AM_NOEDIT: number;
   export var AM_NODEID: number;
   export var AM_COMPONENTID: number;
   export var AM_NODEIDVECTOR: number;


   export var USE_UPDATE: number;
   export var USE_POSTUPDATE: number;
   export var USE_FIXEDUPDATE: number;
   export var USE_FIXEDPOSTUPDATE: number;
   export var MAX_NETWORK_ATTRIBUTES: number;
   export var FIRST_REPLICATED_ID: number;
   export var LAST_REPLICATED_ID: number;
   export var FIRST_LOCAL_ID: number;
   export var LAST_LOCAL_ID: number;
   export var SMOOTH_NONE: number;
   export var SMOOTH_POSITION: number;
   export var SMOOTH_ROTATION: number;


   export var DEFAULT_NEARCLIP: number;
   export var DEFAULT_FARCLIP: number;
   export var DEFAULT_CAMERA_FOV: number;
   export var DEFAULT_ORTHOSIZE: number;
   export var VO_NONE: number;
   export var VO_LOW_MATERIAL_QUALITY: number;
   export var VO_DISABLE_SHADOWS: number;
   export var VO_DISABLE_OCCLUSION: number;
   export var DRAWABLE_GEOMETRY: number;
   export var DRAWABLE_LIGHT: number;
   export var DRAWABLE_ZONE: number;
   export var DRAWABLE_GEOMETRY2D: number;
   export var DRAWABLE_ANY: number;
   export var DEFAULT_VIEWMASK: number;
   export var DEFAULT_LIGHTMASK: number;
   export var DEFAULT_SHADOWMASK: number;
   export var DEFAULT_ZONEMASK: number;
   export var MAX_VERTEX_LIGHTS: number;
   export var ANIMATION_LOD_BASESCALE: number;
   export var QUALITY_LOW: number;
   export var QUALITY_MEDIUM: number;
   export var QUALITY_HIGH: number;
   export var QUALITY_MAX: number;
   export var SHADOWQUALITY_LOW_16BIT: number;
   export var SHADOWQUALITY_LOW_24BIT: number;
   export var SHADOWQUALITY_HIGH_16BIT: number;
   export var SHADOWQUALITY_HIGH_24BIT: number;
   export var CLEAR_COLOR: number;
   export var CLEAR_DEPTH: number;
   export var CLEAR_STENCIL: number;
   export var MASK_NONE: number;
   export var MASK_POSITION: number;
   export var MASK_NORMAL: number;
   export var MASK_COLOR: number;
   export var MASK_TEXCOORD1: number;
   export var MASK_TEXCOORD2: number;
   export var MASK_CUBETEXCOORD1: number;
   export var MASK_CUBETEXCOORD2: number;
   export var MASK_TANGENT: number;
   export var MASK_BLENDWEIGHTS: number;
   export var MASK_BLENDINDICES: number;
   export var MASK_INSTANCEMATRIX1: number;
   export var MASK_INSTANCEMATRIX2: number;
   export var MASK_INSTANCEMATRIX3: number;
   export var MASK_DEFAULT: number;
   export var NO_ELEMENT: number;
   export var MAX_RENDERTARGETS: number;
   export var MAX_VERTEX_STREAMS: number;
   export var MAX_CONSTANT_REGISTERS: number;
   export var BITS_PER_COMPONENT: number;
   export var SHADOW_MIN_QUANTIZE: number;
   export var SHADOW_MIN_VIEW: number;
   export var MAX_LIGHT_SPLITS: number;
   export var MAX_CASCADE_SPLITS: number;
   export var OCCLUSION_MIN_SIZE: number;
   export var OCCLUSION_DEFAULT_MAX_TRIANGLES: number;
   export var OCCLUSION_RELATIVE_BIAS: number;
   export var OCCLUSION_FIXED_BIAS: number;
   export var OCCLUSION_X_SCALE: number;
   export var OCCLUSION_Z_SCALE: number;
   export var NUM_OCTANTS: number;
   export var ROOT_INDEX: number;
   export var SHADOW_MIN_PIXELS: number;
   export var INSTANCING_BUFFER_DEFAULT_SIZE: number;
   export var MAX_VIEWPORT_TEXTURES: number;
   export var MAX_TEXTURE_QUALITY_LEVELS: number;


   export var CHANNEL_POSITION: number;
   export var CHANNEL_ROTATION: number;
   export var CHANNEL_SCALE: number;
   export var MAX_BILLBOARDS: number;
   export var DEFAULT_NUM_PARTICLES: number;
   export var BONECOLLISION_NONE: number;
   export var BONECOLLISION_SPHERE: number;
   export var BONECOLLISION_BOX: number;


   export var PIXEL_SIZE: number;


   export var STREAM_BUFFER_LENGTH: number;


   export var DEFAULT_MAX_NETWORK_ANGULAR_VELOCITY: number;


   export var MOUSEB_LEFT: number;
   export var MOUSEB_MIDDLE: number;
   export var MOUSEB_RIGHT: number;
   export var MOUSEB_X1: number;
   export var MOUSEB_X2: number;
   export var QUAL_SHIFT: number;
   export var QUAL_CTRL: number;
   export var QUAL_ALT: number;
   export var QUAL_ANY: number;
   export var KEY_A: number;
   export var KEY_B: number;
   export var KEY_C: number;
   export var KEY_D: number;
   export var KEY_E: number;
   export var KEY_F: number;
   export var KEY_G: number;
   export var KEY_H: number;
   export var KEY_I: number;
   export var KEY_J: number;
   export var KEY_K: number;
   export var KEY_L: number;
   export var KEY_M: number;
   export var KEY_N: number;
   export var KEY_O: number;
   export var KEY_P: number;
   export var KEY_Q: number;
   export var KEY_R: number;
   export var KEY_S: number;
   export var KEY_T: number;
   export var KEY_U: number;
   export var KEY_V: number;
   export var KEY_W: number;
   export var KEY_X: number;
   export var KEY_Y: number;
   export var KEY_Z: number;
   export var KEY_0: number;
   export var KEY_1: number;
   export var KEY_2: number;
   export var KEY_3: number;
   export var KEY_4: number;
   export var KEY_5: number;
   export var KEY_6: number;
   export var KEY_7: number;
   export var KEY_8: number;
   export var KEY_9: number;
   export var KEY_BACKSPACE: number;
   export var KEY_TAB: number;
   export var KEY_RETURN: number;
   export var KEY_RETURN2: number;
   export var KEY_KP_ENTER: number;
   export var KEY_SHIFT: number;
   export var KEY_CTRL: number;
   export var KEY_ALT: number;
   export var KEY_GUI: number;
   export var KEY_PAUSE: number;
   export var KEY_CAPSLOCK: number;
   export var KEY_ESC: number;
   export var KEY_SPACE: number;
   export var KEY_PAGEUP: number;
   export var KEY_PAGEDOWN: number;
   export var KEY_END: number;
   export var KEY_HOME: number;
   export var KEY_LEFT: number;
   export var KEY_UP: number;
   export var KEY_RIGHT: number;
   export var KEY_DOWN: number;
   export var KEY_SELECT: number;
   export var KEY_PRINTSCREEN: number;
   export var KEY_INSERT: number;
   export var KEY_DELETE: number;
   export var KEY_LGUI: number;
   export var KEY_RGUI: number;
   export var KEY_APPLICATION: number;
   export var KEY_KP_0: number;
   export var KEY_KP_1: number;
   export var KEY_KP_2: number;
   export var KEY_KP_3: number;
   export var KEY_KP_4: number;
   export var KEY_KP_5: number;
   export var KEY_KP_6: number;
   export var KEY_KP_7: number;
   export var KEY_KP_8: number;
   export var KEY_KP_9: number;
   export var KEY_KP_MULTIPLY: number;
   export var KEY_KP_PLUS: number;
   export var KEY_KP_MINUS: number;
   export var KEY_KP_PERIOD: number;
   export var KEY_KP_DIVIDE: number;
   export var KEY_F1: number;
   export var KEY_F2: number;
   export var KEY_F3: number;
   export var KEY_F4: number;
   export var KEY_F5: number;
   export var KEY_F6: number;
   export var KEY_F7: number;
   export var KEY_F8: number;
   export var KEY_F9: number;
   export var KEY_F10: number;
   export var KEY_F11: number;
   export var KEY_F12: number;
   export var KEY_F13: number;
   export var KEY_F14: number;
   export var KEY_F15: number;
   export var KEY_F16: number;
   export var KEY_F17: number;
   export var KEY_F18: number;
   export var KEY_F19: number;
   export var KEY_F20: number;
   export var KEY_F21: number;
   export var KEY_F22: number;
   export var KEY_F23: number;
   export var KEY_F24: number;
   export var KEY_NUMLOCKCLEAR: number;
   export var KEY_SCROLLLOCK: number;
   export var KEY_LSHIFT: number;
   export var KEY_RSHIFT: number;
   export var KEY_LCTRL: number;
   export var KEY_RCTRL: number;
   export var KEY_LALT: number;
   export var KEY_RALT: number;
   export var SCANCODE_UNKNOWN: number;
   export var SCANCODE_CTRL: number;
   export var SCANCODE_SHIFT: number;
   export var SCANCODE_ALT: number;
   export var SCANCODE_GUI: number;
   export var SCANCODE_A: number;
   export var SCANCODE_B: number;
   export var SCANCODE_C: number;
   export var SCANCODE_D: number;
   export var SCANCODE_E: number;
   export var SCANCODE_F: number;
   export var SCANCODE_G: number;
   export var SCANCODE_H: number;
   export var SCANCODE_I: number;
   export var SCANCODE_J: number;
   export var SCANCODE_K: number;
   export var SCANCODE_L: number;
   export var SCANCODE_M: number;
   export var SCANCODE_N: number;
   export var SCANCODE_O: number;
   export var SCANCODE_P: number;
   export var SCANCODE_Q: number;
   export var SCANCODE_R: number;
   export var SCANCODE_S: number;
   export var SCANCODE_T: number;
   export var SCANCODE_U: number;
   export var SCANCODE_V: number;
   export var SCANCODE_W: number;
   export var SCANCODE_X: number;
   export var SCANCODE_Y: number;
   export var SCANCODE_Z: number;
   export var SCANCODE_1: number;
   export var SCANCODE_2: number;
   export var SCANCODE_3: number;
   export var SCANCODE_4: number;
   export var SCANCODE_5: number;
   export var SCANCODE_6: number;
   export var SCANCODE_7: number;
   export var SCANCODE_8: number;
   export var SCANCODE_9: number;
   export var SCANCODE_0: number;
   export var SCANCODE_RETURN: number;
   export var SCANCODE_ESCAPE: number;
   export var SCANCODE_BACKSPACE: number;
   export var SCANCODE_TAB: number;
   export var SCANCODE_SPACE: number;
   export var SCANCODE_MINUS: number;
   export var SCANCODE_EQUALS: number;
   export var SCANCODE_LEFTBRACKET: number;
   export var SCANCODE_RIGHTBRACKET: number;
   export var SCANCODE_BACKSLASH: number;
   export var SCANCODE_NONUSHASH: number;
   export var SCANCODE_SEMICOLON: number;
   export var SCANCODE_APOSTROPHE: number;
   export var SCANCODE_GRAVE: number;
   export var SCANCODE_COMMA: number;
   export var SCANCODE_PERIOD: number;
   export var SCANCODE_SLASH: number;
   export var SCANCODE_CAPSLOCK: number;
   export var SCANCODE_F1: number;
   export var SCANCODE_F2: number;
   export var SCANCODE_F3: number;
   export var SCANCODE_F4: number;
   export var SCANCODE_F5: number;
   export var SCANCODE_F6: number;
   export var SCANCODE_F7: number;
   export var SCANCODE_F8: number;
   export var SCANCODE_F9: number;
   export var SCANCODE_F10: number;
   export var SCANCODE_F11: number;
   export var SCANCODE_F12: number;
   export var SCANCODE_PRINTSCREEN: number;
   export var SCANCODE_SCROLLLOCK: number;
   export var SCANCODE_PAUSE: number;
   export var SCANCODE_INSERT: number;
   export var SCANCODE_HOME: number;
   export var SCANCODE_PAGEUP: number;
   export var SCANCODE_DELETE: number;
   export var SCANCODE_END: number;
   export var SCANCODE_PAGEDOWN: number;
   export var SCANCODE_RIGHT: number;
   export var SCANCODE_LEFT: number;
   export var SCANCODE_DOWN: number;
   export var SCANCODE_UP: number;
   export var SCANCODE_NUMLOCKCLEAR: number;
   export var SCANCODE_KP_DIVIDE: number;
   export var SCANCODE_KP_MULTIPLY: number;
   export var SCANCODE_KP_MINUS: number;
   export var SCANCODE_KP_PLUS: number;
   export var SCANCODE_KP_ENTER: number;
   export var SCANCODE_KP_1: number;
   export var SCANCODE_KP_2: number;
   export var SCANCODE_KP_3: number;
   export var SCANCODE_KP_4: number;
   export var SCANCODE_KP_5: number;
   export var SCANCODE_KP_6: number;
   export var SCANCODE_KP_7: number;
   export var SCANCODE_KP_8: number;
   export var SCANCODE_KP_9: number;
   export var SCANCODE_KP_0: number;
   export var SCANCODE_KP_PERIOD: number;
   export var SCANCODE_NONUSBACKSLASH: number;
   export var SCANCODE_APPLICATION: number;
   export var SCANCODE_POWER: number;
   export var SCANCODE_KP_EQUALS: number;
   export var SCANCODE_F13: number;
   export var SCANCODE_F14: number;
   export var SCANCODE_F15: number;
   export var SCANCODE_F16: number;
   export var SCANCODE_F17: number;
   export var SCANCODE_F18: number;
   export var SCANCODE_F19: number;
   export var SCANCODE_F20: number;
   export var SCANCODE_F21: number;
   export var SCANCODE_F22: number;
   export var SCANCODE_F23: number;
   export var SCANCODE_F24: number;
   export var SCANCODE_EXECUTE: number;
   export var SCANCODE_HELP: number;
   export var SCANCODE_MENU: number;
   export var SCANCODE_SELECT: number;
   export var SCANCODE_STOP: number;
   export var SCANCODE_AGAIN: number;
   export var SCANCODE_UNDO: number;
   export var SCANCODE_CUT: number;
   export var SCANCODE_COPY: number;
   export var SCANCODE_PASTE: number;
   export var SCANCODE_FIND: number;
   export var SCANCODE_MUTE: number;
   export var SCANCODE_VOLUMEUP: number;
   export var SCANCODE_VOLUMEDOWN: number;
   export var SCANCODE_KP_COMMA: number;
   export var SCANCODE_KP_EQUALSAS400: number;
   export var SCANCODE_INTERNATIONAL1: number;
   export var SCANCODE_INTERNATIONAL2: number;
   export var SCANCODE_INTERNATIONAL3: number;
   export var SCANCODE_INTERNATIONAL4: number;
   export var SCANCODE_INTERNATIONAL5: number;
   export var SCANCODE_INTERNATIONAL6: number;
   export var SCANCODE_INTERNATIONAL7: number;
   export var SCANCODE_INTERNATIONAL8: number;
   export var SCANCODE_INTERNATIONAL9: number;
   export var SCANCODE_LANG1: number;
   export var SCANCODE_LANG2: number;
   export var SCANCODE_LANG3: number;
   export var SCANCODE_LANG4: number;
   export var SCANCODE_LANG5: number;
   export var SCANCODE_LANG6: number;
   export var SCANCODE_LANG7: number;
   export var SCANCODE_LANG8: number;
   export var SCANCODE_LANG9: number;
   export var SCANCODE_ALTERASE: number;
   export var SCANCODE_SYSREQ: number;
   export var SCANCODE_CANCEL: number;
   export var SCANCODE_CLEAR: number;
   export var SCANCODE_PRIOR: number;
   export var SCANCODE_RETURN2: number;
   export var SCANCODE_SEPARATOR: number;
   export var SCANCODE_OUT: number;
   export var SCANCODE_OPER: number;
   export var SCANCODE_CLEARAGAIN: number;
   export var SCANCODE_CRSEL: number;
   export var SCANCODE_EXSEL: number;
   export var SCANCODE_KP_00: number;
   export var SCANCODE_KP_000: number;
   export var SCANCODE_THOUSANDSSEPARATOR: number;
   export var SCANCODE_DECIMALSEPARATOR: number;
   export var SCANCODE_CURRENCYUNIT: number;
   export var SCANCODE_CURRENCYSUBUNIT: number;
   export var SCANCODE_KP_LEFTPAREN: number;
   export var SCANCODE_KP_RIGHTPAREN: number;
   export var SCANCODE_KP_LEFTBRACE: number;
   export var SCANCODE_KP_RIGHTBRACE: number;
   export var SCANCODE_KP_TAB: number;
   export var SCANCODE_KP_BACKSPACE: number;
   export var SCANCODE_KP_A: number;
   export var SCANCODE_KP_B: number;
   export var SCANCODE_KP_C: number;
   export var SCANCODE_KP_D: number;
   export var SCANCODE_KP_E: number;
   export var SCANCODE_KP_F: number;
   export var SCANCODE_KP_XOR: number;
   export var SCANCODE_KP_POWER: number;
   export var SCANCODE_KP_PERCENT: number;
   export var SCANCODE_KP_LESS: number;
   export var SCANCODE_KP_GREATER: number;
   export var SCANCODE_KP_AMPERSAND: number;
   export var SCANCODE_KP_DBLAMPERSAND: number;
   export var SCANCODE_KP_VERTICALBAR: number;
   export var SCANCODE_KP_DBLVERTICALBAR: number;
   export var SCANCODE_KP_COLON: number;
   export var SCANCODE_KP_HASH: number;
   export var SCANCODE_KP_SPACE: number;
   export var SCANCODE_KP_AT: number;
   export var SCANCODE_KP_EXCLAM: number;
   export var SCANCODE_KP_MEMSTORE: number;
   export var SCANCODE_KP_MEMRECALL: number;
   export var SCANCODE_KP_MEMCLEAR: number;
   export var SCANCODE_KP_MEMADD: number;
   export var SCANCODE_KP_MEMSUBTRACT: number;
   export var SCANCODE_KP_MEMMULTIPLY: number;
   export var SCANCODE_KP_MEMDIVIDE: number;
   export var SCANCODE_KP_PLUSMINUS: number;
   export var SCANCODE_KP_CLEAR: number;
   export var SCANCODE_KP_CLEARENTRY: number;
   export var SCANCODE_KP_BINARY: number;
   export var SCANCODE_KP_OCTAL: number;
   export var SCANCODE_KP_DECIMAL: number;
   export var SCANCODE_KP_HEXADECIMAL: number;
   export var SCANCODE_LCTRL: number;
   export var SCANCODE_LSHIFT: number;
   export var SCANCODE_LALT: number;
   export var SCANCODE_LGUI: number;
   export var SCANCODE_RCTRL: number;
   export var SCANCODE_RSHIFT: number;
   export var SCANCODE_RALT: number;
   export var SCANCODE_RGUI: number;
   export var SCANCODE_MODE: number;
   export var SCANCODE_AUDIONEXT: number;
   export var SCANCODE_AUDIOPREV: number;
   export var SCANCODE_AUDIOSTOP: number;
   export var SCANCODE_AUDIOPLAY: number;
   export var SCANCODE_AUDIOMUTE: number;
   export var SCANCODE_MEDIASELECT: number;
   export var SCANCODE_WWW: number;
   export var SCANCODE_MAIL: number;
   export var SCANCODE_CALCULATOR: number;
   export var SCANCODE_COMPUTER: number;
   export var SCANCODE_AC_SEARCH: number;
   export var SCANCODE_AC_HOME: number;
   export var SCANCODE_AC_BACK: number;
   export var SCANCODE_AC_FORWARD: number;
   export var SCANCODE_AC_STOP: number;
   export var SCANCODE_AC_REFRESH: number;
   export var SCANCODE_AC_BOOKMARKS: number;
   export var SCANCODE_BRIGHTNESSDOWN: number;
   export var SCANCODE_BRIGHTNESSUP: number;
   export var SCANCODE_DISPLAYSWITCH: number;
   export var SCANCODE_KBDILLUMTOGGLE: number;
   export var SCANCODE_KBDILLUMDOWN: number;
   export var SCANCODE_KBDILLUMUP: number;
   export var SCANCODE_EJECT: number;
   export var SCANCODE_SLEEP: number;
   export var SCANCODE_APP1: number;
   export var SCANCODE_APP2: number;
   export var HAT_CENTER: number;
   export var HAT_UP: number;
   export var HAT_RIGHT: number;
   export var HAT_DOWN: number;
   export var HAT_LEFT: number;
   export var CONTROLLER_BUTTON_A: number;
   export var CONTROLLER_BUTTON_B: number;
   export var CONTROLLER_BUTTON_X: number;
   export var CONTROLLER_BUTTON_Y: number;
   export var CONTROLLER_BUTTON_BACK: number;
   export var CONTROLLER_BUTTON_GUIDE: number;
   export var CONTROLLER_BUTTON_START: number;
   export var CONTROLLER_BUTTON_LEFTSTICK: number;
   export var CONTROLLER_BUTTON_RIGHTSTICK: number;
   export var CONTROLLER_BUTTON_LEFTSHOULDER: number;
   export var CONTROLLER_BUTTON_RIGHTSHOULDER: number;
   export var CONTROLLER_BUTTON_DPAD_UP: number;
   export var CONTROLLER_BUTTON_DPAD_DOWN: number;
   export var CONTROLLER_BUTTON_DPAD_LEFT: number;
   export var CONTROLLER_BUTTON_DPAD_RIGHT: number;
   export var CONTROLLER_AXIS_LEFTX: number;
   export var CONTROLLER_AXIS_LEFTY: number;
   export var CONTROLLER_AXIS_RIGHTX: number;
   export var CONTROLLER_AXIS_RIGHTY: number;
   export var CONTROLLER_AXIS_TRIGGERLEFT: number;
   export var CONTROLLER_AXIS_TRIGGERRIGHT: number;


   export var UI_VERTEX_SIZE: number;


   export var COLOR_LUT_SIZE: number;
   export var PRIORITY_LAST: number;


   export var MSG_IDENTITY: number;
   export var MSG_CONTROLS: number;
   export var MSG_SCENELOADED: number;
   export var MSG_REQUESTPACKAGE: number;
   export var MSG_PACKAGEDATA: number;
   export var MSG_LOADSCENE: number;
   export var MSG_SCENECHECKSUMERROR: number;
   export var MSG_CREATENODE: number;
   export var MSG_NODEDELTAUPDATE: number;
   export var MSG_NODELATESTDATA: number;
   export var MSG_REMOVENODE: number;
   export var MSG_CREATECOMPONENT: number;
   export var MSG_COMPONENTDELTAUPDATE: number;
   export var MSG_COMPONENTLATESTDATA: number;
   export var MSG_REMOVECOMPONENT: number;
   export var MSG_REMOTEEVENT: number;
   export var MSG_REMOTENODEEVENT: number;
   export var MSG_PACKAGEINFO: number;
   export var CONTROLS_CONTENT_ID: number;
   export var PACKAGE_FRAGMENT_SIZE: number;


   export var LOG_RAW: number;
   export var LOG_DEBUG: number;
   export var LOG_INFO: number;
   export var LOG_WARNING: number;
   export var LOG_ERROR: number;
   export var LOG_NONE: number;
   export var SCAN_FILES: number;
   export var SCAN_DIRS: number;
   export var SCAN_HIDDEN: number;


//----------------------------------------------------
// MODULE: Container
//----------------------------------------------------


   export class RefCounted {

      // Construct. Allocate the reference count structure and set an initial self weak reference.
      constructor();

      // Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
      addRef(): void;
      // Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
      releaseRef(): void;
      // Return reference count.
      refs(): number;
      // Return weak reference count.
      weakRefs(): number;
      isObject(): boolean;

   }



//----------------------------------------------------
// MODULE: Math
//----------------------------------------------------


   export class BoundingBox {


   }

   export class Color {


   }

   export class Quaternion {


   }

   export class Rect {


   }

   export class IntRect {


   }

   export class Vector2 {


   }

   export class IntVector2 {


   }

   export class Vector3 {


   }

   export class Vector4 {


   }



//----------------------------------------------------
// MODULE: Core
//----------------------------------------------------


   export class Context extends RefCounted {

      eventSender: AObject;
      editorContext: boolean;

      // Construct.
      constructor();

      // Register a subsystem.
      registerSubsystem(subsystem: AObject): void;
      // Remove a subsystem.
      removeSubsystem(objectType: string): void;
      // Copy base class attributes to derived class.
      copyBaseAttributes(baseType: string, derivedType: string): void;
      // Return subsystem by type.
      getSubsystem(type: string): AObject;
      // Return active event sender. Null outside event handling.
      getEventSender(): AObject;
      // Return object type name from hash, or empty if unknown.
      getTypeName(objectType: string): string;
      // Get whether an Editor Context
      getEditorContext(): boolean;
      // Get whether an Editor Context
      setEditorContext(editor: boolean): void;

   }

   export class AObject extends RefCounted {

      type: string;
      baseType: string;
      typeName: string;
      context: Context;
      eventSender: AObject;
      category: string;
      typeNameStatic: string;

      // Construct.
      constructor();

      // Return type hash.
      getType(): string;
      // Return base class type hash.
      getBaseType(): string;
      // Return type name.
      getTypeName(): string;
      // Unsubscribe from a specific sender's events.
      unsubscribeFromEvents(sender: AObject): void;
      // Unsubscribe from all events.
      unsubscribeFromAllEvents(): void;
      // Return execution context.
      getContext(): Context;
      // Return subsystem by type.
      getSubsystem(type: string): AObject;
      // Return active event sender. Null outside event handling.
      getEventSender(): AObject;
      // Return whether has subscribed to any event.
      hasEventHandlers(): boolean;
      // Return object category. Categories are (optionally) registered along with the object factory. Return an empty string if the object category is not registered.
      getCategory(): string;
      isObject(): boolean;
      getTypeNameStatic(): string;
      sendEvent(eventType:string, data?:Object);
      subscribeToEvent(eventType:string, callback:(data:any)=>void);
      subscribeToEvent(sender:AObject, eventType:string, callback:(data:any)=>void);

   }



//----------------------------------------------------
// MODULE: Scene
//----------------------------------------------------


   export class Animatable extends Serializable {

      animationEnabled: boolean;
      objectAnimation: ObjectAnimation;

      // Construct.
      constructor();

      // Set animation enabled.
      setAnimationEnabled(enable: boolean): void;
      // Set object animation.
      setObjectAnimation(objectAnimation: ObjectAnimation): void;
      // Set attribute animation.
      setAttributeAnimation(name: string, attributeAnimation: ValueAnimation, wrapMode?: WrapMode, speed?: number): void;
      // Set attribute animation wrap mode.
      setAttributeAnimationWrapMode(name: string, wrapMode: WrapMode): void;
      // Set attribute animation speed.
      setAttributeAnimationSpeed(name: string, speed: number): void;
      // Return animation enabled.
      getAnimationEnabled(): boolean;
      // Return object animation.
      getObjectAnimation(): ObjectAnimation;
      // Return attribute animation.
      getAttributeAnimation(name: string): ValueAnimation;
      // Return attribute animation wrap mode.
      getAttributeAnimationWrapMode(name: string): WrapMode;
      // Return attribute animation speed.
      getAttributeAnimationSpeed(name: string): number;

   }

   export class Component extends Animatable {

      enabled: boolean;
      id: number;
      node: Node;
      scene: Scene;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Mark for attribute check on the next network update.
      markNetworkUpdate(): void;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set enabled/disabled state.
      setEnabled(enable: boolean): void;
      // Remove from the scene node. If no other shared pointer references exist, causes immediate deletion.
      remove(): void;
      // Return ID.
      getID(): number;
      // Return scene node.
      getNode(): Node;
      // Return the scene the node belongs to.
      getScene(): Scene;
      // Return whether is enabled.
      isEnabled(): boolean;
      // Return whether is effectively enabled (node is also enabled.)
      isEnabledEffective(): boolean;
      // Return component in the same scene node by type. If there are several, returns the first.
      getComponent(type: string): Component;
      // Prepare network update by comparing attributes and marking replication states dirty as necessary.
      prepareNetworkUpdate(): void;

   }

   export class Node extends Animatable {

      name: string;
      position: Vector3;
      position2D: Vector2;
      rotation: Quaternion;
      rotation2D: number;
      direction: Vector3;
      scale: Vector3;
      scale2D: Vector2;
      worldPosition: Vector3;
      worldRotation: Quaternion;
      worldRotation2D: number;
      worldDirection: Vector3;
      enabled: boolean;
      deepEnabled: boolean;
      enabledRecursive: boolean;
      parent: Node;
      id: number;
      nameHash: string;
      scene: Scene;
      up: Vector3;
      right: Vector3;
      worldPosition2D: Vector2;
      worldUp: Vector3;
      worldRight: Vector3;
      worldScale: Vector3;
      worldScale2D: Vector2;
      numComponents: number;
      numNetworkComponents: number;
      netPositionAttr: Vector3;
      numPersistentChildren: number;
      numPersistentComponents: number;
      positionSilent: Vector3;
      rotationSilent: Quaternion;
      scaleSilent: Vector3;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately recursively to child nodes and components.
      applyAttributes(): void;
      // Return whether should save default-valued attributes into XML. Always save node transforms for readability, even if identity.
      saveDefaultAttributes(): boolean;
      // Mark for attribute check on the next network update.
      markNetworkUpdate(): void;
      // Set name of the scene node. Names are not required to be unique.
      setName(name: string): void;
      // Set position in parent space. If the scene node is on the root level (is child of the scene itself), this is same as world space.
      setPosition(position: Vector3): void;
      // Set position in parent space (for Atomic2D).
      setPosition2D(position: Vector2): void;
      // Set rotation in parent space.
      setRotation(rotation: Quaternion): void;
      // Set rotation in parent space (for Atomic2D).
      setRotation2D(rotation: number): void;
      // Set forward direction in parent space. Positive Z axis equals identity rotation.
      setDirection(direction: Vector3): void;
      // Set scale in parent space.
      setScale(scale: Vector3): void;
      // Set scale in parent space (for Atomic2D).
      setScale2D(scale: Vector2): void;
      // Set position in world space.
      setWorldPosition(position: Vector3): void;
      // Set rotation in world space.
      setWorldRotation(rotation: Quaternion): void;
      // Set rotation in world space (for Atomic2D).
      setWorldRotation2D(rotation: number): void;
      // Set forward direction in world space.
      setWorldDirection(direction: Vector3): void;
      // Move the scene node in the chosen transform space.
      translate(delta: Vector3, space?: TransformSpace): void;
      // Move the scene node in the chosen transform space (for Atomic2D).
      translate2D(delta: Vector2, space?: TransformSpace): void;
      // Rotate the scene node in the chosen transform space.
      rotate(delta: Quaternion, space?: TransformSpace): void;
      // Rotate the scene node in the chosen transform space (for Atomic2D).
      rotate2D(delta: number, space?: TransformSpace): void;
      // Rotate around a point in the chosen transform space.
      rotateAround(point: Vector3, delta: Quaternion, space?: TransformSpace): void;
      // Rotate around a point in the chosen transform space (for Atomic2D).
      rotateAround2D(point: Vector2, delta: number, space?: TransformSpace): void;
      // Rotate around the X axis.
      pitch(angle: number, space?: TransformSpace): void;
      // Rotate around the Y axis.
      yaw(angle: number, space?: TransformSpace): void;
      // Rotate around the Z axis.
      roll(angle: number, space?: TransformSpace): void;
      // Look at a target position in the chosen transform space. Note that the up vector is always specified in world space. Return true if successful, or false if resulted in an illegal rotation, in which case the current rotation remains.
      lookAt(target: Vector3, up?: Vector3, space?: TransformSpace): boolean;
      // Set enabled/disabled state without recursion. Components in a disabled node become effectively disabled regardless of their own enable/disable state.
      setEnabled(enable: boolean): void;
      // Set enabled state on self and child nodes. Nodes' own enabled state is remembered (IsEnabledSelf) and can be restored.
      setDeepEnabled(enable: boolean): void;
      // Reset enabled state to the node's remembered state prior to calling SetDeepEnabled.
      resetDeepEnabled(): void;
      // Set enabled state on self and child nodes. Unlike SetDeepEnabled this does not remember the nodes' own enabled state, but overwrites it.
      setEnabledRecursive(enable: boolean): void;
      // Mark node and child nodes to need world transform recalculation. Notify listener components.
      markDirty(): void;
      // Create a child scene node (with specified ID if provided).
      createChild(name?: string, mode?: CreateMode, id?: number): Node;
      // Add a child scene node at a specific index. If index is not explicitly specified or is greater than current children size, append the new child at the end.
      addChild(node: Node, index?: number): void;
      // Remove a child scene node.
      removeChild(node: Node): void;
      // Remove all child scene nodes.
      removeAllChildren(): void;
      // Remove child scene nodes that match criteria.
      removeChildren(removeReplicated: boolean, removeLocal: boolean, recursive: boolean): void;
      // Create a component to this node (with specified ID if provided).
      createComponent(type: string, mode?: CreateMode, id?: number): Component;
      // Create a component to this node if it does not exist already.
      getOrCreateComponent(type: string, mode?: CreateMode, id?: number): Component;
      // Remove all components from this node.
      removeAllComponents(): void;
      // Remove components that match criteria.
      removeComponents(removeReplicated: boolean, removeLocal: boolean): void;
      // Clone scene node, components and child nodes. Return the clone.
      clone(mode?: CreateMode): Node;
      // Remove from the parent node. If no other shared pointer references exist, causes immediate deletion.
      remove(): void;
      // Set parent scene node. Retains the world transform.
      setParent(parent: Node): void;
      // Add listener component that is notified of node being dirtied. Can either be in the same node or another.
      addListener(component: Component): void;
      // Remove listener component.
      removeListener(component: Component): void;
      // Return ID.
      getID(): number;
      // Return name.
      getName(): string;
      // Return name hash.
      getNameHash(): string;
      // Return parent scene node.
      getParent(): Node;
      // Return scene.
      getScene(): Scene;
      // Return whether is enabled. Disables nodes effectively disable all their components.
      isEnabled(): boolean;
      // Returns the node's last own enabled state. May be different than the value returned by IsEnabled when SetDeepEnabled has been used.
      isEnabledSelf(): boolean;
      // Return position in parent space.
      getPosition(): Vector3;
      // Return position in parent space (for Atomic2D).
      getPosition2D(): Vector2;
      // Return rotation in parent space.
      getRotation(): Quaternion;
      // Return rotation in parent space (for Atomic2D).
      getRotation2D(): number;
      // Return forward direction in parent space. Positive Z axis equals identity rotation.
      getDirection(): Vector3;
      // Return up direction in parent space. Positive Y axis equals identity rotation.
      getUp(): Vector3;
      // Return right direction in parent space. Positive X axis equals identity rotation.
      getRight(): Vector3;
      // Return scale in parent space.
      getScale(): Vector3;
      // Return scale in parent space (for Atomic2D).
      getScale2D(): Vector2;
      // Return position in world space.
      getWorldPosition(): Vector3;
      // Return position in world space (for Atomic2D).
      getWorldPosition2D(): Vector2;
      // Return rotation in world space.
      getWorldRotation(): Quaternion;
      // Return rotation in world space (for Atomic2D).
      getWorldRotation2D(): number;
      // Return direction in world space.
      getWorldDirection(): Vector3;
      // Return node's up vector in world space.
      getWorldUp(): Vector3;
      // Return node's right vector in world space.
      getWorldRight(): Vector3;
      // Return scale in world space.
      getWorldScale(): Vector3;
      // Return scale in world space (for Atomic2D).
      getWorldScale2D(): Vector2;
      // Convert a local space position or rotation to world space (for Atomic2D).
      localToWorld2D(vector: Vector2): Vector2;
      // Convert a world space position or rotation to local space (for Atomic2D).
      worldToLocal2D(vector: Vector2): Vector2;
      // Return whether transform has changed and world transform needs recalculation.
      isDirty(): boolean;
      // Return number of child scene nodes.
      getNumChildren(recursive?: boolean): number;
      // Return child scene node by name.
      getChild(name: string, recursive?: boolean): Node;
      // Return number of components.
      getNumComponents(): number;
      // Return number of non-local components.
      getNumNetworkComponents(): number;
      // Return component by type. If there are several, returns the first.
      getComponent(type: string): Component;
      // Return whether has a specific component.
      hasComponent(type: string): boolean;
      // Set ID. Called by Scene.
      setID(id: number): void;
      // Set scene. Called by Scene.
      setScene(scene: Scene): void;
      // Reset scene, ID and owner. Called by Scene.
      resetScene(): void;
      // Set network position attribute.
      setNetPositionAttr(value: Vector3): void;
      // Return network position attribute.
      getNetPositionAttr(): Vector3;
      // Prepare network update by comparing attributes and marking replication states dirty as necessary.
      prepareNetworkUpdate(): void;
      // Mark node dirty in scene replication states.
      markReplicationDirty(): void;
      // Add a pre-created component.
      addComponent(component: Component, id: number, mode: CreateMode): void;
      // Calculate number of non-temporary child nodes.
      getNumPersistentChildren(): number;
      // Calculate number of non-temporary components.
      getNumPersistentComponents(): number;
      // Set position in parent space silently without marking the node & child nodes dirty. Used by animation code.
      setPositionSilent(position: Vector3): void;
      // Set position in parent space silently without marking the node & child nodes dirty. Used by animation code.
      setRotationSilent(rotation: Quaternion): void;
      // Set scale in parent space silently without marking the node & child nodes dirty. Used by animation code.
      setScaleSilent(scale: Vector3): void;
      // Set local transform silently without marking the node & child nodes dirty. Used by animation code.
      setTransformSilent(position: Vector3, rotation: Quaternion, scale: Vector3): void;
      saveXML(file:File):boolean;
      getChildrenWithName(name:string, recursive?:boolean):Node[];
      getChildrenWithComponent(componentType:string, recursive?:boolean):Node[];
      getComponents(componentType?:string, recursive?:boolean):Component[];
      getChildAtIndex(index:number):Node;
      createJSComponent(name:string, args?:{});
      getJSComponent(name:string):JSComponent;

   }

   export class ObjectAnimation extends Resource {

      // Construct.
      constructor();

      // Add attribute animation, attribute name can in following format: "attribute" or "#0/#1/attribute" or ""#0/#1/@component#1/attribute.
      addAttributeAnimation(name: string, attributeAnimation: ValueAnimation, wrapMode?: WrapMode, speed?: number): void;
      // Return attribute animation by name.
      getAttributeAnimation(name: string): ValueAnimation;
      // Return attribute animation wrap mode by name.
      getAttributeAnimationWrapMode(name: string): WrapMode;
      // Return attribute animation speed by name.
      getAttributeAnimationSpeed(name: string): number;
      // Return attribute animation info by name.
      getAttributeAnimationInfo(name: string): ValueAnimationInfo;

   }

   export class PrefabComponent extends Component {

      prefabGUID: string;
      prefabNode: Node;

      // Construct.
      constructor();

      setPrefabGUID(guid: string): void;
      getPrefabGUID(): string;
      savePrefab(): boolean;
      undoPrefab(): void;
      breakPrefab(): void;
      getPrefabNode(): Node;

   }

   export class Scene extends Node {

      updateEnabled: boolean;
      timeScale: number;
      elapsedTime: number;
      smoothingConstant: number;
      snapThreshold: number;
      asyncLoadingMs: number;
      asyncProgress: number;
      asyncLoadMode: LoadMode;
      fileName: string;
      checksum: number;
      varNamesAttr: string;

      // Construct.
      constructor();

      // Load from a binary file asynchronously. Return true if started successfully. The LOAD_RESOURCES_ONLY mode can also be used to preload resources from object prefab files.
      loadAsync(file: File, mode?: LoadMode): boolean;
      // Load from an XML file asynchronously. Return true if started successfully. The LOAD_RESOURCES_ONLY mode can also be used to preload resources from object prefab files.
      loadAsyncXML(file: File, mode?: LoadMode): boolean;
      // Stop asynchronous loading.
      stopAsyncLoading(): void;
      // Clear scene completely of either replicated, local or all nodes and components.
      clear(clearReplicated?: boolean, clearLocal?: boolean): void;
      // Enable or disable scene update.
      setUpdateEnabled(enable: boolean): void;
      // Set update time scale. 1.0 = real time (default.)
      setTimeScale(scale: number): void;
      // Set elapsed time in seconds. This can be used to prevent inaccuracy in the timer if the scene runs for a long time.
      setElapsedTime(time: number): void;
      // Set network client motion smoothing constant.
      setSmoothingConstant(constant: number): void;
      // Set network client motion smoothing snap threshold.
      setSnapThreshold(threshold: number): void;
      // Set maximum milliseconds per frame to spend on async scene loading.
      setAsyncLoadingMs(ms: number): void;
      // Clear required package files.
      clearRequiredPackageFiles(): void;
      // Register a node user variable hash reverse mapping (for editing.)
      registerVar(name: string): void;
      // Unregister a node user variable hash reverse mapping.
      unregisterVar(name: string): void;
      // Clear all registered node user variable hash reverse mappings.
      unregisterAllVars(): void;
      // Return node from the whole scene by ID, or null if not found.
      getNode(id: number): Node;
      // Return whether updates are enabled.
      isUpdateEnabled(): boolean;
      // Return whether an asynchronous loading operation is in progress.
      isAsyncLoading(): boolean;
      // Return asynchronous loading progress between 0.0 and 1.0, or 1.0 if not in progress.
      getAsyncProgress(): number;
      // Return the load mode of the current asynchronous loading operation.
      getAsyncLoadMode(): LoadMode;
      // Return source file name.
      getFileName(): string;
      // Return source file checksum.
      getChecksum(): number;
      // Return update time scale.
      getTimeScale(): number;
      // Return elapsed time in seconds.
      getElapsedTime(): number;
      // Return motion smoothing constant.
      getSmoothingConstant(): number;
      // Return motion smoothing snap threshold.
      getSnapThreshold(): number;
      // Return maximum milliseconds per frame to spend on async loading.
      getAsyncLoadingMs(): number;
      // Return a node user variable name, or empty if not registered.
      getVarName(hash: string): string;
      // Update scene. Called by HandleUpdate.
      update(timeStep: number): void;
      // Begin a threaded update. During threaded update components can choose to delay dirty processing.
      beginThreadedUpdate(): void;
      // End a threaded update. Notify components that marked themselves for delayed dirty processing.
      endThreadedUpdate(): void;
      // Add a component to the delayed dirty notify queue. Is thread-safe.
      delayedMarkedDirty(component: Component): void;
      // Return threaded update flag.
      isThreadedUpdate(): boolean;
      // Get free node ID, either non-local or local.
      getFreeNodeID(mode: CreateMode): number;
      // Get free component ID, either non-local or local.
      getFreeComponentID(mode: CreateMode): number;
      // Node added. Assign scene pointer and add to ID map.
      nodeAdded(node: Node): void;
      // Node removed. Remove from ID map.
      nodeRemoved(node: Node): void;
      // Component added. Add to ID map.
      componentAdded(component: Component): void;
      // Component removed. Remove from ID map.
      componentRemoved(component: Component): void;
      // Set node user variable reverse mappings.
      setVarNamesAttr(value: string): void;
      // Return node user variable reverse mappings.
      getVarNamesAttr(): string;
      // Prepare network update by comparing attributes and marking replication states dirty as necessary.
      prepareNetworkUpdate(): void;
      getMainCamera():Camera;

   }

   export class Serializable extends AObject {

      temporary: boolean;
      numAttributes: number;
      numNetworkAttributes: number;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Return whether should save default-valued attributes into XML. Default false.
      saveDefaultAttributes(): boolean;
      // Mark for attribute check on the next network update.
      markNetworkUpdate(): void;
      // Reset all editable attributes to their default values.
      resetToDefault(): void;
      // Remove instance's default values if they are set previously.
      removeInstanceDefault(): void;
      // Set temporary flag. Temporary objects will not be saved.
      setTemporary(enable: boolean): void;
      // Enable interception of an attribute from network updates. Intercepted attributes are sent as events instead of applying directly. This can be used to implement client side prediction.
      setInterceptNetworkUpdate(attributeName: string, enable: boolean): void;
      // Allocate network attribute state.
      allocateNetworkState(): void;
      // Return number of attributes.
      getNumAttributes(): number;
      // Return number of network replication attributes.
      getNumNetworkAttributes(): number;
      // Return whether is temporary.
      isTemporary(): boolean;
      // Return whether an attribute's network updates are being intercepted.
      getInterceptNetworkUpdate(attributeName: string): boolean;
      getAttributes():AttributeInfo[];
      getAttribute(name:string):any;
      setAttribute(name:string, value:any):void;

   }

   export class SmoothedTransform extends Component {

      targetPosition: Vector3;
      targetRotation: Quaternion;
      targetWorldPosition: Vector3;
      targetWorldRotation: Quaternion;

      // Construct.
      constructor();

      // Update smoothing.
      update(constant: number, squaredSnapThreshold: number): void;
      // Set target position in parent space.
      setTargetPosition(position: Vector3): void;
      // Set target rotation in parent space.
      setTargetRotation(rotation: Quaternion): void;
      // Set target position in world space.
      setTargetWorldPosition(position: Vector3): void;
      // Set target rotation in world space.
      setTargetWorldRotation(rotation: Quaternion): void;
      // Return target position in parent space.
      getTargetPosition(): Vector3;
      // Return target rotation in parent space.
      getTargetRotation(): Quaternion;
      // Return target position in world space.
      getTargetWorldPosition(): Vector3;
      // Return target rotation in world space.
      getTargetWorldRotation(): Quaternion;
      // Return whether smoothing is in progress.
      isInProgress(): boolean;

   }

   export class SplinePath extends Component {

      interpolationMode: InterpolationMode;
      speed: number;
      position: Vector3;
      controlledNode: Node;
      controlledIdAttr: number;

      // Construct an Empty SplinePath.
      constructor();

      // Apply Attributes to the SplinePath.
      applyAttributes(): void;
      // Draw the Debug Geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Add a Node to the SplinePath as a Control Point.
      addControlPoint(point: Node, index?: number): void;
      // Remove a Node Control Point from the SplinePath.
      removeControlPoint(point: Node): void;
      // Clear the Control Points from the SplinePath.
      clearControlPoints(): void;
      // Set the Interpolation Mode.
      setInterpolationMode(interpolationMode: InterpolationMode): void;
      // Set the movement Speed.
      setSpeed(speed: number): void;
      // Set the controlled Node's position on the SplinePath.
      setPosition(factor: number): void;
      // Set the Node to be moved along the SplinePath.
      setControlledNode(controlled: Node): void;
      // Get the Interpolation Mode.
      getInterpolationMode(): InterpolationMode;
      // Get the movement Speed.
      getSpeed(): number;
      // Get the parent Node's last position on the spline.
      getPosition(): Vector3;
      // Get the controlled Node.
      getControlledNode(): Node;
      // Get a point on the SplinePath from 0.f to 1.f where 0 is the start and 1 is the end.
      getPoint(factor: number): Vector3;
      // Move the controlled Node to the next position along the SplinePath based off the Speed value.
      move(timeStep: number): void;
      // Reset movement along the path.
      reset(): void;
      // Returns whether the movement along the SplinePath is complete.
      isFinished(): boolean;
      // Set Controlled Node ID attribute.
      setControlledIdAttr(value: number): void;
      // Get Controlled Node ID attribute.
      getControlledIdAttr(): number;

   }

   export class ValueAnimation extends Resource {

      interpolationMethod: InterpMethod;
      splineTension: number;
      valueType: VariantType;
      beginTime: number;
      endTime: number;

      // Construct.
      constructor();

      // Set interpolation method.
      setInterpolationMethod(method: InterpMethod): void;
      // Set spline tension, should be between 0.0f and 1.0f, but this is not a must.
      setSplineTension(tension: number): void;
      // Set value type.
      setValueType(valueType: VariantType): void;
      // Return animation is valid.
      isValid(): boolean;
      // Return interpolation method.
      getInterpolationMethod(): InterpMethod;
      // Return spline tension.
      getSplineTension(): number;
      // Return value type.
      getValueType(): VariantType;
      // Return begin time.
      getBeginTime(): number;
      // Return end time.
      getEndTime(): number;
      // Has event frames.
      hasEventFrames(): boolean;

   }

   export class ValueAnimationInfo extends RefCounted {

      wrapMode: WrapMode;
      speed: number;
      target: AObject;
      animation: ValueAnimation;

      // Construct without target object.
      constructor(animation: ValueAnimation, wrapMode: WrapMode, speed: number);

      // Update. Return true when the animation is finished. No-op when the target object is not defined.
      update(timeStep: number): boolean;
      // Set wrap mode.
      setWrapMode(wrapMode: WrapMode): void;
      // Set speed.
      setSpeed(speed: number): void;
      // Return target object.
      getTarget(): AObject;
      // Return animation.
      getAnimation(): ValueAnimation;
      // Return wrap mode.
      getWrapMode(): WrapMode;
      // Return speed.
      getSpeed(): number;

   }



//----------------------------------------------------
// MODULE: Graphics
//----------------------------------------------------


   export class Camera extends Component {

      nearClip: number;
      farClip: number;
      fov: number;
      orthoSize: number;
      aspectRatio: number;
      fillMode: FillMode;
      zoom: number;
      lodBias: number;
      viewMask: number;
      viewOverrideFlags: number;
      orthographic: boolean;
      autoAspectRatio: boolean;
      projectionOffset: Vector2;
      useReflection: boolean;
      useClipping: boolean;
      flipVertical: boolean;
      halfViewSize: number;
      reverseCulling: boolean;
      aspectRatioInternal: number;
      orthoSizeAttr: number;
      reflectionPlaneAttr: Vector4;
      clipPlaneAttr: Vector4;

      // Construct.
      constructor();

      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set near clip distance.
      setNearClip(nearClip: number): void;
      // Set far clip distance.
      setFarClip(farClip: number): void;
      // Set vertical field of view in degrees.
      setFov(fov: number): void;
      // Set orthographic mode view uniform size.
      setOrthoSize(orthoSize: number): void;
      // Set aspect ratio manually. Disables the auto aspect ratio -mode.
      setAspectRatio(aspectRatio: number): void;
      // Set polygon fill mode to use when rendering a scene.
      setFillMode(mode: FillMode): void;
      // Set zoom.
      setZoom(zoom: number): void;
      // Set LOD bias.
      setLodBias(bias: number): void;
      // Set view mask. Will be and'ed with object's view mask to see if the object should be rendered.
      setViewMask(mask: number): void;
      // Set view override flags.
      setViewOverrideFlags(flags: number): void;
      // Set orthographic mode enabled/disabled.
      setOrthographic(enable: boolean): void;
      // Set automatic aspect ratio based on viewport dimensions. Enabled by default.
      setAutoAspectRatio(enable: boolean): void;
      // Set projection offset. It needs to be calculated as (offset in pixels) / (viewport dimensions.)
      setProjectionOffset(offset: Vector2): void;
      // Set reflection mode.
      setUseReflection(enable: boolean): void;
      // Set whether to use a custom clip plane.
      setUseClipping(enable: boolean): void;
      // Set vertical flipping mode. Called internally by View to resolve OpenGL / Direct3D9 rendertarget sampling differences.
      setFlipVertical(enable: boolean): void;
      // Return far clip distance.
      getFarClip(): number;
      // Return near clip distance.
      getNearClip(): number;
      // Return vertical field of view in degrees.
      getFov(): number;
      // Return orthographic mode size.
      getOrthoSize(): number;
      // Return aspect ratio.
      getAspectRatio(): number;
      // Return zoom.
      getZoom(): number;
      // Return LOD bias.
      getLodBias(): number;
      // Return view mask.
      getViewMask(): number;
      // Return view override flags.
      getViewOverrideFlags(): number;
      // Return fill mode.
      getFillMode(): FillMode;
      // Return orthographic flag.
      isOrthographic(): boolean;
      // Return auto aspect ratio flag.
      getAutoAspectRatio(): boolean;
      // Return frustum near and far sizes.
      getFrustumSize(near: Vector3, far: Vector3): void;
      // Return half view size.
      getHalfViewSize(): number;
      worldToScreenPoint(worldPos: Vector3): Vector2;
      screenToWorldPoint(screenPos: Vector3): Vector3;
      // Return projection offset.
      getProjectionOffset(): Vector2;
      // Return whether is using reflection.
      getUseReflection(): boolean;
      // Return whether is using a custom clipping plane.
      getUseClipping(): boolean;
      // Return vertical flipping mode.
      getFlipVertical(): boolean;
      // Return whether to reverse culling; affected by vertical flipping and reflection.
      getReverseCulling(): boolean;
      // Return distance to position. In orthographic mode uses only Z coordinate.
      getDistance(worldPos: Vector3): number;
      // Return squared distance to position. In orthographic mode uses only Z coordinate.
      getDistanceSquared(worldPos: Vector3): number;
      // Return a scene node's LOD scaled distance.
      getLodDistance(distance: number, scale: number, bias: number): number;
      // Return a world rotation for facing a camera on certain axes based on the existing world rotation.
      getFaceCameraRotation(position: Vector3, rotation: Quaternion, mode: FaceCameraMode): Quaternion;
      // Return if projection parameters are valid for rendering and raycasting.
      isProjectionValid(): boolean;
      // Set aspect ratio without disabling the "auto aspect ratio" mode. Called internally by View.
      setAspectRatioInternal(aspectRatio: number): void;
      // Set orthographic size attribute without forcing the aspect ratio.
      setOrthoSizeAttr(orthoSize: number): void;
      // Set reflection plane attribute.
      setReflectionPlaneAttr(value: Vector4): void;
      // Return reflection plane attribute.
      getReflectionPlaneAttr(): Vector4;
      // Set clipping plane attribute.
      setClipPlaneAttr(value: Vector4): void;
      // Return clipping plane attribute.
      getClipPlaneAttr(): Vector4;

   }

   export class DebugRenderer extends Component {

      view: Camera;

      // Construct.
      constructor();

      // Set the camera viewpoint. Call before rendering, or before adding geometry if you want to use culling.
      setView(camera: Camera): void;
      // Add a scene node represented as its coordinate axes.
      addNode(node: Node, scale?: number, depthTest?: boolean): void;
      // Add a bounding box.
      addBoundingBox(box: BoundingBox, color: Color, depthTest?: boolean): void;
      // Add a cylinder
      addCylinder(position: Vector3, radius: number, height: number, color: Color, depthTest?: boolean): void;
      // Update vertex buffer and render all debug lines. The viewport and rendertarget should be set before.
      render(): void;
      // Check whether a bounding box is inside the view frustum.
      isInside(box: BoundingBox): boolean;
      // Return whether has something to render.
      hasContent(): boolean;

   }

   export class Drawable extends Component {

      updateGeometryType: UpdateGeometryType;
      numOccluderTriangles: number;
      drawDistance: number;
      shadowDistance: number;
      lodBias: number;
      viewMask: number;
      lightMask: number;
      shadowMask: number;
      zoneMask: number;
      maxLights: number;
      castShadows: boolean;
      occluder: boolean;
      occludee: boolean;
      boundingBox: BoundingBox;
      worldBoundingBox: BoundingBox;
      drawableFlags: number;
      sortValue: number;
      basePass: number;
      zone: Zone;
      distance: number;
      lodDistance: number;
      firstLight: Light;
      minZ: number;
      maxZ: number;

      // Construct.
      constructor(drawableFlags: number);

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
      getUpdateGeometryType(): UpdateGeometryType;
      // Return number of occlusion geometry triangles.
      getNumOccluderTriangles(): number;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set draw distance.
      setDrawDistance(distance: number): void;
      // Set shadow draw distance.
      setShadowDistance(distance: number): void;
      // Set LOD bias.
      setLodBias(bias: number): void;
      // Set view mask. Is and'ed with camera's view mask to see if the object should be rendered.
      setViewMask(mask: number): void;
      // Set light mask. Is and'ed with light's and zone's light mask to see if the object should be lit.
      setLightMask(mask: number): void;
      // Set shadow mask. Is and'ed with light's light mask and zone's shadow mask to see if the object should be rendered to a shadow map.
      setShadowMask(mask: number): void;
      // Set zone mask. Is and'ed with zone's zone mask to see if the object should belong to the zone.
      setZoneMask(mask: number): void;
      // Set maximum number of per-pixel lights. Default 0 is unlimited.
      setMaxLights(num: number): void;
      // Set shadowcaster flag.
      setCastShadows(enable: boolean): void;
      // Set occlusion flag.
      setOccluder(enable: boolean): void;
      // Set occludee flag.
      setOccludee(enable: boolean): void;
      // Mark for update and octree reinsertion. Update is automatically queued when the drawable's scene node moves or changes scale.
      markForUpdate(): void;
      // Return local space bounding box. May not be applicable or properly updated on all drawables.
      getBoundingBox(): BoundingBox;
      // Return world-space bounding box.
      getWorldBoundingBox(): BoundingBox;
      // Return drawable flags.
      getDrawableFlags(): number;
      // Return draw distance.
      getDrawDistance(): number;
      // Return shadow draw distance.
      getShadowDistance(): number;
      // Return LOD bias.
      getLodBias(): number;
      // Return view mask.
      getViewMask(): number;
      // Return light mask.
      getLightMask(): number;
      // Return shadow mask.
      getShadowMask(): number;
      // Return zone mask.
      getZoneMask(): number;
      // Return maximum number of per-pixel lights.
      getMaxLights(): number;
      // Return shadowcaster flag.
      getCastShadows(): boolean;
      // Return occluder flag.
      isOccluder(): boolean;
      // Return occludee flag.
      isOccludee(): boolean;
      // Set new zone. Zone assignment may optionally be temporary, meaning it needs to be re-evaluated on the next frame.
      setZone(zone: Zone, temporary?: boolean): void;
      // Set sorting value.
      setSortValue(value: number): void;
      // Set view-space depth bounds.
      setMinMaxZ(minZ: number, maxZ: number): void;
      // Mark in view without specifying a camera. Used for shadow casters.
      markInView(frameNumber: number): void;
      // Sort and limit per-pixel lights to maximum allowed. Convert extra lights into vertex lights.
      limitLights(): void;
      // Sort and limit per-vertex lights to maximum allowed.
      limitVertexLights(removeConvertedLights: boolean): void;
      // Set base pass flag for a batch.
      setBasePass(batchIndex: number): void;
      // Return current zone.
      getZone(): Zone;
      // Return whether current zone is inconclusive or dirty due to the drawable moving.
      isZoneDirty(): boolean;
      // Return distance from camera.
      getDistance(): number;
      // Return LOD scaled distance from camera.
      getLodDistance(): number;
      // Return sorting value.
      getSortValue(): number;
      // Return whether has a base pass.
      hasBasePass(batchIndex: number): boolean;
      // Return the first added per-pixel light.
      getFirstLight(): Light;
      // Return the minimum view-space depth.
      getMinZ(): number;
      // Return the maximum view-space depth.
      getMaxZ(): number;
      addLight(light: Light): void;
      addVertexLight(light: Light): void;

   }

   export class Light extends Drawable {

      lightType: LightType;
      perVertex: boolean;
      color: Color;
      specularIntensity: number;
      brightness: number;
      range: number;
      fov: number;
      aspectRatio: number;
      fadeDistance: number;
      shadowFadeDistance: number;
      shadowIntensity: number;
      shadowResolution: number;
      shadowNearFarRatio: number;
      rampTexture: Texture;
      shapeTexture: Texture;
      effectiveColor: Color;
      effectiveSpecularIntensity: number;
      numShadowSplits: number;

      // Construct.
      constructor();

      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set light type.
      setLightType(type: LightType): void;
      // Set vertex lighting mode.
      setPerVertex(enable: boolean): void;
      // Set color.
      setColor(color: Color): void;
      // Set specular intensity. Zero disables specular calculations.
      setSpecularIntensity(intensity: number): void;
      // Set light brightness multiplier. Both the color and specular intensity are multiplied with this to get final values for rendering.
      setBrightness(brightness: number): void;
      // Set range.
      setRange(range: number): void;
      // Set spotlight field of view.
      setFov(fov: number): void;
      // Set spotlight aspect ratio.
      setAspectRatio(aspectRatio: number): void;
      // Set fade out start distance.
      setFadeDistance(distance: number): void;
      // Set shadow fade out start distance. Only has effect if shadow distance is also non-zero.
      setShadowFadeDistance(distance: number): void;
      // Set shadow intensity between 0.0 - 1.0. 0.0 (the default) gives fully dark shadows.
      setShadowIntensity(intensity: number): void;
      // Set shadow resolution between 0.25 - 1.0. Determines the shadow map to use.
      setShadowResolution(resolution: number): void;
      // Set shadow camera near/far clip distance ratio.
      setShadowNearFarRatio(nearFarRatio: number): void;
      // Set range attenuation texture.
      setRampTexture(texture: Texture): void;
      // Set spotlight attenuation texture.
      setShapeTexture(texture: Texture): void;
      // Return light type.
      getLightType(): LightType;
      // Return vertex lighting mode.
      getPerVertex(): boolean;
      // Return color.
      getColor(): Color;
      // Return specular intensity.
      getSpecularIntensity(): number;
      // Return brightness multiplier.
      getBrightness(): number;
      // Return effective color, multiplied by brightness. Do not multiply the alpha so that can compare against the default black color to detect a light with no effect.
      getEffectiveColor(): Color;
      // Return effective specular intensity, multiplied by absolute value of brightness.
      getEffectiveSpecularIntensity(): number;
      // Return range.
      getRange(): number;
      // Return spotlight field of view.
      getFov(): number;
      // Return spotlight aspect ratio.
      getAspectRatio(): number;
      // Return fade start distance.
      getFadeDistance(): number;
      // Return shadow fade start distance.
      getShadowFadeDistance(): number;
      // Return shadow intensity.
      getShadowIntensity(): number;
      // Return shadow resolution.
      getShadowResolution(): number;
      // Return shadow camera near/far clip distance ratio.
      getShadowNearFarRatio(): number;
      // Return range attenuation texture.
      getRampTexture(): Texture;
      // Return spotlight attenuation texture.
      getShapeTexture(): Texture;
      // Return number of shadow map cascade splits for a directional light, considering also graphics API limitations.
      getNumShadowSplits(): number;
      // Return whether light has negative (darkening) color.
      isNegative(): boolean;
      // Return a divisor value based on intensity for calculating the sort value.
      getIntensityDivisor(attenuation?: number): number;
      getShadowCascade():Number[];
      setShadowCascade(args:Number[]);
      setShadowCascadeParameter(index:number, value:number);

   }

   export class Octree extends Component {

      numLevels: number;

      // Construct.
      constructor();

      // Set size and maximum subdivision levels. If octree is not empty, drawable objects will be temporarily moved to the root.
      setSize(box: BoundingBox, numLevels: number): void;
      // Add a drawable manually.
      addManualDrawable(drawable: Drawable): void;
      // Remove a manually added drawable.
      removeManualDrawable(drawable: Drawable): void;
      // Return subdivision levels.
      getNumLevels(): number;
      // Mark drawable object as requiring an update and a reinsertion.
      queueUpdate(drawable: Drawable): void;
      // Cancel drawable object's update.
      cancelUpdate(drawable: Drawable): void;

   }

   export class Renderer extends AObject {

      numViewports: number;
      hDRRendering: boolean;
      specularLighting: boolean;
      textureAnisotropy: number;
      textureFilterMode: TextureFilterMode;
      textureQuality: number;
      materialQuality: number;
      drawShadows: boolean;
      shadowMapSize: number;
      shadowQuality: number;
      reuseShadowMaps: boolean;
      maxShadowMaps: number;
      dynamicInstancing: boolean;
      minInstances: number;
      maxSortedInstances: number;
      maxOccluderTriangles: number;
      occlusionBufferSize: number;
      occluderSizeThreshold: number;
      mobileShadowBiasMul: number;
      mobileShadowBiasAdd: number;
      defaultRenderPath: RenderPath;
      numViews: number;
      numPrimitives: number;
      numBatches: number;
      defaultZone: Zone;
      defaultMaterial: Material;
      defaultLightRamp: Texture2D;
      defaultLightSpot: Texture2D;
      faceSelectCubeMap: TextureCube;
      indirectionCubeMap: TextureCube;
      shadowCamera: Camera;

      // Construct.
      constructor();

      // Set number of backbuffer viewports to render.
      setNumViewports(num: number): void;
      // Set a backbuffer viewport.
      setViewport(index: number, viewport: Viewport): void;
      // Set HDR rendering on/off.
      setHDRRendering(enable: boolean): void;
      // Set specular lighting on/off.
      setSpecularLighting(enable: boolean): void;
      // Set texture anisotropy.
      setTextureAnisotropy(level: number): void;
      // Set texture filtering.
      setTextureFilterMode(mode: TextureFilterMode): void;
      // Set texture quality level. See the QUALITY constants in GraphicsDefs.h.
      setTextureQuality(quality: number): void;
      // Set material quality level. See the QUALITY constants in GraphicsDefs.h.
      setMaterialQuality(quality: number): void;
      // Set shadows on/off.
      setDrawShadows(enable: boolean): void;
      // Set shadow map resolution.
      setShadowMapSize(size: number): void;
      // Set shadow quality mode. See the SHADOWQUALITY constants in GraphicsDefs.h.
      setShadowQuality(quality: number): void;
      // Set reuse of shadow maps. Default is true. If disabled, also transparent geometry can be shadowed.
      setReuseShadowMaps(enable: boolean): void;
      // Set maximum number of shadow maps created for one resolution. Only has effect if reuse of shadow maps is disabled.
      setMaxShadowMaps(shadowMaps: number): void;
      // Set dynamic instancing on/off.
      setDynamicInstancing(enable: boolean): void;
      // Set minimum number of instances required in a batch group to render as instanced.
      setMinInstances(instances: number): void;
      // Set maximum number of sorted instances per batch group. If exceeded, instances are rendered unsorted.
      setMaxSortedInstances(instances: number): void;
      // Set maximum number of occluder trianges.
      setMaxOccluderTriangles(triangles: number): void;
      // Set occluder buffer width.
      setOcclusionBufferSize(size: number): void;
      // Set required screen size (1.0 = full screen) for occluders.
      setOccluderSizeThreshold(screenSize: number): void;
      // Set shadow depth bias multiplier for mobile platforms (OpenGL ES.) No effect on desktops. Default 2.
      setMobileShadowBiasMul(mul: number): void;
      // Set shadow depth bias addition for mobile platforms (OpenGL ES.)  No effect on desktops. Default 0.0001.
      setMobileShadowBiasAdd(add: number): void;
      // Force reload of shaders.
      reloadShaders(): void;
      // Return number of backbuffer viewports.
      getNumViewports(): number;
      // Return backbuffer viewport by index.
      getViewport(index: number): Viewport;
      // Return default renderpath.
      getDefaultRenderPath(): RenderPath;
      // Return whether HDR rendering is enabled.
      getHDRRendering(): boolean;
      // Return whether specular lighting is enabled.
      getSpecularLighting(): boolean;
      // Return whether drawing shadows is enabled.
      getDrawShadows(): boolean;
      // Return texture anisotropy.
      getTextureAnisotropy(): number;
      // Return texture filtering.
      getTextureFilterMode(): TextureFilterMode;
      // Return texture quality level.
      getTextureQuality(): number;
      // Return material quality level.
      getMaterialQuality(): number;
      // Return shadow map resolution.
      getShadowMapSize(): number;
      // Return shadow quality.
      getShadowQuality(): number;
      // Return whether shadow maps are reused.
      getReuseShadowMaps(): boolean;
      // Return maximum number of shadow maps per resolution.
      getMaxShadowMaps(): number;
      // Return whether dynamic instancing is in use.
      getDynamicInstancing(): boolean;
      // Return minimum number of instances required in a batch group to render as instanced.
      getMinInstances(): number;
      // Return maximum number of sorted instances per batch group.
      getMaxSortedInstances(): number;
      // Return maximum number of occluder triangles.
      getMaxOccluderTriangles(): number;
      // Return occlusion buffer width.
      getOcclusionBufferSize(): number;
      // Return occluder screen size threshold.
      getOccluderSizeThreshold(): number;
      // Return shadow depth bias multiplier for mobile platforms.
      getMobileShadowBiasMul(): number;
      // Return shadow depth bias addition for mobile platforms.
      getMobileShadowBiasAdd(): number;
      // Return number of views rendered.
      getNumViews(): number;
      // Return number of primitives rendered.
      getNumPrimitives(): number;
      // Return number of batches rendered.
      getNumBatches(): number;
      // Return number of geometries rendered.
      getNumGeometries(allViews?: boolean): number;
      // Return number of lights rendered.
      getNumLights(allViews?: boolean): number;
      // Return number of shadow maps rendered.
      getNumShadowMaps(allViews?: boolean): number;
      // Return number of occluders rendered.
      getNumOccluders(allViews?: boolean): number;
      // Return the default zone.
      getDefaultZone(): Zone;
      // Return the default material.
      getDefaultMaterial(): Material;
      // Return the default range attenuation texture.
      getDefaultLightRamp(): Texture2D;
      // Return the default spotlight attenuation texture.
      getDefaultLightSpot(): Texture2D;
      // Return the shadowed pointlight face selection cube map.
      getFaceSelectCubeMap(): TextureCube;
      // Return the shadowed pointlight indirection cube map.
      getIndirectionCubeMap(): TextureCube;
      // Update for rendering. Called by HandleRenderUpdate().
      update(timeStep: number): void;
      // Render. Called by Engine.
      render(): void;
      // Add debug geometry to the debug renderer.
      drawDebugGeometry(depthTest: boolean): void;
      // Queue a render surface's viewports for rendering. Called by the surface, or by View.
      queueRenderSurface(renderTarget: RenderSurface): void;
      // Queue a viewport for rendering. Null surface means backbuffer.
      queueViewport(renderTarget: RenderSurface, viewport: Viewport): void;
      // Allocate a shadow map. If shadow map reuse is disabled, a different map is returned each time.
      getShadowMap(light: Light, camera: Camera, viewWidth: number, viewHeight: number): Texture2D;
      // Allocate a rendertarget or depth-stencil texture for deferred rendering or postprocessing. Should only be called during actual rendering, not before.
      getScreenBuffer(width: number, height: number, format: number, cubemap: boolean, filtered: boolean, srgb: boolean, persistentKey?: number): Texture;
      // Allocate a depth-stencil surface that does not need to be readable. Should only be called during actual rendering, not before.
      getDepthStencil(width: number, height: number): RenderSurface;
      // Allocate a temporary shadow camera and a scene node for it. Is thread-safe.
      getShadowCamera(): Camera;
      // Set cull mode while taking possible projection flipping into account.
      setCullMode(mode: CullMode, camera: Camera): void;
      // Ensure sufficient size of the instancing vertex buffer. Return true if successful.
      resizeInstancingBuffer(numInstances: number): boolean;
      // Save the screen buffer allocation status. Called by View.
      saveScreenBufferAllocations(): void;
      // Restore the screen buffer allocation status. Called by View.
      restoreScreenBufferAllocations(): void;
      // Optimize a light by scissor rectangle.
      optimizeLightByScissor(light: Light, camera: Camera): void;
      // Optimize a light by marking it to the stencil buffer and setting a stencil test.
      optimizeLightByStencil(light: Light, camera: Camera): void;
      // Return a scissor rectangle for a light.
      getLightScissor(light: Light, camera: Camera): Rect;

   }

   export class Shader extends Resource {

      timeStamp: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Return a variation with defines.
      getVariation(type: ShaderType, defines: string): ShaderVariation;
      // Return either vertex or pixel shader source code.
      getSourceCode(type: ShaderType): string;
      // Return the latest timestamp of the shader code and its includes.
      getTimeStamp(): number;

   }

   export class ShaderPrecache extends AObject {

      // Construct and begin collecting shader combinations. Load existing combinations from XML if the file exists.
      constructor(fileName: string);

      // Collect a shader combination. Called by Graphics when shaders have been set.
      storeShaders(vs: ShaderVariation, ps: ShaderVariation): void;

   }

   export class View extends AObject {

      graphics: Graphics;
      renderer: Renderer;
      scene: Scene;
      octree: Octree;
      camera: Camera;
      renderTarget: RenderSurface;
      drawDebug: boolean;

      // Construct.
      constructor();

      // Define with rendertarget and viewport. Return true if successful.
      define(renderTarget: RenderSurface, viewport: Viewport): boolean;
      // Render batches.
      render(): void;
      // Return graphics subsystem.
      getGraphics(): Graphics;
      // Return renderer subsystem.
      getRenderer(): Renderer;
      // Return scene.
      getScene(): Scene;
      // Return octree.
      getOctree(): Octree;
      // Return camera.
      getCamera(): Camera;
      // Return the rendertarget. 0 if using the backbuffer.
      getRenderTarget(): RenderSurface;
      // Return whether should draw debug geometry.
      getDrawDebug(): boolean;
      // Set global (per-frame) shader parameters. Called by Batch and internally by View.
      setGlobalShaderParameters(): void;
      // Set camera-specific shader parameters. Called by Batch and internally by View.
      setCameraShaderParameters(camera: Camera, setProjectionMatrix: boolean): void;
      // Set G-buffer offset and inverse size shader parameters. Called by Batch and internally by View.
      setGBufferShaderParameters(texSize: IntVector2, viewRect: IntRect): void;

   }

   export class Viewport extends AObject {

      scene: Scene;
      camera: Camera;
      rect: IntRect;
      renderPath: RenderPath;
      drawDebug: boolean;
      view: View;
      width: number;
      height: number;

      // Construct with a full rectangle.
      constructor(scene: Scene, camera: Camera, renderPath?: RenderPath);

      // Set scene.
      setScene(scene: Scene): void;
      // Set camera.
      setCamera(camera: Camera): void;
      // Set rectangle.
      setRect(rect: IntRect): void;
      // Set rendering path from an XML file.
      setRenderPath(file: XMLFile): void;
      // Set whether to render debug geometry. Default true.
      setDrawDebug(enable: boolean): void;
      // Return scene.
      getScene(): Scene;
      // Return camera.
      getCamera(): Camera;
      // Return the internal rendering structure. May be null if the viewport has not been rendered yet.
      getView(): View;
      // Return rectangle.
      getRect(): IntRect;
      // Return the viewport width
      getWidth(): number;
      // Return the viewport height
      getHeight(): number;
      // Return rendering path.
      getRenderPath(): RenderPath;
      // Return whether to draw debug geometry.
      getDrawDebug(): boolean;
      worldToScreenPoint(worldPos: Vector3): IntVector2;
      screenToWorldPoint(x: number, y: number, depth: number): Vector3;
      // Allocate the view structure. Called by Renderer.
      allocateView(): void;

   }

   export class Zone extends Drawable {

      boundingBox: BoundingBox;
      ambientColor: Color;
      fogColor: Color;
      fogStart: number;
      fogEnd: number;
      fogHeight: number;
      fogHeightScale: number;
      priority: number;
      heightFog: boolean;
      override: boolean;
      ambientGradient: boolean;
      zoneTexture: Texture;
      ambientStartColor: Color;
      ambientEndColor: Color;

      // Construct.
      constructor();

      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set local-space bounding box. Will be used as an oriented bounding box to test whether objects or the camera are inside.
      setBoundingBox(box: BoundingBox): void;
      // Set ambient color
      setAmbientColor(color: Color): void;
      // Set fog color.
      setFogColor(color: Color): void;
      // Set fog start distance.
      setFogStart(start: number): void;
      // Set fog end distance.
      setFogEnd(end: number): void;
      // Set fog height distance relative to the scene node's world position. Effective only in height fog mode.
      setFogHeight(height: number): void;
      // Set fog height scale. Effective only in height fog mode.
      setFogHeightScale(scale: number): void;
      // Set zone priority. If an object or camera is inside several zones, the one with highest priority is used.
      setPriority(priority: number): void;
      // Set height fog mode.
      setHeightFog(enable: boolean): void;
      // Set override mode. If camera is inside an override zone, that zone will be used for all rendered objects instead of their own zone.
      setOverride(enable: boolean): void;
      // Set ambient gradient mode. In gradient mode ambient color is interpolated from neighbor zones.
      setAmbientGradient(enable: boolean): void;
      // Set zone texture. This will be bound to the zone texture unit when rendering objects inside the zone. Note that the default shaders do not use it.
      setZoneTexture(texture: Texture): void;
      // Return zone's own ambient color, disregarding gradient mode.
      getAmbientColor(): Color;
      // Return ambient start color. Not safe to call from worker threads due to possible octree query.
      getAmbientStartColor(): Color;
      // Return ambient end color. Not safe to call from worker threads due to possible octree query.
      getAmbientEndColor(): Color;
      // Return fog color.
      getFogColor(): Color;
      // Return fog start distance.
      getFogStart(): number;
      // Return fog end distance.
      getFogEnd(): number;
      // Return fog height distance relative to the scene node's world position.
      getFogHeight(): number;
      // Return fog height scale.
      getFogHeightScale(): number;
      // Return zone priority.
      getPriority(): number;
      // Return whether height fog mode is enabled.
      getHeightFog(): boolean;
      // Return whether override mode is enabled.
      getOverride(): boolean;
      // Return whether ambient gradient mode is enabled.
      getAmbientGradient(): boolean;
      // Return zone texture.
      getZoneTexture(): Texture;
      // Check whether a point is inside.
      isInside(point: Vector3): boolean;

   }

   export class Material extends Resource {

      numTechniques: number;
      cullMode: CullMode;
      shadowCullMode: CullMode;
      fillMode: FillMode;
      scene: Scene;
      auxViewFrameNumber: number;
      occlusion: boolean;
      specular: boolean;
      shaderParameterHash: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Set number of techniques.
      setNumTechniques(num: number): void;
      // Set technique.
      setTechnique(index: number, tech: Technique, qualityLevel?: number, lodDistance?: number): void;
      setShaderParameterAnimation(name: string, animation: ValueAnimation, wrapMode?: WrapMode, speed?: number): void;
      // Set shader parameter animation wrap mode.
      setShaderParameterAnimationWrapMode(name: string, wrapMode: WrapMode): void;
      // Set shader parameter animation speed.
      setShaderParameterAnimationSpeed(name: string, speed: number): void;
      // Set texture.
      setTexture(unit: TextureUnit, texture: Texture): void;
      // Set culling mode.
      setCullMode(mode: CullMode): void;
      // Set culling mode for shadows.
      setShadowCullMode(mode: CullMode): void;
      // Set polygon fill mode. Interacts with the camera's fill mode setting so that the "least filled" mode will be used.
      setFillMode(mode: FillMode): void;
      // Associate the material with a scene to ensure that shader parameter animation happens in sync with scene update, respecting the scene time scale. If no scene is set, the global update events will be used.
      setScene(scene: Scene): void;
      // Remove shader parameter.
      removeShaderParameter(name: string): void;
      // Reset all shader pointers.
      releaseShaders(): void;
      // Clone the material.
      clone(cloneName?: string): Material;
      // Ensure that material techniques are listed in correct order.
      sortTechniques(): void;
      // Mark material for auxiliary view rendering.
      markForAuxView(frameNumber: number): void;
      // Return number of techniques.
      getNumTechniques(): number;
      // Return technique by index.
      getTechnique(index: number): Technique;
      // Return pass by technique index and pass name.
      getPass(index: number, passName: string): Pass;
      // Return texture by unit.
      getTexture(unit: TextureUnit): Texture;
      // Return shader parameter animation.
      getShaderParameterAnimation(name: string): ValueAnimation;
      // Return shader parameter animation wrap mode.
      getShaderParameterAnimationWrapMode(name: string): WrapMode;
      // Return shader parameter animation speed.
      getShaderParameterAnimationSpeed(name: string): number;
      // Return normal culling mode.
      getCullMode(): CullMode;
      // Return culling mode for shadows.
      getShadowCullMode(): CullMode;
      // Return polygon fill mode.
      getFillMode(): FillMode;
      // Return last auxiliary view rendered frame number.
      getAuxViewFrameNumber(): number;
      // Return whether should render occlusion.
      getOcclusion(): boolean;
      // Return whether should render specular.
      getSpecular(): boolean;
      // Return the scene associated with the material for shader parameter animation updates.
      getScene(): Scene;
      // Return shader parameter hash value. Used as an optimization to avoid setting shader parameters unnecessarily.
      getShaderParameterHash(): number;
      // Return name for texture unit.
      getTextureUnitName(unit: TextureUnit): string;
      static getTextureUnitName(unit:TextureUnit):string;
      getShaderParameters():ShaderParameter[];

   }

   export class RenderPath extends RefCounted {

      numRenderTargets: number;
      numCommands: number;

      // Construct.
      constructor();

      // Clone the rendering path.
      clone(): RenderPath;
      // Clear existing data and load from an XML file. Return true if successful.
      load(file: XMLFile): boolean;
      // Append data from an XML file. Return true if successful.
      append(file: XMLFile): boolean;
      // Enable/disable commands and rendertargets by tag.
      setEnabled(tag: string, active: boolean): void;
      // Toggle enabled state of commands and rendertargets by tag.
      toggleEnabled(tag: string): void;
      // Remove rendertargets by tag name.
      removeRenderTargets(tag: string): void;
      // Remove a command by index.
      removeCommand(index: number): void;
      // Remove commands by tag name.
      removeCommands(tag: string): void;
      // Return number of rendertargets.
      getNumRenderTargets(): number;
      // Return number of commands.
      getNumCommands(): number;

   }

   export class Pass extends RefCounted {

      blendMode: BlendMode;
      depthTestMode: CompareMode;
      lightingMode: PassLightingMode;
      depthWrite: boolean;
      alphaMask: boolean;
      isDesktop: boolean;
      vertexShader: string;
      pixelShader: string;
      vertexShaderDefines: string;
      pixelShaderDefines: string;
      name: string;
      index: number;
      shadersLoadedFrameNumber: number;

      // Construct.
      constructor(passName: string);

      // Set blend mode.
      setBlendMode(mode: BlendMode): void;
      // Set depth compare mode.
      setDepthTestMode(mode: CompareMode): void;
      // Set pass lighting mode, affects what shader variations will be attempted to be loaded.
      setLightingMode(mode: PassLightingMode): void;
      // Set depth write on/off.
      setDepthWrite(enable: boolean): void;
      // Set alpha masking hint. Completely opaque draw calls will be performed before alpha masked.
      setAlphaMask(enable: boolean): void;
      // Set whether requires desktop level hardware.
      setIsDesktop(enable: boolean): void;
      // Set vertex shader name.
      setVertexShader(name: string): void;
      // Set pixel shader name.
      setPixelShader(name: string): void;
      // Set vertex shader defines.
      setVertexShaderDefines(defines: string): void;
      // Set pixel shader defines.
      setPixelShaderDefines(defines: string): void;
      // Reset shader pointers.
      releaseShaders(): void;
      // Mark shaders loaded this frame.
      markShadersLoaded(frameNumber: number): void;
      // Return pass name.
      getName(): string;
      // Return pass index. This is used for optimal render-time pass queries that avoid map lookups.
      getIndex(): number;
      // Return blend mode.
      getBlendMode(): BlendMode;
      // Return depth compare mode.
      getDepthTestMode(): CompareMode;
      // Return pass lighting mode.
      getLightingMode(): PassLightingMode;
      // Return last shaders loaded frame number.
      getShadersLoadedFrameNumber(): number;
      // Return depth write mode.
      getDepthWrite(): boolean;
      // Return alpha masking hint.
      getAlphaMask(): boolean;
      // Return vertex shader name.
      getVertexShader(): string;
      // Return pixel shader name.
      getPixelShader(): string;
      // Return vertex shader defines.
      getVertexShaderDefines(): string;
      // Return pixel shader defines.
      getPixelShaderDefines(): string;

   }

   export class Technique extends Resource {

      isDesktop: boolean;
      numPasses: number;

      // Construct.
      constructor();

      // Set whether requires desktop level hardware.
      setIsDesktop(enable: boolean): void;
      // Create a new pass.
      createPass(passName: string): Pass;
      // Remove a pass.
      removePass(passName: string): void;
      // Reset shader pointers in all passes.
      releaseShaders(): void;
      // Return whether technique is supported by the current hardware.
      isSupported(): boolean;
      // Return number of passes.
      getNumPasses(): number;
      // Return a pass type index by name. Allocate new if not used yet.
      getPassIndex(passName: string): number;

   }

   export class Graphics extends AObject {

      windowTitle: string;
      windowIcon: Image;
      srgb: boolean;
      flushGPU: boolean;
      orientations: string;
      defaultTextureFilterMode: TextureFilterMode;
      textureAnisotropy: number;
      viewport: IntRect;
      blendMode: BlendMode;
      colorWrite: boolean;
      cullMode: CullMode;
      depthTest: CompareMode;
      depthWrite: boolean;
      fillMode: FillMode;
      apiName: string;
      windowPosition: IntVector2;
      width: number;
      height: number;
      multiSample: number;
      fullscreen: boolean;
      resizable: boolean;
      borderless: boolean;
      vSync: boolean;
      tripleBuffer: boolean;
      numPrimitives: number;
      numBatches: number;
      dummyColorFormat: number;
      shadowMapFormat: number;
      hiresShadowMapFormat: number;
      instancingSupport: boolean;
      lightPrepassSupport: boolean;
      deferredSupport: boolean;
      hardwareShadowSupport: boolean;
      readableDepthSupport: boolean;
      sRGBSupport: boolean;
      sRGBWriteSupport: boolean;
      desktopResolution: IntVector2;
      vertexShader: ShaderVariation;
      pixelShader: ShaderVariation;
      depthStencil: RenderSurface;
      depthConstantBias: number;
      depthSlopeScaledBias: number;
      stencilTest: boolean;
      scissorTest: boolean;
      scissorRect: IntRect;
      stencilTestMode: CompareMode;
      stencilPass: StencilOp;
      stencilFail: StencilOp;
      stencilZFail: StencilOp;
      stencilRef: number;
      stencilCompareMask: number;
      stencilWriteMask: number;
      useClipPlane: boolean;
      renderTargetDimensions: IntVector2;
      alphaFormat: number;
      luminanceFormat: number;
      luminanceAlphaFormat: number;
      rGBFormat: number;
      rGBAFormat: number;
      rGBA16Format: number;
      rGBAFloat16Format: number;
      rGBAFloat32Format: number;
      rG16Format: number;
      rGFloat16Format: number;
      rGFloat32Format: number;
      float16Format: number;
      float32Format: number;
      linearDepthFormat: number;
      depthStencilFormat: number;
      readableDepthFormat: number;
      pixelUVOffset: Vector2;
      maxBones: number;

      // Construct.
      constructor();

      // Set window title.
      setWindowTitle(windowTitle: string): void;
      // Set window icon.
      setWindowIcon(windowIcon: Image): void;
      // Set window size.
      setWindowSize(width: number, height: number): void;
      // Center window.
      centerWindow(): void;
      // Bring the window to front with focus
      raiseWindow(): void;
      // Set whether the main window uses sRGB conversion on write.
      setSRGB(enable: boolean): void;
      // Set whether to flush the GPU command buffer to prevent multiple frames being queued and uneven frame timesteps. Default off, may decrease performance if enabled.
      setFlushGPU(enable: boolean): void;
      // Set allowed screen orientations as a space-separated list of "LandscapeLeft", "LandscapeRight", "Portrait" and "PortraitUpsideDown". Affects currently only iOS platform.
      setOrientations(orientations: string): void;
      // Toggle between full screen and windowed mode. Return true if successful.
      toggleFullscreen(): boolean;
      // Close the window.
      close(): void;
      // Take a screenshot. Return true if successful.
      takeScreenShot(destImage: Image): boolean;
      // Begin frame rendering. Return true if device available and can render.
      beginFrame(): boolean;
      // End frame rendering and swap buffers.
      endFrame(): void;
      // Clear any or all of rendertarget, depth buffer and stencil buffer.
      clear(flags: number, color?: Color, depth?: number, stencil?: number): void;
      // Resolve multisampled backbuffer to a texture rendertarget. The texture's size should match the viewport size.
      resolveToTexture(destination: Texture2D, viewport: IntRect): boolean;
      // Draw indexed, instanced geometry. An instancing vertex buffer must be set.
      drawInstanced(type: PrimitiveType, indexStart: number, indexCount: number, minVertex: number, vertexCount: number, instanceCount: number): void;
      // Set shaders.
      setShaders(vs: ShaderVariation, ps: ShaderVariation): void;
      // Check whether a shader parameter exists on the currently set shaders.
      hasShaderParameter(param: string): boolean;
      // Check whether the current pixel shader uses a texture unit.
      hasTextureUnit(unit: TextureUnit): boolean;
      // Clear remembered shader parameter source group.
      clearParameterSource(group: ShaderParameterGroup): void;
      // Clear remembered shader parameter sources.
      clearParameterSources(): void;
      // Clear remembered transform shader parameter sources.
      clearTransformSources(): void;
      // Set texture.
      setTexture(index: number, texture: Texture): void;
      // Set default texture filtering mode.
      setDefaultTextureFilterMode(mode: TextureFilterMode): void;
      // Set texture anisotropy.
      setTextureAnisotropy(level: number): void;
      // Reset all rendertargets, depth-stencil surface and viewport.
      resetRenderTargets(): void;
      // Reset specific rendertarget.
      resetRenderTarget(index: number): void;
      // Reset depth-stencil surface.
      resetDepthStencil(): void;
      // Set viewport.
      setViewport(rect: IntRect): void;
      // Set blending mode.
      setBlendMode(mode: BlendMode): void;
      // Set color write on/off.
      setColorWrite(enable: boolean): void;
      // Set hardware culling mode.
      setCullMode(mode: CullMode): void;
      // Set depth bias.
      setDepthBias(constantBias: number, slopeScaledBias: number): void;
      // Set depth compare.
      setDepthTest(mode: CompareMode): void;
      // Set depth write on/off.
      setDepthWrite(enable: boolean): void;
      // Set polygon fill mode.
      setFillMode(mode: FillMode): void;
      // Set stencil test.
      setStencilTest(enable: boolean, mode?: CompareMode, pass?: StencilOp, fail?: StencilOp, zFail?: StencilOp, stencilRef?: number, compareMask?: number, writeMask?: number): void;
      // Begin dumping shader variation names to an XML file for precaching.
      beginDumpShaders(fileName: string): void;
      // End dumping shader variations names.
      endDumpShaders(): void;
      // Return whether rendering initialized.
      isInitialized(): boolean;
      // Return window title.
      getWindowTitle(): string;
      // Return graphics API name.
      getApiName(): string;
      // Return window position.
      getWindowPosition(): IntVector2;
      // Return window width.
      getWidth(): number;
      // Return window height.
      getHeight(): number;
      // Return multisample mode (1 = no multisampling.)
      getMultiSample(): number;
      // Return whether window is fullscreen.
      getFullscreen(): boolean;
      // Return whether window is resizable.
      getResizable(): boolean;
      // Return whether window is borderless.
      getBorderless(): boolean;
      // Return whether vertical sync is on.
      getVSync(): boolean;
      // Return whether triple buffering is enabled.
      getTripleBuffer(): boolean;
      // Return whether the main window is using sRGB conversion on write.
      getSRGB(): boolean;
      // Return whether the GPU command buffer is flushed each frame.
      getFlushGPU(): boolean;
      // Return allowed screen orientations.
      getOrientations(): string;
      // Return whether Direct3D device is lost, and can not yet render. This happens during fullscreen resolution switching.
      isDeviceLost(): boolean;
      // Return number of primitives drawn this frame.
      getNumPrimitives(): number;
      // Return number of batches drawn this frame.
      getNumBatches(): number;
      // Return dummy color texture format for shadow maps. Is "NULL" (consume no video memory) if supported.
      getDummyColorFormat(): number;
      // Return shadow map depth texture format, or 0 if not supported.
      getShadowMapFormat(): number;
      // Return 24-bit shadow map depth texture format, or 0 if not supported.
      getHiresShadowMapFormat(): number;
      // Return whether hardware instancing is supported..
      getInstancingSupport(): boolean;
      // Return whether light pre-pass rendering is supported.
      getLightPrepassSupport(): boolean;
      // Return whether deferred rendering is supported.
      getDeferredSupport(): boolean;
      // Return whether shadow map depth compare is done in hardware.
      getHardwareShadowSupport(): boolean;
      // Return whether a readable hardware depth format is available.
      getReadableDepthSupport(): boolean;
      // Return whether sRGB conversion on texture sampling is supported.
      getSRGBSupport(): boolean;
      // Return whether sRGB conversion on rendertarget writing is supported.
      getSRGBWriteSupport(): boolean;
      // Return the desktop resolution.
      getDesktopResolution(): IntVector2;
      // Return a shader variation by name and defines.
      getShader(type: ShaderType, name: string, defines?: string): ShaderVariation;
      // Return current vertex shader.
      getVertexShader(): ShaderVariation;
      // Return current pixel shader.
      getPixelShader(): ShaderVariation;
      // Return texture unit index by name.
      getTextureUnit(name: string): TextureUnit;
      // Return texture unit name by index.
      getTextureUnitName(unit: TextureUnit): string;
      // Return current texture by texture unit index.
      getTexture(index: number): Texture;
      // Return default texture filtering mode.
      getDefaultTextureFilterMode(): TextureFilterMode;
      // Return current rendertarget by index.
      getRenderTarget(index: number): RenderSurface;
      // Return current depth-stencil surface.
      getDepthStencil(): RenderSurface;
      // Return the viewport coordinates.
      getViewport(): IntRect;
      // Return texture anisotropy.
      getTextureAnisotropy(): number;
      // Return blending mode.
      getBlendMode(): BlendMode;
      // Return whether color write is enabled.
      getColorWrite(): boolean;
      // Return hardware culling mode.
      getCullMode(): CullMode;
      // Return depth constant bias.
      getDepthConstantBias(): number;
      // Return depth slope scaled bias.
      getDepthSlopeScaledBias(): number;
      // Return depth compare mode.
      getDepthTest(): CompareMode;
      // Return whether depth write is enabled.
      getDepthWrite(): boolean;
      // Return polygon fill mode.
      getFillMode(): FillMode;
      // Return whether stencil test is enabled.
      getStencilTest(): boolean;
      // Return whether scissor test is enabled.
      getScissorTest(): boolean;
      // Return scissor rectangle coordinates.
      getScissorRect(): IntRect;
      // Return stencil compare mode.
      getStencilTestMode(): CompareMode;
      // Return stencil operation to do if stencil test passes.
      getStencilPass(): StencilOp;
      // Return stencil operation to do if stencil test fails.
      getStencilFail(): StencilOp;
      // Return stencil operation to do if depth compare fails.
      getStencilZFail(): StencilOp;
      // Return stencil reference value.
      getStencilRef(): number;
      // Return stencil compare bitmask.
      getStencilCompareMask(): number;
      // Return stencil write bitmask.
      getStencilWriteMask(): number;
      // Return whether a custom clipping plane is in use.
      getUseClipPlane(): boolean;
      // Return rendertarget width and height.
      getRenderTargetDimensions(): IntVector2;
      // Window was resized through user interaction. Called by Input subsystem.
      windowResized(): void;
      // Window was moved through user interaction. Called by Input subsystem.
      windowMoved(): void;
      // Maximize the Window.
      maximize(): void;
      // Minimize the Window.
      minimize(): void;
      // Clean up too large scratch buffers.
      cleanupScratchBuffers(): void;
      // Clean up shader programs when a shader variation is released or destroyed.
      cleanupShaderPrograms(variation: ShaderVariation): void;
      // Return the API-specific alpha texture format.
      getAlphaFormat(): number;
      // Return the API-specific luminance texture format.
      getLuminanceFormat(): number;
      // Return the API-specific luminance alpha texture format.
      getLuminanceAlphaFormat(): number;
      // Return the API-specific RGB texture format.
      getRGBFormat(): number;
      // Return the API-specific RGBA texture format.
      getRGBAFormat(): number;
      // Return the API-specific RGBA 16-bit texture format.
      getRGBA16Format(): number;
      // Return the API-specific RGBA 16-bit float texture format.
      getRGBAFloat16Format(): number;
      // Return the API-specific RGBA 32-bit float texture format.
      getRGBAFloat32Format(): number;
      // Return the API-specific RG 16-bit texture format.
      getRG16Format(): number;
      // Return the API-specific RG 16-bit float texture format.
      getRGFloat16Format(): number;
      // Return the API-specific RG 32-bit float texture format.
      getRGFloat32Format(): number;
      // Return the API-specific single channel 16-bit float texture format.
      getFloat16Format(): number;
      // Return the API-specific single channel 32-bit float texture format.
      getFloat32Format(): number;
      // Return the API-specific linear depth texture format.
      getLinearDepthFormat(): number;
      // Return the API-specific hardware depth-stencil texture format.
      getDepthStencilFormat(): number;
      // Return the API-specific readable hardware depth format, or 0 if not supported.
      getReadableDepthFormat(): number;
      // Return UV offset required for pixel perfect rendering.
      getPixelUVOffset(): Vector2;
      // Return maximum number of supported bones for skinning.
      getMaxBones(): number;

   }

   export class Texture extends Resource {

      numLevels: number;
      filterMode: TextureFilterMode;
      shadowCompare: boolean;
      borderColor: Color;
      srgb: boolean;
      backupTexture: Texture;
      format: number;
      levels: number;
      width: number;
      height: number;
      depth: number;
      usage: TextureUsage;
      components: number;
      parameters: XMLFile;

      // Construct.
      constructor();

      // Set number of requested mip levels. Needs to be called before setting size.
      setNumLevels(levels: number): void;
      // Set filtering mode.
      setFilterMode(filter: TextureFilterMode): void;
      // Set addressing mode by texture coordinate.
      setAddressMode(coord: TextureCoordinate, address: TextureAddressMode): void;
      // Set shadow compare mode. No-op on D3D9.
      setShadowCompare(enable: boolean): void;
      // Set border color for border addressing mode.
      setBorderColor(color: Color): void;
      // Set sRGB sampling and writing mode.
      setSRGB(enable: boolean): void;
      // Set backup texture to use when rendering to this texture.
      setBackupTexture(texture: Texture): void;
      // Set mip levels to skip on a quality setting when loading. Ensures higher quality levels do not skip more.
      setMipsToSkip(quality: number, mips: number): void;
      // Return texture format.
      getFormat(): number;
      // Return whether the texture format is compressed.
      isCompressed(): boolean;
      // Return number of mip levels.
      getLevels(): number;
      // Return width.
      getWidth(): number;
      // Return height.
      getHeight(): number;
      // Return height.
      getDepth(): number;
      // Return filtering mode.
      getFilterMode(): TextureFilterMode;
      // Return addressing mode by texture coordinate.
      getAddressMode(coord: TextureCoordinate): TextureAddressMode;
      // Return whether shadow compare is enabled. Always false on D3D9.
      getShadowCompare(): boolean;
      // Return border color.
      getBorderColor(): Color;
      // Return whether is using sRGB sampling and writing.
      getSRGB(): boolean;
      // Return backup texture.
      getBackupTexture(): Texture;
      // Return mip levels to skip on a quality setting when loading.
      getMipsToSkip(quality: number): number;
      // Return mip level width, or 0 if level does not exist.
      getLevelWidth(level: number): number;
      // Return mip level width, or 0 if level does not exist.
      getLevelHeight(level: number): number;
      // Return mip level depth, or 0 if level does not exist.
      getLevelDepth(level: number): number;
      // Return texture usage type.
      getUsage(): TextureUsage;
      // Return data size in bytes for a pixel or block row.
      getRowDataSize(width: number): number;
      // Return number of image components required to receive pixel data from GetData(), or 0 for compressed images.
      getComponents(): number;
      // Set additional parameters from an XML file.
      setParameters(xml: XMLFile): void;

   }

   export class Texture2D extends Texture {

      renderSurface: RenderSurface;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Release default pool resources.
      onDeviceLost(): void;
      // Recreate default pool resources.
      onDeviceReset(): void;
      // Release texture.
      release(): void;
      // Set size, format and usage. Zero size will follow application window size. Return true if successful.
      setSize(width: number, height: number, format: number, usage?: TextureUsage): boolean;
      // Return render surface.
      getRenderSurface(): RenderSurface;

   }

   export class Texture3D extends Texture {

      renderSurface: RenderSurface;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Release default pool resources.
      onDeviceLost(): void;
      // Recreate default pool resources.
      onDeviceReset(): void;
      // Release texture.
      release(): void;
      // Set size, format and usage. Zero size will follow application window size. Return true if successful.
      setSize(width: number, height: number, depth: number, format: number, usage?: TextureUsage): boolean;
      // Return render surface.
      getRenderSurface(): RenderSurface;

   }

   export class TextureCube extends Texture {

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Release default pool resources.
      onDeviceLost(): void;
      // ReCreate default pool resources.
      onDeviceReset(): void;
      // Release texture.
      release(): void;
      // Set size, format and usage. Return true if successful.
      setSize(size: number, format: number, usage?: TextureUsage): boolean;
      // Return render surface for one face.
      getRenderSurface(face: CubeMapFace): RenderSurface;

   }

   export class RenderSurface extends RefCounted {

      numViewports: number;
      updateMode: RenderSurfaceUpdateMode;
      linkedRenderTarget: RenderSurface;
      linkedDepthStencil: RenderSurface;
      parentTexture: Texture;
      width: number;
      height: number;
      usage: TextureUsage;

      // Construct with parent texture.
      constructor(parentTexture: Texture);

      // Set number of viewports.
      setNumViewports(num: number): void;
      // Set viewport.
      setViewport(index: number, viewport: Viewport): void;
      // Set viewport update mode. Default is to update when visible.
      setUpdateMode(mode: RenderSurfaceUpdateMode): void;
      // Set linked color rendertarget.
      setLinkedRenderTarget(renderTarget: RenderSurface): void;
      // Set linked depth-stencil surface.
      setLinkedDepthStencil(depthStencil: RenderSurface): void;
      // Queue manual update of the viewport(s).
      queueUpdate(): void;
      // Release surface.
      release(): void;
      // Return parent texture.
      getParentTexture(): Texture;
      // Return width.
      getWidth(): number;
      // Return height.
      getHeight(): number;
      // Return usage.
      getUsage(): TextureUsage;
      // Return number of viewports.
      getNumViewports(): number;
      // Return viewport by index.
      getViewport(index: number): Viewport;
      // Return viewport update mode.
      getUpdateMode(): RenderSurfaceUpdateMode;
      // Return linked color rendertarget.
      getLinkedRenderTarget(): RenderSurface;
      // Return linked depth-stencil surface.
      getLinkedDepthStencil(): RenderSurface;
      // Clear update flag. Called by Renderer.
      wasUpdated(): void;

   }

   export class ShaderVariation extends RefCounted {

      name: string;
      defines: string;
      owner: Shader;
      shaderType: ShaderType;
      fullName: string;
      compilerOutput: string;

      // Construct.
      constructor(owner: Shader, type: ShaderType);

      // Release the shader.
      release(): void;
      // Compile the shader. Return true if successful.
      create(): boolean;
      // Set name.
      setName(name: string): void;
      // Set defines.
      setDefines(defines: string): void;
      // Return the owner resource.
      getOwner(): Shader;
      // Return shader type.
      getShaderType(): ShaderType;
      // Return shader name.
      getName(): string;
      // Return defines.
      getDefines(): string;
      // Return full shader name.
      getFullName(): string;
      // Return compile error/warning string.
      getCompilerOutput(): string;
      // Return whether uses a parameter.
      hasParameter(param: string): boolean;
      // Return whether uses a texture unit (only for pixel shaders.)
      hasTextureUnit(unit: TextureUnit): boolean;

   }



//----------------------------------------------------
// MODULE: Atomic3D
//----------------------------------------------------


   export class AnimatedModel extends StaticModel {

      updateGeometryType: UpdateGeometryType;
      animationLodBias: number;
      updateInvisible: boolean;
      numAnimationStates: number;
      numMorphs: number;
      boneCreationEnabled: boolean;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
      getUpdateGeometryType(): UpdateGeometryType;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set model.
      setModel(model: Model, createBones?: boolean): void;
      // Add an animation.
      addAnimationState(animation: Animation): AnimationState;
      // Remove all animations.
      removeAllAnimationStates(): void;
      // Set animation LOD bias.
      setAnimationLodBias(bias: number): void;
      // Set whether to update animation and the bounding box when not visible. Recommended to enable for physically controlled models like ragdolls.
      setUpdateInvisible(enable: boolean): void;
      // Reset all vertex morphs to zero.
      resetMorphWeights(): void;
      // Return number of animation states.
      getNumAnimationStates(): number;
      // Return animation LOD bias.
      getAnimationLodBias(): number;
      // Return whether to update animation when not visible.
      getUpdateInvisible(): boolean;
      // Return number of vertex morphs.
      getNumMorphs(): number;
      // Return whether is the master (first) animated model.
      isMaster(): boolean;
      // Globally enable/disable bone creation, useful for when in the editor
      setBoneCreationEnabled(enabled: boolean): void;

   }

   export class Animation extends Resource {

      animationName: string;
      length: number;
      numTriggers: number;
      animationNameHash: string;
      numTracks: number;

      // Construct.
      constructor();

      // Set animation name.
      setAnimationName(name: string): void;
      // Set animation length.
      setLength(length: number): void;
      // Remove a trigger point by index.
      removeTrigger(index: number): void;
      // Remove all trigger points.
      removeAllTriggers(): void;
      // Resize trigger point vector.
      setNumTriggers(num: number): void;
      // Return animation name.
      getAnimationName(): string;
      // Return animation name hash.
      getAnimationNameHash(): string;
      // Return animation length.
      getLength(): number;
      // Return number of animation tracks.
      getNumTracks(): number;
      // Return number of animation trigger points.
      getNumTriggers(): number;

   }

   export class AnimationController extends Component {

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Update the animations. Is called from HandleScenePostUpdate().
      update(timeStep: number): void;
      // Play an animation and set full target weight. Name must be the full resource name. Return true on success.
      play(name: string, layer: number, looped: boolean, fadeInTime?: number): boolean;
      // Play an animation, set full target weight and fade out all other animations on the same layer. Name must be the full resource name. Return true on success.
      playExclusive(name: string, layer: number, looped: boolean, fadeTime?: number): boolean;
      // Stop an animation. Zero fadetime is instant. Return true on success.
      stop(name: string, fadeOutTime?: number): boolean;
      // Stop all animations on a specific layer. Zero fadetime is instant.
      stopLayer(layer: number, fadeOutTime?: number): void;
      // Stop all animations. Zero fadetime is instant.
      stopAll(fadeTime?: number): void;
      // Fade animation to target weight. Return true on success.
      fade(name: string, targetWeight: number, fadeTime: number): boolean;
      // Fade other animations on the same layer to target weight. Return true on success.
      fadeOthers(name: string, targetWeight: number, fadeTime: number): boolean;
      // Set animation blending layer priority. Return true on success.
      setLayer(name: string, layer: number): boolean;
      // Set animation start bone. Return true on success.
      setStartBone(name: string, startBoneName: string): boolean;
      // Set animation time position. Return true on success.
      setTime(name: string, time: number): boolean;
      // Set animation weight. Return true on success.
      setWeight(name: string, weight: number): boolean;
      // Set animation looping. Return true on success.
      setLooped(name: string, enable: boolean): boolean;
      // Set animation speed. Return true on success.
      setSpeed(name: string, speed: number): boolean;
      // Set animation autofade at end (non-looped animations only.) Zero time disables. Return true on success.
      setAutoFade(name: string, fadeOutTime: number): boolean;
      // Return whether an animation is active. Note that non-looping animations that are being clamped at the end also return true.
      isPlaying(name: string): boolean;
      // Return whether an animation is fading in.
      isFadingIn(name: string): boolean;
      // Return whether an animation is fading out.
      isFadingOut(name: string): boolean;
      // Return whether an animation is at its end. Will return false if the animation is not active at all.
      isAtEnd(name: string): boolean;
      // Return animation blending layer.
      getLayer(name: string): number;
      // Return animation start bone name, or empty string if no such animation.
      getStartBoneName(name: string): string;
      // Return animation time position.
      getTime(name: string): number;
      // Return animation weight.
      getWeight(name: string): number;
      // Return animation looping.
      isLooped(name: string): boolean;
      // Return animation length.
      getLength(name: string): number;
      // Return animation speed.
      getSpeed(name: string): number;
      // Return animation fade target weight.
      getFadeTarget(name: string): number;
      // Return animation fade time.
      getFadeTime(name: string): number;
      // Return animation autofade time.
      getAutoFade(name: string): number;
      addAnimationResource(animation: Animation): void;
      removeAnimationResource(animation: Animation): void;
      clearAnimationResources(): void;

   }

   export class BillboardSet extends Drawable {

      updateGeometryType: UpdateGeometryType;
      material: Material;
      numBillboards: number;
      relative: boolean;
      scaled: boolean;
      sorted: boolean;
      faceCameraMode: FaceCameraMode;
      animationLodBias: number;

      // Construct.
      constructor();

      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
      getUpdateGeometryType(): UpdateGeometryType;
      // Set material.
      setMaterial(material: Material): void;
      // Set number of billboards.
      setNumBillboards(num: number): void;
      // Set whether billboards are relative to the scene node. Default true.
      setRelative(enable: boolean): void;
      // Set whether scene node scale affects billboards' size. Default true.
      setScaled(enable: boolean): void;
      // Set whether billboards are sorted by distance. Default false.
      setSorted(enable: boolean): void;
      // Set how the billboards should rotate in relation to the camera. Default is to follow camera rotation on all axes (FC_ROTATE_XYZ.)
      setFaceCameraMode(mode: FaceCameraMode): void;
      // Set animation LOD bias.
      setAnimationLodBias(bias: number): void;
      // Mark for bounding box and vertex buffer update. Call after modifying the billboards.
      commit(): void;
      // Return material.
      getMaterial(): Material;
      // Return number of billboards.
      getNumBillboards(): number;
      // Return whether billboards are relative to the scene node.
      isRelative(): boolean;
      // Return whether scene node scale affects billboards' size.
      isScaled(): boolean;
      // Return whether billboards are sorted.
      isSorted(): boolean;
      // Return how the billboards rotate in relation to the camera.
      getFaceCameraMode(): FaceCameraMode;
      // Return animation LOD bias.
      getAnimationLodBias(): number;

   }

   export class CustomGeometry extends Drawable {

      numOccluderTriangles: number;
      numGeometries: number;
      dynamic: boolean;

      // Construct.
      constructor();

      // Return number of occlusion geometry triangles.
      getNumOccluderTriangles(): number;
      // Clear all geometries.
      clear(): void;
      // Set number of geometries.
      setNumGeometries(num: number): void;
      // Set vertex buffer dynamic mode. A dynamic buffer should be faster to update frequently. Effective at the next Commit() call.
      setDynamic(enable: boolean): void;
      // Begin defining a geometry. Clears existing vertices in that index.
      beginGeometry(index: number, type: PrimitiveType): void;
      // Define a vertex position. This begins a new vertex.
      defineVertex(position: Vector3): void;
      // Define a vertex normal.
      defineNormal(normal: Vector3): void;
      // Define a vertex color.
      defineColor(color: Color): void;
      // Define a vertex UV coordinate.
      defineTexCoord(texCoord: Vector2): void;
      // Define a vertex tangent.
      defineTangent(tangent: Vector4): void;
      // Set the primitive type, number of vertices and elements in a geometry, after which the vertices can be edited with GetVertex(). An alternative to BeginGeometry() / DefineVertex().
      defineGeometry(index: number, type: PrimitiveType, numVertices: number, hasNormals: boolean, hasColors: boolean, hasTexCoords: boolean, hasTangents: boolean): void;
      // Update vertex buffer and calculate the bounding box. Call after finishing defining geometry.
      commit(): void;
      // Return number of geometries.
      getNumGeometries(): number;
      // Return number of vertices in a geometry.
      getNumVertices(index: number): number;
      // Return whether vertex buffer dynamic mode is enabled.
      isDynamic(): boolean;
      // Return material by geometry index.
      getMaterial(index?: number): Material;

   }

   export class DecalSet extends Drawable {

      updateGeometryType: UpdateGeometryType;
      material: Material;
      maxVertices: number;
      maxIndices: number;
      numDecals: number;
      numVertices: number;
      numIndices: number;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
      getUpdateGeometryType(): UpdateGeometryType;
      // Set material. The material should use a small negative depth bias to avoid Z-fighting.
      setMaterial(material: Material): void;
      // Set maximum number of decal vertices.
      setMaxVertices(num: number): void;
      // Set maximum number of decal vertex indices.
      setMaxIndices(num: number): void;
      // Add a decal at world coordinates, using a target drawable's geometry for reference. If the decal needs to move with the target, the decal component should be created to the target's node. Return true if successful.
      addDecal(target: Drawable, worldPosition: Vector3, worldRotation: Quaternion, size: number, aspectRatio: number, depth: number, topLeftUV: Vector2, bottomRightUV: Vector2, timeToLive?: number, normalCutoff?: number, subGeometry?: number): boolean;
      // Remove n oldest decals.
      removeDecals(num: number): void;
      // Remove all decals.
      removeAllDecals(): void;
      // Return material.
      getMaterial(): Material;
      // Return number of decals.
      getNumDecals(): number;
      // Retur number of vertices in the decals.
      getNumVertices(): number;
      // Retur number of vertex indices in the decals.
      getNumIndices(): number;
      // Return maximum number of decal vertices.
      getMaxVertices(): number;
      // Return maximum number of decal vertex indices.
      getMaxIndices(): number;

   }

   export class Model extends Resource {

      boundingBox: BoundingBox;
      numGeometries: number;
      numMorphs: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Set local-space bounding box.
      setBoundingBox(box: BoundingBox): void;
      // Set number of geometries.
      setNumGeometries(num: number): void;
      // Set number of LOD levels in a geometry.
      setNumGeometryLodLevels(index: number, num: number): boolean;
      // Set geometry center.
      setGeometryCenter(index: number, center: Vector3): boolean;
      // Clone the model. The geometry data is deep-copied and can be modified in the clone without affecting the original.
      clone(cloneName?: string): Model;
      // Return bounding box.
      getBoundingBox(): BoundingBox;
      // Return number of geometries.
      getNumGeometries(): number;
      // Return number of LOD levels in geometry.
      getNumGeometryLodLevels(index: number): number;
      // Return geometry center by index.
      getGeometryCenter(index: number): Vector3;
      // Return number of vertex morphs.
      getNumMorphs(): number;
      // Return vertex buffer morph range start.
      getMorphRangeStart(bufferIndex: number): number;
      // Return vertex buffer morph range vertex count.
      getMorphRangeCount(bufferIndex: number): number;

   }

   export class ParticleEffect extends Resource {

      material: Material;
      numParticles: number;
      updateInvisible: boolean;
      relative: boolean;
      scaled: boolean;
      sorted: boolean;
      animationLodBias: number;
      emitterType: EmitterType;
      emitterSize: Vector3;
      minDirection: Vector3;
      maxDirection: Vector3;
      constantForce: Vector3;
      dampingForce: number;
      activeTime: number;
      inactiveTime: number;
      minEmissionRate: number;
      maxEmissionRate: number;
      minParticleSize: Vector2;
      maxParticleSize: Vector2;
      minTimeToLive: number;
      maxTimeToLive: number;
      minVelocity: number;
      maxVelocity: number;
      minRotation: number;
      maxRotation: number;
      minRotationSpeed: number;
      maxRotationSpeed: number;
      sizeAdd: number;
      sizeMul: number;
      numColorFrames: number;
      numTextureFrames: number;
      randomDirection: Vector3;
      randomSize: Vector2;
      randomVelocity: number;
      randomTimeToLive: number;
      randomRotationSpeed: number;
      randomRotation: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Set material.
      setMaterial(material: Material): void;
      // Set maximum number of particles.
      setNumParticles(num: number): void;
      // Set whether to update when particles are not visible.
      setUpdateInvisible(enable: boolean): void;
      // Set whether billboards are relative to the scene node. Default true.
      setRelative(enable: boolean): void;
      // Set scaled.
      setScaled(enable: boolean): void;
      // Set sorted.
      setSorted(enable: boolean): void;
      // Set animation LOD bias.
      setAnimationLodBias(lodBias: number): void;
      // Set emitter type.
      setEmitterType(type: EmitterType): void;
      // Set emitter size.
      setEmitterSize(size: Vector3): void;
      // Set negative direction limit.
      setMinDirection(direction: Vector3): void;
      // Set positive direction limit.
      setMaxDirection(direction: Vector3): void;
      // Set constant force acting on particles.
      setConstantForce(force: Vector3): void;
      // Set particle velocity damping force.
      setDampingForce(force: number): void;
      // Set emission active period length (0 = infinite.)
      setActiveTime(time: number): void;
      // Set emission inactive period length (0 = infinite.)
      setInactiveTime(time: number): void;
      // Set minimum emission rate.
      setMinEmissionRate(rate: number): void;
      // Set maximum emission rate.
      setMaxEmissionRate(rate: number): void;
      // Set particle minimum size.
      setMinParticleSize(size: Vector2): void;
      // Set particle maximum size.
      setMaxParticleSize(size: Vector2): void;
      // Set particle minimum time to live.
      setMinTimeToLive(time: number): void;
      // Set particle maximum time to live.
      setMaxTimeToLive(time: number): void;
      // Set particle minimum velocity.
      setMinVelocity(velocity: number): void;
      // Set particle maximum velocity.
      setMaxVelocity(velocity: number): void;
      // Set particle minimum rotation.
      setMinRotation(rotation: number): void;
      // Set particle maximum rotation.
      setMaxRotation(rotation: number): void;
      // Set particle minimum rotation speed.
      setMinRotationSpeed(speed: number): void;
      // Set particle maximum rotation speed.
      setMaxRotationSpeed(speed: number): void;
      // Set particle size additive modifier.
      setSizeAdd(sizeAdd: number): void;
      // Set particle size multiplicative modifier.
      setSizeMul(sizeMul: number): void;
      // Add a color frame sorted in the correct position based on time.
      addColorTime(color: Color, time: number): void;
      // Remove color frame at index
      removeColorFrame(index: number): void;
      // Set number of color frames.
      setNumColorFrames(number: number): void;
      // Sort the list of color frames based on time.
      sortColorFrames(): void;
      // Add a texture frame sorted in the correct position based on time.
      addTextureTime(uv: Rect, time: number): void;
      // Remove texture frame at index
      removeTextureFrame(index: number): void;
      // Set number of texture frames.
      setNumTextureFrames(number: number): void;
      // Sort the list of texture frames based on time.
      sortTextureFrames(): void;
      // Return material.
      getMaterial(): Material;
      // Return maximum number of particles.
      getNumParticles(): number;
      // Return whether to update when particles are not visible.
      getUpdateInvisible(): boolean;
      // Return whether billboards are relative to the scene node.
      isRelative(): boolean;
      // Return whether scene node scale affects billboards' size.
      isScaled(): boolean;
      // Return whether billboards are sorted.
      isSorted(): boolean;
      // Return animation Lod bias.
      getAnimationLodBias(): number;
      // Return emitter type.
      getEmitterType(): EmitterType;
      // Return emitter size.
      getEmitterSize(): Vector3;
      // Return negative direction limit.
      getMinDirection(): Vector3;
      // Return positive direction limit.
      getMaxDirection(): Vector3;
      // Return constant force acting on particles.
      getConstantForce(): Vector3;
      // Return particle velocity damping force.
      getDampingForce(): number;
      // Return emission active period length (0 = infinite.)
      getActiveTime(): number;
      // Return emission inactive period length (0 = infinite.)
      getInactiveTime(): number;
      // Return minimum emission rate.
      getMinEmissionRate(): number;
      // Return maximum emission rate.
      getMaxEmissionRate(): number;
      // Return particle minimum size.
      getMinParticleSize(): Vector2;
      // Return particle maximum size.
      getMaxParticleSize(): Vector2;
      // Return particle minimum time to live.
      getMinTimeToLive(): number;
      // Return particle maximum time to live.
      getMaxTimeToLive(): number;
      // Return particle minimum velocity.
      getMinVelocity(): number;
      // Return particle maximum velocity.
      getMaxVelocity(): number;
      // Return particle minimum rotation.
      getMinRotation(): number;
      // Return particle maximum rotation.
      getMaxRotation(): number;
      // Return particle minimum rotation speed.
      getMinRotationSpeed(): number;
      // Return particle maximum rotation speed.
      getMaxRotationSpeed(): number;
      // Return particle size additive modifier.
      getSizeAdd(): number;
      // Return particle size multiplicative modifier.
      getSizeMul(): number;
      // Return number of color animation frames.
      getNumColorFrames(): number;
      // Return number of texture animation frames.
      getNumTextureFrames(): number;
      // Return random direction.
      getRandomDirection(): Vector3;
      // Return random size.
      getRandomSize(): Vector2;
      // Return random velocity.
      getRandomVelocity(): number;
      // Return random timetolive.
      getRandomTimeToLive(): number;
      // Return random rotationspeed.
      getRandomRotationSpeed(): number;
      // Return random rotation.
      getRandomRotation(): number;

   }

   export class ParticleEmitter extends BillboardSet {

      effect: ParticleEffect;
      numParticles: number;
      emitting: boolean;
      serializeParticles: boolean;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set particle effect.
      setEffect(effect: ParticleEffect): void;
      // Set maximum number of particles.
      setNumParticles(num: number): void;
      // Set whether should be emitting. If the state was changed, also resets the emission period timer.
      setEmitting(enable: boolean): void;
      // Set whether particles should be serialized. Default true, set false to reduce scene file size.
      setSerializeParticles(enable: boolean): void;
      // Reset the emission period timer.
      resetEmissionTimer(): void;
      // Remove all current particles.
      removeAllParticles(): void;
      // Reset the particle emitter completely. Removes current particles, sets emitting state on, and resets the emission timer.
      reset(): void;
      // Apply not continuously updated values such as the material, the number of particles and sorting mode from the particle effect. Call this if you change the effect programmatically.
      applyEffect(): void;
      // Return particle effect.
      getEffect(): ParticleEffect;
      // Return maximum number of particles.
      getNumParticles(): number;
      // Return whether is currently emitting.
      isEmitting(): boolean;
      // Return whether particles are to be serialized.
      getSerializeParticles(): boolean;

   }

   export class Skybox extends StaticModel {

      // Construct.
      constructor();


   }

   export class StaticModel extends Drawable {

      numOccluderTriangles: number;
      model: Model;
      material: Material;
      occlusionLodLevel: number;
      numGeometries: number;

      // Construct.
      constructor();

      // Return number of occlusion geometry triangles.
      getNumOccluderTriangles(): number;
      // Set model.
      setModel(model: Model): void;
      // Set material on all geometries.
      setMaterial(material: Material): void;
      // Set occlusion LOD level. By default (M_MAX_UNSIGNED) same as visible.
      setOcclusionLodLevel(level: number): void;
      // Apply default materials from a material list file. If filename is empty (default), the model's resource name with extension .txt will be used.
      applyMaterialList(fileName?: string): void;
      // Return model.
      getModel(): Model;
      // Return number of geometries.
      getNumGeometries(): number;
      // Return material by geometry index.
      getMaterial(index?: number): Material;
      // Return occlusion LOD level.
      getOcclusionLodLevel(): number;
      // Determines if the given world space point is within the model geometry.
      isInside(point: Vector3): boolean;
      // Determines if the given local space point is within the model geometry.
      isInsideLocal(point: Vector3): boolean;

   }

   export class StaticModelGroup extends StaticModel {

      numOccluderTriangles: number;
      numInstanceNodes: number;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Return number of occlusion geometry triangles.
      getNumOccluderTriangles(): number;
      // Add an instance scene node. It does not need any drawable components of its own.
      addInstanceNode(node: Node): void;
      // Remove an instance scene node.
      removeInstanceNode(node: Node): void;
      // Remove all instance scene nodes.
      removeAllInstanceNodes(): void;
      // Return number of instance nodes.
      getNumInstanceNodes(): number;
      // Return instance node by index.
      getInstanceNode(index: number): Node;

   }

   export class Terrain extends Component {

      patchSize: number;
      spacing: Vector3;
      smoothing: boolean;
      material: Material;
      drawDistance: number;
      shadowDistance: number;
      lodBias: number;
      viewMask: number;
      lightMask: number;
      shadowMask: number;
      zoneMask: number;
      maxLights: number;
      castShadows: boolean;
      occluder: boolean;
      occludee: boolean;
      numVertices: IntVector2;
      numPatches: IntVector2;
      heightMap: Image;
      patchSizeAttr: number;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set patch quads per side. Must be a power of two.
      setPatchSize(size: number): void;
      // Set vertex (XZ) and height (Y) spacing.
      setSpacing(spacing: Vector3): void;
      // Set smoothing of heightmap.
      setSmoothing(enable: boolean): void;
      // Set heightmap image. Dimensions should be a power of two + 1. Uses 8-bit grayscale, or optionally red as MSB and green as LSB for 16-bit accuracy. Return true if successful.
      setHeightMap(image: Image): boolean;
      // Set material.
      setMaterial(material: Material): void;
      // Set draw distance for patches.
      setDrawDistance(distance: number): void;
      // Set shadow draw distance for patches.
      setShadowDistance(distance: number): void;
      // Set LOD bias for patches. Affects which terrain LOD to display.
      setLodBias(bias: number): void;
      // Set view mask for patches. Is and'ed with camera's view mask to see if the object should be rendered.
      setViewMask(mask: number): void;
      // Set light mask for patches. Is and'ed with light's and zone's light mask to see if the object should be lit.
      setLightMask(mask: number): void;
      // Set shadow mask for patches. Is and'ed with light's light mask and zone's shadow mask to see if the object should be rendered to a shadow map.
      setShadowMask(mask: number): void;
      // Set zone mask for patches. Is and'ed with zone's zone mask to see if the object should belong to the zone.
      setZoneMask(mask: number): void;
      // Set maximum number of per-pixel lights for patches. Default 0 is unlimited.
      setMaxLights(num: number): void;
      // Set shadowcaster flag for patches.
      setCastShadows(enable: boolean): void;
      // Set occlusion flag for patches. Occlusion uses the coarsest LOD and may potentially be too aggressive, so use with caution.
      setOccluder(enable: boolean): void;
      // Set occludee flag for patches.
      setOccludee(enable: boolean): void;
      // Apply changes from the heightmap image.
      applyHeightMap(): void;
      // Return patch quads per side.
      getPatchSize(): number;
      // Return vertex and height spacing.
      getSpacing(): Vector3;
      // Return heightmap size in vertices.
      getNumVertices(): IntVector2;
      // Return heightmap size in patches.
      getNumPatches(): IntVector2;
      // Return whether smoothing is in use.
      getSmoothing(): boolean;
      // Return heightmap image.
      getHeightMap(): Image;
      // Return material.
      getMaterial(): Material;
      // Return height at world coordinates.
      getHeight(worldPosition: Vector3): number;
      // Return normal at world coordinates.
      getNormal(worldPosition: Vector3): Vector3;
      // Convert world position to heightmap pixel position. Note that the internal height data representation is reversed vertically, but in the heightmap image north is at the top.
      worldToHeightMap(worldPosition: Vector3): IntVector2;
      // Return draw distance.
      getDrawDistance(): number;
      // Return shadow draw distance.
      getShadowDistance(): number;
      // Return LOD bias.
      getLodBias(): number;
      // Return view mask.
      getViewMask(): number;
      // Return light mask.
      getLightMask(): number;
      // Return shadow mask.
      getShadowMask(): number;
      // Return zone mask.
      getZoneMask(): number;
      // Return maximum number of per-pixel lights.
      getMaxLights(): number;
      // Return visible flag.
      isVisible(): boolean;
      // Return shadowcaster flag.
      getCastShadows(): boolean;
      // Return occluder flag.
      isOccluder(): boolean;
      // Return occludee flag.
      isOccludee(): boolean;
      // Regenerate patch geometry.
      createPatchGeometry(patch: TerrainPatch): void;
      // Update patch based on LOD and neighbor LOD.
      updatePatchLod(patch: TerrainPatch): void;
      // Set patch size attribute.
      setPatchSizeAttr(value: number): void;

   }

   export class TerrainPatch extends Drawable {

      updateGeometryType: UpdateGeometryType;
      numOccluderTriangles: number;
      owner: Terrain;
      material: Material;
      boundingBox: BoundingBox;
      coordinates: IntVector2;
      occlusionOffset: number;
      northPatch: TerrainPatch;
      southPatch: TerrainPatch;
      westPatch: TerrainPatch;
      eastPatch: TerrainPatch;
      lodLevel: number;

      // Construct.
      constructor();

      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
      getUpdateGeometryType(): UpdateGeometryType;
      // Return number of occlusion geometry triangles.
      getNumOccluderTriangles(): number;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set owner terrain.
      setOwner(terrain: Terrain): void;
      // Set neighbor patches.
      setNeighbors(north: TerrainPatch, south: TerrainPatch, west: TerrainPatch, east: TerrainPatch): void;
      // Set material.
      setMaterial(material: Material): void;
      // Set local-space bounding box.
      setBoundingBox(box: BoundingBox): void;
      // Set patch coordinates.
      setCoordinates(coordinates: IntVector2): void;
      // Set vertical offset for occlusion geometry. Should be negative.
      setOcclusionOffset(offset: number): void;
      // Reset to LOD level 0.
      resetLod(): void;
      // Return owner terrain.
      getOwner(): Terrain;
      // Return north neighbor patch.
      getNorthPatch(): TerrainPatch;
      // Return south neighbor patch.
      getSouthPatch(): TerrainPatch;
      // Return west neighbor patch.
      getWestPatch(): TerrainPatch;
      // Return east neighbor patch.
      getEastPatch(): TerrainPatch;
      // Return patch coordinates.
      getCoordinates(): IntVector2;
      // Return current LOD level.
      getLodLevel(): number;
      // Return vertical offset for occlusion geometry..
      getOcclusionOffset(): number;

   }

   export class AnimationState extends RefCounted {

      looped: boolean;
      weight: number;
      time: number;
      layer: number;
      animation: Animation;
      model: AnimatedModel;
      node: Node;
      length: number;

      // Construct with animated model and animation pointers.
      constructor(model: AnimatedModel, animation: Animation);

      // Set looping enabled/disabled.
      setLooped(looped: boolean): void;
      // Set blending weight.
      setWeight(weight: number): void;
      // Set time position. Does not fire animation triggers.
      setTime(time: number): void;
      // Modify blending weight.
      addWeight(delta: number): void;
      // Modify time position. %Animation triggers will be fired.
      addTime(delta: number): void;
      // Set blending layer.
      setLayer(layer: number): void;
      // Return animation.
      getAnimation(): Animation;
      // Return animated model this state belongs to (model mode.)
      getModel(): AnimatedModel;
      // Return root scene node this state controls (node hierarchy mode.)
      getNode(): Node;
      // Return whether weight is nonzero.
      isEnabled(): boolean;
      // Return whether looped.
      isLooped(): boolean;
      // Return blending weight.
      getWeight(): number;
      // Return time position.
      getTime(): number;
      // Return animation length.
      getLength(): number;
      // Return blending layer.
      getLayer(): number;
      // Apply the animation at the current time position.
      apply(): void;

   }



//----------------------------------------------------
// MODULE: Atomic2D
//----------------------------------------------------


   export class AnimatedSprite2D extends StaticSprite2D {

      speed: number;
      animationSet: AnimationSet2D;
      loopMode: LoopMode2D;
      animation: string;
      rootNode: Node;
      animationAttr: string;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set speed.
      setSpeed(speed: number): void;
      // Set animation by animation set, name and loop mode.
      setAnimation(animationSet: AnimationSet2D, name: string, loopMode?: LoopMode2D): void;
      // Set animation set.
      setAnimationSet(animationSet: AnimationSet2D): void;
      // Set loop mode.
      setLoopMode(loopMode: LoopMode2D): void;
      // Return speed.
      getSpeed(): number;
      // Return animation name.
      getAnimation(): string;
      // Return animation.
      getAnimationSet(): AnimationSet2D;
      // Return loop mode.
      getLoopMode(): LoopMode2D;
      // Return root node.
      getRootNode(): Node;
      // Set animation by name.
      setAnimationAttr(name: string): void;

   }

   export class AnimationSet2D extends Resource {

      numAnimations: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Get number of animations.
      getNumAnimations(): number;

   }

   export class CollisionBox2D extends CollisionShape2D {

      size: Vector2;
      angle: number;
      center: Vector2;

      // Construct.
      constructor();

      // Set size.
      setSize(size: Vector2): void;
      // Set angle.
      setAngle(angle: number): void;
      // Return size.
      getSize(): Vector2;
      // Return center.
      getCenter(): Vector2;
      // Return angle.
      getAngle(): number;

   }

   export class CollisionChain2D extends CollisionShape2D {

      loop: boolean;
      vertexCount: number;

      // Construct.
      constructor();

      // Set loop.
      setLoop(loop: boolean): void;
      // Set vertex count.
      setVertexCount(count: number): void;
      // Set vertex.
      setVertex(index: number, vertex: Vector2): void;
      // Return loop.
      getLoop(): boolean;
      // Return vertex count.
      getVertexCount(): number;
      // Return vertex.
      getVertex(index: number): Vector2;

   }

   export class CollisionCircle2D extends CollisionShape2D {

      radius: number;
      center: Vector2;

      // Construct.
      constructor();

      // Set radius.
      setRadius(radius: number): void;
      // Return radius.
      getRadius(): number;
      // Return center.
      getCenter(): Vector2;

   }

   export class CollisionEdge2D extends CollisionShape2D {

      vertex1: Vector2;
      vertex2: Vector2;

      // Construct.
      constructor();

      // Set vertex 1.
      setVertex1(vertex: Vector2): void;
      // Set vertex 2.
      setVertex2(vertex: Vector2): void;
      // Set vertices.
      setVertices(vertex1: Vector2, vertex2: Vector2): void;
      // Return vertex 1.
      getVertex1(): Vector2;
      // Return vertex 2.
      getVertex2(): Vector2;

   }

   export class CollisionPolygon2D extends CollisionShape2D {

      vertexCount: number;

      // Construct.
      constructor();

      // Set vertex count.
      setVertexCount(count: number): void;
      // Set vertex.
      setVertex(index: number, vertex: Vector2): void;
      // Return vertex count.
      getVertexCount(): number;
      // Return vertex.
      getVertex(index: number): Vector2;

   }

   export class CollisionShape2D extends Component {

      trigger: boolean;
      categoryBits: number;
      maskBits: number;
      groupIndex: number;
      density: number;
      friction: number;
      restitution: number;
      mass: number;
      inertia: number;
      massCenter: Vector2;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set trigger.
      setTrigger(trigger: boolean): void;
      // Set filter category bits.
      setCategoryBits(categoryBits: number): void;
      // Set filter mask bits.
      setMaskBits(maskBits: number): void;
      // Set filter group index.
      setGroupIndex(groupIndex: number): void;
      // Set density.
      setDensity(density: number): void;
      // Set friction.
      setFriction(friction: number): void;
      // Set restitution .
      setRestitution(restitution: number): void;
      // Create fixture.
      createFixture(): void;
      // Release fixture.
      releaseFixture(): void;
      // Return trigger.
      isTrigger(): boolean;
      // Return filter category bits.
      getCategoryBits(): number;
      // Return filter mask bits.
      getMaskBits(): number;
      // Return filter group index.
      getGroupIndex(): number;
      // Return density.
      getDensity(): number;
      // Return friction.
      getFriction(): number;
      // Return restitution.
      getRestitution(): number;
      // Return mass.
      getMass(): number;
      // Return inertia.
      getInertia(): number;
      // Return mass center.
      getMassCenter(): Vector2;

   }

   export class Constraint2D extends Component {

      otherBody: RigidBody2D;
      collideConnected: boolean;
      attachedConstraint: Constraint2D;
      ownerBody: RigidBody2D;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Create Joint.
      createJoint(): void;
      // Release Joint.
      releaseJoint(): void;
      // Set other rigid body.
      setOtherBody(body: RigidBody2D): void;
      // Set collide connected.
      setCollideConnected(collideConnected: boolean): void;
      // Set attached constriant (for gear).
      setAttachedConstraint(constraint: Constraint2D): void;
      // Return owner body.
      getOwnerBody(): RigidBody2D;
      // Return other body.
      getOtherBody(): RigidBody2D;
      // Return collide connected.
      getCollideConnected(): boolean;
      // Return attached constraint (for gear).
      getAttachedConstraint(): Constraint2D;

   }

   export class ConstraintDistance2D extends Constraint2D {

      ownerBodyAnchor: Vector2;
      otherBodyAnchor: Vector2;
      frequencyHz: number;
      dampingRatio: number;

      // Construct.
      constructor();

      // Set owner body anchor.
      setOwnerBodyAnchor(anchor: Vector2): void;
      // Set other body anchor.
      setOtherBodyAnchor(anchor: Vector2): void;
      // Set frequency Hz.
      setFrequencyHz(frequencyHz: number): void;
      // Set damping ratio.
      setDampingRatio(dampingRatio: number): void;
      // Return owner body anchor.
      getOwnerBodyAnchor(): Vector2;
      // Return other body anchor.
      getOtherBodyAnchor(): Vector2;
      // Return frequency Hz.
      getFrequencyHz(): number;
      // Return damping ratio.
      getDampingRatio(): number;

   }

   export class ConstraintFriction2D extends Constraint2D {

      anchor: Vector2;
      maxForce: number;
      maxTorque: number;

      // Construct.
      constructor();

      // Set anchor.
      setAnchor(anchor: Vector2): void;
      // Set max force.
      setMaxForce(maxForce: number): void;
      // Set max torque.
      setMaxTorque(maxTorque: number): void;
      // Return anchor.
      getAnchor(): Vector2;
      // Set max force.
      getMaxForce(): number;
      // Set max torque.
      getMaxTorque(): number;

   }

   export class ConstraintGear2D extends Constraint2D {

      ownerConstraint: Constraint2D;
      otherConstraint: Constraint2D;
      ratio: number;

      // Construct.
      constructor();

      // Set owner constraint.
      setOwnerConstraint(constraint: Constraint2D): void;
      // Set other constraint.
      setOtherConstraint(constraint: Constraint2D): void;
      // Set ratio.
      setRatio(ratio: number): void;
      // Return owner constraint.
      getOwnerConstraint(): Constraint2D;
      // Return other constraint.
      getOtherConstraint(): Constraint2D;
      // Return ratio.
      getRatio(): number;

   }

   export class ConstraintMotor2D extends Constraint2D {

      linearOffset: Vector2;
      angularOffset: number;
      maxForce: number;
      maxTorque: number;
      correctionFactor: number;

      // Construct.
      constructor();

      // Set linear offset.
      setLinearOffset(linearOffset: Vector2): void;
      // Set angular offset.
      setAngularOffset(angularOffset: number): void;
      // Set max force.
      setMaxForce(maxForce: number): void;
      // Set max torque.
      setMaxTorque(maxTorque: number): void;
      // Set correction factor.
      setCorrectionFactor(correctionFactor: number): void;
      // Return linear offset.
      getLinearOffset(): Vector2;
      // Return angular offset.
      getAngularOffset(): number;
      // Return max force.
      getMaxForce(): number;
      // Return max torque.
      getMaxTorque(): number;
      // Return correction factor.
      getCorrectionFactor(): number;

   }

   export class ConstraintMouse2D extends Constraint2D {

      target: Vector2;
      maxForce: number;
      frequencyHz: number;
      dampingRatio: number;

      // Construct.
      constructor();

      // Set target.
      setTarget(target: Vector2): void;
      // Set max force.
      setMaxForce(maxForce: number): void;
      // Set frequency Hz.
      setFrequencyHz(frequencyHz: number): void;
      // Set damping ratio.
      setDampingRatio(dampingRatio: number): void;
      // Return target.
      getTarget(): Vector2;
      // Return max force.
      getMaxForce(): number;
      // Return frequency Hz.
      getFrequencyHz(): number;
      // Return damping ratio.
      getDampingRatio(): number;

   }

   export class ConstraintPrismatic2D extends Constraint2D {

      anchor: Vector2;
      axis: Vector2;
      enableLimit: boolean;
      lowerTranslation: number;
      upperTranslation: number;
      enableMotor: boolean;
      maxMotorForce: number;
      motorSpeed: number;

      // Construct.
      constructor();

      // Set anchor.
      setAnchor(anchor: Vector2): void;
      // Set axis.
      setAxis(axis: Vector2): void;
      // Set enable limit.
      setEnableLimit(enableLimit: boolean): void;
      // Set lower translation.
      setLowerTranslation(lowerTranslation: number): void;
      // Set upper translation.
      setUpperTranslation(upperTranslation: number): void;
      // Set enable motor.
      setEnableMotor(enableMotor: boolean): void;
      // Set maxmotor force.
      setMaxMotorForce(maxMotorForce: number): void;
      // Set motor speed.
      setMotorSpeed(motorSpeed: number): void;
      // Return anchor.
      getAnchor(): Vector2;
      // Return axis.
      getAxis(): Vector2;
      // Return enable limit.
      getEnableLimit(): boolean;
      // Return lower translation.
      getLowerTranslation(): number;
      // Return upper translation.
      getUpperTranslation(): number;
      // Return enable motor.
      getEnableMotor(): boolean;
      // Return maxmotor force.
      getMaxMotorForce(): number;
      // Return motor speed.
      getMotorSpeed(): number;

   }

   export class ConstraintPulley2D extends Constraint2D {

      ownerBodyGroundAnchor: Vector2;
      otherBodyGroundAnchor: Vector2;
      ownerBodyAnchor: Vector2;
      otherBodyAnchor: Vector2;
      ratio: number;

      // Construct.
      constructor();

      // Set other body ground anchor point.
      setOwnerBodyGroundAnchor(groundAnchor: Vector2): void;
      // Set other body ground anchor point.
      setOtherBodyGroundAnchor(groundAnchor: Vector2): void;
      // Set owner body anchor point.
      setOwnerBodyAnchor(anchor: Vector2): void;
      // Set other body anchor point.
      setOtherBodyAnchor(anchor: Vector2): void;
      // Set ratio.
      setRatio(ratio: number): void;
      // Return owner body ground anchor.
      getOwnerBodyGroundAnchor(): Vector2;
      // return other body ground anchor.
      getOtherBodyGroundAnchor(): Vector2;
      // Return owner body anchor.
      getOwnerBodyAnchor(): Vector2;
      // Return other body anchor.
      getOtherBodyAnchor(): Vector2;
      // Return ratio.
      getRatio(): number;

   }

   export class ConstraintRevolute2D extends Constraint2D {

      anchor: Vector2;
      enableLimit: boolean;
      lowerAngle: number;
      upperAngle: number;
      enableMotor: boolean;
      motorSpeed: number;
      maxMotorTorque: number;

      // Construct.
      constructor();

      // Set anchor.
      setAnchor(anchor: Vector2): void;
      // Set enable limit.
      setEnableLimit(enableLimit: boolean): void;
      // Set lower angle.
      setLowerAngle(lowerAngle: number): void;
      // Set upper angle.
      setUpperAngle(upperAngle: number): void;
      // Set enable motor.
      setEnableMotor(enableMotor: boolean): void;
      // Set motor speed.
      setMotorSpeed(motorSpeed: number): void;
      // Set max motor torque.
      setMaxMotorTorque(maxMotorTorque: number): void;
      // Return anchor.
      getAnchor(): Vector2;
      // Return enable limit.
      getEnableLimit(): boolean;
      // Return lower angle.
      getLowerAngle(): number;
      // Return upper angle.
      getUpperAngle(): number;
      // Return enable motor.
      getEnableMotor(): boolean;
      // Return motor speed.
      getMotorSpeed(): number;
      // Return max motor torque.
      getMaxMotorTorque(): number;

   }

   export class ConstraintRope2D extends Constraint2D {

      ownerBodyAnchor: Vector2;
      otherBodyAnchor: Vector2;
      maxLength: number;

      // Construct.
      constructor();

      // Set owner body anchor.
      setOwnerBodyAnchor(anchor: Vector2): void;
      // Set other body anchor.
      setOtherBodyAnchor(anchor: Vector2): void;
      // Set max length.
      setMaxLength(maxLength: number): void;
      // Return owner body anchor.
      getOwnerBodyAnchor(): Vector2;
      // Return other body anchor.
      getOtherBodyAnchor(): Vector2;
      // Return max length.
      getMaxLength(): number;

   }

   export class ConstraintWeld2D extends Constraint2D {

      anchor: Vector2;
      frequencyHz: number;
      dampingRatio: number;

      // Construct.
      constructor();

      // Set anchor.
      setAnchor(anchor: Vector2): void;
      // Set frequency Hz.
      setFrequencyHz(frequencyHz: number): void;
      // Set damping ratio.
      setDampingRatio(dampingRatio: number): void;
      // Return anchor.
      getAnchor(): Vector2;
      // Return frequency Hz.
      getFrequencyHz(): number;
      // Return damping ratio.
      getDampingRatio(): number;

   }

   export class ConstraintWheel2D extends Constraint2D {

      anchor: Vector2;
      axis: Vector2;
      enableMotor: boolean;
      maxMotorTorque: number;
      motorSpeed: number;
      frequencyHz: number;
      dampingRatio: number;

      // Construct.
      constructor();

      // Set anchor.
      setAnchor(anchor: Vector2): void;
      // Set axis.
      setAxis(axis: Vector2): void;
      // Set enable motor.
      setEnableMotor(enableMotor: boolean): void;
      // Set max motor torque.
      setMaxMotorTorque(maxMotorTorque: number): void;
      // Set motor speed.
      setMotorSpeed(motorSpeed: number): void;
      // Set frequency Hz.
      setFrequencyHz(frequencyHz: number): void;
      // Set damping ratio.
      setDampingRatio(dampingRatio: number): void;
      // Return anchor.
      getAnchor(): Vector2;
      // Return axis.
      getAxis(): Vector2;
      // Return enable motor.
      getEnableMotor(): boolean;
      // Return maxMotor torque.
      getMaxMotorTorque(): number;
      // Return motor speed.
      getMotorSpeed(): number;
      // Return frequency Hz.
      getFrequencyHz(): number;
      // Return damping ratio.
      getDampingRatio(): number;

   }

   export class Drawable2D extends Drawable {

      layer: number;
      orderInLayer: number;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set layer.
      setLayer(layer: number): void;
      // Set order in layer.
      setOrderInLayer(orderInLayer: number): void;
      // Return layer.
      getLayer(): number;
      // Return order in layer.
      getOrderInLayer(): number;

   }

   export class Light2D extends Component {

      lightGroup: Light2DGroup;
      color: Color;
      numRays: number;
      lightType: LightType2D;
      castShadows: boolean;
      softShadows: boolean;
      softShadowLength: number;
      backtrace: boolean;

      // Construct.
      constructor();

      setLightGroup(group: Light2DGroup): void;
      getLightGroup(): Light2DGroup;
      getColor(): Color;
      setColor(color: Color): void;
      updateVertices(): void;
      setNumRays(numRays: number): void;
      getNumRays(): number;
      onSetEnabled(): void;
      getLightType(): LightType2D;
      getCastShadows(): boolean;
      setCastShadows(castShadows: boolean): void;
      getSoftShadows(): boolean;
      setSoftShadows(softShadows: boolean): void;
      getSoftShadowLength(): number;
      setSoftShadowLength(softShadowLength: number): void;
      getBacktrace(): boolean;
      setBacktrace(backtrace: boolean): void;

   }

   export class DirectionalLight2D extends Light2D {

      direction: number;

      // Construct.
      constructor();

      updateVertices(): void;
      getDirection(): number;
      setDirection(direction: number): void;

   }

   export class PositionalLight2D extends Light2D {

      // Construct.
      constructor();

      updateVertices(): void;

   }

   export class PointLight2D extends PositionalLight2D {

      radius: number;

      // Construct.
      constructor();

      updateVertices(): void;
      setRadius(radius: number): void;
      getRadius(): number;

   }

   export class Light2DGroup extends Drawable2D {

      physicsWorld: PhysicsWorld2D;
      ambientColor: Color;
      frustumBox: BoundingBox;

      // Construct.
      constructor();

      setPhysicsWorld(physicsWorld: PhysicsWorld2D): void;
      getPhysicsWorld(): PhysicsWorld2D;
      addLight2D(light: Light2D): void;
      setDirty(): void;
      setAmbientColor(color: Color): void;
      getAmbientColor(): Color;
      getFrustumBox(): BoundingBox;

   }

   export class ParticleEffect2D extends Resource {

      sprite: Sprite2D;
      sourcePositionVariance: Vector2;
      speed: number;
      speedVariance: number;
      particleLifeSpan: number;
      particleLifespanVariance: number;
      angle: number;
      angleVariance: number;
      gravity: Vector2;
      radialAcceleration: number;
      tangentialAcceleration: number;
      radialAccelVariance: number;
      tangentialAccelVariance: number;
      startColor: Color;
      startColorVariance: Color;
      finishColor: Color;
      finishColorVariance: Color;
      maxParticles: number;
      startParticleSize: number;
      startParticleSizeVariance: number;
      finishParticleSize: number;
      finishParticleSizeVariance: number;
      duration: number;
      emitterType: EmitterType2D;
      maxRadius: number;
      maxRadiusVariance: number;
      minRadius: number;
      minRadiusVariance: number;
      rotatePerSecond: number;
      rotatePerSecondVariance: number;
      blendMode: BlendMode;
      rotationStart: number;
      rotationStartVariance: number;
      rotationEnd: number;
      rotationEndVariance: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Set sprite.
      setSprite(sprite: Sprite2D): void;
      // Set source position variance.
      setSourcePositionVariance(sourcePositionVariance: Vector2): void;
      // Set speed.
      setSpeed(speed: number): void;
      // Set speed variance.
      setSpeedVariance(speedVariance: number): void;
      // Set particle lifespan.
      setParticleLifeSpan(particleLifeSpan: number): void;
      // Set particle lifespan variance.
      setParticleLifespanVariance(particleLifespanVariance: number): void;
      // Set angle.
      setAngle(angle: number): void;
      // Set angle variance.
      setAngleVariance(angleVariance: number): void;
      // Set gravity.
      setGravity(gravity: Vector2): void;
      // Set radial acceleration.
      setRadialAcceleration(radialAcceleration: number): void;
      // Set tangential acceleration.
      setTangentialAcceleration(tangentialAcceleration: number): void;
      // Set radial acceleration variance.
      setRadialAccelVariance(radialAccelVariance: number): void;
      // Set tangential acceleration variance.
      setTangentialAccelVariance(tangentialAccelVariance: number): void;
      // Set start color.
      setStartColor(startColor: Color): void;
      // Set start color variance.
      setStartColorVariance(startColorVariance: Color): void;
      // Set finish color.
      setFinishColor(finishColor: Color): void;
      // Set finish color variance.
      setFinishColorVariance(finishColorVariance: Color): void;
      // Set max particles.
      setMaxParticles(maxParticles: number): void;
      // Set start particle size.
      setStartParticleSize(startParticleSize: number): void;
      // Set start particle size variance.
      setStartParticleSizeVariance(startParticleSizeVariance: number): void;
      // Set finish particle size.
      setFinishParticleSize(finishParticleSize: number): void;
      // Set finish particle size variance.
      setFinishParticleSizeVariance(FinishParticleSizeVariance: number): void;
      // Set duration.
      setDuration(duration: number): void;
      // Set emitter type.
      setEmitterType(emitterType: EmitterType2D): void;
      // Set max radius.
      setMaxRadius(maxRadius: number): void;
      // Set max radius variance.
      setMaxRadiusVariance(maxRadiusVariance: number): void;
      // Set min radius.
      setMinRadius(minRadius: number): void;
      // Set min radius variance.
      setMinRadiusVariance(minRadiusVariance: number): void;
      // Set rotate per second.
      setRotatePerSecond(rotatePerSecond: number): void;
      // Set rotate per second variance.
      setRotatePerSecondVariance(rotatePerSecondVariance: number): void;
      // Set blend mode.
      setBlendMode(blendMode: BlendMode): void;
      // Set rotation start.
      setRotationStart(rotationStart: number): void;
      // Set rotation start variance.
      setRotationStartVariance(rotationStartVariance: number): void;
      // Set rotation end.
      setRotationEnd(rotationEnd: number): void;
      // Set rotation end variance.
      setRotationEndVariance(rotationEndVariance: number): void;
      // Return sprite.
      getSprite(): Sprite2D;
      // Return source position variance.
      getSourcePositionVariance(): Vector2;
      // Return speed.
      getSpeed(): number;
      // Return speed variance.
      getSpeedVariance(): number;
      // Return particle lifespan.
      getParticleLifeSpan(): number;
      // Return particle lifespan variance.
      getParticleLifespanVariance(): number;
      // Return angle.
      getAngle(): number;
      // Return angle variance.
      getAngleVariance(): number;
      // Return gravity.
      getGravity(): Vector2;
      // Return radial acceleration.
      getRadialAcceleration(): number;
      // Return tangential acceleration.
      getTangentialAcceleration(): number;
      // Return radial acceleration variance.
      getRadialAccelVariance(): number;
      // Return tangential acceleration variance.
      getTangentialAccelVariance(): number;
      // Return start color.
      getStartColor(): Color;
      // Return start color variance.
      getStartColorVariance(): Color;
      // Return finish color.
      getFinishColor(): Color;
      // Return finish color variance.
      getFinishColorVariance(): Color;
      // Return max particles.
      getMaxParticles(): number;
      // Return start particle size.
      getStartParticleSize(): number;
      // Return start particle size variance.
      getStartParticleSizeVariance(): number;
      // Return finish particle size.
      getFinishParticleSize(): number;
      // Return finish particle size variance.
      getFinishParticleSizeVariance(): number;
      // Return duration.
      getDuration(): number;
      // Return emitter type.
      getEmitterType(): EmitterType2D;
      // Return max radius.
      getMaxRadius(): number;
      // Return max radius variance.
      getMaxRadiusVariance(): number;
      // Return min radius.
      getMinRadius(): number;
      // Return min radius variance.
      getMinRadiusVariance(): number;
      // Return rotate per second.
      getRotatePerSecond(): number;
      // Return rotate per second variance.
      getRotatePerSecondVariance(): number;
      // Return blend mode.
      getBlendMode(): BlendMode;
      // Return rotation start.
      getRotationStart(): number;
      // Return rotation start variance.
      getRotationStartVariance(): number;
      // Return rotation end.
      getRotationEnd(): number;
      // Return rotation end variance.
      getRotationEndVariance(): number;

   }

   export class ParticleEmitter2D extends Drawable2D {

      effect: ParticleEffect2D;
      sprite: Sprite2D;
      blendMode: BlendMode;
      maxParticles: number;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set particle effect.
      setEffect(effect: ParticleEffect2D): void;
      // Set sprite.
      setSprite(sprite: Sprite2D): void;
      // Set blend mode.
      setBlendMode(blendMode: BlendMode): void;
      // Set max particles.
      setMaxParticles(maxParticles: number): void;
      // Return particle effect.
      getEffect(): ParticleEffect2D;
      // Return sprite.
      getSprite(): Sprite2D;
      // Return blend mode.
      getBlendMode(): BlendMode;
      // Return max particles.
      getMaxParticles(): number;

   }

   export class PhysicsWorld2D extends Component {

      drawShape: boolean;
      drawJoint: boolean;
      drawAabb: boolean;
      drawPair: boolean;
      drawCenterOfMass: boolean;
      allowSleeping: boolean;
      warmStarting: boolean;
      continuousPhysics: boolean;
      subStepping: boolean;
      gravity: Vector2;
      autoClearForces: boolean;
      velocityIterations: number;
      positionIterations: number;
      applyingTransforms: boolean;

      // Construct.
      constructor();

      // Step the simulation forward.
      update(timeStep: number): void;
      // Add debug geometry to the debug renderer.
      drawDebugGeometry(): void;
      // Set draw shape.
      setDrawShape(drawShape: boolean): void;
      // Set draw joint.
      setDrawJoint(drawJoint: boolean): void;
      // Set draw aabb.
      setDrawAabb(drawAabb: boolean): void;
      // Set draw pair.
      setDrawPair(drawPair: boolean): void;
      // Set draw center of mass.
      setDrawCenterOfMass(drawCenterOfMass: boolean): void;
      // Set allow sleeping.
      setAllowSleeping(enable: boolean): void;
      // Set warm starting.
      setWarmStarting(enable: boolean): void;
      // Set continuous physics.
      setContinuousPhysics(enable: boolean): void;
      // Set sub stepping.
      setSubStepping(enable: boolean): void;
      // Set gravity.
      setGravity(gravity: Vector2): void;
      // Set auto clear forces.
      setAutoClearForces(enable: boolean): void;
      // Set velocity iterations.
      setVelocityIterations(velocityIterations: number): void;
      // Set position iterations.
      setPositionIterations(positionIterations: number): void;
      // Add rigid body.
      addRigidBody(rigidBody: RigidBody2D): void;
      // Remove rigid body.
      removeRigidBody(rigidBody: RigidBody2D): void;
      // Return draw shape.
      getDrawShape(): boolean;
      // Return draw joint.
      getDrawJoint(): boolean;
      // Return draw aabb.
      getDrawAabb(): boolean;
      // Return draw pair.
      getDrawPair(): boolean;
      // Return draw center of mass.
      getDrawCenterOfMass(): boolean;
      // Return allow sleeping.
      getAllowSleeping(): boolean;
      // Return warm starting.
      getWarmStarting(): boolean;
      // Return continuous physics.
      getContinuousPhysics(): boolean;
      // Return sub stepping.
      getSubStepping(): boolean;
      // Return auto clear forces.
      getAutoClearForces(): boolean;
      // Return gravity.
      getGravity(): Vector2;
      // Return velocity iterations.
      getVelocityIterations(): number;
      // Return position iterations.
      getPositionIterations(): number;
      // Set node dirtying to be disregarded.
      setApplyingTransforms(enable: boolean): void;
      // Return whether node dirtying should be disregarded.
      isApplyingTransforms(): boolean;

   }

   export class RigidBody2D extends Component {

      bodyType: BodyType2D;
      mass: number;
      inertia: number;
      massCenter: Vector2;
      useFixtureMass: boolean;
      linearDamping: number;
      angularDamping: number;
      allowSleep: boolean;
      fixedRotation: boolean;
      bullet: boolean;
      gravityScale: number;
      awake: boolean;
      linearVelocity: Vector2;
      angularVelocity: number;
      castShadows: boolean;

      // Construct.
      constructor();

      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Set body type.
      setBodyType(bodyType: BodyType2D): void;
      // Set Mass.
      setMass(mass: number): void;
      // Set inertia.
      setInertia(inertia: number): void;
      // Set mass center.
      setMassCenter(center: Vector2): void;
      // Use fixture mass (default is true).
      setUseFixtureMass(useFixtureMass: boolean): void;
      // Set linear damping.
      setLinearDamping(linearDamping: number): void;
      // Set angular damping.
      setAngularDamping(angularDamping: number): void;
      // Set allow sleep.
      setAllowSleep(allowSleep: boolean): void;
      // Set fixed rotation.
      setFixedRotation(fixedRotation: boolean): void;
      // Set bullet.
      setBullet(bullet: boolean): void;
      // Set gravity scale.
      setGravityScale(gravityScale: number): void;
      // Set awake.
      setAwake(awake: boolean): void;
      // Set linear velocity.
      setLinearVelocity(linearVelocity: Vector2): void;
      // Set angular velocity.
      setAngularVelocity(angularVelocity: number): void;
      // Apply force.
      applyForce(force: Vector2, point: Vector2, wake: boolean): void;
      // Apply force to center.
      applyForceToCenter(force: Vector2, wake: boolean): void;
      // Apply Torque.
      applyTorque(torque: number, wake: boolean): void;
      // Apply linear impulse.
      applyLinearImpulse(impulse: Vector2, point: Vector2, wake: boolean): void;
      // Apply angular impulse.
      applyAngularImpulse(impulse: number, wake: boolean): void;
      // Create body.
      createBody(): void;
      // Release body.
      releaseBody(): void;
      // Apply world transform.
      applyWorldTransform(): void;
      // Add collision shape.
      addCollisionShape2D(collisionShape: CollisionShape2D): void;
      // Remove collision shape.
      removeCollisionShape2D(collisionShape: CollisionShape2D): void;
      // Add constraint.
      addConstraint2D(constraint: Constraint2D): void;
      // Remove constraint.
      removeConstraint2D(constraint: Constraint2D): void;
      // Return body type.
      getBodyType(): BodyType2D;
      // Return Mass.
      getMass(): number;
      // Return inertia.
      getInertia(): number;
      // Return mass center.
      getMassCenter(): Vector2;
      // Return use fixture mass.
      getUseFixtureMass(): boolean;
      // Return linear damping.
      getLinearDamping(): number;
      // Return angular damping.
      getAngularDamping(): number;
      // Return allow sleep.
      isAllowSleep(): boolean;
      // Return fixed rotation.
      isFixedRotation(): boolean;
      // Return bullet.
      isBullet(): boolean;
      // Return gravity scale.
      getGravityScale(): number;
      // Return awake.
      isAwake(): boolean;
      // Return linear velocity.
      getLinearVelocity(): Vector2;
      // Return angular velocity.
      getAngularVelocity(): number;
      getCastShadows(): boolean;
      setCastShadows(castShadows: boolean): void;

   }

   export class Sprite2D extends Resource {

      texture: Texture2D;
      rectangle: IntRect;
      hotSpot: Vector2;
      offset: IntVector2;
      spriteSheet: SpriteSheet2D;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Set texture.
      setTexture(texture: Texture2D): void;
      // Set rectangle.
      setRectangle(rectangle: IntRect): void;
      // Set hot spot.
      setHotSpot(hotSpot: Vector2): void;
      // Set offset.
      setOffset(offset: IntVector2): void;
      // Set sprite sheet.
      setSpriteSheet(spriteSheet: SpriteSheet2D): void;
      // Return texture.
      getTexture(): Texture2D;
      // Return rectangle.
      getRectangle(): IntRect;
      // Return hot spot.
      getHotSpot(): Vector2;
      // Return offset.
      getOffset(): IntVector2;
      // Return sprite sheet.
      getSpriteSheet(): SpriteSheet2D;
      // Return texture rectangle.
      getTextureRectangle(rect: Rect, flipX?: boolean, flipY?: boolean): boolean;

   }

   export class SpriteSheet2D extends Resource {

      texture: Texture2D;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Return texture.
      getTexture(): Texture2D;
      // Return sprite.
      getSprite(name: string): Sprite2D;
      // Define sprite.
      defineSprite(name: string, rectangle: IntRect, hotSpot?: Vector2, offset?: IntVector2): void;

   }

   export class StaticSprite2D extends Drawable2D {

      sprite: Sprite2D;
      blendMode: BlendMode;
      flipX: boolean;
      flipY: boolean;
      color: Color;
      alpha: number;
      useHotSpot: boolean;
      hotSpot: Vector2;
      customMaterial: Material;

      // Construct.
      constructor();

      // Set sprite.
      setSprite(sprite: Sprite2D): void;
      // Set blend mode.
      setBlendMode(blendMode: BlendMode): void;
      // Set flip.
      setFlip(flipX: boolean, flipY: boolean): void;
      // Set flip X.
      setFlipX(flipX: boolean): void;
      // Set flip Y.
      setFlipY(flipY: boolean): void;
      // Set color.
      setColor(color: Color): void;
      // Set alpha.
      setAlpha(alpha: number): void;
      // Set use hot spot.
      setUseHotSpot(useHotSpot: boolean): void;
      // Set hot spot.
      setHotSpot(hotspot: Vector2): void;
      // Set custom material.
      setCustomMaterial(customMaterial: Material): void;
      // Return sprite.
      getSprite(): Sprite2D;
      // Return blend mode.
      getBlendMode(): BlendMode;
      // Return flip X.
      getFlipX(): boolean;
      // Return flip Y.
      getFlipY(): boolean;
      // Return color.
      getColor(): Color;
      // Return alpha.
      getAlpha(): number;
      // Return use hot spot.
      getUseHotSpot(): boolean;
      // Return hot spot.
      getHotSpot(): Vector2;
      // Return custom material.
      getCustomMaterial(): Material;

   }

   export class TileMap2D extends Component {

      tmxFile: TmxFile2D;
      numLayers: number;

      // Construct.
      constructor();

      // Set tmx file.
      setTmxFile(tmxFile: TmxFile2D): void;
      // Return tmx file.
      getTmxFile(): TmxFile2D;
      // Return number of layers.
      getNumLayers(): number;
      // Return tile map layer at index.
      getLayer(index: number): TileMapLayer2D;
      getLayerByName(name: string): TileMapLayer2D;
      // Convert tile index to position.
      tileIndexToPosition(x: number, y: number): Vector2;

   }

   export class TileMapLayer2D extends Component {

      drawOrder: number;
      visible: boolean;
      tileMap: TileMap2D;
      tmxLayer: TmxLayer2D;
      layerType: TileMapLayerType2D;
      width: number;
      height: number;
      numObjects: number;
      imageNode: Node;
      name: string;

      // Construct.
      constructor();

      // Add debug geometry to the debug renderer.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Initialize with tile map and tmx layer.
      initialize(tileMap: TileMap2D, tmxLayer: TmxLayer2D): void;
      // Set draw order
      setDrawOrder(drawOrder: number): void;
      // Set visible.
      setVisible(visible: boolean): void;
      // Return tile map.
      getTileMap(): TileMap2D;
      // Return tmx layer.
      getTmxLayer(): TmxLayer2D;
      // Return draw order.
      getDrawOrder(): number;
      // Return visible.
      isVisible(): boolean;
      // Return has property
      hasProperty(name: string): boolean;
      // Return property.
      getProperty(name: string): string;
      // Return layer type.
      getLayerType(): TileMapLayerType2D;
      // Return width (for tile layer only).
      getWidth(): number;
      // Return height (for tile layer only).
      getHeight(): number;
      // Return tile node (for tile layer only).
      getTileNode(x: number, y: number): Node;
      // Return tile (for tile layer only).
      getTile(x: number, y: number): Tile2D;
      // Return number of tile map objects (for object group only).
      getNumObjects(): number;
      // Return tile map object (for object group only).
      getObject(index: number): TileMapObject2D;
      // Return object node (for object group only).
      getObjectNode(index: number): Node;
      // Return image node (for image layer only).
      getImageNode(): Node;
      getName(): string;

   }

   export class Animation2D extends RefCounted {

      name: string;
      length: number;
      looped: boolean;
      animationSet: AnimationSet2D;
      numTracks: number;

      // Construct.
      constructor(animationSet: AnimationSet2D);

      // Set name.
      setName(name: string): void;
      // Set length.
      setLength(length: number): void;
      // Set looped.
      setLooped(looped: boolean): void;
      // Return animation set.
      getAnimationSet(): AnimationSet2D;
      // Return name.
      getName(): string;
      // Return length.
      getLength(): number;
      // Return looped.
      isLooped(): boolean;
      // Return number of animation tracks.
      getNumTracks(): number;

   }

   export class PropertySet2D extends RefCounted {

      constructor();

      // Return has property.
      hasProperty(name: string): boolean;
      // Return property value.
      getProperty(name: string): string;

   }

   export class Tile2D extends RefCounted {

      gid: number;
      sprite: Sprite2D;
      objectGroup: TmxObjectGroup2D;

      // Construct.
      constructor();

      // Return gid.
      getGid(): number;
      // Return sprite.
      getSprite(): Sprite2D;
      // Return Object Group.
      getObjectGroup(): TmxObjectGroup2D;
      // Return has property.
      hasProperty(name: string): boolean;
      // Return property.
      getProperty(name: string): string;

   }

   export class TileMapObject2D extends RefCounted {

      objectType: TileMapObjectType2D;
      name: string;
      type: string;
      position: Vector2;
      size: Vector2;
      numPoints: number;
      tileGid: number;
      tileSprite: Sprite2D;

      constructor();

      // Return type.
      getObjectType(): TileMapObjectType2D;
      // Return name.
      getName(): string;
      // Return type.
      getType(): string;
      // Return position.
      getPosition(): Vector2;
      // Return size (for rectangle and ellipse).
      getSize(): Vector2;
      // Return number of points (use for script).
      getNumPoints(): number;
      // Return point at index (use for script).
      getPoint(index: number): Vector2;
      // Return tile Gid.
      getTileGid(): number;
      // Return tile sprite.
      getTileSprite(): Sprite2D;
      // Return has property.
      hasProperty(name: string): boolean;
      // Return property value.
      getProperty(name: string): string;
      validCollisionShape(): boolean;
      createCollisionShape(node: Node): CollisionShape2D;

   }

   export class TmxLayer2D extends RefCounted {

      tmxFile: TmxFile2D;
      type: TileMapLayerType2D;
      name: string;
      width: number;
      height: number;

      constructor(tmxFile: TmxFile2D, type: TileMapLayerType2D);

      // Return tmx file.
      getTmxFile(): TmxFile2D;
      // Return type.
      getType(): TileMapLayerType2D;
      // Return name.
      getName(): string;
      // Return width.
      getWidth(): number;
      // Return height.
      getHeight(): number;
      // Return is visible.
      isVisible(): boolean;
      // Return has property (use for script).
      hasProperty(name: string): boolean;
      // Return property value (use for script).
      getProperty(name: string): string;

   }

   export class TmxTileLayer2D extends TmxLayer2D {

      constructor(tmxFile: TmxFile2D);

      // Return tile.
      getTile(x: number, y: number): Tile2D;

   }

   export class TmxObjectGroup2D extends TmxLayer2D {

      numObjects: number;

      constructor(tmxFile: TmxFile2D);

      // Return number of objects.
      getNumObjects(): number;
      // Return tile map object at index.
      getObject(index: number): TileMapObject2D;

   }

   export class TmxImageLayer2D extends TmxLayer2D {

      position: Vector2;
      source: string;
      sprite: Sprite2D;

      constructor(tmxFile: TmxFile2D);

      // Return position.
      getPosition(): Vector2;
      // Return source.
      getSource(): string;
      // Return sprite.
      getSprite(): Sprite2D;

   }

   export class TmxFile2D extends Resource {

      numLayers: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Return tile sprite by gid, if not exist return 0.
      getTileSprite(gid: number): Sprite2D;
      // Return tile property set by gid, if not exist return 0.
      getTilePropertySet(gid: number): PropertySet2D;
      // Return tile object group by gid, if not exist return 0.
      getTileObjectGroup(gid: number): TmxObjectGroup2D;
      // Return number of layers.
      getNumLayers(): number;
      // Return layer at index.
      getLayer(index: number): TmxLayer2D;

   }



//----------------------------------------------------
// MODULE: Audio
//----------------------------------------------------


   export class Sound extends Resource {

      size: number;
      looped: boolean;
      length: number;
      dataSize: number;
      sampleSize: number;
      frequency: number;
      intFrequency: number;

      // Construct.
      constructor();

      // Set sound size in bytes. Also resets the sound to be uncompressed and one-shot.
      setSize(dataSize: number): void;
      // Set uncompressed sound data format.
      setFormat(frequency: number, sixteenBit: boolean, stereo: boolean): void;
      // Set loop on/off. If loop is enabled, sets the full sound as loop range.
      setLooped(enable: boolean): void;
      // Define loop.
      setLoop(repeatOffset: number, endOffset: number): void;
      // Return length in seconds.
      getLength(): number;
      // Return total sound data size.
      getDataSize(): number;
      // Return sample size.
      getSampleSize(): number;
      // Return default frequency as a float.
      getFrequency(): number;
      // Return default frequency as an integer.
      getIntFrequency(): number;
      // Return whether is looped.
      isLooped(): boolean;
      // Return whether data is sixteen bit.
      isSixteenBit(): boolean;
      // Return whether data is stereo.
      isStereo(): boolean;
      // Return whether is compressed.
      isCompressed(): boolean;
      // Fix interpolation by copying data from loop start to loop end (looped), or adding silence (oneshot.) Called internally, does not normally need to be called, unless the sound data is modified manually on the fly.
      fixInterpolation(): void;

   }

   export class SoundSource extends Component {

      soundType: string;
      frequency: number;
      gain: number;
      attenuation: number;
      panning: number;
      autoRemove: boolean;
      sound: Sound;
      timePosition: number;
      positionAttr: number;
      playingAttr: boolean;

      // Construct.
      constructor();

      // Play a sound.
      play(sound: Sound): void;
      // Stop playback.
      stop(): void;
      // Set sound type, determines the master gain group.
      setSoundType(type: string): void;
      // Set frequency.
      setFrequency(frequency: number): void;
      // Set gain. 0.0 is silence, 1.0 is full volume.
      setGain(gain: number): void;
      // Set attenuation. 1.0 is unaltered. Used for distance attenuated playback.
      setAttenuation(attenuation: number): void;
      // Set stereo panning. -1.0 is full left and 1.0 is full right.
      setPanning(panning: number): void;
      // Set whether sound source will be automatically removed from the scene node when playback stops.
      setAutoRemove(enable: boolean): void;
      // Return sound.
      setSound(sound: Sound): void;
      // Return sound.
      getSound(): Sound;
      // Return sound type, determines the master gain group.
      getSoundType(): string;
      // Return playback time position.
      getTimePosition(): number;
      // Return frequency.
      getFrequency(): number;
      // Return gain.
      getGain(): number;
      // Return attenuation.
      getAttenuation(): number;
      // Return stereo panning.
      getPanning(): number;
      // Return autoremove mode.
      getAutoRemove(): boolean;
      // Return whether is playing.
      isPlaying(): boolean;
      // Update the sound source. Perform subclass specific operations. Called by Audio.
      update(timeStep: number): void;
      // Update the effective master gain. Called internally and by Audio when the master gain changes.
      updateMasterGain(): void;
      // Set sound position attribute.
      setPositionAttr(value: number): void;
      // Set sound playing attribute
      setPlayingAttr(value: boolean): void;
      // Return sound position attribute.
      getPositionAttr(): number;

   }



//----------------------------------------------------
// MODULE: Physics
//----------------------------------------------------


   export class Constraint extends Component {

      constraintType: ConstraintType;
      otherBody: RigidBody;
      position: Vector3;
      rotation: Quaternion;
      axis: Vector3;
      otherPosition: Vector3;
      otherRotation: Quaternion;
      otherAxis: Vector3;
      worldPosition: Vector3;
      highLimit: Vector2;
      lowLimit: Vector2;
      erp: number;
      cfm: number;
      disableCollision: boolean;
      physicsWorld: PhysicsWorld;
      ownBody: RigidBody;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set constraint type and recreate the constraint.
      setConstraintType(type: ConstraintType): void;
      // Set other body to connect to. Set to null to connect to the static world.
      setOtherBody(body: RigidBody): void;
      // Set constraint position relative to own body.
      setPosition(position: Vector3): void;
      // Set constraint rotation relative to own body.
      setRotation(rotation: Quaternion): void;
      // Set constraint rotation relative to own body by specifying the axis.
      setAxis(axis: Vector3): void;
      // Set constraint position relative to the other body. If connected to the static world, is a world space position.
      setOtherPosition(position: Vector3): void;
      // Set constraint rotation relative to the other body. If connected to the static world, is a world space rotation.
      setOtherRotation(rotation: Quaternion): void;
      // Set constraint rotation relative to the other body by specifying the axis.
      setOtherAxis(axis: Vector3): void;
      // Set constraint world space position. Resets both own and other body relative position, ie. zeroes the constraint error.
      setWorldPosition(position: Vector3): void;
      // Set high limit. Interpretation is constraint type specific.
      setHighLimit(limit: Vector2): void;
      // Set low limit. Interpretation is constraint type specific.
      setLowLimit(limit: Vector2): void;
      // Set constraint error reduction parameter. Zero = leave to default.
      setERP(erp: number): void;
      // Set constraint force mixing parameter. Zero = leave to default.
      setCFM(cfm: number): void;
      // Set whether to disable collisions between connected bodies.
      setDisableCollision(disable: boolean): void;
      // Return physics world.
      getPhysicsWorld(): PhysicsWorld;
      // Return constraint type.
      getConstraintType(): ConstraintType;
      // Return rigid body in own scene node.
      getOwnBody(): RigidBody;
      // Return the other rigid body. May be null if connected to the static world.
      getOtherBody(): RigidBody;
      // Return constraint position relative to own body.
      getPosition(): Vector3;
      // Return constraint rotation relative to own body.
      getRotation(): Quaternion;
      // Return constraint position relative to other body.
      getOtherPosition(): Vector3;
      // Return constraint rotation relative to other body.
      getOtherRotation(): Quaternion;
      // Return constraint world position, calculated from own body.
      getWorldPosition(): Vector3;
      // Return high limit.
      getHighLimit(): Vector2;
      // Return low limit.
      getLowLimit(): Vector2;
      // Return constraint error reduction parameter.
      getERP(): number;
      // Return constraint force mixing parameter.
      getCFM(): number;
      // Return whether collisions between connected bodies are disabled.
      getDisableCollision(): boolean;
      // Release the constraint.
      releaseConstraint(): void;
      // Apply constraint frames.
      applyFrames(): void;

   }

   export class PhysicsWorld extends Component {

      fps: number;
      gravity: Vector3;
      maxSubSteps: number;
      numIterations: number;
      interpolation: boolean;
      internalEdge: boolean;
      splitImpulse: boolean;
      maxNetworkAngularVelocity: number;
      debugRenderer: DebugRenderer;
      debugDepthTest: boolean;
      applyingTransforms: boolean;

      // Construct.
      constructor();

      // Set debug draw flags.
      setDebugMode(debugMode: number): void;
      // Return debug draw flags.
      getDebugMode(): number;
      // Step the simulation forward.
      update(timeStep: number): void;
      // Refresh collisions only without updating dynamics.
      updateCollisions(): void;
      // Set simulation substeps per second.
      setFps(fps: number): void;
      // Set gravity.
      setGravity(gravity: Vector3): void;
      // Set maximum number of physics substeps per frame. 0 (default) is unlimited. Positive values cap the amount. Use a negative value to enable an adaptive timestep. This may cause inconsistent physics behavior.
      setMaxSubSteps(num: number): void;
      // Set number of constraint solver iterations.
      setNumIterations(num: number): void;
      // Set whether to interpolate between simulation steps.
      setInterpolation(enable: boolean): void;
      // Set whether to use Bullet's internal edge utility for trimesh collisions. Disabled by default.
      setInternalEdge(enable: boolean): void;
      // Set split impulse collision mode. This is more accurate, but slower. Disabled by default.
      setSplitImpulse(enable: boolean): void;
      // Set maximum angular velocity for network replication.
      setMaxNetworkAngularVelocity(velocity: number): void;
      // Invalidate cached collision geometry for a model.
      removeCachedGeometry(model: Model): void;
      // Return gravity.
      getGravity(): Vector3;
      // Return maximum number of physics substeps per frame.
      getMaxSubSteps(): number;
      // Return number of constraint solver iterations.
      getNumIterations(): number;
      // Return whether interpolation between simulation steps is enabled.
      getInterpolation(): boolean;
      // Return whether Bullet's internal edge utility for trimesh collisions is enabled.
      getInternalEdge(): boolean;
      // Return whether split impulse collision mode is enabled.
      getSplitImpulse(): boolean;
      // Return simulation steps per second.
      getFps(): number;
      // Return maximum angular velocity for network replication.
      getMaxNetworkAngularVelocity(): number;
      // Add a rigid body to keep track of. Called by RigidBody.
      addRigidBody(body: RigidBody): void;
      // Remove a rigid body. Called by RigidBody.
      removeRigidBody(body: RigidBody): void;
      // Add a collision shape to keep track of. Called by CollisionShape.
      addCollisionShape(shape: CollisionShape): void;
      // Remove a collision shape. Called by CollisionShape.
      removeCollisionShape(shape: CollisionShape): void;
      // Add a constraint to keep track of. Called by Constraint.
      addConstraint(joint: Constraint): void;
      // Remove a constraint. Called by Constraint.
      removeConstraint(joint: Constraint): void;
      // Set debug renderer to use. Called both by PhysicsWorld itself and physics components.
      setDebugRenderer(debug: DebugRenderer): void;
      // Set debug geometry depth test mode. Called both by PhysicsWorld itself and physics components.
      setDebugDepthTest(enable: boolean): void;
      // Clean up the geometry cache.
      cleanupGeometryCache(): void;
      // Set node dirtying to be disregarded.
      setApplyingTransforms(enable: boolean): void;
      // Return whether node dirtying should be disregarded.
      isApplyingTransforms(): boolean;

   }

   export class RigidBody extends Component {

      mass: number;
      position: Vector3;
      rotation: Quaternion;
      linearVelocity: Vector3;
      linearFactor: Vector3;
      linearRestThreshold: number;
      linearDamping: number;
      angularVelocity: Vector3;
      angularFactor: Vector3;
      angularRestThreshold: number;
      angularDamping: number;
      friction: number;
      anisotropicFriction: Vector3;
      rollingFriction: number;
      restitution: number;
      contactProcessingThreshold: number;
      ccdRadius: number;
      ccdMotionThreshold: number;
      useGravity: boolean;
      gravityOverride: Vector3;
      kinematic: boolean;
      trigger: boolean;
      collisionLayer: number;
      collisionMask: number;
      collisionEventMode: CollisionEventMode;
      physicsWorld: PhysicsWorld;
      centerOfMass: Vector3;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set mass. Zero mass makes the body static.
      setMass(mass: number): void;
      // Set rigid body position in world space.
      setPosition(position: Vector3): void;
      // Set rigid body rotation in world space.
      setRotation(rotation: Quaternion): void;
      // Set rigid body position and rotation in world space as an atomic operation.
      setTransform(position: Vector3, rotation: Quaternion): void;
      // Set linear velocity.
      setLinearVelocity(velocity: Vector3): void;
      // Set linear degrees of freedom. Use 1 to enable an axis or 0 to disable. Default is all axes enabled (1, 1, 1).
      setLinearFactor(factor: Vector3): void;
      // Set linear velocity deactivation threshold.
      setLinearRestThreshold(threshold: number): void;
      // Set linear velocity damping factor.
      setLinearDamping(damping: number): void;
      // Set angular velocity.
      setAngularVelocity(angularVelocity: Vector3): void;
      // Set angular degrees of freedom. Use 1 to enable an axis or 0 to disable. Default is all axes enabled (1, 1, 1).
      setAngularFactor(factor: Vector3): void;
      // Set angular velocity deactivation threshold.
      setAngularRestThreshold(threshold: number): void;
      // Set angular velocity damping factor.
      setAngularDamping(factor: number): void;
      // Set friction coefficient.
      setFriction(friction: number): void;
      // Set anisotropic friction.
      setAnisotropicFriction(friction: Vector3): void;
      // Set rolling friction coefficient.
      setRollingFriction(friction: number): void;
      // Set restitution coefficient.
      setRestitution(restitution: number): void;
      // Set contact processing threshold.
      setContactProcessingThreshold(threshold: number): void;
      // Set continuous collision detection swept sphere radius.
      setCcdRadius(radius: number): void;
      // Set continuous collision detection motion-per-simulation-step threshold. 0 disables, which is the default.
      setCcdMotionThreshold(threshold: number): void;
      // Set whether gravity is applied to rigid body.
      setUseGravity(enable: boolean): void;
      // Set gravity override. If zero, uses physics world's gravity.
      setGravityOverride(gravity: Vector3): void;
      // Set rigid body kinematic mode. In kinematic mode forces are not applied to the rigid body.
      setKinematic(enable: boolean): void;
      // Set rigid body trigger mode. In trigger mode collisions are reported but do not apply forces.
      setTrigger(enable: boolean): void;
      // Set collision layer.
      setCollisionLayer(layer: number): void;
      // Set collision mask.
      setCollisionMask(mask: number): void;
      // Set collision group and mask.
      setCollisionLayerAndMask(layer: number, mask: number): void;
      // Set collision event signaling mode. Default is to signal when rigid bodies are active.
      setCollisionEventMode(mode: CollisionEventMode): void;
      // Apply torque.
      applyTorque(torque: Vector3): void;
      // Apply impulse to center of mass.
      applyImpulse(impulse: Vector3): void;
      // Apply torque impulse.
      applyTorqueImpulse(torque: Vector3): void;
      // Reset accumulated forces.
      resetForces(): void;
      // Activate rigid body if it was resting.
      activate(): void;
      // Readd rigid body to the physics world to clean up internal state like stale contacts.
      reAddBodyToWorld(): void;
      // Disable mass update. Call this to optimize performance when adding or editing multiple collision shapes in the same node.
      disableMassUpdate(): void;
      // Re-enable mass update and recalculate the mass/inertia by calling UpdateMass(). Call when collision shape changes are finished.
      enableMassUpdate(): void;
      // Return physics world.
      getPhysicsWorld(): PhysicsWorld;
      // Return mass.
      getMass(): number;
      // Return rigid body position in world space.
      getPosition(): Vector3;
      // Return rigid body rotation in world space.
      getRotation(): Quaternion;
      // Return linear velocity.
      getLinearVelocity(): Vector3;
      // Return linear degrees of freedom.
      getLinearFactor(): Vector3;
      // Return linear velocity at local point.
      getVelocityAtPoint(position: Vector3): Vector3;
      // Return linear velocity deactivation threshold.
      getLinearRestThreshold(): number;
      // Return linear velocity damping factor.
      getLinearDamping(): number;
      // Return angular velocity.
      getAngularVelocity(): Vector3;
      // Return angular degrees of freedom.
      getAngularFactor(): Vector3;
      // Return angular velocity deactivation threshold.
      getAngularRestThreshold(): number;
      // Return angular velocity damping factor.
      getAngularDamping(): number;
      // Return friction coefficient.
      getFriction(): number;
      // Return anisotropic friction.
      getAnisotropicFriction(): Vector3;
      // Return rolling friction coefficient.
      getRollingFriction(): number;
      // Return restitution coefficient.
      getRestitution(): number;
      // Return contact processing threshold.
      getContactProcessingThreshold(): number;
      // Return continuous collision detection swept sphere radius.
      getCcdRadius(): number;
      // Return continuous collision detection motion-per-simulation-step threshold.
      getCcdMotionThreshold(): number;
      // Return whether rigid body uses gravity.
      getUseGravity(): boolean;
      // Return gravity override. If zero (default), uses the physics world's gravity.
      getGravityOverride(): Vector3;
      // Return center of mass offset.
      getCenterOfMass(): Vector3;
      // Return kinematic mode flag.
      isKinematic(): boolean;
      // Return whether this RigidBody is acting as a trigger.
      isTrigger(): boolean;
      // Return whether rigid body is active (not sleeping.)
      isActive(): boolean;
      // Return collision layer.
      getCollisionLayer(): number;
      // Return collision mask.
      getCollisionMask(): number;
      // Return collision event signaling mode.
      getCollisionEventMode(): CollisionEventMode;
      // Apply new world transform after a simulation step. Called internally.
      applyWorldTransform(newWorldPosition: Vector3, newWorldRotation: Quaternion): void;
      // Update mass and inertia to the Bullet rigid body.
      updateMass(): void;
      // Update gravity parameters to the Bullet rigid body.
      updateGravity(): void;
      // Add a constraint that refers to this rigid body.
      addConstraint(constraint: Constraint): void;
      // Remove a constraint that refers to this rigid body.
      removeConstraint(constraint: Constraint): void;
      // Remove the rigid body.
      releaseBody(): void;

   }

   export class CollisionShape extends Component {

      terrain: number;
      shapeType: ShapeType;
      size: Vector3;
      position: Vector3;
      rotation: Quaternion;
      margin: number;
      model: Model;
      lodLevel: number;
      physicsWorld: PhysicsWorld;
      worldBoundingBox: BoundingBox;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Handle enabled/disabled state change.
      onSetEnabled(): void;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set as a box.
      setBox(size: Vector3, position?: Vector3, rotation?: Quaternion): void;
      // Set as a sphere.
      setSphere(diameter: number, position?: Vector3, rotation?: Quaternion): void;
      // Set as a static plane.
      setStaticPlane(position?: Vector3, rotation?: Quaternion): void;
      // Set as a cylinder.
      setCylinder(diameter: number, height: number, position?: Vector3, rotation?: Quaternion): void;
      // Set as a capsule.
      setCapsule(diameter: number, height: number, position?: Vector3, rotation?: Quaternion): void;
      // Set as a cone.
      setCone(diameter: number, height: number, position?: Vector3, rotation?: Quaternion): void;
      // Set as a triangle mesh from Model. If you update a model's geometry and want to reapply the shape, call physicsWorld->RemoveCachedGeometry(model) first.
      setTriangleMesh(model: Model, lodLevel?: number, scale?: Vector3, position?: Vector3, rotation?: Quaternion): void;
      // Set as a triangle mesh from CustomGeometry.
      setCustomTriangleMesh(custom: CustomGeometry, scale?: Vector3, position?: Vector3, rotation?: Quaternion): void;
      // Set as a convex hull from Model.
      setConvexHull(model: Model, lodLevel?: number, scale?: Vector3, position?: Vector3, rotation?: Quaternion): void;
      // Set as a convex hull from CustomGeometry.
      setCustomConvexHull(custom: CustomGeometry, scale?: Vector3, position?: Vector3, rotation?: Quaternion): void;
      // Set as a terrain. Only works if the same scene node contains a Terrain component.
      setTerrain(lodLevel?: number): void;
      // Set shape type.
      setShapeType(type: ShapeType): void;
      // Set shape size.
      setSize(size: Vector3): void;
      // Set offset position.
      setPosition(position: Vector3): void;
      // Set offset rotation.
      setRotation(rotation: Quaternion): void;
      // Set offset transform.
      setTransform(position: Vector3, rotation: Quaternion): void;
      // Set collision margin.
      setMargin(margin: number): void;
      // Set triangle mesh / convex hull model.
      setModel(model: Model): void;
      // Set model LOD level.
      setLodLevel(lodLevel: number): void;
      // Return physics world.
      getPhysicsWorld(): PhysicsWorld;
      // Return shape type.
      getShapeType(): ShapeType;
      // Return shape size.
      getSize(): Vector3;
      // Return offset position.
      getPosition(): Vector3;
      // Return offset rotation.
      getRotation(): Quaternion;
      // Return collision margin.
      getMargin(): number;
      // Return triangle mesh / convex hull model.
      getModel(): Model;
      // Return model LOD level.
      getLodLevel(): number;
      // Return world-space bounding box.
      getWorldBoundingBox(): BoundingBox;
      // Update the new collision shape to the RigidBody.
      notifyRigidBody(updateMass?: boolean): void;
      // Release the collision shape.
      releaseShape(): void;

   }



//----------------------------------------------------
// MODULE: Navigation
//----------------------------------------------------


   export class NavigationMesh extends Component {

      tileSize: number;
      cellSize: number;
      cellHeight: number;
      agentHeight: number;
      agentRadius: number;
      agentMaxClimb: number;
      agentMaxSlope: number;
      regionMinSize: number;
      regionMergeSize: number;
      edgeMaxLength: number;
      edgeMaxError: number;
      detailSampleDistance: number;
      detailSampleMaxError: number;
      padding: Vector3;
      randomPoint: Vector3;
      meshName: string;
      boundingBox: BoundingBox;
      worldBoundingBox: BoundingBox;
      numTiles: IntVector2;
      partitionType: NavmeshPartitionType;
      drawOffMeshConnections: boolean;
      drawNavAreas: boolean;

      // Construct.
      constructor();

      // Set tile size.
      setTileSize(size: number): void;
      // Set cell size.
      setCellSize(size: number): void;
      // Set cell height.
      setCellHeight(height: number): void;
      // Set navigation agent height.
      setAgentHeight(height: number): void;
      // Set navigation agent radius.
      setAgentRadius(radius: number): void;
      // Set navigation agent max vertical climb.
      setAgentMaxClimb(maxClimb: number): void;
      // Set navigation agent max slope.
      setAgentMaxSlope(maxSlope: number): void;
      // Set region minimum size.
      setRegionMinSize(size: number): void;
      // Set region merge size.
      setRegionMergeSize(size: number): void;
      // Set edge max length.
      setEdgeMaxLength(length: number): void;
      // Set edge max error.
      setEdgeMaxError(error: number): void;
      // Set detail sampling distance.
      setDetailSampleDistance(distance: number): void;
      // Set detail sampling maximum error.
      setDetailSampleMaxError(error: number): void;
      // Set padding of the navigation mesh bounding box. Having enough padding allows to add geometry on the extremities of the navigation mesh when doing partial rebuilds.
      setPadding(padding: Vector3): void;
      // Set the cost of an area.
      setAreaCost(areaID: number, cost: number): void;
      // Find the nearest point on the navigation mesh to a given point. Extens specifies how far out from the specified point to check along each axis.
      findNearestPoint(point: Vector3, extents?: Vector3): Vector3;
      // Try to move along the surface from one point to another.
      moveAlongSurface(start: Vector3, end: Vector3, extents?: Vector3, maxVisited?: number): Vector3;
      // Return a random point on the navigation mesh.
      getRandomPoint(): Vector3;
      // Return a random point on the navigation mesh within a circle. The circle radius is only a guideline and in practice the returned point may be further away.
      getRandomPointInCircle(center: Vector3, radius: number, extents?: Vector3): Vector3;
      // Return distance to wall from a point. Maximum search radius must be specified.
      getDistanceToWall(point: Vector3, radius: number, extents?: Vector3): number;
      // Perform a walkability raycast on the navigation mesh between start and end and return the point where a wall was hit, or the end point if no walls.
      raycast(start: Vector3, end: Vector3, extents?: Vector3): Vector3;
      // Return the given name of this navigation mesh.
      getMeshName(): string;
      // Set the name of this navigation mesh.
      setMeshName(newName: string): void;
      // Return tile size.
      getTileSize(): number;
      // Return cell size.
      getCellSize(): number;
      // Return cell height.
      getCellHeight(): number;
      // Return navigation agent height.
      getAgentHeight(): number;
      // Return navigation agent radius.
      getAgentRadius(): number;
      // Return navigation agent max vertical climb.
      getAgentMaxClimb(): number;
      // Return navigation agent max slope.
      getAgentMaxSlope(): number;
      // Return region minimum size.
      getRegionMinSize(): number;
      // Return region merge size.
      getRegionMergeSize(): number;
      // Return edge max length.
      getEdgeMaxLength(): number;
      // Return edge max error.
      getEdgeMaxError(): number;
      // Return detail sampling distance.
      getDetailSampleDistance(): number;
      // Return detail sampling maximum error.
      getDetailSampleMaxError(): number;
      // Return navigation mesh bounding box padding.
      getPadding(): Vector3;
      // Get the current cost of an area
      getAreaCost(areaID: number): number;
      // Return whether has been initialized with valid navigation data.
      isInitialized(): boolean;
      // Return local space bounding box of the navigation mesh.
      getBoundingBox(): BoundingBox;
      // Return world space bounding box of the navigation mesh.
      getWorldBoundingBox(): BoundingBox;
      // Return number of tiles.
      getNumTiles(): IntVector2;
      // Set the partition type used for polygon generation.
      setPartitionType(aType: NavmeshPartitionType): void;
      // Return Partition Type.
      getPartitionType(): NavmeshPartitionType;
      // Draw debug geometry for OffMeshConnection components.
      setDrawOffMeshConnections(enable: boolean): void;
      // Return whether to draw OffMeshConnection components.
      getDrawOffMeshConnections(): boolean;
      // Draw debug geometry for NavArea components.
      setDrawNavAreas(enable: boolean): void;
      // Return whether to draw NavArea components.
      getDrawNavAreas(): boolean;

   }

   export class OffMeshConnection extends Component {

      endPoint: Node;
      radius: number;
      bidirectional: boolean;
      mask: number;
      areaID: number;

      // Construct.
      constructor();

      // Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
      applyAttributes(): void;
      // Visualize the component as debug geometry.
      drawDebugGeometry(debug: DebugRenderer, depthTest: boolean): void;
      // Set endpoint node.
      setEndPoint(node: Node): void;
      // Set radius.
      setRadius(radius: number): void;
      // Set bidirectional flag. Default true.
      setBidirectional(enabled: boolean): void;
      // Set a user assigned mask
      setMask(newMask: number): void;
      // Sets the assigned area Id for the connection
      setAreaID(newAreaID: number): void;
      // Return endpoint node.
      getEndPoint(): Node;
      // Return radius.
      getRadius(): number;
      // Return whether is bidirectional.
      isBidirectional(): boolean;
      // Return the user assigned mask
      getMask(): number;
      // Return the user assigned area ID
      getAreaID(): number;

   }

   export class Navigable extends Component {

      recursive: boolean;

      // Construct.
      constructor();

      // Set whether geometry is automatically collected from child nodes. Default true.
      setRecursive(enable: boolean): void;
      // Return whether geometry is automatically collected from child nodes.
      isRecursive(): boolean;

   }



//----------------------------------------------------
// MODULE: Input
//----------------------------------------------------


   export class Input extends AObject {

      toggleFullscreen: boolean;
      mouseGrabbed: boolean;
      mouseMode: MouseMode;
      screenKeyboardVisible: boolean;
      touchEmulation: boolean;
      qualifiers: number;
      mousePosition: IntVector2;
      mouseMove: IntVector2;
      mouseMoveX: number;
      mouseMoveY: number;
      mouseMoveWheel: number;
      numTouches: number;
      numJoysticks: number;
      screenKeyboardSupport: boolean;

      // Construct.
      constructor();

      // Poll for window messages. Called by HandleBeginFrame().
      update(): void;
      // Set whether ALT-ENTER fullscreen toggle is enabled.
      setToggleFullscreen(enable: boolean): void;
      // Set whether the operating system mouse cursor is visible. When not visible (default), is kept centered to prevent leaving the window. Mouse visibility event can be suppressed-- this also recalls any unsuppressed SetMouseVisible which can be returned by ResetMouseVisible().
      setMouseVisible(enable: boolean, suppressEvent?: boolean): void;
      // Reset last mouse visibility that was not suppressed in SetMouseVisible.
      resetMouseVisible(): void;
      // Set whether the mouse is currently being grabbed by an operation.
      setMouseGrabbed(grab: boolean): void;
      setMouseMode(mode: MouseMode): void;
      // Show or hide on-screen keyboard on platforms that support it. When shown, keypresses from it are delivered as key events.
      setScreenKeyboardVisible(enable: boolean): void;
      // Set touch emulation by mouse. Only available on desktop platforms. When enabled, actual mouse events are no longer sent and the mouse cursor is forced visible.
      setTouchEmulation(enable: boolean): void;
      // Begin recording a touch gesture. Return true if successful. The E_GESTURERECORDED event (which contains the ID for the new gesture) will be sent when recording finishes.
      recordGesture(): boolean;
      // Remove an in-memory gesture by ID. Return true if was found.
      removeGesture(gestureID: number): boolean;
      // Remove all in-memory gestures.
      removeAllGestures(): void;
      // Return keycode from key name.
      getKeyFromName(name: string): number;
      // Return keycode from scancode.
      getKeyFromScancode(scancode: number): number;
      // Return name of key from keycode.
      getKeyName(key: number): string;
      // Return scancode from keycode.
      getScancodeFromKey(key: number): number;
      // Return scancode from key name.
      getScancodeFromName(name: string): number;
      // Return name of key from scancode.
      getScancodeName(scancode: number): string;
      // Check if a key is held down.
      getKeyDown(key: number): boolean;
      // Check if a key has been pressed on this frame.
      getKeyPress(key: number): boolean;
      // Check if a key is held down by scancode.
      getScancodeDown(scancode: number): boolean;
      // Check if a key has been pressed on this frame by scancode.
      getScancodePress(scancode: number): boolean;
      // Check if a mouse button is held down.
      getMouseButtonDown(button: number): boolean;
      // Check if a mouse button has been pressed on this frame.
      getMouseButtonPress(button: number): boolean;
      // Check if a qualifier key is held down.
      getQualifierDown(qualifier: number): boolean;
      // Check if a qualifier key has been pressed on this frame.
      getQualifierPress(qualifier: number): boolean;
      // Return the currently held down qualifiers.
      getQualifiers(): number;
      // Return mouse position within window. Should only be used with a visible mouse cursor.
      getMousePosition(): IntVector2;
      // Return mouse movement since last frame.
      getMouseMove(): IntVector2;
      // Return horizontal mouse movement since last frame.
      getMouseMoveX(): number;
      // Return vertical mouse movement since last frame.
      getMouseMoveY(): number;
      // Return mouse wheel movement since last frame.
      getMouseMoveWheel(): number;
      // Return number of active finger touches.
      getNumTouches(): number;
      // Return number of connected joysticks.
      getNumJoysticks(): number;
      // Return whether fullscreen toggle is enabled.
      getToggleFullscreen(): boolean;
      // Return whether on-screen keyboard is supported.
      getScreenKeyboardSupport(): boolean;
      // Return whether on-screen keyboard is being shown.
      isScreenKeyboardVisible(): boolean;
      // Return whether touch emulation is enabled.
      getTouchEmulation(): boolean;
      // Return whether the operating system mouse cursor is visible.
      isMouseVisible(): boolean;
      // Return whether the mouse is currently being grabbed by an operation.
      isMouseGrabbed(): boolean;
      // Return the mouse mode.
      getMouseMode(): MouseMode;
      // Return whether application window has input focus.
      hasFocus(): boolean;
      // Return whether application window is minimized.
      isMinimized(): boolean;

   }



//----------------------------------------------------
// MODULE: UI
//----------------------------------------------------


   export class UIButton extends UIWidget {

      squeezable: boolean;

      constructor(createWidget?: boolean);

      setSqueezable(value: boolean): void;
      onClick: () => void;

   }

   export class UICheckBox extends UIWidget {

      constructor(createWidget?: boolean);


   }

   export class UIClickLabel extends UIWidget {

      constructor(createWidget?: boolean);


   }

   export class UIFontDescription extends AObject {

      id: string;
      size: number;

      constructor();

      setId(id: string): void;
      setSize(size: number): void;

   }

   export class UISkinImage extends UIWidget {

      constructor(bitmapID: string, createWidget?: boolean);


   }

   export class UITabContainer extends UIWidget {

      numPages: number;
      currentPage: number;
      currentPageWidget: UIWidget;

      constructor(createWidget?: boolean);

      getNumPages(): number;
      setCurrentPage(page: number): void;
      getCurrentPageWidget(): UIWidget;

   }

   export class UIView extends UIWidget {

      constructor();


   }

   export class UIContainer extends UIWidget {

      constructor(createWidget?: boolean);


   }

   export class UIDimmer extends UIWidget {

      constructor(createWidget?: boolean);


   }

   export class UIDragObject extends AObject {

      text: string;
      icon: string;
      object: AObject;
      filenames: string[];

      // Construct.
      constructor(object?: AObject, text?: string, icon?: string);

      getText(): string;
      getIcon(): string;
      getObject(): AObject;
      getFilenames(): string[];
      setText(text: string): void;
      setIcon(icon: string): void;
      setObject(object: AObject): void;
      addFilename(filename: string): void;

   }

   export class UIEditField extends UIWidget {

      textAlign: TEXT_ALIGN;
      editType: UI_EDIT_TYPE;
      readOnly: boolean;
      wrapping: boolean;

      constructor(createWidget?: boolean);

      appendText(text: string): void;
      setTextAlign(align: TEXT_ALIGN): void;
      setEditType(type: UI_EDIT_TYPE): void;
      setReadOnly(readonly: boolean): void;
      scrollTo(x: number, y: number): void;
      setWrapping(wrap: boolean): void;
      getWrapping(): boolean;

   }

   export class UIImageWidget extends UIWidget {

      image: string;

      constructor(createWidget?: boolean);

      setImage(imagePath: string): void;

   }

   export class UIInlineSelect extends UIWidget {

      editFieldLayoutParams: UILayoutParams;

      constructor(createWidget?: boolean);

      setLimits(minimum: number, maximum: number): void;
      setEditFieldLayoutParams(params: UILayoutParams): void;

   }

   export class UILayoutParams extends AObject {

      width: number;
      height: number;
      minWidth: number;
      minHeight: number;
      maxWidth: number;
      maxHeight: number;

      constructor();

      setWidth(width: number): void;
      setHeight(height: number): void;
      setMinWidth(width: number): void;
      setMinHeight(height: number): void;
      setMaxWidth(width: number): void;
      setMaxHeight(height: number): void;

   }

   export class UILayout extends UIWidget {

      spacing: number;
      axis: UI_AXIS;
      layoutSize: UI_LAYOUT_SIZE;
      layoutPosition: UI_LAYOUT_POSITION;
      layoutDistribution: UI_LAYOUT_DISTRIBUTION;
      layoutDistributionPosition: UI_LAYOUT_DISTRIBUTION_POSITION;

      constructor(axis?: UI_AXIS, createWidget?: boolean);

      setSpacing(spacing: number): void;
      setAxis(axis: UI_AXIS): void;
      setLayoutSize(size: UI_LAYOUT_SIZE): void;
      setLayoutPosition(position: UI_LAYOUT_POSITION): void;
      setLayoutDistribution(distribution: UI_LAYOUT_DISTRIBUTION): void;
      setLayoutDistributionPosition(distribution_pos: UI_LAYOUT_DISTRIBUTION_POSITION): void;

   }

   export class UIListView extends UIWidget {

      hoverItemID: string;
      selectedItemID: string;
      rootList: UISelectList;

      // Construct.
      constructor(createWidget?: boolean);

      addRootItem(text: string, icon: string, id: string): number;
      addChildItem(parentItemID: number, text: string, icon: string, id: string): number;
      setItemText(id: string, text: string): void;
      setItemIcon(id: string, icon: string): void;
      deleteItemByID(id: string): void;
      setExpanded(itemID: number, value: boolean): void;
      deleteAllItems(): void;
      selectItemByID(id: string): void;
      getHoverItemID(): string;
      getSelectedItemID(): string;
      getRootList(): UISelectList;

   }

   export class UIMenuWindow extends UIWidget {

      constructor(target: UIWidget, id: string);

      show(source: UISelectItemSource, x?: number, y?: number): void;
      close(): void;

   }

   export class UIMenuItem extends UISelectItem {

      constructor(str?: string, id?: string, shortcut?: string, skinBg?: string);


   }

   export class UIMenuItemSource extends UISelectItemSource {

      constructor();


   }

   export class UIMessageWindow extends UIWindow {

      constructor(target: UIWidget, id: string, createWidget?: boolean);

      show(title: string, message: string, settings: UI_MESSAGEWINDOW_SETTINGS, dimmer: boolean, width: number, height: number): void;

   }

   export class UISceneView extends UIWidget {

      format: number;
      autoUpdate: boolean;
      scene: Scene;
      cameraNode: Node;
      renderTexture: Texture2D;
      depthTexture: Texture2D;
      viewport: Viewport;
      size: IntVector2;

      constructor(createWidget?: boolean);

      // React to resize.
      onResize(newSize: IntVector2): void;
      // Define the scene and camera to use in rendering. When ownScene is true the View3D will take ownership of them with shared pointers.
      setView(scene: Scene, camera: Camera): void;
      // Set render texture pixel format. Default is RGB.
      setFormat(format: number): void;
      // Set render target auto update mode. Default is true.
      setAutoUpdate(enable: boolean): void;
      // Queue manual update on the render texture.
      queueUpdate(): void;
      // Return render texture pixel format.
      getFormat(): number;
      // Return whether render target updates automatically.
      getAutoUpdate(): boolean;
      // Return scene.
      getScene(): Scene;
      // Return camera scene node.
      getCameraNode(): Node;
      // Return render texture.
      getRenderTexture(): Texture2D;
      // Return depth stencil texture.
      getDepthTexture(): Texture2D;
      // Return viewport.
      getViewport(): Viewport;
      setResizeRequired(): void;
      getSize(): IntVector2;

   }

   export class UIScrollContainer extends UIWidget {

      scrollMode: UI_SCROLL_MODE;
      adaptToContentSize: boolean;
      adaptContentSize: boolean;

      constructor(createWidget?: boolean);

      setScrollMode(mode: UI_SCROLL_MODE): void;
      getScrollMode(): UI_SCROLL_MODE;
      // Set to true if the preferred size of this container should adapt to the preferred size of the content. This is disabled by default.
      setAdaptToContentSize(adapt: boolean): void;
      getAdaptToContentSize(): boolean;
      // Set to true if the content should adapt to the available size of this container when it's larger than the preferred size. */
      setAdaptContentSize(adapt: boolean): void;
      getAdaptContentSize(): boolean;

   }

   export class UISection extends UIWidget {

      constructor(createWidget?: boolean);


   }

   export class UISelectItem extends AObject {

      string: string;
      id: string;
      skinImage: string;
      subSource: UISelectItemSource;

      constructor(str?: string, id?: string, skinImage?: string);

      setString(str: string): void;
      setID(id: string): void;
      setSkinImage(skinImage: string): void;
      setSubSource(subSource: UISelectItemSource): void;

   }

   export class UISelectItemSource extends AObject {

      constructor();

      addItem(item: UISelectItem): void;
      clear(): void;

   }

   export class UISelectList extends UIWidget {

      filter: string;
      source: UISelectItemSource;
      value: number;
      hoverItemID: string;
      selectedItemID: string;

      constructor(createWidget?: boolean);

      setFilter(filter: string): void;
      setSource(source: UISelectItemSource): void;
      invalidateList(): void;
      setValue(value: number): void;
      getValue(): number;
      getHoverItemID(): string;
      getSelectedItemID(): string;

   }

   export class UISeparator extends UIWidget {

      constructor(createWidget?: boolean);


   }

   export class UITextField extends UIWidget {

      textAlign: UI_TEXT_ALIGN;

      constructor(createWidget?: boolean);

      setTextAlign(align: UI_TEXT_ALIGN): void;

   }

   export class UITextureWidget extends UIWidget {

      texture: Texture;

      constructor(createWidget?: boolean);

      setTexture(texture: Texture): void;
      getTexture(): Texture;

   }

   export class UIWidget extends AObject {

      id: string;
      parent: UIWidget;
      contentRoot: UIWidget;
      rect: IntRect;
      preferredSize: UIPreferredSize;
      text: string;
      skinBg: string;
      layoutParams: UILayoutParams;
      fontDescription: UIFontDescription;
      gravity: UI_GRAVITY;
      value: number;
      focus: boolean;
      visibility: UI_WIDGET_VISIBILITY;
      stateRaw: number;
      dragObject: UIDragObject;
      firstChild: UIWidget;
      next: UIWidget;
      isFocusable: boolean;
      view: UIView;

      constructor(createWidget?: boolean);

      load(filename: string): boolean;
      getId(): string;
      getParent(): UIWidget;
      getContentRoot(): UIWidget;
      getRect(): IntRect;
      getPreferredSize(): UIPreferredSize;
      getText(): string;
      setRect(r: IntRect): void;
      setSize(width: number, height: number): void;
      setPosition(x: number, y: number): void;
      setText(text: string): void;
      setSkinBg(id: string): void;
      setLayoutParams(params: UILayoutParams): void;
      setFontDescription(fd: UIFontDescription): void;
      removeChild(child: UIWidget, cleanup?: boolean): void;
      deleteAllChildren(): void;
      setId(id: string): void;
      center(): void;
      setGravity(gravity: UI_GRAVITY): void;
      setValue(value: number): void;
      getValue(): number;
      setFocus(): void;
      getFocus(): boolean;
      // Set focus to first widget which accepts it
      setFocusRecursive(): void;
      onFocusChanged(focused: boolean): void;
      setState(state: number, on: boolean): void;
      getState(state: number): boolean;
      setVisibility(visibility: UI_WIDGET_VISIBILITY): void;
      getVisibility(): UI_WIDGET_VISIBILITY;
      setStateRaw(state: number): void;
      getStateRaw(): number;
      invalidate(): void;
      die(): void;
      setDragObject(object: UIDragObject): void;
      getDragObject(): UIDragObject;
      getFirstChild(): UIWidget;
      getNext(): UIWidget;
      isAncestorOf(widget: UIWidget): boolean;
      setIsFocusable(value: boolean): void;
      getWidget(id: string): UIWidget;
      getView(): UIView;
      addChild(child: UIWidget): void;
      // This takes a relative Z and insert the child before or after the given reference widget.*/
      addChildRelative(child: UIWidget, z: UI_WIDGET_Z_REL, reference: UIWidget): void;

   }

   export class UIWindow extends UIWidget {

      settings: UI_WINDOW_SETTINGS;

      constructor(createWidget?: boolean);

      getSettings(): UI_WINDOW_SETTINGS;
      setSettings(settings: UI_WINDOW_SETTINGS): void;
      resizeToFitContent(): void;
      addChild(child: UIWidget): void;
      close(): void;

   }

   export class UIPreferredSize extends RefCounted {

      minWidth: number;
      minHeight: number;
      maxWidth: number;
      maxHeight: number;
      prefWidth: number;
      prefHeight: number;
      sizeDep: UI_SIZE_DEP;

      constructor(w?: number, h?: number);

      getMinWidth(): number;
      getMinHeight(): number;
      getMaxWidth(): number;
      getMaxHeight(): number;
      getPrefWidth(): number;
      getPrefHeight(): number;
      getSizeDep(): UI_SIZE_DEP;
      setMinWidth(w: number): void;
      setMinHeight(h: number): void;
      setMaxWidth(w: number): void;
      setMaxHeight(h: number): void;
      setPrefWidth(w: number): void;
      setPrefHeight(h: number): void;
      setSizeDep(dep: UI_SIZE_DEP): void;

   }



//----------------------------------------------------
// MODULE: Resource
//----------------------------------------------------


   export class Image extends Resource {

      width: number;
      height: number;
      depth: number;
      components: number;
      compressedFormat: CompressedFormat;
      numCompressedLevels: number;
      nextLevel: Image;

      // Construct empty.
      constructor();

      // Flip image horizontally. Return true if successful.
      flipHorizontal(): boolean;
      // Flip image vertically. Return true if successful.
      flipVertical(): boolean;
      // Resize image by bilinear resampling. Return true if successful.
      resize(width: number, height: number): boolean;
      // Clear the image with a color.
      clear(color: Color): void;
      // Clear the image with an integer color. R component is in the 8 lowest bits.
      clearInt(uintColor: number): void;
      // Save in BMP format. Return true if successful.
      saveBMP(fileName: string): boolean;
      // Save in PNG format. Return true if successful.
      savePNG(fileName: string): boolean;
      // Save in TGA format. Return true if successful.
      saveTGA(fileName: string): boolean;
      // Save in JPG format with compression quality. Return true if successful.
      saveJPG(fileName: string, quality: number): boolean;
      // Return a bilinearly sampled 2D pixel color. X and Y have the range 0-1.
      getPixelBilinear(x: number, y: number): Color;
      // Return a trilinearly sampled 3D pixel color. X, Y and Z have the range 0-1.
      getPixelTrilinear(x: number, y: number, z: number): Color;
      // Return width.
      getWidth(): number;
      // Return height.
      getHeight(): number;
      // Return depth.
      getDepth(): number;
      // Return number of color components.
      getComponents(): number;
      // Return whether is compressed.
      isCompressed(): boolean;
      // Return compressed format.
      getCompressedFormat(): CompressedFormat;
      // Return number of compressed mip levels.
      getNumCompressedLevels(): number;
      // Return next mip level by bilinear filtering.
      getNextLevel(): Image;
      // Return image converted to 4-component (RGBA) to circumvent modern rendering API's not supporting e.g. the luminance-alpha format.
      convertToRGBA(): Image;
      // Return subimage from the image by the defined rect or null if failed. 3D images are not supported. You must free the subimage yourself.
      getSubimage(rect: IntRect): Image;
      // Precalculate the mip levels. Used by asynchronous texture loading.
      precalculateLevels(): void;

   }

   export class JSONFile extends Resource {

      // Construct.
      constructor();


   }

   export class PListFile extends Resource {

      // Construct.
      constructor();


   }

   export class Resource extends AObject {

      name: string;
      memoryUse: number;
      asyncLoadState: AsyncLoadState;
      nameHash: string;
      useTimer: number;

      // Construct.
      constructor();

      // Finish resource loading. Always called from the main thread. Return true if successful.
      endLoad(): boolean;
      // Set name.
      setName(name: string): void;
      // Set memory use in bytes, possibly approximate.
      setMemoryUse(size: number): void;
      // Reset last used timer.
      resetUseTimer(): void;
      // Set the asynchronous loading state. Called by ResourceCache. Resources in the middle of asynchronous loading are not normally returned to user.
      setAsyncLoadState(newState: AsyncLoadState): void;
      // Return name.
      getName(): string;
      // Return name hash.
      getNameHash(): string;
      // Return memory use in bytes, possibly approximate.
      getMemoryUse(): number;
      // Return time since last use in milliseconds. If referred to elsewhere than in the resource cache, returns always zero.
      getUseTimer(): number;
      // Return the asynchronous loading state.
      getAsyncLoadState(): AsyncLoadState;

   }

   export class ResourceCache extends AObject {

      autoReloadResources: boolean;
      returnFailedResources: boolean;
      searchPackagesFirst: boolean;
      finishBackgroundResourcesMs: number;
      numBackgroundLoadResources: number;
      resourceDirs: string[];
      totalMemoryUse: number;

      // Construct.
      constructor();

      // Add a resource load directory. Optional priority parameter which will control search order.
      addResourceDir(pathName: string, priority?: number): boolean;
      // Add a package file for loading resources from by name. Optional priority parameter which will control search order.
      addPackageFile(fileName: string, priority?: number): boolean;
      // Add a manually created resource. Must be uniquely named.
      addManualResource(resource: Resource): boolean;
      // Remove a resource load directory.
      removeResourceDir(pathName: string): void;
      // Remove a package file by name. Optionally release the resources loaded from it.
      removePackageFile(fileName: string, releaseResources?: boolean, forceRelease?: boolean): void;
      // Release a resource by name.
      releaseResource(type: string, name: string, force?: boolean): void;
      // Release all resources. When called with the force flag false, releases all currently unused resources.
      releaseAllResources(force?: boolean): void;
      // Reload a resource. Return true on success. The resource will not be removed from the cache in case of failure.
      reloadResource(resource: Resource): boolean;
      // Reload a resource based on filename. Causes also reload of dependent resources if necessary.
      reloadResourceWithDependencies(fileName: string): void;
      // Set memory budget for a specific resource type, default 0 is unlimited.
      setMemoryBudget(type: string, budget: number): void;
      // Enable or disable automatic reloading of resources as files are modified. Default false.
      setAutoReloadResources(enable: boolean): void;
      // Enable or disable returning resources that failed to load. Default false. This may be useful in editing to not lose resource ref attributes.
      setReturnFailedResources(enable: boolean): void;
      // Define whether when getting resources should check package files or directories first. True for packages, false for directories.
      setSearchPackagesFirst(value: boolean): void;
      // Set how many milliseconds maximum per frame to spend on finishing background loaded resources.
      setFinishBackgroundResourcesMs(ms: number): void;
      // Open and return a file from the resource load paths or from inside a package file. If not found, use a fallback search with absolute path. Return null if fails. Can be called from outside the main thread.
      getFile(name: string, sendEventOnFailure?: boolean): File;
      // Return a resource by type and name. Load if not loaded yet. Return null if not found or if fails, unless SetReturnFailedResources(true) has been called. Can be called only from the main thread.
      getResource(type: string, name: string, sendEventOnFailure?: boolean): Resource;
      // Load a resource without storing it in the resource cache. Return null if not found or if fails. Can be called from outside the main thread if the resource itself is safe to load completely (it does not possess for example GPU data.)
      getTempResource(type: string, name: string, sendEventOnFailure?: boolean): Resource;
      // Background load a resource. An event will be sent when complete. Return true if successfully stored to the load queue, false if eg. already exists. Can be called from outside the main thread.
      backgroundLoadResource(type: string, name: string, sendEventOnFailure?: boolean, caller?: Resource): boolean;
      // Return number of pending background-loaded resources.
      getNumBackgroundLoadResources(): number;
      // Return an already loaded resource of specific type & name, or null if not found. Will not load if does not exist.
      getExistingResource(type: string, name: string): Resource;
      // Return added resource load directories.
      getResourceDirs(): string[];
      // Return whether a file exists by name.
      exists(name: string): boolean;
      // Return memory budget for a resource type.
      getMemoryBudget(type: string): number;
      // Return total memory use for a resource type.
      getMemoryUse(type: string): number;
      // Return total memory use for all resources.
      getTotalMemoryUse(): number;
      // Return full absolute file name of resource if possible.
      getResourceFileName(name: string): string;
      // Return whether automatic resource reloading is enabled.
      getAutoReloadResources(): boolean;
      // Return whether resources that failed to load are returned.
      getReturnFailedResources(): boolean;
      // Return whether when getting resources should check package files or directories first.
      getSearchPackagesFirst(): boolean;
      // Return how many milliseconds maximum to spend on finishing background loaded resources.
      getFinishBackgroundResourcesMs(): number;
      // Return either the path itself or its parent, based on which of them has recognized resource subdirectories.
      getPreferredResourceDir(path: string): string;
      // Remove unsupported constructs from the resource name to prevent ambiguity, and normalize absolute filename to resource path relative if possible.
      sanitateResourceName(name: string): string;
      // Remove unnecessary constructs from a resource directory name and ensure it to be an absolute path.
      sanitateResourceDirName(name: string): string;
      // Store a dependency for a resource. If a dependency file changes, the resource will be reloaded.
      storeResourceDependency(resource: Resource, dependency: string): void;
      // Reset dependencies for a resource.
      resetDependencies(resource: Resource): void;

   }

   export class XMLFile extends Resource {

      // Construct.
      constructor();

      // Deserialize from a string. Return true if successful.
      fromString(source: string): boolean;
      // Serialize the XML content to a string.
      toString(indentation?: string): string;
      // Patch the XMLFile with another XMLFile. Based on RFC 5261.
      patch(patchFile: XMLFile): void;

   }



//----------------------------------------------------
// MODULE: Network
//----------------------------------------------------


   export class Network extends AObject {

      updateFps: number;
      simulatedLatency: number;
      simulatedPacketLoss: number;
      packageCacheDir: string;

      // Construct.
      constructor();

      // Disconnect the connection to the server. If wait time is non-zero, will block while waiting for disconnect to finish.
      disconnect(waitMSec?: number): void;
      // Start a server on a port using UDP protocol. Return true if successful.
      startServer(port: number): boolean;
      // Stop the server.
      stopServer(): void;
      // Set network update FPS.
      setUpdateFps(fps: number): void;
      // Set simulated latency in milliseconds. This adds a fixed delay before sending each packet.
      setSimulatedLatency(ms: number): void;
      // Set simulated packet loss probability between 0.0 - 1.0.
      setSimulatedPacketLoss(probability: number): void;
      // Register a remote event as allowed to be received. There is also a fixed blacklist of events that can not be allowed in any case, such as ConsoleCommand.
      registerRemoteEvent(eventType: string): void;
      // Unregister a remote event as allowed to received.
      unregisterRemoteEvent(eventType: string): void;
      // Unregister all remote events.
      unregisterAllRemoteEvents(): void;
      // Set the package download cache directory.
      setPackageCacheDir(path: string): void;
      // Perform an HTTP request to the specified URL. Empty verb defaults to a GET request. Return a request object which can be used to read the response data.
      makeHttpRequest(url: string, verb?: string, headers?: string[], postData?: string): HttpRequest;
      // Return network update FPS.
      getUpdateFps(): number;
      // Return simulated latency in milliseconds.
      getSimulatedLatency(): number;
      // Return simulated packet loss probability.
      getSimulatedPacketLoss(): number;
      // Return whether the server is running.
      isServerRunning(): boolean;
      // Return whether a remote event is allowed to be received.
      checkRemoteEvent(eventType: string): boolean;
      // Return the package download cache directory.
      getPackageCacheDir(): string;
      // Process incoming messages from connections. Called by HandleBeginFrame.
      update(timeStep: number): void;
      // Send outgoing messages after frame logic. Called by HandleRenderUpdate.
      postUpdate(timeStep: number): void;

   }

   export class NetworkPriority extends Component {

      basePriority: number;
      distanceFactor: number;
      minPriority: number;
      alwaysUpdateOwner: boolean;

      // Construct.
      constructor();

      // Set base priority. Default 100 (send updates at full frequency.)
      setBasePriority(priority: number): void;
      // Set priority reduction distance factor. Default 0 (no effect.)
      setDistanceFactor(factor: number): void;
      // Set minimum priority. Default 0 (no updates when far away enough.)
      setMinPriority(priority: number): void;
      // Set whether updates to owner should be sent always at full rate. Default true.
      setAlwaysUpdateOwner(enable: boolean): void;
      // Return base priority.
      getBasePriority(): number;
      // Return priority reduction distance factor.
      getDistanceFactor(): number;
      // Return minimum priority.
      getMinPriority(): number;
      // Return whether updates to owner should be sent always at full rate.
      getAlwaysUpdateOwner(): boolean;

   }

   export class HttpRequest extends RefCounted {

      url: string;
      verb: string;
      error: string;
      state: HttpRequestState;
      availableSize: number;

      // Construct with parameters.
      constructor(url: string, verb: string, headers: string[], postData: string);

      // Process the connection in the worker thread until closed.
      threadFunction(): void;
      // Set position from the beginning of the stream. Not supported.
      seek(position: number): number;
      // Return URL used in the request.
      getURL(): string;
      // Return verb used in the request. Default GET if empty verb specified on construction.
      getVerb(): string;
      // Return error. Only non-empty in the error state.
      getError(): string;
      // Return connection state.
      getState(): HttpRequestState;
      // Return amount of bytes in the read buffer.
      getAvailableSize(): number;
      // Return whether connection is in the open state.
      isOpen(): boolean;

   }



//----------------------------------------------------
// MODULE: IO
//----------------------------------------------------


   export class File extends AObject {

      name: string;
      checksum: number;
      fullPath: string;
      mode: FileMode;

      // Construct and open a filesystem file.
      constructor(fileName: string, mode?: FileMode);

      // Set position from the beginning of the file.
      seek(position: number): number;
      // Return the file name.
      getName(): string;
      // Return a checksum of the file contents using the SDBM hash algorithm.
      getChecksum(): number;
      // Open a filesystem file. Return true if successful.
      open(fileName: string, mode?: FileMode): boolean;
      // Close the file.
      close(): void;
      // Flush any buffered output to the file.
      flush(): void;
      // Change the file name. Used by the resource system.
      setName(name: string): void;
      // Set the fullpath to the file
      setFullPath(path: string): void;
      // Return the open mode.
      getMode(): FileMode;
      // Return whether is open.
      isOpen(): boolean;
      // Return whether the file originates from a package.
      isPackaged(): boolean;
      // Return the fullpath to the file
      getFullPath(): string;
      // Unlike FileSystem.Copy this copy works when the source file is in a package file
      copy(srcFile: File): boolean;
      readText():string;
      writeString(text:string):void;

   }

   export class FileWatcher extends AObject {

      delay: number;
      path: string;

      // Construct.
      constructor();

      // Directory watching loop.
      threadFunction(): void;
      // Start watching a directory. Return true if successful.
      startWatching(pathName: string, watchSubDirs: boolean): boolean;
      // Stop watching the directory.
      stopWatching(): void;
      // Set the delay in seconds before file changes are notified. This (hopefully) avoids notifying when a file save is still in progress. Default 1 second.
      setDelay(interval: number): void;
      // Add a file change into the changes queue.
      addChange(fileName: string): void;
      // Return the path being watched, or empty if not watching.
      getPath(): string;
      // Return the delay in seconds for notifying file changes.
      getDelay(): number;

   }

   export class Log extends AObject {

      level: number;
      timeStamp: boolean;
      quiet: boolean;
      lastMessage: string;

      // Construct.
      constructor();

      // Open the log file.
      open(fileName: string): void;
      // Close the log file.
      close(): void;
      // Set logging level.
      setLevel(level: number): void;
      // Set whether to timestamp log messages.
      setTimeStamp(enable: boolean): void;
      // Set quiet mode ie. only print error entries to standard error stream (which is normally redirected to console also). Output to log file is not affected by this mode.
      setQuiet(quiet: boolean): void;
      // Return logging level.
      getLevel(): number;
      // Return whether log messages are timestamped.
      getTimeStamp(): boolean;
      // Return last log message.
      getLastMessage(): string;
      // Return whether log is in quiet mode (only errors printed to standard error stream).
      isQuiet(): boolean;
      // Write to the log. If logging level is higher than the level of the message, the message is ignored.
      write(level: number, message: string): void;
      // Write raw output to the log.
      writeRaw(message: string, error?: boolean): void;

   }

   export class FileSystem extends AObject {

      executeConsoleCommands: boolean;
      currentDir: string;
      programDir: string;
      userDocumentsDir: string;
      appBundleResourceFolder: string;

      // Construct.
      constructor();

      // Set the current working directory.
      setCurrentDir(pathName: string): boolean;
      // Create a directory.
      createDir(pathName: string): boolean;
      // Set whether to execute engine console commands as OS-specific system command.
      setExecuteConsoleCommands(enable: boolean): void;
      // Run a program using the command interpreter, block until it exits and return the exit code. Will fail if any allowed paths are defined.
      systemCommand(commandLine: string, redirectStdOutToLog?: boolean): number;
      // Run a specific program, block until it exits and return the exit code. Will fail if any allowed paths are defined.
      systemRun(fileName: string, args: string[]): number;
      // Run a program using the command interpreter asynchronously. Return a request ID or M_MAX_UNSIGNED if failed. The exit code will be posted together with the request ID in an AsyncExecFinished event. Will fail if any allowed paths are defined.
      systemCommandAsync(commandLine: string): number;
      // Run a specific program asynchronously. Return a request ID or M_MAX_UNSIGNED if failed. The exit code will be posted together with the request ID in an AsyncExecFinished event. Will fail if any allowed paths are defined.
      systemRunAsync(fileName: string, args: string[]): number;
      // Open a file in an external program, with mode such as "edit" optionally specified. Will fail if any allowed paths are defined.
      systemOpen(fileName: string, mode?: string): boolean;
      // Copy a file. Return true if successful.
      copy(srcFileName: string, destFileName: string): boolean;
      // Rename a file. Return true if successful.
      rename(srcFileName: string, destFileName: string): boolean;
      // Delete a file. Return true if successful.
      delete(fileName: string): boolean;
      // Register a path as allowed to access. If no paths are registered, all are allowed. Registering allowed paths is considered securing the Atomic execution environment: running programs and opening files externally through the system will fail afterward.
      registerPath(pathName: string): void;
      // Set a file's last modified time as seconds since 1.1.1970. Return true on success.
      setLastModifiedTime(fileName: string, newTime: number): boolean;
      // Return the absolute current working directory.
      getCurrentDir(): string;
      // Return whether is executing engine console commands as OS-specific system command.
      getExecuteConsoleCommands(): boolean;
      // Return whether paths have been registered.
      hasRegisteredPaths(): boolean;
      // Check if a path is allowed to be accessed. If no paths are registered, all are allowed.
      checkAccess(pathName: string): boolean;
      // Returns the file's last modified time as seconds since 1.1.1970, or 0 if can not be accessed.
      getLastModifiedTime(fileName: string): number;
      // Check if a file exists.
      fileExists(fileName: string): boolean;
      // Check if a directory exists.
      dirExists(pathName: string): boolean;
      // Return the program's directory. If it does not contain the Atomic default CoreData and Data directories, and the current working directory does, return the working directory instead.
      getProgramDir(): string;
      // Return the user documents directory.
      getUserDocumentsDir(): string;
      // Return the application preferences directory.
      getAppPreferencesDir(org: string, app: string): string;
      getAppBundleResourceFolder(): string;
      // Remove a directory
      removeDir(directoryIn: string, recursive: boolean): boolean;
      // Create directory and all necessary subdirectories below a given root
      createDirs(root: string, subdirectory: string): boolean;
      // Copy a directory, directoryOut must not exist
      copyDir(directoryIn: string, directoryOut: string): boolean;
      // Check if a file or directory exists at the specified path
      exists(pathName: string): boolean;
      createDirsRecursive(directoryIn: string, directoryOut: string): boolean;
      scanDir(pathName:string, filter:string, flags:number, recursive:boolean);

   }



//----------------------------------------------------
// MODULE: Engine
//----------------------------------------------------


   export class Engine extends AObject {

      minFps: number;
      maxFps: number;
      maxInactiveFps: number;
      timeStepSmoothing: number;
      pauseMinimized: boolean;
      autoExit: boolean;
      nextTimeStep: number;

      // Construct.
      constructor();

      // Run one frame.
      runFrame(): void;
      // Set minimum frames per second. If FPS goes lower than this, time will appear to slow down.
      setMinFps(fps: number): void;
      // Set maximum frames per second. The engine will sleep if FPS is higher than this.
      setMaxFps(fps: number): void;
      // Set maximum frames per second when the application does not have input focus.
      setMaxInactiveFps(fps: number): void;
      // Set how many frames to average for timestep smoothing. Default is 2. 1 disables smoothing.
      setTimeStepSmoothing(frames: number): void;
      // Set whether to pause update events and audio when minimized.
      setPauseMinimized(enable: boolean): void;
      // Set whether to exit automatically on exit request (window close button.)
      setAutoExit(enable: boolean): void;
      // Override timestep of the next frame. Should be called in between RunFrame() calls.
      setNextTimeStep(seconds: number): void;
      // Close the graphics window and set the exit flag. No-op on iOS, as an iOS application can not legally exit.
      exit(): void;
      // Dump profiling information to the log.
      dumpProfiler(): void;
      // Dump information of all resources to the log.
      dumpResources(dumpFileName?: boolean): void;
      // Dump information of all memory allocations to the log. Supported in MSVC debug mode only.
      dumpMemory(): void;
      // Get timestep of the next frame. Updated by ApplyFrameLimit().
      getNextTimeStep(): number;
      // Return the minimum frames per second.
      getMinFps(): number;
      // Return the maximum frames per second.
      getMaxFps(): number;
      // Return the maximum frames per second when the application does not have input focus.
      getMaxInactiveFps(): number;
      // Return how many frames to average for timestep smoothing.
      getTimeStepSmoothing(): number;
      // Return whether to pause update events and audio when minimized.
      getPauseMinimized(): boolean;
      // Return whether to exit automatically on exit request.
      getAutoExit(): boolean;
      // Return whether engine has been initialized.
      isInitialized(): boolean;
      // Return whether exit has been requested.
      isExiting(): boolean;
      // Return whether the engine has been created in headless mode.
      isHeadless(): boolean;
      // Send frame update events.
      update(): void;
      // Render after frame update.
      render(): void;
      // Get the timestep for the next frame and sleep for frame limiting if necessary.
      applyFrameLimit(): void;

   }



//----------------------------------------------------
// MODULE: Javascript
//----------------------------------------------------


   export class ScriptObject extends AObject {

      // Construct.
      constructor();


   }

   export class JSComponent extends Component {

      componentFile: JSComponentFile;
      updateEventMask: number;

      // Construct.
      constructor();

      applyAttributes(): void;
      getComponentFile(): JSComponentFile;
      // Match script name
      matchScriptName(path: string): boolean;
      // Handle enabled/disabled state change. Changes update event subscription.
      onSetEnabled(): void;
      // Set what update events should be subscribed to. Use this for optimization: by default all are in use. Note that this is not an attribute and is not saved or network-serialized, therefore it should always be called eg. in the subclass constructor.
      setUpdateEventMask(mask: number): void;
      // Return what update events are subscribed to.
      getUpdateEventMask(): number;
      // Return whether the DelayedStart() function has been called.
      isDelayedStartCalled(): boolean;
      setComponentFile(cfile: JSComponentFile): void;
      setDestroyed(): void;
      initInstance(hasArgs?: boolean, argIdx?: number): void;

   }

   export class JSComponentFile extends Resource {

      scriptClass: boolean;

      // Construct.
      constructor();

      getScriptClass(): boolean;
      createJSComponent(): JSComponent;
      pushModule(): boolean;

   }

   export class JSEventHelper extends AObject {

      // Construct.
      constructor(object: AObject);


   }



//----------------------------------------------------
// MODULE: Environment
//----------------------------------------------------


   export class ProcSky extends Drawable {

      dayTime: number;
      autoUpdate: boolean;
      updateGeometryType: UpdateGeometryType;
      timeOfDay: number;

      constructor();

      setDayTime(time: number): number;
      getDayTime(): number;
      setAutoUpdate(autoUpdate: boolean): void;
      // Return whether a geometry update is necessary, and if it can happen in a worker thread.
      getUpdateGeometryType(): UpdateGeometryType;
      getTimeOfDay(): number;

   }



}
