// Copyright (c) Microsoft.  All Rights Reserved.  Licensed under the Apache License, Version 2.0.  See License.txt in the project root for license information.

using System.Collections.Immutable;

//using Roslyn.Utilities;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Text;
using System.Reflection.Emit;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;

// References

//https://github.com/Microsoft/dotnetsamples/tree/master/System.Reflection.Metadata
//https://github.com/dotnet/corefx/tree/master/src/System.Reflection.Metadata/tests
//http://www.cnetion.com/getting-field-values-using-mono-cecil-qq-AUvBjRFgivICeoL1jxJy.php

// https://github.com/Reactive-Extensions/IL2JS/blob/master/CCI2/PeReader/ILReader.cs

// https://github.com/Reactive-Extensions/IL2JS

// custom attr loading: https://github.com/Reactive-Extensions/IL2JS/blob/a4570f9c69b6c40d001e7539b952266d67609ca9/CST/PELoader.cs#L2352
// custom attr: https://www.simple-talk.com/blogs/2011/06/03/anatomy-of-a-net-assembly-custom-attribute-encoding/
// custom attr: https://github.com/jbevain/cecil/blob/67a2569688a13a6cb487f9af5c3418f7a8f43e3c/Mono.Cecil/AssemblyReader.cs

// https://github.com/dotnet/roslyn/tree/master/src/Compilers/Core/Portable/MetadataReader


namespace AtomicTools
{

	public class CSComponentInspector
	{

		InspectorComponent _inspectorComponent;

		public CSComponentInspector (TypeDefinition typeDef, PEReader peFile, MetadataReader metaReader)
		{
			this.typeDef = typeDef;
			this.peFile = peFile;
			this.metaReader = metaReader;

			this._inspectorComponent = new InspectorComponent ();
			this._inspectorComponent.Name = metaReader.GetString (typeDef.Name);
			this._inspectorComponent.Namespace = metaReader.GetString (typeDef.Namespace);
		}

		// Inspect a CSComponent derived class
		internal InspectorComponent Inspect ()
		{

			var fields = typeDef.GetFields ();

			foreach (var fieldHandle in fields) {

				var inspectorField = new InspectorField ();

				var fieldDef = metaReader.GetFieldDefinition (fieldHandle);

				var customAttr = fieldDef.GetCustomAttributes ();

				foreach (var caHandle in customAttr) {

					// Look for InspectorAttribute
					if (DecodeCustomAttribute (caHandle, inspectorField)) {

						BlobReader sigReader = metaReader.GetBlobReader (fieldDef.Signature);
						SignatureHeader header = sigReader.ReadSignatureHeader ();

						if (header.Kind != SignatureKind.Field)
							continue;

						var typeCode = sigReader.ReadSignatureTypeCode ();

						string typeName = typeCode.ToString ();

						if (typeCode == SignatureTypeCode.TypeHandle) {

							EntityHandle token = sigReader.ReadTypeHandle ();
							HandleKind tokenType = token.Kind;

							if (tokenType == HandleKind.TypeDefinition) {

								// can store local enum typedefs
								// enum initializers are stored as constant value in the IL

								var typeDef = metaReader.GetTypeDefinition ((TypeDefinitionHandle)token);

								var baseTypeToken = typeDef.BaseType;

								if (baseTypeToken.Kind != HandleKind.TypeReference)
									continue;

								var baseTypeRef = metaReader.GetTypeReference ((TypeReferenceHandle)baseTypeToken);

								if (metaReader.GetString (baseTypeRef.Name) != "Enum")
									continue;

								inspectorField.IsEnum = true;
								typeName = metaReader.GetString (typeDef.Name);

							} else if (tokenType == HandleKind.TypeReference) {

								// TypeReference, ok
								var typeRef = metaReader.GetTypeReference ((TypeReferenceHandle)token);

								typeName = metaReader.GetString (typeRef.Name);

							} else {
								// ???
								continue;
							}

						}

						inspectorField.TypeName = typeName;
						inspectorField.Name = metaReader.GetString (fieldDef.Name);
						_inspectorComponent.Fields [inspectorField.Name] = inspectorField;

						break;
					}
				}
			}

			// There is no way to get the initializer value of a field
			// other than to inspect the IL code of the constructor
			var methods = typeDef.GetMethods ();

			foreach (var methodHandle in methods) {

				var methodDef = metaReader.GetMethodDefinition (methodHandle);

				if (metaReader.GetString (methodDef.Name) == ".ctor") {

					var body = peFile.GetMethodBody (methodDef.RelativeVirtualAddress);
					var ilBytes = body.GetILContent ();

					InspectILBlock (ilBytes, ilBytes.Length);
				}

			}

			//Dump ();

			return _inspectorComponent;

		}

