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
            this.Type = type;

            // Fields

            var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance)
            .Where(field => field.IsDefined(typeof(InspectorAttribute), true));

            InspectorFields = fields.ToArray<FieldInfo>();

            foreach (var field in InspectorFields)
            {
                fieldLookup[AtomicNET.StringToStringHash(field.Name)] = field;
            }

            // Methods
            Type[] updateParms = new Type[1] { typeof(float) };
            UpdateMethod = type.GetMethod("Update", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, updateParms, null);

            Type[] startParms = new Type[0] { };
            StartMethod = type.GetMethod("Start", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, startParms, null);

        }

        public void ApplyFieldValues(CSComponent component, IntPtr fieldValuePtr)
        {
            // FIXME: This will need to be optimized, specifically to use uint key hashes for value lookup

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

        }

        public void RegisterInstance(CSComponent component)
        {
            Instances.Add(component);

            if (StartMethod != null)
            {
                StartList.Add(component);
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

                    if (node != null && node.IsEnabled())
                    {

                        if (node.Scene != null)
                        {
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
            string assemblyPath = eventData["AssemblyPath"];

            Dictionary<string, CSComponentInfo> assemblyTypes = null;

            if (!componentCache.TryGetValue(assemblyPath, out assemblyTypes))
            {
                componentCache[assemblyPath] = assemblyTypes = new Dictionary<string, CSComponentInfo>();
            }

            // HACK!
            if (PlayerApp.DeployedApp)
            {
                assemblyPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "/AtomicProject.dll";
            }

            Assembly assembly = Assembly.LoadFrom(assemblyPath);

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
        }

        internal static void Initialize()
        {
            instance = new CSComponentCore();

            instance.SubscribeToEvent("CSComponentAssemblyReference", instance.HandleComponentAssemblyReference);
            instance.SubscribeToEvent("CSComponentLoad", instance.HandleComponentLoad);
            instance.SubscribeToEvent("Update", instance.HandleUpdate);
        }

        Dictionary<string, Dictionary<string, CSComponentInfo>> componentCache = new Dictionary<string, Dictionary<string, CSComponentInfo>>();

        Dictionary<Type, CSComponentInfo> csinfoLookup = new Dictionary<Type, CSComponentInfo>();

        static CSComponentCore instance;
    }
}