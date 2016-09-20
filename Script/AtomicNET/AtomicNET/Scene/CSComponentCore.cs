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

            Type[] fixedUpdateParms = new Type[1] { typeof(float) };
            FixedUpdateMethod = type.GetMethod("FixedUpdate", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, fixedUpdateParms, null);

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

        public void RegisterInstance(CSComponent component)
        {
            Instances.Add(component);

            if (StartMethod != null)
            {
                StartList.Add(component);
            }

        }

        public void FixedUpdate(Object[] args)
        {
            if (FixedUpdateMethod == null)
                return;

            foreach (var instance in Instances)
            { 
                var node = instance.Node;

                if (node != null && node.IsEnabled())
                {

                    if (node.Scene != null)
                    {
                        FixedUpdateMethod.Invoke(instance, args);
                    }
                }
            }
        }

        public void PostUpdate(Object[] args)
        {
            if (PostUpdateMethod == null)
                return;

            foreach (var instance in Instances)
            {
                var node = instance.Node;

                if (node != null && node.IsEnabled())
                {

                    if (node.Scene != null)
                    {
                        PostUpdateMethod.Invoke(instance, args);
                    }
                }
            }
        }


        public void Update(Object[] args)
        {
            if (StartMethod != null)
            {
                foreach (var instance in StartList)
                {
                    var node = instance.Node;

                    if (node != null && node.IsEnabled())
                    {
                        if (node.Scene != null)
                        {
                            StartMethod.Invoke(instance, null);
                        }
                    }

                }

                // TODO: need to handle delayed starts when node isn't enabled
                StartList.Clear();

            }

            if (UpdateMethod != null)
            {
                foreach (var instance in Instances)
                {
                    bool remove = false;

                    var node = instance.Node;

                    // TODO: Ideally we want to remove disabled instances,
                    // and re-add them when re-enabled
                    if (node != null /*&& node.IsEnabled()*/)
                    {

                        if (node.Scene != null)
                        {
                            if (node.IsEnabled())
                                UpdateMethod.Invoke(instance, args);
                        }
                        else
                        {
                            remove = true;
                        }
                    }
                    else
                    {
                        remove = true;
                    }

                    if (remove)
                        RemoveList.Add(instance);
                }
            }

            foreach (var instance in RemoveList)
            {
                Instances.Remove(instance);                
            }

            RemoveList.Clear();
        }

        public List<CSComponent> Instances = new List<CSComponent>();
        public List<CSComponent> StartList = new List<CSComponent>();

        public List<CSComponent> RemoveList = new List<CSComponent>();

        public FieldInfo[] InspectorFields;
        public Type Type;

        public MethodInfo FixedUpdateMethod = null;
        public MethodInfo PostUpdateMethod = null;
        public MethodInfo UpdateMethod = null;        
        public MethodInfo StartMethod = null;
        

        ScriptVariantMap fieldMap = new ScriptVariantMap();

        public Dictionary<uint, FieldInfo> fieldLookup = new Dictionary<uint, FieldInfo>();
    }

    public class CSComponentCore : NETScriptObject
    {

        public static void RegisterInstance(CSComponent component)
        {
            instance.csinfoLookup[component.GetType()].RegisterInstance(component);
        }

        void HandleUpdate(uint eventType, ScriptVariantMap eventData)
        {
            Object[] args = new Object[1] { eventData.GetFloat("timestep") };

            foreach (var csinfo in csinfoLookup.Values)
            {
                csinfo.Update(args);
            }

        }

        void HandlePhysicsPreStep(uint eventType, ScriptVariantMap eventData)
        {
            // TODO: eventData also has a PhysicsWorld pointer, which could be factored in for multiworld support

            Object[] args = new Object[1] { eventData.GetFloat("timestep") };

            foreach (var csinfo in csinfoLookup.Values)
            {
                csinfo.FixedUpdate(args);
            }

        }

        void HandlePostUpdate(uint eventType, ScriptVariantMap eventData)
        {
            Object[] args = new Object[1] { eventData.GetFloat("timestep") };

            foreach (var csinfo in csinfoLookup.Values)
            {
                csinfo.PostUpdate(args);
            }

        }

        void HandleComponentLoad(uint eventType, ScriptVariantMap eventData)
        {
            var assemblyPath = eventData["AssemblyPath"];

            var className = eventData["ClassName"];
            IntPtr csnative = eventData.GetVoidPtr("NativeInstance");
            IntPtr fieldValues = IntPtr.Zero;

            if (eventData.Contains("FieldValues"))
                fieldValues = eventData.GetVoidPtr("FieldValues");

            Dictionary<string, CSComponentInfo> assemblyTypes = null;

            if (!componentCache.TryGetValue(assemblyPath, out assemblyTypes))
            {
                return;
            }

            CSComponentInfo csinfo;

            if (!assemblyTypes.TryGetValue(className, out csinfo))
            {
                return;
            }

            NativeCore.NativeContructorOverride = csnative;
            var component = (CSComponent)Activator.CreateInstance(csinfo.Type);
            NativeCore.VerifyNativeContructorOverrideConsumed();

            if (fieldValues != IntPtr.Zero)
                csinfo.ApplyFieldValues(component, fieldValues);

            csinfo.RegisterInstance(component);

        }

        void HandleComponentAssemblyReference(uint eventType, ScriptVariantMap eventData)
        {
#if ATOMIC_DESKTOP || ATOMIC_MOBILE
            string assemblyPath = eventData["AssemblyPath"];

            string assemblyName = Path.GetFileNameWithoutExtension(assemblyPath);
            if (componentCache.ContainsKey(assemblyName))
                return;

            Assembly assembly = Assembly.LoadFrom(assemblyPath);

            ParseAssembly(assembly);
#endif
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
            String assemblyPath = assembly.GetName().Name;

            Dictionary<string, CSComponentInfo> assemblyTypes = null;

            if (!componentCache.TryGetValue(assemblyPath, out assemblyTypes))
            {
                componentCache[assemblyPath] = assemblyTypes = new Dictionary<string, CSComponentInfo>();
            }

            Type[] types = assembly.GetTypes();

            foreach (var type in types)
            {
                if (type.IsSubclassOf(typeof(CSComponent)))
                {
                    var csinfo = new CSComponentInfo(type);
                    csinfoLookup[csinfo.Type] = csinfo;
                    assemblyTypes[type.Name] = csinfo;
                }
            }
#endif
        }

        internal static void Initialize()
        {

            instance = new CSComponentCore();
            instance.ParseComponents();

            instance.SubscribeToEvent("CSComponentAssemblyReference", instance.HandleComponentAssemblyReference);
            instance.SubscribeToEvent("CSComponentLoad", instance.HandleComponentLoad);
            instance.SubscribeToEvent("Update", instance.HandleUpdate);

            // PhysicsPreStep gets mapped to FixedUpdate
            instance.SubscribeToEvent("PhysicsPreStep", instance.HandlePhysicsPreStep);

            instance.SubscribeToEvent("PostUpdate", instance.HandlePostUpdate);

        }

        Dictionary<string, Dictionary<string, CSComponentInfo>> componentCache = new Dictionary<string, Dictionary<string, CSComponentInfo>>();

        Dictionary<Type, CSComponentInfo> csinfoLookup = new Dictionary<Type, CSComponentInfo>();

        static CSComponentCore instance;
    }
}