		private bool DecodeCustomAttribute (CustomAttributeHandle caHandle, InspectorField inspectorField)
		{

			// GetCustomAttribute: https://github.com/dotnet/roslyn/blob/master/src/Compilers/Core/Portable/MetadataReader/MetadataDecoder.cs#L1370

			// Custom Attribute
			var ca = metaReader.GetCustomAttribute (caHandle);

			// MethodDefinitionHandle or MemberReferenceHandle
			if (ca.Constructor.Kind != HandleKind.MemberReference) {
				Console.WriteLine ("ca.Constructor.Kind != HandleKind.MemberReference");
				return false;
			}

			// constructor of the custom attr which contains the signature
			var memberRef = metaReader.GetMemberReference ((MemberReferenceHandle)ca.Constructor);

			// parent of the constructor is the TypeReference
			var parent = memberRef.Parent;

			if (parent.Kind != HandleKind.TypeReference) {
				Console.WriteLine ("parent.Kind != HandleKind.TypeReference");
				return false;
			}

			var parentTypeRef = metaReader.GetTypeReference ((TypeReferenceHandle)parent);

			// check whether we have an InspectorAttribute
			if (metaReader.GetString (parentTypeRef.Name) != "InspectorAttribute") {
				Console.WriteLine ("parentTypeRef != InspectorAttribute");
				return false;
			}

			// args
			var argsReader = metaReader.GetBlobReader ((BlobHandle)ca.Value);

			uint prolog = argsReader.ReadUInt16 ();

			if (prolog != 1) {
				Console.WriteLine ("prolog != 1");
				return false;
			}

			// sig reader is on constructor
			BlobReader sigReader = metaReader.GetBlobReader (memberRef.Signature);
			SignatureHeader header = sigReader.ReadSignatureHeader ();

			// Get the type parameter count.
			if (header.IsGeneric && sigReader.ReadCompressedInteger () != 0) {
				Console.WriteLine ("header.IsGeneric && sigReader.ReadCompressedInteger() != 0");
				return false;
			}

			// Get the parameter count
			int paramCount = sigReader.ReadCompressedInteger ();

			// Get the type return type.
			var returnTypeCode = sigReader.ReadSignatureTypeCode ();

			if (returnTypeCode != SignatureTypeCode.Void) {
				Console.WriteLine ("returnTypeCode != SignatureTypeCode.Void");
				return false;
			}

			List<SignatureTypeCode> sigTypeCodes = new List<SignatureTypeCode> ();

			// position args
			for (int i = 0; i < paramCount; i++) {

				SignatureTypeCode paramTypeCode = sigReader.ReadSignatureTypeCode ();

				// support string custom attr for now to simplify things
				if (paramTypeCode != SignatureTypeCode.String)
					return false;

				string value;

				if (CrackStringInAttributeValue (out value, ref argsReader)) {
					inspectorField.CustomAttrPositionalArgs.Add (value);
				}

				sigTypeCodes.Add (paramTypeCode);
			}

			// named args

			short namedParamCount = argsReader.ReadInt16 ();

			for (short i = 0; i < namedParamCount; i++) {

				// Ecma-335 23.3 - A NamedArg is simply a FixedArg preceded by information to identify which field or
				// property it represents. [Note: Recall that the CLI allows fields and properties to have the same name; so
				// we require a means to disambiguate such situations. end note] FIELD is the single byte 0x53. PROPERTY is
				// the single byte 0x54.

				// https://github.com/dotnet/roslyn/blob/master/src/Compilers/Core/Portable/MetadataReader/MetadataDecoder.cs#L1305

				var kind = (CustomAttributeNamedArgumentKind)argsReader.ReadCompressedInteger ();

				if (kind != CustomAttributeNamedArgumentKind.Field && kind != CustomAttributeNamedArgumentKind.Property) {
					return false;
				}

				var typeCode = argsReader.ReadSerializationTypeCode ();

				// support string custom attr for now to simplify things
				if (typeCode != SerializationTypeCode.String)
					return false;

				string name;

				if (!CrackStringInAttributeValue (out name, ref argsReader))
					return false;

				string value;

				if (!CrackStringInAttributeValue (out value, ref argsReader))
					return false;

				inspectorField.CustomAttrNamedArgs [name] = value;

			}

			return true;

		}

