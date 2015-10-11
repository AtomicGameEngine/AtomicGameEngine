using System;
using System.Reflection;
using System.Collections.Generic;
using System.Runtime.Loader;
using System.Runtime.InteropServices;
using System.Linq;

namespace AtomicEngine
{


public partial class CSComponent : ScriptComponent
{

  public void StartInternal()
  {
    ApplyFieldValues();
    Start();
  }

  public virtual void Start()
  {

  }


  public virtual void Update(float timeStep)
  {

  }



}

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

  public static IntPtr CSComponentCreate(string assemblyName, string classTypeName)
  {

    Assembly assembly = AssemblyLoadContext.Default.LoadFromAssemblyName(new AssemblyName(assemblyName));

    Type type = assembly.GetType("AtomicNETTest."  + classTypeName);

    // TODO: check type is derived from CSComponent

    var component = (CSComponent) Activator.CreateInstance(type);

    if (!componentClassFields.ContainsKey(type))
    {
      var fields = type.GetFields()
          .Where(field => field.IsDefined(typeof(InspectorAttribute), true));

      componentClassFields[type] = fields.ToArray<FieldInfo>();

    }

    startQueue.Add(component);

    liveComponents[component.nativeInstance] = component;

    return component.nativeInstance;

  }

}

}
