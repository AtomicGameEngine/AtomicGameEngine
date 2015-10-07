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

namespace AtomicEditor
{


	public class CSComponentInspector
	{
		private List<string> inspectorFieldNames = new List<string> ();

		public CSComponentInspector (TypeDefinition typeDef, PEReader peFile, MetadataReader metaReader)
		{
			this.typeDef = typeDef;
			this.peFile = peFile;
			this.metaReader = metaReader;
		}

		public bool Inspect ()
		{

			var fields = typeDef.GetFields ();

			foreach (var fieldHandle in fields) {

				var fieldDef = metaReader.GetFieldDefinition (fieldHandle);

				var customAttr = fieldDef.GetCustomAttributes ();

				foreach (var caHandle in customAttr) {

					var ca = metaReader.GetCustomAttribute (caHandle);

					// MethodDefinitionHandle or MemberReferenceHandle
					if (ca.Constructor.Kind == HandleKind.MemberReference) {

						var memberRef = metaReader.GetMemberReference ((MemberReferenceHandle)ca.Constructor);

						var parent = memberRef.Parent;

						if (parent.Kind == HandleKind.TypeReference) {

							var parentTypeRef = metaReader.GetTypeReference ((TypeReferenceHandle)parent);

							var attrName = metaReader.GetString (parentTypeRef.Name);

							if (attrName == "InspectorAttribute") {

								var fieldName = metaReader.GetString (fieldDef.Name);
								inspectorFieldNames.Add (fieldName);
								Console.WriteLine ("Found inspector field: {0}", fieldName);

								var blobReader =  metaReader.GetBlobReader( (BlobHandle) ca.Value);

								Console.WriteLine("Blob: {0}", blobReader.ReadByte());
								Console.WriteLine("Blob: {0}", blobReader.ReadByte());
								Console.WriteLine("Blob: {0}", blobReader.ReadByte());
								Console.WriteLine("Blob: {0}", blobReader.ReadByte());

								Console.WriteLine("Blob: {0}", blobReader.ReadByte());
								Console.WriteLine("Blob: {0}", blobReader.ReadByte());
								Console.WriteLine("Blob: {0}", blobReader.ReadByte());
								Console.WriteLine("Blob: {0}", blobReader.ReadByte());

								//Console.WriteLine("Blob: {0}", blobReader.ReadSerializedString());
								//Console.WriteLine("Blob: {0}", blobReader.ReadUInt16());
								//Console.WriteLine("Blob: {0}", blobReader.ReadSerializedString());

							}

							//Console.WriteLine("CustomAttr MemberReference {0} {1}", , metaReader.GetString(memberRef.Name));

						}
					}
				}
			}

			var methods = typeDef.GetMethods ();

			foreach (var methodHandle in methods) {
				var methodDef = metaReader.GetMethodDefinition (methodHandle);

				if (metaReader.GetString (methodDef.Name) == ".ctor") {
					var body = peFile.GetMethodBody (methodDef.RelativeVirtualAddress);
				}

			}

			return true;

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
					//sb.AppendFormat(opCode.OperandType == OperandType.InlineNone ? "{0}" : "{0,-10}", opCode);

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

						break;
					case OperandType.InlineMethod:
					case OperandType.InlineTok:
					case OperandType.InlineType:
						ReadUInt32 (ilBytes, ref curIndex);
						break;

					case OperandType.InlineSig: // signature (calli), not emitted by C#/VB
						ReadUInt32 (ilBytes, ref curIndex);
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
						string stringValue = metaReader.GetUserString (handle);

                          //sb.AppendFormat("\"{0}\"", );


						break;

					case OperandType.InlineNone:
                          // ldc.i4.1 // load 1 for instance
                          //sb.AppendFormat(" InlineNone");
						break;

					case OperandType.ShortInlineI:
						ReadSByte (ilBytes, ref curIndex);
						break;

					case OperandType.ShortInlineVar:
						ReadByte (ilBytes, ref curIndex);
						break;

					case OperandType.InlineVar:
						ReadUInt16 (ilBytes, ref curIndex);
						break;

					case OperandType.InlineI:
						ReadUInt32 (ilBytes, ref curIndex);
						break;

					case OperandType.InlineI8:
						ReadUInt64 (ilBytes, ref curIndex);
						break;

					case OperandType.ShortInlineR:
						{
							var value = ReadSingle (ilBytes, ref curIndex);
							if (value == 0 && 1 / value < 0) {
								//sb.Append(" 423 -0.0");
							} else {
								//sb.AppendFormat("477 {0}", value.ToString(CultureInfo.InvariantCulture));
							}
						}
						break;

					case OperandType.InlineR:
						{
							var value = ReadDouble (ilBytes, ref curIndex);
							if (value == 0 && 1 / value < 0) {
								//sb.Append("437 -0.0");
							} else {
								//sb.AppendFormat("441 {0}", value.ToString(CultureInfo.InvariantCulture));
							}
						}
						break;

					case OperandType.ShortInlineBrTarget:
						var sbyteValue = ReadSByte (ilBytes, ref curIndex) + curIndex + blockOffset;
						break;

					case OperandType.InlineBrTarget:
						var int32value = ReadInt32 (ilBytes, ref curIndex) + curIndex + blockOffset;
						break;

					case OperandType.InlineSwitch:
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