		internal static bool CrackStringInAttributeValue (out string value, ref BlobReader sig)
		{
			try {
				int strLen;
				if (sig.TryReadCompressedInteger (out strLen) && sig.RemainingBytes >= strLen) {
					value = sig.ReadUTF8 (strLen);

					// Trim null characters at the end to mimic native compiler behavior.
					// There are libraries that have them and leaving them in breaks tests.
					value = value.TrimEnd ('\0');

					return true;
				}

				value = null;

				// Strings are stored as UTF8, but 0xFF means NULL string.
				return sig.RemainingBytes >= 1 && sig.ReadByte () == 0xFF;
			} catch (BadImageFormatException) {
				value = null;
				return false;
			}
		}

		public void InspectILBlock (
			ImmutableArray<byte> ilBytes,
			int length,
			IReadOnlyList<HandlerSpan> spans = null,
			int blockOffset = 0,
			IReadOnlyDictionary<int, string> markers = null)
		{
			if (ilBytes == null) {
				return;
			}

			int spanIndex = 0;
			int curIndex = InspectILBlock (ilBytes, length, spans, blockOffset, 0, spanIndex, markers, out spanIndex);
		}

		private int InspectILBlock (
			ImmutableArray<byte> ilBytes,
			int length,
			IReadOnlyList<HandlerSpan> spans,
			int blockOffset,
			int curIndex,
			int spanIndex,
			IReadOnlyDictionary<int, string> markers,
			out int nextSpanIndex)
		{
			int lastSpanIndex = spanIndex - 1;

			List<string> loadedValues = new List<string> ();

			while (curIndex < length) {

				if (lastSpanIndex > 0 && StartsFilterHandler (spans, lastSpanIndex, curIndex + blockOffset)) {

				}

				if (StartsSpan (spans, spanIndex, curIndex + blockOffset)) {
					curIndex = InspectILBlock (ilBytes, length, spans, blockOffset, curIndex, spanIndex + 1, markers, out spanIndex);
				} else {

					int ilOffset = curIndex + blockOffset;
					string marker;
					if (markers != null && markers.TryGetValue (ilOffset, out marker)) {
					} else {
					}

					OpCode opCode;
					int expectedSize;

					byte op1 = ilBytes [curIndex++];
					if (op1 == 0xfe && curIndex < length) {
						byte op2 = ilBytes [curIndex++];
						opCode = s_twoByteOpCodes [op2];
						expectedSize = 2;
					} else {
						opCode = s_oneByteOpCodes [op1];
						expectedSize = 1;
					}

					if (opCode.Size != expectedSize) {
						//sb.AppendLine(string.Format("  <unknown 0x{0}{1:X2}>", expectedSize == 2 ? "fe" : "", op1));
						continue;
					}

					//sb.Append("  ");

					// Console.WriteLine (opCode.OperandType == OperandType.InlineNone ? "{0} {1}" : "{0,-10} {1}", opCode, opCode.OperandType);

					switch (opCode.OperandType) {

					case OperandType.InlineField:

						// read token
						uint fieldToken = ReadUInt32 (ilBytes, ref curIndex);
						// get the kind
						uint tokenKind = fieldToken & TokenTypeIds.TokenTypeMask;
						// and the rowId
						uint rowId = fieldToken & TokenTypeIds.RIDMask;

						var fieldHandle = MetadataTokens.FieldDefinitionHandle ((int)rowId);

						var fieldDef = metaReader.GetFieldDefinition (fieldHandle);

						var fieldName = metaReader.GetString (fieldDef.Name);

						if (opCode.ToString () == "stfld") {

							InspectorField inspectorField;

							if (_inspectorComponent.Fields.TryGetValue (fieldName, out inspectorField)) {
								inspectorField.DefaultValue = String.Join (" ", loadedValues.ToArray ());
							}

						}

						loadedValues.Clear ();

						break;
					case OperandType.InlineMethod:

						// new Vector3, etc
						if (opCode.ToString () == "newobj") {

						} else
							loadedValues.Clear ();

						break;
					case OperandType.InlineTok:
					case OperandType.InlineType:
						ReadUInt32 (ilBytes, ref curIndex);
						loadedValues.Clear ();
						break;

					case OperandType.InlineSig: // signature (calli), not emitted by C#/VB
						ReadUInt32 (ilBytes, ref curIndex);
						loadedValues.Clear ();
						break;

					case OperandType.InlineString:
						//sb.Append(" 391 ");
						//sb.Append(VisualizeUserString());

						uint stringToken = ReadUInt32 (ilBytes, ref curIndex);

						// get the kind
						//uint tokenKind = stringToken & TokenTypeIds.TokenTypeMask;
						// and the rowId
						//uint rowId = stringToken & TokenTypeIds.RIDMask;


						UserStringHandle handle = MetadataTokens.UserStringHandle ((int)stringToken);
						loadedValues.Add (metaReader.GetUserString (handle));

						break;

					case OperandType.InlineNone:

						if (opCode == OpCodes.Ldc_I4_0)
							loadedValues.Add ("0");
						else if (opCode == OpCodes.Ldc_I4_1)
							loadedValues.Add ("1");
						else if (opCode == OpCodes.Ldc_I4_2)
							loadedValues.Add ("2");
						else if (opCode == OpCodes.Ldc_I4_3)
							loadedValues.Add ("3");
						else if (opCode == OpCodes.Ldc_I4_4)
							loadedValues.Add ("4");
						else if (opCode == OpCodes.Ldc_I4_5)
							loadedValues.Add ("5");
						else if (opCode == OpCodes.Ldc_I4_6)
							loadedValues.Add ("6");
						else if (opCode == OpCodes.Ldc_I4_7)
							loadedValues.Add ("7");
						else if (opCode == OpCodes.Ldc_I4_8)
							loadedValues.Add ("8");
						else if (opCode == OpCodes.Ldc_I4_M1)
							loadedValues.Add ("-1");

						break;

					case OperandType.ShortInlineI:
						loadedValues.Add (ReadSByte (ilBytes, ref curIndex).ToString ());
						break;

					case OperandType.ShortInlineVar:
						loadedValues.Add (ReadByte (ilBytes, ref curIndex).ToString ());
						break;

					case OperandType.InlineVar:
						loadedValues.Add (ReadUInt16 (ilBytes, ref curIndex).ToString ());
						break;

					case OperandType.InlineI:
						loadedValues.Add (ReadUInt32 (ilBytes, ref curIndex).ToString ());
						break;

					case OperandType.InlineI8:
						loadedValues.Add (ReadUInt64 (ilBytes, ref curIndex).ToString ());
						break;

					case OperandType.ShortInlineR:
						{
							loadedValues.Add (ReadSingle (ilBytes, ref curIndex).ToString ());
						}
						break;

					case OperandType.InlineR:
						{
							loadedValues.Add (ReadDouble (ilBytes, ref curIndex).ToString ());
						}
						break;

					case OperandType.ShortInlineBrTarget:
						loadedValues.Clear ();
						var sbyteValue = ReadSByte (ilBytes, ref curIndex) + curIndex + blockOffset;
						break;

					case OperandType.InlineBrTarget:
						loadedValues.Clear ();
						var int32value = ReadInt32 (ilBytes, ref curIndex) + curIndex + blockOffset;
						break;

					case OperandType.InlineSwitch:
						loadedValues.Clear ();
						int labelCount = ReadInt32 (ilBytes, ref curIndex);
						int instrEnd = curIndex + labelCount * 4;
						for (int i = 0; i < labelCount; i++) {
							var int32LabelValue = ReadInt32 (ilBytes, ref curIndex) + instrEnd + blockOffset;
							//sb.AppendLine((i == labelCount - 1) ? ")" : ",");
						}
						break;

					default:
						throw new InvalidOperationException ();
					//throw ExceptionUtilities.UnexpectedValue(opCode.OperandType);
					}

					//sb.AppendLine();
				}

				if (EndsSpan (spans, lastSpanIndex, curIndex + blockOffset)) {
					break;
				}
			}

			nextSpanIndex = spanIndex;
			return curIndex;
		}

