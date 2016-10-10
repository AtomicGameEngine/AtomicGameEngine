using System;
using System.IO;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;

namespace AtomicEngine
{
    internal class CSComponentInfo
    {
        public CSComponentInfo(Type type)
        {

#if ATOMIC_DESKTOP || ATOMIC_MOBILE

            this.Type = type;

            // Fields
            List<FieldInfo> fields = new List<FieldInfo>();
            fields.AddRange(type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance)
                .Where(field => field.IsDefined(typeof(InspectorAttribute), true)));

            // Inspector fields may be private. BindingFlags.NonPublic does not report private fields in superclasses
            var baseType = type.BaseType;
            while (baseType != typeof(CSComponent))
            {
                fields.AddRange(baseType.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance)
                .Where(field => field.IsDefined(typeof(InspectorAttribute), true)));
                baseType = baseType.BaseType;
            }

            InspectorFields = fields.ToArray<FieldInfo>();

            foreach (var field in InspectorFields)
            {
                fieldLookup[AtomicNET.StringToStringHash(field.Name)] = field;
            }

            // Methods
            Type[] updateParms = new Type[1] { typeof(float) };
            UpdateMethod = type.GetMethod("Update", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, updateParms, null);

            Type[] postUpdateParms = new Type[1] { typeof(float) };
            PostUpdateMethod = type.GetMethod("PostUpdate", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, postUpdateParms, null);

            Type[] physicsPreStepParms = new Type[1] { typeof(float) };
            PhysicsPreStepMethod = type.GetMethod("PhysicsPreStep", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, physicsPreStepParms, null);

            Type[] physicsPostStepParms = new Type[1] { typeof(float) };
            PhysicsPostStepMethod = type.GetMethod("PhysicsPostStep", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, physicsPostStepParms, null);

            Type[] startParms = new Type[0] { };
            StartMethod = type.GetMethod("Start", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, startParms, null);
#endif

        }

        public void ApplyFieldValues(CSComponent component, IntPtr fieldValuePtr)
        {
            // FIXME: This will need to be optimized, specifically to use uint key hashes for value lookup

#if ATOMIC_DESKTOP || ATOMIC_MOBILE

            fieldMap.CopyVariantMap(fieldValuePtr);

            foreach (var field in InspectorFields)
            {
                if (fieldMap.Contains(field.Name))
                {
                    var fieldType = field.FieldType;

                    if (fieldType.IsEnum)
                    {
                        field.SetValue(component, fieldMap.GetInt(field.Name));
                        continue;
                    }

                    switch (Type.GetTypeCode(fieldType))
                    {
                        case TypeCode.Boolean:
                            field.SetValue(component, fieldMap.GetBool(field.Name));
                            break;

                        case TypeCode.Int32:
                            field.SetValue(component, fieldMap.GetInt(field.Name));
                            break;

                        case TypeCode.Single:
                            field.SetValue(component, fieldMap.GetFloat(field.Name));
                            break;

                        case TypeCode.String:
                            field.SetValue(component, fieldMap.GetString(field.Name));
                            break;

                        default:

                            if (fieldType == typeof(Vector3))
                            {
                                field.SetValue(component, fieldMap.GetVector3(field.Name));
                            }
                            else if (fieldType == typeof(Quaternion))
                            {
                                field.SetValue(component, fieldMap.GetQuaternion(field.Name));
                            }
                            else if (fieldType.IsSubclassOf(typeof(Resource)))
                            {
                                field.SetValue(component, fieldMap.GetResourceFromRef(field.Name));
                            }
                            else if (fieldType.IsSubclassOf(typeof(RefCounted)))
                            {
                                field.SetValue(component, fieldMap.GetPtr(field.Name));
                            }

                            break;
                    }
                }
            }

#endif

        }

        public FieldInfo[] InspectorFields;
        public Type Type;

        // Start method called once
        public MethodInfo StartMethod = null;

        // Update called first
        public MethodInfo UpdateMethod = null;

        // Physics steps if any
        public MethodInfo PhysicsPreStepMethod = null;
        public MethodInfo PhysicsPostStepMethod = null;

        // Post Update
        public MethodInfo PostUpdateMethod = null;
                
        ScriptVariantMap fieldMap = new ScriptVariantMap();

        public Dictionary<uint, FieldInfo> fieldLookup = new Dictionary<uint, FieldInfo>();
    }

    public class CSComponentCore : NETScriptObject
    {
        [Obsolete("Method HandleComponentAssemblyReference is deprecated (loading component assemblies at runtime, will be changed to preload them)")]
        void HandleComponentAssemblyReference(uint eventType, ScriptVariantMap eventData)
        {
            string assemblyPath = eventData["AssemblyPath"];
            string assemblyName = Path.GetFileNameWithoutExtension(assemblyPath);
            Log.Info($"Component Assembly referenced {assemblyName} ");
        }

        void ParseComponents()
        {
#if ATOMIC_DESKTOP || ATOMIC_MOBILE

            var assemblies = AppDomain.CurrentDomain.GetAssemblies().ToList();

            foreach (Assembly assembly in assemblies)
            {
                ParseAssembly(assembly);
            }
#endif
        }

        void ParseAssembly(Assembly assembly)
        {
#if ATOMIC_DESKTOP || ATOMIC_MOBILE

            if (parsedAssemblies.ContainsKey(assembly))
            {
                return;
            }

            parsedAssemblies[assembly] = true;

            Type[] types = assembly.GetTypes();

            foreach (var type in types)
            {
                if (type.IsSubclassOf(typeof(CSComponent)))
                {
                    var csinfo = new CSComponentInfo(type);
                    csinfoLookup[csinfo.Type] = csinfo;
                    componentCache[type.Name] = csinfo;
                }
            }
#endif
        }

        internal static void Initialize()
        {

            instance = new CSComponentCore();
            instance.ParseComponents();

            instance.SubscribeToEvent("CSComponentAssemblyReference", instance.HandleComponentAssemblyReference);

        }

        // type name -> CSComponentInfo lookup TODO: store with namespace to solve ambiguities
        internal static Dictionary<string, CSComponentInfo> componentCache = new Dictionary<string, CSComponentInfo>();

        [Obsolete("Member parsedAssemblies is temporarily required for runtime component assemblies loading")]
        Dictionary<Assembly, bool> parsedAssemblies = new Dictionary<Assembly, bool>();

        internal static Dictionary<Type, CSComponentInfo> csinfoLookup = new Dictionary<Type, CSComponentInfo>();

        static CSComponentCore instance;
    }
}