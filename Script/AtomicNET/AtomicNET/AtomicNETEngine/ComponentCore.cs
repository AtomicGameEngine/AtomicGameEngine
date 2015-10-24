using System;
using System.Reflection;
using System.Collections.Generic;
using System.Runtime.Loader;
using System.Runtime.InteropServices;
using System.Linq;

namespace AtomicEngine
{

  public static class ComponentCore
  {

    static List<CSComponent> startQueue = new List<CSComponent>();

    // holds a reference
    static Dictionary<IntPtr, CSComponent> liveComponents = new Dictionary<IntPtr, CSComponent>();

    public static void Update (float timeStep)
    {

      List<CSComponent> _startQueue = new List<CSComponent>(startQueue);
      startQueue.Clear();

      foreach (var c in _startQueue)
      {
        c.StartInternal();
      }

      foreach (var c in liveComponents.Values)
      {
        c.Update(timeStep);
      }
    }

    // This will need to be optimized
    public static void CSComponentApplyFields(IntPtr componentPtr, IntPtr fieldMapPtr)
    {
      NETVariantMap fieldMap = NativeCore.WrapNative<NETVariantMap>(fieldMapPtr);;
      CSComponent component = NativeCore.WrapNative<CSComponent>(componentPtr);;

      if (fieldMap == null || component == null)
      return;

      FieldInfo[] fields = componentClassFields[component.GetType()];

      foreach (var field in fields)
      {
        if (fieldMap.Contains(field.Name))
        {
          //Console.WriteLine("Applying: {0} {1}", field.Name, field.FieldType.Name);

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

    static Dictionary<Type, FieldInfo[] > componentClassFields = new Dictionary<Type, FieldInfo[]>();

    static Dictionary<string, Dictionary<string, Type>> componentTypeLookup = new Dictionary<string, Dictionary<string, Type>>();

    public static IntPtr CSComponentCreate(string assemblyName, string classTypeName)
    {

      Assembly assembly = AssemblyLoadContext.Default.LoadFromAssemblyName(new AssemblyName(assemblyName));

      string classNamespace = "";
      if (!componentTypeLookup.ContainsKey(assemblyName))
      {
        componentTypeLookup[assemblyName] = new Dictionary<string, Type>();
      }

      var lookup = componentTypeLookup[assemblyName];

      Type type = null;

      if (!lookup.TryGetValue(classTypeName, out type))
      {
        var types = assembly.GetTypes();
        foreach (var atype in types)
        {
          if (atype.Name == classTypeName)
          {
            lookup[classTypeName] = atype;
            type = atype;
            break;
          }
        }
      }

      if (type == null)
      return IntPtr.Zero;

      // TODO: check type is derived from CSComponent

      var component = (CSComponent) Activator.CreateInstance(type);

      if (!componentClassFields.ContainsKey(type))
      {
        var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance)
        .Where(field => field.IsDefined(typeof(InspectorAttribute), true));

        componentClassFields[type] = fields.ToArray<FieldInfo>();

      }

      startQueue.Add(component);

      liveComponents[component.nativeInstance] = component;

      return component.nativeInstance;

    }

  }

}