		TypeDefinition typeDef;
		PEReader peFile;
		MetadataReader metaReader;

		private static readonly OpCode[] s_oneByteOpCodes;
		private static readonly OpCode[] s_twoByteOpCodes;

		static CSComponentInspector ()
		{
			s_oneByteOpCodes = new OpCode[0x100];
			s_twoByteOpCodes = new OpCode[0x100];

			var typeOfOpCode = typeof(OpCode);

			foreach (FieldInfo fi in typeof(OpCodes).GetTypeInfo().DeclaredFields) {
				if (fi.FieldType != typeOfOpCode) {
					continue;
				}

				OpCode opCode = (OpCode)fi.GetValue (null);
				var value = unchecked((ushort)opCode.Value);
				if (value < 0x100) {
					s_oneByteOpCodes [value] = opCode;
				} else if ((value & 0xff00) == 0xfe00) {
					s_twoByteOpCodes [value & 0xff] = opCode;
				}
			}
		}

		private static ulong ReadUInt64 (ImmutableArray<byte> buffer, ref int pos)
		{
			ulong result =
				buffer [pos] |
				(ulong)buffer [pos + 1] << 8 |
				(ulong)buffer [pos + 2] << 16 |
				(ulong)buffer [pos + 3] << 24 |
				(ulong)buffer [pos + 4] << 32 |
				(ulong)buffer [pos + 5] << 40 |
				(ulong)buffer [pos + 6] << 48 |
				(ulong)buffer [pos + 7] << 56;

			pos += sizeof(ulong);
			return result;
		}

		private static uint ReadUInt32 (ImmutableArray<byte> buffer, ref int pos)
		{
			uint result = buffer [pos] | (uint)buffer [pos + 1] << 8 | (uint)buffer [pos + 2] << 16 | (uint)buffer [pos + 3] << 24;
			pos += sizeof(uint);
			return result;
		}

		private static int ReadInt32 (ImmutableArray<byte> buffer, ref int pos)
		{
			return unchecked((int)ReadUInt32 (buffer, ref pos));
		}

		private static ushort ReadUInt16 (ImmutableArray<byte> buffer, ref int pos)
		{
			ushort result = (ushort)(buffer [pos] | buffer [pos + 1] << 8);
			pos += sizeof(ushort);
			return result;
		}

		private static byte ReadByte (ImmutableArray<byte> buffer, ref int pos)
		{
			byte result = buffer [pos];
			pos += sizeof(byte);
			return result;
		}

		private static sbyte ReadSByte (ImmutableArray<byte> buffer, ref int pos)
		{
			sbyte result = unchecked((sbyte)buffer [pos]);
			pos += 1;
			return result;
		}

		private unsafe static float ReadSingle (ImmutableArray<byte> buffer, ref int pos)
		{
			uint value = ReadUInt32 (buffer, ref pos);
			return *(float*)&value;
		}

		private unsafe static double ReadDouble (ImmutableArray<byte> buffer, ref int pos)
		{
			ulong value = ReadUInt64 (buffer, ref pos);
			return *(double*)&value;
		}

		public enum HandlerKind
		{
			Try,
			Catch,
			Filter,
			Finally,
			Fault
		}

		public struct HandlerSpan : IComparable<HandlerSpan>
		{
			public readonly HandlerKind Kind;
			public readonly object ExceptionType;
			public readonly int StartOffset;
			public readonly int FilterHandlerStart;
			public readonly int EndOffset;

			public HandlerSpan (HandlerKind kind, object exceptionType, int startOffset, int endOffset, int filterHandlerStart = 0)
			{
				this.Kind = kind;
				this.ExceptionType = exceptionType;
				this.StartOffset = startOffset;
				this.EndOffset = endOffset;
				this.FilterHandlerStart = filterHandlerStart;
			}

			public int CompareTo (HandlerSpan other)
			{
				int result = this.StartOffset - other.StartOffset;
				if (result == 0) {
					// Both blocks have same start. Order larger (outer) before smaller (inner).
					result = other.EndOffset - this.EndOffset;
				}

				return result;
			}

			public string ToString (CSComponentInspector visualizer)
			{
				switch (this.Kind) {
				default:
					return ".try";
				case HandlerKind.Catch:
					return "catch **exceptiontype**";// + visualizer.VisualizeLocalType(this.ExceptionType);
				case HandlerKind.Filter:
					return "filter";
				case HandlerKind.Finally:
					return "finally";
				case HandlerKind.Fault:
					return "fault";
				}
			}

			public override string ToString ()
			{
				throw new NotSupportedException ("Use ToString(CSComponentInspector)");
			}
		}

		private static bool StartsSpan (IReadOnlyList<HandlerSpan> spans, int spanIndex, int curIndex)
		{
			return spans != null && spanIndex < spans.Count && spans [spanIndex].StartOffset == (uint)curIndex;
		}

		private static bool EndsSpan (IReadOnlyList<HandlerSpan> spans, int spanIndex, int curIndex)
		{
			return spans != null && spanIndex >= 0 && spans [spanIndex].EndOffset == (uint)curIndex;
		}

		private static bool StartsFilterHandler (IReadOnlyList<HandlerSpan> spans, int spanIndex, int curIndex)
		{
			return spans != null &&
			spanIndex < spans.Count &&
			spans [spanIndex].Kind == HandlerKind.Filter &&
			spans [spanIndex].FilterHandlerStart == (uint)curIndex;
		}

		public static IReadOnlyList<HandlerSpan> GetHandlerSpans (ImmutableArray<ExceptionRegion> entries)
		{
			if (entries.Length == 0) {
				return new HandlerSpan[0];
			}

			var result = new List<HandlerSpan> ();
			foreach (ExceptionRegion entry in entries) {
				int tryStartOffset = entry.TryOffset;
				int tryEndOffset = entry.TryOffset + entry.TryLength;
				var span = new HandlerSpan (HandlerKind.Try, null, tryStartOffset, tryEndOffset);

				if (result.Count == 0 || span.CompareTo (result [result.Count - 1]) != 0) {
					result.Add (span);
				}
			}

			foreach (ExceptionRegion entry in entries) {
				int handlerStartOffset = entry.HandlerOffset;
				int handlerEndOffset = entry.HandlerOffset + entry.HandlerLength;

				HandlerSpan span;
				switch (entry.Kind) {
				case ExceptionRegionKind.Catch:
					span = new HandlerSpan (HandlerKind.Catch, MetadataTokens.GetToken (entry.CatchType), handlerStartOffset, handlerEndOffset);
					break;

				case ExceptionRegionKind.Fault:
					span = new HandlerSpan (HandlerKind.Fault, null, handlerStartOffset, handlerEndOffset);
					break;

				case ExceptionRegionKind.Filter:
					span = new HandlerSpan (HandlerKind.Filter, null, handlerStartOffset, handlerEndOffset, entry.FilterOffset);
					break;

				case ExceptionRegionKind.Finally:
					span = new HandlerSpan (HandlerKind.Finally, null, handlerStartOffset, handlerEndOffset);
					break;

				default:
					throw new InvalidOperationException ();
				}

				result.Add (span);
			}

			return result;
		}

		public void Dump ()
		{
			/*
				foreach (var entry in InspectorFields) {
					var field = entry.Value;

					Console.WriteLine ("Inspector Field: {0}", field.Name);

					Console.WriteLine ("   Type Name: {0}", field.TypeName);
					Console.WriteLine ("   Default Value: {0}", field.DefaultValue);

					Console.WriteLine ("   Positional Custom Attr:");
					foreach (var p in field.CustomAttrPositionalArgs)
					if (p.Length != 0)
					Console.WriteLine ("      {0}", p);
					Console.WriteLine ("   Named Custom Attr:");
					foreach (var nentry in field.CustomAttrNamedArgs)
					Console.WriteLine ("      {0}:{1}", nentry.Key, nentry.Value);
				}
				*/
		}

	}

	internal static class TokenTypeIds
	{
		internal const uint Module = 0x00000000;
		internal const uint TypeRef = 0x01000000;
		internal const uint TypeDef = 0x02000000;
		internal const uint FieldDef = 0x04000000;
		internal const uint MethodDef = 0x06000000;
		internal const uint ParamDef = 0x08000000;
		internal const uint InterfaceImpl = 0x09000000;
		internal const uint MemberRef = 0x0a000000;
		internal const uint CustomAttribute = 0x0c000000;
		internal const uint Permission = 0x0e000000;
		internal const uint Signature = 0x11000000;
		internal const uint Event = 0x14000000;
		internal const uint Property = 0x17000000;
		internal const uint ModuleRef = 0x1a000000;
		internal const uint TypeSpec = 0x1b000000;
		internal const uint Assembly = 0x20000000;
		internal const uint AssemblyRef = 0x23000000;
		internal const uint File = 0x26000000;
		internal const uint ExportedType = 0x27000000;
		internal const uint ManifestResource = 0x28000000;
		internal const uint GenericParam = 0x2a000000;
		internal const uint MethodSpec = 0x2b000000;
		internal const uint GenericParamConstraint = 0x2c000000;
		internal const uint String = 0x70000000;
		internal const uint Name = 0x71000000;
		internal const uint BaseType = 0x72000000;
		// Leave this on the high end value. This does not correspond to metadata table???

		internal const uint RIDMask = 0x00FFFFFF;
		internal const uint TokenTypeMask = 0xFF000000;
	}


}
